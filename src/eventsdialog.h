#ifndef EVENTSDIALOG_H
#define EVENTSDIALOG_H

#include <QDialog>
#include "common_inc.h"

namespace Ui {
class EventsDialog;
}

class EventsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EventsDialog(QWidget *parent = 0);
    ~EventsDialog();
    void refreshEvents();

private:
    Ui::EventsDialog *ui;
    QSqlQueryModel *eventsQueryModel;
};

#endif // EVENTSDIALOG_H
