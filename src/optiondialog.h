/****************************************************************************
**
** Copyright (C) 2010 Kaufman V.S. All rights reserved.
**
****************************************************************************/

#ifndef OPTIONDIALOG_H
#define OPTIONDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <QtSql>

#include "ui_optiondialog.h"

class OptionDialog: public QDialog
{
	Q_OBJECT
public:
    OptionDialog(int optionid, QWidget *parent = 0);
// Members.
public:
private:
	Ui::OptionDialog ui;
    int m_optionid;
// Methods.
public:
private:
protected:
private slots:
	void on_savePushButton_clicked();
	void on_cancelPushButton_clicked();
};

#endif
