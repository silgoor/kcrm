#ifndef PERFORMERDIALOG_H
#define PERFORMERDIALOG_H

#include <QDialog>

namespace Ui {
class PerformerDialog;
}

class PerformerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PerformerDialog(int performerid, QWidget *parent = 0);
    ~PerformerDialog();
    void refreshOffices();
    void fillPerformerOffices();

private slots:
    void on_okPushButton_clicked();

    void on_folderPushButton_clicked();

private:
    Ui::PerformerDialog *ui;
    int m_performerid;
};

#endif // PERFORMERDIALOG_H
