#include "selectconnectiondialog.h"
#include "ui_selectconnectiondialog.h"
#include "common_inc.h"
#include <QMessageBox>

extern XMLSettings settings;
extern unsigned int globaluid;
//extern QString globalConnectionName;
extern QList <QSqlDatabase> dbList;

//-------------------------------------------------------------------------------------
SelectConnectionDialog::SelectConnectionDialog(unsigned int rights, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectConnectionDialog)
{
    ui->setupUi(this);
    m_rights=rights;
    m_connectionNameList=settings.groupnames("connection");
    if(!m_connectionNameList.size())
    {
        QMessageBox::warning(NULL,PROGNAME,QObject::tr("Ошибка 18529: пустой список подключений в файле настроек. Запустите программу настроек и создайте подключение."));
        QMetaObject::invokeMethod(this, "reject", Qt::QueuedConnection); // Close dialog.
        return;
    }
    for(int i=0;i<m_connectionNameList.size();i++)
	{
        ui->connectionComboBox->addItem(m_connectionNameList.at(i));
	}
	m_selectedIndex=0;
//    ui->connectionComboBox->setFocus();
}
//-------------------------------------------------------------------------------------
SelectConnectionDialog::~SelectConnectionDialog()
{
    delete ui;
}
//-------------------------------------------------------------------------------------
void SelectConnectionDialog::on_okPushButton_clicked()
{
    if(ui->connectionComboBox->currentIndex()<0)
    {
        QMessageBox::warning(this, PROGNAME, tr("Не выбрано подключение"));
        return;
    }
    if(ui->passLineEdit->text().isEmpty())
	{
        return;
	}
    m_selectedIndex=ui->connectionComboBox->currentIndex();
    if(m_selectedIndex<0)
	{
        return;
	}
//    globalConnectionName=m_connectionNameList.at(m_selectedIndex);
    // Clear database connections.
    clearDatabaseConnections();
	glo.connectionName=m_connectionNameList.at(m_selectedIndex);
	if(!addDatabase(glo.connectionName,settings))
	{
		clearDatabaseConnections();
		return;
	}
    m_uid=GetUID(this,ui->passLineEdit->text());
//    QMessageBox::warning(this, PROGNAME, tr("m_uid=%1").arg(m_uid));
	if(m_uid<=0)
    {
        QMessageBox::warning(this, tr("Ошибка ввода пароля"), tr("Неверный пароль!"));
        ui->passLineEdit->clear();
        ui->passLineEdit->setFocus();
		return;
    }
	globaluid=m_uid;
	glo.loadDatabaseData(this);
	accept();
}
//-------------------------------------------------------------------------------------
