#include "reminddialog.h"
#include "ui_reminddialog.h"
#include "common_inc.h"

//-------------------------------------------------------------------------------------
RemindDialog::RemindDialog(QWidget *parent) :
	QDialog(parent),
    ui(new Ui::RemindDialog)
{
    ui->setupUi(this);
}
//-------------------------------------------------------------------------------------
RemindDialog::~RemindDialog()
{
    delete ui;
}
//-------------------------------------------------------------------------------------
void RemindDialog::loadRequestInfo(quint64 requestId)
{
    ui->infoLabel->clear();
    QSqlQuery sqlQuery;
    QueryExec(this,"SELECT performdate, devicename, defect, clients.surname, clients.name, clients.phone1, clients.address, users.name, users.phone1, users.phone2 FROM requests LEFT JOIN clients ON clients.id=requests.clientid LEFT JOIN users ON users.id=requests.performerid WHERE requests.id='"+num(requestId)+"';",sqlQuery);
    if(!sqlQuery.next())
        return;
    QString info;
    info+=tr("<h3>Заявка #%1</h3>").arg(num(requestId));
    info+=tr("<p><b>Дата:</b> %1</p>").arg(sqlQuery.value(0).toString());
    info+=tr("<p><b>Марка/модель:</b> %1</p>").arg(sqlQuery.value(1).toString());
    info+=tr("<p><b>Неисправность:</b> %1</p>").arg(sqlQuery.value(2).toString());
    info+=tr("<p><b>Клиент:</b> %1 %2 %3</p>").arg(sqlQuery.value(3).toString()).arg(sqlQuery.value(4).toString()).arg(sqlQuery.value(5).toString());
    info+=tr("<p><b>Адрес:</b> %1</p>").arg(sqlQuery.value(6).toString());
    info+=tr("<p><b>Мастер:</b> %1 %2 %3</p>").arg(sqlQuery.value(7).toString()).arg(sqlQuery.value(8).toString()).arg(sqlQuery.value(9).toString());
    ui->infoLabel->setText(info);
}
//-------------------------------------------------------------------------------------
void RemindDialog::on_okPushButton_clicked()
{
    accept();
}
//-------------------------------------------------------------------------------------
