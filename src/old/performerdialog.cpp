#include "performerdialog.h"
#include "ui_performerdialog.h"
#include "common_inc.h"

//-------------------------------------------------------------------------------------
PerformerDialog::PerformerDialog(int performerid, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PerformerDialog)
{
    ui->setupUi(this);
    m_performerid=performerid;
    refreshOffices();
    if(m_performerid)
    {
        QSqlQuery performerQuery;
        QueryExec(this,"SELECT enabled, login, password, rights, sex, name, birthday, phone1, phone2, email, address, pseries, pnumber, pdepartment, paddress, notes FROM users WHERE id='"+num(m_performerid)+"';",performerQuery);
        if(!performerQuery.next())
        {
            QMetaObject::invokeMethod(this, "reject", Qt::QueuedConnection); // Close dialog.
            return;
        }
        if(performerQuery.value(0).toInt()==1)
            ui->enabledCheckBox->setChecked(true);
        ui->loginLineEdit->setText(performerQuery.value(1).toString());
        ui->passwordLineEdit->setText(performerQuery.value(2).toString());
        int rights=performerQuery.value(3).toInt();
        if(rights&CSRIGHT_WEBADMIN)
            ui->webadminCheckBox->setChecked(true);
        if(performerQuery.value(4).toInt()==0)
            ui->maleRadioButton->setChecked(true);
        else
            ui->femaleRadioButton->setChecked(true);
        ui->nameLineEdit->setText(performerQuery.value(5).toString());
        ui->birthdayDateEdit->setDate(QDate::fromString(performerQuery.value(6).toString(),"yyyy-MM-dd"));
        ui->phone1LineEdit->setText(performerQuery.value(7).toString());
        ui->phone2LineEdit->setText(performerQuery.value(8).toString());
        ui->emailLineEdit->setText(performerQuery.value(9).toString());
        ui->addressPlainTextEdit->setPlainText(performerQuery.value(10).toString());
        ui->pseriesLineEdit->setText(decryptPassword(performerQuery.value(11).toByteArray()));
        ui->pnumberLineEdit->setText(decryptPassword(performerQuery.value(12).toByteArray()));
        ui->pdepartmentLineEdit->setText(decryptPassword(performerQuery.value(13).toByteArray()));
        ui->paddressPlainTextEdit->setPlainText(decryptPassword(performerQuery.value(14).toByteArray()));
        ui->notesPlainTextEdit->setPlainText(performerQuery.value(15).toString());
        fillPerformerOffices();
        setWindowTitle(tr("Редактирование мастера %1").arg(m_performerid));
    }
}
//-------------------------------------------------------------------------------------
void PerformerDialog::refreshOffices()
{
    ui->officesListWidget->clear();
    QSqlQuery officeQuery;
    if(!QueryExec(this,"SELECT id,name FROM offices WHERE enabled>0;",officeQuery))
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
void PerformerDialog::fillPerformerOffices()
{
    QSqlQuery performerOfficeQuery;
    if(!QueryExec(this,"SELECT officeid FROM performeroffices WHERE performerid='"+num(m_performerid)+"';",performerOfficeQuery))
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
PerformerDialog::~PerformerDialog()
{
    delete ui;
}
//-------------------------------------------------------------------------------------
void PerformerDialog::on_okPushButton_clicked()
{
    QString enabled="0";
    if(ui->enabledCheckBox->isChecked())
        enabled="1";
    QString login=escapeQuotes(ui->loginLineEdit->text());
    QString password=escapeQuotes(ui->passwordLineEdit->text());
    int rights=0;
    if(ui->webadminCheckBox->isChecked())
        rights|=CSRIGHT_WEBADMIN;
    QString sex="0";
    if(ui->femaleRadioButton->isChecked())
        sex="1";
    QString name=escapeQuotes(ui->nameLineEdit->text().trimmed());
    QString birthday=ui->birthdayDateEdit->date().toString("yyyy-MM-dd");
    QString phone1=escapeQuotes(ui->phone1LineEdit->text());
    QString phone2=escapeQuotes(ui->phone2LineEdit->text().trimmed());
    QString email=escapeQuotes(ui->emailLineEdit->text().trimmed());
    QString address=escapeQuotes(ui->addressPlainTextEdit->toPlainText().trimmed());
    QString pseries=encryptPassword(ui->pseriesLineEdit->text());
    QString pnumber=encryptPassword(ui->pnumberLineEdit->text());
    QString pdepartment=encryptPassword(ui->pdepartmentLineEdit->text());
    QString paddress=encryptPassword(ui->paddressPlainTextEdit->toPlainText());
    QString notes=escapeQuotes(ui->notesPlainTextEdit->toPlainText().trimmed());
    if(m_performerid)
    {
        if(!QueryExec(this,"UPDATE performers SET enabled='"+enabled+"', login='"+login+"', password='"+password+"', rights='"+num(rights)+"', sex='"+sex+"', name='"+name+"', birthday='"+birthday+"', phone1='"+phone1+"', phone2='"+phone2+"', email='"+email+"', address='"+address+"', pseries='"+pseries+"', pnumber='"+pnumber+"', pdepartment='"+pdepartment+"', paddress='"+paddress+"', notes='"+notes+"' WHERE id='"+num(m_performerid)+"';"))
            return;
    }
    else
    {
        QSqlQuery insertSqlQuery;
        if(!QueryExec(this,"INSERT INTO performers(enabled, login, password, rights, sex, name, birthday, phone1, phone2, email, address, pseries, pnumber, pdepartment, paddress, notes, ctime) VALUES('"+enabled+"', '"+login+"', '"+password+"', '"+num(rights)+"', '"+sex+"', '"+name+"', '"+birthday+"', '"+phone1+"', '"+phone2+"', '"+email+"', '"+address+"', '"+pseries+"', '"+pnumber+"', '"+pdepartment+"', '"+paddress+"', '"+notes+"', '"+timestamp()+"');",insertSqlQuery))
            return;
        m_performerid=insertSqlQuery.lastInsertId().toInt();
    }
    // Update performer offices.
    QueryExec(this,"DELETE FROM performeroffices WHERE performerid='"+num(m_performerid)+"';");
    for(int i=0;i<ui->officesListWidget->count();i++)
    {
        if(ui->officesListWidget->item(i)->checkState()==Qt::Checked)
            QueryExec(this,"INSERT INTO performeroffices(performerid,officeid) VALUES('"+num(m_performerid)+"','"+num(ui->officesListWidget->item(i)->data(Qt::UserRole).toInt())+"');");
    }
    accept();
}
//-------------------------------------------------------------------------------------
void PerformerDialog::on_folderPushButton_clicked()
{
    if(!m_performerid)
        return;
    QString folderNameString=num(m_performerid);
    QString usersDocumentsPath=getOption(this,"PERFORMERS_DOCUMENTS_PATH");
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
