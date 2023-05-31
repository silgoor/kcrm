#ifndef PERFORMERSDIALOG_H
#define PERFORMERSDIALOG_H

#include <QDialog>
#include <QSqlQueryModel>

namespace Ui {
class PerformersDialog;
}

class PerformersDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PerformersDialog(QWidget *parent = 0);
    ~PerformersDialog();
    void refreshPerformers();

private slots:
    void on_performersTableView_activated(const QModelIndex &index);
    void on_newPushButton_clicked();
    void on_closePushButton_clicked();
    void On_performersHeaderView_sectionClicked();

    void on_nameFilterLineEdit_textChanged(const QString &arg1);

    void on_enabledFilterCheckBox_stateChanged(int arg1);

private:
    Ui::PerformersDialog *ui;
    QSqlQueryModel *performersQueryModel;
};

#endif // PERFORMERSDIALOG_H
