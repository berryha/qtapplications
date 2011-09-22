/*
 ****************************************************************************
 * networkmanagerbase.cpp
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




#include <QStringList>
#include <QProcess>
#include <QDebug>

#include "networkmanagerbase.h"
//#include "./udpServer/udpServer.h"
//#include "tcp/tcpserver.h"
#include "tcp/tcpsocketthread.h"


namespace HEHUI {


//QMultiHash<quint16, UDPServer *> NetworkManagerBase::udpServers = QMultiHash<quint16, UDPServer *> ();
//QMultiHash<quint16, TcpServer *> NetworkManagerBase::tcpServers = QMultiHash<quint16, TcpServer *> ();
//QMultiHash<QHostAddress, TcpSocketConnection *> NetworkManagerBase::tcpSocketConnections = QMultiHash<QHostAddress, TcpSocketConnection *> ();

//QMutex * NetworkManagerBase::udpMutex = new QMutex();
//QMutex * NetworkManagerBase::tcpMutex = new QMutex();


NetworkManagerBase::NetworkManagerBase(PacketHandlerBase *packetHandlerBase, NetworkType type, CommunicationMode mode, QObject *parent)
    : QObject(parent), m_packetHandlerBase(packetHandlerBase), networkType(type), communicationMode(mode)
{

    //Q_ASSERT_X(m_packetHandlerBase, "NetworkManagerBase::NetworkManagerBase(...)", "Invalid PacketHandlerBase!");


    qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
    qRegisterMetaType<QHostAddress>("QHostAddress");

    m_errorString = "";


}

NetworkManagerBase::~NetworkManagerBase() {
    qDebug()<<"NetworkManagerBase::~NetworkManagerBase()";

    closeAllServers();

}

void NetworkManagerBase::closeUDPServer(quint16 port){
    QMutexLocker udpLocker(&udpMutex);
    if(!udpServers.isEmpty()){
        QList<UDPServer *> udpServerList = (port == 0)?(udpServers.values()):(udpServers.values(port));
        foreach(UDPServer *udpServer, udpServerList){
            if(!udpServer){continue;}
            //if(udpServer->localPort() == port){
                udpServer->close();
                delete udpServer;
                udpServer = 0;
                udpServers.remove(port, udpServer);
            //}

        }

    }
    
}

void NetworkManagerBase::closeRUDPServer(quint16 port){
    QMutexLocker rudpLocker(&rudpMutex);
    if(!rudpServers.isEmpty()){
        QList<RUDPServer *> rudpServerList = (port == 0)?(rudpServers.values()):(rudpServers.values(port));
        foreach(RUDPServer *rudpServer, rudpServerList){
            if(!rudpServer){continue;}
                rudpServer->close();
                delete rudpServer;
                rudpServer = 0;
                rudpServers.remove(port, rudpServer);
        }

    }

}

void NetworkManagerBase::closeTCPServer(quint16 port){

    QMutexLocker tcpLocker(&tcpMutex);
    if(!tcpServers.isEmpty()){
        QList<TcpServer *> tcpServerList = (port == 0)?(tcpServers.values()):(tcpServers.values(port));
        foreach(TcpServer *tcpServer, tcpServerList){
            if(!tcpServer){continue;}
            //if(tcpServer->serverPort() == port){
                tcpServer->close();
                delete tcpServer;
                tcpServer = 0;
                tcpServers.remove(port, tcpServer);
            //}

        }

    }

}

void NetworkManagerBase::closeAllServers(){
    qDebug()<<"NetworkManagerBase::closeAllServers()";

    QMutexLocker mcLocker(&udpMutex);
    if(!udpServers.isEmpty()){
        QList<UDPServer *> udpServerList = udpServers.values();
        foreach(UDPServer *udpServer, udpServerList){
            if(!udpServer){continue;}
            udpServer->close();
            delete udpServer;
            udpServer = 0;
        }
        udpServerList.clear();
        udpServers.clear();
    }

    QMutexLocker rudpLocker(&rudpMutex);
    if(!rudpServers.isEmpty()){
        QList<RUDPServer *> rudpServerList = rudpServers.values();
        foreach(RUDPServer *rudpServer, rudpServerList){
            if(!rudpServer){continue;}
                rudpServer->close();
                delete rudpServer;
                rudpServer = 0;
        }
        rudpServerList.clear();
        rudpServers.clear();
    }

    QMutexLocker tcpLocker(&tcpMutex);
    if(!tcpServers.isEmpty()){
        QList<TcpServer *> tcpServerList = tcpServers.values();
        foreach(TcpServer *tcpServer, tcpServerList){
            if(!tcpServer){continue;}
            tcpServer->close();
            delete tcpServer;
            tcpServer = 0;
        }
        tcpServerList.clear();
        tcpServers.clear();

    }

    if(!tcpSocketConnections.isEmpty()){
        //TODO:connection?
        QList<TcpSocketConnection *> connections = tcpSocketConnections.values();
        foreach (TcpSocketConnection *connection, connections){
            if(connection){connection->destory();}
        }
        connections.clear();
        tcpSocketConnections.clear();
    }

}

UDPServer * NetworkManagerBase::getUDPServer(quint16 port, const QHostAddress &localAddress){
    //    qDebug()<<"----NetworkManagerBase::getUDPServer(...)";

    QMutexLocker locker(&udpMutex);

    QHostAddress address = localAddress;
    if(localAddress == QHostAddress::Null){
        address = QHostAddress::Any;
    }

    QList<UDPServer *> list; // = udpServers.values(port);
    if(port == 0){
        list = udpServers.values();
    }else{
        list = udpServers.values(port);
    }
    if(!list.isEmpty()){
        if(address == QHostAddress::Any){
            return list.at(0);
        }
        foreach(UDPServer *udpServer, list){
            if((udpServer->localAddress() == address) || (udpServer->localAddress() == QHostAddress::Any)){  
                return udpServer;
            }
        }
    }

    return 0;

}

UDPServer * NetworkManagerBase::startUDPServerListening(const QHostAddress &localAddress, quint16 localPort) {
    //    qDebug()<< "----NetworkManagerBase::startUDPServerListening(...)-"<<localAddress.toString()<<" Port:"<<localPort;

    m_errorString = "";

    UDPServer *udpServer = getUDPServer(localPort, localAddress);
    if(udpServer){
        m_errorString = tr("UDP Server has already started!");
        qWarning() << m_errorString;
        return udpServer;
    }else{
        udpServer = new UDPServer(m_packetHandlerBase, this);
    }

    QHostAddress address = localAddress;
    if(localAddress == QHostAddress::Null){
        address = QHostAddress::Any;
    }

    if (udpServer->startSimpleListening(address, localPort)) {
        QMutexLocker locker(&udpMutex);
        udpServers.insert(udpServer->localPort(), udpServer);
        return udpServer;
    }

    m_errorString = udpServer->errorString();
    qCritical()<<QString("ERROR! Failed to start UDP Server Listening! Local Address:%1, Port:%2. %3 ").arg(localAddress.toString()).arg(localPort).arg(errorString());

    delete udpServer;
    udpServer = 0;


    return udpServer;

}

bool NetworkManagerBase::startIPMulticastServerListening(const QHostAddress &ipMulticastGroupAddress, quint16 ipMulticastGroupPort) {
    //    qDebug()<< "----NetworkManagerBase::startIPMulticastServerListening(const QHostAddress &localAddress, quint16 localPort)";

    m_errorString = "";

    UDPServer *udpServer = getUDPServer(ipMulticastGroupPort, QHostAddress::Any);
    if(udpServer){
        qWarning("IP Multicast Server has already started!");
        return true;
    }else{
        udpServer = new UDPServer(m_packetHandlerBase, this);

        if (udpServer->startIPMulticastListening(ipMulticastGroupAddress, ipMulticastGroupPort)) {
            QMutexLocker locker(&udpMutex);
            udpServers.insert(ipMulticastGroupPort, udpServer);
            return true;
        }else{
            m_errorString = udpServer->errorString();
            qCritical()<<QString("ERROR! Failed to start IPMulticastServer Listening! Multicast Group Address:%1, Port:%2. %3").arg(ipMulticastGroupAddress.toString()).arg(ipMulticastGroupPort).arg(m_errorString);

            delete udpServer;
            udpServer = 0;

            return false;
        }

    }


    return false;

}

RUDPServer * NetworkManagerBase::getRUDPServer(quint16 port, const QHostAddress &localAddress){

    QMutexLocker locker(&rudpMutex);

    QHostAddress address = localAddress;
    if(localAddress == QHostAddress::Null){
        address = QHostAddress::Any;
    }

    QList<RUDPServer *> list;
    if(port == 0){
        list = rudpServers.values();
    }else{
        list = rudpServers.values(port);
    }
    if(!list.isEmpty()){
        if(address == QHostAddress::Any){
            return list.at(0);
        }
        foreach(RUDPServer *rudpServer, list){
            if((rudpServer->localAddress() == address) || (rudpServer->localAddress() == QHostAddress::Any)){
                return rudpServer;
            }
        }
    }

    return 0;

}

RUDPServer * NetworkManagerBase::startRUDPServerListening(const QHostAddress &localAddress, quint16 localPort){

    m_errorString = "";

    RUDPServer *rudpServer = getRUDPServer(localPort, localAddress);
    if(rudpServer){
        m_errorString = tr("RUDP Server has already started!");
        qWarning()<<m_errorString;
        return rudpServer;
    }else{
        rudpServer = new RUDPServer(m_packetHandlerBase, this);
    }

    QHostAddress address = localAddress;
    if(localAddress == QHostAddress::Null){
        address = QHostAddress::Any;
    }

    if (rudpServer->bind(address, localPort)) {
        QMutexLocker locker(&rudpMutex);
        rudpServers.insert(rudpServer->localPort(), rudpServer);
        return rudpServer;
    }

    m_errorString = rudpServer->errorString();

    qCritical()<<QString("ERROR! Failed to start RUDP Server Listening! Local Address:%1, Port:%2. %3").arg(localAddress.toString()).arg(localPort).arg(m_errorString);

    delete rudpServer;
    rudpServer = 0;
    return 0;

}

TcpServer * NetworkManagerBase::getTcpServer(quint16 port, const QHostAddress &serverIPAddress){
    QMutexLocker locker(&tcpMutex);

    QList<TcpServer *> list = tcpServers.values(port);
    if(!list.isEmpty()){
        if(serverIPAddress == QHostAddress::Any){
            return list.at(0);
        }
        foreach(TcpServer *server, list){
            QHostAddress serverAddress = server->serverAddress();
            if((serverAddress == serverIPAddress) || (serverAddress == QHostAddress::Any)){
                return server;
            }
        }
    }

    return 0;
}

bool NetworkManagerBase::startTCPServerListening(const QHostAddress &localAddress, quint16 port) {
    //qDebug()<< "----NetworkManagerBase::slotStartTCPServerListening(const QString &IP, quint16 Port)";

    m_errorString = "";

    TcpServer *tcpServer = getTcpServer(port, localAddress);
    if(tcpServer){
        qWarning("TCP Server has already started!");
        return true;
    }else{
        QHostAddress serverAddress = localAddress;
        if(serverAddress.isNull()){
            serverAddress = QHostAddress::Any;
        }
        tcpServer = new TcpServer(serverAddress, port, this);
    }

    if (tcpServer->isListening()) {
        connect(tcpServer, SIGNAL(signalNewIncomingTCPConnection(int)), this, SLOT(slotProcessNewIncomingTCPConnection(int)));
        QMutexLocker locker(&tcpMutex);
        tcpServers.insert(tcpServer->serverPort(), tcpServer);
        return true;
    } else {

        m_errorString = tcpServer->errorString();
        qCritical()<<QString("ERROR! Failed to start TCPServer Listening! %1").arg(m_errorString);

        tcpServer->close();
        tcpServer->deleteLater();

        return false;
    }

}

void NetworkManagerBase::slotProcessNewIncomingTCPConnection(int socketDescriptor) {
    qDebug("----NetworkManagerBase::slotProcessNewIncomingConnection(int socketDescriptor)");

    TcpSocketThread *thread = new TcpSocketThread(socketDescriptor, m_packetHandlerBase, this);
    connect(thread, SIGNAL(signalNewTCPConnectionConnected(TcpSocketConnection *)), this, SLOT(slotProcessNewTCPConnectionConnected(TcpSocketConnection *)));
    //	connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
}

bool NetworkManagerBase::slotCreateTCPConnection(const QString &IP, quint16 Port) {

    m_errorString = "";

    QMutexLocker locker(&tcpMutex);

    TcpSocketThread *thread = new TcpSocketThread(QHostAddress(IP), Port, m_packetHandlerBase, this);
    connect(thread, SIGNAL(signalNewTCPConnectionConnected(TcpSocketConnection *)), this, SLOT(slotProcessNewTCPConnectionConnected(TcpSocketConnection *)));
    //	connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();

    return thread->isRunning();

}

bool NetworkManagerBase::slotSendNewTCPDatagram(const QHostAddress &targetAddress, quint16 targetPort, const QByteArray &data) {
    qDebug()<<"----NetworkManagerBase::slotSendNewTCPDatagram(...)";

    m_errorString = "";

    if (data.isEmpty()) {
        m_errorString = tr("Data is empty!");
        return false;
    }

    bool result = false;

    if (targetAddress == QHostAddress::Null) {
        QList<TcpSocketConnection *> connections = tcpSocketConnections.values();
        foreach (TcpSocketConnection *connection, connections)
        {
            result = connection->sendTCPDatagram(data);
        }
    } else {
        if (!hasConnection(targetAddress, targetPort)) {
            slotCreateTCPConnection(targetAddress.toString(), targetPort);
        }

        //TODO:同步！
        TcpSocketConnection *connection = getConnection(targetAddress.toString(), targetPort);
        if (connection) {
            result = connection->sendTCPDatagram(data);
            m_errorString = connection->errorString();
        }else{
            qWarning()<<"Can not get tcp connection!"<<" Target:"<<targetAddress.toString()<<" Port:"<<targetPort;;;
        }

    }

    if (result) {
        qDebug()<< "TCP Datagram Sent Successfully! "<<" Target:"<<targetAddress.toString()<<" Port:"<<targetPort;;
    } else {
        qCritical()<< "ERROR! TCP Datagram Sent Failed! "<<" Target:"<<targetAddress.toString()<<" Port:"<<targetPort;
    }


    return result;

}

bool NetworkManagerBase::slotSendNewUDPDatagram(const QHostAddress &targetAddress, quint16 targetPort, QByteArray *data, quint16 localPort, bool useRUDP){
    //qDebug()<< "NetworkManagerBase::slotSendNewUDPDatagram(...): Target Address:"<<targetAddress.toString()<<" Target Port:"<<targetPort << " Local Port"<<localPort;

    m_errorString = "";

    bool result = false;

    RUDPServer *rudpServer = getRUDPServer(localPort, QHostAddress::Any);
    if(!rudpServer){
        m_errorString = tr("RUDP Server Not Running On Port %1!").arg(localPort);
        qCritical()<<m_errorString;
        return false;
    }
    int sentSize = rudpServer->sendDatagram(targetAddress, targetPort, data, useRUDP);
    result = (sentSize == data->size());
    m_errorString = rudpServer->errorString();

//    if(useRUDP){
//        RUDPServer *rudpServer = getRUDPServer(localPort, QHostAddress::Any);
//        if(!rudpServer){
//            m_errorString = tr("RUDP Server Not Running!");
//            qCritical()<<m_errorString;
//            return false;
//        }
//        int sentSize = rudpServer->sendDatagram(targetAddress, targetPort, data);
//        result = (sentSize == data->size());
//        m_errorString = rudpServer->errorString();

//    }else{
//        if(localPort == 0){
//            result = UDPSocket::sendUDPDatagram(targetAddress, targetPort, *data, &m_errorString);
//        }else{
//            UDPServer *udpServer = getUDPServer(localPort, QHostAddress::Any);
//            if (udpServer) {
//                qint64 size = udpServer->writeDatagram(*data, targetAddress,targetPort);
//                //result = (size == data->size())?true:false;
//                result = (size == data->size());
//                m_errorString = udpServer->errorString();

//            } else {
//                result = UDPSocket::sendUDPDatagram(targetAddress, targetPort, *data, &m_errorString);
//            }


//        }

//        if (result) {
//            //qDebug()<< "UDP Datagram Sent Successfully! "<<" Target:"<<targetAddress.toString()<<" Port:"<<targetPort;;
//        } else {
//            qCritical()<< "ERROR! UDP Datagram Sent Failed! "<<" Target:"<<targetAddress.toString()<<" Port:"<<targetPort;
//        }

//    }


    return result;



}

bool NetworkManagerBase::slotSendPacket(Packet *packet){

    if (!packet) {
        qCritical()<<"ERROR! NULL Packet!";
        return false;
    }

    if (!packet->isValid()) {
        qCritical()<<"ERROR! Invalid Packet!";
        delete packet;
        packet = 0;
        return false;
    }


    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_7);
    QVariant v;
    v.setValue(*packet);
    out << v;
    qDebug()<<"----processOutgoingPackets~~block.size():"<<block.size();

    TransmissionProtocol transmissionProtocol = packet->getTransmissionProtocol();

    bool result = false;
    if (transmissionProtocol == TP_TCP) {
        result = slotSendNewTCPDatagram(QHostAddress(packet->getPeerHostAddress()), packet->getPeerHostPort(), block);
    } else if (transmissionProtocol == TP_UDP) {
        //UDPPacket *udpPacket = static_cast<UDPPacket *> (packet);
        result = slotSendNewUDPDatagram(QHostAddress(packet->getPeerHostAddress()), packet->getPeerHostPort(), &block, packet->getLocalHostPort(), false);

    }else if(transmissionProtocol == TP_RUDP){
        result = slotSendNewUDPDatagram(QHostAddress(packet->getPeerHostAddress()), packet->getPeerHostPort(), &block, packet->getLocalHostPort(), true);
    }

    return result;

}

void NetworkManagerBase::setPacketHandler(PacketHandlerBase *packetHandlerBase){
    this->m_packetHandlerBase = packetHandlerBase;
}

PacketHandlerBase * NetworkManagerBase::getPacketHandler(){
    return this->m_packetHandlerBase;
}

void NetworkManagerBase::setNetworkType(NetworkType type) {
    this->networkType = type;

}

NetworkManagerBase::NetworkType NetworkManagerBase::getNetworkType() const {
    return this->networkType;

}

void NetworkManagerBase::setCommunicationMode(CommunicationMode mode) {
    this->communicationMode = mode;

}

NetworkManagerBase::CommunicationMode NetworkManagerBase::getCommunicationMode() const {
    return this->communicationMode;

}

bool NetworkManagerBase::hasConnection(const QHostAddress &senderIp, quint16 senderPort){
    QMutexLocker locker(&tcpMutex);

    if (senderPort == 0)
        return tcpSocketConnections.contains(senderIp);

    if (!tcpSocketConnections.contains(senderIp))
        return false;

    QList<TcpSocketConnection *> connections = tcpSocketConnections.values(senderIp);
    foreach (TcpSocketConnection *connection, connections)
    {
        if (connection->peerPort() == senderPort)
            return true;
    }

    return false;
}

void NetworkManagerBase::slotProcessNewTCPConnectionConnected(TcpSocketConnection *tcpSocketConnection) {
    connect(tcpSocketConnection, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotConnectionError(QAbstractSocket::SocketError)));

    connect(tcpSocketConnection, SIGNAL(disconnected()), this, SLOT(slotProcessTcpSocketDisconnected()));

    connect(tcpSocketConnection, SIGNAL(readyRead()), this, SLOT(slotTcpSocketReadyForUse()));

    //connect(tcpSocketConnection, SIGNAL(signalNewTCPPacketReceived(Packet *)), this, SIGNAL(signalNewPacketReceived(Packet *)));

    appendConnection(tcpSocketConnection);

}

void NetworkManagerBase::slotTcpSocketReadyForUse() {
    TcpSocketConnection *connection = qobject_cast<TcpSocketConnection *> (
                sender());
    if (!connection){
        return;
    }
    if( hasConnection(connection->peerAddress(), connection->peerPort())){
        return;
    }

    //TODO
    connect(connection, SIGNAL(signalTCPDatagramsReceived(const QString &, quint16 , const QByteArray &)),
            this, SLOT(slotTCPDatagramsReceived(const QString &, quint16 , const QByteArray &)));



}

void NetworkManagerBase::slotProcessTcpSocketDisconnected() {
    if (TcpSocketConnection *connection = qobject_cast<TcpSocketConnection *>(sender())){
        emit signalTCPConnectionDisconnected(connection->peerAddress().toString(), connection->peerPort());
        removeConnection(connection);
    }

}

void NetworkManagerBase::slotConnectionError(QAbstractSocket::SocketError socketError ) {
    if (TcpSocketConnection *connection = qobject_cast<TcpSocketConnection *>(sender())) {
        emit signalTCPConnectionError(connection->peerAddress().toString(), connection->peerPort(), socketError);
        removeConnection(connection);
    }

}

void NetworkManagerBase::appendConnection(TcpSocketConnection *connection){
    QMutexLocker locker(&tcpMutex);
    tcpSocketConnections.insert(connection->peerAddress(), connection);

}

void NetworkManagerBase::removeConnection(TcpSocketConnection *connection) {
    QMutexLocker locker(&tcpMutex);
    Q_ASSERT(connection);
    if (tcpSocketConnections.contains(connection->peerAddress())) {
        tcpSocketConnections.remove(connection->peerAddress());
    }
    connection->destory();
    //	connection->deleteLater();
}

TcpSocketConnection* NetworkManagerBase::getConnection(const QString &ip,
                                                       quint16 port) {
    QMutexLocker locker(&tcpMutex);

    if (!tcpSocketConnections.contains(QHostAddress(ip))) {
        return 0;
    }

    QList<TcpSocketConnection *> connections = tcpSocketConnections.values(QHostAddress(ip));
    foreach (TcpSocketConnection *connection, connections)
    {
        if (connection->peerPort() == port)
            return connection;
    }

    return 0;

}

QString NetworkManagerBase::errorString() const{
    return m_errorString;
}

void NetworkManagerBase::setErrorString(const QString &errorString){
    this->m_errorString = errorString;
}






} //namespace HEHUI
