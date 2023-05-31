#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include <QTreeWidgetItem>
#include "colorsqlquerymodel.h"
#include "agreementscolorsqlquerymodel.h"
#include "clientscolorsqlquerymodel.h"


#define TAB_REQUESTS 0
#define TAB_AGREEMENTS 1
#define TAB_CLIENTS 2
#define TAB_DOCUMENTS 3

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    bool openDatabaseConnection();
    void refreshOffices();
    void refreshPerformers();
    void refreshCurrentTab();
    void refreshRequests();
    void refreshAgreements();
    void refreshClients();
    void refreshDates();
    void refreshTypeComboBox();
    void refreshTrestComboBox();
    void refreshDocumentsButtons();
protected:
    void closeEvent(QCloseEvent *);
    void contextMenuEvent(QContextMenuEvent *event);
private slots:
    void On_connectionTimer_timeout();
    void On_remindTimer_timeout();
    void on_newRequestPushButton_clicked();
    void on_officeComboBox_currentIndexChanged(int index);
    void on_requestsTableView_activated(const QModelIndex &index);
    void On_requestsTableView_selectionChanged(const QItemSelection & selected, const QItemSelection & deselected);
    void On_requestsHeaderView_sectionClicked();
    void On_agreementsHeaderView_sectionClicked();
    void On_clientsHeaderView_sectionClicked();
    void on_datesTreeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void on_refreshAction_triggered();
//    void on_performersAction_triggered();
    void on_optionsAction_triggered();
    void on_officesAction_triggered();
    void on_copySelectedAction_triggered();
//    void on_printRequestPushButton_clicked();
    void on_performerComboBox_currentIndexChanged(int index);
    void on_copyRequestPushButton_clicked();
    void on_columnCreateDateAction_triggered();
    void on_columnPerformDateAction_triggered();
	void on_columnDirectionAction_triggered();
	void on_columnAddressAction_triggered();
    void on_columnSurnameAction_triggered();
    void on_columnNameAction_triggered();
    void on_columnPhone1Action_triggered();
    void on_columnPhone2Action_triggered();
    void on_columnDeviceNameAction_triggered();
    void on_columnDefectAction_triggered();
    void on_columnCostAction_triggered();
    void on_columnPerformerAction_triggered();
    void on_columnCreatorAction_triggered();
    void on_columnStatusAction_triggered();
    void on_columnCollectionAction_triggered();
    void on_eventsAction_triggered();
    void on_timesheetAction_triggered();
    void on_todayPushButton_clicked();
    void on_tabWidget_currentChanged(int index);
//    void on_datesAction_triggered();
    void on_backupAction_triggered();
    void on_agreementsTableView_activated(const QModelIndex &index);
    void on_newAgreementPushButton_clicked();
    void on_performerBillsAction_triggered();
    void on_showTerminatedAgreementsCheckBox_stateChanged(int arg1);
    void on_agrFilterTrestComboBox_activated(int index);
    void on_agrFilterTypeComboBox_activated(int index);
    void On_printDocumentPushButton_clicked();
    void on_filterLineEdit_textChanged(const QString &arg1);
    void on_resetAmountsAction_triggered();
    void on_usersAction_triggered();

    void on_trestsAction_triggered();

    void on_clientsTableView_activated(const QModelIndex &index);

	void on_directionsAction_triggered();

private:
    Ui::MainWindow *ui;
    ColorSqlQueryModel *requestsQueryModel;
    AgreementsColorSqlQueryModel *agreementsQueryModel;
    ClientsColorSqlQueryModel *clientsQueryModel;
    QTimer m_connectionTimer;
    QTimer m_remindTimer;
    QStringList m_monthList;
    QTreeWidgetItem *m_allTreeWidgetItem;
    QDate m_lastPerformDate;
    bool m_adminFlag;
    QList<quint64> remindRequestIdList;
};

#endif // MAINWINDOW_H
