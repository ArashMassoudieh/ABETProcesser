#ifndef STUDENTCOURSEDATA_H
#define STUDENTCOURSEDATA_H

#include <QString>
#include <QList>

struct StudentCourseInfo
{
    QString StudentID;
    QString StudentName;
    QString StrtLevel;
    QString AcadProg;
    QString AcadPlan;
    QString Subject;
    QString Catalog;
    QString Section;
    QString Title;
    int unit = 0;
    QString RepeatCode;
    QString RepeatCodeDescr;
    QString GradeIn;
    QString OfficialGrade;
    QString GradingBasis;
    double CumGPA = 0;
    double CurrentGPA = 0;

};

class StudentCourseData: public QList<StudentCourseInfo>
{
public:
    StudentCourseData();
    StudentCourseData(const StudentCourseData &stcoursedata);
    StudentCourseData& operator=(const StudentCourseData &stcoursedata);
    bool ReadFromExcel(const QString &filename);
};

#endif // STUDENTCOURSEDATA_H
