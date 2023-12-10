#ifndef FORMCREATESPREADSHEETS_H
#define FORMCREATESPREADSHEETS_H

#include <QDialog>
#include "FilePushButton.h"
#include "pidata.h"

namespace Ui {
class formCreateSpreadSheets;
}

class formCreateSpreadSheets : public QDialog
{
    Q_OBJECT

public:
    explicit formCreateSpreadSheets(QWidget *parent = nullptr);
    ~formCreateSpreadSheets();
    PIData pi_data;

private:
    Ui::formCreateSpreadSheets *ui;
    FilePushButton *StudentData_pb = nullptr;
    FilePushButton *PITable_pb = nullptr;

public slots:
    void OnOkPressed();
};

#endif // FORMCREATESPREADSHEETS_H
