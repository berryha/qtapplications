/*
 ****************************************************************************
 * networkmanagerbase.h
 *
 * Created on: 2009-9-5
 *     Author: 贺辉
 *    License: LGPL
 *    Comment:
 *
 *
 *    =============================  Usage  =============================
 *|
 *|
 *    ===================================================================
 *
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 ****************************************************************************
 */

/*
 ***************************************************************************
 * Last Modified on: 2011-07-23
 * Last Modified by: 贺辉
 ***************************************************************************
 */




#ifndef NETWORKMANAGERBASE_H_
#define NETWORKMANAGERBASE_H_

#include <QObject>
#include <QHostAddress>
#include <QMutex>

#include "global_network.h"
#include "./udp/udpsocket.h"
#include "./tcp/tcpsocketconnection.h"
#include "./tcp/tcpserver.h"
//#include "udp/multicast/multicast.h"

#include "mysharedlib_global.h"

namespace HEHUI {


class MYSHAREDLIB_API NetworkManagerBase : public QObject {
    Q_OBJECT

public:
    enum NetworkType {LAN = 0, INTERNET = 1, NWTYPE_MIXED};
    enum CommunicationMode {P2P = 0, CS = 1};

    NetworkManagerBase(PacketHandlerBase *packetHandlerBase = 0, NetworkType type = LAN, CommunicationMode mode = P2P, QObject *parent = 0);
    virtual ~NetworkManagerBase();

    void closeUDPServer(quint16 port);
    void closeTCPServer(quint16 port);
    void closeAllServers();

    void setPacketHandler(PacketHandlerBase *packetHandlerBase);
    PacketHandlerBase * getPacketHandler();

    void setNetworkType(NetworkType type);
    NetworkType getNetworkType() const;

    void setCommunicationMode(CommunicationMode mode);
    CommunicationMode getCommunicationMode() const;

    UDPServer *getUDPServer(quint16 port, const QHostAddress &localAddress);
    UDPServer * startUDPServerListening(const QHostAddress &localAddress, quint16 localPort);
    bool startIPMulticastServerListening(const QHostAddress &ipMulticastGroupAddress, quint16 ipMulticastGroupPort);

    TcpServer *getTcpServer(quint16 port, const QHostAddress &serverIPAddress);
    bool startTCPServerListening(const QHostAddress &localAddress = QHostAddress::Any, quint16 port = 0);
    bool hasConnection(const QHostAddress &senderIp, quint16 senderPort = 0);
    void appendConnection(TcpSocketConnection *connection);
    void removeConnection(TcpSocketConnection *connection);
    TcpSocketConnection* getConnection(const QString &ip, quint16 port);


signals:
    void signalTCPConnectionDisconnected(const QString &peerAddress, quint16 peerPort);
    void signalTCPConnectionError(const QString &peerAddress, quint16 peerPort, QAbstractSocket::SocketError error);

public slots:

    bool slotCreateTCPConnection(const QString &IP, quint16 Port);

    void slotProcessNewTCPConnectionConnected(TcpSocketConnection *tcpSocketConnection);

    bool slotSendNewTCPDatagram(const QHostAddress &targetAddress, quint16 targetPort, const QByteArray &data);
    bool slotSendNewUDPDatagram(const QHostAddress &targetAddress, quint16 targetPort, const QByteArray &data, quint16 localPort);

    bool slotSendPacket(Packet *packet);

private slots:
    void slotProcessNewIncomingTCPConnection(int socketDescriptor);
    void slotConnectionError(QAbstractSocket::SocketError socketError);
    void slotProcessTcpSocketDisconnected();
    void slotTcpSocketReadyForUse();

    //接收到数据时执行
    //	virtual void slotUDPDatagramsReceived(const QString &ip, quint16 port, const QByteArray &data) = 0;
    //	virtual void slotTCPDatagramsReceived(const QString &ip, quint16 port, const QByteArray &data) = 0;



private:


private:

    PacketHandlerBase *m_packetHandlerBase;

    NetworkType networkType;
    CommunicationMode communicationMode;

    QMultiHash<quint16, UDPServer *> udpServers;
    QMultiHash<quint16, TcpServer *> tcpServers;
    QMultiHash<QHostAddress, TcpSocketConnection *> tcpSocketConnections;
    QMutex udpMutex;
    QMutex tcpMutex;




};

} //namespace HEHUI

Q_DECLARE_METATYPE(QAbstractSocket::SocketError);


#endif /* NETWORKMANAGERBASE_H_ */
