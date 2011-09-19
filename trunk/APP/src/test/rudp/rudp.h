#ifndef RUDPWidget_H
#define RUDPWidget_H

#include <QtGui/QWidget>

#include "ui_rudp.h"

#include "HHSharedNetwork/hrudpsocket.h"


namespace HEHUI {

class RUDPWidget : public QWidget
{
    Q_OBJECT

public:
    RUDPWidget(QWidget *parent = 0);
    ~RUDPWidget();


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
    void send();
    void clean();


    void connected(const QHostAddress &peerAddress, quint16 peerPort);
    void signalConnectToPeerTimeout(const QHostAddress &peerAddress, quint16 peerPort);
    void disconnected(const QHostAddress &peerAddress, quint16 peerPort);

    void dataReceived(const QHostAddress &peerAddress, quint16 peerPort, const QByteArray &data);



private:
    Ui::RUDPClass ui;



    PacketHandlerBase *m_packetHandlerBase;
    RUDPSocket *rudpSocket;


    bool isListening;

    bool isConnected;

    QHostAddress m_peerAddress;
    quint16 m_peerPort;

    quint16 m_receivedDataCount;







};
} //namespace HEHUI

#endif // RUDPWidget_H
