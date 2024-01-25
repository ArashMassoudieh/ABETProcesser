#pragma once
#include <qmap.h>
#include <qvector.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>

struct CourseEvalItems
{
	QString Subject;
	QString Catalog;
	QString Section;
	QString CourseTitle; 
	QString Instructor; 
	QString Year_Semester; 
	int Evaluated; 
	int Enrolled;
	double score; 

};

class CourseEvaluationData : public QVector<CourseEvalItems>
{
public:
	CourseEvaluationData();
	CourseEvaluationData(const CourseEvaluationData& RHS);
	CourseEvaluationData& operator==(const CourseEvaluationData& RHS);
	bool ReadFromExcel(QString filename);
	bool ReadFromDirectory(QString Folder);
	bool AppendtoSqlit(QSqlDatabase* db);
private:


};

