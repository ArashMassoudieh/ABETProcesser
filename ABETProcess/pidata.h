#ifndef PIDATA_H
#define PIDATA_H

#include <QMap>

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
};

#endif // PIDATA_H
