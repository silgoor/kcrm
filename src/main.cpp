#include "mainwindow.h"
#include <QApplication>
#include "xmlsettings.h"
#include "common_inc.h"

extern char aesKey[];

XMLSettings settings;

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	if(app.arguments().size()>1)// Если есть параметры командной строки.
	{
		if(app.arguments().at(1)=="-debug")
		{
			setDebug(true);
#ifdef Q_OS_WIN
			if(AttachConsole(ATTACH_PARENT_PROCESS) || AllocConsole())
			{
				freopen("CONOUT$","w",stdout);
				freopen("CONOUT$","w",stderr);
			}
#endif
		}
	}

/*    QFile file(QApplication::applicationDirPath()+"/settings.xml");
    if(!file.exists())
        QMessageBox::information(0,PROGNAME,QObject::tr("Не найден файл настроек: ")+programDataPath()+"/settings.xml");
    if(!file.open(QIODevice::ReadOnly))
        QMessageBox::information(0,PROGNAME,QObject::tr("Ошибка открытия файла настроек."));
    if(!settings.loadFromData(file.readAll()))
        QMessageBox::information(0,PROGNAME,QObject::tr("Ошибка чтения файла настроек."));*/

    settings.loadCrypted(aesKey,programDataPath()+"/cashop.settings");

    MainWindow w;
    w.show();

	return app.exec();
}
