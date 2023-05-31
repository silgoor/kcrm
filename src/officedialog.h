#ifndef OFFICEDIALOG_H
#define OFFICEDIALOG_H

#include <QDialog>

namespace Ui {
class OfficeDialog;
}

class OfficeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OfficeDialog(int officeid, QWidget *parent = 0);
    ~OfficeDialog();

private slots:
    void on_okPushButton_clicked();

private:
    Ui::OfficeDialog *ui;
    int m_officeid;
};

#endif // OFFICEDIALOG_H
