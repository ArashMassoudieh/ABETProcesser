#include "pidata.h"
#include "xlsxdocument.h"
#include "xlsxchartsheet.h"
#include "xlsxcellrange.h"
#include "xlsxchart.h"
#include "xlsxrichstring.h"
#include "xlsxworkbook.h"
#include <QDebug>

using namespace QXlsx;

PIData::PIData()
{

}

PIData::PIData(const PIData &RHS)
{
    CourseList = RHS.CourseList;
    PIList = RHS.PIList;

}
PIData& PIData::operator==(const PIData &RHS)
{

    CourseList = RHS.CourseList;
    PIList = RHS.PIList;
    return *this;
}

bool PIData::ReadFromExcel(QString filename)
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
    qDebug()<<xlsxR.cellAt(1,1)->readValue().toString();

    int col = 3;
    while (xlsxR.cellAt(1,col)) {
        if (!xlsxR.cellAt(1, col)->readValue().toString().isEmpty())
        {
            CourseList.append(xlsxR.cellAt(1, col)->readValue().toString());
            qDebug()<<xlsxR.cellAt(1, col)->readValue().toString();
        }
        col++;
    }
    int row = 2;
    while (xlsxR.cellAt(row, 1) || xlsxR.cellAt(row+1, 1))
    {
        if (xlsxR.cellAt(row,1))
            if (!xlsxR.cellAt(row, 1)->readValue().toString().isEmpty())
            {
                qDebug() << row << "  " << xlsxR.cellAt(row, 1)->readValue().toString();
                PI item;
                item.ID = xlsxR.cellAt(row, 1)->readValue().toString();
                item.Description = xlsxR.cellAt(row, 2)->readValue().toString();
                for (int col = 3; col<CourseList.count()+3; col++)
                {
                    if (xlsxR.cellAt(row,col))
                    {   qDebug()<<xlsxR.cellAt(row, col)->readValue().toString();
                        if (!xlsxR.cellAt(row, col)->readValue().toString().isEmpty())
                        {
                            if (xlsxR.cellAt(row, col)->readValue().toString().contains("*"))
                                item.CoursesApplied.append(xlsxR.cellAt(1, col)->readValue().toString());
                        }
                    }
                }
                PIList[item.ID] = item;
            }
        row++;
    }
    return true;
}

QStringList PIData::GetPIsForCourse(const QString &coursename)
{
    QStringList out;
    for (QMap<QString,PI>::iterator it = PIList.begin(); it!=PIList.end(); it++)
    {
        if (it->CoursesApplied.contains(coursename))
            out.append(it.key());
    }
    return out;
}
