#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_ABETProcessor.h"



struct data_item
{
    QString firstname;
    QString lastname;
    QString studentID;
    QString program;
    QString Score; 
    QString PerformanceIndicator; 
    QString CourseName; 
    QString Semester; 

};

struct course_pi_aggregate_item
{
    QString SO; 
    double PercentSatisfactory;
    QString PerformanceIndicator;
    QString CourseName;
    int number_evaluated; 
    
};

struct pi_aggregate_item {
    QString SO;
    QString PerformanceIndicator;
    double WeightedAvgPercentSatisfactory;  // Changed from Mean
    double MinPercentSatisfactory;
    int NumberOfCourses;
    int TotalStudentsEvaluated;  // Add this field
    QStringList CoursesList;
};

struct so_aggregate_item {
    QString SO;
    double MeanPercentSatisfactory;
    double MinPercentSatisfactory;
    int NumberOfPIs;
    int TotalStudentsEvaluated;
    QStringList PIsList;
};

class ABETProcessor : public QMainWindow
{
    Q_OBJECT

public:
    ABETProcessor(QWidget *parent = Q_NULLPTR);
    bool WriteToCSV(const QString& fileaName, const QVector<data_item>& data);
    static int scoretonumber(const QString& score);
    QStringList AllCourseNames(QVector<data_item>& data);
    QVector<course_pi_aggregate_item> ExtractAggregatePI(QVector<data_item>& data);
    QStringList PIsforCourse(QString& CourseName, QVector<data_item>& data);
    bool WritePISummaryToCSV(const QString& fileName, const QVector<course_pi_aggregate_item>& coursepiaggdata);
    enum class program { ce, environmental } Program = program::environmental;
    QVector<pi_aggregate_item> ExtractPILevelAggregate(const QVector<course_pi_aggregate_item>& coursepiaggdata);
    bool WritePIAggregateToExcel(const QString& fileName, const QVector<pi_aggregate_item>& piaggdata);
    QVector<so_aggregate_item> ExtractSOLevelAggregate(const QVector<pi_aggregate_item>& piaggdata);
    bool WriteSOAggregateToExcel(const QString& fileName, const QVector<so_aggregate_item>& soaggdata);
    bool WriteCoursePIAggregateToExcel(const QString& fileName, const QVector<course_pi_aggregate_item>& coursepiaggdata);
private:
    Ui::ABETProcessorClass ui;

public slots:
    void OnProcessPIFiles();
    void OnCreatePITables();
    void OnCourseEvaluations();
};


QString selectFolder(QWidget* parent = nullptr);
bool createSubfolder(const QString& parentFolder, const QString& subfolderName);