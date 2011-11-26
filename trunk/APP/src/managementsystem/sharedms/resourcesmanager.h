/*
 * networkmanager.h
 *
 *  Created on: 2010-7-21
 *      Author: HeHui
 */

#ifndef RESOURCESMANAGER_H_
#define RESOURCESMANAGER_H_

#include <QObject>
#include <QCoreApplication>

#include "udtprotocol.h"

#include "global_shared.h"
#include "HHSharedCore/hmysharedlib_global.h"
#include "HHSharedNetwork/hudpsocket.h"


namespace HEHUI {


class MYSHAREDLIB_API ResourcesManager : public QObject {
    Q_OBJECT

public:
    ResourcesManager(QObject *parent);
    virtual ~ResourcesManager();


    UDPServer * getIPMCServer(){return ipmcServer;}
    UDPServer * startIPMCServer(const QHostAddress &ipmcGroupAddress = QHostAddress(IP_MULTICAST_GROUP_ADDRESS), quint16 ipmcGroupPort = quint16(IP_MULTICAST_GROUP_PORT), QString *errorMessage = 0);

    UDPServer * getUDPServer(){return udpServer;}
    UDPServer * startUDPServer(const QHostAddress &address = QHostAddress::Any, quint16 port = 0, bool tryOtherPort = true, QString *errorMessage = 0);

    UDTProtocol * getUDTProtocol(){return udtProtocol;}
    UDTProtocol * startUDTProtocol(const QHostAddress &localAddress = QHostAddress::Any, quint16 localPort = 0, bool tryOtherPort = true, QString *errorMessage = 0);


private:


    UDPServer *ipmcServer;
    UDPServer *udpServer;
    UDTProtocol *udtProtocol;
    
    

};

}

#endif /* NETWORKMANAGER_H_ */
