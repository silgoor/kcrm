#include "sendsmsdialog.h"
#include "ui_sendsmsdialog.h"
#include "xmlsettings.h"
#include "common_inc.h"
#include <QMessageBox>

extern XMLSettings settings;
//-------------------------------------------------------------------------------------
SendSmsDialog::SendSmsDialog(QString phone, QString text, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SendSmsDialog),
    m_phone(phone),
    m_text(text)
{
    ui->setupUi(this);
    m_smsType=settings.value("sms","","type","").toInt();
    if(m_smsType==2)
    {
        m_smsHost="auth.terasms.ru";
        connect(&m_sslSocket, SIGNAL(encrypted()), this, SLOT(On_sslSocket_encrypted()));
        connect(&m_sslSocket, SIGNAL(sslErrors(QList<QSslError>)),this, SLOT(On_sslSocket_sslErrors(QList<QSslError>)));
        connect(&m_sslSocket, SIGNAL(readyRead()), this, SLOT(On_sslSocket_readyRead()));
		connect(&m_sslSocket, SIGNAL(disconnected()), this, SLOT(On_sslSocket_disconnected()));
        connect(&m_sslSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(On_sslSocket_error(QAbstractSocket::SocketError)));

        m_smsSender=getOption(0,"SMS_GATE_SENDER").toUtf8();
        m_smslogin=getOption(0,"SMS_GATE_LOGIN").toUtf8();
        m_smspassword=getOption(0,"SMS_GATE_PASSWORD").toUtf8();
        m_sslSocket.connectToHostEncrypted(m_smsHost,443);
        ui->statusLabel->setText(tr("Соединение"));

        exec();
    }
}
//-------------------------------------------------------------------------------------
SendSmsDialog::~SendSmsDialog()
{
    delete ui;
}
//-------------------------------------------------------------------------------------
void SendSmsDialog::On_sslSocket_encrypted()
{
    QByteArray smspage="/outbox/send/?login="+m_smslogin+"&password="+m_smspassword+"&target="+m_phone.toUtf8()+"&sender="+m_smsSender+"&message="+m_text.toUtf8();
    smspage= QUrl::toPercentEncoding(smspage,"/?&=");
//    QMessageBox::information(this,PROGNAME,smspage);
    m_sslSocket.write("GET "+smspage+" HTTP/1.1\r\n");
//а также имя хоста
    m_sslSocket.write("Host: "+m_smsHost.toUtf8()+"\r\n");
//типы принимаемых данных. */* означает, что принимаем все типы данных
    m_sslSocket.write("Accept:*/*\r\n");
//представимся оперой
    m_sslSocket.write("User-agent:Opera 10.00\r\n");
    m_sslSocket.write("Connection:Close\r\n");
    m_sslSocket.write("\r\n");
    ui->statusLabel->setText(tr("Ожидание ответа"));
}
//-------------------------------------------------------------------------------------
void SendSmsDialog::On_sslSocket_sslErrors(QList<QSslError> sslErrors)
{
    QString errors;
    for(int i=0;i<sslErrors.size();i++)
        errors+=sslErrors.at(i).errorString()+"\r\n";
    ui->statusLabel->setText(tr("Ошибки ssl: ")+errors);
    if (QMessageBox::question(this, PROGNAME, tr("Обнаружены ошибки ssl:\r\n%1Продолжить?").arg(errors),QMessageBox::Yes|QMessageBox::No,QMessageBox::No)==QMessageBox::Yes)
    {
        m_sslSocket.ignoreSslErrors();
    }
    else
        reject();
}
//-------------------------------------------------------------------------------------
void SendSmsDialog::On_sslSocket_readyRead()
{
    QByteArray reply=m_sslSocket.readAll();
    if(!reply.contains("200 OK"))
        ui->statusLabel->setText(tr("Ошибка отправки SMS: ")+reply);
    else
    {
        ui->statusLabel->setText(reply);
        int replyCodeIndex=reply.indexOf("\r\n\r\n");
        if(replyCodeIndex>0)
            m_replyCode=reply.mid(replyCodeIndex+4);
//        QMessageBox::information(this,PROGNAME,tr("code size:%1 code:%2").arg(m_replyCode.size()).arg(m_replyCode));
        if(m_replyCode.size()>0)
        {
            if(m_replyCode.at(0)!='-')
                accept();
        }
    }
}
//-------------------------------------------------------------------------------------
void SendSmsDialog::On_sslSocket_disconnected()
{
//	ui->statusLabel->setText("Разъединён");
//	accept();
}
//-------------------------------------------------------------------------------------
void SendSmsDialog::On_sslSocket_error(QAbstractSocket::SocketError socketError)
{
    ui->statusLabel->setText(QString("Ошибка соединения %1: ").arg(socketError)+m_sslSocket.errorString());
}
//-------------------------------------------------------------------------------------
