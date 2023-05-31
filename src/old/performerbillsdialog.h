#ifndef PERFORMERBILLSDIALOG_H
#define PERFORMERBILLSDIALOG_H

#include <QDialog>
#include <QtSql>

namespace Ui {
class PerformerbillsDialog;
}

class PerformerbillsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PerformerbillsDialog(QWidget *parent = 0);
    ~PerformerbillsDialog();
    void refreshPerformers();
    void refreshPerformerBills();

private slots:
    void on_performerComboBox_currentIndexChanged(int index);

    void on_generatePushButton_clicked();

    void on_performerBillsTableView_activated(const QModelIndex &index);

    void on_printSelectedPushButton_clicked();

private:
    Ui::PerformerbillsDialog *ui;
    QSqlQueryModel *performerBillsQueryModel;
};

#endif // PERFORMERBILLSDIALOG_H
