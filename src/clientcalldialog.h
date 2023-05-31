#ifndef CLIENTCALLDIALOG_H
#define CLIENTCALLDIALOG_H

#include <QDialog>
#include <QtSql>

namespace Ui {
class ClientCallDialog;
}

class ClientCallDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ClientCallDialog(quint64 clientId, QWidget *parent = nullptr);
    ~ClientCallDialog();
    void refreshRequests();
    void refreshAgreements();

private slots:
    void on_savePushButton_clicked();

    void on_requestsTableView_activated(const QModelIndex &index);

    void on_agreementsTableView_activated(const QModelIndex &index);

private:
    Ui::ClientCallDialog *ui;
    quint64 m_clientId;
    QSqlQueryModel *m_agreementsQueryModel;
    QSqlQueryModel *m_requestsQueryModel;
};

#endif // CLIENTCALLDIALOG_H
