#ifndef SELECTCONNECTIONDIALOG_H
#define SELECTCONNECTIONDIALOG_H

#include <QDialog>
#include "xmlsettings.h"

namespace Ui {
    class SelectConnectionDialog;
}

class SelectConnectionDialog : public QDialog
{
    Q_OBJECT
	int m_selectedIndex;
    int m_uid;
    unsigned int m_rights;
    QStringList m_connectionNameList;
public:
    explicit SelectConnectionDialog(unsigned int rights,QWidget *parent = 0);
    ~SelectConnectionDialog();
	int selectedIndex(){return m_selectedIndex;}
private:
    Ui::SelectConnectionDialog *ui;
private slots:
	void on_okPushButton_clicked();
};

#endif // SELECTCONNECTIONDIALOG_H
