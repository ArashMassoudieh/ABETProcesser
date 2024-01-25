#include "formCourseEvaluations.h"
#include "ui_formCourseEvaluations.h"
#include "FilePushButton.h"
#include "QLabel"
#include "QtSql/QSql"
#include "pidata.h"
#include "qfiledialog.h"
#include "QtSql/qsqldatabase.h"
#include "QtSql/qsqlquery.h"
#include "qdebug.h"


formCourseEvaluations::formCourseEvaluations(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::formCourseEvaluations)
{
    ui->setupUi(this);
    QLabel* label1 = new QLabel("Course Evaluation Folder", this);
    QLabel* label2 = new QLabel("PI Table", this);
    CourseEvaluationFolder = new QPushButton(this);
    PITable_pb = new FilePushButton(this);
    ui->formLayout->insertRow(0, label1, CourseEvaluationFolder);
    ui->formLayout->insertRow(1, label2, PITable_pb);

    connect(ui->OkButton, SIGNAL(clicked()), this, SLOT(OnOkPressed()));
    connect(CourseEvaluationFolder, SIGNAL(clicked()), this, SLOT(OnChooseFolder()));
}

formCourseEvaluations::~formCourseEvaluations()
{
    delete ui;
}

void formCourseEvaluations::OnChooseFolder()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
        "/home",
        QFileDialog::ShowDirsOnly
        | QFileDialog::DontResolveSymlinks);
    CourseEvaluationFolder->setText(dir);
}

void formCourseEvaluations::OnOkPressed()
{
    QString PIExcelFileName = PITable_pb->text();
    QString EvaluationsFolder = CourseEvaluationFolder->text();
    pi_data.ReadFromExcel(PIExcelFileName);
    course_eval_data.ReadFromDirectory(CourseEvaluationFolder->text());
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");//not dbConnection
    db.setDatabaseName(CourseEvaluationFolder->text() + "/" + "AY_Evals.db");
    qDebug() << db.databaseName();
    if (!QFileInfo::exists(CourseEvaluationFolder->text() + "/" + "AY_Evals.db")) {
        db.open();
        QSqlQuery query;
        query.exec("create table CourseEvals "
            "(id integer primary key, "
            "subject varchar(20), "
            "catalog varchar(30), "
            "section varchar(30), "
            "instructor varchar(30), "
            "year_semester varchar(30), "
            "evaluated integer, "
            "enrolled integer, "
            "score float)");

        db.close();
    }
    
    course_eval_data.AppendtoSqlit(&db);

}

