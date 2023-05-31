#ifndef USERDIALOG_H
#define USERDIALOG_H

#include <QDialog>

namespace Ui {
class UserDialog;
}

class UserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UserDialog(int userid,QWidget *parent = 0);
    ~UserDialog();
    void refreshOffices();
    void fillPerformerOffices();
private slots:
    void on_okPushButton_clicked();

    void on_folderPushButton_clicked();

private:
    Ui::UserDialog *ui;
    int m_userid;
};

#endif // USERDIALOG_H
