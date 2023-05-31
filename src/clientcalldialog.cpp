#include "clientcalldialog.h"
#include "ui_clientcalldialog.h"
#include "common_inc.h"
#include "agreementdialog.h"
#include "requestdialog.h"

//-------------------------------------------------------------------------------------
ClientCallDialog::ClientCallDialog(quint64 clientId, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ClientCallDialog),
    m_clientId(clientId)
{
    ui->setupUi(this);

    // Agreements.
    m_agreementsQueryModel = new QSqlQueryModel;
    ui->agreementsTableView->setModel(m_agreementsQueryModel);
    // Requests.
    m_requestsQueryModel = new QSqlQueryModel;
    ui->requestsTableView->setModel(m_requestsQueryModel);

    ui->callResultComboBox->addItem(tr("-"),0);
	ui->callResultComboBox->addItem(tr("Оформили ТО"),CALL_RESULT_MAINTENANCE_SCHEDULED);
	ui->callResultComboBox->addItem(tr("Думают"),CALL_RESULT_STULL);
	ui->callResultComboBox->addItem(tr("Отказались"),CALL_RESULT_REJECTED);
	ui->callResultComboBox->addItem(tr("Не звонить"),CALL_RESULT_DONT_CALL);
	ui->callResultComboBox->addItem(tr("Не отвечает"),CALL_RESULT_DOESNT_ANSWER);

    QSqlQuery clientQuery;
    QueryExec(this,"SELECT surname, name, phone1, phone2, address, call_result, next_call_date FROM clients WHERE id="+num(m_clientId)+";",clientQuery);
    if(!clientQuery.next())
    {
        QMetaObject::invokeMethod(this, "reject", Qt::QueuedConnection); // Close dialog.
        return;
    }
    ui->clientSurnameLabel->setText(clientQuery.value(0).toString());
    ui->clientNameLabel->setText(clientQuery.value(1).toString());
    QString phone1=clientQuery.value(2).toString();
    if(phone1.size()==12)
    {
        ui->clientPhone1Label->setText("<a href=\"tel:"+phone1+"\">"+phone1+"</a>");
    }
    else
    {
        ui->clientPhone1Label->setText(phone1);
    }
    QString phone2=clientQuery.value(3).toString();
    if(phone2.size()==12)
    {
        ui->clientPhone2Label->setText("<a href=\"tel:"+phone2+"\">"+phone2+"</a>");
    }
    else
    {
        ui->clientPhone2Label->setText(phone2);
    }
    ui->clientAddressLabel->setText(clientQuery.value(4).toString());
    ui->callResultComboBox->setCurrentIndex(ui->callResultComboBox->findData(clientQuery.value(5).toULongLong()));
    QString nextCallDateString=clientQuery.value(6).toString();
    if(!nextCallDateString.isEmpty())
    {
        ui->nextCallGroupBox->setChecked(true);
        ui->nextCallDateEdit->setDate(QDate::fromString(nextCallDateString,Qt::ISODate));
    }
    else
    {
        ui->nextCallDateEdit->setDate(QDate::currentDate());
    }

    refreshAgreements();
    refreshRequests();
}
//-------------------------------------------------------------------------------------
ClientCallDialog::~ClientCallDialog()
{
    delete ui;
}
//-------------------------------------------------------------------------------------
void ClientCallDialog::refreshAgreements()
{
	debug("refreshAgreements()");
    QModelIndex savedIndex=ui->agreementsTableView->currentIndex();
    QueryModelExec(this,"SELECT agreements.id AS agreementsid, agreementtypes.name AS agreementtypename, trests.name AS trestname, startdate, timelimit, CASE WHEN status = 1 THEN 'обслуживается' WHEN status = 2 THEN 'расторгнут' END AS status, cost, alertdate FROM agreements LEFT JOIN agreementtypes ON agreementtypes.id=agreements.type LEFT JOIN trests ON trests.id=agreements.trest WHERE client="+num(m_clientId)+";",m_agreementsQueryModel);
//    qDebug(agreementsQueryModel->query().lastQuery().toUtf8());
    int column_cnt=0;
    m_agreementsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("№"));
    m_agreementsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Тип"));
    m_agreementsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Трест"));
    m_agreementsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Дата заключения"));
    m_agreementsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Срок договора"));
    m_agreementsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Состояние"));
    m_agreementsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Стоимость"));
    m_agreementsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Следующее ТО"));
//    ui->agreementsTableView->resizeColumnsToContents();
    ui->agreementsTableView->setCurrentIndex(savedIndex);
}
//-------------------------------------------------------------------------------------
void ClientCallDialog::refreshRequests()
{
    QueryModelExec(this,"SELECT requests.id AS requestsid, requests.ctime, performdate, devicename, defect, cost, performer.name AS pname, users.name AS uname, states.name FROM requests LEFT JOIN states ON states.id=requests.status LEFT JOIN users ON users.id = requests.cuserid LEFT JOIN users AS performer ON performer.id = requests.performerid WHERE clientid="+num(m_clientId)+";",m_requestsQueryModel);
    int column_cnt=0;
    m_requestsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("№"));
    m_requestsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Дата создания"));
    m_requestsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Дата выполнения"));
    m_requestsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Марка,модель"));
    m_requestsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Неисправность"));
    m_requestsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Сумма мастера"));
    m_requestsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Мастер"));
    m_requestsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Диспетчер"));
    m_requestsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Состояние"));
    ui->requestsTableView->resizeColumnsToContents();
}
//-------------------------------------------------------------------------------------
void ClientCallDialog::on_savePushButton_clicked()
{
    QString nextCallDateString;
    if(ui->nextCallGroupBox->isChecked())
    {
        nextCallDateString=ui->nextCallDateEdit->date().toString(Qt::ISODate);
    }
    if(!QueryExec(this,"UPDATE clients SET call_result="+num(ui->callResultComboBox->currentData().toUInt())+", next_call_date='"+nextCallDateString+"' WHERE id="+num(m_clientId)+";"))
    {
        return;
    }
    accept();

}
//-------------------------------------------------------------------------------------
void ClientCallDialog::on_requestsTableView_activated(const QModelIndex &index)
{
    int id=m_requestsQueryModel->record(index.row()).field("requestsid").value().toInt();
    if(!id)
        return;
    RequestDialog *requestDialog = new RequestDialog(id,0,0,false,this);
    if(requestDialog->exec()==QDialog::Accepted)
    {
        refreshRequests();
    }
    delete requestDialog;
}
//-------------------------------------------------------------------------------------
void ClientCallDialog::on_agreementsTableView_activated(const QModelIndex &index)
{
    int id=m_agreementsQueryModel->record(index.row()).field("agreementsid").value().toInt();
    if(!id)
        return;
    AgreementDialog *agreementDialog = new AgreementDialog(id,this);
    if(agreementDialog->exec()==QDialog::Accepted)
    {
        refreshAgreements();
    }
    delete agreementDialog;
}
//-------------------------------------------------------------------------------------
