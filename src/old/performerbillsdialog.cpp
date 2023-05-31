#include "performerbillsdialog.h"
#include "billdialog.h"
#include "ui_performerbillsdialog.h"
#include "common_inc.h"
#include "printdialog.h"

extern unsigned int globaluid;

//-------------------------------------------------------------------------------------
PerformerbillsDialog::PerformerbillsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PerformerbillsDialog)
{
    ui->setupUi(this);
    performerBillsQueryModel = new QSqlQueryModel;
    ui->performerBillsTableView->setModel(performerBillsQueryModel);
//    refreshPerformerBills();
    refreshPerformers();
}
//-------------------------------------------------------------------------------------
PerformerbillsDialog::~PerformerbillsDialog()
{
    delete ui;
}
//-------------------------------------------------------------------------------------
void PerformerbillsDialog::refreshPerformers()
{
    ui->performerComboBox->clear();
    ui->performerComboBox->addItem(tr("Все"),0);
    QSqlQuery performerSqlQuery;
    QueryExec(this,"SELECT id,name FROM users WHERE enabled>0 AND rights&'"+num(CSRIGHT_PERFORMER)+"';",performerSqlQuery);
    while(performerSqlQuery.next())
    {
        ui->performerComboBox->addItem(performerSqlQuery.value(1).toString(),performerSqlQuery.value(0));
    }
}
//-------------------------------------------------------------------------------------
void PerformerbillsDialog::on_performerComboBox_currentIndexChanged(int index)
{
    refreshPerformerBills();
}
//-------------------------------------------------------------------------------------
void PerformerbillsDialog::refreshPerformerBills()
{
    // F I L T E R
    QString filter;
    int performerid=ui->performerComboBox->currentData().toInt();
    if(performerid)
        filter+=" WHERE performerid='"+num(performerid)+"'";
    QModelIndex savedIndex=ui->performerBillsTableView->currentIndex();
    QueryModelExec(this,"SELECT performerbills.id AS pbid, performer.name AS pname, amount, users.name AS uname, performerbills.ctime AS pbctime FROM performerbills LEFT JOIN users ON users.id = performerbills.cuserid LEFT JOIN users AS performer ON performer.id = performerbills.performerid"+filter+";",performerBillsQueryModel);
    int column_cnt=0;
    performerBillsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("№"));
    performerBillsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Мастер"));
    performerBillsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Сумма"));
    performerBillsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Создатель"));
    performerBillsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Дата создания"));
    ui->performerBillsTableView->resizeColumnsToContents();
    ui->performerBillsTableView->setCurrentIndex(savedIndex);
}
//-------------------------------------------------------------------------------------
void PerformerbillsDialog::on_generatePushButton_clicked()
{
    // Получаем мастеров.
    QSqlQuery performersQuery;
    QueryExec(this,"SELECT id FROM users;",performersQuery);
    while(performersQuery.next())
    {
        int billid=0;
        double amount=0;
        int performerid=performersQuery.value(0).toInt();
//        QMessageBox::information(this,PROGNAME,tr("performer %1").arg(performerid));
        QSqlQuery performerRequestsQuery;
        QueryExec(this,"SELECT requests.id, cost, percent FROM requests WHERE performerid='"+num(performerid)+"' AND status='10' AND performerbillid<1;",performerRequestsQuery);
        while(performerRequestsQuery.next())
        {
            if(!billid)
            {
                QSqlQuery createBillQuery;
                if(!QueryExec(this,"INSERT INTO performerbills(performerid,amount,cuserid,ctime) VALUES('"+num(performerid)+"','0','"+num(globaluid)+"','"+timestamp()+"');",createBillQuery))
                    return;
                billid=createBillQuery.lastInsertId().toInt();
//                QMessageBox::information(this,PROGNAME,tr("create performerbills %1").arg(billid));
                if(!billid)
                    return;
            }
            amount+=performerRequestsQuery.value(1).toDouble()*performerRequestsQuery.value(2).toDouble()/100;
            if(!QueryExec(this,"UPDATE requests SET performerbillid='"+num(billid)+"' WHERE id='"+performerRequestsQuery.value(0).toString()+"';"))
                return;
        }
        if(billid)
        {
            if(!QueryExec(this,"UPDATE performerbills SET amount='"+num(amount)+"' WHERE id='"+num(billid)+"';"))
                return;
//            QMessageBox::information(this,PROGNAME,tr("update performerbills %1 amount %2").arg(billid).arg(amount));
        }

    }
    refreshPerformerBills();
}
//-------------------------------------------------------------------------------------
void PerformerbillsDialog::on_performerBillsTableView_activated(const QModelIndex &index)
{
    int id=performerBillsQueryModel->record(index.row()).field("pbid").value().toInt();
    if(!id)
        return;
    BillDialog *billDialog = new BillDialog(id,this);
    if(billDialog->exec()==QDialog::Accepted)
    {
        refreshPerformerBills();
    }
}
//-------------------------------------------------------------------------------------
/*void PerformerbillsDialog::on_printSelectedPushButton_clicked()
{
    QStringList selectedList=GetSelectedList("pbid",performerBillsQueryModel, ui->performerBillsTableView);
    if(selectedList.isEmpty())
        return;
    QFile file(QApplication::applicationDirPath()+"/bills.html");
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(this,PROGNAME,"Не могу открыть файл "+QApplication::applicationDirPath()+"/request.html");
        return;
    }
    QByteArray html=file.readAll();
    QByteArray tables;
    for(int i=0;i<selectedList.size();i++)
    {
        int billid=selectedList.at(i).toInt();
        // ЗАГОЛОВОК
        QSqlQuery billQuery;
        billQuery.setForwardOnly(true);
        if(!QueryExec(this,"SELECT performers.name AS pname, amount, users.name AS uname, performerbills.ctime AS pbctime FROM performerbills LEFT JOIN users ON users.id = performerbills.cuserid LEFT JOIN performers ON performers.id = performerbills.performerid WHERE performerbills.id='"+num(billid)+"';",billQuery))
            return;
        if(!billQuery.next())
            return;
        tables+=tr("<h4>Счёт &#8470; %1 от %2</h4>\r\n").arg(billid).arg(billQuery.value(3).toString());
        tables+=tr("<h4>Заказчик: %1</h4>\r\n").arg(billQuery.value(0).toString());
        // ТАБЛИЦА
        double total=0;
        QSqlQuery requestsQuery;
        QueryExec(this,"SELECT requests.id, requests.ctime, cost, clientcost, percent, collection FROM requests WHERE performerbillid='"+num(billid)+"';",requestsQuery);
        tables+=tr("<table border=\"1\">\r\n<tr><td>№ заявки</td><td>Дата создания</td><td>Сумма мастера</td><td>Сумма клиента</td><td>К оплате</td><td>Принято</td></tr>\r\n");
        while(requestsQuery.next())
        {
            QString collectedStr=tr("Нет");
            if(encodeCollected(requestsQuery.value(0).toInt()+int(requestsQuery.value(2).toDouble()))==requestsQuery.value(5).toString().toUInt())
                collectedStr=tr("Да");
            double toPay=requestsQuery.value(2).toDouble()*requestsQuery.value(4).toInt()/100;
            tables+="<tr><td>"+requestsQuery.value(0).toString()+"</td><td>"+requestsQuery.value(1).toString()+"</td><td>"+requestsQuery.value(2).toString()+"</td><td>"+requestsQuery.value(3).toString()+"</td><td>"+num(toPay)+"</td><td>"+collectedStr+"</td></tr>\r\n";
            total+=toPay;
        }
        tables+=tr("</table>\r\n");
        tables+=tr("<h4>Итог: %1</h4>\r\n").arg(total);
    }
    html.replace("[BILLS]",tables);

    PrintDialog *printDialog = new PrintDialog(html,this);
    printDialog->exec();
}*/
//-------------------------------------------------------------------------------------
void PerformerbillsDialog::on_printSelectedPushButton_clicked()
{
    QFile file(QApplication::applicationDirPath()+"/bills.html");
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(this,PROGNAME,"Не могу открыть файл "+QApplication::applicationDirPath()+"/request.html");
        return;
    }
    QByteArray html=file.readAll();
    QByteArray tables;
    // ЗАГОЛОВОК
    QSqlQuery performersQuery;
    performersQuery.setForwardOnly(true);
    if(!QueryExec(this,"SELECT id, name FROM users WHERE enabled>0;",performersQuery))
        return;
    while(performersQuery.next())
    {
        tables+=tr("<h4>Счёт от %1</h4>\r\n").arg(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss"));
        tables+=tr("<h4>Заказчик: %1</h4>\r\n").arg(performersQuery.value(1).toString());
        // ТАБЛИЦА
        double total=0;
        QSqlQuery requestsQuery;
        QueryExec(this,"SELECT requests.id, requests.ctime, cost, clientcost, percent, collection FROM requests WHERE performerid='"+num(performersQuery.value(0).toInt())+"';",requestsQuery);
        tables+=tr("<table border=\"1\">\r\n<tr><td>№ заявки</td><td>Дата создания</td><td>Сумма мастера</td><td>Сумма клиента</td><td>К оплате</td></tr>\r\n");
        while(requestsQuery.next())
        {
            if(encodeCollected(requestsQuery.value(0).toInt()+int(requestsQuery.value(2).toDouble()))!=requestsQuery.value(5).toString().toUInt())
            {
                double toPay=requestsQuery.value(2).toDouble()*requestsQuery.value(4).toInt()/100;
                tables+="<tr><td>"+requestsQuery.value(0).toString()+"</td><td>"+requestsQuery.value(1).toString()+"</td><td>"+requestsQuery.value(2).toString()+"</td><td>"+requestsQuery.value(3).toString()+"</td><td>"+num(toPay)+"</td></tr>\r\n";
                total+=toPay;
            }
        }
        tables+=tr("</table>\r\n");
        tables+=tr("<h4>Итог: %1</h4>\r\n").arg(total);
    }
    html.replace("[BILLS]",tables);

    PrintDialog *printDialog = new PrintDialog(html,this);
    printDialog->exec();
}
//-------------------------------------------------------------------------------------
