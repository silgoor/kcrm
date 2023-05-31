#ifndef TRESTSDIALOG_H
#define TRESTSDIALOG_H

#include <QDialog>
#include <QSqlQueryModel>

namespace Ui {
class TrestsDialog;
}

class TrestsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TrestsDialog(QWidget *parent = nullptr);
    ~TrestsDialog();
    void refreshTrests();

private slots:
    void on_trestsTableView_activated(const QModelIndex &index);

    void on_newPushButton_clicked();

private:
    Ui::TrestsDialog *ui;
    QSqlQueryModel *trestsQueryModel;
};

#endif // TRESTSDIALOG_H
