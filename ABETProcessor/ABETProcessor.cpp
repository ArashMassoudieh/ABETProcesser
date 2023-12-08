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
                            
                            data.append(Row);
                        }
                        row++;
                    }
                }
            }
            
        }
       
    }
    WriteToCSV("G:/Shared drives/ENGR-CEE-COMMON/ABET/Collected_Course_Material/22-23-All/AllData.csv", data);
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
