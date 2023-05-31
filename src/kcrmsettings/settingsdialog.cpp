/****************************************************************************
**
** Copyright (C) 2008 Kaufman V.S. All rights reserved.
**
****************************************************************************/

#include "settingsdialog.h"
#include "connectiondialog.h"

extern char aesKey[];

//-------------------------------------------------------------------------------------
SettingsDialog::SettingsDialog(QWidget * parent, Qt::WindowFlags flags)
	: QDialog(parent,flags)
{
	ui.setupUi(this);
    settings.loadCrypted(aesKey,programDataPath()+"/cashop.settings");
    ui.templatesPathLineEdit->setText(settings.value("interface","","templatespath","",""));
    refreshConnectionsTableWidget();
}
//-------------------------------------------------------------------------------------
SettingsDialog::~SettingsDialog()
{
    clearDatabaseConnections();
}
//-------------------------------------------------------------------------------------
void SettingsDialog::on_okButton_clicked()
{
// Save settings to registry.
    settings.setValue("interface","","templatespath","",ui.templatesPathLineEdit->text());
    settings.saveCrypted(aesKey,QApplication::applicationDirPath()+"/settings.xml");
	accept();
}
//-------------------------------------------------------------------------------------
void SettingsDialog::on_cancelButton_clicked()
{
	reject();
}
//-------------------------------------------------------------------------------------
void SettingsDialog::on_newConnectionPushButton_clicked()
{
	connectionDialogExec("");
}
//-------------------------------------------------------------------------------------
void SettingsDialog::on_editConnectionPushButton_clicked()
{
	if(ui.connectionsTableWidget->currentRow()<0)
		return;
	QString connectionName=ui.connectionsTableWidget->item(ui.connectionsTableWidget->currentRow(),0)->text();
	if(connectionName.isEmpty())
		return;
	connectionDialogExec(connectionName);
}
//-------------------------------------------------------------------------------------
void SettingsDialog::connectionDialogExec(QString connectionName)
{
	ConnectionDialog connectionDialog(this);
	if(!connectionName.isEmpty())
	{
		connectionDialog.ui.connectionNameLineEdit->setReadOnly(true);
		connectionDialog.ui.connectionNameLineEdit->setText(connectionName);
		// База данных.
		QString dbtype=settings.value("connection",connectionName,"dbtype","",DBTYPE_SQLITE);
		connectionDialog.ui.sqliteRadioButton->setChecked(true);
		if(dbtype==DBTYPE_MYSQL)
			connectionDialog.ui.mysqlRadioButton->setChecked(true);
        if(dbtype==DBTYPE_POSTGRESQL)
            connectionDialog.ui.postgresqlRadioButton->setChecked(true);
        connectionDialog.ui.dbhostnameLineEdit->setText(settings.value("connection",connectionName,"dbhostname","","localhost"));
		connectionDialog.ui.databasenameLineEdit->setText(settings.value("connection",connectionName,"databasename","","cashopdb"));
		connectionDialog.ui.dbusernameLineEdit->setText(settings.value("connection",connectionName,"dbusername",""));
		connectionDialog.ui.dbpasswordLineEdit->setText(settings.value("connection",connectionName,"dbpassword",""));
	}
	if(connectionDialog.exec()==QDialog::Accepted)
	{
		connectionName=connectionDialog.ui.connectionNameLineEdit->text();
		QString dbtype=DBTYPE_SQLITE;
		if(connectionDialog.ui.mysqlRadioButton->isChecked())
			dbtype=DBTYPE_MYSQL;
        if(connectionDialog.ui.postgresqlRadioButton->isChecked())
            dbtype=DBTYPE_POSTGRESQL;
        settings.setValue("connection",connectionName,"dbtype","", dbtype);
		settings.setValue("connection",connectionName,"dbhostname","", connectionDialog.ui.dbhostnameLineEdit->text());
		settings.setValue("connection",connectionName,"databasename","", connectionDialog.ui.databasenameLineEdit->text());
		settings.setValue("connection",connectionName,"dbusername","", connectionDialog.ui.dbusernameLineEdit->text());
		settings.setValue("connection",connectionName,"dbpassword","", connectionDialog.ui.dbpasswordLineEdit->text());
	}
	refreshConnectionsTableWidget();
}
//-------------------------------------------------------------------------------------
void SettingsDialog::on_deleteConnectionPushButton_clicked()
{
	if(ui.connectionsTableWidget->currentRow()<0)
		return;
	QString connectionName=ui.connectionsTableWidget->item(ui.connectionsTableWidget->currentRow(),0)->text();
	if(connectionName.isEmpty())
		return;
	if (QMessageBox::question(this, PROGNAME, tr("Удалить подключение \"%1\"?").arg(connectionName),QMessageBox::Yes|QMessageBox::No,QMessageBox::No)==QMessageBox::Yes)
		settings.removeGroupname("connection",connectionName);
	refreshConnectionsTableWidget();
}
//-------------------------------------------------------------------------------------
void SettingsDialog::refreshConnectionsTableWidget()
{
	QStringList connectionNames=settings.groupnames("connection");
	ui.connectionsTableWidget->clear();
	ui.connectionsTableWidget->setRowCount(connectionNames.size());
	ui.connectionsTableWidget->setColumnCount(2);
	QStringList headerList;
	headerList<<tr("Подключение")<<tr("Тип");
	ui.connectionsTableWidget->setHorizontalHeaderLabels(headerList);
	for(int i=0;i<connectionNames.size();i++)
	{
		QTableWidgetItem *newItem1 = new QTableWidgetItem(connectionNames.at(i));
		ui.connectionsTableWidget->setItem(i,0,newItem1);
		QTableWidgetItem *newItem2 = new QTableWidgetItem(settings.value("connection",connectionNames.at(i),"dbtype",""));
		ui.connectionsTableWidget->setItem(i,1,newItem2);
	}
}
//-------------------------------------------------------------------------------
void SettingsDialog::on_templatesPathPushButton_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Папка с шаблонами документов"),ui.templatesPathLineEdit->text(),QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(path.isEmpty())
        return;
    ui.templatesPathLineEdit->setText(path);
}
//-------------------------------------------------------------------------------
