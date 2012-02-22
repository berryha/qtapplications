#ifndef TCPPWidget_H
#define TCPPWidget_H

#include <QtGui/QWidget>

#include "ui_tcpwidget.h"


#include "tcp.h"


namespace HEHUI {

class TCPWidget : public QWidget
{
    Q_OBJECT

public:
    TCPWidget(QWidget *parent = 0);
    ~TCPWidget();


private slots:

    void on_toolButtonListen_clicked(){
        listen();
    }
    void on_toolButtonConnect_clicked(){
        connectToPeer();
    }
    void on_toolButtonSend_clicked(){
        send();
    }
    void on_toolButtonClean_clicked(){
        clean();
    }

    void listen();
    void connectToPeer();
    bool startServer(quint16 port);
    void send();
    void clean();




    void connected(int socketID, const QHostAddress &peerAddress, quint16 peerPort);
    void signalConnectToPeerTimeout(const QHostAddress &peerAddress, quint16 peerPort);
    void disconnected(int socketID, const QHostAddress &peerAddress, quint16 peerPort);

    void dataReceived(const QString &peerAddress, quint16 peerPort, const QByteArray &data);



private:
    Ui::TCPClass ui;




    TCP *tcpProtocol;

    int peerSockeet;

    bool isListening;

    bool isConnected;

    QHostAddress m_peerAddress;
    quint16 m_peerPort;

    quint16 localPort;

    quint16 m_receivedDataCount;







};
} //namespace HEHUI

#endif // TCPPWidget_H
