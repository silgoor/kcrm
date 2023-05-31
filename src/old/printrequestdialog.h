#ifndef PRINTREQUESTDIALOG_H
#define PRINTREQUESTDIALOG_H

#include <QDialog>

namespace Ui {
class PrintRequestDialog;
}

class PrintRequestDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PrintRequestDialog(int requestid, QWidget *parent = 0);
    ~PrintRequestDialog();

private slots:
    void on_printPushButton_clicked();

private:
    Ui::PrintRequestDialog *ui;
    int m_requestid;
};

#endif // PRINTREQUESTDIALOG_H
