#include "printdialog.h"
#include "ui_printdialog.h"
#include <QPrinter>
#include <QPrintDialog>
#include <QFileDialog>

//-------------------------------------------------------------------------------------
PrintDialog::PrintDialog(QByteArray html, const QUrl & baseUrl = QUrl(), QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PrintDialog)
{
    ui->setupUi(this);
    ui->webView->setHtml(html,baseUrl);
}
//-------------------------------------------------------------------------------------
PrintDialog::~PrintDialog()
{
    delete ui;
}
//-------------------------------------------------------------------------------------
void PrintDialog::on_printPushButton_clicked()
{
    QPrinter printer;
    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    if (dlg->exec() != QDialog::Accepted)
        return;
    ui->webView->print(&printer);
    accept();
}
//-------------------------------------------------------------------------------------
void PrintDialog::on_saveToPdfPushButton_clicked()
{
    QString fileNameTemplate=ui->webView->title();
    QString fileName = QFileDialog::getSaveFileName(this, tr("Сохранить как"),fileNameTemplate,tr("PDF Document (*.pdf)"));
    if(fileName.isEmpty())
        return;
    QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    ui->webView->print(&printer);
    accept();
}
//-------------------------------------------------------------------------------------
