#include "pidata.h"
#include "xlsxdocument.h"
#include "xlsxchartsheet.h"
#include "xlsxcellrange.h"
#include "xlsxchart.h"
#include "xlsxrichstring.h"
#include "xlsxworkbook.h"
#include <QDebug>

using namespace QXlsx;

PIData::PIData():QMap<QString, PI>()
{

}

PIData::PIData(const PIData &RHS):QMap<QString, PI>(RHS)
{
    CourseList = RHS.CourseList;
}
PIData& PIData::operator==(const PIData &RHS)
{

    CourseList = RHS.CourseList;
    QMap<QString,PI>::operator=(RHS);
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
                operator[](item.ID) = item;
            }
        row++;
    }
    return true;
}

QStringList PIData::GetPIsForCourse(const QString &coursename)
{
    QStringList out;
    for (QMap<QString,PI>::iterator it = begin(); it!=end(); it++)
    {
        if (it->CoursesApplied.contains(coursename))
            out.append(it.key());
    }
    return out;
}

bool PIData::CreateExcelFiles(StudentCourseData *studentData,const QDir &OutputLocation)
{
    for (int i=0; i<CourseList.count(); i++)
    {
        CreateExcelFile(studentData,CourseList[i],OutputLocation.absolutePath() + "/" + CourseList[i] );
    }
    return true;
}

bool PIData::CreateExcelFile(StudentCourseData *studentData,const QString &course, const QString &OutputFile)
{
    StudentCourseInfo info;
    if (course.split(" ").count()!=2)
        return false;
    info.Subject = course.split(" ")[0];
    info.Catalog = course.split(" ")[1];
    StudentCourseData extracted_data = studentData->Filter(info);
    QMap<QString,StudentCourseData> extracted_splitted_data = extracted_data.SplitBySections();
    for (QMap<QString, StudentCourseData>::iterator it = extracted_splitted_data.begin() ; it!= extracted_splitted_data.end(); it++)
    {
        CreateExcelFile(&extracted_splitted_data[it.key()],course,OutputFile+".xlsx",it.key());
    }
    return true;

}

bool PIData::CreateExcelFile(StudentCourseData *studentData,const QString &course, const QString &OutputFile, const QString &section)
{
    QXlsx::Document xlsxW;
    int start_row = 5;
    QStringList PIs = GetPIsForCourse(course);
    if (PIs.count()>0)
    {   for (int i=0; i<PIs.count(); i++)
        {

            xlsxW.addSheet(operator[](PIs[i]).ID);
            xlsxW.selectSheet(operator[](PIs[i]).ID);
            xlsxW.write(2,2,operator[](PIs[i]).ID);
            xlsxW.write(2,3,operator[](PIs[i]).Description);
            xlsxW.write(3,2,"Please enter the scores as follows: 1: Unsartisfactory, 2: Developing, 3: Satisfactory, and 4: Exemplary");
            QXlsx::Format bold;
            bold.setFontBold(true);

            xlsxW.write(start_row, 1, "Name",bold);
            xlsxW.setColumnWidth(1,30);
            xlsxW.write(start_row, 2, "Student_ID",bold);
            xlsxW.setColumnWidth(2,30);
            xlsxW.write(start_row, 3, "Program",bold);
            xlsxW.setColumnWidth(3,30);
            xlsxW.write(start_row, 4, "Score",bold);
            xlsxW.setColumnWidth(4,30);
            for (int j=0; j<studentData->count(); j++)
            {
                xlsxW.write(start_row+j+1,1, studentData->at(j).StudentName);
                xlsxW.write(start_row+j+1,2, studentData->at(j).StudentID);
                xlsxW.write(start_row+j+1,3, studentData->at(j).AcadPlan);
                xlsxW.write(start_row+j+1,4, "");
            }
            xlsxW.deleteSheet("Sheet1");
        }

        xlsxW.saveAs(OutputFile.split(".")[0] + "_" + section + "." + OutputFile.split(".")[1]);
    }
    return true;
}
