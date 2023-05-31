#ifndef PRINTDIALOG_H
#define PRINTDIALOG_H

#include <QDialog>
#include <QPrinter>
#include "common_inc.h"

namespace Ui {
class PrintDialog;
}

class PrintDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PrintDialog(QByteArray html, const QUrl &baseUrl, QWidget *parent = nullptr);
    ~PrintDialog();

private slots:
    void on_printPushButton_clicked();

    void on_saveToPdfPushButton_clicked();

    void on_sendMailPushButton_clicked();

    void On_pdfPrintingFinished(const QString filePathName, bool success);
private:
    Ui::PrintDialog *ui;
    QPrinter::Orientation m_orientation;
    bool mailFlag;
};

#endif // PRINTDIALOG_H
