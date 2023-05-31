#include "clearamountsdialog.h"
#include "ui_clearamountsdialog.h"
#include "common_inc.h"
#include <QMessageBox>

//-------------------------------------------------------------------------------------
ClearAmountsDialog::ClearAmountsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ClearAmountsDialog)
{
    ui->setupUi(this);
}
//-------------------------------------------------------------------------------------
ClearAmountsDialog::~ClearAmountsDialog()
{
    delete ui;
}
//-------------------------------------------------------------------------------------
void ClearAmountsDialog::on_clearPushButton_clicked()
{
    QString performDate=ui->earlerDateEdit->date().toString("yyyy-MM-dd 00:00:00");
    if(QMessageBox::question(this, PROGNAME, tr("Обнулить все суммы до %1 ?").arg(performDate),tr("ОБНУЛИТЬ"),tr("Отмена"))!=0)
        return;
    if(!QueryExec(this,"UPDATE requests SET clientcost=0, cost=0 WHERE performdate<'"+performDate+"';"))
        return;
    accept();
}
//-------------------------------------------------------------------------------------
