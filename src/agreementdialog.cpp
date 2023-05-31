#include "agreementdialog.h"
#include "ui_agreementdialog.h"
#include "common_inc.h"
#include "clientsdialog.h"
#include "requestdialog.h"
#include "colorsqlquerymodel.h"
#include "printdialog.h"

extern unsigned int globaluid;
extern XMLSettings settings;

//-------------------------------------------------------------------------------------
AgreementDialog::AgreementDialog(int agreementid, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AgreementDialog),
    m_agreementid(agreementid),
    m_clientid(0)
{
    ui->setupUi(this);

	int office=0,types=0,trest=0;
    ui->statusComboBox->addItem(tr("Обслуживается"),1);
    ui->statusComboBox->addItem(tr("Расторгнут"),2);
    // Requests.
    requestsQueryModel = new ColorSqlQueryModel;
    ui->requestsTableView->setModel(requestsQueryModel);
    // Messages.
    m_messagesQueryModel = new QSqlQueryModel;
    ui->messagesTableView->setModel(m_messagesQueryModel);

    QSettings registrySettings(ORGNAME,PROGNAME);
    restoreGeometry(registrySettings.value("agreement_geometry").toByteArray());
    ui->deviceTableWidget->horizontalHeader()->restoreState(registrySettings.value("agreement_device_table_state").toByteArray());

    if(!m_agreementid || !checkRights(this,globaluid,CSRIGHT_EXEC_ADMIN))
// Инкассация доступна только администраторам и только для созданного заказа.
    {
        ui->collectComboBox->setEnabled(false);
    }
    if(!m_agreementid)
	{
        ui->unlockPushButton->setVisible(false);
	}
    if(m_agreementid)
    {
//        if(!checkRights(this,globaluid,CSRIGHT_EXEC_ADMIN)) // Редактирование доступно только первым двум пользователям.
//            ui->unlockPushButton->setVisible(false);
        QSqlQuery agreementQuery;
		QueryExec(this,"SELECT officeid, types, trest, startdate, timelimit, status, cost, collected, client, json, alertdate, cuser, ctime FROM agreements WHERE id='"+num(m_agreementid)+"';",agreementQuery);
        if(!agreementQuery.next())
        {
            QMetaObject::invokeMethod(this, "reject", Qt::QueuedConnection); // Close dialog.
            return;
        }
        ui->agreementGroupBox->setEnabled(false);
		ui->typeGroupBox->setEnabled(false);
        ui->devicesGroupBox->setEnabled(false);
		ui->vdpoGroupBox->setEnabled(false);

//        ui->okPushButton->setEnabled(false);

        ui->numberSpinBox->setValue(m_agreementid);
        office=agreementQuery.value(0).toInt();
		types=agreementQuery.value(1).toInt();
        trest=agreementQuery.value(2).toInt();
        ui->startDateEdit->setDate(QDate::fromString(agreementQuery.value(3).toString(),"yyyy-MM-dd"));
        ui->timeLimitSpinBox->setValue(agreementQuery.value(4).toInt());
        ui->statusComboBox->setCurrentIndex(ui->statusComboBox->findData(agreementQuery.value(5).toInt()));
        if(agreementQuery.value(5).toInt()==2) // Если расторгнут
		{
            ui->newRequestPushButton->setEnabled(false);
		}
        double cost=agreementQuery.value(6).toDouble();
        unsigned int collected=agreementQuery.value(7).toUInt();
        ui->costDoubleSpinBox->setValue(cost);
        m_clientid=agreementQuery.value(8).toInt();
        // Инкассация
//        QMessageBox::information(this,PROGNAME,num());
        if(collected)// Если не пусто, значит была инкассация.
        {
            int collectedDecoded=encodeCollected(collected)-m_agreementid;
            ui->collectComboBox->setCurrentIndex(1);
            ui->collectComboBox->setItemText(1,tr("Принято ")+num(collectedDecoded));
            ui->collectComboBox->setItemData(1,collectedDecoded);
//            if(globaluid!=1 && globaluid!=2)
            ui->costDoubleSpinBox->setReadOnly(true);
            if(!checkRights(this,globaluid,CSRIGHT_EXEC_ADMIN))
			{
                ui->unlockPushButton->setVisible(false); // Редактирование инкассированного заказа только для администратора.
			}
        }
        QSqlQuery clientSqlQuery;
        QueryExec(this,"SELECT surname, name, address, phone1, phone2 FROM clients WHERE id='"+num(m_clientid)+"';",clientSqlQuery);
        if(clientSqlQuery.next())
        {
            ui->clientPushButton->setText(clientSqlQuery.value(0).toString()+" "+clientSqlQuery.value(1).toString());
            ui->clientAddressLabel->setText(clientSqlQuery.value(2).toString());
            ui->clientPhone1Label->setText(clientSqlQuery.value(3).toString());
            ui->clientPhone2Label->setText(clientSqlQuery.value(4).toString());
        }
		m_joExtra=QJsonDocument::fromJson(agreementQuery.value(9).toByteArray()).object();
		if(m_joExtra.contains("devices"))
		{
			refreshDeviceTableWidget(m_joExtra["devices"].toArray());
		}
		if(m_joExtra.contains("VDPO"))
		{
			QJsonObject joVdpo=m_joExtra["VDPO"].toObject();
			ui->objectLineEdit->setText(joVdpo.value("object").toString());
			ui->proverkaTehSostojaniaComboBox->setCurrentText(joVdpo.value("proverkaTehSostojania").toString());
			ui->oborudovanieLineEdit->setText(joVdpo.value("oborudovanie").toString());
			ui->celProverkiComboBox->setCurrentText(joVdpo.value("celProverki").toString());
			ui->vytyajnayaVentiljaciaComboBox->setCurrentText(joVdpo.value("vytyajnayaVentiljacia").toString());
			ui->protivopojarnayaRazdelkaComboBox->setCurrentText(joVdpo.value("protivopojarnayaRazdelka").toString());
			ui->karmanyDymovyhKanalovComboBox->setCurrentText(joVdpo.value("karmanyDymovyhKanalov").toString());
			ui->ispravnostTrubComboBox->setCurrentText(joVdpo.value("ispravnostTrub").toString());
			ui->dlinaTrubComboBox->setCurrentText(joVdpo.value("dlinaTrub").toString());
			ui->ogolovkiTrubComboBox->setCurrentText(joVdpo.value("ogolovkiTrub").toString());
			ui->nalichieProektaComboBox->setCurrentText(joVdpo.value("nalichieProekta").toString());
			ui->toplivoComboBox->setCurrentText(joVdpo.value("toplivo").toString());
			ui->vidObjectaComboBox->setCurrentText(joVdpo.value("vidObjecta").toString());
			ui->srokDeistviaAktaComboBox->setCurrentText(joVdpo.value("srokDeistviaAkta").toString());
			refreshProverkaTableWidget();
		}
        ui->alertDateEdit->setDate(QDate::fromString(agreementQuery.value(10).toString(),"yyyy-MM-dd"));
        setWindowTitle(tr("Договор %1").arg(m_agreementid));
        refreshRequests();
        refreshMessages();
    }
    else // New
    {
//        ui->unlockPushButton->setVisible(false);
        ui->startDateEdit->setDate(QDate::currentDate());
/*        ui->officeComboBox->setCurrentIndex(-1);
        ui->typeComboBox->setCurrentIndex(-1);
        ui->trestComboBox->setCurrentIndex(-1);
        ui->deviceTypeComboBox->setCurrentIndex(-1);*/
        ui->newRequestPushButton->setEnabled(false);
        ui->alertDateEdit->setDate(QDate::currentDate());
        ui->newMessageLineEdit->setEnabled(false);
    }
    refreshOfficeComboBox(office);
//    refreshTypeComboBox(type);
	refreshAgreementTypes(types);
    refreshTrestComboBox(trest);
    refreshDocumentsButtons();
//    refreshDeviceTypesComboBox(devicetype);

	// !!!!!!!!  REFILL field 'types'  !!!!!!!!
/*	{
		QSqlQuery agreementTypeQuery;
		QueryExec(this,"SELECT id, type FROM agreements WHERE id>12000;",agreementTypeQuery);
		while(agreementTypeQuery.next())
		{
			int agreementId=agreementTypeQuery.value(0).toInt();
			int typeId=agreementTypeQuery.value(1).toInt();
			int types=1<<(typeId-1);
			qDebug()<<"agreement typeId: "<<typeId<<" types: "<<types;
			QueryExec(this,"UPDATE agreements SET types="+num(types)+" WHERE id="+num(agreementId)+";");
		}
	}*/
}
//-------------------------------------------------------------------------------------
AgreementDialog::~AgreementDialog()
{
    delete ui;
}
//-------------------------------------------------------------------------------------
void AgreementDialog::closeEvent(QCloseEvent *event)
{
    QSettings registrySettings(ORGNAME,PROGNAME);
    registrySettings.setValue("agreement_device_table_state",ui->deviceTableWidget->horizontalHeader()->saveState());
    registrySettings.setValue("agreement_geometry", saveGeometry());
    QDialog::closeEvent(event);
}
//-------------------------------------------------------------------------------------
void AgreementDialog::refreshOfficeComboBox(int defaultId)
{
    ui->officeComboBox->clear();
    QSqlQuery officeSqlQuery;
    QueryExec(this,"SELECT id,name FROM offices WHERE enabled>0 OR id='"+num(defaultId)+"';",officeSqlQuery);
    while(officeSqlQuery.next())
    {
        ui->officeComboBox->addItem(officeSqlQuery.value(1).toString(),officeSqlQuery.value(0));
    }
    ui->officeComboBox->setCurrentIndex(ui->officeComboBox->findData(defaultId));
}
//-------------------------------------------------------------------------------------
/*void AgreementDialog::refreshTypeComboBox(int defaultId)
{
    ui->typeComboBox->clear();
//    ui->typeComboBox->addItem(tr("Не выбран"),0);
    QSqlQuery typeQuery;
    QueryExec(this,"SELECT id,name FROM agreementtypes WHERE enabled>0 OR id='"+num(defaultId)+"';",typeQuery);
    while(typeQuery.next())
    {
        ui->typeComboBox->addItem(typeQuery.value(0).toString()+". "+typeQuery.value(1).toString(),typeQuery.value(0));
    }
    ui->typeComboBox->setCurrentIndex(ui->typeComboBox->findData(defaultId));
}*/
//-------------------------------------------------------------------------------------
void AgreementDialog::refreshTrestComboBox(int defaultId)
{
    ui->trestComboBox->clear();
//    ui->trestComboBox->addItem(tr("Не выбран"),0);
    QSqlQuery trestQuery;
    QueryExec(this,"SELECT id,name FROM trests WHERE enabled>0 OR id='"+num(defaultId)+"';",trestQuery);
    while(trestQuery.next())
    {
        ui->trestComboBox->addItem(trestQuery.value(0).toString()+". "+trestQuery.value(1).toString(),trestQuery.value(0));
    }
    ui->trestComboBox->setCurrentIndex(ui->trestComboBox->findData(defaultId));
}
//-------------------------------------------------------------------------------------
void AgreementDialog::refreshDocumentsButtons()
{
	QList<QWidget *> widgets = ui->documentsVerticalLayout->findChildren<QWidget *>();
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
    QString agreementPath=settings.value("interface","","templatespath","",programDataPath()+"/templates")+"/agreement";
    QDir dir(agreementPath);
    QStringList filelist=dir.entryList(QStringList("*.html"),QDir::Files);
    for(int i=0;i<filelist.size();i++)
	{
        filelist[i].prepend(agreementPath+"/");
	}
    // Type dir.
	quint32 types=selectedTypes();
	for(int i=0;i<32;i++)
	{
		quint32 flag=1<<i;
		if(types&flag)
		{
			QString typeDirName="type"+num(i+1);
			QString agreementTypePath=agreementPath+"/"+typeDirName;
			QDir agreementTypeDir(agreementTypePath);
			QStringList typeFileList=agreementTypeDir.entryList(QStringList("*.html"),QDir::Files);
			for(int i=0;i<typeFileList.size();i++)
				typeFileList[i].prepend(agreementTypePath+"/");
			filelist.append(typeFileList);
			// Trest dir.
			QString trestDirName="trest"+ui->trestComboBox->currentData().toString();
			QString agreementTrestPath=agreementPath+"/"+typeDirName+"/"+trestDirName;
			QDir agreementTrestDir(agreementTrestPath);
			QStringList trestFileList=agreementTrestDir.entryList(QStringList("*.html"),QDir::Files);
			for(int i=0;i<trestFileList.size();i++)
				trestFileList[i].prepend(agreementTrestPath+"/");
			filelist.append(trestFileList);
		}
	}
// Заполняем комбобокс.
    for(int i=0;i<filelist.size();i++)
    {
        QFileInfo fileInfo(filelist.at(i));
//        ui.documentsComboBox->addItem(fileInfo.baseName(),filelist.at(i));
        // Кнопки
		QPushButton *button=new QPushButton(fileInfo.baseName(), ui->documentsVerticalLayout->widget());
        button->setProperty("filePathName",filelist.at(i));
		ui->documentsVerticalLayout->addWidget(button);
        connect(button,SIGNAL(clicked()),SLOT(On_printDocumentPushButton_clicked()));
    }
}
//-------------------------------------------------------------------------------------
void AgreementDialog::refreshAgreementTypes(int types)
{
	// Clear.
	for(int i=0;i<m_typeCheckBoxList.size();i++)
	{
		m_typeCheckBoxList.at(i)->deleteLater();
	}
	m_typeCheckBoxList.clear();
	// Fill.
	QSqlQuery agreementTypeQuery;
	QueryExec(this,"SELECT id, name FROM agreementtypes WHERE enabled>0;",agreementTypeQuery);
	while(agreementTypeQuery.next())
	{
		QCheckBox *checkBox=new QCheckBox(agreementTypeQuery.value(1).toString(), ui->typeVerticalLayout->widget());
		checkBox->setProperty("typeId",agreementTypeQuery.value(0));
		checkBox->setCheckState(Qt::Unchecked); // AND initialize check state
		if(isTypeSelected(types,agreementTypeQuery.value(0).toInt()))
		{
			checkBox->setCheckState(Qt::Checked); // AND initialize check state
		}
		ui->typeVerticalLayout->addWidget(checkBox);
		m_typeCheckBoxList.append(checkBox);
	}
}
//-------------------------------------------------------------------------------------
bool AgreementDialog::isTypeSelected(int types, int id)
{
	int flag=1<<(id-1);
	debug("isTypeSelected types: "+num(types)+" id: "+num(id)+" flag: "+num(flag));
	return (flag&types);
}
//-------------------------------------------------------------------------------------
int AgreementDialog::selectedTypes()
{
	int selTypes=0;
	for(int i=0;i<m_typeCheckBoxList.size();i++)
	{
		if(m_typeCheckBoxList.at(i)->isChecked())
		{
			int typeId=m_typeCheckBoxList.at(i)->property("typeId").toInt();
			selTypes|=(1<<(typeId-1));
		}
	}
//	qDebug()<<"selected types: "<<selTypes;
	return selTypes;
}
//-------------------------------------------------------------------------------------
/*void AgreementDialog::refreshDeviceTypes()
{
    ui->deviceTypeListWidget->clear();
    QSqlQuery deviceTypeQuery;
    QueryExec(this,"SELECT devicetypes.id, name, agreementdevicetypes.id FROM devicetypes LEFT JOIN agreementdevicetypes ON dtid=devicetypes.id AND aid='"+num(m_agreementid)+"' WHERE enabled>0 OR aid='"+num(m_agreementid)+"' ORDER BY name;",deviceTypeQuery);
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
void AgreementDialog::refreshRequests()
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
    filter+=QString((filter.isEmpty())?" WHERE":" AND")+" requests.agreementid='"+num(m_agreementid)+"'";
//    QMessageBox::information(this,PROGNAME,tr("Фильтр: ")+filter);
    QModelIndex savedIndex=ui->requestsTableView->currentIndex();
    QueryModelExec(this,"SELECT requests.id AS requestsid, requests.ctime, performdate, clients.address, clients.surname, clients.name AS cname, clients.phone1, clients.phone2, devicename, defect, cost, performer.name AS pname, users.name AS uname, states.name, requests.collection FROM requests LEFT JOIN states ON states.id=requests.status LEFT JOIN clients ON clients.id = requests.clientid LEFT JOIN users ON users.id = requests.cuserid LEFT JOIN users AS performer ON performer.id = requests.performerid"+filter+" GROUP BY requests.id"+sortString+";",requestsQueryModel);
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
    ui->requestsTableView->resizeColumnsToContents();
    ui->requestsTableView->setCurrentIndex(savedIndex);
/*    if(globaluid==1 || globaluid==2)
    {
        double total=0;
        for(int i=0;i<requestsQueryModel->rowCount();i++)
        {
            total+=requestsQueryModel->record(i).value(11).toDouble();
        }
        ui->statusBar->showMessage(QString::number(total,'f',2));
    }*/
}
//-------------------------------------------------------------------------------------
void AgreementDialog::refreshMessages()
{
    QModelIndex savedIndex=ui->messagesTableView->currentIndex();
    QueryModelExec(this,"SELECT messages.id, messages.message, users.name, messages.ctime FROM messages LEFT JOIN users ON users.id = messages.cuser WHERE chat_type='"+num(CHAT_TYPE_AGREEMENT)+"' AND chat_id='"+num(m_agreementid)+"' ORDER BY messages.id;",m_messagesQueryModel);
    int column_cnt=1;
    m_messagesQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Сообщение"));
    m_messagesQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Создал"));
    m_messagesQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Дата"));
    ui->messagesTableView->hideColumn(0);
    ui->messagesTableView->resizeColumnsToContents();
    ui->messagesTableView->setCurrentIndex(savedIndex);
    QTimer::singleShot(0,(ui->messagesTableView),SLOT(scrollToBottom())); // Скроллим вниз, но после обновления tableview.
}
//-------------------------------------------------------------------------------------
void AgreementDialog::on_okPushButton_clicked()
{
    QString status=ui->statusComboBox->currentData().toString();
    if(status.toInt()==1 && ui->alertDateEdit->date()<QDate::currentDate())
    {
        if(QMessageBox::question(this, PROGNAME, tr("Дата следующего ТО не задана!"),tr("Вернуться"),tr("Сохранить как есть"))==0)
		{
            return;
		}
    }

	QueryParameters qParam;
	qParam.append(":officeid",ui->officeComboBox->currentData().toString());
	qParam.append(":types",selectedTypes());
	qParam.append(":startdate",ui->startDateEdit->date().toString("yyyy-MM-dd"));
	qParam.append(":timelimit",num(ui->timeLimitSpinBox->value()));
	qParam.append(":status",status);
	qParam.append(":client",m_clientid);
	qParam.append(":trest",ui->trestComboBox->currentData().toString());
	qParam.append(":cost",num(ui->costDoubleSpinBox->value()));
	qParam.append(":alertdate", ui->alertDateEdit->date().toString("yyyy-MM-dd"));
    unsigned int collected=0;
//    QMessageBox::information(this,PROGNAME,num(collected));
    ui->deviceTableWidget->setCurrentItem(0);
	QJsonArray jaDevices;
    for(int i=0;i<ui->deviceTableWidget->rowCount();i++)
    {
        QTableWidgetItem * deviceNameItem=ui->deviceTableWidget->item(i,0);
        QTableWidgetItem * quantityItem=ui->deviceTableWidget->item(i,1);
        if(deviceNameItem==0)
		{
            continue;
		}
        QString deviceName=deviceNameItem->text();
        QString quantity="0";
        if(quantityItem!=0)
		{
            quantity=quantityItem->text();
		}
//        qDebug(QString("device name: %1").arg(deviceName).toUtf8());
        if(deviceName.isEmpty())
		{
            continue;
		}
		QJsonArray jaDevice;
		jaDevice.append(escapeJson(deviceName));
		jaDevice.append(escapeJson(quantity));
		jaDevices.append(jaDevice);
	}
	m_joExtra["devices"]=jaDevices;
	getVdpo();

	qParam.append(":json",QJsonDocument(m_joExtra).toJson());
//	debug(json);
    if(m_agreementid)
    {
		QString updateStr;
        if(checkRights(this,globaluid,CSRIGHT_EXEC_ADMIN)) // Если админ.
        {
            if(ui->collectComboBox->currentIndex()==1)
            {
                if(ui->collectComboBox->currentData().toUInt()) // Если прочитанная инкассация была ненулевой, кодируем её.
				{
                    collected=encodeCollected(m_agreementid+ui->collectComboBox->currentData().toUInt());
				}
                else
				{
                    collected=encodeCollected(m_agreementid+int(ui->costDoubleSpinBox->value())); // Иначе создаём на имеющуюся сумму.
				}
            }
            if(ui->agreementGroupBox->isEnabled())
			{
				updateStr+=", officeid=:officeid, types=:types, startdate=:startdate, timelimit=:timelimit, status=:status, client=:client";
			}
			qParam.append(":collected",collected);
			updateStr+=", collected=:collected";
        }
        if(ui->devicesGroupBox->isEnabled())
        {
			updateStr+=", trest=:trest";
			updateStr+=", json=:json";
        }
        if(!ui->costDoubleSpinBox->isReadOnly())
        {
			updateStr+=", cost=:cost";
        }
		if(!QueryExecPrepared(this,"UPDATE agreements SET alertdate=:alertdate"+updateStr+"  WHERE id='"+num(m_agreementid)+"';", qParam))
		{
            return;
		}
    }
    else // New.
    {
//        qDebug(tr("3").toUtf8());
        if(ui->officeComboBox->currentIndex()==-1)
        {
            QMessageBox::information(this,PROGNAME,tr("Выберите офис."));
            return;
        }
		if(!selectedTypes())
        {
            QMessageBox::information(this,PROGNAME,tr("Выберите тип договора."));
            return;
        }
        if(ui->trestComboBox->currentIndex()==-1)
        {
            QMessageBox::information(this,PROGNAME,tr("Выберите трест."));
            return;
        }
        if(!m_clientid)
        {
            QMessageBox::information(this,PROGNAME,tr("Выберите клиента."));
            return;
        }
/*        if(ui->costDoubleSpinBox->value()==0)
        {
            QMessageBox::information(this,PROGNAME,tr("Выберите стоимость договора."));
            return;
        }*/
        QString idQuery1,idQuery2;
        if(ui->numberSpinBox->value()>0)
        {
            idQuery1="id, ";
            idQuery2="'"+num(ui->numberSpinBox->value())+"', ";
        }
//        qDebug(tr("4").toUtf8());
        QSqlQuery insertSqlQuery;
		if(!QueryExecPrepared(this,"INSERT INTO agreements("+idQuery1+"officeid, types, trest, startdate, timelimit, status, cost, collected, client, json, alertdate, cuser, ctime) VALUES("+idQuery2+":officeid, :types, :trest, :startdate, :timelimit, :status, :cost,'"+num(collected)+"' , :client, :json, :alertdate, "+num(globaluid)+",'"+timestamp()+"');", qParam, insertSqlQuery))
            return;
        m_agreementid=insertSqlQuery.lastInsertId().toInt();
    }
//    qDebug(tr("5").toUtf8());
    accept();
}
//-------------------------------------------------------------------------------------
void AgreementDialog::on_clientPushButton_clicked()
{
    ClientsDialog *clientsDialog = new ClientsDialog(m_clientid,this);
    if(clientsDialog->exec()==QDialog::Accepted)
    {
        m_clientid=clientsDialog->clientId();
        QString name="...",phone1="-",phone2="-",address="-";
        QSqlQuery clientSqlQuery;
        QueryExec(this,"SELECT surname, name, address, phone1, phone2 FROM clients WHERE id='"+num(m_clientid)+"';",clientSqlQuery);
        if(clientSqlQuery.next())
        {
            name=clientSqlQuery.value(0).toString()+" "+clientSqlQuery.value(1).toString();
            address=clientSqlQuery.value(2).toString();
            phone1=clientSqlQuery.value(3).toString();
            phone2=clientSqlQuery.value(4).toString();
        }
        ui->clientPushButton->setText(name);
        ui->clientAddressLabel->setText(address);
        ui->clientPhone1Label->setText(phone1);
        ui->clientPhone2Label->setText(phone2);
    }
    delete clientsDialog;
}
//-------------------------------------------------------------------------------------
void AgreementDialog::on_newRequestPushButton_clicked()
{
    RequestDialog *requestDialog = new RequestDialog(0,ui->officeComboBox->currentData().toInt(),m_agreementid,false,this);
    if(requestDialog->exec()==QDialog::Accepted)
    {
        refreshRequests();
    }
    delete requestDialog;
}
//-------------------------------------------------------------------------------------
void AgreementDialog::on_unlockPushButton_clicked()
{
    if(ui->unlockPushButton->isChecked()) // Разблокируем.
    {
        if(checkRights(this,globaluid,CSRIGHT_EXEC_ADMIN))
        {
            ui->agreementGroupBox->setEnabled(true);
			ui->typeGroupBox->setEnabled(true);
            ui->devicesGroupBox->setEnabled(true);
			ui->vdpoGroupBox->setEnabled(true);
        }
        else
        {
//            if(ui->collectComboBox->currentIndex()==0) // Разрешаем редактирование только в не инкассированном договоре.
				if(ui->collectComboBox->currentIndex()<1)
				{
					ui->agreementGroupBox->setEnabled(true);
					ui->typeGroupBox->setEnabled(true);
					ui->devicesGroupBox->setEnabled(true);
					ui->vdpoGroupBox->setEnabled(true);
				}
		}
        ui->unlockPushButton->setIcon(QIcon(":/images/unlock64.png"));
    }
    else // Блокируем.
    {
        ui->agreementGroupBox->setEnabled(false);
		ui->typeGroupBox->setEnabled(false);
//            ui->okPushButton->setEnabled(false);
        ui->devicesGroupBox->setEnabled(false);
		ui->vdpoGroupBox->setEnabled(false);
		ui->unlockPushButton->setIcon(QIcon(":/images/lock64.png"));
    }
}
//-------------------------------------------------------------------------------------
void AgreementDialog::on_requestsTableView_activated(const QModelIndex &index)
{
    int id=requestsQueryModel->record(index.row()).field("requestsid").value().toInt();
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
void AgreementDialog::On_printDocumentPushButton_clicked()
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
    QString jsonData="{";
    // C L I E N T
    QString cSurname,cName,cPhone1,cPhone2,cEmail,cAddress,cPassport,cPAddress,cInn,cKpp,cOkpo,cBank,cRschet,cKschet,cBik,cOgrn,cOkved,cStoreAddress,cStorePhone,cGendirector,cManager1,cManager1Phone,cManager1Email,cManager2,cManager2Phone,cManager2Email,cSite,cPayType,cDelivery,cBrands,cWorktime;
    QSqlQuery clientQuery;
    if(QueryExec(this,"SELECT surname, name, phone1, phone2, email, address, passport, paddress, inn, kpp, okpo, bank, rschet, kschet, bik, ogrn, okved, store_address, store_phone, gendirector, manager1, manager1_phone, manager1_email, manager2, manager2_phone, manager2_email, site, paytype, delivery, brands, worktime FROM clients WHERE id='"+num(m_clientid)+"';",clientQuery))
    {
        if(clientQuery.next())
        {
            int val=0;
            cSurname=clientQuery.value(val++).toString();
            cName=clientQuery.value(val++).toString();
            cPhone1=clientQuery.value(val++).toString();
            cPhone2=clientQuery.value(val++).toString();
            cEmail=clientQuery.value(val++).toString();
            cAddress=clientQuery.value(val++).toString();
            cPassport=clientQuery.value(val++).toString();
            cPAddress=clientQuery.value(val++).toString();
            cInn=clientQuery.value(val++).toString();
            cKpp=clientQuery.value(val++).toString();
            cOkpo=clientQuery.value(val++).toString();
            cBank=clientQuery.value(val++).toString();
            cRschet=clientQuery.value(val++).toString();
            cKschet=clientQuery.value(val++).toString();
            cBik=clientQuery.value(val++).toString();
            cOgrn=clientQuery.value(val++).toString();
            cOkved=clientQuery.value(val++).toString();
            cStoreAddress=clientQuery.value(val++).toString();
            cStorePhone=clientQuery.value(val++).toString();
            cGendirector=clientQuery.value(val++).toString();
            cManager1=clientQuery.value(val++).toString();
            cManager1Phone=clientQuery.value(val++).toString();
            cManager1Email=clientQuery.value(val++).toString();
            cManager2=clientQuery.value(val++).toString();
            cManager2Phone=clientQuery.value(val++).toString();
            cManager2Email=clientQuery.value(val++).toString();
            cSite=clientQuery.value(val++).toString();
            cPayType=clientQuery.value(val++).toString();
            cDelivery=clientQuery.value(val++).toString();
            cBrands=clientQuery.value(val++).toString();
            cWorktime=clientQuery.value(val++).toString();
        }
    }
    html.replace("[C_SURNAME]",cSurname.toUtf8());
    html.replace("[C_NAME]",cName.toUtf8());
    html.replace("[C_PHONE1]",cPhone1.toUtf8());
    html.replace("[C_PHONE2]",cPhone2.toUtf8());
    html.replace("[C_EMAIL]",cEmail.toUtf8());
    html.replace("[C_ADDRESS]",cAddress.toUtf8());
    html.replace("[C_PASSPORT]",cPassport.toUtf8());
    html.replace("[C_PADDRESS]",cPAddress.toUtf8());
    html.replace("[C_INN]",cInn.toUtf8());
    html.replace("[C_KPP]",cKpp.toUtf8());
    html.replace("[C_OKPO]",cOkpo.toUtf8());
    html.replace("[C_BANK]",cBank.toUtf8());
    html.replace("[C_RSCHET]",cRschet.toUtf8());
    html.replace("[C_KSCHET]",cKschet.toUtf8());
    html.replace("[C_BIK]",cBik.toUtf8());
    html.replace("[C_OGRN]",cOgrn.toUtf8());
    html.replace("[C_OKVED]",cOkved.toUtf8());
    html.replace("[C_STORE_ADDRESS]",cStoreAddress.toUtf8());
    html.replace("[C_STORE_PHONE]",cStorePhone.toUtf8());
    html.replace("[C_GENDIRECTOR]",cGendirector.toUtf8());
    html.replace("[C_MANAGER1]",cManager1.toUtf8());
    html.replace("[C_MANAGER1_PHONE]",cManager1Phone.toUtf8());
    html.replace("[C_MANAGER1_EMAIL]",cManager1Email.toUtf8());
    html.replace("[C_MANAGER2]",cManager2.toUtf8());
    html.replace("[C_MANAGER2_PHONE]",cManager2Phone.toUtf8());
    html.replace("[C_MANAGER2_EMAIL]",cManager2Email.toUtf8());
    html.replace("[C_SITE]",cSite.toUtf8());
    html.replace("[C_PAYTYPE]",cPayType.toUtf8());
    html.replace("[C_DELIVERY]",cDelivery.toUtf8());
    html.replace("[C_BRANDS]",cBrands.toUtf8());
    html.replace("[C_WORKTIME]",cWorktime.toUtf8());
    // AGREEMENT
    QSqlQuery agreementQuery;
	QueryExec(this,"SELECT agreements.ctime, agreements.startdate, users.name AS creator_name, cost FROM agreements LEFT JOIN users ON users.id=agreements.cuser WHERE agreements.id='"+num(m_agreementid)+"';",agreementQuery);
    if(!agreementQuery.next())
        return;
    html.replace("[AGREEMENT_ID]",num(m_agreementid).toUtf8());
    QDateTime currentDateTime=QDateTime::currentDateTime();
    html.replace("[CURRENT_DATE]",currentDateTime.toString("dd.MM.yyyy").toUtf8());
    QDateTime createDateTime=QDateTime::fromString(agreementQuery.value(0).toString(),DATETIME_FORMAT);
    html.replace("[AGREEMENT_CREATE_DATE]",createDateTime.toString("dd.MM.yyyy").toUtf8()); // Дата добавления
    QDateTime startDateTime=QDateTime::fromString(agreementQuery.value(1).toString(),DATETIME_FORMAT);
    html.replace("[AGREEMENT_START_DATE]",startDateTime.toString("dd.MM.yyyy").toUtf8());
    html.replace("[AGREEMENT_CREATOR_NAME]",agreementQuery.value(2).toString().toUtf8());
    html.replace("[AGREEMENT_COST]",agreementQuery.value(3).toString().toUtf8());
	html.replace("[JSON_DATA]",QJsonDocument(m_joExtra).toJson());

    if(html.contains("[CURRENT_USER]"))
    {
        html.replace("[CURRENT_USER]",getUserName(this,globaluid).toUtf8());
    }
//    QMessageBox::information(this,PROGNAME,jsonData);

    PrintDialog *printDialog = new PrintDialog(html,QUrl::fromLocalFile(filePathName),this);
    printDialog->exec();
    delete printDialog;

}
//-------------------------------------------------------------------------------------
void AgreementDialog::refreshDeviceTableWidget(QJsonArray deviceArray)//QByteArray jsonData)
{
    ui->deviceTableWidget->clear();
    // Load json data.
/*    QJsonParseError jsonParseError;
    QJsonDocument jsonDoc=QJsonDocument::fromJson(jsonData,&jsonParseError);
    if(jsonParseError.error!=QJsonParseError::NoError)
    {
        QMessageBox::warning(this,"",jsonParseError.errorString());
        return;
    }
    QJsonObject jsonObject=jsonDoc.object();
    QJsonArray deviceArray = jsonObject["devices"].toArray();
*/

    ui->deviceTableWidget->setRowCount(deviceArray.size());
//    qDebug("deviceArray size: "+num(deviceArray.size()).toUtf8());
//    ui->deviceTableWidget->setColumnCount(2);
    for(int i=0;i<deviceArray.size();i++)
    {
        QJsonArray rowArray = deviceArray.at(i).toArray();
        QTableWidgetItem *newItem1 = new QTableWidgetItem(rowArray.at(0).toString());
        ui->deviceTableWidget->setItem(i,0,newItem1);
        QTableWidgetItem *newItem2 = new QTableWidgetItem(rowArray.at(1).toString());
        ui->deviceTableWidget->setItem(i,1,newItem2);
    }
}
//-------------------------------------------------------------------------------------
void AgreementDialog::on_addDevicePushButton_clicked()
{
    ui->deviceTableWidget->setRowCount(ui->deviceTableWidget->rowCount()+1);
}
//-------------------------------------------------------------------------------------
void AgreementDialog::on_removeDevicePushButton_clicked()
{
    ui->deviceTableWidget->removeRow(ui->deviceTableWidget->currentRow());
}
//-------------------------------------------------------------------------------------
void AgreementDialog::hideNewRequestButton()
{
    ui->newRequestPushButton->hide();
}
//-------------------------------------------------------------------------------------
void AgreementDialog::on_newMessagePushButton_clicked()
{
    if(!m_agreementid)
    {
        return;
    }
    if(!QueryExec(this,"INSERT INTO messages(chat_type, chat_id, message, cuser, ctime) VALUES("+num(CHAT_TYPE_AGREEMENT)+", "+num(m_agreementid)+", '"+escapeQuotes(ui->newMessageLineEdit->text().trimmed())+"', '"+num(globaluid)+"','"+timestamp()+"');"))
    {
        return;
    }
    ui->newMessageLineEdit->clear();
    refreshMessages();
}
//-------------------------------------------------------------------------------------
void AgreementDialog::on_newMessageLineEdit_returnPressed()
{
    on_newMessagePushButton_clicked();
}
//-------------------------------------------------------------------------------------
void AgreementDialog::on_newProverkaToolButton_clicked()
{
	QJsonObject joProverkaRow;
	ProverkaDialog proverkaDialog(joProverkaRow,this);
	if(proverkaDialog.exec()!=QDialog::Accepted)
	{
		return;
	}
	QJsonObject joVDPO=m_joExtra["VDPO"].toObject();
	QJsonArray jaProverka=joVDPO["proverka"].toArray();
	jaProverka.append(joProverkaRow);
	joVDPO["proverka"]=jaProverka;
	m_joExtra["VDPO"]=joVDPO;
	refreshProverkaTableWidget();
}
//-------------------------------------------------------------------------------------
void AgreementDialog::on_editProverkaToolButton_clicked()
{
	QList<QTableWidgetItem *> itemList=ui->proverkaTableWidget->selectedItems();
	if(!itemList.size())
	{
		return;
	}
	int row=itemList.at(0)->row();
	QJsonObject joVDPO=m_joExtra["VDPO"].toObject();
	QJsonArray jaProverka=joVDPO["proverka"].toArray();
	if(row>=jaProverka.size())
	{
		return;
	}
	QJsonObject joProverkaRow=jaProverka.at(row).toObject();
	ProverkaDialog proverkaDialog(joProverkaRow,this);
	proverkaDialog.setWindowTitle(tr("Редактирование строки"));
	if(proverkaDialog.exec()!=QDialog::Accepted)
	{
		return;
	}
	jaProverka.replace(row,joProverkaRow);
	joVDPO["proverka"]=jaProverka;
	m_joExtra["VDPO"]=joVDPO;

	refreshProverkaTableWidget();
}
//-------------------------------------------------------------------------------------
void AgreementDialog::on_proverkaTableWidget_activated(const QModelIndex &index)
{
	Q_UNUSED(index)
	on_editProverkaToolButton_clicked();
}
//-------------------------------------------------------------------------------------
void AgreementDialog::on_removeProverkaToolButton_clicked()
{
	QList<QTableWidgetItem *> itemList=ui->proverkaTableWidget->selectedItems();
	if(!itemList.size())
	{
		return;
	}
	int row=itemList.at(0)->row();
	QJsonObject joVDPO=m_joExtra["VDPO"].toObject();
	QJsonArray jaProverka=joVDPO["proverka"].toArray();
	if(row>=jaProverka.size())
	{
		return;
	}

	if(QMessageBox::question(this, PROGNAME, tr("Удалить строку?"),tr("Да"),tr("Нет"))!=0)
	{
		return;
	}
	jaProverka.removeAt(row);
	joVDPO["proverka"]=jaProverka;
	m_joExtra["VDPO"]=joVDPO;
	refreshProverkaTableWidget();
}
//-------------------------------------------------------------------------------------
void AgreementDialog::refreshProverkaTableWidget()
{
	ui->proverkaTableWidget->clear();
	QJsonArray jaProverka=m_joExtra["VDPO"].toObject()["proverka"].toArray();
	ui->proverkaTableWidget->setRowCount(jaProverka.size());
	for(int i=0;i<jaProverka.size();i++)
	{
		QJsonObject joRow=jaProverka.at(i).toObject();
		QTableWidgetItem *newItem1 = new QTableWidgetItem(joRow.value("tipKanala").toString());
		ui->proverkaTableWidget->setItem(i,0,newItem1);
		QTableWidgetItem *newItem2 = new QTableWidgetItem(joRow.value("materialKanala").toString());
		ui->proverkaTableWidget->setItem(i,1,newItem2);
		QTableWidgetItem *newItem3 = new QTableWidgetItem(joRow.value("diametrKanala").toString());
		ui->proverkaTableWidget->setItem(i,2,newItem3);
		QTableWidgetItem *newItem4 = new QTableWidgetItem(joRow.value("gazoispOborudovanie").toString());
		ui->proverkaTableWidget->setItem(i,3,newItem4);
		QTableWidgetItem *newItem5 = new QTableWidgetItem(joRow.value("sostoyanieKanala").toString());
		ui->proverkaTableWidget->setItem(i,4,newItem5);
		QTableWidgetItem *newItem6 = new QTableWidgetItem(joRow.value("nalichieTyagi").toString());
		ui->proverkaTableWidget->setItem(i,5,newItem6);
		QTableWidgetItem *newItem7 = new QTableWidgetItem(joRow.value("sostojanieOgolovka").toString());
		ui->proverkaTableWidget->setItem(i,6,newItem7);
	}
}
//-------------------------------------------------------------------------------------
void AgreementDialog::getVdpo()
{
	QJsonObject joVdpo=m_joExtra["VDPO"].toObject();
	joVdpo["object"]=ui->objectLineEdit->text();
	joVdpo["proverkaTehSostojania"]=ui->proverkaTehSostojaniaComboBox->currentText();
	joVdpo["oborudovanie"]=ui->oborudovanieLineEdit->text();
	joVdpo["celProverki"]=ui->celProverkiComboBox->currentText();
	joVdpo["vytyajnayaVentiljacia"]=ui->vytyajnayaVentiljaciaComboBox->currentText();
	joVdpo["protivopojarnayaRazdelka"]=ui->protivopojarnayaRazdelkaComboBox->currentText();
	joVdpo["karmanyDymovyhKanalov"]=ui->karmanyDymovyhKanalovComboBox->currentText();
	joVdpo["ispravnostTrub"]=ui->ispravnostTrubComboBox->currentText();
	joVdpo["dlinaTrub"]=ui->dlinaTrubComboBox->currentText();
	joVdpo["ogolovkiTrub"]=ui->ogolovkiTrubComboBox->currentText();
	joVdpo["nalichieProekta"]=ui->nalichieProektaComboBox->currentText();
	joVdpo["toplivo"]=ui->toplivoComboBox->currentText();
	joVdpo["vidObjecta"]=ui->vidObjectaComboBox->currentText();
	joVdpo["srokDeistviaAkta"]=ui->srokDeistviaAktaComboBox->currentText();
	m_joExtra["VDPO"]=joVdpo;
}
//-------------------------------------------------------------------------------------
