#include "printdialog.h"
#include "ui_printdialog.h"
#include <QPrintDialog>
#include <QFileDialog>
#include <QProcess>

//-------------------------------------------------------------------------------------
PrintDialog::PrintDialog(QByteArray html, const QUrl & baseUrl = QUrl(), QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PrintDialog),
    m_orientation(QPrinter::Portrait),
    mailFlag(false)
{
    ui->setupUi(this);

	if(debugFlag)
	{
		QFile testFile(programDataPath()+"/test.html");
		testFile.open(QIODevice::WriteOnly|QIODevice::Truncate);
		testFile.write(html);
		testFile.close();
	}
//    ui->textBrowser->setHtml(html);
//#ifndef Q_OS_LINUX
//    ui->sendMailPushButton->hide();
//#endif
    ui->webView->setHtml(html,baseUrl);
    if(html.contains("size: landscape"))
	{
        m_orientation=QPrinter::Landscape;
	}

    connect(ui->webView->page(),SIGNAL(pdfPrintingFinished(const QString, bool)),this,SLOT(On_pdfPrintingFinished(const QString, bool)));
}
//-------------------------------------------------------------------------------------
PrintDialog::~PrintDialog()
{
    delete ui;
}
//-------------------------------------------------------------------------------------
void PrintDialog::on_printPushButton_clicked()
{
    QPrinter *printer=new QPrinter();
    printer->setOrientation(m_orientation);

	{
        QPrintDialog *dialog = new QPrintDialog(printer, this);
		dialog->setWindowTitle(tr("Кашоп - печать"));
		int result=dialog->exec();
		delete dialog;
		if (result != QDialog::Accepted)
        {
            delete printer;
            return;
        }
	}
    ui->webView->page()->print(printer, [=](bool result) {Q_UNUSED(result) delete printer;accept(); });
//    ui->webView->print(&printer);
//    accept(); // Закрывает окно раньше, чем напечатается.
}
//-------------------------------------------------------------------------------------
void PrintDialog::on_saveToPdfPushButton_clicked()
{
    QString fileNameTemplate=ui->webView->title();
    QString fileName = QFileDialog::getSaveFileName(this, tr("Сохранить как"),fileNameTemplate,tr("PDF Document (*.pdf)"));
    if(fileName.isEmpty())
        return;
/*    QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    ui->webView->print(&printer);*/
    ui->webView->page()->printToPdf(fileName);
//    accept(); // Закрывает окно раньше, чем напечатается.
}
//-------------------------------------------------------------------------------------
void PrintDialog::on_sendMailPushButton_clicked()
{
    QString tmpPathName=userDataPath()+"/tmp";
    QString filePathName=tmpPathName+"/"+ui->webView->title()+".pdf";
//    filePathName.replace(" ","_");

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
    mailFlag=true;
    ui->webView->page()->printToPdf(filePathName);
}
//-------------------------------------------------------------------------------------
void PrintDialog::On_pdfPrintingFinished(const QString filePathName, bool success)
{
    if(!success)
    {
        mailFlag=false;
        logString(this,tr("Ошибка сохранения документа %1").arg(filePathName));
        return;
    }
/*    QString tmpPathName=userDataPath()+"/tmp";
    QString filePathName=tmpPathName+"/"+ui->webView->title()+".pdf";
    filePathName.replace(" ","_");*/
    if(mailFlag)
    {
        mailFlag=false;
#ifdef Q_OS_LINUX
    QProcess::startDetached("thunderbird -remote \"xfeDoCommand(composeMessage,subject='"+ui->webView->title()+"',body='-',attachment='"+filePathName+"')\"");
#endif
#ifdef Q_OS_MAC
        QProcess::startDetached("open -a Mail \""+filePathName+"\"");
#endif
#ifdef Q_OS_WINDOWS
        QProcess::startDetached("outlook.exe /c ipm.note /a \""+filePathName+"\"");
#endif
    }
}
//-------------------------------------------------------------------------------------
