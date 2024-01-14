#include "formcreatespreadsheets.h"
#include "ui_formcreatespreadsheets.h"
#include "FilePushButton.h"
#include "QLabel"
#include "QtSql/QSql"
#include "pidata.h"


formCreateSpreadSheets::formCreateSpreadSheets(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::formCreateSpreadSheets)
{
    ui->setupUi(this);
    QLabel *label1 = new QLabel("Students' spreadsheet",this);
    QLabel *label2 = new QLabel("PI Table",this);
    StudentData_pb = new FilePushButton(this);
    PITable_pb = new FilePushButton(this);
    ui->formLayout->insertRow(0,label1,StudentData_pb);
    ui->formLayout->insertRow(1,label2,PITable_pb);

    connect(ui->OkButton, SIGNAL(clicked()),this, SLOT(OnOkPressed()));
}

formCreateSpreadSheets::~formCreateSpreadSheets()
{
    delete ui;
}

void formCreateSpreadSheets::OnOkPressed()
{
    QString PIExcelFileName = PITable_pb->text();
    QString StudentsExcelFileName = StudentData_pb->text();
    pi_data.ReadFromExcel(PIExcelFileName);
    student_course_data.ReadFromExcel(StudentsExcelFileName);
    pi_data.CreateExcelFiles(&student_course_data,QDir());
}
