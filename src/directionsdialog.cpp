#include "directionsdialog.h"
#include "ui_directionsdialog.h"
#include "common_inc.h"
#include "directiondialog.h"

//-------------------------------------------------------------------------------------
DirectionsDialog::DirectionsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DirectionsDialog)
{
	ui->setupUi(this);
	m_directionsQueryModel = new QSqlQueryModel;
	ui->directionsTableView->setModel(m_directionsQueryModel);
//    ui->directionsTableView->horizontalHeader()->setSortIndicator(2,Qt::AscendingOrder); // Сортировка по умолчанию по имени по возрастающей.
	refreshDirections();
}
//-------------------------------------------------------------------------------------
DirectionsDialog::~DirectionsDialog()
{
	delete ui;
}
//-------------------------------------------------------------------------------------
void DirectionsDialog::refreshDirections()
{
	QueryModelExec(this,"SELECT id, CASE enabled WHEN 0 THEN '-' ELSE '"+tr("Да")+"' END AS directionenabled, name FROM directions ORDER BY name;",m_directionsQueryModel);
	ui->directionsTableView->resizeColumnsToContents();
	int column_cnt=0;
	m_directionsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Ид"));
	m_directionsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Активно"));
	m_directionsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Название"));
}
//-------------------------------------------------------------------------------------
void DirectionsDialog::on_newToolButton_clicked()
{
	DirectionDialog *directionDialog = new DirectionDialog(0,this);
	if(directionDialog->exec()==QDialog::Accepted)
	{
		refreshDirections();
	}
	delete directionDialog;
}
//-------------------------------------------------------------------------------------
void DirectionsDialog::on_editToolButton_clicked()
{
	int id=m_directionsQueryModel->record(ui->directionsTableView->currentIndex().row()).field("id").value().toInt();
	if(!id)
	{
		qDebug() << "No id.";
		return;
	}
	DirectionDialog *directionDialog = new DirectionDialog(id,this);
	if(directionDialog->exec()==QDialog::Accepted)
	{
		refreshDirections();
	}
	delete directionDialog;
}
//-------------------------------------------------------------------------------------
void DirectionsDialog::on_directionsTableView_activated(const QModelIndex &index)
{
	Q_UNUSED(index)
	on_editToolButton_clicked();
}

