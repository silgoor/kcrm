#include "optionsdialog.h"
#include "ui_optionsdialog.h"
#include "common_inc.h"
#include "optiondialog.h"

//-------------------------------------------------------------------------------------
OptionsDialog::OptionsDialog(QWidget *parent) :
    QDialog(parent,Qt::WindowMinMaxButtonsHint|Qt::WindowCloseButtonHint),
    ui(new Ui::OptionsDialog)
{
    ui->setupUi(this);
    optionsQueryModel = new QSqlQueryModel;
    ui->optionsTableView->setModel(optionsQueryModel);
    refreshOptions();
}
//-------------------------------------------------------------------------------------
OptionsDialog::~OptionsDialog()
{
    delete ui;
}
//-------------------------------------------------------------------------------------
void OptionsDialog::on_closePushButton_clicked()
{
    accept();
}
//-------------------------------------------------------------------------------------
void OptionsDialog::refreshOptions()
{
    QueryModelExec(this,"SELECT id,localname,value,timestamp FROM options;",optionsQueryModel);
    ui->optionsTableView->resizeColumnsToContents();
    int column_cnt=0;
    optionsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Ид"));
    optionsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Параметр"));
    optionsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Значение"));
    optionsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Дата изменения"));
}
//-------------------------------------------------------------------------------------
void OptionsDialog::on_optionsTableView_activated(const QModelIndex &index)
{
    int id=optionsQueryModel->record(index.row()).field("id").value().toInt();
    if(!id)
        return;
    OptionDialog *optionDialog = new OptionDialog(id,this);
    if(optionDialog->exec()==QDialog::Accepted)
    {
        refreshOptions();
    }
    delete optionDialog;
}
//-------------------------------------------------------------------------------------
