#ifndef CLIENTSDIALOG_H
#define CLIENTSDIALOG_H

#include <QDialog>
#include "common_inc.h"

namespace Ui {
class ClientsDialog;
}

class ClientsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ClientsDialog(int clientid, QWidget *parent = 0);
    ~ClientsDialog();
    void refreshClients();
    int clientId();

private slots:
    void on_clientsTableView_activated(const QModelIndex &index);
    void on_newclientPushButton_clicked();

    void on_editPushButton_clicked();

    void on_filterLineEdit_textChanged(const QString &arg1);

    void on_okPushButton_clicked();

private:
    Ui::ClientsDialog *ui;
    QSqlQueryModel *clientsQueryModel;
    int m_id;
};

#endif // CLIENTSDIALOG_H
