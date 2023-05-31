#include "clientsdialog.h"
#include "ui_clientsdialog.h"
#include "clientdialog.h"


//-------------------------------------------------------------------------------------
ClientsDialog::ClientsDialog(int clientid,QWidget *parent) :
    QDialog(parent,Qt::WindowMinMaxButtonsHint|Qt::WindowCloseButtonHint),
    ui(new Ui::ClientsDialog)
{
    m_id=0;
    ui->setupUi(this);
    // Clients.
    clientsQueryModel = new QSqlQueryModel;
    ui->clientsTableView->setModel(clientsQueryModel);
    refreshClients();
    if(clientid>0)// Выделение определенного клиента для удобства
    {
        QModelIndexList modelIndexList=clientsQueryModel->match(clientsQueryModel->index(0,0),Qt::DisplayRole,num(clientid),1,Qt::MatchExactly);
        if(modelIndexList.size())
        {
            ui->clientsTableView->selectRow(modelIndexList.at(0).row());
        }
    }
}
//-------------------------------------------------------------------------------------
ClientsDialog::~ClientsDialog()
{
    delete ui;
}
//-------------------------------------------------------------------------------------
int ClientsDialog::clientId()
{
    return m_id;
}
//-------------------------------------------------------------------------------------
void ClientsDialog::refreshClients()
{
    QString filter;
    QString filterLineEdit=escapeQuotes(ui->filterLineEdit->text());
    if(!filterLineEdit.isEmpty())
        filter+=QString((filter.isEmpty())?" WHERE":" AND")+" (clients.surname LIKE '%"+filterLineEdit+"%' OR clients.name LIKE '%"+filterLineEdit+"%' OR clients.phone1 LIKE '%"+filterLineEdit+"%' OR clients.phone2 LIKE '%"+filterLineEdit+"%')";
    QModelIndex savedIndex=ui->clientsTableView->currentIndex();
    QueryModelExec(this,"SELECT clients.id, surname, name, phone1, phone2, address, clients.ctime FROM clients "+filter+" ORDER BY clients.name;",clientsQueryModel);
    int column_cnt=0;
    clientsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Ид"));
    clientsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Фамилия"));
    clientsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Имя"));
    clientsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Телефон 1"));
    clientsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Телефон 2"));
    clientsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Адрес"));
//    clientsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Создал"));
    clientsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Дата Создания"));
    ui->clientsTableView->resizeColumnsToContents();
    ui->clientsTableView->setCurrentIndex(savedIndex);
    if(clientsQueryModel->rowCount()<30)
        ui->newclientPushButton->setEnabled(true);
    else
        ui->newclientPushButton->setEnabled(false);
}
//-------------------------------------------------------------------------------------
void ClientsDialog::on_clientsTableView_activated(const QModelIndex &index)
{
    m_id=clientsQueryModel->record(index.row()).field("id").value().toInt();
    if(!m_id)
        return;
    accept();
}
//-------------------------------------------------------------------------------------
void ClientsDialog::on_newclientPushButton_clicked()
{
    ClientDialog * clientDialog = new ClientDialog(0,ui->filterLineEdit->text(),this);
    if(clientDialog->exec()==QDialog::Accepted)
        refreshClients();
    delete clientDialog;
    // Выбираем строку с созданным клиентом.
    QModelIndexList modelIndexList=clientsQueryModel->match(clientsQueryModel->index(0,0),Qt::DisplayRole,num(clientDialog->clientId()),1,Qt::MatchExactly);
    if(modelIndexList.size())
    {
        ui->clientsTableView->selectRow(modelIndexList.at(0).row());
    }
}
//-------------------------------------------------------------------------------------
void ClientsDialog::on_editPushButton_clicked()
{
    m_id=clientsQueryModel->record(ui->clientsTableView->currentIndex().row()).field("id").value().toInt();
    if(!m_id)
        return;
    ClientDialog * clientDialog = new ClientDialog(m_id,"",this);
    if(clientDialog->exec()==QDialog::Accepted)
        refreshClients();
    delete clientDialog;
}
//-------------------------------------------------------------------------------------
void ClientsDialog::on_filterLineEdit_textChanged(const QString &arg1)
{
    refreshClients();
}
//-------------------------------------------------------------------------------------
void ClientsDialog::on_okPushButton_clicked()
{
    on_clientsTableView_activated(ui->clientsTableView->currentIndex());
}
//-------------------------------------------------------------------------------------
