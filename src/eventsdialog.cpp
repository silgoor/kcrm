#include "eventsdialog.h"
#include "ui_eventsdialog.h"

//-------------------------------------------------------------------------------------
EventsDialog::EventsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EventsDialog)
{
    ui->setupUi(this);
    eventsQueryModel = new QSqlQueryModel;
    ui->eventsTableView->setModel(eventsQueryModel);
    refreshEvents();
}
//-------------------------------------------------------------------------------------
EventsDialog::~EventsDialog()
{
    delete ui;
}
//-------------------------------------------------------------------------------------
void EventsDialog::refreshEvents()
{
    QModelIndex savedIndex=ui->eventsTableView->currentIndex();
    QueryModelExec(this,"SELECT events.id, actions.name, events.rowid, users.name, events.ctime FROM events LEFT JOIN actions ON actions.id=events.actionid LEFT JOIN users ON users.id=events.userid ORDER BY events.id DESC;",eventsQueryModel);
    int column_cnt=0;
    eventsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Ид"));
    eventsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Действие"));
    eventsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Ид записи"));
    eventsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Пользователь"));
    eventsQueryModel->setHeaderData(column_cnt++, Qt::Horizontal, tr("Дата"));
    ui->eventsTableView->resizeColumnsToContents();
    ui->eventsTableView->setCurrentIndex(savedIndex);
}
//-------------------------------------------------------------------------------------
