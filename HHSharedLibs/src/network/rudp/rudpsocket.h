#ifndef RUDPSOCKET_H
#define RUDPSOCKET_H

#include <QUdpSocket>

//#include "../mysharedlib_global.h"

#include "rudpchannel.h"


namespace HEHUI{

class MYSHAREDLIB_API RUDPSocket : public QUdpSocket
{
    Q_OBJECT
public:
    explicit RUDPSocket(QObject *parent = 0);

    ~RUDPSocket();


    void connectToPeer(const QString &peerAddressString, quint16 peerPort, int msecTimeout = RUDP_CONNECTION_TIMEOUT);
    void connectToPeer(const QHostAddress &peerAddress, quint16 peerPort, int msecTimeout = RUDP_CONNECTION_TIMEOUT);
    void disconnectFromPeer(const QHostAddress &peerAddress, quint16 peerPort);

//    bool canSendData(quint64 size);

signals:
    void peerConnected(const QHostAddress &peerAddress, quint16 peerPort);
    void signalConnectToPeerTimeout(const QHostAddress &peerAddress, quint16 peerPort);
    void peerDisconnected(const QHostAddress &peerAddress, quint16 peerPort);

    void dataReceived(const QHostAddress &peerAddress, quint16 peerPort, const QByteArray &data);


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

    QHash<QString/*IP:Port*/, RUDPChannel*> peers;

    QByteArray *datagram;

    QList<RUDPChannel*> m_unusedRUDPChannels;




};

} //namespace HEHUI

#endif // RUDPSOCKET_H
