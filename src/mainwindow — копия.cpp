#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "common_inc.h"
#include "selectconnectiondialog.h"
#include "requestdialog.h"
#include "usersdialog.h"
#include "optionsdialog.h"
#include "officesdialog.h"
#include "printdialog.h"
#include "eventsdialog.h"
#include "agreementdialog.h"
//#include "performerbillsdialog.h"
#include "reminddialog.h"
#include <QSound>
#include "clearamountsdialog.h"
#include "trestsdialog.h"
#include "clientcalldialog.h"

extern unsigned int globaluid;
extern QString globalConnectionName;
extern XMLSettings settings;

//-------------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QSettings registrySettings(ORGNAME,PROGNAME);
    restoreGeometry(registrySettings.value("cartgeometry").toByteArray());
    restoreState(registrySettings.value("cartstate").toByteArray());
    ui->requestsSplitter->restoreGeometry(registrySettings.value("rsplittergeometry").toByteArray());
    ui->requestsSplitter->restoreState(registrySettings.value("rsplitterstate").toByteArray());
    m_monthList << "Январь" << "Февраль" << "Март" << "Апрель" << "Май" << "Июнь" << "Июль" << "Август" << "Сентябрь" << "Октябрь" << "Ноябрь" << "Декабрь";
    // Dates.
    m_allTreeWidgetItem = new QTreeWidgetItem((QTreeWidget*)0, QStringList("Все"));
    ui->datesTreeWidget->addTopLevelItem(m_allTreeWidgetItem);
    // Requests.
    requestsQueryModel = new ColorSqlQueryModel;
    ui->requestsTableView->setModel(requestsQueryModel);
    ui->requestsTableView->horizontalHeader()->setSortIndicator(1,Qt::AscendingOrder);
    connect(ui->requestsTableView->horizontalHeader(),SIGNAL(sectionClicked(int)),this,SLOT(On_requestsHeaderView_sectionClicked()));
    connect(ui->requestsTableView->selectionModel(),SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),SLOT(On_requestsTableView_selectionChanged(const QItemSelection &, const QItemSelection &)));
    // Agreements.
    agreementsQueryModel = new AgreementsColorSqlQueryModel;
    ui->agreementsTableView->setModel(agreementsQueryModel);
    ui->agreementsTableView->horizontalHeader()->setSortIndicator(0,Qt::AscendingOrder);
    connect(ui->agreementsTableView->horizontalHeader(),SIGNAL(sectionClicked(int)),this,SLOT(On_agreementsHeaderView_sectionClicked()));
    // Clients.
    clientsQueryModel = new ClientsColorSqlQueryModel;
    ui->clientsTableView->setModel(clientsQueryModel);
// open db and refresh tables.
    if(!openDatabaseConnection())
    {
        QMetaObject::invokeMethod(this, "close", Qt::QueuedConnection); // Close MainWindow
        return;
    }
    m_adminFlag=checkRights(this,globaluid,CSRIGHT_EXEC_ADMIN);
    requestsQueryModel->setAdminFlag(m_adminFlag);
    if(!m_adminFlag)
    {
        ui->usersAction->setVisible(false);
        ui->optionsAction->setVisible(false);
        ui->officesAction->setVisible(false);
        ui->trestsAction->setVisible(false);
        ui->backupAction->setVisible(false);
        ui->eventsAction->setVisible(false);
        ui->performerBillsAction->setVisible(false);
        ui->timesheetAction->setVisible(false);
        ui->resetAmountsAction->setVisible(false);
        ui->copySelectedAction->setVisible(false);
    }
    refreshTypeComboBox();
    refreshTrestComboBox();
    refreshDocumentsButtons();
    // Timers.
    connect(&m_connectionTimer,SIGNAL(timeout()),SLOT(On_connectionTimer_timeout()));
    m_connectionTimer.start(1000);
    if(settings.value("interface","","reminder-alerts","","").toInt())
    {
        connect(&m_remindTimer,SIGNAL(timeout()),SLOT(On_remindTimer_timeout()));
        m_remindTimer.start(600000);// 600'000=10 min
    }
// С Т О Л Б Ц Ы
    ui->agreementsTableView->horizontalHeader()->restoreState(registrySettings.value("agreements_table_state").toByteArray());
    ui->requestsTableView->horizontalHeader()->restoreState(registrySettings.value("requeststablestate").toByteArray());

    ui->columnCreateDateAction->setChecked(!ui->requestsTableView->isColumnHidden(1));
    ui->columnPerformDateAction->setChecked(!ui->requestsTableView->isColumnHidden(2));
    ui->columnAddressAction->setChecked(!ui->requestsTableView->isColumnHidden(3));
    ui->columnSurnameAction->setChecked(!ui->requestsTableView->isColumnHidden(4));
    ui->columnNameAction->setChecked(!ui->requestsTableView->isColumnHidden(5));
    ui->columnPhone1Action->setChecked(!ui->requestsTableView->isColumnHidden(6));
    ui->columnPhone2Action->setChecked(!ui->requestsTableView->isColumnHidden(7));
//    ui->columnDeviceTypeAction->setChecked(!ui->requestsTableView->isColumnHidden(8));
    ui->columnDeviceNameAction->setChecked(!ui->requestsTableView->isColumnHidden(9));
    ui->columnDefectAction->setChecked(!ui->requestsTableView->isColumnHidden(10));
    ui->columnCostAction->setChecked(!ui->requestsTableView->isColumnHidden(11));
    ui->columnPerformerAction->setChecked(!ui->requestsTableView->isColumnHidden(12));
    ui->columnCreatorAction->setChecked(!ui->requestsTableView->isColumnHidden(13));
    ui->columnStatusAction->setChecked(!ui->requestsTableView->isColumnHidden(14));
    ui->columnCollectionAction->setChecked(!ui->requestsTableView->isColumnHidden(15));

    QueryExec(this,"INSERT INTO events(actionid,userid,ctime) VALUES('"+num(ACTIONID_LOGIN_KCRM)+"','"+num(globaluid)+"','"+timestamp()+"');");
}
//-------------------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    clearDatabaseConnections();
    delete ui;
}
//-------------------------------------------------------------------------------------
void MainWindow::closeEvent(QCloseEvent *)
{
    QueryExec(this,"INSERT INTO events(actionid,userid,ctime) VALUES('"+num(ACTIONID_LOGOUT_KCRM)+"','"+num(globaluid)+"','"+timestamp()+"');");
    QSettings registrySettings(ORGNAME,PROGNAME);
    registrySettings.setValue("rsplittergeometry", ui->requestsSplitter->saveGeometry());
    registrySettings.setValue("rsplitterstate", ui->requestsSplitter->saveState());
    registrySettings.setValue("cartgeometry", saveGeometry());
    registrySettings.setValue("cartstate", saveState());
    registrySettings.setValue("requeststablestate",ui->requestsTableView->horizontalHeader()->saveState());
    registrySettings.setValue("agreements_table_state",ui->agreementsTableView->horizontalHeader()->saveState());
}
//-------------------------------------------------------------------------------------
void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    if(!checkRights(this,globaluid,CSRIGHT_EXEC_ADMIN))
        return;
//	QMessageBox::warning(this,PROGNAME,"carTableView x="+QString::number(ui.cartTableView->geometry().x())+" y="+QString::number(ui.cartTableView->geometry().y())+" event x="+QString::number(event->pos().x())+" y="+QString::number(event->pos().y()));
    if(ui->requestsTableView->hasFocus())
    {
        QMenu menu(this);
        menu.addAction(ui->copySelectedAction);
        menu.exec(event->globalPos());
    }
}
//-------------------------------------------------------------------------------------
void MainWindow::On_connectionTimer_timeout()
{
    connectionTimer();
}
//-------------------------------------------------------------------------------------
void MainWindow::On_remindTimer_timeout()
{
//    qDebug("remind timer");
    QSqlQuery sqlQuery;
    QDateTime currentDateTime=QDateTime::currentDateTime();
	bool remindSound=false;
//    QString userFilter=" cuserid='"+num(globaluid)+"' AND";
    QueryExec(this,"SELECT id FROM requests WHERE performdate>='"+currentDateTime.toString("yyyy-MM-dd HH:mm")+"' AND performdate<'"+currentDateTime.addSecs(3600).toString("yyyy-MM-dd HH:mm")+"';",sqlQuery);
    while(sqlQuery.next())
    {
        quint64 requestId=sqlQuery.value(0).toULongLong();
        bool finded=false;
        for(int i=0;i<remindRequestIdList.size();i++)
        {
            if(remindRequestIdList.at(i)==requestId)
            {
                finded=true;
                break;
            }
        }
        if(finded)
            continue;
//        qDebug("finded remind");
        remindRequestIdList.append(requestId);
        RemindDialog *remindDialog = new RemindDialog(0,Qt::WindowStaysOnTopHint);
        remindDialog->loadRequestInfo(requestId);
        remindDialog->show();
		remindSound=true;
    }
	if(remindSound)
		QSound::play(programDataPath()+"/remind.mp3");
}
//-------------------------------------------------------------------------------------
bool MainWindow::openDatabaseConnection()
{
    // Select connection, check password and rights.
    SelectConnectionDialog *selectConnectionDialog = new SelectConnectionDialog(CSRIGHT_EXEC_CART,this);
    if(selectConnectionDialog->exec()!=QDialog::Accepted)
    {
        delete selectConnectionDialog;
        return false;
    }
    delete selectConnectionDialog;
    // Refresh.
    setWindowTitle("Kcrm - "+getUserName(this,globaluid));
    bool dtwBlock=ui->datesTreeWidget->blockSignals(true);
    // Dates.
    refreshDates();
    on_todayPushButton_clicked();
    ui->datesTreeWidget->blockSignals(dtwBlock);
    bool oBlock=ui->officeComboBox->blockSignals(true);
    refreshOffices();
    ui->officeComboBox->blockSignals(oBlock);
    bool pBlock=ui->performerComboBox->blockSignals(true);
    refreshPerformers();
    ui->performerComboBox->blockSignals(pBlock);
    refreshRequests();
    return true;
}
//-------------------------------------------------------------------------------------
void MainWindow::refreshOffices()
{
    ui->officeComboBox->clear();
    ui->officeComboBox->addItem(tr("Все"),0);
    QSqlQuery officeSqlQuery;
    QueryExec(this,"SELECT id,name FROM offices WHERE enabled>0 ORDER BY name;",officeSqlQuery);
    while(officeSqlQuery.next())
    {
        ui->officeComboBox->addItem(officeSqlQuery.value(1).toString(),officeSqlQuery.value(0));
    }
}
//-------------------------------------------------------------------------------------
void MainWindow::refreshPerformers()
{
    ui->performerComboBox->clear();
    ui->performerComboBox->addItem(tr("Все"),0);
    // Office filter.
    QString officeLeftJoin, officeFilter;
    int currentOffice=ui->officeComboBox->currentData(Qt::UserRole).toInt();
    if(currentOffice>0)
    {
        officeLeftJoin=" LEFT JOIN performeroffices ON performeroffices.performerid=users.id";
        officeFilter=" AND performeroffices.officeid='"+num(currentOffice)+"'";
    }

    QSqlQuery performerSqlQuery;
    QueryExec(this,"SELECT users.id,users.name FROM users"+officeLeftJoin+" WHERE users.enabled>0 AND users.rights&'"+num(CSRIGHT_PERFORMER)+"'"+officeFilter+" ORDER BY users.name;",performerSqlQuery);
    while(performerSqlQuery.next())
    {
        ui->performerComboBox->addItem(performerSqlQuery.value(1).toString(),performerSqlQuery.value(0));
    }
}
//-------------------------------------------------------------------------------------
void MainWindow::refreshDates()
{
//    ui->datesTreeWidget->clear();
    QSqlQuery dateQuery;
    QueryExec(this,"SELECT MIN(performdate) FROM requests;",dateQuery);
    if(!dateQuery.next())
        return;
    QString dateStr=dateQuery.value(0).toString();
    dateStr.truncate(10);
    QDate date=QDate::fromString(dateStr,"yyyy-MM-dd");
    if(!date.isValid())
        return;
    QueryExec(this,"SELECT MAX(performdate) FROM requests;",dateQuery);
    if(!dateQuery.next())
        return;
    QString lastDateStr=dateQuery.value(0).toString();
    lastDateStr.truncate(10);
    m_lastPerformDate=QDate::fromString(lastDateStr,"yyyy-MM-dd");
    QDate currentDate=QDate::currentDate();
    if(currentDate>m_lastPerformDate)
        m_lastPerformDate=currentDate;

    while(date<=m_lastPerformDate)
    {
        // Ищем год
        QTreeWidgetItem *yearTreeWidgetItem=0;
        for (int i = 0; i < m_allTreeWidgetItem->childCount(); i++)
        {
            if(m_allTreeWidgetItem->child(i)->data(0,Qt::UserRole).toString()==date.toString("yyyy"))
                yearTreeWidgetItem=m_allTreeWidgetItem->child(i);
        }
        if(!yearTreeWidgetItem)// Добавляем год
        {
            yearTreeWidgetItem = new QTreeWidgetItem(m_allTreeWidgetItem, QStringList(date.toString("yyyy")));
            yearTreeWidgetItem->setData(0,Qt::UserRole,date.toString("yyyy"));
        }

        // Ищем месяц
        QTreeWidgetItem *monthTreeWidgetItem=0;
        for (int i = 0; i < yearTreeWidgetItem->childCount(); i++)
        {
            if(yearTreeWidgetItem->child(i)->data(0,Qt::UserRole).toString()==date.toString("yyyy-MM"))
                monthTreeWidgetItem=yearTreeWidgetItem->child(i);
        }
        if(!monthTreeWidgetItem)// Добавляем месяц
        {
            monthTreeWidgetItem = new QTreeWidgetItem(yearTreeWidgetItem, QStringList(m_monthList.at(date.month()-1)));
            monthTreeWidgetItem->setData(0,Qt::UserRole,date.toString("yyyy-MM"));
        }
        // Ищем день
        QTreeWidgetItem *dayTreeWidgetItem=0;
        for (int i = 0; i < monthTreeWidgetItem->childCount(); i++)
        {
            if(monthTreeWidgetItem->child(i)->data(0,Qt::UserRole).toString()==date.toString("yyyy-MM-dd"))
                dayTreeWidgetItem=monthTreeWidgetItem->child(i);
//            qDebug(QString("monthTreeWidget "+monthTreeWidgetItem->child(i)->data(0,Qt::UserRole).toString()+" date "+date.toString("dd")).toUtf8());
        }
        if(!dayTreeWidgetItem)// Добавляем день
        {
            dayTreeWidgetItem = new QTreeWidgetItem(monthTreeWidgetItem, QStringList(date.toString("dd")));
            dayTreeWidgetItem->setData(0,Qt::UserRole,date.toString("yyyy-MM-dd"));
        }

//        QTreeWidgetItem *dayTreeWidgetItem=new QTreeWidgetItem((QTreeWidget*)0, QStringList(date.toString("dd")));
//        dayTreeWidgetItem->setData(0,Qt::UserRole,date.toString("yyyy-MM-dd"));
//        monthTreeWidgetItem->addChild(dayTreeWidgetItem);
        date=date.addDays(1);
    }
}
//-------------------------------------------------------------------------------------
/*void MainWindow::appendDates(QDate performDate)
{
    if(performDate<=m_lastPerformDate)
        return;
    QDate date=m_lastPerformDate;
    date.addDays(1);// Начинаем добавлять со следующего дня.
    m_lastPerformDate=performDate;
    while(date<=m_lastPerformDate)
    {
        // Ищем год
        QTreeWidgetItem *yearTreeWidgetItem=0;
        for (int i = 0; i < m_allTreeWidgetItem->childCount(); i++)
        {
            if(m_allTreeWidgetItem->child(i)->data(0,Qt::UserRole).toString()==date.toString("yyyy"))
                yearTreeWidgetItem=m_allTreeWidgetItem->child(i);
        }
        if(!yearTreeWidgetItem)// Добавляем год
        {
            yearTreeWidgetItem = new QTreeWidgetItem(m_allTreeWidgetItem, QStringList(date.toString("yyyy")));
            yearTreeWidgetItem->setData(0,Qt::UserRole,date.toString("yyyy"));
        }

        // Ищем месяц
        QTreeWidgetItem *monthTreeWidgetItem=0;
        for (int i = 0; i < yearTreeWidgetItem->childCount(); i++)
        {
            if(yearTreeWidgetItem->child(i)->data(0,Qt::UserRole).toString()==date.toString("yyyy-MM"))
                monthTreeWidgetItem=yearTreeWidgetItem->child(i);
        }
        if(!monthTreeWidgetItem)// Добавляем месяц
        {
            monthTreeWidgetItem = new QTreeWidgetItem(yearTreeWidgetItem, QStringList(m_monthList.at(date.month()-1)));
            monthTreeWidgetItem->setData(0,Qt::UserRole,date.toString("yyyy-MM"));
        }
        QTreeWidgetItem *dayTreeWidgetItem=new QTreeWidgetItem((QTreeWidget*)0, QStringList(date.toString("dd")));
        dayTreeWidgetItem->setData(0,Qt::UserRole,date.toString("yyyy-MM-dd"));
        monthTreeWidgetItem->addChild(dayTreeWidgetItem);
        date=date.addDays(1);
    }
}*/
//-------------------------------------------------------------------------------------
void MainWindow::on_todayPushButton_clicked()
{
    QString currentDate=QDate::currentDate().toString("yyyy-MM-dd");

    QList<QTreeWidgetItem *> items=ui->datesTreeWidget->findItems("",Qt::MatchContains|Qt::MatchRecursive);
    foreach (QTreeWidgetItem * item, items)
    {
        if(item->data(0,Qt::UserRole).toString()==currentDate)
        {
            ui->datesTreeWidget->setCurrentItem(item);
            return;
        }
    }
}
//-------------------------------------------------------------------------------------
void MainWindow::refreshRequests()
{
    qDebug()<<"refreshRequests()";
    // S O R T
    QString fieldNameString= requestsQueryModel->record().fieldName(ui->requestsTableView->horizontalHeader()->sortIndicatorSection());
    if(fieldNameString.isEmpty())
        fieldNameString="requestsid";
    QString sortString=" ORDER BY "+fieldNameString;
    if(ui->requestsTableView->horizontalHeader()->sortIndicatorOrder()==Qt::DescendingOrder)
        sortString+=" DESC";
    // F I L T E R
    QString filter;
    // Date.
    QTreeWidgetItem *currentTreeWidgetItem=ui->datesTreeWidget->currentItem();
    if(currentTreeWidgetItem)
    {
        QString dateStr=currentTreeWidgetItem->data(0,Qt::UserRole).toString();
        if(!dateStr.isEmpty())
            filter+=QString((filter.isEmpty())?" WHERE":" AND")+" requests.performdate LIKE '"+dateStr+"%'";
    }
    // Office.
    int currentOffice=ui->officeComboBox->currentData(Qt::UserRole).toInt();
    if(currentOffice>0)
        filter+=QString((filter.isEmpty())?" WHERE":" AND")+" requests.officeid='"+num(currentOffice)+"'";
    // Performer.
    int currentPerformer=ui->performerComboBox->currentData(Qt::UserRole).toInt();
    if(currentPerformer>0)
        filter+=QString((filter.isEmpty())?" WHERE":" AND")+" requests.performerid='"+num(currentPerformer)+"'";
    // FilterLineEdit
    QString filterLineEdit=escapeQuotes(ui->filterLineEdit->text());
    if(!filterLineEdit.isEmpty())
    {
        filter+=QString((filter.isEmpty())?" WHERE":" AND")+" (requests.id='"+filterLineEdit+"' OR clients.surname LIKE '%"+filterLineEdit+"%' OR clients.name LIKE '%"+filterLineEdit+"%' OR clients.address LIKE '%"+filterLineEdit+"%' OR clients.phone1 LIKE '%"+filterLineEdit+"%' OR clients.phone2 LIKE '%"+filterLineEdit+"%')";
    }
//    QMessageBox::information(this,PROGNAME,tr("Фильтр: ")+filter);
    QModelIndex savedIndex=ui->requestsTableView->currentIndex();
    QueryModelExec(this,"SELECT requests.id AS requestsid, requests.ctime AS rctime, performdate, clients.address, clients.surname, clients.name AS cname, clients.phone1, clients.phone2, devicename, defect, cost, performer.name AS pname, users.name AS uname, states.name, requests.collection FROM requests LEFT JOIN states ON states.id=requests.status LEFT JOIN clients ON clients.id = requests.clientid LEFT JOIN users ON users.id = requests.cuserid LEFT JOIN users AS performer ON performer.id = requests.performerid"+filter+""+sortString+";",requestsQueryModel);
    int column_cnt=0;
    requestsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("№"));
    requestsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Дата создания"));
    requestsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Дата выполнения"));
    requestsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Адрес"));
    requestsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Фамилия"));
    requestsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Имя"));
    requestsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Телефон 1"));
    requestsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Телефон 2"));
    requestsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Марка,модель"));
    requestsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Неисправность"));
    requestsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Сумма мастера"));
    requestsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Мастер"));
    requestsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Диспетчер"));
    requestsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Состояние"));
    requestsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Инкассация"));
//    ui->requestsTableView->resizeColumnsToContents();
    ui->requestsTableView->setCurrentIndex(savedIndex);
    if(m_adminFlag)
    {
        double total=0;
        for(int i=0;i<requestsQueryModel->rowCount();i++)
        {
            total+=requestsQueryModel->record(i).value(10).toDouble();
        }
        ui->statusBar->showMessage(QString::number(total,'f',2));
    }
//    qDebug("refresh requests");
}
//-------------------------------------------------------------------------------------
void MainWindow::refreshAgreements()
{
    qDebug()<<"refreshAgreements()";
    // S O R T
    QString fieldNameString= agreementsQueryModel->record().fieldName(ui->agreementsTableView->horizontalHeader()->sortIndicatorSection());
    if(fieldNameString.isEmpty())
        fieldNameString="agreementsid";
    QString sortString=" ORDER BY "+fieldNameString;
    if(ui->agreementsTableView->horizontalHeader()->sortIndicatorOrder()==Qt::DescendingOrder)
        sortString+=" DESC";
    // F I L T E R
    QString filter;
    if(!ui->showTerminatedAgreementsCheckBox->isChecked())
        filter+=QString((filter.isEmpty())?" WHERE":" AND")+" agreements.status<>'2'";
    // Office.
    int currentOffice=ui->officeComboBox->currentData(Qt::UserRole).toInt();
    if(currentOffice>0)
        filter+=QString((filter.isEmpty())?" WHERE":" AND")+" agreements.officeid='"+num(currentOffice)+"'";
    // Type
    if(ui->agrFilterTypeComboBox->currentData().toInt()>0)
        filter+=QString((filter.isEmpty())?" WHERE":" AND")+" agreements.type='"+num(ui->agrFilterTypeComboBox->currentData().toInt())+"'";
    // Trest
    if(ui->agrFilterTrestComboBox->currentData().toInt()>0)
        filter+=QString((filter.isEmpty())?" WHERE":" AND")+" agreements.trest='"+num(ui->agrFilterTrestComboBox->currentData().toInt())+"'";
    // FilterLineEdit
    QString filterLineEdit=escapeQuotes(ui->filterLineEdit->text());
    if(!filterLineEdit.isEmpty())
    {
        filter+=QString((filter.isEmpty())?" WHERE":" AND")+" (agreements.id='"+filterLineEdit+"' OR clients.surname LIKE '%"+filterLineEdit+"%' OR clients.name LIKE '%"+filterLineEdit+"%' OR clients.phone1 LIKE '%"+filterLineEdit+"%' OR clients.phone2 LIKE '%"+filterLineEdit+"%' OR clients.address LIKE '%"+filterLineEdit+"%')";
    }
//    QMessageBox::information(this,PROGNAME,tr("Фильтр: ")+filter);
    QModelIndex savedIndex=ui->agreementsTableView->currentIndex();
    QueryModelExec(this,"SELECT agreements.id AS agreementsid, agreementtypes.name AS agreementtypename, trests.name AS trestname, startdate, timelimit, CASE WHEN status = 1 THEN 'обслуживается' WHEN status = 2 THEN 'расторгнут' END AS status, cost, clients.surname, clients.name AS clientname, clients.address, alertdate, collected FROM agreements LEFT JOIN clients ON clients.id = agreements.client LEFT JOIN users ON users.id = agreements.cuser LEFT JOIN agreementtypes ON agreementtypes.id=agreements.type LEFT JOIN trests ON trests.id=agreements.trest"+filter+sortString+";",agreementsQueryModel);
//    qDebug(agreementsQueryModel->query().lastQuery().toUtf8());
    int column_cnt=0;
    agreementsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("№"));
    agreementsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Тип"));
    agreementsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Трест"));
    agreementsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Дата заключения"));
    agreementsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Срок договора"));
    agreementsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Состояние"));
    agreementsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Стоимость"));
    agreementsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Фамилия"));
    agreementsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Имя"));
    agreementsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Адрес"));
    agreementsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Следующее ТО"));
    agreementsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Инкассация"));
//    agreementsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Отметка о выполнении"));
//    ui->agreementsTableView->hideColumn(7);
//    agreementsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Мастер"));
//    agreementsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Стоимость"));
//    agreementsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Мастер"));
//    agreementsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Диспетчер"));
//    agreementsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Состояние"));
//    agreementsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Инкассация"));
//    ui->agreementsTableView->resizeColumnsToContents();
    ui->agreementsTableView->setCurrentIndex(savedIndex);
    ui->statusBar->showMessage("");
}
//-------------------------------------------------------------------------------------
void MainWindow::refreshClients()
{
    qDebug()<<"refreshClients()";
    // S O R T
/*    QString fieldNameString= clientsQueryModel->record().fieldName(ui->clientsTableView->horizontalHeader()->sortIndicatorSection());
    if(fieldNameString.isEmpty())
        fieldNameString="clientid";
    QString sortString=" ORDER BY "+fieldNameString;
    if(ui->clientsTableView->horizontalHeader()->sortIndicatorOrder()==Qt::DescendingOrder)
        sortString+=" DESC";*/
    // F I L T E R
    QString filter;
/*    if(!ui->showTerminatedAgreementsCheckBox->isChecked())
        filter+=QString((filter.isEmpty())?" WHERE":" AND")+" agreements.status<>'2'";
    // Office.
    int currentOffice=ui->officeComboBox->currentData(Qt::UserRole).toInt();
    if(currentOffice>0)
        filter+=QString((filter.isEmpty())?" WHERE":" AND")+" agreements.officeid='"+num(currentOffice)+"'";
    // Type
    if(ui->agrFilterTypeComboBox->currentData().toInt()>0)
        filter+=QString((filter.isEmpty())?" WHERE":" AND")+" agreements.type='"+num(ui->agrFilterTypeComboBox->currentData().toInt())+"'";
    // Trest
    if(ui->agrFilterTrestComboBox->currentData().toInt()>0)
        filter+=QString((filter.isEmpty())?" WHERE":" AND")+" agreements.trest='"+num(ui->agrFilterTrestComboBox->currentData().toInt())+"'";
    // FilterLineEdit
    QString filterLineEdit=escapeQuotes(ui->filterLineEdit->text());
    if(!filterLineEdit.isEmpty())
    {
        filter+=QString((filter.isEmpty())?" WHERE":" AND")+" (agreements.id='"+filterLineEdit+"' OR clients.surname LIKE '%"+filterLineEdit+"%' OR clients.name LIKE '%"+filterLineEdit+"%' OR clients.phone1 LIKE '%"+filterLineEdit+"%' OR clients.phone2 LIKE '%"+filterLineEdit+"%' OR clients.address LIKE '%"+filterLineEdit+"%')";
    }*/
    QModelIndex savedIndex=ui->clientsTableView->currentIndex();
    QueryModelExec(this,"SELECT clients.id AS clientid, clients.surname AS clientsurname, clients.name AS clientname, clients.phone1, clients.address, CASE WHEN call_result = 0 THEN '-' WHEN call_result = 1 THEN 'Оформили ТО' WHEN call_result = 2 THEN 'Думают' WHEN call_result = 3 THEN 'Отказались' WHEN call_result = 4 THEN 'Не звонить' END AS call_result_text, next_call_date, call_result FROM clients"+filter/*+sortString*/+" ORDER BY call_result, next_call_date, id;",clientsQueryModel);
//    qDebug(clientsQueryModel->query().lastQuery().toUtf8());
    int column_cnt=0;
    clientsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("№"));
    clientsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Фамилия"));
    clientsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Имя"));
    clientsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Телефон 1"));
    clientsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Адрес"));
    clientsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Результат звонка"));
    clientsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Позвонить"));
    ui->clientsTableView->hideColumn(column_cnt);
    ui->clientsTableView->setCurrentIndex(savedIndex);
    ui->clientsTableView->resizeColumnsToContents();
    ui->statusBar->showMessage("");
}
//-------------------------------------------------------------------------------------
void MainWindow::on_newRequestPushButton_clicked()
{
    RequestDialog *requestDialog = new RequestDialog(0,ui->officeComboBox->currentData(Qt::UserRole).toInt(),0,false,this);
    if(requestDialog->exec()==QDialog::Accepted)
    {
        refreshRequests();
        refreshDates();
    }
/*    RequestDialog requestDialog(0,ui->officeComboBox->currentData(Qt::UserRole).toInt(),0,false,this);
    if(requestDialog.exec()==QDialog::Accepted)
    {
        refreshRequests();
        refreshDates();
    }*/
}
//-------------------------------------------------------------------------------------
void MainWindow::on_officeComboBox_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    if(ui->tabWidget->currentIndex()==0)
    {
        bool performerComboBlock=ui->performerComboBox->blockSignals(true);
        ui->performerComboBox->setCurrentIndex(0);
        refreshPerformers();
        ui->performerComboBox->blockSignals(performerComboBlock);
        refreshRequests();
    }
    if(ui->tabWidget->currentIndex()==1)
        refreshAgreements();
}
//-------------------------------------------------------------------------------------
void MainWindow::on_performerComboBox_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    refreshRequests();
}
//-------------------------------------------------------------------------------------
void MainWindow::on_requestsTableView_activated(const QModelIndex &index)
{
    int id=requestsQueryModel->record(index.row()).field("requestsid").value().toInt();
    if(!id)
        return;
    RequestDialog *requestDialog = new RequestDialog(id,0,0,false,this);
    if(requestDialog->exec()==QDialog::Accepted)
    {
        refreshRequests();
        refreshDates();
    }
    delete requestDialog;
}
//-------------------------------------------------------------------------------------
void MainWindow::On_requestsTableView_selectionChanged(const QItemSelection & selected, const QItemSelection & deselected)
{
    Q_UNUSED(deselected);
    ui->requestInfoTextEdit->clear();
    if(!selected.indexes().size())
        return;
    int id=requestsQueryModel->record(selected.indexes().at(0).row()).field("requestsid").value().toInt();
    if(!id)
        return;
// Выводим инфо
    QSqlQuery requestQuery;
    QueryExec(this,"SELECT requests.id, requests.ctime, performdate, clients.address, clients.surname, clients.name, clients.phone1, clients.phone2, devicename, defect, cost, creator.name, offices.name, performer.name, performer.phone1 FROM requests LEFT JOIN offices ON offices.id=requests.officeid LEFT JOIN clients ON clients.id = requests.clientid LEFT JOIN users AS creator ON creator.id = requests.cuserid LEFT JOIN users AS performer ON performer.id = requests.performerid WHERE requests.id='"+num(id)+"';",requestQuery);
    if(!requestQuery.next())
        return;
    QString text=tr("Заявка <b>%1</b><br>").arg(requestQuery.value(0).toString());
    text+=tr("Дата приёма: <b>%1</b><br>").arg(requestQuery.value(1).toString());
    text+=tr("Дата выполнения: <b>%1</b><br>").arg(requestQuery.value(2).toString());
    text+=tr("Адрес: <b>%1</b><br>").arg(requestQuery.value(3).toString());
    text+=tr("Клиент: <b>%1 %2</b><br>").arg(requestQuery.value(4).toString()).arg(requestQuery.value(5).toString());
    text+=tr("Телефоны: <b>%1 %2</b><br>").arg(requestQuery.value(6).toString()).arg(requestQuery.value(7).toString());
//    text+=tr("Тип: <b>%1</b><br>").arg(requestQuery.value(8).toString());
    text+=tr("Модель: <b>%1</b><br>").arg(requestQuery.value(8).toString());
    text+=tr("Неисправность: <b>%1</b><br>").arg(requestQuery.value(9).toString());
    text+=tr("Стоимость: <b>%1</b><br>").arg(requestQuery.value(10).toString());
    text+=tr("Диспетчер: <b>%1</b><br>").arg(requestQuery.value(11).toString());
    text+=tr("Офис: <b>%1</b><br>").arg(requestQuery.value(12).toString());
    text+=tr("Мастер: <b>%1, %2</b><br>").arg(requestQuery.value(13).toString()).arg(requestQuery.value(14).toString());

    ui->requestInfoTextEdit->setHtml(text);
}
//-------------------------------------------------------------------------------------
void MainWindow::On_requestsHeaderView_sectionClicked()
{ // Сортировка по выбранной колонке.
    refreshRequests();
}
//-------------------------------------------------------------------------------------
void MainWindow::On_agreementsHeaderView_sectionClicked()
{ // Сортировка по выбранной колонке.
    refreshAgreements();
}
//-------------------------------------------------------------------------------------
void MainWindow::on_datesTreeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    if(current==0)
        return;
    current->setExpanded(true);
    refreshRequests();
}
//-------------------------------------------------------------------------------------
void MainWindow::on_refreshAction_triggered()
{
    refreshRequests();
}
//-------------------------------------------------------------------------------------
/*void MainWindow::on_performersAction_triggered()
{
    PerformersDialog *performersDialog = new PerformersDialog(this);
    performersDialog->exec();
    delete performersDialog;
}*/
//-------------------------------------------------------------------------------------
void MainWindow::on_optionsAction_triggered()
{
    OptionsDialog *optionsDialog = new OptionsDialog(this);
    optionsDialog->exec();
    delete optionsDialog;
}
//-------------------------------------------------------------------------------------
void MainWindow::on_officesAction_triggered()
{
    OfficesDialog *officesDialog = new OfficesDialog(this);
    officesDialog->exec();
    delete officesDialog;
}
//-------------------------------------------------------------------------------------
void MainWindow::on_trestsAction_triggered()
{
    TrestsDialog *trestsDialog = new TrestsDialog(this);
    trestsDialog->exec();
    delete trestsDialog;
}
//-------------------------------------------------------------------------------------
void MainWindow::on_copySelectedAction_triggered()
{
/*    if(ui->tabWidget->currentIndex()==0)// Requests
    {
        QModelIndexList selected=ui->requestsTableView->selectionModel()->selectedRows();
        int fields=16;
        QString bufferString;
        for(int i=0;i<selected.size();i++)
        {
            int id = requestsQueryModel->record(selected.at(i).row()).field(0).value().toInt();
            double cost = requestsQueryModel->record(selected.at(i).row()).field(11).value().toDouble();
            unsigned int collection = requestsQueryModel->record(selected.at(i).row()).field(fields-1).value().toUInt();
            QString collectionStr;
            if(unsigned((id+int(cost))^0xE3649BDC)==collection)
                collectionStr=tr("Принято");
            for(int field=0;field<fields-1;field++)
            {
                QString separator="	";
                bufferString.append(requestsQueryModel->record(selected.at(i).row()).field(field).value().toString()+separator);
            }
            bufferString.append(collectionStr+"\r\n");

        }
        QApplication::clipboard()->setText(bufferString);
    }
*/
//    qDebug("copy start");

    QSqlQueryModel *sqlQueryModel=0;
    QTableView *tableView=0;
    QString copyString;
    if(ui->tabWidget->currentIndex()==0)
    {
//        qDebug("copy requests");
        sqlQueryModel=requestsQueryModel;
        tableView=ui->requestsTableView;
    }
    if(ui->tabWidget->currentIndex()==1)
    {
//        qDebug("copy agreements");
        sqlQueryModel=agreementsQueryModel;
        tableView=ui->agreementsTableView;
    }
    if(sqlQueryModel==0)
        return;
    QModelIndexList selected=tableView->selectionModel()->selectedRows();
    for(int i=0;i<selected.size();i++)
    {
        int count=sqlQueryModel->record(selected.at(i).row()).count()-1;
        for(int j=0;j<count;j++)
        {
//            QMessageBox::information(this,PROGNAME,num(count));
            QString separator="	";
            if(j==count-1)
                separator="\r\n";
            copyString.append(escapeCSV(sqlQueryModel->record(selected.at(i).row()).value(j).toString())+separator);
        }
    }
    QApplication::clipboard()->setText(copyString);
}
//-------------------------------------------------------------------------------------
/*void MainWindow::on_printRequestPushButton_clicked()
{
    int id=requestsQueryModel->record(ui->requestsTableView->currentIndex().row()).field("requestsid").value().toInt();
    if(!id)
        return;

    QFile file(QApplication::applicationDirPath()+"/request.html");
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(this,PROGNAME,"Не могу открыть файл "+QApplication::applicationDirPath()+"/request.html");
        return;
    }
    QByteArray html=file.readAll();
    QSqlQuery requestQuery;
    QueryExec(this,"SELECT requests.id, requests.ctime, performdate, clients.address, clients.surname, clients.name, clients.phone1, clients.phone2, GROUP_CONCAT(devicetypes.name SEPARATOR ', ') AS devicetypenames, devicename, defect, cost, users.name, offices.name, states.name, performer.name, requests.percent FROM requests LEFT JOIN  states ON states.id=requests.status LEFT JOIN offices ON offices.id=requests.officeid LEFT JOIN clients ON clients.id = requests.clientid LEFT JOIN users ON users.id = requests.cuserid LEFT JOIN requestdevicetypes ON requestdevicetypes.rid = requests.id LEFT JOIN devicetypes ON devicetypes.id = requestdevicetypes.dtid LEFT JOIN users AS performer ON performer.id=requests.performerid WHERE requests.id='"+num(id)+"';",requestQuery);
    if(!requestQuery.next())
        return;
    html.replace("[REQUEST_ID]",requestQuery.value(0).toString().toUtf8());
    html.replace("[CREATE_DATE]",requestQuery.value(1).toString().toUtf8());
    html.replace("[PERFORM_DATE]",requestQuery.value(2).toString().toUtf8());
    html.replace("[CLIENT_ADDRESS]",requestQuery.value(3).toString().toUtf8());
    html.replace("[CLIENT_NAME]",requestQuery.value(4).toString().toUtf8()+" "+requestQuery.value(5).toString().toUtf8());
    html.replace("[CLIENT_PHONE1]",requestQuery.value(6).toString().toUtf8());
    html.replace("[CLIENT_PHONE2]",requestQuery.value(7).toString().toUtf8());

    html.replace("[DEVICE_TYPE]",requestQuery.value(8).toString().toUtf8());
    html.replace("[DEVICE_NAME]",requestQuery.value(9).toString().toUtf8());
    html.replace("[DEFECT]",requestQuery.value(10).toString().toUtf8());
    html.replace("[COST]",requestQuery.value(11).toString().toUtf8());
    html.replace("[DISPATCHER]",requestQuery.value(12).toString().toUtf8());
    html.replace("[OFFICE]",requestQuery.value(13).toString().toUtf8());
    html.replace("[STATUS]",requestQuery.value(14).toString().toUtf8());
    html.replace("[PERFORMER_NAME]",requestQuery.value(15).toString().toUtf8());
    html.replace("[PERCENT]",requestQuery.value(16).toString().toUtf8());
    html.replace("[CURRENT_DATE]",QDate::currentDate().toString("dd.MM.yyyy").toUtf8());

    PrintDialog *printDialog = new PrintDialog(html,this);
    printDialog->exec();
}*/
//-------------------------------------------------------------------------------------
void MainWindow::on_copyRequestPushButton_clicked()
{
    int id=requestsQueryModel->record(ui->requestsTableView->currentIndex().row()).field("requestsid").value().toInt();
    if(!id)
        return;
    RequestDialog *requestDialog = new RequestDialog(id,0,0,true,this);
    if(requestDialog->exec()==QDialog::Accepted)
    {
        refreshRequests();
        refreshDates();
    }
}
//-------------------------------------------------------------------------------------
void MainWindow::on_columnCreateDateAction_triggered()
{
    if(!ui->columnCreateDateAction->isChecked())
        ui->requestsTableView->hideColumn(1);
    else
        ui->requestsTableView->showColumn(1);
}
//-------------------------------------------------------------------------------------
void MainWindow::on_columnPerformDateAction_triggered()
{
    if(!ui->columnPerformDateAction->isChecked())
        ui->requestsTableView->hideColumn(2);
    else
        ui->requestsTableView->showColumn(2);
}
//-------------------------------------------------------------------------------------
void MainWindow::on_columnAddressAction_triggered()
{
    if(!ui->columnAddressAction->isChecked())
        ui->requestsTableView->hideColumn(3);
    else
        ui->requestsTableView->showColumn(3);
}
//-------------------------------------------------------------------------------------
void MainWindow::on_columnSurnameAction_triggered()
{
    if(!ui->columnSurnameAction->isChecked())
        ui->requestsTableView->hideColumn(4);
    else
        ui->requestsTableView->showColumn(4);
}
//-------------------------------------------------------------------------------------
void MainWindow::on_columnNameAction_triggered()
{
    if(!ui->columnNameAction->isChecked())
        ui->requestsTableView->hideColumn(5);
    else
        ui->requestsTableView->showColumn(5);
}
//-------------------------------------------------------------------------------------
void MainWindow::on_columnPhone1Action_triggered()
{
    if(!ui->columnPhone1Action->isChecked())
        ui->requestsTableView->hideColumn(6);
    else
        ui->requestsTableView->showColumn(6);
}
//-------------------------------------------------------------------------------------
void MainWindow::on_columnPhone2Action_triggered()
{
    if(!ui->columnPhone2Action->isChecked())
        ui->requestsTableView->hideColumn(7);
    else
        ui->requestsTableView->showColumn(7);
}
//-------------------------------------------------------------------------------------
void MainWindow::on_columnDeviceNameAction_triggered()
{
    if(!ui->columnDeviceNameAction->isChecked())
        ui->requestsTableView->hideColumn(8);
    else
        ui->requestsTableView->showColumn(8);
}
//-------------------------------------------------------------------------------------
void MainWindow::on_columnDefectAction_triggered()
{
    if(!ui->columnDefectAction->isChecked())
        ui->requestsTableView->hideColumn(9);
    else
        ui->requestsTableView->showColumn(9);
}
//-------------------------------------------------------------------------------------
void MainWindow::on_columnCostAction_triggered()
{
    if(!ui->columnCostAction->isChecked())
        ui->requestsTableView->hideColumn(10);
    else
        ui->requestsTableView->showColumn(10);
}
//-------------------------------------------------------------------------------------
void MainWindow::on_columnPerformerAction_triggered()
{
    if(!ui->columnPerformerAction->isChecked())
        ui->requestsTableView->hideColumn(11);
    else
        ui->requestsTableView->showColumn(11);
}
//-------------------------------------------------------------------------------------
void MainWindow::on_columnCreatorAction_triggered()
{
    if(!ui->columnCreatorAction->isChecked())
        ui->requestsTableView->hideColumn(12);
    else
        ui->requestsTableView->showColumn(12);
}
//-------------------------------------------------------------------------------------
void MainWindow::on_columnStatusAction_triggered()
{
    if(!ui->columnStatusAction->isChecked())
        ui->requestsTableView->hideColumn(13);
    else
        ui->requestsTableView->showColumn(13);
}
//-------------------------------------------------------------------------------------
void MainWindow::on_columnCollectionAction_triggered()
{
    if(!ui->columnCollectionAction->isChecked())
        ui->requestsTableView->hideColumn(14);
    else
        ui->requestsTableView->showColumn(14);
}
//-------------------------------------------------------------------------------------
void MainWindow::on_eventsAction_triggered()
{
    if(!m_adminFlag)
        return;
    EventsDialog *eventsDialog = new EventsDialog(this);
    eventsDialog->exec();
    delete eventsDialog;
}
//-------------------------------------------------------------------------------------
void MainWindow::on_timesheetAction_triggered()
{
    QTreeWidgetItem *currentTreeWidgetItem=ui->datesTreeWidget->currentItem();
    if(!currentTreeWidgetItem || currentTreeWidgetItem->data(0,Qt::UserRole).toString().length()<7)
    {
        QMessageBox::information(this,PROGNAME,tr("Необходимо выбрать месяц в окне \"Даты\"."));
        return;
    }
    QString dateStr=currentTreeWidgetItem->data(0,Qt::UserRole).toString();
    dateStr.truncate(7);
    QDate date=QDate::fromString(dateStr,"yyyy-MM");
    QString monthStr=m_monthList.at(date.month()-1);
    QByteArray html;
    html="<html><head>";
    html+="<style type=\"text/css\">*{font-size:2.7mm;} table {border-collapse: collapse;} tr,td {border: 0.3mm solid black;} td{text-align:center;vertical-align:middle;width:9mm;} .font_l{font-size:4mm;} .font_xl{font-size:5mm;}</style>";
    html+="</head><body>";
    html+="<p class=\"font_xl\">"+tr("ТАБЕЛЬ РАБОЧЕГО ВРЕМЕНИ")+"</p>";
    html+=tr("<table><tr><td>Год</td><td>Месяц</td></tr><tr><td>%1</td><td>%2</td></tr></table>").arg(date.year()).arg(monthStr);
    html+="<table>";
//    html+="<colgroup span=\"32\"></colgroup>";
//    html+="<tr><td><br></td><td colspan=\"32\">"+monthStr+"</td></tr>";
    QStringList userIdList,userNameList;
    {
        QSqlQuery userSqlQuery;
        QueryExec(this,"SELECT id, name FROM users;",userSqlQuery);
        while(userSqlQuery.next())
        {
            userIdList.append(userSqlQuery.value(0).toString());
            userNameList.append(userSqlQuery.value(1).toString());
        }
    }
    for(int i=0;i<userIdList.size();i++)
    {
        QString loginDays[31];
        QString logoutDays[31];
        int totalEvents=0;
        QSqlQuery sqlQuery;
        QueryExec(this,"SELECT id, actionid, ctime FROM events WHERE userid='"+userIdList.at(i)+"' AND (actionid='"+num(ACTIONID_LOGIN_KCRM)+"' OR actionid='"+num(ACTIONID_LOGOUT_KCRM)+"' OR actionid='"+num(ACTIONID_LOGIN_CART)+"' OR actionid='"+num(ACTIONID_LOGOUT_CART)+"') AND events.ctime LIKE '"+date.toString("yyyy-MM")+"%' ORDER BY ctime;",sqlQuery);
        while(sqlQuery.next())
        {
            totalEvents++;
            QDateTime eventDateTime=QDateTime::fromString(sqlQuery.value(2).toString(),DATETIME_FORMAT);
            int day=eventDateTime.date().day();
            if(day==0)
            {
                QMessageBox::information(this,PROGNAME,tr("Неверный формат даты в событии %1").arg(sqlQuery.value(0).toString()));
                continue;
            }
//            QByteArray debugDay="day="+num(day).toUtf8();
//            qDebug(debugDay.data());
            int actionId=sqlQuery.value(1).toInt();
            if(actionId==ACTIONID_LOGIN_KCRM || actionId==ACTIONID_LOGIN_CART)
            {
//                qDebug("login");
                if(loginDays[day-1].isEmpty())
                    loginDays[day-1]=eventDateTime.toString("hh:mm");
//                qDebug(loginDays[day-1].toUtf8());
            }
            else if(actionId==ACTIONID_LOGOUT_KCRM || actionId==ACTIONID_LOGOUT_CART)
            {
                logoutDays[day-1]=eventDateTime.toString("hh:mm");
            }
        }
        if(!totalEvents)
            continue;
// ФИО пользователя.
        html+="<tr><td colspan=\"32\" class=\"font_l\">"+userNameList.at(i)+"</td></tr>";
// Нумерация дней.
        html+="<tr><td>День</td>";
        for(int day=1;day<=date.daysInMonth();day++)
        {
            html+="<td>"+num(day)+"</td>";
        }
        html+="</tr>";
// Вход в программу.
        html+="<tr><td>Вход</td>";
        for(int day=1;day<=date.daysInMonth();day++)
        {
            html+="<td>"+loginDays[day-1]+"</td>";
        }
        html+="</tr>";
// Выход из программы.
        html+="<tr><td>Выход</td>";
        for(int day=1;day<=date.daysInMonth();day++)
        {
            html+="<td>"+logoutDays[day-1]+"</td>";
        }
        html+="</tr>";
    }
/*    int userid=0;
    int actionid=0;
    int currentDay=1;
    QSqlQuery sqlQuery;
    QueryExec(this,"SELECT actionid, userid, users.name, events.ctime FROM events LEFT JOIN users ON users.id=userid WHERE (actionid='"+num(ACTIONID_LOGIN)+"' OR actionid='"+num(ACTIONID_LOGOUT)+"') AND events.ctime LIKE '"+date.toString("yyyy-MM")+"%' ORDER BY actionid, userid, ctime;",sqlQuery);
    while(sqlQuery.next())
    {
        if(userid!=sqlQuery.value(1).toInt())//Next user.
        {
            currentDay=1;
            userid=sqlQuery.value(1).toInt();
            html+="<tr><td colspan=\"31\">"+sqlQuery.value(2).toString()+"</td></tr><tr>";
            for(int i=1;i<=date.daysInMonth();i++)
            {
                html+="<td>"+num(i)+"</td>";
            }
            html+="</tr>";
        }
        if(actionid!=sqlQuery.value(0).toInt())
        {

        }
        QDateTime eventDateTime=QDateTime::fromString(sqlQuery.value(3).toString(),DATETIME_FORMAT);
        for(currentDay;currentDay<eventDateTime.date().day();currentDay++)
        {
            html+="<td>"+sqlQuery.value(2).toString()+"</td>";

        }

    }*/

    html+="</table></body></html>";
    PrintDialog *printDialog = new PrintDialog(html,QUrl(),this);
    printDialog->exec();
    delete printDialog;
}
//-------------------------------------------------------------------------------------
void MainWindow::on_tabWidget_currentChanged(int index)
{
    if(index==0)
    {
        refreshRequests();
    }
    else if(index==1)
    {
        refreshAgreements();
    }
    else if(index==2)
    {
        refreshClients();
    }
}
//-------------------------------------------------------------------------------------
void MainWindow::on_backupAction_triggered()
{
    // Добавить номер порта!!!
    QString datetime=QDateTime::currentDateTime().toString("yyyy-MM-dd_hh_mm_ss_zzz");
    QString globalConnectionNameUnderscored=globalConnectionName;
    globalConnectionNameUnderscored.replace(" ","_");
    if(settings.value("connection",globalConnectionName,"dbtype","")=="mysql")
    {
        QString fileName = QFileDialog::getSaveFileName(this,tr("Сохранение резервной копии БД MySQL"),globalConnectionNameUnderscored+"_"+datetime+".sql",tr("Файл sql (*.sql)"));
        if(fileName.isEmpty())
            return;
        QString dbuser=settings.value("connection",globalConnectionName,"dbusername","");
        QString dbpass=settings.value("connection",globalConnectionName,"dbpassword","");
        QString dbhost=settings.value("connection",globalConnectionName,"dbhostname","");
        QString dbname=settings.value("connection",globalConnectionName,"databasename","");
        QString mysqldumppath=settings.value("database_common","","mysqldump_path","");
        QStringList temp=dbhost.split(':');
        QString dbport;
        if(temp.size()>1)
        {
            dbhost=temp.at(0);
            dbport=" -P"+temp.at(1);
        }
        if(!mysqldumppath.isEmpty())
        {
            if(mysqldumppath.at(mysqldumppath.size()-1)!='/' && mysqldumppath.at(mysqldumppath.size()-1)!='\\')
                    mysqldumppath.append("/");
        }
        QString execstring=mysqldumppath+"mysqldump -u"+dbuser+" -p"+dbpass+" -h"+dbhost+dbport+" --single-transaction "+dbname;

        QProcess *myProcess = new QProcess();
        myProcess->start(execstring);


        if (myProcess->waitForStarted(1000) == false)
        qDebug() << "Error starting external program";

        else
        qDebug() << "external program running";

        myProcess->waitForReadyRead(300000);
        myProcess->waitForFinished(300000);
        QByteArray backupdata=myProcess->readAllStandardOutput();
        QFile backupFile(fileName);
        if(!backupFile.open(QIODevice::WriteOnly|QIODevice::Truncate))
        {
            logString(this,tr("Не могу создать файл %1.").arg(backupFile.fileName()));
            return;
        }
        backupFile.write(backupdata);
        backupFile.close();
    }
    if(settings.value("connection",globalConnectionName,"dbtype","")=="sqlite")
    {
        QString fileName = QFileDialog::getSaveFileName(this,tr("Сохранение резервной копии БД sqlite"),globalConnectionNameUnderscored+"_"+datetime);
        if(fileName.isEmpty())
            return;
        QString dbfilename=settings.value("connection",globalConnectionName,"databasename","");
        if(!dbfilename.contains('/') && !dbfilename.contains('\\'))
            dbfilename.prepend(programDataPath()+"/");
        if(!QFile::copy(dbfilename,fileName))
        {
            logString(this,tr("Не могу создать файл %1.").arg(fileName));
            return;
        }
//			QMessageBox::information(this,PROGNAME,dbfilename);
//			QMessageBox::information(this,PROGNAME,programDataPath()+"/backup/"+globalConnectionName+"_"+QDateTime::currentDateTime().toString("yyyy-MM-dd_hh_mm_ss_zzz"));
    }
}
//-------------------------------------------------------------------------------------
void MainWindow::on_agreementsTableView_activated(const QModelIndex &index)
{
    int id=agreementsQueryModel->record(index.row()).field("agreementsid").value().toInt();
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
void MainWindow::on_newAgreementPushButton_clicked()
{
    AgreementDialog *agreementDialog = new AgreementDialog(0,this);
    if(agreementDialog->exec()==QDialog::Accepted)
    {
        refreshAgreements();
    }
    delete agreementDialog;
}
//-------------------------------------------------------------------------------------
void MainWindow::on_performerBillsAction_triggered()
{
/*    PerformerbillsDialog *performerbillsDialog = new PerformerbillsDialog(this);
    performerbillsDialog->exec();*/
    QString filePathName=programDataPath()+"/templates/bills.html";
    QFile file(filePathName);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(this,PROGNAME,"Не могу открыть файл "+filePathName);
        return;
    }
    QByteArray html=file.readAll();
    QByteArray tables;
    // ЗАГОЛОВОК
    QSqlQuery performersQuery;
    performersQuery.setForwardOnly(true);
    if(!QueryExec(this,"SELECT id, name FROM users WHERE enabled>0 AND rights&'"+num(CSRIGHT_PERFORMER)+"';",performersQuery))
        return;
    while(performersQuery.next())
    {
        QByteArray bill;
        bill+=tr("<h3>Счёт от %1</h3>\r\n").arg(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss"));
        bill+=tr("<h4>Заказчик: %1</h4>\r\n").arg(performersQuery.value(1).toString());
        // ТАБЛИЦА
        double total=0;
        QSqlQuery requestsQuery;
        QueryExec(this,"SELECT requests.id, requests.ctime, cost, clientcost, percent, collection FROM requests WHERE performerid='"+num(performersQuery.value(0).toInt())+"';",requestsQuery);
        bill+=tr("<table border=\"1\">\r\n<tr><td>№ заявки</td><td>Дата создания</td><td>Сумма мастера</td><td>Сумма клиента</td><td>К оплате</td></tr>\r\n");
        while(requestsQuery.next())
        {
            if(encodeCollected(requestsQuery.value(0).toInt()+int(requestsQuery.value(2).toDouble()))!=requestsQuery.value(5).toString().toUInt())
            {
                double toPay=requestsQuery.value(2).toDouble()*requestsQuery.value(4).toInt()/100;
                bill+="<tr><td>"+requestsQuery.value(0).toString()+"</td><td>"+requestsQuery.value(1).toString()+"</td><td>"+requestsQuery.value(2).toString()+"</td><td>"+requestsQuery.value(3).toString()+"</td><td>"+num(toPay)+"</td></tr>\r\n";
                total+=toPay;
            }
        }
        bill+=tr("</table>\r\n");
        bill+=tr("<h4>Итог: %1</h4>\r\n").arg(total);
        if(total)
            tables+=bill;
    }
    html.replace("[BILLS]",tables);

    PrintDialog *printDialog = new PrintDialog(html,QUrl::fromLocalFile(filePathName),this);
    printDialog->exec();
    delete printDialog;
}
//-------------------------------------------------------------------------------------
void MainWindow::on_showTerminatedAgreementsCheckBox_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    refreshAgreements();
}
//-------------------------------------------------------------------------------------
void MainWindow::refreshTypeComboBox()
{
    ui->agrFilterTypeComboBox->clear();
    ui->agrFilterTypeComboBox->addItem(tr("Не выбран"),0);
    QSqlQuery typeQuery;
    QueryExec(this,"SELECT id,name FROM agreementtypes WHERE enabled>0;",typeQuery);
    while(typeQuery.next())
    {
        ui->agrFilterTypeComboBox->addItem(typeQuery.value(1).toString(),typeQuery.value(0));
    }
    ui->agrFilterTypeComboBox->setCurrentIndex(0);
}
//-------------------------------------------------------------------------------------
void MainWindow::refreshTrestComboBox()
{
    ui->agrFilterTrestComboBox->clear();
    ui->agrFilterTrestComboBox->addItem(tr("Не выбран"),0);
    QSqlQuery trestQuery;
    QueryExec(this,"SELECT id,name FROM trests WHERE enabled>0;",trestQuery);
    while(trestQuery.next())
    {
        ui->agrFilterTrestComboBox->addItem(trestQuery.value(1).toString(),trestQuery.value(0));
    }
    ui->agrFilterTrestComboBox->setCurrentIndex(0);
}
//-------------------------------------------------------------------------------------
void MainWindow::on_agrFilterTrestComboBox_activated(int index)
{
    Q_UNUSED(index)
    refreshAgreements();
}
//-------------------------------------------------------------------------------------
void MainWindow::on_agrFilterTypeComboBox_activated(int index)
{
    Q_UNUSED(index)
    refreshAgreements();
}
//-------------------------------------------------------------------------------------
void MainWindow::refreshDocumentsButtons()
{
    QList<QWidget *> widgets = ui->documentsGridLayout->findChildren<QWidget *>();
    foreach(QWidget * widget, widgets)
    {
        delete widget;
    }

/*    QObjectList objList=ui.printDocumentsGroupBox->layout()->children();
    for(int i=0;i<objList.size();i++)
    {
        delete objList.at(i);
//        objList.at(i)->deleteLater();
    }*/
//        ui.printDocumentsGroupBox->layout()->ch(objList.at(i));
//    ui.documentsComboBox->clear();
// Ищем файлы с настройками.
    QString documentsPath=settings.value("interface","","templatespath","",programDataPath()+"/templates")+"/kcrm";
//    QMessageBox::information(this,PROGNAME,documentsPath);
    QDir dir(documentsPath);
    QStringList filelist=dir.entryList(QStringList("*.html"),QDir::Files);
    for(int i=0;i<filelist.size();i++)
        filelist[i].prepend(documentsPath+"/");
// Заполняем комбобокс.
    for(int i=0;i<filelist.size();i++)
    {
        QFileInfo fileInfo(filelist.at(i));
//        ui.documentsComboBox->addItem(fileInfo.baseName(),filelist.at(i));
        // Кнопки
        QPushButton *button=new QPushButton(fileInfo.baseName(), ui->documentsGridLayout->widget());
        button->setProperty("filePathName",filelist.at(i));
        ui->documentsGridLayout->addWidget(button,i%20,i/20);
        connect(button,SIGNAL(clicked()),SLOT(On_printDocumentPushButton_clicked()));
    }
}
//-------------------------------------------------------------------------------------
void MainWindow::On_printDocumentPushButton_clicked()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    QString filePathName=button->property("filePathName").toString();
//    QString filePathName=ui.documentsComboBox->currentData().toString();
    if(filePathName.isEmpty())
    {
        QMessageBox::information(this,PROGNAME,"Имя файла не задано! "+filePathName);
        return;
    }
    QFile file(filePathName);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(this,PROGNAME,"Не могу открыть файл "+filePathName);
        return;
    }
    QByteArray html=file.readAll();
    PrintDialog *printDialog = new PrintDialog(html,QUrl::fromLocalFile(filePathName),this);
    printDialog->exec();
    delete printDialog;
}
//-------------------------------------------------------------------------------------
void MainWindow::on_filterLineEdit_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    if(ui->tabWidget->currentIndex()==0)// Requests
        refreshRequests();
    if(ui->tabWidget->currentIndex()==1)// Agreements
        refreshAgreements();
}
//-------------------------------------------------------------------------------------
void MainWindow::on_resetAmountsAction_triggered()
{
    ClearAmountsDialog * clearAmountsDialog = new ClearAmountsDialog(this);
    if(clearAmountsDialog->exec()==QDialog::Accepted)
        refreshRequests();
    delete clearAmountsDialog;
}
//-------------------------------------------------------------------------------------
void MainWindow::on_usersAction_triggered()
{
    UsersDialog * usersDialog = new UsersDialog(this);
    if(usersDialog->exec()==QDialog::Accepted)
    {
        refreshPerformers();
    }
    delete usersDialog;
}
//-------------------------------------------------------------------------------------
void MainWindow::on_clientsTableView_activated(const QModelIndex &index)
{
    int id=clientsQueryModel->record(index.row()).field("clientid").value().toInt();
    if(!id)
    {
        qDebug() << "No id.";
        return;
    }
    ClientCallDialog *clientCallDialog = new ClientCallDialog(id,this);
    if(clientCallDialog->exec()==QDialog::Accepted)
    {
        refreshClients();
    }
    delete clientCallDialog;
}
//-------------------------------------------------------------------------------------
