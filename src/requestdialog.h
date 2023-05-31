#ifndef REQUESTDIALOG_H
#define REQUESTDIALOG_H

#include <QDialog>
#include <QCompleter>
#include <QStringListModel>
#include <QtWidgets>

class RFlags
{
    int m_bit;
    int m_type;
    QCheckBox *m_checkBox;
    QRadioButton *m_radioButton;
public:
    RFlags(int bit, QCheckBox *checkBox){m_bit=bit;m_type=0;m_checkBox=checkBox;m_radioButton=0;}
    RFlags(int bit, QRadioButton *radioButton){m_bit=bit;m_type=1;m_checkBox=0;m_radioButton=radioButton;}
    int bit() const {return m_bit;}
    bool isChecked() const {if(m_type==1)return m_radioButton->isChecked();else return m_checkBox->isChecked();}
};

namespace Ui {
class RequestDialog;
}

class RequestDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RequestDialog(int requestid, int officeid, int agreementid, bool copyFlag, QWidget *parent = 0);
    ~RequestDialog();
//    QDate performDate();
private:
    void refreshOffices();
//    void refreshDeviceTypes();
    void refreshFlags(quint64 flags);
//    void fillDeviceTypes();
    void refreshPerformers();
    void refreshStates();
    bool refreshClient();
    bool saveRequest();
    void refreshDeviceCompleter();
    void refreshBonusUserComboBox(int defaultId);
    bool updatePerformerRatingAvg(int performerId);
//    void refreshDeviceComboBox();
//    void refreshRequestDevices();
    void closeEvent(QCloseEvent *);

private slots:
    void on_okPushButton_clicked();

    void on_clientPushButton_clicked();

    void on_officeComboBox_currentIndexChanged(int index);

    void on_calendarPushButton_clicked();

    void on_printPushButton_clicked();

    void on_costDoubleSpinBox_valueChanged(double arg1);

    void On_agreementPushButton_clicked();
//    void on_addDeviceComboBox_activated(int index);
//    void On_deviceCompleter_Activated(QModelIndex index);

private:
    Ui::RequestDialog *ui;
    QCompleter *deviceCompleter;
    QStringListModel *deviceCompleterModel;
    QList<RFlags> rFlagsList;
    int m_requestid;
    int m_officeid;
    int m_agreementid;
    int m_clientid;
    int m_performerid;
    int m_percent;
};

#endif // REQUESTDIALOG_H
