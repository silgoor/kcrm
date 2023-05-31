/****************************************************************************
**
** Copyright (C) 2008 Kaufman V.S. All rights reserved.
**
****************************************************************************/

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QtGui/QtGui>
#include <QtSql/QtSql>

#include "../xmlsettings.h"
#include "../common_inc.h"

#include "ui_settingsdialog.h"

class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
	SettingsDialog(QWidget * parent = 0, Qt::WindowFlags flags = 0); // Constructor.
    ~SettingsDialog();
// Members.
public:
	Ui::SettingsDialog ui;
	XMLSettings settings;
private:
// Methods.
public:
private:
//	bool createTables();
	void refreshConnectionsTableWidget();
	void connectionDialogExec(QString connectionName);
private slots:
	void on_okButton_clicked();
	void on_cancelButton_clicked();
	void on_newConnectionPushButton_clicked();
	void on_editConnectionPushButton_clicked();
	void on_deleteConnectionPushButton_clicked();
    void on_templatesPathPushButton_clicked();
};

#endif
