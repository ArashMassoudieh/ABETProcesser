#include "ABETProcessor.h"
#include <qdir>
#include <qdiriterator.h>
#include <qdebug.h>
#include "xlsxdocument.h"
#include "xlsxchartsheet.h"
#include "xlsxcellrange.h"
#include "xlsxchart.h"
#include "xlsxrichstring.h"
#include "xlsxworkbook.h"

using namespace QXlsx;

ABETProcessor::ABETProcessor(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    QDir dir("G:/Shared drives/ENGR-CEE-COMMON/ABET/Collected_Course_Material/September (Fall) 2022 - May (spring) 2023 ABET Collected Course Materials");
    
    
    dir.setFilter(QDir::Dirs);
    QDirIterator directories(dir.absolutePath(), QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    QStringList all_dirs;
    QVector<data_item> data;
    while (directories.hasNext()) {
        directories.next();
        all_dirs << directories.filePath();
        qDebug()<< directories.filePath();
        QDir courseDir(directories.filePath());
        QStringList nameFilter("*.xlsx");
        QStringList xlsFilesAndDirectories = courseDir.entryList(nameFilter);
        qDebug() << xlsFilesAndDirectories;
        
        for (int i = 0; i < xlsFilesAndDirectories.count(); i++)
        {
            QString fullfilename = courseDir.absolutePath() + "/" + xlsFilesAndDirectories[i];
            qDebug() << fullfilename;
            Document xlsxR(fullfilename);
            xlsxR.saveAs("G:/Shared drives/ENGR-CEE-COMMON/ABET/Collected_Course_Material/22-23-All/" + xlsFilesAndDirectories[i]);
        
            QStringList sheetnames;
            if (xlsxR.load()) // load excel file
            {
                qDebug() << "[debug] success to load xlsx file.";
                sheetnames = xlsxR.sheetNames();
            }

            for (int j = 0; j < sheetnames.count(); j++)
            {
                qDebug() << sheetnames[j];
                if (sheetnames[j].contains("."))
                {
                    xlsxR.selectSheet(sheetnames[j]);
                    int row = 19; 
                    while (xlsxR.cellAt(row, 1) && xlsxR.cellAt(row, 2))
                    {
                        if (!xlsxR.cellAt(row, 1)->readValue().toString().isEmpty() && !xlsxR.cellAt(row, 2)->readValue().toString().isEmpty())
                        {
                            data_item Row;
                            qDebug() << row;
                            Row.lastname = xlsxR.cellAt(row, 1)->readValue().toString().remove(",");
                            Row.firstname = xlsxR.cellAt(row, 2)->readValue().toString().remove(",");
                            Row.Score = xlsxR.cellAt(row, 3)->readValue().toString().remove(",");
                            Row.PerformanceIndicator = sheetnames[j];
                            Row.CourseName = sheetnames[0];
                            Row.Semester = xlsxR.cellAt(6, 7)->readValue().toString();
                            if (!Row.Score.trimmed().isEmpty())
                                data.append(Row);
                        }
                        row++;
                    }
                }
            }
            
        }
       
    }
    WriteToCSV("G:/Shared drives/ENGR-CEE-COMMON/ABET/Collected_Course_Material/22-23-All/AllData.csv", data);
    QVector<course_pi_aggregate_item> aggregate_data = ExtractAggregatePI(data);
    WritePISummaryToCSV("G:/Shared drives/ENGR-CEE-COMMON/ABET/Collected_Course_Material/22-23-All/Aggregate_data.csv", aggregate_data);
}

bool ABETProcessor::WriteToCSV(const QString& fileaName, const QVector<data_item> &data)
{
    QFile file(fileaName);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out << "firstname,lastname,CourseName,PerformanceIndicator,Semester,Score,ScoreValue\n";

    for (int i = 0; i < data.size(); i++)
        out << data[i].firstname << "," << data[i].lastname << "," << data[i].CourseName << "," << data[i].PerformanceIndicator << "," << data[i].Semester << "," << data[i].Score << "," << scoretonumber(data[i].Score)<<"\n";
    file.close();

    return true;
}


bool ABETProcessor::WritePISummaryToCSV(const QString& fileName, const QVector<course_pi_aggregate_item>& coursepiaggdata)
{
    QFile file(fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out << "SO,CourseName,PerformanceIndicator,Percent Satisfactory\n";

    for (int i = 0; i < coursepiaggdata.size(); i++)
        out << "SO " + coursepiaggdata[i].SO << "," << coursepiaggdata[i].CourseName << "," << coursepiaggdata[i].PerformanceIndicator << "," << coursepiaggdata[i].PercentSatisfactory * 100 << "\n";
    file.close();

    return true;
}

int ABETProcessor::scoretonumber(const QString& score)
{
    if (score == "Unsatisfactory")
        return 1;
    else if (score == "Developing")
        return 2;
    else if (score == "Satisfactory")
        return 3;
    else if (score == "Exemplary")
        return 4; 
    return -999; 
}

QStringList ABETProcessor::AllCourseNames(QVector<data_item> &data)
{
    QStringList out; 
    for (int i = 0; i < data.size(); i++)
    {
        if (!out.contains(data[i].CourseName))
            out.append(data[i].CourseName);
    }
    return out; 
}

QStringList ABETProcessor::PIsforCourse(QString &CourseName, QVector<data_item>& data)
{
    QStringList out;
    for (int i = 0; i < data.size(); i++)
    {
        if (data[i].CourseName == CourseName)
            if (!out.contains(data[i].PerformanceIndicator))
                out.append(data[i].PerformanceIndicator);
    }
    return out;
}



QVector<course_pi_aggregate_item>  ABETProcessor::ExtractAggregatePI(QVector<data_item>& data)
{
    QVector<course_pi_aggregate_item> out; 
    QStringList CourseNames = AllCourseNames(data);
    for (int course_counter = 0; course_counter < CourseNames.size(); course_counter++)
    {
        QStringList PIs = PIsforCourse(CourseNames[course_counter],data);
        for (int picounter = 0; picounter < PIs.count(); picounter++)
        {
            double counttotal = 0;
            double countsatisfied = 0;
            for (int data_counter = 0; data_counter < data.size(); data_counter++)
            {
                if (data[data_counter].CourseName == CourseNames[course_counter] && data[data_counter].PerformanceIndicator == PIs[picounter])
                {
                    counttotal++;
                    if (scoretonumber(data[data_counter].Score) > 2)
                        countsatisfied++;
                }
            }
            course_pi_aggregate_item PI_item;
            PI_item.CourseName = CourseNames[course_counter];
            PI_item.PerformanceIndicator = PIs[picounter];
            PI_item.SO = PIs[picounter].split(".")[0];
            PI_item.PercentSatisfactory = countsatisfied / counttotal;
            out.append(PI_item);

        }

    }
    return out;
}
