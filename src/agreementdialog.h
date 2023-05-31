#ifndef AGREEMENTDIALOG_H
#define AGREEMENTDIALOG_H

#include <QDialog>
#include <QtSql>
#include <QCheckBox>
#include "proverkadialog.h"

namespace Ui {
class AgreementDialog;
}

class AgreementDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AgreementDialog(int agreementid, QWidget *parent = 0);
    ~AgreementDialog();
    void hideNewRequestButton();
private slots:
    void on_okPushButton_clicked();
    void on_clientPushButton_clicked();
    void on_newRequestPushButton_clicked();
    
    void on_unlockPushButton_clicked();
    
    void on_requestsTableView_activated(const QModelIndex &index);
    void On_printDocumentPushButton_clicked();

    void on_addDevicePushButton_clicked();
    void on_removeDevicePushButton_clicked();
    void on_newMessagePushButton_clicked();
    void on_newMessageLineEdit_returnPressed();
	void on_newProverkaToolButton_clicked();
	void on_editProverkaToolButton_clicked();
	void on_removeProverkaToolButton_clicked();

	void on_proverkaTableWidget_activated(const QModelIndex &index);

private:
    void closeEvent(QCloseEvent *event);
    void refreshOfficeComboBox(int defaultId);
//    void refreshTypeComboBox(int defaultId);
    void refreshTrestComboBox(int defaultId);
    void refreshDocumentsButtons();
	void refreshAgreementTypes(int types);
	bool isTypeSelected(int types, int id);
	int selectedTypes();
//    void fillDeviceTypes();
//    void refreshDeviceTypesComboBox(int defaultId);
	void refreshDeviceTableWidget(QJsonArray deviceArray);//QByteArray jsonData);
    void refreshRequests();
    void refreshMessages();
	void refreshProverkaTableWidget();
	void getVdpo();
private:
    Ui::AgreementDialog *ui;
    int m_agreementid;
    int m_clientid;
	QJsonObject m_joExtra;
    QSqlQueryModel *requestsQueryModel;
    QSqlQueryModel *m_messagesQueryModel;
	QList <QCheckBox*> m_typeCheckBoxList;
};

#endif // AGREEMENTDIALOG_H
