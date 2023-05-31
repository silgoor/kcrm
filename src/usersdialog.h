#ifndef USERSDIALOG_H
#define USERSDIALOG_H

#include <QDialog>
#include <QSqlQueryModel>

namespace Ui {
class UsersDialog;
}

class UsersDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UsersDialog(QWidget *parent = 0);
    ~UsersDialog();
    void refreshUsers();

private slots:
    void on_newPushButton_clicked();
    void on_usersTableView_activated(const QModelIndex &index);
    void on_editPushButton_clicked();
    void on_closePushButton_clicked();
    void On_usersHeaderView_sectionClicked();

private:
    Ui::UsersDialog *ui;
    QSqlQueryModel *usersQueryModel;
};

#endif // USERSDIALOG_H
