#include "proverkadialog.h"
#include "ui_proverkadialog.h"

ProverkaDialog::ProverkaDialog(QJsonObject &joRow, QWidget *parent) :
	QDialog(parent),
	m_joRow(&joRow),
	ui(new Ui::ProverkaDialog)
{
	ui->setupUi(this);
// Init controls.
//	ui->tipKanalaComboBox->addItem(tr("Вентиляционный"),"ventilacionny");
//	ui->tipKanalaComboBox->addItem(tr("Ды"),"ventilacionny");
// Set values.
	ui->tipKanalaComboBox->setCurrentText(m_joRow->value("tipKanala").toString());
	ui->materialKanalaComboBox->setCurrentText(m_joRow->value("materialKanala").toString());
	ui->diametrKanalaComboBox->setCurrentText(m_joRow->value("diametrKanala").toString());
	ui->gazoispOborudovanieComboBox->setCurrentText(m_joRow->value("gazoispOborudovanie").toString());
	ui->sostoyanieKanalaComboBox->setCurrentText(m_joRow->value("sostoyanieKanala").toString());
	ui->nalichieTyagiComboBox->setCurrentText(m_joRow->value("nalichieTyagi").toString());
	ui->sostojanieOgolovkaComboBox->setCurrentText(m_joRow->value("sostojanieOgolovka").toString());
}
//-------------------------------------------------------------------------------------
ProverkaDialog::~ProverkaDialog()
{
	delete ui;
}
//-------------------------------------------------------------------------------------
void ProverkaDialog::on_savePushButton_clicked()
{
	m_joRow->insert("tipKanala",ui->tipKanalaComboBox->currentText());
	m_joRow->insert("materialKanala",ui->materialKanalaComboBox->currentText());
	m_joRow->insert("diametrKanala",ui->diametrKanalaComboBox->currentText());
	m_joRow->insert("gazoispOborudovanie",ui->gazoispOborudovanieComboBox->currentText());
	m_joRow->insert("sostoyanieKanala",ui->sostoyanieKanalaComboBox->currentText());
	m_joRow->insert("nalichieTyagi",ui->nalichieTyagiComboBox->currentText());
	m_joRow->insert("sostojanieOgolovka",ui->sostojanieOgolovkaComboBox->currentText());
	accept();
}
//-------------------------------------------------------------------------------------
/*QString ProverkaDialog::tipKanala()
{
	return ui->tipKanalaComboBox->currentText();
}
//-------------------------------------------------------------------------------------
QString ProverkaDialog::materialKanala()
{
	return ui->materialKanalaComboBox->currentText();
}
//-------------------------------------------------------------------------------------
QString ProverkaDialog::diametrKanala()
{
	return ui->diametrKanalaComboBox->currentText();
}
//-------------------------------------------------------------------------------------
QString ProverkaDialog::gazoispOborudovanie()
{
	return ui->gazoispOborudovanieComboBox->currentText();
}
//-------------------------------------------------------------------------------------
QString ProverkaDialog::sostoyanieKanala()
{
	return ui->sostoyanieKanalaComboBox->currentText();
}
//-------------------------------------------------------------------------------------
QString ProverkaDialog::nalichieTyagi()
{
	return ui->nalichieTyagiComboBox->currentText();
}
//-------------------------------------------------------------------------------------
QString ProverkaDialog::sostojanieOgolovka()
{
	return ui->sostojanieOgolovkaComboBox->currentText();
}
//-------------------------------------------------------------------------------------
*/
