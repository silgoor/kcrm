#include "clientdevicedialog.h"
#include "ui_clientdevicedialog.h"
#include "common_inc.h"
#include "devicedialog.h"

//-------------------------------------------------------------------------------------
ClientDeviceDialog::ClientDeviceDialog(int clientId,int clientDeviceId,QWidget *parent) :
    m_clientId(clientId),
    m_clientDeviceId(clientDeviceId),
    QDialog(parent),
    ui(new Ui::ClientDeviceDialog)
{
    ui->setupUi(this);
    if(m_clientDeviceId)
    {
        QSqlQuery sqlQuery;
        QueryExec(this,"SELECT clientid, deviceid, serial FROM clientdevices WHERE id='"+num(m_clientDeviceId)+"';",sqlQuery);
        if(!sqlQuery.next())
        {
            QMetaObject::invokeMethod(this, "reject", Qt::QueuedConnection); // Close dialog.
            return;
        }
        m_clientId=sqlQuery.value(0).toInt();
        refreshDeviceComboBox(sqlQuery.value(1).toInt());
        ui->serialLineEdit->setText(sqlQuery.value(2).toString());
        setWindowTitle(tr("Редактирование клиентского оборудования %1").arg(m_clientDeviceId));
    }
    else
    {
        refreshDeviceComboBox(0);
    }
}
//-------------------------------------------------------------------------------------
ClientDeviceDialog::~ClientDeviceDialog()
{
    delete ui;
}
//-------------------------------------------------------------------------------------
void ClientDeviceDialog::on_okPushButton_clicked()
{
    QString serial=escapeQuotes(ui->serialLineEdit->text());
    if(m_clientDeviceId)
    {
        if(!QueryExec(this,"UPDATE clientdevices SET deviceid='"+ui->deviceComboBox->currentData().toString()+"', serial='"+serial+"' WHERE id='"+num(m_clientDeviceId)+"';"))
            return;
    }
    else // new
    {
        QSqlQuery insertQuery;
        if(!QueryExec(this,"INSERT INTO clientdevices(clientid,deviceid,serial,ctime) VALUES('"+num(m_clientId)+"','"+ui->deviceComboBox->currentData().toString()+"','"+serial+"', '"+timestamp()+"');",insertQuery))
            return;
        m_clientDeviceId=insertQuery.lastInsertId().toInt();
    }
    accept();

}
//-------------------------------------------------------------------------------------
void ClientDeviceDialog::refreshDeviceComboBox(int defaultId)
{
    bool deviceComboBlock=ui->deviceComboBox->blockSignals(true);
    ui->deviceComboBox->clear();
    QSqlQuery sqlQuery;
    QueryExec(this,"SELECT devices.id, devicetypes.name, devices.brand, devices.model FROM devices LEFT JOIN devicetypes ON devices.devicetypeid=devicetypes.id ORDER BY devicetypes.name, devices.brand, devices.model;",sqlQuery);
    while(sqlQuery.next())
    {
        ui->deviceComboBox->addItem(sqlQuery.value(1).toString()+" "+sqlQuery.value(2).toString()+" "+sqlQuery.value(3).toString(),sqlQuery.value(0));
    }
    ui->deviceComboBox->insertSeparator(ui->deviceComboBox->count()-1);
    ui->deviceComboBox->addItem(tr("Добавить"),-2);
    ui->deviceComboBox->blockSignals(deviceComboBlock);
    if(defaultId>0)
        ui->deviceComboBox->setCurrentIndex(ui->deviceComboBox->findData(defaultId));
    else
        ui->deviceComboBox->setCurrentIndex(-1);
}
//-------------------------------------------------------------------------------------
void ClientDeviceDialog::on_deviceComboBox_activated(int index)
{
    // New
    if(ui->deviceComboBox->currentData().toInt()==-2)
    {
        DeviceDialog deviceDialog(0,this);
        if(deviceDialog.exec()==QDialog::Accepted)
        {
            int savedId=ui->deviceComboBox->currentData().toInt();
            refreshDeviceComboBox(0);
            ui->deviceComboBox->setCurrentIndex(ui->deviceComboBox->findData(savedId));
        }
        else
            ui->deviceComboBox->setCurrentIndex(-1);
    }
}
