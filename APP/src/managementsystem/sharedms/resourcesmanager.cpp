/*
 * networkmanager.cpp
 *
 *  Created on: 2010-7-21
 *      Author: HeHui
 */



#include <QHostInfo>
#include <QDebug>

#include "resourcesmanager.h"


#include "HHSharedNetwork/hnetworkutilities.h"
//#include "HHSharedNetwork/hpacketstreamoperator.h"


namespace HEHUI {


ResourcesManager::ResourcesManager(QObject *parent)
    :QObject(parent)
{


    ipmcServer = 0;
    udpServer = 0;
    udtProtocol = new UDTProtocol(true, 0, this);
    m_tcpServer = new TCPServer(this);

    m_fileManager = 0;

    m_udtProtocolForFileTransmission = 0;


    //注册自定义类型，必须重载“<<”和“>>”, 见"packetstreamoperator.h"
    //if(!QMetaType::isRegistered(QMetaType::type("HEHUI::Packet"))){
    //    qRegisterMetaTypeStreamOperators<HEHUI::Packet>("HEHUI::Packet");
    //}

}

ResourcesManager::~ResourcesManager() {

    qDebug()<<"ResourcesManager::~ResourcesManager()";

    if(ipmcServer){
        ipmcServer->close();
        delete ipmcServer;
        ipmcServer = 0;
    }


    if(udpServer){
        udpServer->close();
        delete udpServer;
        udpServer = 0;
    }

    if(udtProtocol){
        udtProtocol->closeUDTProtocol();
        delete udtProtocol;
        udtProtocol = 0;
    }

    if(m_tcpServer){
        delete m_tcpServer;
        m_tcpServer = 0;
    }

    if(m_fileManager){
        m_fileManager->exit();
        delete m_fileManager;
        m_fileManager = 0;
    }



}


UDPServer * ResourcesManager::startIPMCServer(const QHostAddress &ipmcGroupAddress, quint16 ipmcGroupPort, QString *errorMessage){


    if(!ipmcServer){
        ipmcServer = new UDPServer(this);
    }

    if (ipmcServer->startIPMulticastListening(ipmcGroupAddress, ipmcGroupPort)) {
        return ipmcServer;
    }else{
        if(errorMessage){
            *errorMessage = ipmcServer->errorString();
        }

        delete ipmcServer;
        ipmcServer = 0;

        return 0;
    }

    return ipmcServer;

}

UDPServer * ResourcesManager::startUDPServer(const QHostAddress &address, quint16 startPort, bool tryOtherPort, QString *errorMessage){

    if(!udpServer){
        udpServer = new UDPServer(this);
    }

    if(udpServer->localPort() == startPort){
        return udpServer;
    }

    if (udpServer->startSimpleListening(address, startPort)) {
        return udpServer;
    }else{
        if(tryOtherPort){
            if (udpServer->startSimpleListening(address, 0)) {
                return udpServer;
            }
        }

        if(errorMessage){
            *errorMessage = udpServer->errorString();
        }
        delete udpServer;
        udpServer = 0;

        return 0;
    }

    return udpServer;

}

UDTProtocol * ResourcesManager::startUDTProtocol(const QHostAddress &localAddress, quint16 localPort, bool tryOtherPort, QString *errorMessage){

    if(!udtProtocol){
        udtProtocol = new UDTProtocol(true, 0, this);
    }

    UDTSOCKET socket = udtProtocol->listen(localPort, localAddress);
    if(socket == UDTProtocolBase::INVALID_UDT_SOCK && tryOtherPort){
        socket = udtProtocol->listen();
    }

    if(socket == UDTProtocolBase::INVALID_UDT_SOCK){
        if(errorMessage){
            *errorMessage = udtProtocol->getLastErrorMessage();
        }
        delete udtProtocol;
        udtProtocol = 0;
        return 0;
    }

    return udtProtocol;

}

TCPServer * ResourcesManager::startTCPServer(const QHostAddress &address, quint16 port, bool tryOtherPort, QString *errorMessage){

    if(!m_tcpServer){
        m_tcpServer = new TCPServer(this);
    }

    if( (!m_tcpServer->listen(address, port)) && tryOtherPort){
        m_tcpServer->listen();
    }

    if(!m_tcpServer->isListening()){
        if(errorMessage){
            *errorMessage = m_tcpServer->serverErrorString();
        }
        delete m_tcpServer;
        m_tcpServer = 0;
    }

    return m_tcpServer;

}

bool ResourcesManager::sendReliableData(int socketID, const QByteArray *byteArray){
    if(!m_tcpServer->sendData(socketID, byteArray)){
        return udtProtocol->sendData(socketID, byteArray);
    }
    return true;

}

FileManager * ResourcesManager::getFileManager(){
    if(!m_fileManager){
        m_fileManager = new FileManager(this);
        m_fileManager->start(QThread::LowestPriority);
    }

    return m_fileManager;

}

UDTProtocolForFileTransmission * ResourcesManager::getUDTProtocolForFileTransmission(){

    if(!m_udtProtocolForFileTransmission){
        m_udtProtocolForFileTransmission = new UDTProtocolForFileTransmission(this);
        m_udtProtocolForFileTransmission->listen(UDT_FILE_LISTENING_PORT);
        m_udtProtocolForFileTransmission->startWaitingForNewConnectionInOneThread(50);

    }

    return m_udtProtocolForFileTransmission;

}
















}
