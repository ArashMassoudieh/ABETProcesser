#ifndef STUDENTCOURSEDATA_H
#define STUDENTCOURSEDATA_H

#include <QString>
#include <QList>
#include <QMap>

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
    bool operator == (const StudentCourseInfo &rhs) const
    {
        bool out=false;
        if (StudentID == rhs.StudentID || rhs.StudentID=="")
            if (Subject == rhs.Subject || rhs.Subject == "")
                if (Catalog == rhs.Catalog || rhs.Catalog == "")
                    if (Section == rhs.Section || rhs.Section == "")
                        out=true;

        return out;
    }

};

class StudentCourseData: public QList<StudentCourseInfo>
{
public:
    StudentCourseData();
    StudentCourseData(const StudentCourseData &stcoursedata);
    StudentCourseData& operator=(const StudentCourseData &stcoursedata);
    bool ReadFromExcel(const QString &filename);
    StudentCourseData Filter(const StudentCourseInfo &info);
    QMap<QString,StudentCourseData> SplitBySections();
};

#endif // STUDENTCOURSEDATA_H
