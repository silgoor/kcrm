#include "usersdialog.h"
#include "ui_usersdialog.h"
#include "userdialog.h"
#include "common_inc.h"

extern unsigned int globaluid;
//-------------------------------------------------------------------------------------
UsersDialog::UsersDialog(QWidget *parent) :
    QDialog(parent,Qt::WindowMinMaxButtonsHint|Qt::WindowCloseButtonHint),
    ui(new Ui::UsersDialog)
{
    ui->setupUi(this);
    usersQueryModel = new QSqlQueryModel;
    ui->usersTableView->setModel(usersQueryModel);
    ui->usersTableView->horizontalHeader()->setSortIndicator(2,Qt::AscendingOrder); // Сортировка по умолчанию по имени по возрастающей.
    refreshUsers();
    connect(ui->usersTableView->horizontalHeader(),SIGNAL(sectionClicked(int)),this,SLOT(On_usersHeaderView_sectionClicked()));
    if(!checkRights(this,globaluid,CSRIGHT_EXEC_ADMIN))
    {
        ui->newPushButton->setVisible(false);
        ui->editPushButton->setVisible(false);
    }
}
//-------------------------------------------------------------------------------------
UsersDialog::~UsersDialog()
{
    delete ui;
}
//-------------------------------------------------------------------------------------
void UsersDialog::on_newPushButton_clicked()
{
    if(!checkRights(this,globaluid,CSRIGHT_EXEC_ADMIN))
        return;
    UserDialog * userDialog = new UserDialog(0,this);
    if(userDialog->exec()==QDialog::Accepted)
    {
        refreshUsers();
    }
    delete userDialog;
}
//-------------------------------------------------------------------------------------
void UsersDialog::refreshUsers()
{
    // S O R T
    QString fieldNameString=usersQueryModel->record().fieldName(ui->usersTableView->horizontalHeader()->sortIndicatorSection());
    if(fieldNameString.isEmpty())
        fieldNameString="name";
    QString sortString=" ORDER BY "+fieldNameString;
    if(ui->usersTableView->horizontalHeader()->sortIndicatorOrder()==Qt::DescendingOrder)
        sortString+=" DESC";
    QModelIndex savedIndex=ui->usersTableView->currentIndex();
    QueryModelExec(this,"SELECT id, enabled, name, phone1, phone2, address, ctime FROM users"+sortString+";",usersQueryModel);
    int column_cnt=0;
    usersQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Ид"));
    usersQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Разрешен"));
    usersQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Имя"));
    usersQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Телефон1"));
    usersQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Телефон2"));
    usersQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Адрес"));
    usersQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Создан"));
    ui->usersTableView->resizeColumnsToContents();
    ui->usersTableView->setCurrentIndex(savedIndex);
}
//-------------------------------------------------------------------------------------
void UsersDialog::on_usersTableView_activated(const QModelIndex &index)
{
    on_editPushButton_clicked();
}
//-------------------------------------------------------------------------------------
void UsersDialog::on_editPushButton_clicked()
{
    if(!checkRights(this,globaluid,CSRIGHT_EXEC_ADMIN))
        return;
    int id=usersQueryModel->record(ui->usersTableView->currentIndex().row()).field("id").value().toInt();
    if(!id)
        return;
    UserDialog * userDialog = new UserDialog(id,this);
    if(userDialog->exec()==QDialog::Accepted)
    {
        refreshUsers();
    }
    delete userDialog;
}
//-------------------------------------------------------------------------------------
void UsersDialog::on_closePushButton_clicked()
{
    accept();
}
//-------------------------------------------------------------------------------------
void UsersDialog::On_usersHeaderView_sectionClicked()
{ // Сортировка по выбранной колонке.
    refreshUsers();
}
//-------------------------------------------------------------------------------------
