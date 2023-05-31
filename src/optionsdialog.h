#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>
#include <QSqlQueryModel>

namespace Ui {
class OptionsDialog;
}

class OptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptionsDialog(QWidget *parent = 0);
    ~OptionsDialog();
    void refreshOptions();

private slots:
    void on_closePushButton_clicked();

    void on_optionsTableView_activated(const QModelIndex &index);

private:
    Ui::OptionsDialog *ui;
    QSqlQueryModel *optionsQueryModel;
};

#endif // OPTIONSDIALOG_H
