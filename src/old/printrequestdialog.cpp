#include "printrequestdialog.h"
#include "ui_printrequestdialog.h"
#include "common_inc.h"
#include <QPrinter>
#include <QPrintDialog>

//-------------------------------------------------------------------------------------
PrintRequestDialog::PrintRequestDialog(int requestid,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PrintRequestDialog)
{
    m_requestid=requestid;
    ui->setupUi(this);
    if(m_requestid)
    {
        QFile file(QApplication::applicationDirPath()+"/request.html");
        if(!file.open(QIODevice::ReadOnly))
        {
            QMessageBox::information(this,PROGNAME,"Не могу открыть файл "+QApplication::applicationDirPath()+"/request.html");
            return;
        }
        QByteArray html=file.readAll();
        QSqlQuery requestQuery;
        QueryExec(this,"SELECT requests.id, requests.ctime, performdate, clients.address, clients.surname, clients.name, clients.phone1, clients.phone2, devicename, defect, cost, users.name, offices.name, states.name, performer.name FROM requests LEFT JOIN  states ON states.id=requests.status LEFT JOIN offices ON offices.id=requests.officeid LEFT JOIN clients ON clients.id = requests.clientid LEFT JOIN users ON users.id = requests.cuserid LEFT JOIN users AS performer ON performer.id=requests.performerid WHERE requests.id='"+num(m_requestid)+"';",requestQuery);
        if(!requestQuery.next())
            return;
        html.replace("[ID]",requestQuery.value(0).toString().toUtf8());
        html.replace("[CREATE_DATE]",requestQuery.value(1).toString().toUtf8());
        html.replace("[PERFORM_DATE]",requestQuery.value(2).toString().toUtf8());
        html.replace("[ADDRESS]",requestQuery.value(3).toString().toUtf8());
        html.replace("[CLIENT_NAME]",requestQuery.value(4).toString().toUtf8()+" "+requestQuery.value(5).toString().toUtf8());
        html.replace("[PHONE1]",requestQuery.value(6).toString().toUtf8());
        html.replace("[PHONE2]",requestQuery.value(7).toString().toUtf8());

        html.replace("[DEVICE_NAME]",requestQuery.value(8).toString().toUtf8());
        html.replace("[DEFECT]",requestQuery.value(9).toString().toUtf8());
        html.replace("[COST]",requestQuery.value(10).toString().toUtf8());
        html.replace("[DISPATCHER]",requestQuery.value(11).toString().toUtf8());
        html.replace("[OFFICE]",requestQuery.value(12).toString().toUtf8());
        html.replace("[STATUS]",requestQuery.value(13).toString().toUtf8());
        html.replace("[MASTER]",requestQuery.value(14).toString().toUtf8());
        ui->textBrowser->setHtml(html);
    }
}
//-------------------------------------------------------------------------------------
PrintRequestDialog::~PrintRequestDialog()
{
    delete ui;
}
//-------------------------------------------------------------------------------------
void PrintRequestDialog::on_printPushButton_clicked()
{
    QPrinter printer;

    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    if (dlg->exec() != QDialog::Accepted)
        return;
    ui->textBrowser->print(&printer);
}
//-------------------------------------------------------------------------------------
