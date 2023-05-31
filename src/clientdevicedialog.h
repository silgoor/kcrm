#ifndef CLIENTDEVICEDIALOG_H
#define CLIENTDEVICEDIALOG_H

#include <QDialog>

namespace Ui {
class ClientDeviceDialog;
}

class ClientDeviceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ClientDeviceDialog(int clientId, int clientDeviceId, QWidget *parent = 0);
    ~ClientDeviceDialog();
    void refreshDeviceComboBox(int defaultId);

private slots:
    void on_okPushButton_clicked();
    void on_deviceComboBox_activated(int index);

private:
    Ui::ClientDeviceDialog *ui;
    int m_clientId,m_clientDeviceId;
};

#endif // CLIENTDEVICEDIALOG_H
