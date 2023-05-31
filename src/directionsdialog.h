#ifndef DIRECTIONSDIALOG_H
#define DIRECTIONSDIALOG_H

#include <QDialog>
#include <QSqlQueryModel>

namespace Ui {
class DirectionsDialog;
}

class DirectionsDialog : public QDialog
{
	Q_OBJECT

public:
	explicit DirectionsDialog(QWidget *parent = nullptr);
	~DirectionsDialog();
	void refreshDirections();

private slots:
	void on_newToolButton_clicked();

	void on_editToolButton_clicked();

	void on_directionsTableView_activated(const QModelIndex &index);

private:
	Ui::DirectionsDialog *ui;
	QSqlQueryModel *m_directionsQueryModel;
};

#endif // DIRECTIONSDIALOG_H
