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
    
    while (xlsxR.cellAt(row, 1) && xlsxR.cellAt(row, 2))
    {
        if (!xlsxR.cellAt(row, 1)->readValue().toString().isEmpty() && !xlsxR.cellAt(row, 2)->readValue().toString().isEmpty())
        {
            CourseEvalItems Row;
            Row.Catalog = xlsxR.cellAt(row, 3)->readValue().toString();
            Row.CourseTitle = xlsxR.cellAt(row, 5)->readValue().toString();
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
    qDebug() << db->databaseName();
    if (!db->open())
    {
        qDebug() << "Could not open the database";
    }
    QSqlQuery query(*db);
    bool res = true; 
    for (int i = 0; i < size(); i++)
    {
        QString qrystatement = "INSERT INTO CourseEvals ( subject, catalog, section, instructor, year_semester, evaluated, enrolled, score ) "
            "VALUES ('" + at(i).Subject + "','" + at(i).Catalog + "','" + at(i).Section + "','" + at(i).Instructor + "','" + at(i).Year_Semester + "'," + QString::number(at(i).Evaluated) + "," + QString::number(at(i).Enrolled) + "," + QString::number(at(i).score) + ")";
        res &= query.prepare(qrystatement);
        if (res) query.exec(qrystatement);
        if (!res)
        {
            qDebug() << query.lastError(); 
        }
    }
    db->close();
    return res; 
}
