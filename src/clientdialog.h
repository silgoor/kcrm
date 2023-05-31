#ifndef CLIENTDIALOG_H
#define CLIENTDIALOG_H

#include <QDialog>
#include "common_inc.h"

namespace Ui {
class ClientDialog;
}

class ClientDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ClientDialog(quint64 clientid, QString filter, QWidget *parent = 0);
    ~ClientDialog();
    int clientId();
//    void refreshClientDevices();
	bool refreshDirectionsComboBox(int currentId);

private slots:
    void on_okPushButton_clicked();

//    void on_newClientDevicePushButton_clicked();
//    void on_editClientDevicePushButton_clicked();
//    void on_clientDevicesTableView_activated(const QModelIndex &index);

private:
    Ui::ClientDialog *ui;
    quint64 m_clientid;
//    QSqlQueryModel *clientDevicesQueryModel;
};

#endif // CLIENTDIALOG_H
