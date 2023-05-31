#include "directiondialog.h"
#include "ui_directiondialog.h"

DirectionDialog::DirectionDialog(int directionId, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DirectionDialog),
	m_directionId(directionId)
{
	ui->setupUi(this);
	if(m_directionId)
	{
		setWindowTitle(tr("Редактирование направления %1").arg(m_directionId));
		QSqlQuery directionSqlQuery;
		if(!QueryExec(this,"SELECT enabled, name FROM directions WHERE id='"+num(m_directionId)+"';",directionSqlQuery) || !directionSqlQuery.next())
		{
			QMetaObject::invokeMethod(this, "reject", Qt::QueuedConnection); // Close dialog.
			return;
		}
		ui->enabledCheckBox->setChecked(directionSqlQuery.value(0).toBool());
		ui->nameLineEdit->setText(directionSqlQuery.value(1).toString());

	}
	else
	{
		ui->enabledCheckBox->setChecked(true);
	}
}
//-------------------------------------------------------------------------------------
DirectionDialog::~DirectionDialog()
{
	delete ui;
}
//-------------------------------------------------------------------------------------
void DirectionDialog::on_savePushButton_clicked()
{
	QString enabled=num(ui->enabledCheckBox->isChecked());
	QString name=escapeQuotes(ui->nameLineEdit->text());
	if(m_directionId)
	{
		if(!QueryExec(this,"UPDATE directions SET enabled='"+enabled+"', name='"+name+"' WHERE id='"+num(m_directionId)+"';"))
			return;
	}
	else // new
	{
		QSqlQuery insertQuery;
		if(!QueryExec(this,"INSERT INTO directions(enabled,name) VALUES('"+enabled+"', '"+name+"');",insertQuery))
			return;
		m_directionId=insertQuery.lastInsertId().toInt();

	}
	accept();
}
//-------------------------------------------------------------------------------------
