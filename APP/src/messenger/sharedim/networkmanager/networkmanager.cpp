/*
 * networkmanager.cpp
 *
 *  Created on: 2010-7-21
 *      Author: HeHui
 */



#include <QHostInfo>
#include <QDebug>

#include "networkmanager.h"


#include "HHSharedNetwork/hnetworkutilities.h"



namespace HEHUI {


NetworkManager::NetworkManager(PacketHandlerBase *packetHandlerBase, NetworkType type, CommunicationMode mode, QObject *parent)
    :NetworkManagerBase(packetHandlerBase, type, mode, parent)
{

    //    Q_ASSERT_X(!localAddress.isNull(), "NetworkManager(...)", "Invalid local hostaddress!");
    //    Q_ASSERT_X(localPort != 0, "NetworkManager(...)", "Invalid local port!");


    //    QList<QHostAddress> ips = NetworkUtilities::ipAddresses();
    //    foreach(QHostAddress address, ips){
    //        if(address.toString().contains("200.200", Qt::CaseInsensitive) || address.toString().contains("193.168", Qt::CaseInsensitive)){
    //            localAddress = address;
    //            qWarning()<<"localAddress:"<<address.toString();
    //        }
    //    }

    m_hostName = QHostInfo::localHostName();
//    m_localTCPListeningAddress = QHostAddress::Any;
//    m_localTCPListeningPort = quint16(IM_SERVER_TCP_LISTENING_PORT);
    m_localIPMCListeningAddress = QHostAddress::Any;
    m_localIPMCListeningPort = quint16(IM_SERVER_IPMC_LISTENING_PORT);
    m_ipMCGroupAddress = QHostAddress(IM_SERVER_IPMC_ADDRESS);

    m_localRUDPListeningAddress = QHostAddress::Any;
    m_localRUDPListeningPort = 0;


    //    if(!isNetworkReady()){
    //        startWaitingNetworkReady();
    //    }

    checkNetworkStatusTimer = 0;

}

NetworkManager::~NetworkManager() {

    qDebug()<<"NetworkManager::~NetworkManager()";

    if(checkNetworkStatusTimer){
        checkNetworkStatusTimer->stop();
        delete checkNetworkStatusTimer;
        checkNetworkStatusTimer = 0;
    }

}

QString NetworkManager::hostName(){
    return m_hostName;
}

//QHostAddress NetworkManager::localTCPListeningAddress(){
//    return m_localTCPListeningAddress;
//}

//void NetworkManager::setLocalTCPListeningAddress(const QHostAddress &address){
//    this->m_localTCPListeningAddress = address;
//}

//quint16 NetworkManager::localTCPListeningPort(){
//    return m_localTCPListeningPort;
//}

//void NetworkManager::setLocalTCPListeningPort(quint16 port){
//    this->m_localTCPListeningPort = port;
//}

QHostAddress NetworkManager::localIPMCListeningAddress(){
    return m_localIPMCListeningAddress;
}

void NetworkManager::setLocalIPMCListeningAddress(const QHostAddress &address){
    this->m_localIPMCListeningAddress = address;
}

quint16 NetworkManager::localIPMCListeningPort(){
    return m_localIPMCListeningPort;
}

void NetworkManager::setLocalIPMCListeningPort(quint16 port){
    this->m_localIPMCListeningPort = port;
}

QHostAddress NetworkManager::ipMCGroupAddress(){
    return m_ipMCGroupAddress;
}

void NetworkManager::setIPMCGroupAddress(const QHostAddress &address){
    this->m_ipMCGroupAddress = address;
}

QHostAddress NetworkManager::localRUDPListeningAddress(){
    return m_localRUDPListeningAddress;
}

quint16 NetworkManager::localRUDPListeningPort(){
    return m_localRUDPListeningPort;
}

//QString NetworkManager::hardwareAddress() const{
//    return m_hardwareAddress;
//}


UDPServer * NetworkManager::startIPMCServer(){
    //    if(!isNetworkReady()){
    //        startWaitingNetworkReady();
    //        return false;
    //    }



    return startIPMulticastServerListening(m_ipMCGroupAddress, m_localIPMCListeningPort);
}

quint16 NetworkManager::startUDPServer(const QHostAddress &address, quint16 startPort, bool tryOtherPort){
    UDPServer *udpServer = 0;
    udpServer = startUDPServerListening(address, startPort);
    if(!udpServer && tryOtherPort){
        udpServer = startUDPServerListening(address, 0);
    }

    if(udpServer){
        return udpServer->localPort();
    }else{
        return 0;
    }

    //    if(startPort == 0){
    //        if(startUDPServerListening(address, 0)){
    //            return 0;
    //        }
    //    }else{
    //        for(quint16 i = startPort; i<65535; i++){
    //            if(startUDPServerListening(address, i)){
    //                return i;
    //            }
    //        }
    //    }

    //    return 0;

}

RUDPSocket * NetworkManager::startRUDPServer(const QHostAddress &address, quint16 port, bool tryOtherPort){

    RUDPSocket *rudpSocket = 0;
    if(m_localRUDPListeningPort){
        rudpSocket = getRUDPServer(m_localRUDPListeningPort, m_localRUDPListeningAddress);
    }
    if(!rudpSocket){
        rudpSocket = startRUDPServerListening(address, port, IM_RUDP_KEEPALIVE_TIMER_INTERVAL);
    }
    if(!rudpSocket && tryOtherPort){
        rudpSocket = startRUDPServerListening(address, 0, IM_RUDP_KEEPALIVE_TIMER_INTERVAL);
    }
    if(rudpSocket){
        m_localRUDPListeningPort = rudpSocket->localPort();
        m_localRUDPListeningAddress = address;
    }

    return rudpSocket;
}

bool NetworkManager::isNetworkReady(){
    qDebug()<<"----NetworkManager::isNetworkReady()";

    qApp->processEvents();
    localIPAddresses = NetworkUtilities::validIPAddresses();

    if(!localIPAddresses.isEmpty()){
        if(checkNetworkStatusTimer){
            checkNetworkStatusTimer->stop();
            delete checkNetworkStatusTimer;
            checkNetworkStatusTimer = 0;
        }
        emit signalNetworkReady();
        return true;
    }

    return false;
}

void NetworkManager::startWaitingNetworkReady(){
    qDebug()<<"----NetworkManager::startWaitingNetworkReady()";

    if(checkNetworkStatusTimer){
        //        if(!checkNetworkStatusTimer->isActive()){
        //            checkNetworkStatusTimer->start();
        //        }
        return;
    }
    checkNetworkStatusTimer = new QTimer(this);
    checkNetworkStatusTimer->setSingleShot(false);
    checkNetworkStatusTimer->setInterval(10000);
    connect(checkNetworkStatusTimer, SIGNAL(timeout()), this, SLOT(isNetworkReady()));
    checkNetworkStatusTimer->start();
}
















}
