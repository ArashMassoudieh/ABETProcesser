#include "formCourseEvaluations.h"
#include "ui_formCourseEvaluations.h"
#include "FilePushButton.h"
#include "QLabel"
#include "QtSql/QSql"
#include "pidata.h"
#include "qfiledialog.h"
#include "QtSql/qsqldatabase.h"
#include "QtSql/qsqlquery.h"
#include "QtSql/qsqlerror.h"
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
    pi_data.ReadFromExcel(PIExcelFileName, true);
    
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
            "coursenumber varchar(30), "
            "section varchar(30), "
            "instructor varchar(30), "
            "year_semester varchar(30), "
            "evaluated integer, "
            "enrolled integer, "
            "score float)");
        
        db.close();
    }
    else if (!db.tables().contains("CourseEvals"))
    {
        db.open();
        QSqlQuery query;
        query.exec("create table CourseEvals "
            "(id integer primary key, "
            "subject varchar(20), "
            "catalog varchar(30), "
            "coursenumber varchar(30), "
            "section varchar(30), "
            "instructor varchar(30), "
            "year_semester varchar(30), "
            "evaluated integer, "
            "enrolled integer, "
            "score float)");

        db.close();
    }
    
    db.open();
    pi_data.savePIDataToDB(db);
    course_eval_data.AppendtoSqlit(&db);
    createCourseAverageTable(db);
    createPIAverageScoreTable(db);
	createSOAggregatesTable(db);
    db.close(); 
    

}

bool formCourseEvaluations::createPIAverageScoreTable(QSqlDatabase& db)
{
    if (!db.isOpen()) {
        qWarning() << "Database not open!";
        return false;
    }

    QSqlQuery query(db);
    db.transaction();

    // 1. Drop table if it exists
    if (!query.exec("DROP TABLE IF EXISTS PI_Avg_Scores")) {
        qWarning() << "Failed to drop existing table:" << query.lastError().text();
        db.rollback();
        return false;
    }

    // 2. Create new table with SO
    if (!query.exec(R"(
        CREATE TABLE PI_Avg_Scores (
            PI TEXT,
            AvgScore REAL,
            MinScore REAL,
            SO TEXT
        )
    )")) {
        qWarning() << "Failed to create PI_Avg_Scores:" << query.lastError().text();
        db.rollback();
        return false;
    }

    // 3. Insert joined/grouped data including SO
    if (!query.exec(R"(
        INSERT INTO PI_Avg_Scores (PI, AvgScore, MinScore, SO)
        SELECT 
            PI_Course_Map.PI,
            AVG(AvgScore) AS AvgScore,
            MIN(AvgScore) AS MinScore,
            SUBSTR(PI_Course_Map.PI, 1, INSTR(PI_Course_Map.PI, '.') - 1) AS SO
        FROM PI_Course_Map
        INNER JOIN Course_Avg_Scores ON PI_Course_Map.Course = Course_Avg_Scores.coursenumber
        GROUP BY PI_Course_Map.PI
    )")) {
        qWarning() << "Failed to populate PI_Avg_Scores:" << query.lastError().text();
        db.rollback();
        return false;
    }

    return db.commit();
}

bool formCourseEvaluations::createSOAggregatesTable(QSqlDatabase& db)
{
    if (!db.isOpen()) {
        qWarning() << "Database not open!";
        return false;
    }

    QSqlQuery query(db);
    db.transaction();

    // 1. Drop existing table
    if (!query.exec("DROP TABLE IF EXISTS SO_Aggregates")) {
        qWarning() << "Failed to drop SO_Aggregates:" << query.lastError().text();
        db.rollback();
        return false;
    }

    // 2. Create new table
    if (!query.exec(R"(
        CREATE TABLE SO_Aggregates (
            SO TEXT,
            AvgScore REAL,
            MinScore REAL
        )
    )")) {
        qWarning() << "Failed to create SO_Aggregates:" << query.lastError().text();
        db.rollback();
        return false;
    }

    // 3. Populate table
    if (!query.exec(R"(
        INSERT INTO SO_Aggregates (SO, AvgScore, MinScore)
        SELECT 
            SO,
            AVG(AvgScore) AS AvgScore,
            MIN(MinScore) AS MinScore
        FROM PI_Avg_Scores
        GROUP BY SO
    )")) {
        qWarning() << "Failed to populate SO_Aggregates:" << query.lastError().text();
        db.rollback();
        return false;
    }

    return db.commit();
}

bool formCourseEvaluations::createCourseAverageTable(QSqlDatabase& db)
{
    if (!db.isOpen()) {
        qWarning() << "Database not open!";
        return false;
    }

    QSqlQuery query(db);
    db.transaction();

    // 1. Drop the table if it exists
    if (!query.exec("DROP TABLE IF EXISTS Course_Avg_Scores")) {
        qWarning() << "Failed to drop Course_Avg_Scores:" << query.lastError().text();
        db.rollback();
        return false;
    }

    // 2. Create new table
    if (!query.exec(R"(
        CREATE TABLE Course_Avg_Scores (
            coursenumber TEXT,
            AvgScore REAL
        )
    )")) {
        qWarning() << "Failed to create Course_Avg_Scores:" << query.lastError().text();
        db.rollback();
        return false;
    }

    // 3. Populate the table with aggregated data
    if (!query.exec(R"(
        INSERT INTO Course_Avg_Scores (coursenumber, AvgScore)
        SELECT coursenumber, AVG(score)
        FROM CourseEvals
        GROUP BY coursenumber
    )")) {
        qWarning() << "Failed to populate Course_Avg_Scores:" << query.lastError().text();
        db.rollback();
        return false;
    }

    return db.commit();
}