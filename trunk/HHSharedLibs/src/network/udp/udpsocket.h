/*
 ****************************************************************************
 * udpsocket.h
 *
 * Created on: 2009-9-3
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



#ifndef UDPSOCKET_H_
#define UDPSOCKET_H_

#include <QObject>

#include "multicast/ipmulticast.h"
#include "../packethandler/packethandlerbase.h"

namespace HEHUI {

class UDPSocket : public QUdpSocket {
    Q_OBJECT
public:
    enum ListeningState{NotListening = 1, SimpleListening = 2, IPMulticastListening = 3};

    UDPSocket(PacketHandlerBase *packetHandlerBase, QObject *parent);
    virtual ~UDPSocket();

    const IPMulticastSocket * getIPMulticastSocket() const;

    ListeningState getListeningState() const;

    bool startSimpleListening(const QHostAddress &localAddress, quint16 localPort);
    bool startIPMulticastListening(const QHostAddress &ipMulticastGroupAddress, quint16 ipMulticastGroupPort);


    static bool sendUDPDatagram(const QHostAddress &targetAddress, quint16 targetPort, const QByteArray &data, QString *errorString = 0);


signals:
    //    void signalNewUDPPacketReceived(Packet *packet);

private slots:
    virtual void readPendingDatagrams();

private:

    PacketHandlerBase *m_packetHandlerBase;

    QByteArray *datagram;

    IPMulticastSocket *ipMulticastSocket;

    ListeningState listeningState;

};

typedef UDPSocket UDPServer;

} //namespace HEHUI

#endif /* UDPSOCKET_H_ */
