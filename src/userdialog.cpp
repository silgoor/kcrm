#include "userdialog.h"
#include "ui_userdialog.h"
#include "common_inc.h"


//-------------------------------------------------------------------------------------
UserDialog::UserDialog(int userid, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserDialog)
{
    m_userid=userid;
    ui->setupUi(this);
    refreshOffices();
    if(m_userid>0)
    {
        QSqlQuery userQuery;
        QueryExec(this,"SELECT enabled, login, password, rights, sex, name, birthday, phone1, phone2, email, address, position, pseries, pnumber, pdepartment, paddress, notes FROM users WHERE id='"+num(m_userid)+"';",userQuery);
        if(!userQuery.next())
        {
            QMetaObject::invokeMethod(this, "reject", Qt::QueuedConnection); // Close dialog.
            return;
        }
        if(userQuery.value(0).toInt()==1)
            ui->enabledCheckBox->setChecked(true);
        ui->loginLineEdit->setText(userQuery.value(1).toString());
        ui->passwordLineEdit->setText(decryptPassword(userQuery.value(2).toByteArray()));
        int rights=userQuery.value(3).toInt();
        if(rights&CSRIGHT_EXEC_CART)
            ui->execCartCheckBox->setChecked(true);
        if(rights&CSRIGHT_RETURNSALE)
            ui->returnSaleCheckBox->setChecked(true);
        if(rights&CSRIGHT_EXEC_INCOME)
            ui->execIncomeCheckBox->setChecked(true);
        if(rights&CSRIGHT_EXEC_ADMIN)
            ui->execAdminCheckBox->setChecked(true);
        if(rights&CSRIGHT_EXEC_AUDIT)
            ui->execAuditCheckBox->setChecked(true);
        if(rights&CSRIGHT_EXEC_ORDER)
            ui->execOrderCheckBox->setChecked(true);
        if(rights&CSRIGHT_WEBADMIN)
            ui->webAdminCheckBox->setChecked(true);
        if(rights&CSRIGHT_PERFORMER)
            ui->performerCheckBox->setChecked(true);
        if(userQuery.value(4).toInt()==0)
            ui->maleRadioButton->setChecked(true);
        else
            ui->femaleRadioButton->setChecked(true);
        ui->nameLineEdit->setText(userQuery.value(5).toString());
        ui->birthdayDateEdit->setDate(QDate::fromString(userQuery.value(6).toString(),"yyyy-MM-dd"));
        ui->phone1LineEdit->setText(userQuery.value(7).toString());
        ui->phone2LineEdit->setText(userQuery.value(8).toString());
        ui->emailLineEdit->setText(userQuery.value(9).toString());
        ui->addressPlainTextEdit->setPlainText(userQuery.value(10).toString());
        ui->positionLineEdit->setText(userQuery.value(11).toString());
        ui->pseriesLineEdit->setText(decryptPassword(userQuery.value(12).toByteArray()));
        ui->pnumberLineEdit->setText(decryptPassword(userQuery.value(13).toByteArray()));
        ui->pdepartmentLineEdit->setText(decryptPassword(userQuery.value(14).toByteArray()));
        ui->paddressPlainTextEdit->setPlainText(decryptPassword(userQuery.value(15).toByteArray()));
        ui->notesPlainTextEdit->setPlainText(userQuery.value(16).toString());
        fillPerformerOffices();
    }
}
//-------------------------------------------------------------------------------------
UserDialog::~UserDialog()
{
    delete ui;
}
//-------------------------------------------------------------------------------------
void UserDialog::refreshOffices()
{
    ui->officesListWidget->clear();
    QSqlQuery officeQuery;
    if(!QueryExec(this,"SELECT id,name FROM offices WHERE enabled>0 ORDER BY name;",officeQuery))
        return;
    while(officeQuery.next())
    {
        QListWidgetItem* item = new QListWidgetItem(officeQuery.value(1).toString(), ui->officesListWidget);
        item->setData(Qt::UserRole,officeQuery.value(0));
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable); // set checkable flag
        item->setCheckState(Qt::Unchecked); // AND initialize check state
    }
}
//-------------------------------------------------------------------------------------
void UserDialog::fillPerformerOffices()
{
    QSqlQuery performerOfficeQuery;
    if(!QueryExec(this,"SELECT officeid FROM performeroffices WHERE performerid='"+num(m_userid)+"';",performerOfficeQuery))
        return;
    while(performerOfficeQuery.next())
    {
        int officeId=performerOfficeQuery.value(0).toInt();
        for(int i=0;i<ui->officesListWidget->count();i++)
        {
            if(ui->officesListWidget->item(i)->data(Qt::UserRole).toInt()==officeId)
                ui->officesListWidget->item(i)->setCheckState(Qt::Checked);
        }
    }
}
//-------------------------------------------------------------------------------------
void UserDialog::on_okPushButton_clicked()
{
    QString enabled="0";
    if(ui->enabledCheckBox->isChecked())
        enabled="1";
    QString login=escapeQuotes(ui->loginLineEdit->text());
    QString password=encryptPassword(ui->passwordLineEdit->text());
    int rights=0;
    if(ui->execCartCheckBox->checkState()==Qt::Checked)
        rights|=CSRIGHT_EXEC_CART;
    if(ui->returnSaleCheckBox->checkState()==Qt::Checked)
        rights|=CSRIGHT_RETURNSALE;
    if(ui->execIncomeCheckBox->checkState()==Qt::Checked)
        rights|=CSRIGHT_EXEC_INCOME;
    if(ui->execAdminCheckBox->checkState()==Qt::Checked)
        rights|=CSRIGHT_EXEC_ADMIN;
    if(ui->execAuditCheckBox->checkState()==Qt::Checked)
        rights|=CSRIGHT_EXEC_AUDIT;
    if(ui->execOrderCheckBox->checkState()==Qt::Checked)
        rights|=CSRIGHT_EXEC_ORDER;
    if(ui->webAdminCheckBox->checkState()==Qt::Checked)
        rights|=CSRIGHT_WEBADMIN;
    if(ui->performerCheckBox->checkState()==Qt::Checked)
        rights|=CSRIGHT_PERFORMER;
    QString sex="0";
    if(ui->femaleRadioButton->isChecked())
        sex="1";
    QString name=escapeQuotes(ui->nameLineEdit->text().trimmed());
    QString birthday=ui->birthdayDateEdit->date().toString("yyyy-MM-dd");
    QString phone1=escapeQuotes(ui->phone1LineEdit->text());
    QString phone2=escapeQuotes(ui->phone2LineEdit->text());
    QString email=escapeQuotes(ui->emailLineEdit->text());
    QString address=escapeQuotes(ui->addressPlainTextEdit->toPlainText());
    QString position=escapeQuotes(ui->positionLineEdit->text());
    QString pseries=encryptPassword(ui->pseriesLineEdit->text());
    QString pnumber=encryptPassword(ui->pnumberLineEdit->text());
    QString pdepartment=encryptPassword(ui->pdepartmentLineEdit->text());
    QString paddress=encryptPassword(ui->paddressPlainTextEdit->toPlainText());
    QString notes=escapeQuotes(ui->notesPlainTextEdit->toPlainText());
/*    if(name.isEmpty())
    {
        QMessageBox::information(this,PROGNAME,"Введите пожалуйста логин");
        return;
    }
    if(password.isEmpty())
    {
        QMessageBox::information(this,PROGNAME,"Введите пожалуйста пароль");
        return;
    }*/
    if(m_userid)
    {
        if(!QueryExec(this,"UPDATE users SET enabled='"+enabled+"', login='"+login+"', password='"+password+"', rights='"+num(rights)+"', sex='"+sex+"', name='"+name+"', birthday='"+birthday+"', phone1='"+phone1+"', phone2='"+phone2+"', email='"+email+"', address='"+address+"', position='"+position+"', pseries='"+pseries+"', pnumber='"+pnumber+"', pdepartment='"+pdepartment+"', paddress='"+paddress+"', notes='"+notes+"' WHERE id='"+num(m_userid)+"';"))
            return;
    }
    else
    {
        QSqlQuery insertSqlQuery;
        if(!QueryExec(this,"INSERT INTO users(enabled,login,password,rights,sex,name,birthday,phone1,phone2,email,address,position,pseries,pnumber,pdepartment,paddress,notes,ctime) VALUES('"+enabled+"', '"+login+"', '"+password+"', '"+num(rights)+"', '"+sex+"', '"+name+"', '"+birthday+"', '"+phone1+"', '"+phone2+"', '"+email+"', '"+address+"', '"+position+"', '"+pseries+"', '"+pnumber+"', '"+pdepartment+"', '"+paddress+"', '"+notes+"','"+timestamp()+"');",insertSqlQuery))
            return;
        m_userid=insertSqlQuery.lastInsertId().toInt();
    }
    // Update performer offices.
    QueryExec(this,"DELETE FROM performeroffices WHERE performerid='"+num(m_userid)+"';");
    for(int i=0;i<ui->officesListWidget->count();i++)
    {
        if(ui->officesListWidget->item(i)->checkState()==Qt::Checked)
            QueryExec(this,"INSERT INTO performeroffices(performerid,officeid) VALUES('"+num(m_userid)+"','"+num(ui->officesListWidget->item(i)->data(Qt::UserRole).toInt())+"');");
    }
    accept();
}
//-------------------------------------------------------------------------------------
void UserDialog::on_folderPushButton_clicked()
{
    if(!m_userid)
        return;
    QString folderNameString=num(m_userid);
    QString usersDocumentsPath=getOption(this,"USERS_DOCUMENTS_PATH");
#ifdef Q_OS_WIN
    QString cmdString="explorer.exe "+usersDocumentsPath+"/"+folderNameString;
    cmdString.replace('/','\\');
#endif
#ifdef Q_OS_LINUX
    QString cmdString="nautilus "+usersDocumentsPath+"/"+folderNameString;
#endif
#ifdef Q_OS_MAC
    QString cmdString="open \""+usersDocumentsPath+"/"+folderNameString+"\"";
#endif
//    QMessageBox::information(this,"",cmdString);
    QProcess::startDetached(cmdString);
}
//-------------------------------------------------------------------------------------
