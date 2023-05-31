#include "devicedialog.h"
#include "ui_devicedialog.h"
#include "common_inc.h"

//-------------------------------------------------------------------------------------
DeviceDialog::DeviceDialog(int deviceId,QWidget *parent) :
    m_deviceId(deviceId),
    QDialog(parent),
    ui(new Ui::DeviceDialog)
{
    ui->setupUi(this);
    if(m_deviceId)
    {
        QSqlQuery sqlQuery;
        QueryExec(this,"SELECT devicetypeid, brand, model, notes FROM devices WHERE id='"+num(m_deviceId)+"';",sqlQuery);
        if(!sqlQuery.next())
        {
            QMetaObject::invokeMethod(this, "reject", Qt::QueuedConnection); // Close dialog.
            return;
        }
        refreshDeviceTypeComboBox(sqlQuery.value(0).toInt());
        ui->brandLineEdit->setText(sqlQuery.value(1).toString());
        ui->modelLineEdit->setText(sqlQuery.value(2).toString());
        ui->notesPlainTextEdit->setPlainText(sqlQuery.value(3).toString());
        setWindowTitle(tr("Редактирование оборудования  %1").arg(m_deviceId));
    }
    else
        refreshDeviceTypeComboBox(0);
}
//-------------------------------------------------------------------------------------
DeviceDialog::~DeviceDialog()
{
    delete ui;
}
//-------------------------------------------------------------------------------------
void DeviceDialog::on_okPushButton_clicked()
{
    QString brand=escapeQuotes(ui->brandLineEdit->text());
    QString model=escapeQuotes(ui->modelLineEdit->text());
    QString notes=escapeQuotes(ui->notesPlainTextEdit->toPlainText());
    if(m_deviceId)
    {
        if(!QueryExec(this,"UPDATE devices SET devicetypeid='"+ui->deviceTypeComboBox->currentData().toString()+"' brand='"+brand+"', model='"+model+"', notes='"+notes+"' WHERE id='"+num(m_deviceId)+"';"))
            return;
    }
    else // new
    {
        QSqlQuery insertQuery;
        if(!QueryExec(this,"INSERT INTO devices(devicetypeid,brand,model,notes) VALUES('"+ui->deviceTypeComboBox->currentData().toString()+"','"+brand+"', '"+model+"', '"+notes+"');",insertQuery))
            return;
        m_deviceId=insertQuery.lastInsertId().toInt();
    }
    accept();
}
//-------------------------------------------------------------------------------------
void DeviceDialog::refreshDeviceTypeComboBox(int defaultId)
{
    ui->deviceTypeComboBox->clear();
    QSqlQuery sqlQuery;
    QueryExec(this,"SELECT id,name FROM devicetypes WHERE enabled>0 OR id='"+num(defaultId)+"';",sqlQuery);
    while(sqlQuery.next())
    {
        ui->deviceTypeComboBox->addItem(sqlQuery.value(1).toString(),sqlQuery.value(0));
    }
    ui->deviceTypeComboBox->setCurrentIndex(ui->deviceTypeComboBox->findData(defaultId));
}
//-------------------------------------------------------------------------------------
