#include "trestdialog.h"
#include "ui_trestdialog.h"
#include "common_inc.h"

//-------------------------------------------------------------------------------------
TrestDialog::TrestDialog(quint64 trestId, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TrestDialog),
    m_trestId(trestId)
{
    ui->setupUi(this);
    if(m_trestId)
    {
        QSqlQuery query;
        QueryExec(this,"SELECT enabled, name FROM trests WHERE id='"+QString::number(m_trestId)+"';",query);
        if(!query.next())
        {
            QMetaObject::invokeMethod(this, "reject", Qt::QueuedConnection); // Close dialog.
            return;
        }
        if(query.value(0).toInt()!=0)
            ui->enabledCheckBox->setChecked(true);
        ui->nameLineEdit->setText(query.value(1).toString());
        setWindowTitle(tr("Редактирование треста %1").arg(m_trestId));
    }
}
//-------------------------------------------------------------------------------------
TrestDialog::~TrestDialog()
{
    delete ui;
}
//-------------------------------------------------------------------------------------
void TrestDialog::on_savePushButton_clicked()
{
    QString enabled="0";
    if(ui->enabledCheckBox->isChecked())
        enabled="1";
    QString name=escapeQuotes(ui->nameLineEdit->text().trimmed());
    if(m_trestId)
    {
        if(!QueryExec(this,"UPDATE trests SET enabled='"+enabled+"', name='"+name+"' WHERE id='"+num(m_trestId)+"';"))
            return;
    }
    else
    {
        QSqlQuery insertSqlQuery;
        if(!QueryExec(this,"INSERT INTO trests(enabled, name) VALUES('"+enabled+"', '"+name+"');",insertSqlQuery))
            return;
        m_trestId=insertSqlQuery.lastInsertId().toULongLong();
    }
    accept();

}
//-------------------------------------------------------------------------------------
