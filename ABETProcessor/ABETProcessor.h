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

class ABETProcessor : public QMainWindow
{
    Q_OBJECT

public:
    ABETProcessor(QWidget *parent = Q_NULLPTR);
    bool WriteToCSV(const QString& fileaName, const QVector<data_item>& data);
    static int scoretonumber(const QString& score);

private:
    Ui::ABETProcessorClass ui;
};
