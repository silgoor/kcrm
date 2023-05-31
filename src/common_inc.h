/****************************************************************************
**
** Copyright (C) 2010 Kaufman V.S. All rights reserved.
**
****************************************************************************/

#ifndef COMMON_INC_H
#define COMMON_INC_H

#define ORGNAME "Meudeus"
#define PROGNAME "Kcrm"

#define DATETIME_FORMAT "yyyy-MM-dd hh:mm:ss"

//#define SOUND_ADDSUCCESS "sounds/addsuccess.wav"
//#define SOUND_WRONGCODE "sounds/wrongcode.wav"

#include <QtGui>
#include <QtSql>
#include <QTableView>
#include "xmlsettings.h"

#define CSRIGHT_EXEC_CART           1
#define CSRIGHT_EXEC_INCOME         2
#define CSRIGHT_EXEC_ADMIN          4
#define CSRIGHT_EXEC_AUDIT          8
#define CSRIGHT_EXEC_ORDER         16
#define CSRIGHT_WEBADMIN           32
#define CSRIGHT_PERFORMER          64 // помещать в список исполнителей заказа
#define CSRIGHT_EXEC_XMLAPI        128
#define CSRIGHT_RETURNSALE         256
#define CSRIGHT_PAY_CLOSE          512 // разрешено работать с деньгами, выдавать и отклонять заказы.

#define DBTYPE_SQLITE "sqlite"
#define DBTYPE_MYSQL "mysql"
#define DBTYPE_POSTGRESQL "postgresql"

// Состояния заказа.
#define STATUS_WAITFORACCEPT 100 // Ожидает подтверждения.
#define STATUS_INWORK 200 // В работе.
#define STATUS_WAITFORSUPPLY 300 // Ожидает поставки.
#define STATUS_READY 400 // Готов.
// Закрытый заказ.
#define STATUS_GIVENOUT 500 // Выдан (от 500 до 599 - считается выдан).
#define STATUS_GIVENOUT_COLLECTED 550 // Выдан и инкассирован.
#define STATUS_REJECTED 600 // Отклонен. (600 и больше - считается отклонён).

#define IMAGES_STORE_LOCAL 0
#define IMAGES_STORE_WEBDAV 1
#define IMAGES_STORE_FTP 2

// Индексы колонок импортированной накладной.
#define IMP_ARTICLE_INDEX 0
#define IMP_NAME_INDEX 1
#define IMP_QUANTITY_INDEX 2
#define IMP_COST_INDEX 3
#define IMP_CODE_INDEX 4
#define IMP_TAXRATE_INDEX 5

// Идентификаторы событий в таблице actions.
#define ACTIONID_LOGIN_KCRM 1
#define ACTIONID_LOGOUT_KCRM 2
#define ACTIONID_LOGIN_CART 3 // Магазин
#define ACTIONID_LOGOUT_CART 4

#define CHAT_TYPE_AGREEMENT 1

#define CALL_RESULT_MAINTENANCE_SCHEDULED 1 // Договорились о ТО
#define CALL_RESULT_STULL 2 // Думает
#define CALL_RESULT_REJECTED 3 // Отказался
#define CALL_RESULT_DONT_CALL 4 // Не звонить
#define CALL_RESULT_DOESNT_ANSWER 5 // Не отвечает


#define OPTNAME_TIMEZONE "TIMEZONE"
#define OPTNAME_ORGNAME "ORGNAME"
#define OPTNAME_INN "INN"
#define OPTNAME_OGRN "OGRN"
#define OPTNAME_ADDRESS "ADDRESS"
#define OPTNAME_PHONE "PHONE"
#define OPTNAME_TAXATION_SYSTEM "TAXATION_SYSTEM"
//#define OPTNAME_SMSTEXT "SMSTEXT"
#define OPTNAME_THUMB_WIDTH "THUMB_WIDTH"
#define OPTNAME_THUMB_HEIGHT "THUMB_HEIGHT"
#define OPTNAME_IMAGE_WIDTH "IMAGE_WIDTH"
#define OPTNAME_CROP_IMAGE "CROP_IMAGE"
#define OPTNAME_JPEG_QUALITY "JPEG_QUALITY"
#define OPTNAME_SMS_GATE_SENDER "SMS_GATE_SENDER"
#define OPTNAME_SMS_GATE_LOGIN "SMS_GATE_LOGIN"
#define OPTNAME_SMS_GATE_PASSWORD "SMS_GATE_PASSWORD"
#define OPTNAME_SMS_ONSALE_TEXT "SMS_ONSALE_TEXT"
#define OPTNAME_SMTP_HOST "SMTP_HOST"
#define OPTNAME_SMTP_LOGIN "SMTP_LOGIN"
#define OPTNAME_SMTP_PASSWORD "SMTP_PASSWORD"
#define OPTNAME_DISCOUNT_MARGIN_PERCENT "DISCOUNT_MARGIN_PERCENT"
#define OPTNAME_ALLOW_OVERSALE "ALLOW_OVERSALE"
#define OPTNAME_ALLOW_PAY_BANK "ALLOW_PAY_BANK"
#define OPTNAME_ALLOW_READY_ORDER_EDIT "ALLOW_READY_ORDER_EDIT"
#define OPTNAME_ALLOW_GIVEOUT_UNPAID "ALLOW_GIVEOUT_UNPAID"
#define OPTNAME_OTCOME_REPORT_PERCENT "OTCOME_REPORT_PERCENT"
#define OPTNAME_ALLOW_EDIT_CLIENTS "ALLOW_EDIT_CLIENTS"
#define OPTNAME_PAYMENT_URL "PAYMENT_URL"
#define OPTNAME_KPP "KPP"
#define OPTNAME_BANK "BANK"
#define OPTNAME_BIK "BIK"
#define OPTNAME_KSCHET "KSCHET"
#define OPTNAME_RSCHET "RSCHET"
#define OPTNAME_SITE_URL "SITE_URL"

/*class User
{
private:
	quint64 m_id;
	QString m_name;
	QString m_inn;
	quint64 m_permissions;
	unsigned int m_frPassword; // Пароль пользователя для фискального регистратора.
public:
	static User createFromToken(QString login, QString token, QWidget *parent = nullptr);
	User();
	User(QString password, QWidget *parent = nullptr);
	User(QString login, QString password, QWidget *parent = nullptr);
	quint64 id() const{return m_id;}
	QString name() const{return m_name;}
	QString inn() const{return m_inn;}
	unsigned int frPassword() const{return m_frPassword;}
	void setId(quint64 id){m_id=id;}
	void setName(QString name){m_name=name;}
	void setInn(QString inn){m_inn=inn;}
	void setPermissions(quint64 permissions){m_permissions=permissions;}
	void setFrPassword(unsigned int frPassword){m_frPassword=frPassword;}
	bool checkRights(quint64 requestedPermissions) const; // Возвращает true, если у пользователя есть все запрошенные разрешения.
	quint64 permissions() const;
	bool isAnyPermission(quint64 requestedPermissions) const;
	bool isAllPermissions(quint64 requestedPermissions) const;
};*/

class QueryParameters
{
private:
	QStringList m_name;
	QList<QVariant> m_value;
public:
	void append(QString n,QVariant v)
	{
		m_name.append(n);
		m_value.append(v);
	}
	int size() const{return m_name.size();}
	QString name(int i) const{return m_name.at(i);}
	QVariant value(int i) const{return m_value.at(i);}
};

class GlobalClass
{
public:
//    User user; // logged in user.
	QString connectionName;
//    QString friendConnectionName;
// Options.
	int optionTimeZone;
	QString optionOrgname;
	QString optionInn;
	QString optionOgrn;
	QString optionAddress;
	int optionTaxationSystem;
	QString optionPhone;
	int optionThumbWidth;
	int optionThumbHeight;
	int optionImageWidth;
	bool optionCropImage;
	int optionJpegQuality;
	QString optionSmsGateSender;
	QString optionSmsGateLogin;
	QString optionSmsGatePassword;
	QString optionSmsOnSaleText; // Товар поступил в продажу
	QString optionSmtpHost;
	QString optionSmtpLogin;
	QString optionSmtpPassword;
	int optionDiscountMarginPercent;
	bool optionAllowOversale;
	bool optionAllowPayBank;
	bool optionAllowReadyOrderEdit;
	bool optionAllowGiveoutUnpaid;
	double optionOutcomeReportPercent;
	bool optionAllowEditClients;
	QString optionPaymentURL;
	QString optionKPP;
	QString optionBank;
	QString optionBIK;
	QString optionKSchet;
	QString optionRSchet;
	QString optionSiteURL;

	// Methods.
public:
//    bool init();
	GlobalClass();
	bool loadDatabaseData(QWidget * parent);
};

extern GlobalClass glo;
extern bool debugFlag;



QString appVersion();
// Database related functions.
bool addDatabase(QString connectionName, const XMLSettings &s);
bool addDatabase(QString connectionName,QString dbtype,QString dbhostname, QString username, QString password,QString databasename,QString serverCaCertFileName,QString clientKeyFileName,QString clientCertFileName);
void clearDatabaseConnections();
//bool openDatabase(QString connectionName = QLatin1String(QSqlDatabase::defaultConnection));
//bool isConnectionExists(int connectionNum);
//void closeDatabase(QString connectionName = QLatin1String(QSqlDatabase::defaultConnection));
//bool selectConnection(XMLSettings &settings);
void connectionTimer();
QString titleInfo();
QStringList GetSelectedList(QString field, QSqlQueryModel *sqlQueryModel, QTableView *tableView);
qint64 GetUID(QWidget * parent,QString Password);
//quint64 GetUID(QWidget * parent,QString login,QString password);
bool QueryExec(const QString &queryString, const QString connectionName = glo.connectionName); // Silent.
bool QueryExec(const QString &queryString, QSqlQuery &sqlQuery, const QString connectionName = glo.connectionName); // Silent.
bool QueryExec(QWidget *parent, const QString &queryString, QString connectionName = glo.connectionName);
bool QueryExec(QWidget *parent, const QString &queryString, QSqlQuery &sqlQuery, const QString connectionName = glo.connectionName);
bool QueryExecPrepared(const QString &queryString, const QueryParameters &qParam, QString connectionName = glo.connectionName); // Silent.
bool QueryExecPrepared(const QString &queryString, const QueryParameters &qParam, QSqlQuery &sqlQuery, QString connectionName = glo.connectionName); // Silent.
bool QueryExecPrepared(QWidget * parent, const QString &queryString, const QueryParameters &qParam, QString connectionName = glo.connectionName);
bool QueryExecPrepared(QWidget * parent, const QString &queryString, const QueryParameters &qParam, QSqlQuery &sqlQuery, QString connectionName = glo.connectionName);
bool QueryModelExec(QWidget * parent, QString queryString, QSqlQueryModel *sqlQueryModel, QString connectionName = glo.connectionName);
bool transaction(QString connectionName = glo.connectionName); // Не работает с CREATE, ALTER, DROP
bool commit(QString connectionName = glo.connectionName);
bool rollback(QString connectionName = glo.connectionName);
QString getUserName(QWidget * parent, quint64 id);
QString getOption(QWidget * parent,QString name); // Selects the option value from the table 'options'.
QString escapeQuotes(QString string); // Single quote turned into a pair of single quotes.
QString escapeCSV(QString string);
QString escapeJson(QString data);
QString EncodeToXML(QString string);
QString DecodeFromXML(QString string);
QString timestamp();
QString programDataPath();
QString userDataPath();
void logString(QWidget * parent,QString string,bool showMessageBox=true);
QString num(int num);
QString num(unsigned int num);
QString num(quint64 num);
QString num(qint64 num);
QString num(double num);
double roundValute(double value);
QByteArray encryptPassword(QString password);
QString decryptPassword(QByteArray data);
unsigned int encodeCollected(int d); // Кодирует сумму для последующей записи в поле инкассации.
bool checkRights(QWidget * parent,unsigned int userid,unsigned int rights); // Возвращает true, если все запрошенные права у пользователя имеются.
void debug(QString debugStr);
void setDebug(bool debugFl);

#endif
