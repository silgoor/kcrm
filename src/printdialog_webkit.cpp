#include "printdialog.h"
#include "ui_printdialog_webkit.h"
#include <QPrinter>
#include <QPrintDialog>
#include <QFileDialog>
#include <QProcess>
#include "common_inc.h"

//-------------------------------------------------------------------------------------
PrintDialog::PrintDialog(QByteArray html, const QUrl & baseUrl = QUrl(), QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PrintDialog)
{
    ui->setupUi(this);
//    ui->textBrowser->setHtml(html);
#ifndef Q_OS_LINUX
    ui->sendMailPushButton->hide();
#endif
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
//    ui->textBrowser->print(&printer);
    ui->webView->print(&printer);
    delete dlg;
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
void PrintDialog::on_sendMailPushButton_clicked()
{
    QString tmpPathName=userDataPath()+"/tmp";
    QString filePathName=tmpPathName+"/"+ui->webView->title()+".pdf";

    { // Создаём папку для временных файлов, если не существует.
        QDir dir(tmpPathName);
        if(!dir.exists())
        {
            if(!dir.mkpath(tmpPathName))
            {
                logString(this,tr("Не удалось создать папку %1").arg(tmpPathName));
                return;
            }
        }
    }
    QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(filePathName);
    ui->webView->print(&printer);

#ifdef Q_OS_LINUX
    QProcess::startDetached("thunderbird -remote \"xfeDoCommand(composeMessage,subject='"+ui->webView->title()+"',body='-',attachment='"+filePathName+"')\"");
#endif
#ifdef Q_OS_MAC
        QProcess::startDetached("open -a Mail \""+filePathName+"\"");
#endif
#ifdef Q_OS_WINDOWS
        QProcess::startDetached("outlook.exe /c ipm.note /a \""+filePathName+"\""));
#endif
}
//-------------------------------------------------------------------------------------
