#ifndef DIRECTIONDIALOG_H
#define DIRECTIONDIALOG_H

#include <QDialog>
#include "common_inc.h"

namespace Ui {
class DirectionDialog;
}

class DirectionDialog : public QDialog
{
	Q_OBJECT

public:
	explicit DirectionDialog(int directionId, QWidget *parent = nullptr);
	~DirectionDialog();

private slots:
	void on_savePushButton_clicked();

private:
	Ui::DirectionDialog *ui;
	int m_directionId;
};

#endif // DIRECTIONDIALOG_H
