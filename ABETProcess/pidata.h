#ifndef PIDATA_H
#define PIDATA_H

#include <QMap>
#include <studentcoursedata.h>
#include <QDir>

struct PI{
    QString ID;
    QString Description;
    QStringList CoursesApplied;

};


class PIData
{
public:
    PIData();
    PIData(const PIData &RHS);
    PIData& operator==(const PIData &RHS);
    QStringList CourseList;
    QMap<QString,PI> PIList;
    bool ReadFromExcel(QString filename);
    QStringList GetPIsForCourse(const QString &coursename);
    bool CreateExcelFiles(StudentCourseData *studentData,const QDir &OutputLocation);
    bool CreateExcelFile(StudentCourseData *studentData,const QString &course, const QString &OutputFile);
};

#endif // PIDATA_H
