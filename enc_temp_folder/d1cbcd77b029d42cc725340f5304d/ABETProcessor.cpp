#include "ABETProcessor.h"
#include <qdir.h>
#include <qdiriterator.h>
#include <qdebug.h>
#include "xlsxdocument.h"
#include "xlsxchartsheet.h"
#include "xlsxcellrange.h"
#include "xlsxchart.h"
#include "xlsxrichstring.h"
#include "xlsxworkbook.h"
#include "formcreatespreadsheets.h"
#include "formCourseEvaluations.h"
#include <QFileDialog>

using namespace QXlsx;



ABETProcessor::ABETProcessor(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    connect(ui.actionCreate_PI_Spreadsheets,SIGNAL(triggered()),this,SLOT(OnCreatePITables()));
    connect(ui.actionProcess_PI_SpreadSheets,SIGNAL(triggered()),this,SLOT(OnProcessPIFiles()));
    connect(ui.actionProcess_Course_Evaluations, SIGNAL(triggered()), this, SLOT(OnCourseEvaluations()));

}

void ABETProcessor::OnProcessPIFiles()
{
	QString folderPath = selectFolder(this);
    QDir dir(folderPath);
    
    QVector<data_item> data;
    createSubfolder(folderPath, "saved");
    
    QStringList nameFilter("*.xlsx");
    QStringList xlsFilesAndDirectories = dir.entryList(nameFilter);
    qDebug() << xlsFilesAndDirectories;

    for (int i = 0; i < xlsFilesAndDirectories.count(); i++)
    {
        QString fullfilename = dir.absolutePath() + "/" + xlsFilesAndDirectories[i];
        qDebug() << fullfilename;
        Document xlsxR(fullfilename);
        qDebug() << folderPath + "/saved/" + xlsFilesAndDirectories[i];
        xlsxR.saveAs(folderPath + "/saved/" + xlsFilesAndDirectories[i]);

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
                int row = 6;
                while (xlsxR.cellAt(row, 1) && xlsxR.cellAt(row, 2))
                {
                    if (!xlsxR.cellAt(row, 1)->readValue().toString().isEmpty() && !xlsxR.cellAt(row, 2)->readValue().toString().isEmpty())
                    {
                        if ((xlsxR.cellAt(row, 3)->readValue().toString() == "BSARCH/BCE" || xlsxR.cellAt(row, 3)->readValue().toString() == "CE-BCE") && Program == program::ce)
                        {
                            data_item Row;
                            qDebug() << row;
                            if (xlsxR.cellAt(row, 4))
                            {
                                Row.lastname = xlsxR.cellAt(row, 1)->readValue().toString().split(",")[0];
                                Row.firstname = xlsxR.cellAt(row, 1)->readValue().toString().split(",")[1];
                                Row.studentID = xlsxR.cellAt(row, 2)->readValue().toString();
                                Row.program = xlsxR.cellAt(row, 3)->readValue().toString();
                                Row.Score = xlsxR.cellAt(row, 4)->readValue().toString().remove(",");
                                Row.PerformanceIndicator = sheetnames[j];
                                Row.CourseName = xlsFilesAndDirectories[i].split(".")[0].split("_")[0];
                                //Row.Semester = xlsxR.cellAt(6, 7)->readValue().toString();
                                if (!Row.Score.trimmed().isEmpty())
                                    data.append(Row);
                            }
                        }
                        else if (xlsxR.cellAt(row, 3)->readValue().toString() == "ENVEN-BS" && Program == program::environmental)
                        {
                            data_item Row;
                            qDebug() << row;
                            if (xlsxR.cellAt(row, 4))
                            {
                                Row.lastname = xlsxR.cellAt(row, 1)->readValue().toString().split(",")[0];
                                Row.firstname = xlsxR.cellAt(row, 1)->readValue().toString().split(",")[1];
                                Row.studentID = xlsxR.cellAt(row, 2)->readValue().toString();
                                Row.program = xlsxR.cellAt(row, 3)->readValue().toString();
                                Row.Score = xlsxR.cellAt(row, 4)->readValue().toString().remove(",");
                                Row.PerformanceIndicator = sheetnames[j];
                                Row.CourseName = xlsFilesAndDirectories[i].split(".")[0].split("_")[0];
                                //Row.Semester = xlsxR.cellAt(6, 7)->readValue().toString();
                                if (!Row.Score.trimmed().isEmpty())
                                    data.append(Row);
                            }
                        }
                        
                    }
                    row++;
                }
            }
        }

    }

    
    if (Program == program::environmental)
        WriteToCSV(folderPath + "/AllData_ENV.csv", data);
    else if (Program == program::ce)
        WriteToCSV(folderPath + "/AllData_CE.csv", data);
    QVector<course_pi_aggregate_item> aggregate_data = ExtractAggregatePI(data);
    if (Program == program::environmental)
        WriteCoursePIAggregateToExcel(folderPath + "/Course_PI_Summary_ENV.xlsx", aggregate_data);
    else if (Program == program::ce)
        WriteCoursePIAggregateToExcel(folderPath + "/Course_PI_Summary_CE.xlsx", aggregate_data);

    // Add this after the existing WritePISummaryToCSV calls
    QVector<pi_aggregate_item> pi_level_aggregate = ExtractPILevelAggregate(aggregate_data);
    if (Program == program::environmental)
        WritePIAggregateToExcel(folderPath + "/PI_Summary_ENV.xlsx", pi_level_aggregate);
    else if (Program == program::ce)
        WritePIAggregateToExcel(folderPath + "/PI_Summary_CE.xlsx", pi_level_aggregate);

    QVector<so_aggregate_item> so_level_aggregate = ExtractSOLevelAggregate(pi_level_aggregate);
    if (Program == program::environmental)
        WriteSOAggregateToExcel(folderPath + "/SO_Summary_ENV.xlsx", so_level_aggregate);
    else if (Program == program::ce)
        WriteSOAggregateToExcel(folderPath + "/SO_Summary_CE.xlsx", so_level_aggregate);
}
void ABETProcessor::OnCreatePITables()
{
    formCreateSpreadSheets *Formcrtspreadsheet = new formCreateSpreadSheets(this);
    ui.horizontalLayout->addWidget(Formcrtspreadsheet);

}

void ABETProcessor::OnCourseEvaluations()
{
    formCourseEvaluations* FormCrsEvals = new formCourseEvaluations(this);
    ui.horizontalLayout->addWidget(FormCrsEvals);
}

bool ABETProcessor::WriteToCSV(const QString& fileaName, const QVector<data_item> &data)
{
    QFile file(fileaName);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out << "firstname,lastname,CourseName,PerformanceIndicator,Program,Semester,Score,ScoreValue\n";

    for (int i = 0; i < data.size(); i++)
        out << data[i].firstname << "," << data[i].lastname << "," << data[i].CourseName << "," << data[i].PerformanceIndicator << "," << data[i].program << "," << data[i].Semester << "," << data[i].Score << "," << scoretonumber(data[i].Score) << "\n";
    file.close();

    return true;
}


bool ABETProcessor::WritePISummaryToCSV(const QString& fileName, const QVector<course_pi_aggregate_item>& coursepiaggdata)
{
    QFile file(fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out << "SO,CourseName,PerformanceIndicator,Percent Satisfactory, Number evaluated\n";

    for (int i = 0; i < coursepiaggdata.size(); i++)
        out << "SO " + coursepiaggdata[i].SO << "," << coursepiaggdata[i].CourseName << "," << coursepiaggdata[i].PerformanceIndicator << "," << coursepiaggdata[i].PercentSatisfactory * 100 << "," << coursepiaggdata[i].number_evaluated << "\n";
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
                    if (data[data_counter].Score.toDouble() > 2)
                        countsatisfied++;
                }
            }
            course_pi_aggregate_item PI_item;
            PI_item.CourseName = CourseNames[course_counter];
            PI_item.PerformanceIndicator = PIs[picounter];
            PI_item.SO = PIs[picounter].split(".")[0];
            PI_item.PercentSatisfactory = countsatisfied / counttotal;
            PI_item.number_evaluated = counttotal;
            out.append(PI_item);

        }

    }
    return out;
}


QString selectFolder(QWidget* parent) {
    QString folderPath = QFileDialog::getExistingDirectory(
        parent,
        "Select Folder",
        QDir::homePath(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );

    if (!folderPath.isEmpty()) {
        qDebug() << "Selected folder:" << folderPath;
    }
    else {
        qDebug() << "No folder selected.";
    }

    return folderPath;
}

bool createSubfolder(const QString& parentFolder, const QString& subfolderName) {
    QDir dir(parentFolder);

    // Ensure the parent folder exists
    if (!dir.exists()) {
        qDebug() << "Parent folder does not exist:" << parentFolder;
        return false;
    }

    // Attempt to create the subfolder
    if (dir.mkdir(subfolderName)) {
        qDebug() << "Subfolder created:" << dir.filePath(subfolderName);
        return true;
    }
    else {
        qDebug() << "Failed to create subfolder or it already exists.";
        return false;
    }
}

QVector<pi_aggregate_item> ABETProcessor::ExtractPILevelAggregate(const QVector<course_pi_aggregate_item>& coursepiaggdata)
{
    QVector<pi_aggregate_item> out;
    QStringList uniquePIs;

    // First, get all unique PIs
    for (int i = 0; i < coursepiaggdata.size(); i++)
    {
        if (!uniquePIs.contains(coursepiaggdata[i].PerformanceIndicator))
            uniquePIs.append(coursepiaggdata[i].PerformanceIndicator);
    }

    // For each unique PI, calculate weighted average and min across all courses
    for (int pi_counter = 0; pi_counter < uniquePIs.size(); pi_counter++)
    {
        pi_aggregate_item PI_item;
        PI_item.PerformanceIndicator = uniquePIs[pi_counter];

        double weighted_sum = 0.0;
        double min_value = 1.0; // Start with maximum possible percentage
        int total_students = 0;
        int course_count = 0;
        QStringList courses;

        // Collect all data for this PI across courses
        for (int data_counter = 0; data_counter < coursepiaggdata.size(); data_counter++)
        {
            if (coursepiaggdata[data_counter].PerformanceIndicator == uniquePIs[pi_counter])
            {
                // Weighted sum: percentage * number of students
                weighted_sum += coursepiaggdata[data_counter].PercentSatisfactory *
                    coursepiaggdata[data_counter].number_evaluated;
                total_students += coursepiaggdata[data_counter].number_evaluated;

                if (coursepiaggdata[data_counter].PercentSatisfactory < min_value)
                    min_value = coursepiaggdata[data_counter].PercentSatisfactory;

                course_count++;
                courses.append(coursepiaggdata[data_counter].CourseName);
            }
        }

        PI_item.SO = uniquePIs[pi_counter].split(".")[0];
        PI_item.WeightedAvgPercentSatisfactory = (total_students > 0) ? weighted_sum / total_students : 0.0;
        PI_item.MinPercentSatisfactory = min_value;
        PI_item.NumberOfCourses = course_count;
        PI_item.TotalStudentsEvaluated = total_students;
        PI_item.CoursesList = courses;

        out.append(PI_item);
    }

    return out;
}
bool ABETProcessor::WritePIAggregateToExcel(const QString& fileName, const QVector<pi_aggregate_item>& piaggdata)
{
    Document xlsx;

    // Sort data by Performance Indicator
    QVector<pi_aggregate_item> sortedData = piaggdata;
    std::sort(sortedData.begin(), sortedData.end(),
        [](const pi_aggregate_item& a, const pi_aggregate_item& b) {
            return a.PerformanceIndicator < b.PerformanceIndicator;
        });

    // Set up header format
    Format headerFormat;
    headerFormat.setFontBold(true);
    headerFormat.setFontSize(12);
    headerFormat.setPatternBackgroundColor(QColor(79, 129, 189)); // Blue background
    headerFormat.setFontColor(QColor(Qt::white));
    headerFormat.setHorizontalAlignment(Format::AlignHCenter);
    headerFormat.setVerticalAlignment(Format::AlignVCenter);
    headerFormat.setBorderStyle(Format::BorderThin);

    // Set up SO heading format
    Format soHeaderFormat;
    soHeaderFormat.setFontBold(true);
    soHeaderFormat.setFontSize(14);
    soHeaderFormat.setPatternBackgroundColor(QColor(169, 208, 142)); // Light green background
    soHeaderFormat.setFontColor(QColor(Qt::black));
    soHeaderFormat.setHorizontalAlignment(Format::AlignLeft);
    soHeaderFormat.setVerticalAlignment(Format::AlignVCenter);
    soHeaderFormat.setBorderStyle(Format::BorderMedium);

    // Format for data cells
    Format dataFormat;
    dataFormat.setBorderStyle(Format::BorderThin);
    dataFormat.setVerticalAlignment(Format::AlignVCenter);

    Format numberFormat;
    numberFormat.setBorderStyle(Format::BorderThin);
    numberFormat.setVerticalAlignment(Format::AlignVCenter);
    numberFormat.setHorizontalAlignment(Format::AlignHCenter);

    int currentRow = 1;
    QString currentSO = "";

    // Write data rows with SO headers
    for (int i = 0; i < sortedData.size(); i++)
    {
        // Check if we need a new SO header
        if (sortedData[i].SO != currentSO)
        {
            currentSO = sortedData[i].SO;

            // Write SO header row
            xlsx.mergeCells(CellRange(currentRow, 1, currentRow, 7), soHeaderFormat);
            xlsx.write(currentRow, 1, "Student Outcome " + currentSO, soHeaderFormat);
            currentRow++;

            // Write column headers for this section
            xlsx.write(currentRow, 1, "Student Outcome", headerFormat);
            xlsx.write(currentRow, 2, "Performance Indicator", headerFormat);
            xlsx.write(currentRow, 3, "Weighted Avg % Satisfactory", headerFormat);
            xlsx.write(currentRow, 4, "Min % Satisfactory", headerFormat);
            xlsx.write(currentRow, 5, "Number of Courses", headerFormat);
            xlsx.write(currentRow, 6, "Total Students", headerFormat);
            xlsx.write(currentRow, 7, "Courses", headerFormat);
            currentRow++;
        }

        // Create format for weighted average based on value
        Format weightedAvgFormat;
        weightedAvgFormat.setBorderStyle(Format::BorderThin);
        weightedAvgFormat.setVerticalAlignment(Format::AlignVCenter);
        weightedAvgFormat.setNumberFormat("0.00%");
        if (sortedData[i].WeightedAvgPercentSatisfactory < 0.50)
            weightedAvgFormat.setPatternBackgroundColor(QColor(255, 182, 193)); // Light pink
        else if (sortedData[i].WeightedAvgPercentSatisfactory < 0.75)
            weightedAvgFormat.setPatternBackgroundColor(QColor(255, 255, 153)); // Light yellow

        // Create format for min based on value
        Format minFormat;
        minFormat.setBorderStyle(Format::BorderThin);
        minFormat.setVerticalAlignment(Format::AlignVCenter);
        minFormat.setNumberFormat("0.00%");
        if (sortedData[i].MinPercentSatisfactory < 0.50)
            minFormat.setPatternBackgroundColor(QColor(255, 182, 193)); // Light pink
        else if (sortedData[i].MinPercentSatisfactory < 0.75)
            minFormat.setPatternBackgroundColor(QColor(255, 255, 153)); // Light yellow

        // Write data row
        xlsx.write(currentRow, 1, sortedData[i].SO, dataFormat);
        xlsx.write(currentRow, 2, sortedData[i].PerformanceIndicator, dataFormat);
        xlsx.write(currentRow, 3, sortedData[i].WeightedAvgPercentSatisfactory, weightedAvgFormat);
        xlsx.write(currentRow, 4, sortedData[i].MinPercentSatisfactory, minFormat);
        xlsx.write(currentRow, 5, sortedData[i].NumberOfCourses, numberFormat);
        xlsx.write(currentRow, 6, sortedData[i].TotalStudentsEvaluated, numberFormat);
        xlsx.write(currentRow, 7, sortedData[i].CoursesList.join(", "), dataFormat);
        currentRow++;
    }

    // Set column widths for better readability
    xlsx.setColumnWidth(1, 18);  // SO
    xlsx.setColumnWidth(2, 25);  // Performance Indicator
    xlsx.setColumnWidth(3, 22);  // Weighted Avg %
    xlsx.setColumnWidth(4, 20);  // Min %
    xlsx.setColumnWidth(5, 18);  // Number of Courses
    xlsx.setColumnWidth(6, 15);  // Total Students
    xlsx.setColumnWidth(7, 40);  // Courses

    return xlsx.saveAs(fileName);
}

QVector<so_aggregate_item> ABETProcessor::ExtractSOLevelAggregate(const QVector<pi_aggregate_item>& piaggdata)
{
    QVector<so_aggregate_item> out;
    QStringList uniqueSOs;

    // First, get all unique SOs
    for (int i = 0; i < piaggdata.size(); i++)
    {
        if (!uniqueSOs.contains(piaggdata[i].SO))
            uniqueSOs.append(piaggdata[i].SO);
    }

    // Sort SOs
    std::sort(uniqueSOs.begin(), uniqueSOs.end());

    // For each unique SO, calculate mean and min across all PIs
    for (int so_counter = 0; so_counter < uniqueSOs.size(); so_counter++)
    {
        so_aggregate_item SO_item;
        SO_item.SO = uniqueSOs[so_counter];

        double sum = 0.0;
        double min_value = 1.0; // Start with maximum possible percentage
        int pi_count = 0;
        int total_students = 0;
        QStringList pis;

        // Collect all PIs for this SO
        for (int data_counter = 0; data_counter < piaggdata.size(); data_counter++)
        {
            if (piaggdata[data_counter].SO == uniqueSOs[so_counter])
            {
                // Use the weighted average from PI level
                sum += piaggdata[data_counter].WeightedAvgPercentSatisfactory;

                if (piaggdata[data_counter].WeightedAvgPercentSatisfactory < min_value)
                    min_value = piaggdata[data_counter].WeightedAvgPercentSatisfactory;

                total_students += piaggdata[data_counter].TotalStudentsEvaluated;
                pi_count++;
                pis.append(piaggdata[data_counter].PerformanceIndicator);
            }
        }

        SO_item.MeanPercentSatisfactory = (pi_count > 0) ? sum / pi_count : 0.0;
        SO_item.MinPercentSatisfactory = min_value;
        SO_item.NumberOfPIs = pi_count;
        SO_item.TotalStudentsEvaluated = total_students;
        SO_item.PIsList = pis;

        out.append(SO_item);
    }

    return out;
}

bool ABETProcessor::WriteSOAggregateToExcel(const QString& fileName, const QVector<so_aggregate_item>& soaggdata)
{
    Document xlsx;

    // Set up header format
    Format headerFormat;
    headerFormat.setFontBold(true);
    headerFormat.setFontSize(12);
    headerFormat.setPatternBackgroundColor(QColor(79, 129, 189)); // Blue background
    headerFormat.setFontColor(QColor(Qt::white));
    headerFormat.setHorizontalAlignment(Format::AlignHCenter);
    headerFormat.setVerticalAlignment(Format::AlignVCenter);
    headerFormat.setBorderStyle(Format::BorderThin);

    // Set up title format
    Format titleFormat;
    titleFormat.setFontBold(true);
    titleFormat.setFontSize(16);
    titleFormat.setPatternBackgroundColor(QColor(68, 114, 196)); // Dark blue background
    titleFormat.setFontColor(QColor(Qt::white));
    titleFormat.setHorizontalAlignment(Format::AlignHCenter);
    titleFormat.setVerticalAlignment(Format::AlignVCenter);
    titleFormat.setBorderStyle(Format::BorderMedium);

    // Format for data cells
    Format dataFormat;
    dataFormat.setBorderStyle(Format::BorderThin);
    dataFormat.setVerticalAlignment(Format::AlignVCenter);
    dataFormat.setHorizontalAlignment(Format::AlignHCenter);

    // Percentage formats with conditional coloring
    Format percentFormat;
    percentFormat.setBorderStyle(Format::BorderThin);
    percentFormat.setVerticalAlignment(Format::AlignVCenter);
    percentFormat.setNumberFormat("0.00%");

    Format percentYellowFormat;
    percentYellowFormat.setBorderStyle(Format::BorderThin);
    percentYellowFormat.setVerticalAlignment(Format::AlignVCenter);
    percentYellowFormat.setNumberFormat("0.00%");
    percentYellowFormat.setPatternBackgroundColor(QColor(255, 255, 153)); // Light yellow

    Format percentPinkFormat;
    percentPinkFormat.setBorderStyle(Format::BorderThin);
    percentPinkFormat.setVerticalAlignment(Format::AlignVCenter);
    percentPinkFormat.setNumberFormat("0.00%");
    percentPinkFormat.setPatternBackgroundColor(QColor(255, 182, 193)); // Light pink

    Format numberFormat;
    numberFormat.setBorderStyle(Format::BorderThin);
    numberFormat.setVerticalAlignment(Format::AlignVCenter);
    numberFormat.setHorizontalAlignment(Format::AlignHCenter);

    Format piListFormat;
    piListFormat.setBorderStyle(Format::BorderThin);
    piListFormat.setVerticalAlignment(Format::AlignVCenter);
    piListFormat.setTextWrap(true);

    int currentRow = 1;

    // Write title
    xlsx.mergeCells(CellRange(currentRow, 1, currentRow, 6), titleFormat);
    xlsx.write(currentRow, 1, "Student Outcome Level Summary", titleFormat);
    xlsx.setRowHeight(currentRow, 25);
    currentRow += 2; // Skip a row

    // Write column headers
    xlsx.write(currentRow, 1, "Student Outcome", headerFormat);
    xlsx.write(currentRow, 2, "Mean % Satisfactory", headerFormat);
    xlsx.write(currentRow, 3, "Min % Satisfactory", headerFormat);
    xlsx.write(currentRow, 4, "Number of PIs", headerFormat);
    xlsx.write(currentRow, 5, "Total Students", headerFormat);
    xlsx.write(currentRow, 6, "Performance Indicators", headerFormat);
    currentRow++;

    // Write data rows
    for (int i = 0; i < soaggdata.size(); i++)
    {
        // Determine format for mean based on value
        Format meanFormat;
        if (soaggdata[i].MeanPercentSatisfactory < 0.50)
            meanFormat = percentPinkFormat;
        else if (soaggdata[i].MeanPercentSatisfactory < 0.75)
            meanFormat = percentYellowFormat;
        else
            meanFormat = percentFormat;

        // Determine format for min based on value
        Format minFormat;
        if (soaggdata[i].MinPercentSatisfactory < 0.50)
            minFormat = percentPinkFormat;
        else if (soaggdata[i].MinPercentSatisfactory < 0.75)
            minFormat = percentYellowFormat;
        else
            minFormat = percentFormat;

        xlsx.write(currentRow, 1, "SO " + soaggdata[i].SO, dataFormat);
        xlsx.write(currentRow, 2, soaggdata[i].MeanPercentSatisfactory, meanFormat);
        xlsx.write(currentRow, 3, soaggdata[i].MinPercentSatisfactory, minFormat);
        xlsx.write(currentRow, 4, soaggdata[i].NumberOfPIs, numberFormat);
        xlsx.write(currentRow, 5, soaggdata[i].TotalStudentsEvaluated, numberFormat);
        xlsx.write(currentRow, 6, soaggdata[i].PIsList.join(", "), piListFormat);
        currentRow++;
    }

    // Set column widths for better readability
    xlsx.setColumnWidth(1, 20);  // SO
    xlsx.setColumnWidth(2, 22);  // Mean %
    xlsx.setColumnWidth(3, 20);  // Min %
    xlsx.setColumnWidth(4, 18);  // Number of PIs
    xlsx.setColumnWidth(5, 15);  // Total Students
    xlsx.setColumnWidth(6, 45);  // PIs List

    return xlsx.saveAs(fileName);
}

bool ABETProcessor::WriteCoursePIAggregateToExcel(const QString& fileName, const QVector<course_pi_aggregate_item>& coursepiaggdata)
{
    Document xlsx;

    // Sort data by Performance Indicator
    QVector<course_pi_aggregate_item> sortedData = coursepiaggdata;
    std::sort(sortedData.begin(), sortedData.end(),
        [](const course_pi_aggregate_item& a, const course_pi_aggregate_item& b) {
            return a.PerformanceIndicator < b.PerformanceIndicator;
        });

    // Set up header format
    Format headerFormat;
    headerFormat.setFontBold(true);
    headerFormat.setFontSize(12);
    headerFormat.setPatternBackgroundColor(QColor(79, 129, 189)); // Blue background
    headerFormat.setFontColor(QColor(Qt::white));
    headerFormat.setHorizontalAlignment(Format::AlignHCenter);
    headerFormat.setVerticalAlignment(Format::AlignVCenter);
    headerFormat.setBorderStyle(Format::BorderThin);

    // Set up PI heading format
    Format piHeaderFormat;
    piHeaderFormat.setFontBold(true);
    piHeaderFormat.setFontSize(14);
    piHeaderFormat.setPatternBackgroundColor(QColor(169, 208, 142)); // Light green background
    piHeaderFormat.setFontColor(QColor(Qt::black));
    piHeaderFormat.setHorizontalAlignment(Format::AlignLeft);
    piHeaderFormat.setVerticalAlignment(Format::AlignVCenter);
    piHeaderFormat.setBorderStyle(Format::BorderMedium);

    // Set up title format
    Format titleFormat;
    titleFormat.setFontBold(true);
    titleFormat.setFontSize(16);
    titleFormat.setPatternBackgroundColor(QColor(68, 114, 196)); // Dark blue background
    titleFormat.setFontColor(QColor(Qt::white));
    titleFormat.setHorizontalAlignment(Format::AlignHCenter);
    titleFormat.setVerticalAlignment(Format::AlignVCenter);
    titleFormat.setBorderStyle(Format::BorderMedium);

    // Format for data cells
    Format dataFormat;
    dataFormat.setBorderStyle(Format::BorderThin);
    dataFormat.setVerticalAlignment(Format::AlignVCenter);

    Format numberFormat;
    numberFormat.setBorderStyle(Format::BorderThin);
    numberFormat.setVerticalAlignment(Format::AlignVCenter);
    numberFormat.setHorizontalAlignment(Format::AlignHCenter);

    int currentRow = 1;

    // Write title
    xlsx.mergeCells(CellRange(currentRow, 1, currentRow, 5), titleFormat);
    xlsx.write(currentRow, 1, "Course-Level Performance Indicator Summary", titleFormat);
    xlsx.setRowHeight(currentRow, 25);
    currentRow += 2; // Skip a row

    QString currentPI = "";

    // Write data rows with PI headers
    for (int i = 0; i < sortedData.size(); i++)
    {
        // Check if we need a new PI header
        if (sortedData[i].PerformanceIndicator != currentPI)
        {
            currentPI = sortedData[i].PerformanceIndicator;

            // Write PI header row
            xlsx.mergeCells(CellRange(currentRow, 1, currentRow, 5), piHeaderFormat);
            xlsx.write(currentRow, 1, "Performance Indicator: " + currentPI + " (SO " + sortedData[i].SO + ")", piHeaderFormat);
            currentRow++;

            // Write column headers for this section
            xlsx.write(currentRow, 1, "Student Outcome", headerFormat);
            xlsx.write(currentRow, 2, "Course Name", headerFormat);
            xlsx.write(currentRow, 3, "Performance Indicator", headerFormat);
            xlsx.write(currentRow, 4, "% Satisfactory", headerFormat);
            xlsx.write(currentRow, 5, "Number Evaluated", headerFormat);
            currentRow++;
        }

        // Create format for percent satisfactory based on value
        Format percentFormat;
        percentFormat.setBorderStyle(Format::BorderThin);
        percentFormat.setVerticalAlignment(Format::AlignVCenter);
        percentFormat.setNumberFormat("0.00%");
        if (sortedData[i].PercentSatisfactory < 0.50)
            percentFormat.setPatternBackgroundColor(QColor(255, 182, 193)); // Light pink
        else if (sortedData[i].PercentSatisfactory < 0.75)
            percentFormat.setPatternBackgroundColor(QColor(255, 255, 153)); // Light yellow

        // Write data row
        xlsx.write(currentRow, 1, "SO " + sortedData[i].SO, dataFormat);
        xlsx.write(currentRow, 2, sortedData[i].CourseName, dataFormat);
        xlsx.write(currentRow, 3, sortedData[i].PerformanceIndicator, dataFormat);
        xlsx.write(currentRow, 4, sortedData[i].PercentSatisfactory, percentFormat);
        xlsx.write(currentRow, 5, sortedData[i].number_evaluated, numberFormat);
        currentRow++;
    }

    // Set column widths for better readability
    xlsx.setColumnWidth(1, 18);  // SO
    xlsx.setColumnWidth(2, 30);  // Course Name
    xlsx.setColumnWidth(3, 25);  // Performance Indicator
    xlsx.setColumnWidth(4, 20);  // % Satisfactory
    xlsx.setColumnWidth(5, 18);  // Number Evaluated

    return xlsx.saveAs(fileName);
}