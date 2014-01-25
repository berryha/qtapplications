#ifndef UDTPWidget_H
#define UDTPWidget_H

#include <QWidget>

#include "ui_udt.h"


#include "udtsocket.h"


namespace HEHUI {

class UDTWidget : public QWidget
{
    Q_OBJECT

public:
    UDTWidget(QWidget *parent = 0);
    ~UDTWidget();


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
    bool startRUDPServer(quint16 port);
    void send();
    void clean();




    void connected(int socket, const QString &peerAddress, quint16 peerPort);
    void signalConnectToPeerTimeout(const QHostAddress &peerAddress, quint16 peerPort);
    void disconnected(const QHostAddress &peerAddress, quint16 peerPort);
    void disconnected(int socket);

    void dataReceived(const QString &peerAddress, quint16 peerPort, const QByteArray &data);



private:
    Ui::UDTClass ui;




    UDTProtocolTest *udtProtocol;

    UDTSOCKET serverSocket, peerSockeet;

    bool isListening;

    bool isConnected;

    QHostAddress m_peerAddress;
    quint16 m_peerPort;

    quint16 localPort;

    quint16 m_receivedDataCount;

    QDateTime startTime, endTime;
    quint64 totalDataSize;





};
} //namespace HEHUI

#endif // UDTWidget_H
