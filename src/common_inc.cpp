/****************************************************************************
**
** Copyright (C) 2014 Kaufman V.S. All rights reserved.
**
****************************************************************************/

#include "common_inc.h"
//#include "../selectconnectiondialog.h"
//#include "aes.h"
#include <QSslSocket>
#include <QMessageBox>


GlobalClass glo;
//QString globalConnectionName;
//QString internalDbtype,internalDbhostname,internalDbusername,internalDbpassword,internalDatabasename;
unsigned int globaluid=0;
//unsigned int connectionTimeCounter;
//QList <QSqlDatabase> dbList;
QMap<QString, int> connectionTimeCounters;
//QList <int> connectionTimeCounterList;
bool debugFlag=false;
char aesKey[]={'\x9B','\x82',0x03,'\xE9',0x54,0x11,'\xBF','\xA5',0x4C,0x62,'\xFE','\x83',0x15,'\x83',0x27,'\xAC'};
//unsigned char aesKey[16]={0x56,0x32,0xF9,0x82,0x90,0x03,0xAC,0x6D,0xB8,0x03,0x6F,0x32,0x8D,0x74,0x14,0x0A};
unsigned char passwordAesKey[16]={0x82,0x0A,0x11,0x07,0xFD,0x78,0xAD,0x9E,0x06,0x02,0x75,0x31,0xB3,0x64,0x25,0x3C};
//-------------------------------------------------------------------------------------

QString appVersion()
{
	return "1.0.4";
}

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
GlobalClass::GlobalClass() :
	optionTimeZone(0),
	optionThumbWidth(300),
	optionThumbHeight(200),
	optionImageWidth(800),
	optionCropImage(false),
	optionJpegQuality(90),
	optionDiscountMarginPercent(0),
	optionAllowOversale(false)
{
}
//-------------------------------------------------------------------------------------
bool GlobalClass::loadDatabaseData(QWidget * parent)
{
/*	if(!loadCurrencies(parent))
	{
		return false;
	}*/
	{ // Если загружать много опций, то так меньше запросов к БД.
		optionTimeZone=0;
		optionOrgname="";
		optionInn="";
		optionOgrn="";
		optionAddress="";
		optionPhone="";
		optionTaxationSystem=32;// Патент
//		optionSmsText="";
		optionThumbWidth=300;
		optionThumbHeight=200;
		optionImageWidth=800;
		optionCropImage=false;
		optionJpegQuality=90;
		optionSmsGateSender="";
		optionSmsGateLogin="";
		optionSmsGatePassword="";
		optionSmsOnSaleText="";
		optionSmtpHost="";
		optionSmtpLogin="";
		optionSmtpPassword="";
		optionDiscountMarginPercent=0;
		optionAllowOversale=false;
		optionAllowPayBank=false;
		optionAllowReadyOrderEdit=false;
		optionAllowGiveoutUnpaid=false;
		optionOutcomeReportPercent=0;
		optionAllowEditClients=false;
		optionPaymentURL="";
		optionKPP="";
		optionBank="";
		optionBIK="";
		optionKSchet="";
		optionRSchet="";
		optionSiteURL="";
		QSqlQuery sqlQuery;
		if(!QueryExec(parent,"SELECT name, value FROM options;",sqlQuery))
		{
			return false;
		}
		while(sqlQuery.next())
		{
			QString optName=sqlQuery.value(0).toString();
			if(optName==OPTNAME_TIMEZONE)
			{
				optionTimeZone=sqlQuery.value(1).toInt();
			}
			else if(optName==OPTNAME_ORGNAME)
			{
				optionOrgname=sqlQuery.value(1).toString();
			}
			else if(optName==OPTNAME_INN)
			{
				optionInn=sqlQuery.value(1).toString();
			}
			else if(optName==OPTNAME_OGRN)
			{
				optionOgrn=sqlQuery.value(1).toString();
			}
			else if(optName==OPTNAME_ADDRESS)
			{
				optionAddress=sqlQuery.value(1).toString();
			}
			else if(optName==OPTNAME_PHONE)
			{
				optionPhone=sqlQuery.value(1).toString();
			}
			else if(optName==OPTNAME_TAXATION_SYSTEM)
			{
				optionTaxationSystem=sqlQuery.value(1).toInt();
			}
/*			else if(optName==OPTNAME_SMSTEXT)
			{
				optionSmsText=sqlQuery.value(1).toString();
			}*/
			else if(optName==OPTNAME_THUMB_WIDTH)
			{
				optionThumbWidth=sqlQuery.value(1).toInt();
			}
			else if(optName==OPTNAME_THUMB_HEIGHT)
			{
				optionThumbHeight=sqlQuery.value(1).toInt();
			}
			else if(optName==OPTNAME_IMAGE_WIDTH)
			{
				optionImageWidth=sqlQuery.value(1).toInt();
			}
			else if(optName==OPTNAME_CROP_IMAGE)
			{
				optionCropImage=sqlQuery.value(1).toBool();
			}
			else if(optName==OPTNAME_JPEG_QUALITY)
			{
				optionJpegQuality=sqlQuery.value(1).toInt();
			}
			else if(optName==OPTNAME_SMS_GATE_SENDER)
			{
				optionSmsGateSender=sqlQuery.value(1).toString();
			}
			else if(optName==OPTNAME_SMS_GATE_LOGIN)
			{
				optionSmsGateLogin=sqlQuery.value(1).toString();
			}
			else if(optName==OPTNAME_SMS_GATE_PASSWORD)
			{
				optionSmsGatePassword=sqlQuery.value(1).toString();
			}
			else if(optName==OPTNAME_SMS_ONSALE_TEXT)
			{
				optionSmsOnSaleText=sqlQuery.value(1).toString();
			}
			else if(optName==OPTNAME_SMTP_HOST)
			{
				optionSmtpHost=sqlQuery.value(1).toString();
			}
			else if(optName==OPTNAME_SMTP_LOGIN)
			{
				optionSmtpLogin=sqlQuery.value(1).toString();
			}
			else if(optName==OPTNAME_SMTP_PASSWORD)
			{
				optionSmtpPassword=sqlQuery.value(1).toString();
			}
			else if(optName==OPTNAME_DISCOUNT_MARGIN_PERCENT)
			{
				optionDiscountMarginPercent=sqlQuery.value(1).toInt();
				if(optionDiscountMarginPercent<0 || optionDiscountMarginPercent>100)
				{
					optionDiscountMarginPercent=0;
				}
			}
			else if(optName==OPTNAME_ALLOW_OVERSALE)
			{
				optionAllowOversale=sqlQuery.value(1).toBool();
			}
			else if(optName==OPTNAME_ALLOW_PAY_BANK)
			{
				optionAllowPayBank=sqlQuery.value(1).toBool();
			}
			else if(optName==OPTNAME_ALLOW_READY_ORDER_EDIT)
			{
				optionAllowReadyOrderEdit=sqlQuery.value(1).toBool();
			}
			else if(optName==OPTNAME_ALLOW_GIVEOUT_UNPAID)
			{
				optionAllowGiveoutUnpaid=sqlQuery.value(1).toBool();
			}
			else if(optName==OPTNAME_OTCOME_REPORT_PERCENT)
			{
				optionOutcomeReportPercent=sqlQuery.value(1).toDouble();
			}
			else if(optName==OPTNAME_ALLOW_EDIT_CLIENTS)
			{
				optionAllowEditClients=sqlQuery.value(1).toBool();
			}
			else if(optName==OPTNAME_PAYMENT_URL)
			{
				optionPaymentURL=sqlQuery.value(1).toString();
			}
			else if(optName==OPTNAME_KPP)
			{
				optionKPP=sqlQuery.value(1).toString();
			}
			else if(optName==OPTNAME_BANK)
			{
				optionBank=sqlQuery.value(1).toString();
			}
			else if(optName==OPTNAME_BIK)
			{
				optionBIK=sqlQuery.value(1).toString();
			}
			else if(optName==OPTNAME_KSCHET)
			{
				optionKSchet=sqlQuery.value(1).toString();
			}
			else if(optName==OPTNAME_RSCHET)
			{
				optionRSchet=sqlQuery.value(1).toString();
			}
			else if(optName==OPTNAME_SITE_URL)
			{
				optionSiteURL=sqlQuery.value(1).toString();
			}
		}
//		orderStates.loadFromDB();
	}
	return true;
}
//-------------------------------------------------------------------------------------
bool addDatabase(QString connectionName, const XMLSettings &s)
{
	return addDatabase(connectionName,s.value("connection",connectionName,"dbtype",""),s.value("connection",connectionName,"dbhostname",""),s.value("connection",connectionName,"dbusername",""),s.value("connection",connectionName,"dbpassword",""),s.value("connection",connectionName,"databasename",""),s.value("connection",connectionName,"dbcacert",""),s.value("connection",connectionName,"dbclientkey",""),s.value("connection",connectionName,"dbclientcert",""));
}
//-------------------------------------------------------------------------------------
bool addDatabase(QString connectionName,QString dbtype,QString dbhostname, QString username, QString password,QString databasename,QString serverCaCertFileName,QString clientKeyFileName,QString clientCertFileName)
{
	QSqlDatabase db;
	if(dbtype==DBTYPE_MYSQL)
	{
		db = QSqlDatabase::addDatabase("QMYSQL",connectionName);
		QString connectOptions;
		if(!serverCaCertFileName.isEmpty())
		{
			connectOptions.isEmpty()?0:connectOptions+=";"; // Разделяем опции
			connectOptions+="SSL_CA="+serverCaCertFileName;
		}
		if(!clientKeyFileName.isEmpty())
		{
			connectOptions.isEmpty()?0:connectOptions+=";";
			connectOptions+="SSL_KEY="+clientKeyFileName;
		}
		if(!clientCertFileName.isEmpty())
		{
			connectOptions.isEmpty()?0:connectOptions+=";";
			connectOptions+="SSL_CERT="+clientCertFileName;
		}
		if(!connectOptions.isEmpty())
		{
			qDebug() << "MySQL connect options: " << connectOptions;
			db.setConnectOptions(connectOptions);
		}
//        db.setConnectOptions("CLIENT_SSL=1;CLIENT_IGNORE_SPACE=1");// use an SSL connection to the server
		QStringList host=dbhostname.split(':');
		db.setHostName(host.at(0));
		if(host.size()>1)
			db.setPort(host.at(1).toInt());
		db.setUserName(username);
		db.setPassword(password);
	}
	else if(dbtype==DBTYPE_POSTGRESQL)
	{
		db = QSqlDatabase::addDatabase("QPSQL",connectionName);
		QStringList host=dbhostname.split(':');
		db.setHostName(host.at(0));
		if(host.size()>1)
			db.setPort(host.at(1).toInt());
		db.setUserName(username);
		db.setPassword(password);
	}
	else if(dbtype==DBTYPE_SQLITE)
	{
		db = QSqlDatabase::addDatabase("QSQLITE",connectionName);
		if(!databasename.contains('/') && !databasename.contains('\\'))
		{
			databasename.prepend(userDataPath()+"/");
		}
	}
	db.setDatabaseName(databasename);
	connectionTimeCounters[glo.connectionName]=0;
/*    if(!openDatabase(connectionName))
		return false;*/
	if(!db.isValid())
	{
#ifdef QT_WIDGETS_LIB
		QMessageBox::warning(nullptr,PROGNAME,QObject::tr("Ошибка создания соединения: %1").arg(db.lastError().text()));
#endif
		debug("addDatabase: db not valid. "+db.lastError().text());
		return false;
	}
	return true;
}
//-------------------------------------------------------------------------------------
void clearDatabaseConnections()
{
	connectionTimeCounters.clear();
	QStringList dbConNames=QSqlDatabase::connectionNames();
	for(int i=0;i<dbConNames.size();i++)
	{
		QSqlDatabase::removeDatabase(dbConNames.at(i));
	}
	glo.connectionName.clear();
//	glo.friendConnectionName.clear();
//	glo.user=User();
}
//-------------------------------------------------------------------------------------
void connectionTimer()
{
	int connectionTime=55; // Keep connection opened 'connectionTime' seconds.
	QStringList dbConNames=QSqlDatabase::connectionNames();
	for(int i=0;i<dbConNames.size();i++)
	{
		if(connectionTimeCounters[dbConNames.at(i)]<connectionTime)
		{
			connectionTimeCounters[dbConNames.at(i)]++;
			if(connectionTimeCounters[dbConNames.at(i)]==connectionTime)
			{
				QSqlDatabase::database(dbConNames.at(i)).close();
				debug("Close connection "+dbConNames.at(i));
			}
		}
	}
}
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
bool QueryExec(const QString &queryString, const QString connectionName)
{
	return QueryExec(nullptr, queryString, connectionName);
}
//-------------------------------------------------------------------------------------
bool QueryExec(const QString &queryString, QSqlQuery &sqlQuery, const QString connectionName)
{
	return QueryExec(nullptr, queryString, sqlQuery, connectionName);
}
//-------------------------------------------------------------------------------------
bool QueryExec(QWidget * parent, const QString &queryString, const QString connectionName)
{
	QSqlQuery sqlQuery;
	return QueryExec(parent,queryString,sqlQuery,connectionName);
}
//-------------------------------------------------------------------------------------
bool QueryExec(QWidget * parent, const QString &queryString, QSqlQuery &sqlQuery, const QString connectionName)
{
	bool reconnectOnError=true;
	while(1)
	{
		QSqlDatabase db=QSqlDatabase::database(connectionName);
		if (!db.isOpen())
		{
//            debug("db error: "+QString::number(db.lastError().number())+" "+db.lastError().text());
#ifdef QT_WIDGETS_LIB
			if(parent && QMessageBox::question(parent, PROGNAME, QObject::tr("Ошибка открытия базы данных: %1").arg(db.lastError().text()), QObject::tr("Повторить"), QObject::tr("Отмена"))==0)
				continue;
#endif
			return false;
		}
		connectionTimeCounters[connectionName]=0; // Обнуляем счётчик подключения.
		sqlQuery = QSqlQuery(db);
		debug("QueryExec: "+queryString);
		if(sqlQuery.exec(queryString))
			return true;
		// Если потеря связи с базой данных - переоткрываем её.
		// В MySQL 2006 - это "Server has gone away". Почему-то не относится к типу ConnectionError
		if(sqlQuery.lastError().type() == QSqlError::ConnectionError)
		{
			db.close();
			continue;
		}
		if (sqlQuery.lastError().type() != QSqlError::NoError) // Для остальных ошибок.
		{
			if(reconnectOnError) // Пробуем переподключиться.
			{
				db.close();
				reconnectOnError=false; // Второй раз переподключаться уже не будет.
				continue;
			}
//            debug("QueryExec error: "+QString::number(sqlQuery.lastError().number())+" "+sqlQuery.lastError().text());
			QString errorStr="Ошибка SQL: "+QString::number(sqlQuery.lastError().type())+" "+sqlQuery.lastError().text();
			logString(parent,errorStr,((parent)?true:false));
			logString(parent,sqlQuery.lastQuery(),false);
			qWarning()<<errorStr; // Если подключен syslog, то запишет в системный лог.
			return false;
		}
//        debug("QueryExec ok.");
		return true;
	}
//    return false;
}
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
bool QueryExecPrepared(const QString &queryString, const QueryParameters &qParam, QString connectionName)
{
	return QueryExecPrepared(nullptr,queryString,qParam,connectionName);
}
//-------------------------------------------------------------------------------------
bool QueryExecPrepared(const QString &queryString, const QueryParameters &qParam, QSqlQuery &sqlQuery, QString connectionName)
{
	return QueryExecPrepared(nullptr,queryString,qParam, sqlQuery, connectionName);
}
//-------------------------------------------------------------------------------------
bool QueryExecPrepared(QWidget * parent, const QString &queryString, const QueryParameters &qParam, QString connectionName)
{
	QSqlQuery sqlQuery;
	return QueryExecPrepared(parent,queryString,qParam,sqlQuery,connectionName);
}
//-------------------------------------------------------------------------------------
bool QueryExecPrepared(QWidget * parent, const QString &queryString, const QueryParameters &qParam, QSqlQuery &sqlQuery, QString connectionName)
{
	bool reconnectOnError=true;
	while(1)
	{
		QSqlDatabase db=QSqlDatabase::database(connectionName);
		if (!db.isOpen())
		{
//            debug("db error: "+QString::number(db.lastError().number())+" "+db.lastError().text());
#ifdef QT_WIDGETS_LIB
			if(parent && QMessageBox::question(parent, PROGNAME, QObject::tr("Ошибка открытия базы данных: %1").arg(db.lastError().text()), QObject::tr("Повторить"), QObject::tr("Отмена"))==0)
				continue;
#endif
			return false;
		}
		connectionTimeCounters[connectionName]=0; // Обнуляем счётчик подключения.
		sqlQuery = QSqlQuery(db);
		sqlQuery.prepare(queryString);
		for(int i=0;i<qParam.size();i++)
		{
			sqlQuery.bindValue(qParam.name(i),qParam.value(i));
		}
		debug("QueryExecPrepared: "+queryString);
		if(sqlQuery.exec())
		{
//            qDebug()<<sqlQuery.lastQuery();
			return true;
		}
		// Если потеря связи с базой данных - переоткрываем её.
		// В MySQL 2006 - это "Server has gone away". Почему-то не относится к типу ConnectionError
		if(sqlQuery.lastError().type() == QSqlError::ConnectionError)
		{
			db.close();
			continue;
		}
		if (sqlQuery.lastError().type() != QSqlError::NoError) // Для остальных ошибок.
		{
			if(reconnectOnError) // Пробуем переподключиться.
			{
				db.close();
				reconnectOnError=false; // Второй раз переподключаться уже не будет.
				continue;
			}
			QString errorStr="Ошибка SQL: "+QString::number(sqlQuery.lastError().type())+" "+sqlQuery.lastError().text();
			logString(parent,errorStr,((parent)?true:false));
			logString(parent,sqlQuery.lastQuery(),false);
			qWarning()<<errorStr; // Если подключен syslog, то запишет в системный лог.
			return false;
		}
//        debug("QueryExec ok.");
		return true;
	}
//    return false;
}
//-------------------------------------------------------------------------------------
bool QueryModelExec(QWidget * parent,QString queryString,QSqlQueryModel *sqlQueryModel, QString connectionName)
{
	while(1)
	{
		QSqlDatabase db=QSqlDatabase::database(connectionName);
		if (!db.isOpen())
		{
//            debug("db error: "+QString::number(db.lastError().number())+" "+db.lastError().text());
#ifdef QT_WIDGETS_LIB
			if(QMessageBox::question(parent, PROGNAME, QObject::tr("Ошибка открытия базы данных: %1").arg(db.lastError().text()), QObject::tr("Повторить"), QObject::tr("Отмена"))==0)
				continue;
#endif
			return false;
		}
		connectionTimeCounters[connectionName]=0; // Обнуляем счётчик подключения.
		//qDebug()<<"QueryModelExec: "+queryString;
		sqlQueryModel->setQuery(queryString,db);
		if(sqlQueryModel->lastError().type() == QSqlError::ConnectionError)
		{
			db.close();
			continue;
		}
		if (sqlQueryModel->lastError().type() != QSqlError::NoError)
		{
			logString(parent,"Ошибка SQL: "+QString::number(sqlQueryModel->lastError().type())+" "+sqlQueryModel->lastError().text(),true);
			logString(parent,sqlQueryModel->query().lastQuery(),false);
			return false;
		}
		connectionTimeCounters[connectionName]=0; // Обнуляем счётчик подключения.
		while(sqlQueryModel->canFetchMore())// Fetch the entire query.
		{
			sqlQueryModel->fetchMore();
		}
//        debug("QueryModelExec ok.");
		return true;
	}
//    return false;
}
//-------------------------------------------------------------------------------------
bool transaction(QString connectionName)
{
	QSqlDatabase db=QSqlDatabase::database(connectionName);
	if(!db.transaction())
	{
		if (!db.driver()->hasFeature(QSqlDriver::Transactions))
		{
			qDebug("Database driver does not support transactions.");
		}
		qDebug("Transaction initiate failed.");
		return false;
	}
	return true;
}
//-------------------------------------------------------------------------------------
bool commit(QString connectionName)
{
	QSqlDatabase db=QSqlDatabase::database(connectionName);
	if(!db.commit())
	{
		qDebug("Transaction commit failed.");
		return false;
	}
	return true;
}
//-------------------------------------------------------------------------------------
bool rollback(QString connectionName)
{
	QSqlDatabase db=QSqlDatabase::database(connectionName);
	if(!db.rollback())
	{
		qDebug("Transaction rollback failed.");
		return false;
	}
	return true;
}
//-------------------------------------------------------------------------------------
double calculateOrderQuantity(double autoorder, double onceorder, double rest, double step)
{
//    QMessageBox::information(0,"",QObject::tr("autoorder=%1 onceorder=%2 rest=%3 step=%4").arg(autoorder).arg(onceorder).arg(rest).arg(step));
    double neworderquantity=autoorder-rest;
    if(neworderquantity<0)
        neworderquantity=0;
    neworderquantity+=onceorder;
    if(step>0)
    {
        double steps=neworderquantity/step;// Получаем количество упаковок.
        if(steps-int(steps))// Если кол-во упаковок не целое, округляем в большую сторону.
            steps=int(steps)+1;
        neworderquantity=step*steps;
//        QMessageBox::information(0,"",QObject::tr("neworderquantity=%1 steps=%2").arg(neworderquantity).arg(steps));
    }
    return neworderquantity;
}
//-------------------------------------------------------------------------------------

/*int BWare::InsertToWarehouse(QWidget * parent) // return nonzero wareid if success, or zero if fails.
{
	if(!QueryExec(parent,"INSERT INTO warehouse(enabled,section,groupid,name,alias,step,orderquantity,supplierid,incost,outcost,quantity,correction,latime,timestamp) VALUES ('"+QString::number(Enabled())+"','"+QString::number(Section())+"','"+QString::number(Groupid())+"','"+Name()+"','"+Alias()+"','"+QString::number(Step()).replace(',','.')+"','"+QString::number(Orderquantity()).replace(',','.')+"','"+QString::number(Supplierid())+"','"+QString::number(Incost()).replace(',','.')+"','"+QString::number(Outcost()).replace(',','.')+"','"+QString::number(Quantity()).replace(',','.')+"','"+QString::number(Correction()).replace(',','.')+"','"+Latime()+"','"+Timestamp()+"');"))
		return 0;
	QSqlQuery MaxWareIdQuery;
	if(!QueryExec(parent,"SELECT MAX(id) FROM warehouse;",MaxWareIdQuery))
		return 0;
	if(!MaxWareIdQuery.next())
		return 0;
	int maxid=MaxWareIdQuery.value(0).toInt();
	MaxWareIdQuery.clear();
	return maxid;
}*/
//-------------------------------------------------------------------------------------
/*bool BWare::SelectFromWarehouse(QWidget * parent,int wareid) // return true if success.
{
	QSqlQuery sqlquery("SELECT enabled, section, groupid, name, alias, step, orderquantity, supplierid, incost, outcost, quantity, correction, latime, timestamp FROM warehouse WHERE id='"+QString::number(wareid)+"'");
	if (sqlquery.lastError().type() != QSqlError::NoError)
	{
		logString(parent,sqlquery.lastError().text());
		return false;
	}
	if (!sqlquery.next())
	{
		logString(parent,"Error 41911782: Wareid "+QString::number(wareid)+" not exists.");
		return false;
	}
	setId(wareid);
	setEnabled(sqlquery.value(0).toInt());
	setSection(sqlquery.value(1).toInt());
	setGroupid(sqlquery.value(2).toInt());
	setName(sqlquery.value(3).toString());
	setAlias(sqlquery.value(4).toString());
	setStep(sqlquery.value(5).toDouble());
	setOrderquantity(sqlquery.value(6).toDouble());
	setSupplierid(sqlquery.value(7).toInt());
	setIncost(sqlquery.value(8).toDouble());
	setOutcost(sqlquery.value(9).toDouble());
	setQuantity(sqlquery.value(10).toDouble());
	setCorrection(sqlquery.value(11).toDouble());
	setLatime(sqlquery.value(12).toString());
	setTimestamp(sqlquery.value(13).toString());
	return true;
}*/
//-------------------------------------------------------------------------------------
QStringList GetSelectedList(QString field,QSqlQueryModel *sqlQueryModel, QTableView *tableView)
{
	QStringList idlist;
	QModelIndexList selected=tableView->selectionModel()->selectedRows();
	for(int i=0;i<selected.size();i++)
        idlist.append(sqlQueryModel->record(selected.at(i).row()).field(field).value().toString());
	return idlist;
}
//-------------------------------------------------------------------------------------
qint64 GetUID(QWidget * parent,QString Password)
{
	QSqlQuery passQuery;
    if(!QueryExec(parent,"SELECT id FROM users WHERE enabled='1' AND password='"+encryptPassword(Password)+"';",passQuery))
        return -1;
	if(!passQuery.next())
		return 0;
    return passQuery.value(0).toLongLong();
}
//-------------------------------------------------------------------------------------
QString getUserName(QWidget * parent,quint64 id)
{
    QSqlQuery userQuery;
    QueryExec(parent,"SELECT name FROM users WHERE id='"+num(id)+"';",userQuery);
    userQuery.next();
    return userQuery.value(0).toString();
}
//-------------------------------------------------------------------------------------
QString getOption(QWidget * parent,QString name)
{
	QSqlQuery sqlQuery;
	QueryExec(parent,"SELECT value FROM options WHERE name='"+name+"';",sqlQuery);
	sqlQuery.next();
	return sqlQuery.value(0).toString();
}
//-------------------------------------------------------------------------------------
QString escapeQuotes(QString string) // Single quote turned into a pair of single quotes.
{
	string.replace(QString("'"),QString("''"));
	return string;
}
//-------------------------------------------------------------------------------------
QString escapeCSV(QString string)
{
    string.replace(QString("\""),QString("\"\""));
    string.prepend("\"");
    string.append("\"");
    return string;
}
//-------------------------------------------------------------------------------------
QString escapeJson(QString data)
{
    QString encoded;
    for(int i = 0; i < data.length(); ++i)
    {
        QChar ch = data.at(i);
        ushort uch=ch.unicode();
        if(uch=='"')
            encoded.append("\\\"");
        else if(uch=='\\')
            encoded.append("\\\\");
        else if(uch=='/')
            encoded.append("\\/");
        else// if(uch >= 32 && uch < 128) // printable ASCII character?
            encoded.append(ch);
/*        else
        {
            switch(uch)
            {
            case(8):
                encoded.append("\\b");
                break;
            case(9):
                encoded.append("\\t");
                break;
            case(10):
                encoded.append("\\n");
                break;
            case(12):
                encoded.append("\\f");
                break;
            case(13):
                encoded.append("\\r");
                break;
            default:
                encoded.append("\\\\u" + QString::number(uch, 16)
                               .rightJustified(4, '0'));
            }
        }*/
    }
    return encoded;
}
//-------------------------------------------------------------------------------------
QString EncodeToXML(QString string)
{
	string.replace(QString("&"),QString("&amp;"));
	string.replace(QString("'"),QString("&apos;"));
	string.replace(QString("\""),QString("&quot;"));
	string.replace(QString("<"),QString("&lt;"));
	string.replace(QString(">"),QString("&gt;"));
	return string;
}
//-------------------------------------------------------------------------------------
QString DecodeFromXML(QString string)
{
	string.replace(QString("&apos;"),QString("'"));
	string.replace(QString("&quot;"),QString("\""));
	string.replace(QString("&lt;"),QString("<"));
	string.replace(QString("&gt;"),QString(">"));
	string.replace(QString("&amp;"),QString("&"));
	return string;
}
//-------------------------------------------------------------------------------------
QString timestamp()
{
    return QDateTime::currentDateTime().toString(DATETIME_FORMAT);
}
//-------------------------------------------------------------------------------------
QString programDataPath()
{
#ifdef Q_OS_LINUX
    return QDir::homePath()+"/.cashop";
#else
    return QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)+"/Cashop";
#endif
}
//-------------------------------------------------------------------------------------
QString userDataPath()
{
#ifdef Q_OS_LINUX
    return QDir::homePath()+"/.cashop";
#else
    return QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)+"/Cashop";
#endif
}
//-------------------------------------------------------------------------------------
void logString(QWidget *parent, QString string, bool showMessageBox)
{
	if(showMessageBox)
		QMessageBox::warning(parent,PROGNAME,string);
	QFile logFile(programDataPath()+"/"+QString(PROGNAME)+"_log.txt");
	if(!logFile.open(QIODevice::WriteOnly|QIODevice::Append))
		return;
	logFile.write(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss ").toLocal8Bit()+string.toLocal8Bit()+"\r\n");
	logFile.close();
}
//-------------------------------------------------------------------------------------
QString num(int num)
{
	return QString::number(num);
}
//-------------------------------------------------------------------------------------
QString num(unsigned int num)
{
	return QString::number(num);
}
//-------------------------------------------------------------------------------------
QString num(quint64 num)
{
    return QString::number(num);
}
//-------------------------------------------------------------------------------------
QString num(qint64 num)
{
    return QString::number(num);
}
//-------------------------------------------------------------------------------------
QString num(double num)
{
	return QString::number(num).replace(',','.');
}
//-------------------------------------------------------------------------------------
double roundValute(double value)
{
	return double(round((value)*100))/100;
}
//-------------------------------------------------------------------------------------
QByteArray encryptPassword(QString password)
{
//    Aes aes(128);
//    QMessageBox::information(0,PROGNAME,QObject::tr("password %1").arg(password));
//    QMessageBox::information(0,PROGNAME,QObject::tr("crypted password ")+aes.crypt(passwordAesKey,password.toUtf8()));
//    QMessageBox::information(0,PROGNAME,QObject::tr("crypted base64 password ")+aes.crypt(passwordAesKey,password.toUtf8()).toBase64());
//    return aes.crypt(passwordAesKey,password.toUtf8()).toBase64();
    return password.toUtf8().toBase64();
}
//-------------------------------------------------------------------------------------
QString decryptPassword(QByteArray data)
{
    QByteArray rawData=QByteArray::fromBase64(data); 
//    Aes aes(128);
//    return aes.decrypt(passwordAesKey,rawData);
    return QString::fromUtf8(rawData);
}
//-------------------------------------------------------------------------------------
unsigned int encodeCollected(int d)
{
    return d^0xE3649BDC;
}
//-------------------------------------------------------------------------------------
bool checkRights(QWidget * parent,unsigned int userid,unsigned int rights) // Возвращает true, если все запрошенные права у пользователя имеются.
{
    QSqlQuery rightsQuery;
    if(!QueryExec(parent,"SELECT rights FROM users WHERE id='"+QString::number(userid)+"';",rightsQuery))
        return false;
    if(!rightsQuery.next())
        return false;
    if(rights==(rightsQuery.value(0).toUInt()&rights))
        return true;
    return false;
}
//-------------------------------------------------------------------------------------
void debug(QString debugStr)
{
	if(debugFlag)
	{
		QString debugLine=QString(QTime::currentTime().toString("hh:mm:ss.zzz")+" "+debugStr);
		qDebug() << debugLine;
	}
}
//-------------------------------------------------------------------------------------
void setDebug(bool debugFl)
{
	debugFlag=debugFl;
}
