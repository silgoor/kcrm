#ifndef DEVICEDIALOG_H
#define DEVICEDIALOG_H

#include <QDialog>

namespace Ui {
class DeviceDialog;
}

class DeviceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DeviceDialog(int deviceId, QWidget *parent = 0);
    ~DeviceDialog();

private slots:
    void on_okPushButton_clicked();
    void refreshDeviceTypeComboBox(int defaultId);

private:
    Ui::DeviceDialog *ui;
    int m_deviceId;
};

#endif // DEVICEDIALOG_H
