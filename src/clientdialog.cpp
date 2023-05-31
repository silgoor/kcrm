#include "clientdialog.h"
#include "ui_clientdialog.h"
//#include "clientdevicedialog.h"
#include <QMessageBox>

extern unsigned int globaluid;

//-------------------------------------------------------------------------------------
ClientDialog::ClientDialog(quint64 clientid, QString filter, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ClientDialog)
{
    m_clientid=clientid;
	int directionId=0;
    ui->setupUi(this);
    if(m_clientid)
    {
        QSqlQuery clientSqlQuery;
		QueryExec(this,"SELECT sex, surname, name, phone1, phone2, email, address, passport, paddress, directionid FROM clients WHERE id='"+num(m_clientid)+"';",clientSqlQuery);
        if(!clientSqlQuery.next())
        {
            QMetaObject::invokeMethod(this, "reject", Qt::QueuedConnection); // Close dialog.
            return;
        }
        if(clientSqlQuery.value(0).toInt()==2)
            ui->femaleRadioButton->setChecked(true);
        ui->surnameLineEdit->setText(clientSqlQuery.value(1).toString());
        ui->nameLineEdit->setText(clientSqlQuery.value(2).toString());
        ui->phone1LineEdit->setText(clientSqlQuery.value(3).toString());
        ui->phone2LineEdit->setText(clientSqlQuery.value(4).toString());
        ui->emailLineEdit->setText(clientSqlQuery.value(5).toString());
        ui->addressPlainTextEdit->setPlainText(clientSqlQuery.value(6).toString());
        ui->passportPlainTextEdit->setPlainText(clientSqlQuery.value(7).toString());
        ui->paddressPlainTextEdit->setPlainText(clientSqlQuery.value(8).toString());
		directionId=clientSqlQuery.value(9).toInt();
        setWindowTitle(tr("Редактирование клиента  %1").arg(m_clientid));
        if(!checkRights(this,globaluid,CSRIGHT_EXEC_ADMIN))
            ui->okPushButton->hide();
    }
    else // Если новый, пытаемся заполнить поля из фильтра.
    {
        if(!filter.isEmpty())
        {
            if(filter.toInt())
                ui->phone1LineEdit->setText(filter);
            else
                ui->surnameLineEdit->setText(filter);
        }
    }
	refreshDirectionsComboBox(directionId);
/*    clientDevicesQueryModel = new QSqlQueryModel;
    ui->clientDevicesTableView->setModel(clientDevicesQueryModel);
    refreshClientDevices();*/
}
//-------------------------------------------------------------------------------------
ClientDialog::~ClientDialog()
{
    delete ui;
}
//-------------------------------------------------------------------------------------
int ClientDialog::clientId()
{
    return m_clientid;
}
//-------------------------------------------------------------------------------------
void ClientDialog::on_okPushButton_clicked()
{
    QString sex="1";
    QString surname=escapeQuotes(ui->surnameLineEdit->text().trimmed());
    QString name=escapeQuotes(ui->nameLineEdit->text().trimmed());
    QString phone1=escapeQuotes(ui->phone1LineEdit->text()).replace(" ","");
    QString phone2=escapeQuotes(ui->phone2LineEdit->text()).replace(" ","");
    QString email=escapeQuotes(ui->emailLineEdit->text().trimmed());
	int directionId=ui->directionsComboBox->currentData().toInt();
    QString address=escapeQuotes(ui->addressPlainTextEdit->toPlainText().trimmed());
    QString passport=escapeQuotes(ui->passportPlainTextEdit->toPlainText().trimmed());
    QString paddress=escapeQuotes(ui->paddressPlainTextEdit->toPlainText().trimmed());
    // П Р О В Е Р К И
    if(phone1.isEmpty())
    {
        QMessageBox::information(this,PROGNAME,"Телефон 1 не заполнен");
        return;
    }
    if(address.isEmpty())
    {
        QMessageBox::information(this,PROGNAME,"Адрес не заполнен");
        return;
    }
    if(ui->femaleRadioButton->isChecked())
        sex="2";
    if(m_clientid)
    {
		if(!QueryExec(this,"UPDATE clients SET sex='"+sex+"', surname='"+surname+"', name='"+name+"', phone1='"+phone1+"', phone2='"+phone2+"', email='"+email+"', directionid='"+num(directionId)+"', address='"+address+"', passport='"+passport+"', paddress='"+paddress+"', mtime='"+timestamp()+"' WHERE id='"+num(m_clientid)+"';"))
            return;
    }
    else // new
    {
        QSqlQuery insertQuery;
		if(!QueryExec(this,"INSERT INTO clients(sex,surname,name,phone1,phone2,email,directionid,address,passport,paddress,ctime) VALUES('"+sex+"', '"+surname+"', '"+name+"', '"+phone1+"', '"+phone2+"', '"+email+"', '"+num(directionId)+"', '"+address+"', '"+passport+"', '"+paddress+"', '"+timestamp()+"');",insertQuery))
            return;
        m_clientid=insertQuery.lastInsertId().toInt();

    }
    accept();
}
//-------------------------------------------------------------------------------------
/*void ClientDialog::on_newClientDevicePushButton_clicked()
{
    ClientDeviceDialog clientDeviceDialog(m_clientid,0,this);
    if(clientDeviceDialog.exec()==QDialog::Accepted)
        refreshClientDevices();
}
//-------------------------------------------------------------------------------------
void ClientDialog::on_editClientDevicePushButton_clicked()
{
    int id=clientDevicesQueryModel->record(ui->clientDevicesTableView->currentIndex().row()).field("id").value().toInt();
    if(!id)
        return;
    ClientDeviceDialog clientDeviceDialog(m_clientid,id,this);
    if(clientDeviceDialog.exec()==QDialog::Accepted)
        refreshClientDevices();
}
//-------------------------------------------------------------------------------------
void ClientDialog::refreshClientDevices()
{
    QModelIndex savedIndex=ui->clientDevicesTableView->currentIndex();
    QueryModelExec(this,"SELECT clientdevices.id, devicetypes.name, devices.brand, devices.model, clientdevices.serial FROM clientdevices LEFT JOIN devices ON clientdevices.deviceid=devices.id LEFT JOIN devicetypes ON devices.devicetypeid=devicetypes.id WHERE clientdevices.clientid='"+num(m_clientid)+"';",clientDevicesQueryModel);
    int column_cnt=0;
    clientDevicesQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Ид"));
    clientDevicesQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Тип"));
    clientDevicesQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Марка"));
    clientDevicesQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Модель"));
    clientDevicesQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Серийный номер"));
    ui->clientDevicesTableView->resizeColumnsToContents();
    ui->clientDevicesTableView->setCurrentIndex(savedIndex);
}
//-------------------------------------------------------------------------------------
void ClientDialog::on_clientDevicesTableView_activated(const QModelIndex &index)
{
    on_editClientDevicePushButton_clicked();
}*/
//-------------------------------------------------------------------------------------
bool ClientDialog::refreshDirectionsComboBox(int currentId)
{
	QSqlQuery sqlQuery;
	if(!QueryExec(this,"SELECT id, name FROM directions WHERE enabled>0 OR id="+num(currentId)+";",sqlQuery))
	{
		return false;
	}
	while(sqlQuery.next())
	{
		ui->directionsComboBox->addItem(sqlQuery.value(1).toString(),sqlQuery.value(0).toUInt());
	}
	ui->directionsComboBox->setCurrentIndex(ui->directionsComboBox->findData(currentId));
	return true;
}
//-------------------------------------------------------------------------------------
