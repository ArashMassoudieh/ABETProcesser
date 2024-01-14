#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_ABETProcessor.h"

struct data_item
{
    QString firstname;
    QString lastname; 
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

private:
    Ui::ABETProcessorClass ui;

public slots:
    void OnProcessPIFiles();
    void OnCreatePITables();
};
