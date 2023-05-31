#include "printbilldialog.h"
#include "ui_printbilldialog.h"

PrintBillDialog::PrintBillDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PrintBillDialog)
{
    ui->setupUi(this);
}

PrintBillDialog::~PrintBillDialog()
{
    delete ui;
}
