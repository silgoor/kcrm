#ifndef BILLDIALOG_H
#define BILLDIALOG_H

#include <QDialog>
#include <QtSql>

namespace Ui {
class BillDialog;
}

class BillDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BillDialog(int billid,QWidget *parent = 0);
    ~BillDialog();
private slots:
    void on_okPushButton_clicked();

    void on_requestsTableView_activated(const QModelIndex &index);

private:
    void refreshRequests();
private:
    Ui::BillDialog *ui;
    int m_billid;
    QSqlQueryModel *requestsQueryModel;
};

#endif // BILLDIALOG_H
