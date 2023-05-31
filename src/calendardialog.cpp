#include "calendardialog.h"
#include "ui_calendardialog.h"

//-------------------------------------------------------------------------------------
CalendarDialog::CalendarDialog(QDateTime dateTime,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CalendarDialog)
{
    ui->setupUi(this);
    ui->calendarWidget->setMinimumDate(QDate::currentDate());
    ui->calendarWidget->setSelectedDate(dateTime.date());
    ui->timeEdit->setTime(dateTime.time());
}
//-------------------------------------------------------------------------------------
CalendarDialog::~CalendarDialog()
{
    delete ui;
}
//-------------------------------------------------------------------------------------
void CalendarDialog::on_okPushButton_clicked()
{
    accept();
}
//-------------------------------------------------------------------------------------
QDateTime CalendarDialog::dateTime()
{
    return QDateTime(ui->calendarWidget->selectedDate(),ui->timeEdit->time());
}
//-------------------------------------------------------------------------------------
