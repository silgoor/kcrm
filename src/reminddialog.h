#ifndef REMINDDIALOG_H
#define REMINDDIALOG_H

#include <QDialog>

namespace Ui {
class RemindDialog;
}

class RemindDialog : public QDialog
{
    Q_OBJECT

public:
	explicit RemindDialog(QWidget *parent = 0);
    ~RemindDialog();
    void loadRequestInfo(quint64 requestId);

private slots:
    void on_okPushButton_clicked();

private:
    Ui::RemindDialog *ui;
};

#endif // REMINDDIALOG_H
