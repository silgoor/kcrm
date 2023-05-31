#ifndef CLEARAMOUNTSDIALOG_H
#define CLEARAMOUNTSDIALOG_H

#include <QDialog>

namespace Ui {
class ClearAmountsDialog;
}

class ClearAmountsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ClearAmountsDialog(QWidget *parent = 0);
    ~ClearAmountsDialog();

private slots:
    void on_clearPushButton_clicked();

private:
    Ui::ClearAmountsDialog *ui;
};

#endif // CLEARAMOUNTSDIALOG_H
