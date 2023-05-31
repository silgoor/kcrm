#include "connectiondialog.h"
#include "ui_connectiondialog.h"
#include "../common_inc.h"
#include "QtGui"

//-------------------------------------------------------------------------------------
ConnectionDialog::ConnectionDialog(QWidget *parent) :
	QDialog(parent)
{
	ui.setupUi(this);
}
//-------------------------------------------------------------------------------------
ConnectionDialog::~ConnectionDialog()
{
}
//-------------------------------------------------------------------------------------
void ConnectionDialog::on_okPushButton_clicked()
{
	if(ui.connectionNameLineEdit->text().isEmpty())
	{
		QMessageBox::information(this,PROGNAME,tr("Пожалуйста, введите название соединения."));
		return;
	}
	accept();
}
//-------------------------------------------------------------------------------------
void ConnectionDialog::on_cancelPushButton_clicked()
{
	reject();
}
//-------------------------------------------------------------------------------------
void ConnectionDialog::on_createTablesPushButton_clicked()
{
	QString dbtype=DBTYPE_SQLITE;
	if(ui.mysqlRadioButton->isChecked())
		dbtype=DBTYPE_MYSQL;
    if(ui.postgresqlRadioButton->isChecked())
        dbtype=DBTYPE_POSTGRESQL;
    addDatabase("",dbtype,ui.dbhostnameLineEdit->text(),ui.dbusernameLineEdit->text(),ui.dbpasswordLineEdit->text(),ui.databasenameLineEdit->text());
    createTables();
    clearDatabaseConnections();
}
//-------------------------------------------------------------------------------------
void ConnectionDialog::on_sqliteRadioButton_toggled(bool checked)
{
	if(checked)
	{
		ui.dbhostnameLineEdit->setEnabled(false);
		ui.dbusernameLineEdit->setEnabled(false);
		ui.dbpasswordLineEdit->setEnabled(false);
	}
	else
	{
		ui.dbhostnameLineEdit->setEnabled(true);
		ui.dbusernameLineEdit->setEnabled(true);
		ui.dbpasswordLineEdit->setEnabled(true);
	}
}
//-------------------------------------------------------------------------------------
bool ConnectionDialog::createTables()
{
	QString charsetStr="";
    QString primaryKeyStr="INTEGER PRIMARY KEY AUTOINCREMENT";
	if(ui.mysqlRadioButton->isChecked())
	{
		charsetStr=" CHARACTER SET utf8 COLLATE utf8_general_ci";
        primaryKeyStr="INTEGER PRIMARY KEY AUTO_INCREMENT";
	}
    if(ui.postgresqlRadioButton->isChecked())
    {
        charsetStr=" ENCODING 'UTF8'";
        primaryKeyStr="SERIAL PRIMARY KEY";
    }
    if(!QueryExec(this,"CREATE TABLE IF NOT EXISTS requests (id "+primaryKeyStr+", agreementid INTEGER, officeid INTEGER, status INTEGER, clientid INTEGER, performerid INTEGER, performerbillid INTEGER, rating INTEGER, percent INTEGER, bonususer INTEGER, flags BIGINT, devicename TEXT, defect TEXT, done TEXT, notes TEXT, clientcost REAL, cost REAL, collection INTEGER UNSIGNED, performdate TEXT, cuserid INTEGER, ctime TEXT, mtime TEXT)"+charsetStr+";"))
		return false;
	QueryExec(this,"CREATE TABLE IF NOT EXISTS directions (id "+primaryKeyStr+", officeid INTEGER, name TEXT)"+charsetStr+";"); // Направления выезда на заявку. У каждого офиса свои направления.
	QueryExec(this,"CREATE TABLE IF NOT EXISTS requestflags (id "+primaryKeyStr+", bit INTEGER, name TEXT, rfgroupid INTEGER)"+charsetStr+";");
    QueryExec(this,"CREATE TABLE IF NOT EXISTS requestflaggroups (id "+primaryKeyStr+", type INTEGER, name TEXT)"+charsetStr+";");
//    QueryExec(this,"CREATE TABLE IF NOT EXISTS performerbills (id "+primaryKeyStr+", performerid INTEGER, amount REAL, collected INTEGER UNSIGNED, collect_userid INTEGER, cuserid INTEGER, ctime TEXT)"+charsetStr+";");
//    QueryExec(this,"CREATE TABLE IF NOT EXISTS clients (id "+primaryKeyStr+", sex INTEGER, surname TEXT, name TEXT, phone1 TEXT, phone2 TEXT, email TEXT, address TEXT, passport TEXT, paddress TEXT, inn TEXT, kpp TEXT, okpo TEXT, bank TEXT, rschet TEXT, kschet TEXT, bik TEXT, ogrn TEXT, okved TEXT, store_address TEXT, store_phone TEXT, gendirector TEXT, manager1 TEXT, manager1_phone TEXT, manager1_email TEXT, manager2 TEXT, manager2_phone TEXT, manager2_email TEXT, site TEXT, paytype INTEGER, delivery TEXT, brands TEXT, worktime TEXT, cuserid INTEGER, ctime TEXT, mtime TEXT)"+charsetStr+";");
    QueryExec(this,"CREATE TABLE IF NOT EXISTS states (id "+primaryKeyStr+", name TEXT)"+charsetStr+";");
    {
        QueryExec(this,"REPLACE INTO states(id,name) VALUES(1,'В работе');");
        QueryExec(this,"REPLACE INTO states(id,name) VALUES(3,'Созвон');");
        QueryExec(this,"REPLACE INTO states(id,name) VALUES(5,'Не оплачена');");
        QueryExec(this,"REPLACE INTO states(id,name) VALUES(6,'Долг мастера');");
        QueryExec(this,"REPLACE INTO states(id,name) VALUES(7,'Тест');");
        QueryExec(this,"REPLACE INTO states(id,name) VALUES(8,'Хищение');");
        QueryExec(this,"REPLACE INTO states(id,name) VALUES(10,'Выполнено');");
        QueryExec(this,"REPLACE INTO states(id,name) VALUES(20,'ОТМЕНА');");
    }
    QueryExec(this,"CREATE TABLE IF NOT EXISTS offices (id "+primaryKeyStr+", enabled INTEGER DEFAULT 1, performerbillpercent INTEGER, name TEXT, email TEXT, url TEXT, phone1 TEXT, phone2 TEXT, address TEXT, managername TEXT, notes TEXT, ctime TEXT)"+charsetStr+";");
/*    QueryExec(this,"CREATE TABLE IF NOT EXISTS users (id "+primaryKeyStr+", enabled INTEGER DEFAULT 1, login TEXT, password TEXT, rights INTEGER, sex INTEGER, name TEXT, birthday TEXT, phone1 TEXT, phone2 TEXT, email TEXT, address TEXT, position TEXT, pseries TEXT, pnumber TEXT, pdepartment TEXT, paddress TEXT, notes TEXT, rating_avg REAL, ctime TEXT, websessionuid TEXT)"+charsetStr+";");
    {
        QueryExec(this,"INSERT INTO users(id,enabled,rights,name,password,ctime) VALUES(1,1,31, 'Admin', '"+encryptPassword("111")+"','"+timestamp()+"');");
    }*/
//    addColumnIfNotExists("users","rating_avg", "REAL","notes");
//    addColumnIfNotExists("users","position", "REAL","address");

//    addColumnIfNotExists("clients","call_result", "INTEGER","worktime");
//    addColumnIfNotExists("clients","next_call_date", "TEXT","call_result");
//    createIndexIfNotExists(this,"clients","call_result_index","call_result","");
//    createIndexIfNotExists(this,"clients","next_call_date_index","next_call_date","(10)");

	addColumnIfNotExists("clients","directionid", "INTEGER","email");
	addColumnIfNotExists("directions","enabled", "INTEGER DEFAULT 1","id");

    QueryExec(this,"CREATE TABLE IF NOT EXISTS performeroffices (id "+primaryKeyStr+", performerid INTEGER, officeid INTEGER)"+charsetStr+";");
	{
        QueryExec(this,"REPLACE INTO options(id,name,localname,description,value,timestamp) VALUES(26,'"+QString("SMS_NEW_REQUEST_CLIENT_TEXT")+"','"+tr("Текст СМС-оповещения клиента о создании заявки")+"','"+tr("<RID> заменяется на номер заявки, <RDATE> - дата выполнения, <RPERFORMER> - имя мастера, <RPERFORMER_PHONE> - телефон мастера, <RNOTES> - примечания.")+"','"+tr("Номер заявки <RID>. Дата выполнения: <RDATE>.")+"','"+timestamp()+"');");
        QueryExec(this,"REPLACE INTO options(id,name,localname,description,value,timestamp) VALUES(27,'"+QString("SMS_NEW_REQUEST_PERFORMER_TEXT")+"','"+tr("Текст СМС-оповещения мастера о создании заявки")+"','"+tr("<RINFO> заменяется на полную информацию о заявке.")+"','"+tr("<RINFO>.")+"','"+timestamp()+"');");
        QueryExec(this,"REPLACE INTO options(id,name,localname,description,value,timestamp) VALUES(28,'"+QString("USERS_DOCUMENTS_PATH")+"','"+tr("Путь к папке с документами пользователей")+"','"+tr("Например //server25/userdocuments")+"','"+tr("")+"','"+timestamp()+"');");
        QueryExec(this,"REPLACE INTO options(id,name,localname,description,value,timestamp) VALUES(29,'"+QString("PERFORMERS_DOCUMENTS_PATH")+"','"+tr("Путь к папке с документами мастеров")+"','"+tr("Например //server25/masterocuments")+"','"+tr("")+"','"+timestamp()+"');");
    }
//    QueryExec(this,"CREATE TABLE IF NOT EXISTS events (id "+primaryKeyStr+", actionid INTEGER, rowid INTEGER, userid INTEGER, ctime TEXT)"+charsetStr+";");
//    QueryExec(this,"CREATE TABLE IF NOT EXISTS actions (id "+primaryKeyStr+", name TEXT)"+charsetStr+";");
    {
        QueryExec(this,"REPLACE INTO actions(id,name) VALUES("+num(ACTIONID_LOGIN_KCRM)+",'"+tr("Вход kcrm")+"');");
        QueryExec(this,"REPLACE INTO actions(id,name) VALUES("+num(ACTIONID_LOGOUT_KCRM)+",'"+tr("Выход kcrm")+"');");
    }
    QueryExec(this,"CREATE TABLE IF NOT EXISTS agreements (id "+primaryKeyStr+", officeid INTEGER, type INTEGER, trest INTEGER, startdate TEXT, timelimit INTEGER, status INTEGER, cost DOUBLE, collected INTEGER UNSIGNED, client INTEGER, json TEXT, devicetype INTEGER, alertdate TEXT, cuser INTEGER, ctime TEXT)"+charsetStr+";");
    QueryExec(this,"CREATE TABLE IF NOT EXISTS agreementdevicetypes (id "+primaryKeyStr+", aid INTEGER, dtid INTEGER)"+charsetStr+";");
    QueryExec(this,"CREATE TABLE IF NOT EXISTS agreementtypes (id "+primaryKeyStr+", enabled INTEGER DEFAULT 1, name TEXT)"+charsetStr+";");
//    QueryExec(this,"INSERT INTO agreementtypes(id,name) VALUES(1,'"+tr("Тип 1")+"');");
    QueryExec(this,"CREATE TABLE IF NOT EXISTS trests (id "+primaryKeyStr+", enabled INTEGER DEFAULT 1, name TEXT)"+charsetStr+";");
    QueryExec(this,"CREATE TABLE IF NOT EXISTS messages (id "+primaryKeyStr+", chat_type INTEGER,chat_id INTEGER, message TEXT,cuser INTEGER, ctime TEXT)"+charsetStr+";");
	QueryExec(this,"CREATE TABLE IF NOT EXISTS directions (id "+primaryKeyStr+", enabled INTEGER DEFAULT 1, name TEXT)"+charsetStr+";");
	return true;
}
//-------------------------------------------------------------------------------
bool ConnectionDialog::createIndexIfNotExists(QWidget * parent,QString table,QString indexName, QString columnName,QString indexLength)
{
    int dbmsType=QSqlDatabase::database("").driver()->dbmsType();
    if(dbmsType==QSqlDriver::MySqlServer)
    {
        QSqlQuery keysQuery;
        if(QueryExec(parent,"SHOW KEYS FROM `"+table+"` WHERE Key_name='"+indexName+"';",keysQuery))
        {
            if(keysQuery.next())
            {
                qDebug() << "Index \"" << indexName << "\" already exists in table \"" << table << "\".";
                return true;
            }
            if(QueryExec(parent,"CREATE INDEX "+indexName+" ON `"+table+"`("+columnName+indexLength+");"))
            {
                qDebug() << "Index \""<< indexName << "\" added.";
                return true;
            }
        }
    }
    else //if(dbmsType==QSqlDriver::SQLite)
    { // Подходит для sqlite и postgresql
        if(QueryExec(parent,"CREATE INDEX IF NOT EXISTS "+indexName+" ON "+table+"("+columnName+");"))
        {
            qDebug() << "Index \""<< indexName << "\" added.";
            return true;
        }
    }
    qDebug() << "Index creating \""<< indexName << "\" fail.";
    return false;
}
//-------------------------------------------------------------------------------
bool ConnectionDialog::addColumnIfNotExists(QString table,QString columnName, QString columnType, QString afterColumn)
{
    QString afterColumnSQL="";
    int dbmsType=QSqlDatabase::database("").driver()->dbmsType();
    if(dbmsType==QSqlDriver::MySqlServer)
    {
        afterColumnSQL=" AFTER `"+afterColumn+"`";
    }
    // Подходит для sqlite и postgresql
    if(QueryExec("SELECT `"+columnName+"` FROM `"+table+"` LIMIT 1;"))
    {
        qDebug() << "Column \"" << columnName << "\" already exists in table \"" << table << "\".";
        return true;
    }
    if(QueryExec(this,"ALTER TABLE `"+table+"` ADD `"+columnName+"` "+columnType+afterColumnSQL+";"))
    {
        qDebug() << "Column \""<< columnName << "\" added.";
        return true;
    }
    qDebug() << "Add column \""<< columnName << "\" fail.";
    return false;
}
//-------------------------------------------------------------------------------
