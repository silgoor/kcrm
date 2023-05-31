#ifndef CONNECTIONDIALOG_H
#define CONNECTIONDIALOG_H

#include <QDialog>
#include "ui_connectiondialog.h"

class ConnectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectionDialog(QWidget *parent = 0);
    ~ConnectionDialog();
	bool createTables();
    bool createIndexIfNotExists(QWidget * parent,QString table,QString indexName, QString columnName,QString indexLength);
    bool addColumnIfNotExists(QString table,QString columnName, QString columnType, QString afterColumn);
    Ui::ConnectionDialog ui;
private slots:
	void on_okPushButton_clicked();
	void on_cancelPushButton_clicked();
	void on_createTablesPushButton_clicked();
	void on_sqliteRadioButton_toggled(bool checked);
//    void on_movePerformersPushButton_clicked();
};

#endif // CONNECTIONDIALOG_H
