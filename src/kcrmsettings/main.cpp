/****************************************************************************
**
** Copyright (C) 2010 Kaufman V.S. All rights reserved.
**
****************************************************************************/
#include <QApplication>
//#include <QTextCodec>

#include "settingsdialog.h"
//#include "../xmlsettings.h"

//extern XMLSettings settings;

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
    SettingsDialog settingsDialog;
	settingsDialog.show();
	return app.exec();
}
