#include "trestsdialog.h"
#include "ui_trestsdialog.h"

#include "trestdialog.h"
#include "common_inc.h"

//-------------------------------------------------------------------------------------
TrestsDialog::TrestsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TrestsDialog)
{
    ui->setupUi(this);
    trestsQueryModel = new QSqlQueryModel;
    ui->trestsTableView->setModel(trestsQueryModel);
    refreshTrests();
}
//-------------------------------------------------------------------------------------
TrestsDialog::~TrestsDialog()
{
    delete ui;
}
//-------------------------------------------------------------------------------------
void TrestsDialog::refreshTrests()
{
    // S O R T
    QueryModelExec(this,"SELECT id, CASE enabled WHEN 0 THEN '-' ELSE '"+tr("Да")+"' END AS trestenabled, name FROM trests ORDER BY id;",trestsQueryModel);
    ui->trestsTableView->resizeColumnsToContents();
    int column_cnt=0;
    trestsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Ид"));
    trestsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Разрешен"));
    trestsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Название"));
}
//-------------------------------------------------------------------------------------
void TrestsDialog::on_trestsTableView_activated(const QModelIndex &index)
{
    quint64 id=trestsQueryModel->record(index.row()).field("id").value().toULongLong();
    if(!id)
        return;
    TrestDialog *trestDialog = new TrestDialog(id,this);
    if(trestDialog->exec()==QDialog::Accepted)
        refreshTrests();
    delete trestDialog;
}
//-------------------------------------------------------------------------------------
void TrestsDialog::on_newPushButton_clicked()
{
    TrestDialog *trestDialog = new TrestDialog(0,this);
    if(trestDialog->exec()==QDialog::Accepted)
        refreshTrests();
    delete trestDialog;
}
//-------------------------------------------------------------------------------------
