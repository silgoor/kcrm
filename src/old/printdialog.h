#ifndef PRINTDIALOG_H
#define PRINTDIALOG_H

#include <QDialog>

namespace Ui {
class PrintDialog;
}

class PrintDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PrintDialog(QByteArray html, const QUrl &baseUrl, QWidget *parent = 0);
    ~PrintDialog();

private slots:
    void on_printPushButton_clicked();

    void on_saveToPdfPushButton_clicked();

private:
    Ui::PrintDialog *ui;
};

#endif // PRINTDIALOG_H
