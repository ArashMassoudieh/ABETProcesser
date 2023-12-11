#include "studentcoursedata.h"
#include "xlsxdocument.h"
#include "xlsxchartsheet.h"
#include "xlsxcellrange.h"
#include "xlsxchart.h"
#include "xlsxrichstring.h"
#include "xlsxworkbook.h"
#include <QDebug>


using namespace QXlsx;

StudentCourseData::StudentCourseData():QList<StudentCourseInfo>()
{

}

StudentCourseData::StudentCourseData(const StudentCourseData &stcoursedata):QList<StudentCourseInfo>(stcoursedata)
{

}
StudentCourseData& StudentCourseData::operator=(const StudentCourseData &stcoursedata)
{
    QList<StudentCourseInfo>::operator=(stcoursedata);
    return *this;
}
bool StudentCourseData::ReadFromExcel(const QString &filename)
{
    Document xlsxR(filename);
    QStringList sheetnames;
    if (xlsxR.load()) // load excel file
    {
        qDebug() << "[debug] success to load xlsx file.";
        sheetnames = xlsxR.sheetNames();
    }
    else
    {
        return false;
    }
    if (sheetnames.size()==0)
        return false;
    xlsxR.selectSheet(sheetnames[0]);
    int row = 2;
    while (xlsxR.cellAt(row,1)) {
        if (!xlsxR.cellAt(row, 1)->readValue().toString().isEmpty())
        {
            StudentCourseInfo item;
            item.StudentID = xlsxR.cellAt(row, 1)->readValue().toString().trimmed();
            item.StudentName = xlsxR.cellAt(row, 2)->readValue().toString().trimmed();
            item.StrtLevel = xlsxR.cellAt(row, 3)->readValue().toString().trimmed();
            item.AcadProg = xlsxR.cellAt(row, 4)->readValue().toString().trimmed();
            item.AcadPlan = xlsxR.cellAt(row, 5)->readValue().toString().trimmed();
            item.Subject = xlsxR.cellAt(row, 6)->readValue().toString().trimmed();
            item.Catalog = xlsxR.cellAt(row, 7)->readValue().toString().trimmed();
            item.Section = xlsxR.cellAt(row, 8)->readValue().toString().trimmed();
            item.Title = xlsxR.cellAt(row, 9)->readValue().toString().trimmed();
            item.unit = xlsxR.cellAt(row, 10)->readValue().toInt();
            if (xlsxR.cellAt(row, 11))
                item.RepeatCode = xlsxR.cellAt(row, 11)->readValue().toString().trimmed();
            if (xlsxR.cellAt(row, 12))
                item.RepeatCodeDescr = xlsxR.cellAt(row, 12)->readValue().toString().trimmed();
            if (xlsxR.cellAt(row, 13))
                item.GradeIn = xlsxR.cellAt(row, 13)->readValue().toString().trimmed();
            if (xlsxR.cellAt(row, 14))
                item.OfficialGrade = xlsxR.cellAt(row, 14)->readValue().toString().trimmed();
            if (xlsxR.cellAt(row, 15))
                item.GradeIn = xlsxR.cellAt(row, 15)->readValue().toString().trimmed();
            if (xlsxR.cellAt(row, 16))
                item.GradingBasis = xlsxR.cellAt(row, 16)->readValue().toString().trimmed();
            if (xlsxR.cellAt(row, 17))
                item.CumGPA = xlsxR.cellAt(row, 17)->readValue().toDouble();
            if (xlsxR.cellAt(row, 18))
                item.CurrentGPA = xlsxR.cellAt(row, 17)->readValue().toDouble();

            append(item);

        }
        row++;
    }

    return true;
}

StudentCourseData StudentCourseData::Filter(const StudentCourseInfo &info)
{
    StudentCourseData out;
    for (int i=0; i<count(); i++)
    {
        if (at(i)==info)
            out.append(at(i));
    }
    return out;

}

QMap<QString,StudentCourseData> StudentCourseData::SplitBySections()
{
    QMap<QString,StudentCourseData> out;
    for (int i=0; i<count(); i++)
    {
        if (out.contains(at(i).Section))
        {
            out[at(i).Section].append(at(i));
        }
        else
        {
            out[at(i).Section] = StudentCourseData();
            out[at(i).Section].append(at(i));
        }
    }
    return out;
}
