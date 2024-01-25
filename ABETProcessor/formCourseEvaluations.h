#ifndef FORMCOURSEEVALUATIONS_H
#define FORMCOURSEEVALUATIONS_H

#include <QDialog>
#include "FilePushButton.h"
#include "pidata.h"
#include "studentcoursedata.h"
#include "CourseEvaluationData.h"

namespace Ui {
    class formCourseEvaluations;
}

class formCourseEvaluations : public QDialog
{
    Q_OBJECT

public:
    explicit formCourseEvaluations(QWidget* parent = nullptr);
    ~formCourseEvaluations();
    PIData pi_data;
    CourseEvaluationData course_eval_data;

private:
    Ui::formCourseEvaluations* ui;
    QPushButton* CourseEvaluationFolder = nullptr;
    FilePushButton* PITable_pb = nullptr;

public slots:
    void OnOkPressed();
    void OnChooseFolder();
};

#endif // FORMCOURSEEVALUATIONS_H

