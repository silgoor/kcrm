/****************************************************************************
**
** Copyright (C) 2010 Kaufman V.S. All rights reserved.
**
****************************************************************************/

#include "common_inc.h"
#include "optiondialog.h"

//-------------------------------------------------------------------------------------
OptionDialog::OptionDialog(int optionid,QWidget *parent)
    : QDialog(parent)
{
	m_optionid=optionid;
	ui.setupUi(this);
	if(m_optionid)
	{
		QSqlQuery query;
		QueryExec(this,"SELECT localname, description, value FROM options WHERE id='"+QString::number(m_optionid)+"';",query);
		query.next();
		ui.localNameLineEdit->setText(query.value(0).toString());
		ui.descriptionLabel->setText(query.value(1).toString());
        ui.valuePlainTextEdit->setPlainText(query.value(2).toString());
	}
}
//-------------------------------------------------------------------------------------
void OptionDialog::on_savePushButton_clicked()
{
//	QString name=escapeQuotes(ui.localNameLineEdit->text()).trimmed();
	// Check void name.
/*	if (name.isEmpty())
	{
		QMessageBox::information(this,tr(PROGNAME),tr("Пожалуйста, введите название опции."));
		ui.nameLineEdit->setFocus();
		return;
	}*/
    QString value=escapeQuotes(ui.valuePlainTextEdit->toPlainText()).trimmed();
/*	if(!m_optionid)// New.
	{
		// Check for existance.
		// Insert.
		QueryExec(this,"INSERT INTO options(name,value,timestamp) VALUES('"+name+"','"+value+"','"+timestamp()+"');");
	}
	else*/
	{
		// Update.
		QueryExec(this,"UPDATE options SET value='"+value+"', timestamp='"+timestamp()+"' WHERE id='"+QString::number(m_optionid)+"';");
	}
	accept();
}
//-------------------------------------------------------------------------------------
void OptionDialog::on_cancelPushButton_clicked()
{
	reject();
}
//-------------------------------------------------------------------------------------
