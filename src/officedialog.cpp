#include "officedialog.h"
#include "ui_officedialog.h"
#include "common_inc.h"

//-------------------------------------------------------------------------------------
OfficeDialog::OfficeDialog(int officeid,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OfficeDialog)
{
    m_officeid=officeid;
    ui->setupUi(this);
    if(m_officeid)
    {
        QSqlQuery query;
        QueryExec(this,"SELECT enabled, performerbillpercent, name, email, url, phone1, phone2, address, managername, notes, ctime FROM offices WHERE id='"+QString::number(m_officeid)+"';",query);
        if(!query.next())
        {
            QMetaObject::invokeMethod(this, "reject", Qt::QueuedConnection); // Close dialog.
            return;
        }
        if(query.value(0).toInt()!=0)
            ui->enabledCheckBox->setChecked(true);
        ui->performerBillPercentSpinBox->setValue(query.value(1).toInt());
        ui->nameLineEdit->setText(query.value(2).toString());
        ui->emailLineEdit->setText(query.value(3).toString());
        ui->urlLineEdit->setText(query.value(4).toString());
        ui->phone1LineEdit->setText(query.value(5).toString());
        ui->phone2LineEdit->setText(query.value(6).toString());
        ui->addressPlainTextEdit->setPlainText(query.value(7).toString());
        ui->managernameLineEdit->setText(query.value(8).toString());
        ui->notesPlainTextEdit->setPlainText(query.value(9).toString());
        ui->ctimeLabel->setText(tr("Создан ")+QDateTime::fromString(query.value(10).toString(),DATETIME_FORMAT).toString("dd.MM.yyyy hh:mm"));
        setWindowTitle(tr("Редактирование офиса %1").arg(m_officeid));
    }
}
//-------------------------------------------------------------------------------------
OfficeDialog::~OfficeDialog()
{
    delete ui;
}
//-------------------------------------------------------------------------------------
void OfficeDialog::on_okPushButton_clicked()
{
    QString enabled="0";
    if(ui->enabledCheckBox->isChecked())
        enabled="1";
    QString name=escapeQuotes(ui->nameLineEdit->text().trimmed());
    QString percent=num(ui->performerBillPercentSpinBox->value());
    QString email=escapeQuotes(ui->emailLineEdit->text().trimmed());
    QString url=escapeQuotes(ui->urlLineEdit->text().trimmed());
    QString phone1=escapeQuotes(ui->phone1LineEdit->text().trimmed());
    QString phone2=escapeQuotes(ui->phone2LineEdit->text().trimmed());
    QString address=escapeQuotes(ui->addressPlainTextEdit->toPlainText().trimmed());
    QString managername=escapeQuotes(ui->managernameLineEdit->text().trimmed());
    QString notes=escapeQuotes(ui->notesPlainTextEdit->toPlainText().trimmed());
    if(m_officeid)
    {
        if(!QueryExec(this,"UPDATE offices SET enabled='"+enabled+"', performerbillpercent='"+percent+"', name='"+name+"', email='"+email+"', url='"+url+"', phone1='"+phone1+"', phone2='"+phone2+"', address='"+address+"', managername='"+managername+"', notes='"+notes+"' WHERE id='"+num(m_officeid)+"';"))
            return;
    }
    else
    {
        QSqlQuery insertSqlQuery;
        if(!QueryExec(this,"INSERT INTO offices(enabled, performerbillpercent, name, email, url, phone1, phone2, address, managername, notes, ctime) VALUES('"+enabled+"', '"+percent+"', '"+name+"', '"+email+"', '"+url+"', '"+phone1+"', '"+phone2+"', '"+address+"', '"+managername+"', '"+notes+"', '"+timestamp()+"');",insertSqlQuery))
            return;
        m_officeid=insertSqlQuery.lastInsertId().toInt();
    }
    accept();
}
//-------------------------------------------------------------------------------------
