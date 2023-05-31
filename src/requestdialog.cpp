#include "requestdialog.h"
#include "ui_requestdialog.h"
#include "common_inc.h"
#include "clientsdialog.h"
#include "calendardialog.h"
#include "sendsmsdialog.h"
#include "printdialog.h"
#include "agreementdialog.h"

extern unsigned int globaluid;
extern XMLSettings settings;

//-------------------------------------------------------------------------------------
RequestDialog::RequestDialog(int requestid, int officeid,int agreementid,bool copyFlag, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RequestDialog),m_requestid(requestid),m_officeid(officeid),m_agreementid(agreementid),m_clientid(0),m_performerid(0)
{
    m_percent=0;
    quint64 flags=0;
    ui->setupUi(this);
    ui->ratingComboBox->addItem("Без оценки",0);
    ui->ratingComboBox->addItem("1",1);
    ui->ratingComboBox->addItem("2",2);
    ui->ratingComboBox->addItem("3",3);
    ui->ratingComboBox->addItem("4",4);
    ui->ratingComboBox->addItem("5",5);

    QSettings registrySettings(ORGNAME,PROGNAME);
    restoreGeometry(registrySettings.value("requestgeometry").toByteArray());
    if(!m_officeid)
        m_officeid=1;
//    refreshDeviceTypes();
    refreshStates();
// COMPLETER
    deviceCompleterModel = new QStringListModel(this);
    deviceCompleter = new QCompleter(this);
    deviceCompleter->setFilterMode(Qt::MatchContains);
    deviceCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    deviceCompleter->setModel(deviceCompleterModel);
    ui->deviceNameLineEdit->setCompleter(deviceCompleter);
//    connect(deviceCompleter, SIGNAL(activated(QModelIndex)), this, SLOT(On_deviceCompleter_Activated(QModelIndex)));//    ui.clientComboBox->setModel(clientComboModel);
    refreshDeviceCompleter();

    quint64 userid=globaluid;
    if(!checkRights(this,globaluid,CSRIGHT_EXEC_ADMIN)) // Инкассация доступна только первым двум пользователям.
    {
        ui->collectionComboBox->hide();
    }
    int bonusUserId=0;
    if(m_requestid)
    {
        QSqlQuery requestQuery;
        QueryExec(this,"SELECT agreementid, officeid, status, clientid, performerid, rating, devicename, defect, done, notes, clientcost, cost, collection, performdate, cuserid, ctime, percent, flags, bonususer FROM requests WHERE id='"+num(m_requestid)+"';",requestQuery);
        if(!requestQuery.next())
        {
            QMetaObject::invokeMethod(this, "reject", Qt::QueuedConnection); // Close dialog.
            return;
        }
        m_agreementid=requestQuery.value(0).toInt();
        m_officeid=requestQuery.value(1).toInt();
        if(!copyFlag)
            ui->statusComboBox->setCurrentIndex(ui->statusComboBox->findData(requestQuery.value(2).toInt()));
        m_clientid=requestQuery.value(3).toInt();
        m_performerid=requestQuery.value(4).toInt();
        ui->ratingComboBox->setCurrentIndex(ui->ratingComboBox->findData(requestQuery.value(5)));
        ui->deviceNameLineEdit->setText(requestQuery.value(6).toString());
        ui->defectPlainTextEdit->setPlainText(requestQuery.value(7).toString());
        ui->donePlainTextEdit->setPlainText(requestQuery.value(8).toString());
        ui->notesPlainTextEdit->setPlainText(requestQuery.value(9).toString());
        double clientcost=requestQuery.value(10).toDouble();
        double cost=requestQuery.value(11).toDouble();
        unsigned int collection=requestQuery.value(12).toUInt();
        QString performDateStr=requestQuery.value(13).toString();
        userid=requestQuery.value(14).toULongLong();
        ui->clientcostDoubleSpinBox->setValue(clientcost);
        ui->costDoubleSpinBox->setValue(cost);
        if(performDateStr.size()==10)// Если только дата
            ui->performDateTimeEdit->setDateTime(QDateTime::fromString(performDateStr,"yyyy-MM-dd"));
        else
            ui->performDateTimeEdit->setDateTime(QDateTime::fromString(performDateStr,"yyyy-MM-dd HH:mm"));
        if(!copyFlag)
            ui->ctimeLineEdit->setText(requestQuery.value(15).toString());
        m_percent=requestQuery.value(16).toInt();
        ui->percentSummValueLabel->setText(num(cost*requestQuery.value(16).toInt()/100));
        flags=requestQuery.value(17).toULongLong();
        bonusUserId=requestQuery.value(18).toInt();
        setWindowTitle(tr("Редактирование заявки №%1").arg(m_requestid));
        if(!copyFlag)
        {
            ui->requestidLabel->setText(tr("Заявка %1").arg(m_requestid));
            // Инкассация
//            if(encodeCollected(m_requestid+int(cost))==collection)
            if(collection)
            {
                ui->collectionComboBox->setCurrentIndex(1);
                ui->costDoubleSpinBox->setReadOnly(true);
                ui->clientcostDoubleSpinBox->setReadOnly(true);
            }
        }
        // Если заявка инкассирована, показываем суммы только админу.
/*        if(collection && !checkRights(this,globaluid,CSRIGHT_EXEC_ADMIN))
        {
            ui->sumGroupBox->hide();
        }*/
        // Клиент
        if(m_clientid)
        {
            refreshClient();
//            refreshDeviceComboBox();
        }
//        QMessageBox::information(this,PROGNAME,tr("id=%1 cost=%2 calculated=%3 received=%4").arg(m_requestid).arg(cost).arg((m_requestid+int(cost))^0xE3649BDC).arg(collection));
    }
    else
    {
//        refreshPerformers();
//        ui->collectionComboBox->hide();
        ui->ctimeLineEdit->setText(timestamp());
        ui->performDateTimeEdit->setDate(QDate::currentDate());
        setWindowTitle(tr("Новая заявка"));
    }
    refreshBonusUserComboBox(bonusUserId);
    ui->cuserLineEdit->setText(getUserName(this,userid));
    bool signal=ui->officeComboBox->blockSignals(true);
    refreshOffices();
    refreshFlags(flags);
// Ищем текущий офис.
    bool finded=false;
    for(int i=0;i<ui->officeComboBox->count();i++)
        if(ui->officeComboBox->itemData(i,Qt::UserRole)==m_officeid)
        {
            ui->officeComboBox->setCurrentIndex(i);
            finded=true;
        }
    if(!finded)
    {
        ui->officeComboBox->addItem("???",num(m_officeid));
        ui->officeComboBox->setCurrentIndex(ui->officeComboBox->findData(num(m_officeid)));
    }
    ui->officeComboBox->blockSignals(signal);
    refreshPerformers();
    if(copyFlag)
    {
        setWindowTitle(tr("Копирование заявки %1").arg(m_requestid));
        m_requestid=0; // new request.
    }
    if(m_agreementid)
        ui->agreementLabel->setText(tr("Договор № %1").arg(m_agreementid));
}
//-------------------------------------------------------------------------------------
RequestDialog::~RequestDialog()
{
    delete deviceCompleter;
    delete deviceCompleterModel;
    delete ui;
}
//-------------------------------------------------------------------------------------
void RequestDialog::closeEvent(QCloseEvent *)
{
    QSettings registrySettings(ORGNAME,PROGNAME);
    registrySettings.setValue("requestgeometry", saveGeometry());
}
//-------------------------------------------------------------------------------------
/*QDate RequestDialog::performDate()
{
    return ui->performDateTimeEdit->date();
}*/
//-------------------------------------------------------------------------------------
void RequestDialog::refreshOffices()
{
    ui->officeComboBox->clear();
    QSqlQuery officeSqlQuery;
    QueryExec(this,"SELECT id, name FROM offices WHERE enabled>0 ORDER BY name;",officeSqlQuery);
    while(officeSqlQuery.next())
    {
        ui->officeComboBox->addItem(officeSqlQuery.value(1).toString(),officeSqlQuery.value(0));
    }
}
//-------------------------------------------------------------------------------------
/*void RequestDialog::refreshDeviceTypes()
{
    ui->deviceTypeListWidget->clear();
    QSqlQuery deviceTypeQuery;
    QueryExec(this,"SELECT devicetypes.id, name, requestdevicetypes.id FROM devicetypes LEFT JOIN requestdevicetypes ON dtid=devicetypes.id AND rid='"+num(m_requestid)+"' WHERE enabled>0 OR rid='"+num(m_requestid)+"' ORDER BY name;",deviceTypeQuery);
    while(deviceTypeQuery.next())
    {
        QListWidgetItem* item = new QListWidgetItem(deviceTypeQuery.value(1).toString(), ui->deviceTypeListWidget);
        item->setData(Qt::UserRole,deviceTypeQuery.value(0));
        item->setData(Qt::UserRole+1,deviceTypeQuery.value(2));
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable); // set checkable flag
        item->setCheckState(Qt::Unchecked); // AND initialize check state
        if(deviceTypeQuery.value(2).toInt())
            item->setCheckState(Qt::Checked); // AND initialize check state
    }
}*/
//-------------------------------------------------------------------------------------
void RequestDialog::refreshFlags(quint64 flags)
{
    QVBoxLayout *groupsLayout= new QVBoxLayout(ui->flagsWidget);
    ui->flagsWidget->setLayout(groupsLayout);
    QString currentGroupName;
    QLayout *flagsLayout=groupsLayout;
//    ui->flagsListWidget->clear();
    QSqlQuery flagsQuery;
    if(!QueryExec(this,"SELECT requestflags.bit, requestflags.name, requestflaggroups.type, requestflaggroups.name FROM requestflags LEFT JOIN requestflaggroups ON requestflaggroups.id=requestflags.rfgroupid WHERE requestflags.name!='' ORDER BY requestflaggroups.id, requestflags.id;",flagsQuery))
        return;
    while(flagsQuery.next())
    {
        int bit=flagsQuery.value(0).toInt();
        QString flagName=flagsQuery.value(1).toString();
        int type=flagsQuery.value(2).toInt();
        QString groupName=flagsQuery.value(3).toString();
        // Group box.
        if(currentGroupName!=groupName)
        {
            currentGroupName=groupName;
            QGroupBox *groupBox=new QGroupBox(groupName,this);
            groupsLayout->addWidget(groupBox);
            QVBoxLayout *layout = new QVBoxLayout(groupBox);
            groupBox->setLayout(layout);
            flagsLayout=layout;
        }
        // F L A G S
        if(bit<1 || bit>63)
            continue;
        quint64 flag=(quint64)1<<(bit-1);
        if(type==1) // Radio button
        {
            QRadioButton *radioButton=new QRadioButton(flagName,this);
            flagsLayout->addWidget(radioButton);
            if(flag&flags)
            {
                radioButton->setChecked(true);
            }
            rFlagsList.append(RFlags(bit,radioButton));
        }
        else // Check box
        {
            QCheckBox *checkBox=new QCheckBox(flagName,this);
            flagsLayout->addWidget(checkBox);
            if(flag&flags)
            {
                checkBox->setChecked(true);
            }
            rFlagsList.append(RFlags(bit,checkBox));
        }
/*        QListWidgetItem* item = new QListWidgetItem(flagsQuery.value(1).toString(), ui->flagsListWidget);
        item->setData(Qt::UserRole,bit);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable); // set checkable flag
        item->setCheckState(Qt::Unchecked); // AND initialize check state
        if(flag&flags)
        {
            item->setCheckState(Qt::Checked); // AND initialize check state
            flags&=~flag; // Убираем флаг, если добавили его.
        }*/
    }
    QSpacerItem *spacerItem = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    groupsLayout->addItem(spacerItem);
/*    for(int i=0;i<63;i++) // Если остались не добавленые флаги, добавляем их со знаком вопроса.
    {
        if(((long long)1<<i)&flags)
        {
            QListWidgetItem* item = new QListWidgetItem(tr("?"), ui->flagsListWidget);
            item->setData(Qt::UserRole,i+1);
            item->setFlags(item->flags() | Qt::ItemIsUserCheckable); // set checkable flag
            item->setCheckState(Qt::Checked); // AND initialize check state
        }
    }*/
}
//-------------------------------------------------------------------------------------
bool RequestDialog::refreshClient()
{
    ui->clientPushButton->setText(tr("Клиент")+" "+num(m_clientid));
    // Очистка.
    ui->clientsurnameLineEdit->clear();
    ui->clientnameLineEdit->clear();
    ui->phone1LineEdit->clear();
    ui->phone2LineEdit->clear();
    ui->addressPlainTextEdit->clear();
    for(int i=ui->agreementsWidget->children().count()-1;i>0;i--)
        ui->agreementsWidget->children().at(i)->deleteLater();
    // Заполнение.
    QSqlQuery clientSqlQuery;
	if(!QueryExec(this,"SELECT clients.surname, clients.name, clients.phone1, clients.phone2, directions.name, clients.address FROM clients LEFT JOIN directions ON clients.directionid=directions.id WHERE clients.id='"+num(m_clientid)+"';",clientSqlQuery))
	{
        return false;
	}
    if(!clientSqlQuery.next())
	{
        return false;
	}
    ui->clientsurnameLineEdit->setText(clientSqlQuery.value(0).toString());
    ui->clientnameLineEdit->setText(clientSqlQuery.value(1).toString());
    ui->phone1LineEdit->setText(clientSqlQuery.value(2).toString());
    ui->phone2LineEdit->setText(clientSqlQuery.value(3).toString());
	ui->directionLineEdit->setText(clientSqlQuery.value(4).toString());
	ui->addressPlainTextEdit->setPlainText(clientSqlQuery.value(5).toString());
    QSqlQuery agreementsSqlQuery;
    if(!QueryExec(this,"SELECT id FROM agreements WHERE client='"+num(m_clientid)+"' AND status='1' ORDER BY id DESC;",agreementsSqlQuery))
	{
        return false;
	}
    while(agreementsSqlQuery.next())
    {
        quint64 agreementId=agreementsSqlQuery.value(0).toULongLong();
        QPushButton *pushButton=new QPushButton(num(agreementId),ui->agreementsWidget);
        pushButton->setProperty("agreementId",agreementId);
        ui->agreementsWidget->layout()->addWidget(pushButton);
        connect(pushButton,SIGNAL(clicked()),SLOT(On_agreementPushButton_clicked()));
    }
    return true;
}
//-------------------------------------------------------------------------------------
void RequestDialog::refreshPerformers()
{
    ui->performerComboBox->clear();
    ui->performerComboBox->addItem(tr("Не выбран"),0);
    QSqlQuery performerQuery;
    QueryExec(this,"SELECT users.id, users.name, users.rating_avg FROM performeroffices LEFT JOIN users ON users.id=performeroffices.performerid WHERE (performeroffices.officeid='"+num(m_officeid)+"' AND users.enabled>0) OR users.id='"+num(m_performerid)+"' GROUP BY users.id ORDER BY users.name;",performerQuery);
    while(performerQuery.next())
    {
        ui->performerComboBox->addItem(performerQuery.value(1).toString()+" ("+performerQuery.value(2).toString()+")",performerQuery.value(0));
    }
    ui->performerComboBox->setCurrentIndex(ui->performerComboBox->findData(m_performerid));
}
//-------------------------------------------------------------------------------------
void RequestDialog::refreshStates()
{
    ui->statusComboBox->clear();
    QSqlQuery statesSqlQuery;
    QueryExec(this,"SELECT id,name FROM states;",statesSqlQuery);
    while(statesSqlQuery.next())
    {
        ui->statusComboBox->addItem(statesSqlQuery.value(1).toString(),statesSqlQuery.value(0));
    }
}
//-------------------------------------------------------------------------------------
void RequestDialog::refreshDeviceCompleter()
{
    QSqlQuery sqlQuery;
    QueryExec(this,"SELECT devicename FROM requests LIMIT 300;",sqlQuery);
    QStringList deviceNameList;
    while(sqlQuery.next())
    {
        QString deviceName=sqlQuery.value(0).toString();
        if(!deviceNameList.contains(deviceName))
            deviceNameList.append(deviceName);
    }
    deviceCompleterModel->setStringList(deviceNameList);
}
//-------------------------------------------------------------------------------------
/*void RequestDialog::refreshDeviceComboBox()
{
    ui->addDeviceComboBox->clear();
    QSqlQuery sqlQuery;
    QueryExec(this,"SELECT clientdevices.id, devicetypes.name, devices.brand, devices.model, clientdevices.serial FROM clientdevices LEFT JOIN devices ON clientdevices.deviceid=devices.id LEFT JOIN devicetypes ON devices.devicetypeid=devicetypes.id WHERE clientdevices.clientid='"+num(m_clientid)+"';",sqlQuery);
    while(sqlQuery.next())
    {
        ui->addDeviceComboBox->addItem(sqlQuery.value(1).toString()+" "+sqlQuery.value(2).toString()+" "+sqlQuery.value(3).toString()+", серийный №"+sqlQuery.value(4).toString(),sqlQuery.value(0));
    }
    ui->addDeviceComboBox->setCurrentIndex(-1);
}*/
//-------------------------------------------------------------------------------------
/*void RequestDialog::refreshRequestDevices()
{

}*/
//-------------------------------------------------------------------------------------
void RequestDialog::on_okPushButton_clicked()
{
    if(!saveRequest())
        return;
    accept();
}
//-------------------------------------------------------------------------------------
bool RequestDialog::saveRequest()
{
    QString status=num(ui->statusComboBox->currentData().toInt());
    int performerid=ui->performerComboBox->currentData().toInt();
    int bonusUserId=ui->bonusUserComboBox->currentData().toInt();
    QString rating=ui->ratingComboBox->currentData().toString();
    QString devicename=escapeQuotes(ui->deviceNameLineEdit->text());
    QString defect=escapeQuotes(ui->defectPlainTextEdit->toPlainText());
    QString done=escapeQuotes(ui->donePlainTextEdit->toPlainText());
    QString notes=escapeQuotes(ui->notesPlainTextEdit->toPlainText());
    QString clientcost=num(ui->clientcostDoubleSpinBox->value());
    QString cost=num(ui->costDoubleSpinBox->value());
    QString performdatetime=ui->performDateTimeEdit->dateTime().toString("yyyy-MM-dd HH:mm");
    m_officeid=ui->officeComboBox->currentData(Qt::UserRole).toInt();
    // FLAGS
    quint64 flags=0;
    for(int i=0;i<rFlagsList.count();i++)
    {
        if(rFlagsList.at(i).isChecked())
        {
            flags|=((quint64)1<<(rFlagsList.at(i).bit()-1));
        }
    }
    // ПОЛУЧАЕМ ПРОЦЕНТ ДЛЯ СЧЁТА
    int percent=0;
    {
        QSqlQuery officeSqlQuery;
        if(!QueryExec(this,"SELECT performerbillpercent FROM offices WHERE id='"+num(m_officeid)+"';",officeSqlQuery))
            return false;
        if(!officeSqlQuery.next())
            return false;
        percent=officeSqlQuery.value(0).toInt();
    }
// С О Х Р А Н Е Н И Е
    if(m_requestid)
    {
        // ПРОВЕРКА СОСТОЯНИЯ ЗАКАЗА
        QSqlQuery requestQuery;
        QueryExec(this,"SELECT status FROM requests WHERE id='"+num(m_requestid)+"';",requestQuery);
        if(!requestQuery.next())
            return false;
        if(requestQuery.value(0).toInt()>9)
        {
            if(checkRights(this,globaluid,CSRIGHT_EXEC_ADMIN))
            {
                if(QMessageBox::question(this, PROGNAME, tr("Заявка отмечена как выполнена/отмена и доступна только для чтения. Изменить?"),QMessageBox::Ok|QMessageBox::Cancel,QMessageBox::Cancel)!=QMessageBox::Ok)
                    return false;
            }
            else
            {
                QMessageBox::information(this,PROGNAME,tr("Заявка отмечена как выполнена/отмена и доступна только для чтения."));
                return false;
            }
        }
        QString collection;
        if(checkRights(this,globaluid,CSRIGHT_EXEC_ADMIN))
        {
            collection=" collection='0',";
            if(ui->collectionComboBox->currentIndex()==1)
            {
//                double dcost=cost.toDouble();
//                collection=" collection='"+num(encodeCollected(m_requestid+int(dcost)))+"',";
                collection=" collection='1',";
//            QMessageBox::information(this,PROGNAME,tr("id=%1 cost=%2 calculated=%3").arg(m_requestid).arg(dcost).arg(collection));//(m_requestid+int(dcost))^0xE3649BDC));
            }
        }
        if(!QueryExec(this,"UPDATE requests SET officeid='"+num(m_officeid)+"', status='"+status+"', clientid='"+num(m_clientid)+"', performerid='"+num(performerid)+"', rating='"+rating+"', percent='"+num(percent)+"', bonususer='"+num(bonusUserId)+"', flags='"+QString::number(flags)+"', devicename='"+devicename+"', defect='"+defect+"', done='"+done+"', notes='"+notes+"', clientcost='"+clientcost+"', cost='"+cost+"',"+collection+" performdate='"+performdatetime+"' WHERE id='"+num(m_requestid)+"';"))
            return false;
    }
    else // N E W
    {
        // СОЗДАЁМ ЗАЯВКУ
        QSqlQuery insertSqlQuery;
        if(!QueryExec(this,"INSERT INTO requests(agreementid,officeid,status, clientid, performerid, rating, percent, bonususer, flags, devicename, defect, done, notes, clientcost, cost, performdate, cuserid, ctime) VALUES('"+num(m_agreementid)+"','"+num(m_officeid)+"','"+status+"','"+num(m_clientid)+"', '"+num(performerid)+"', '"+rating+"', '"+num(percent)+"', '"+num(bonusUserId)+"','"+QString::number(flags)+"','"+devicename+"','"+defect+"','"+done+"','"+notes+"','"+clientcost+"','"+cost+"','"+performdatetime+"','"+num(globaluid)+"','"+timestamp()+"');",insertSqlQuery))
            return false;
        m_requestid=insertSqlQuery.lastInsertId().toInt();
        setWindowTitle(tr("Редактирование заявки №%1").arg(m_requestid));
        ui->requestidLabel->setText(tr("Заявка %1").arg(m_requestid));
    }
    updatePerformerRatingAvg(performerid);


    // Update device types.
/*    QString deviceTypes;
    for(int i=0;i<ui->deviceTypeListWidget->count();i++)
    {
        if(ui->deviceTypeListWidget->item(i)->checkState()==Qt::Checked)
        {
            if(!ui->deviceTypeListWidget->item(i)->data(Qt::UserRole+1).toInt()) // Если индекс в таблице requestdevicetypes нулевой, создаём.
            {
                QueryExec(this,"INSERT INTO requestdevicetypes(rid,dtid) VALUES('"+num(m_requestid)+"','"+num(ui->deviceTypeListWidget->item(i)->data(Qt::UserRole).toInt())+"');");
            }
            if(!deviceTypes.isEmpty())
                deviceTypes+=", ";
            deviceTypes+=ui->deviceTypeListWidget->item(i)->text();
        }
        else
        {
            if(ui->deviceTypeListWidget->item(i)->data(Qt::UserRole+1).toInt()) // Если индекс в таблице requestdevicetypes не нулевой, удаляем.
            {
                QueryExec(this,"DELETE FROM requestdevicetypes WHERE id='"+num(ui->deviceTypeListWidget->item(i)->data(Qt::UserRole+1).toInt())+"';");
            }
        }
    }
*/


/*    QString deviceTypes;
    QueryExec(this,"DELETE FROM requestdevicetypes WHERE rid='"+num(m_requestid)+"';");
    for(int i=0;i<ui->deviceTypeListWidget->count();i++)
    {
        if(ui->deviceTypeListWidget->item(i)->checkState()==Qt::Checked)
        {
            QueryExec(this,"INSERT INTO requestdevicetypes(rid,dtid) VALUES('"+num(m_requestid)+"','"+num(ui->deviceTypeListWidget->item(i)->data(Qt::UserRole).toInt())+"');");
            if(!deviceTypes.isEmpty())
                deviceTypes+=", ";
            deviceTypes+=ui->deviceTypeListWidget->item(i)->text();
        }
    }*/
    if(m_performerid!=performerid && performerid>0)
    {
        updatePerformerRatingAvg(m_performerid); // Если поменяли мастера, то предыдущему тоже обновляем баллы.
        // S M S мастеру
        QString performerName,performerPhone;
        QSqlQuery performerQuery;
        QueryExec(this,"SELECT phone1, name FROM users WHERE id='"+num(performerid)+"';",performerQuery);
        if(performerQuery.next())
        {// SMS Мастеру
            performerPhone=performerQuery.value(0).toString();
            performerName=performerQuery.value(1).toString();
            if(performerPhone.size()>=11)
            {
                QByteArray trimmedPhone=performerPhone.right(11).toUtf8();//убираем плюсик
                QString smsText;//=getOption(this,"SMS_NEW_REQUEST_CLIENT_TEXT");
                smsText=tr("#%1\r\n").arg(m_requestid);
                smsText+=QDateTime::fromString(performdatetime,"yyyy-MM-dd HH:mm").toString("dd.MM.yyyy HH:mm")+"\r\n";
                smsText+=ui->clientnameLineEdit->text()+" "+ui->clientsurnameLineEdit->text()+"\r\n";
                smsText+=ui->phone1LineEdit->text()+"\r\n";
                smsText+=ui->addressPlainTextEdit->toPlainText()+"\r\n";
//                smsText+=deviceTypes+"\r\n";
                smsText+=devicename+"\r\n";
                smsText+=defect+"\r\n";
                SendSmsDialog sendSmsDialog(trimmedPhone,smsText,this);
//                sendSMSviaHTTP(trimmedPhone,smsText);
            }
        }
        // S M S клиенту
        QByteArray phone=ui->phone1LineEdit->text().replace(" ","").toUtf8();
        if(phone.size()>=11)
        {
            phone=phone.right(11);// убираем плюсик
            QString smsText=getOption(this,"SMS_NEW_REQUEST_CLIENT_TEXT");
            smsText.replace("<RID>",num(m_requestid));
            smsText.replace("<RDATE>",QDateTime::fromString(performdatetime,"yyyy-MM-dd HH:mm").toString("dd.MM.yyyy HH:mm"));
            smsText.replace("<RPERFORMER>",performerName);
            smsText.replace("<RPERFORMER_PHONE>",performerPhone);
            smsText.replace("<RNOTES>",notes);
            SendSmsDialog sendSmsDialog(phone,smsText,this);
//            sendSMSviaHTTP(phone,smsText);
        }
    }
    return true;
}
//-------------------------------------------------------------------------------------
void RequestDialog::on_clientPushButton_clicked()
{
    ClientsDialog *clientsDialog = new ClientsDialog(m_clientid,this);
    if(clientsDialog->exec()==QDialog::Accepted)
    {
        m_clientid=clientsDialog->clientId();
        refreshClient();
//        refreshDeviceComboBox();
//        refreshRequestDevices();
    }
    delete clientsDialog;
}
//-------------------------------------------------------------------------------------
void RequestDialog::on_officeComboBox_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    m_officeid=ui->officeComboBox->currentData(Qt::UserRole).toInt();
    refreshPerformers();
}
//-------------------------------------------------------------------------------------
void RequestDialog::on_calendarPushButton_clicked()
{
    CalendarDialog * calendarDialog = new CalendarDialog(ui->performDateTimeEdit->dateTime(),this);
    if(calendarDialog->exec()==QDialog::Accepted)
        ui->performDateTimeEdit->setDateTime(calendarDialog->dateTime());
    delete calendarDialog;
}
//-------------------------------------------------------------------------------------
void RequestDialog::on_printPushButton_clicked()
{
    if(!m_requestid)
        saveRequest();
    if(!m_requestid)
        return;

    QString templatesFilePathName=settings.value("interface","","templatespath","",programDataPath()+"/templates/request.html");
    QFile file(templatesFilePathName);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(this,PROGNAME,"Не могу открыть файл "+templatesFilePathName);
        return;
    }
    QByteArray html=file.readAll();
    QSqlQuery requestQuery;
    QueryExec(this,"SELECT requests.id, requests.ctime, performdate, clients.address, clients.surname, clients.name, clients.phone1, clients.phone2, devicename, defect, cost, users.name, offices.name, states.name, performer.name, requests.percent, requests.flags FROM requests LEFT JOIN  states ON states.id=requests.status LEFT JOIN offices ON offices.id=requests.officeid LEFT JOIN clients ON clients.id = requests.clientid LEFT JOIN users ON users.id = requests.cuserid LEFT JOIN users AS performer ON performer.id=requests.performerid WHERE requests.id='"+num(m_requestid)+"';",requestQuery);
    if(!requestQuery.next())
        return;
    html.replace("[REQUEST_ID]",requestQuery.value(0).toString().toUtf8());
    html.replace("[CREATE_DATE]",requestQuery.value(1).toString().toUtf8());
    html.replace("[PERFORM_DATE]",requestQuery.value(2).toString().toUtf8());
    html.replace("[CLIENT_ADDRESS]",requestQuery.value(3).toString().toUtf8());
    html.replace("[CLIENT_NAME]",requestQuery.value(4).toString().toUtf8()+" "+requestQuery.value(5).toString().toUtf8());
    html.replace("[CLIENT_PHONE1]",requestQuery.value(6).toString().toUtf8());
    html.replace("[CLIENT_PHONE2]",requestQuery.value(7).toString().toUtf8());

    html.replace("[DEVICE_NAME]",requestQuery.value(8).toString().toUtf8());
    html.replace("[DEFECT]",requestQuery.value(9).toString().toUtf8());
    html.replace("[COST]",requestQuery.value(10).toString().toUtf8());
    html.replace("[DISPATCHER]",requestQuery.value(11).toString().toUtf8());
    html.replace("[OFFICE]",requestQuery.value(12).toString().toUtf8());
    html.replace("[STATUS]",requestQuery.value(13).toString().toUtf8());
    html.replace("[PERFORMER_NAME]",requestQuery.value(14).toString().toUtf8());
    html.replace("[PERCENT]",requestQuery.value(15).toString().toUtf8());
    quint64 flags=requestQuery.value(16).toULongLong();
    html.replace("[CURRENT_DATE]",QDate::currentDate().toString("dd.MM.yyyy").toUtf8());

// Flags.
    QString jsonData="{";
    jsonData+="\"flags\":[";
    QSqlQuery flagsQuery;
    QueryExec(this,"SELECT requestflags.bit, requestflags.name, requestflaggroups.type, requestflaggroups.name FROM requestflags LEFT JOIN requestflaggroups ON requestflaggroups.id=requestflags.rfgroupid WHERE requestflags.name!='' ORDER BY requestflaggroups.id, requestflags.id;",flagsQuery);
    int counter=0;
    while(flagsQuery.next())
    {
        int bit=flagsQuery.value(0).toInt();
        quint64 flag=(quint64)1<<(bit-1);
        if(flag&flags)
        {
            if(counter>0)
                jsonData+=",";
            jsonData+="{\"name\":\""+flagsQuery.value(1).toString()+"\", \"group\":\""+flagsQuery.value(3).toString()+"\", \"value\":\"1\"}";
            counter++;
        }
    }
    jsonData+="]";
    jsonData+="}";
//    QMessageBox::information(this,PROGNAME,jsonData);
    html.replace("[JSON_DATA]",jsonData.toUtf8());

    PrintDialog *printDialog = new PrintDialog(html,QUrl::fromLocalFile(templatesFilePathName),this);
    printDialog->exec();
    delete printDialog;
}
//-------------------------------------------------------------------------------------
void RequestDialog::on_costDoubleSpinBox_valueChanged(double arg1)
{
    Q_UNUSED(arg1);
    ui->percentSummValueLabel->setText(num(ui->costDoubleSpinBox->value()*m_percent/100));
}
//-------------------------------------------------------------------------------------
void RequestDialog::refreshBonusUserComboBox(int defaultId)
{
    ui->bonusUserComboBox->clear();
    ui->bonusUserComboBox->addItem(tr("Нет"),0);

    QSqlQuery sqlQuery;
    QueryExec(this,"SELECT users.id,users.name FROM users WHERE users.enabled>0 OR users.id='"+num(defaultId)+"' ORDER BY users.name;",sqlQuery);
    while(sqlQuery.next())
    {
        ui->bonusUserComboBox->addItem(sqlQuery.value(1).toString(),sqlQuery.value(0));
    }
    ui->bonusUserComboBox->setCurrentIndex(ui->bonusUserComboBox->findData(defaultId));
}
//-------------------------------------------------------------------------------------
/*void RequestDialog::on_addDeviceComboBox_activated(int index)
{
    int deviceId=ui->addDeviceComboBox->currentData().toInt();
}*/
//-------------------------------------------------------------------------------------
void RequestDialog::On_agreementPushButton_clicked()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    quint64 agreementId=button->property("agreementId").toULongLong();
    if(!agreementId)
        return;
    AgreementDialog agreementDialog(agreementId,this);
    agreementDialog.hideNewRequestButton();
    agreementDialog.exec();
}
//-------------------------------------------------------------------------------------
// Обновляем средний балл исполнителя. Вызывать только при закрытии заявки, когда исполнитель фиксируется.
bool RequestDialog::updatePerformerRatingAvg(int performerId)
{
    if(!performerId)
    {
        return true;
    }
    QSqlQuery ratingQuery;
    if(!QueryExec(this,"SELECT rating FROM requests WHERE performerid="+num(performerId)+" AND rating>0 ORDER BY id DESC LIMIT 10;",ratingQuery))
    {
        return false;
    }
    double rating_avg=0;
    int count=0;
    while(ratingQuery.next())
    {
        int rating=ratingQuery.value(0).toInt();
//        qDebug()<<"user "<<performerId<<" rating "<<rating;
        if(rating>=1 && rating<=5)
        {
            rating_avg+=rating;
            count++;
        }
    }
//    qDebug()<<"sum "<<rating_avg<<" count "<<count;
    if(count>0)
    {
        rating_avg=rating_avg/double(count);
    }
    if(!QueryExec(this,"UPDATE users SET rating_avg="+QString::number(rating_avg,'f', 1).replace(',','.')+" WHERE id="+num(performerId)+";"))
    {
        return false;
    }
    return true;
}
//-------------------------------------------------------------------------------------
