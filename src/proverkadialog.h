#ifndef PROVERKADIALOG_H
#define PROVERKADIALOG_H

#include <QDialog>
#include <QJsonObject>

namespace Ui {
class ProverkaDialog;
}

class ProverkaDialog : public QDialog
{
	Q_OBJECT
	QJsonObject *m_joRow;
public:
	explicit ProverkaDialog(QJsonObject &joRow, QWidget *parent = nullptr);
	~ProverkaDialog();

private slots:
	void on_savePushButton_clicked();

private:
	Ui::ProverkaDialog *ui;
public:
/*	QString tipKanala();
	QString materialKanala();
	QString diametrKanala();
	QString gazoispOborudovanie();
	QString sostoyanieKanala();
	QString nalichieTyagi();
	QString sostojanieOgolovka();
*/
};

#endif // PROVERKADIALOG_H
