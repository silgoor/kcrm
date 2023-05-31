#ifndef OFFICESDIALOG_H
#define OFFICESDIALOG_H

#include <QDialog>
#include <QSqlQueryModel>

namespace Ui {
class OfficesDialog;
}

class OfficesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OfficesDialog(QWidget *parent = nullptr);
    ~OfficesDialog();
    void refreshOffices();

private slots:
    void on_officesTableView_activated(const QModelIndex &index);
    void on_closePushButton_clicked();
    void on_newPushButton_clicked();
    void On_officesHeaderView_sectionClicked();

private:
    Ui::OfficesDialog *ui;
    QSqlQueryModel *officesQueryModel;
};

#endif // OFFICESDIALOG_H
