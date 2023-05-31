#include "billdialog.h"
#include "ui_billdialog.h"
//#include "colorsqlquerymodel.h"
#include "requestdialog.h"
#include "common_inc.h"

extern unsigned int globaluid;

//-------------------------------------------------------------------------------------
BillDialog::BillDialog(int billid,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BillDialog),m_billid(billid)
{
    ui->setupUi(this);
    // Requests.
    requestsQueryModel = new QSqlQueryModel;
    ui->requestsTableView->setModel(requestsQueryModel);
    if(!m_billid || !checkRights(this,globaluid,CSRIGHT_EXEC_ADMIN)) // Инкассация доступна только первым двум пользователям и только для существующего счета.
    {
        ui->collectComboBox->setEnabled(false);
    }
    if(m_billid)
    {
        QSqlQuery billQuery;
        QueryExec(this,"SELECT performer.name, amount, collected, users.name, performerbills.ctime FROM performerbills LEFT JOIN users AS performer ON performer.id=performerbills.performerid LEFT JOIN users ON users.id=performerbills.cuserid WHERE performerbills.id='"+num(m_billid)+"';",billQuery);
        if(!billQuery.next())
        {
            QMetaObject::invokeMethod(this, "reject", Qt::QueuedConnection); // Close dialog.
            return;
        }
        ui->idLineEdit->setText(num(m_billid));
        ui->performerLineEdit->setText(billQuery.value(0).toString());
        ui->amountDoubleSpinBox->setValue(billQuery.value(1).toDouble());
        unsigned int collected=billQuery.value(2).toUInt();
        ui->createInfoLabel->setText(tr("Создан: %1 %2").arg(billQuery.value(3).toString()).arg(billQuery.value(4).toString()));
        if(collected)// Если не пусто, значит была инкассация.
        {
            int collectedDecoded=encodeCollected(collected)-m_billid;
            ui->collectComboBox->setCurrentIndex(1);
            ui->collectComboBox->setItemText(1,tr("Принято ")+num(collectedDecoded));
            ui->collectComboBox->setItemData(1,collectedDecoded);
        }
        setWindowTitle(tr("Счёт %1").arg(m_billid));
        refreshRequests();
    }
}
//-------------------------------------------------------------------------------------
BillDialog::~BillDialog()
{
    delete ui;
}
//-------------------------------------------------------------------------------------
void BillDialog::refreshRequests()
{
    // S O R T
    QString fieldNameString= requestsQueryModel->record().fieldName(ui->requestsTableView->horizontalHeader()->sortIndicatorSection());
    if(fieldNameString.isEmpty())
        fieldNameString="requestsid";
    QString sortString=" ORDER BY "+fieldNameString;
    if(ui->requestsTableView->horizontalHeader()->sortIndicatorOrder()==Qt::DescendingOrder)
        sortString+=" DESC";
    // F I L T E R
    QString filter;
    filter+=QString((filter.isEmpty())?" WHERE":" AND")+" requests.performerbillid='"+num(m_billid)+"'";
//    QMessageBox::information(this,PROGNAME,tr("Фильтр: ")+filter);
    QModelIndex savedIndex=ui->requestsTableView->currentIndex();
    QueryModelExec(this,"SELECT requests.id AS requestsid, requests.ctime AS rctime, cost, clientcost, percent FROM requests"+filter+sortString+";",requestsQueryModel);
    int column_cnt=0;
    requestsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("№"));
    requestsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Дата создания"));
    requestsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Сумма мастера"));
    requestsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Сумма клиента"));
    requestsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Процент"));
    ui->requestsTableView->resizeColumnsToContents();
    ui->requestsTableView->setCurrentIndex(savedIndex);
    double total=0;
    for(int i=0;i<requestsQueryModel->rowCount();i++)
    {
        total+=requestsQueryModel->record(i).value(2).toDouble()*requestsQueryModel->record(i).value(4).toDouble()/100;
    }
    ui->amountDoubleSpinBox->setValue(total);// statusBar->showMessage(QString::number(total,'f',2));
}
//-------------------------------------------------------------------------------------
void BillDialog::on_okPushButton_clicked()
{
    unsigned int collected=0;
    if(m_billid)
    {
        if(ui->collectComboBox->currentIndex()==1 && checkRights(this,globaluid,CSRIGHT_EXEC_ADMIN))
        {
            if(ui->collectComboBox->currentData().toUInt()) // Если прочитанная инкассация была ненулевой, кодируем её.
                collected=encodeCollected(m_billid+ui->collectComboBox->currentData().toUInt());
            else
                collected=encodeCollected(m_billid+int(ui->amountDoubleSpinBox->value())); // Иначе создаём на имеющуюся сумму.
        }
        if(!QueryExec(this,"UPDATE performerbills SET collected='"+num(collected)+"' WHERE id='"+num(m_billid)+"';"))
            return;
        accept();
    }
}
//-------------------------------------------------------------------------------------
void BillDialog::on_requestsTableView_activated(const QModelIndex &index)
{
    int id=requestsQueryModel->record(index.row()).field("requestsid").value().toInt();
    if(!id)
        return;
    RequestDialog *requestDialog = new RequestDialog(id,0,0,false,this);
    if(requestDialog->exec()==QDialog::Accepted)
    {
        refreshRequests();
    }
}
//-------------------------------------------------------------------------------------
