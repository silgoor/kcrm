#ifndef PRINTBILLDIALOG_H
#define PRINTBILLDIALOG_H

#include <QDialog>

namespace Ui {
class PrintBillDialog;
}

class PrintBillDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PrintBillDialog(QWidget *parent = 0);
    ~PrintBillDialog();

private:
    Ui::PrintBillDialog *ui;
};

#endif // PRINTBILLDIALOG_H
