
/**********************************

//Server
RUDPSocket *serverRudpSocket = new RUDPSocket(this);
serverRudpSocket->bind(3000);


//Client
RUDPSocket *rudpSocket = new RUDPSocket(this);
QHostAddress m_peerAddress = QHostAddress("127.0.0.1");
quint16 m_peerPort = 3000;
rudpSocket->connectToPeer(m_peerAddress, m_peerPort);

//Send Method 1
QByteArray data;
data.resize(1024000);

quint16 fragmentDataID = rudpSocket->beginDataTransmission(m_peerAddress, m_peerPort);
int totalSent = 0;
int sent;
while (totalSent < size)
{
    qApp->processEvents();
    if (0 == (sent = rudpSocket->sendDatagram(m_peerAddress, m_peerPort, &data, totalSent, true)) )
    {
        //msleep(10);
    }

    totalSent += sent;
}
rudpSocket->endDataTransmission(m_peerAddress, m_peerPort, fragmentDataID);

**********************************/




#ifndef RUDPSOCKET_H
#define RUDPSOCKET_H

#include <QUdpSocket>

//#include "../mysharedlib_global.h"

#include "rudpchannel.h"
#include "../packethandler/packethandlerbase.h"


namespace HEHUI{

class MYSHAREDLIB_API RUDPSocket : public QUdpSocket
{
    Q_OBJECT
public:
    explicit RUDPSocket(PacketHandlerBase *packetHandlerBase, QObject *parent = 0);

    ~RUDPSocket();


    void connectToPeer(const QString &peerAddressString, quint16 peerPort, int msecTimeout = RUDP_CONNECTION_TIMEOUT);
    void connectToPeer(const QHostAddress &peerAddress, quint16 peerPort, int msecTimeout = RUDP_CONNECTION_TIMEOUT);
    void disconnectFromPeer(const QHostAddress &peerAddress, quint16 peerPort);

//    bool canSendData(quint64 size);

signals:
    void peerConnected(const QHostAddress &peerAddress, quint16 peerPort);
    void signalConnectToPeerTimeout(const QHostAddress &peerAddress, quint16 peerPort);
    void peerDisconnected(const QHostAddress &peerAddress, quint16 peerPort);

    //void dataReceived(const QHostAddress &peerAddress, quint16 peerPort, const QByteArray &data);


public slots:

    quint16 beginDataTransmission(const QHostAddress &peerAddress, quint16 peerPort);
    bool sendData(const QHostAddress &peerAddress, quint16 peerPort, QByteArray &data);
//    quint64 sendData(const QHostAddress &peerAddress, quint16 peerPort, QByteArray *data, quint64 offset);
    quint64 sendDatagram(const QHostAddress &peerAddress, quint16 peerPort, QByteArray *data, quint64 offset, bool fragment);
    quint64 sendDatagram(const QHostAddress &peerAddress, quint16 peerPort, QByteArray *data);

    void endDataTransmission(const QHostAddress &peerAddress, quint16 peerPort, quint16 fragmentDataID);


private slots:
    void readPendingDatagrams();

    void channelclosed();

private:
    RUDPChannel * getRUDPChannel(const QHostAddress &hostAddress = QHostAddress::Null, quint16 port = 0);

    void recyleRUDPChannel(RUDPChannel *channel);

private:
    PacketHandlerBase *m_packetHandlerBase;

    QHash<QString/*IP:Port*/, RUDPChannel*> peers;

    QByteArray *datagram;

    QList<RUDPChannel*> m_unusedRUDPChannels;




};

typedef RUDPSocket RUDPServer;

} //namespace HEHUI

#endif // RUDPSOCKET_H
