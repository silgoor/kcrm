#include "performersdialog.h"
#include "ui_performersdialog.h"
#include "performerdialog.h"
#include "common_inc.h"

extern unsigned int globaluid;
//-------------------------------------------------------------------------------------
PerformersDialog::PerformersDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PerformersDialog)
{
    ui->setupUi(this);
    performersQueryModel = new QSqlQueryModel;
    ui->performersTableView->setModel(performersQueryModel);
    refreshPerformers();
    connect(ui->performersTableView->horizontalHeader(),SIGNAL(sectionClicked(int)),this,SLOT(On_performersHeaderView_sectionClicked()));
    ui->performersTableView->horizontalHeader()->setSortIndicator(1,Qt::AscendingOrder);
    refreshPerformers();
    if(globaluid!=1 && globaluid!=2)
        ui->newPushButton->setVisible(false);
}
//-------------------------------------------------------------------------------------
PerformersDialog::~PerformersDialog()
{
    delete ui;
}
//-------------------------------------------------------------------------------------
void PerformersDialog::refreshPerformers()
{
    // S O R T
    QString fieldNameString=performersQueryModel->record().fieldName(ui->performersTableView->horizontalHeader()->sortIndicatorSection());
    if(fieldNameString.isEmpty())
        fieldNameString="name";
    QString sortString=" ORDER BY "+fieldNameString;
    if(ui->performersTableView->horizontalHeader()->sortIndicatorOrder()==Qt::DescendingOrder)
        sortString+=" DESC";
    // F I L T E R
    QString filter;
    // FilterLineEdit
    QString filterLineEdit=escapeQuotes(ui->nameFilterLineEdit->text());
    if(!filterLineEdit.isEmpty())
    {
        filter+=QString((filter.isEmpty())?" WHERE":" AND")+" performers.name LIKE '%"+filterLineEdit+"%'";
    }
    if(ui->enabledFilterCheckBox->isChecked())
        filter+=QString((filter.isEmpty())?" WHERE":" AND")+" performers.enabled>'0'";

    QModelIndex savedIndex=ui->performersTableView->currentIndex();
    QueryModelExec(this,"SELECT id, name, phone1, phone2, email, address, ctime FROM performers"+filter+sortString+";",performersQueryModel);
    int column_cnt=0;
    performersQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Ид"));
    performersQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("ФИО"));
    performersQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Телефон 1"));
    performersQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Телефон 2"));
    performersQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("E-mail"));
    performersQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Адрес"));
//    clientsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Создал"));
    performersQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Дата Создания"));
    ui->performersTableView->resizeColumnsToContents();
    ui->performersTableView->setCurrentIndex(savedIndex);
}
//-------------------------------------------------------------------------------------
void PerformersDialog::on_performersTableView_activated(const QModelIndex &index)
{
    if(globaluid!=1 && globaluid!=2)
        return;
    int id=performersQueryModel->record(index.row()).field("id").value().toInt();
    if(!id)
        return;
    PerformerDialog *performerDialog = new PerformerDialog(id,this);
    if(performerDialog->exec()==QDialog::Accepted)
        refreshPerformers();
}
//-------------------------------------------------------------------------------------
void PerformersDialog::on_newPushButton_clicked()
{
    if(globaluid!=1 && globaluid!=2)
        return;
    PerformerDialog *performerDialog = new PerformerDialog(0,this);
    if(performerDialog->exec()==QDialog::Accepted)
        refreshPerformers();
}
//-------------------------------------------------------------------------------------
void PerformersDialog::on_closePushButton_clicked()
{
    reject();
}
//-------------------------------------------------------------------------------------
void PerformersDialog::On_performersHeaderView_sectionClicked()
{ // Сортировка по выбранной колонке.
    refreshPerformers();
}
//-------------------------------------------------------------------------------------
void PerformersDialog::on_nameFilterLineEdit_textChanged(const QString &arg1)
{
    refreshPerformers();
}
//-------------------------------------------------------------------------------------
void PerformersDialog::on_enabledFilterCheckBox_stateChanged(int arg1)
{
    refreshPerformers();
}
//-------------------------------------------------------------------------------------
