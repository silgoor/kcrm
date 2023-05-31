#ifndef CALENDARDIALOG_H
#define CALENDARDIALOG_H

#include <QDialog>

namespace Ui {
class CalendarDialog;
}

class CalendarDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CalendarDialog(QDateTime dateTimeValue, QWidget *parent = 0);
    ~CalendarDialog();
    QDateTime dateTime();

private slots:
    void on_okPushButton_clicked();

private:
    Ui::CalendarDialog *ui;
};

#endif // CALENDARDIALOG_H
