#include "officesdialog.h"
#include "ui_officesdialog.h"
#include "common_inc.h"
#include "officedialog.h"

//-------------------------------------------------------------------------------------
OfficesDialog::OfficesDialog(QWidget *parent) :
    QDialog(parent,Qt::WindowMinMaxButtonsHint|Qt::WindowCloseButtonHint),
    ui(new Ui::OfficesDialog)
{
    ui->setupUi(this);
    officesQueryModel = new QSqlQueryModel;
    ui->officesTableView->setModel(officesQueryModel);
    ui->officesTableView->horizontalHeader()->setSortIndicator(2,Qt::AscendingOrder); // Сортировка по умолчанию по имени по возрастающей.
    refreshOffices();
    connect(ui->officesTableView->horizontalHeader(),SIGNAL(sectionClicked(int)),this,SLOT(On_officesHeaderView_sectionClicked()));
}
//-------------------------------------------------------------------------------------
OfficesDialog::~OfficesDialog()
{
    delete ui;
}
//-------------------------------------------------------------------------------------
void OfficesDialog::on_officesTableView_activated(const QModelIndex &index)
{
    int id=officesQueryModel->record(index.row()).field("id").value().toInt();
    if(!id)
        return;
    OfficeDialog *officeDialog = new OfficeDialog(id,this);
    if(officeDialog->exec()==QDialog::Accepted)
        refreshOffices();
    delete officeDialog;
}
//-------------------------------------------------------------------------------------
void OfficesDialog::on_closePushButton_clicked()
{
    accept();
}
//-------------------------------------------------------------------------------------
void OfficesDialog::refreshOffices()
{
    // S O R T
    QString fieldNameString=officesQueryModel->record().fieldName(ui->officesTableView->horizontalHeader()->sortIndicatorSection());
    if(fieldNameString.isEmpty())
        fieldNameString="name";
    QString sortString=" ORDER BY "+fieldNameString;
    if(ui->officesTableView->horizontalHeader()->sortIndicatorOrder()==Qt::DescendingOrder)
        sortString+=" DESC";
    QueryModelExec(this,"SELECT id,enabled,name,ctime FROM offices"+sortString+";",officesQueryModel);
    ui->officesTableView->resizeColumnsToContents();
    int column_cnt=0;
    officesQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Ид"));
    officesQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Разрешен"));
    officesQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Название"));
    officesQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Дата создания"));
}
//-------------------------------------------------------------------------------------
void OfficesDialog::on_newPushButton_clicked()
{
    OfficeDialog *officeDialog = new OfficeDialog(0,this);
    if(officeDialog->exec()==QDialog::Accepted)
        refreshOffices();
    delete officeDialog;
}
//-------------------------------------------------------------------------------------
void OfficesDialog::On_officesHeaderView_sectionClicked()
{ // Сортировка по выбранной колонке.
    refreshOffices();
}
//-------------------------------------------------------------------------------------
