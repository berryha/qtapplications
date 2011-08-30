/*
 ****************************************************************************
 * udpsocket.cpp
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



#include <QUdpSocket>
#include <QDebug>

#include "udpsocket.h"


namespace HEHUI {


UDPSocket::UDPSocket(PacketHandlerBase *packetHandlerBase, QObject *parent)
    :QUdpSocket(parent), m_packetHandlerBase(packetHandlerBase)
{

    Q_ASSERT_X(m_packetHandlerBase, "UDPSocket::UDPSocket(PacketHandlerBase *packetHandlerBase, QObject *parent)", "Invalid PacketHandlerBase!");
    //m_packetHandlerBase = new PacketHandlerBase(this);

    datagram = new QByteArray();
    datagram->resize(0);

    connect(this, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));

    ipMulticastSocket = 0;

    listeningState = NotListening;

}

UDPSocket::~UDPSocket() {
    qDebug()<<"UDPSocket::~UDPSocket()";

    if(ipMulticastSocket){
        delete ipMulticastSocket;
        ipMulticastSocket = 0;
    }

    datagram->clear();
    datagram->resize(0);
    delete datagram;
    datagram = 0;

    close();

}

const IPMulticastSocket * UDPSocket::getIPMulticastSocket() const{
    return ipMulticastSocket;
}

UDPSocket::ListeningState UDPSocket::getListeningState() const{
    return this->listeningState;
}

bool UDPSocket::startSimpleListening(const QHostAddress &localAddress, quint16 localPort){

    if(ipMulticastSocket){
        if(ipMulticastSocket->isBound()){
            ipMulticastSocket->leaveGroup();
        }
    }

    close();

    QHostAddress address = localAddress;
    if(localAddress == QHostAddress::Null){
        address = QHostAddress::Any;
    }

    bool bound = bind(address, localPort);
    if(bound){
        listeningState = SimpleListening;
    }else{
        listeningState = NotListening;
    }

    return bound;

}

bool UDPSocket::startIPMulticastListening(const QHostAddress &ipMulticastGroupAddress, quint16 ipMulticastGroupPort){

    if(!ipMulticastSocket){
        ipMulticastSocket = new IPMulticastSocket(this);

    }else{
        if((ipMulticastSocket->getMulticastGroupAddress() == ipMulticastGroupAddress) && (ipMulticastSocket->getPort() == ipMulticastGroupPort)){
            qWarning("Server already started!");
            return true;
        }
        ipMulticastSocket->leaveGroup();
    }

    close();
    ipMulticastSocket->setUdpSocket(this);

    bool bound = ipMulticastSocket->startIPMulticastListening(ipMulticastGroupAddress, ipMulticastGroupPort);
    if(bound){
        listeningState = IPMulticastListening;
    }else{
        listeningState = NotListening;
    }

    return bound;

}

bool UDPSocket::sendUDPDatagram(const QHostAddress &targetAddress, quint16 targetPort, const QByteArray &data){
    //qDebug()<<"UDPSocket::sendUDPDatagram(...)-targetAddress:"<<targetAddress.toString()<<" targetPort:"<<targetPort;

    QUdpSocket udpSocket;
    qint64 size = udpSocket.writeDatagram(data, targetAddress, targetPort);
    if(size == -1){
        qCritical()<<QString("UDP Datagram Sent Failed! Target Address:%1, Port:%2, Error Code:%3").arg(targetAddress.toString()).arg(targetPort).arg(udpSocket.error());
        return false;
    } 

    return (size == data.size())?true:false;;

}

void UDPSocket::readPendingDatagrams() {
    //qDebug()<<"----UDPSocket::readPendingDatagrams()";

    while (hasPendingDatagrams()) {
        datagram->clear();
        qint64 datagramSize = pendingDatagramSize();
        datagram->resize(datagramSize);
        QHostAddress peerAddress;
        quint16 peerPort;

        qint64 readSize = readDatagram(datagram->data(), datagramSize, &peerAddress, &peerPort);
        if(readSize == -1){
            qWarning()<<"Can not read datagram!";
            break;
        }
        //qDebug()<<"~~datagramSize:"<<datagramSize;

        QDataStream in(datagram, QIODevice::ReadOnly);
        in.setVersion(QDataStream::Qt_4_6);
        QVariant v;
        in >> v;
        if (v.canConvert<Packet>()){
            //Packet *packet = new Packet();
            Packet *packet = m_packetHandlerBase->getPacket();
            *packet = v.value<Packet>();
            packet->setTransmissionProtocol(TP_UDP);
            packet->setPeerHostAddress(peerAddress);
            packet->setPeerHostPort(peerPort);
            packet->setLocalHostAddress(localAddress());
            packet->setLocalHostPort(localPort());

//            qWarning()<<"";
//            qWarning()<<"~~packet->getPacketData().size():"<<packet->getPacketData().size();
//            qWarning()<<"~~peerAddress.toString():"<<peerAddress.toString();
//            qWarning()<<"~~peerPort:"<<peerPort;
//            qWarning()<<"~~localAddress():"<<localAddress().toString();
//            qWarning()<<"~~localPort():"<<localPort();
//            qWarning()<<"";

            m_packetHandlerBase->appendIncomingPacket(packet);

            //                    emit signalNewUDPPacketReceived(packet);

        }

    }


    datagram->clear();
    datagram->resize(0);

}
























} //namespace HEHUI
