#include "CourseEvaluationData.h"
#include "xlsxdocument.h"
#include "xlsxchartsheet.h"
#include "xlsxcellrange.h"
#include "xlsxchart.h"
#include "xlsxrichstring.h"
#include "xlsxworkbook.h"
#include <qdebug.h>
#include <qdir.h>
#include <qdiriterator.h>
#include <qsqlerror.h>

using namespace QXlsx;

CourseEvaluationData::CourseEvaluationData(): QVector<CourseEvalItems>()
{

}
CourseEvaluationData::CourseEvaluationData(const CourseEvaluationData& RHS) : QVector<CourseEvalItems>(RHS)
{

}
CourseEvaluationData& CourseEvaluationData::operator==(const CourseEvaluationData& RHS)
{
	QVector<CourseEvalItems>::operator=(RHS);
    return *this; 
}

bool CourseEvaluationData::ReadFromDirectory(QString Folder)
{
    QDir dir(Folder);
    dir.setFilter(QDir::Dirs);
    qDebug() << dir.absolutePath();
    QStringList nameFilter("*.xlsx");
    QStringList xlsFilesAndDirectories = dir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot);
    qDebug() << xlsFilesAndDirectories;

    for (int i = 0; i < xlsFilesAndDirectories.count(); i++)
    {
        QString fullfilename = dir.absolutePath() + "/" + xlsFilesAndDirectories[i];
        qDebug() << fullfilename;
        ReadFromExcel(fullfilename);

    }
    return true; 
}

bool CourseEvaluationData::ReadFromExcel(QString filename)
{
    Document xlsxR(filename);
    QStringList sheetnames;
    if (xlsxR.load()) // load excel file
    {
        qDebug() << "[debug] success to load xlsx file.";
        sheetnames = xlsxR.sheetNames();
    }
    else
        return false; 

    bool x = xlsxR.selectSheet(sheetnames[0]);
    int row = 8;
    //qDebug() << xlsxR.cellAt(8, 1); 
    //qDebug() << xlsxR.cellAt(8, 2);
    //qDebug() << xlsxR.cellAt(12, 3);
    while (xlsxR.cellAt(row, 1) && xlsxR.cellAt(row, 2))
    {
        if (!xlsxR.cellAt(row, 1)->readValue().toString().isEmpty() && !xlsxR.cellAt(row, 2)->readValue().toString().isEmpty())
        {
            CourseEvalItems Row;
            Row.Catalog = xlsxR.cellAt(row, 3)->readValue().toString();
            Row.CourseTitle = xlsxR.cellAt(row, 5)->readValue().toString();
            Row.CourseNumber = xlsxR.cellAt(row, 2)->readValue().toString() + " " + xlsxR.cellAt(row, 3)->readValue().toString();
            Row.Enrolled = xlsxR.cellAt(row, 10)->readValue().toInt();
            Row.Evaluated = xlsxR.cellAt(row, 11)->readValue().toInt();
            Row.Instructor = xlsxR.cellAt(row, 9)->readValue().toString().remove(",").remove('/');
            Row.score = xlsxR.cellAt(row, 31)->readValue().toDouble();
            Row.Section = xlsxR.cellAt(row, 4)->readValue().toString();
            Row.Subject = xlsxR.cellAt(row, 2)->readValue().toString();
            Row.Year_Semester = xlsxR.cellAt(5, 1)->readValue().toString();
            append(Row);
        }
        row++;
    }
    return true; 
}

bool CourseEvaluationData::AppendtoSqlit(QSqlDatabase* db)
{
    if (!db || !db->isOpen()) {
        qDebug() << "Database not open!";
        return false;
    }

    QSqlQuery query(*db);
    db->transaction();

    // 1. Drop and recreate the table
    if (!query.exec("DROP TABLE IF EXISTS CourseEvals")) {
        qDebug() << "Failed to drop table:" << query.lastError().text();
        db->rollback();
        return false;
    }

    if (!query.exec(R"(
        CREATE TABLE CourseEvals (
            subject TEXT,
            catalog TEXT,
            section TEXT,
            coursenumber TEXT,
            instructor TEXT,
            year_semester TEXT,
            evaluated INTEGER,
            enrolled INTEGER,
            score REAL
        )
    )")) {
        qDebug() << "Failed to create table:" << query.lastError().text();
        db->rollback();
        return false;
    }

    // 2. Prepare insert statement
    query.prepare(R"(
        INSERT INTO CourseEvals (
            subject, catalog, section, coursenumber, instructor,
            year_semester, evaluated, enrolled, score
        ) VALUES (
            :subject, :catalog, :section, :coursenumber, :instructor,
            :year_semester, :evaluated, :enrolled, :score
        )
    )");

    // 3. Insert records
    bool res = true;
    for (int i = 0; i < size(); ++i) {
        const auto& item = at(i);
        query.bindValue(":subject", item.Subject);
        query.bindValue(":catalog", item.Catalog);
        query.bindValue(":section", item.Section);
        query.bindValue(":coursenumber", item.CourseNumber);
        query.bindValue(":instructor", item.Instructor);
        query.bindValue(":year_semester", item.Year_Semester);
        query.bindValue(":evaluated", item.Evaluated);
        query.bindValue(":enrolled", item.Enrolled);
        query.bindValue(":score", item.score);

        if (!query.exec()) {
            qDebug() << "Failed to insert row:" << query.lastError().text();
            res = false;
            db->rollback();
            return false;
        }
    }

    db->commit();
    return res;
}