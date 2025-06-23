#ifndef PIDATA_H
#define PIDATA_H

#include <QMap>
#include <studentcoursedata.h>
#include <QDir>
#include <QSqlDatabase>

struct PI{
    QString ID;
    QString Description;
    QStringList CoursesApplied;

};


class PIData: public QMap<QString,PI>
{
public:
    PIData();
    PIData(const PIData &RHS);
    PIData& operator==(const PIData &RHS);
    QStringList CourseList;

    bool ReadFromExcel(QString filename, bool allcourses);
    bool savePIDataToDB(QSqlDatabase& db);
    void savePIMapToCSV(const QString& filePath);
    QStringList GetPIsForCourse(const QString &coursename);
    bool CreateExcelFiles(StudentCourseData *studentData,const QDir &OutputLocation);
    bool CreateExcelFile(StudentCourseData *studentData,const QString &course, const QString &OutputFile);
    bool CreateExcelFile(StudentCourseData *studentData,const QString &course, const QString &OutputFile, const QString &section);
};

#endif // PIDATA_H
