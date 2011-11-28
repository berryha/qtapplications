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


namespace HEHUI {


ResourcesManager::ResourcesManager(QObject *parent)
    :QObject(parent)
{


    ipmcServer = 0;
    udpServer = 0;
    udtProtocol = 0;



    //注册自定义类型，必须重载“<<”和“>>”, 见"packetstreamoperator.h"
    //qRegisterMetaTypeStreamOperators<HEHUI::Packet>("HEHUI::Packet");


}

ResourcesManager::~ResourcesManager() {

    qDebug()<<"NetworkManager::~NetworkManager()";

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
        udtProtocol = new UDTProtocol(this);
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
















}
