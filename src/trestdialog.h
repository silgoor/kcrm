#ifndef TRESTDIALOG_H
#define TRESTDIALOG_H

#include <QDialog>

namespace Ui {
class TrestDialog;
}

class TrestDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TrestDialog(quint64 trestId,QWidget *parent = nullptr);
    ~TrestDialog();

private slots:
    void on_savePushButton_clicked();

private:
    Ui::TrestDialog *ui;
    quint64 m_trestId;
};

#endif // TRESTDIALOG_H
