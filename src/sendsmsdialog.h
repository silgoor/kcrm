#ifndef SENDSMSDIALOG_H
#define SENDSMSDIALOG_H

#include <QDialog>
#include <QSslSocket>

namespace Ui {
class SendSmsDialog;
}

class SendSmsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SendSmsDialog(QString phone, QString text, QWidget *parent = 0);
    ~SendSmsDialog();
    QSslSocket m_sslSocket;

private:
    Ui::SendSmsDialog *ui;
    QString m_phone;
    QString m_text;
    QByteArray m_smsSender;
    QByteArray m_smslogin;
    QByteArray m_smspassword;
    QString m_smsHost;
    int m_smsType;
    QString m_replyCode;
private slots:
    void On_sslSocket_encrypted();
    void On_sslSocket_sslErrors(QList<QSslError> sslErrors);
    void On_sslSocket_readyRead();
	void On_sslSocket_disconnected();
    void On_sslSocket_error(QAbstractSocket::SocketError socketError);
};

#endif // SENDSMSDIALOG_H
