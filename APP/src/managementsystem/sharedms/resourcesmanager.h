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

#include "global_shared.h"
#include "udtprotocol.h"
#include "filemanager.h"
#include "sharedmslib.h"
#include "tcpserver.h"

#include "HHSharedNetwork/hudpsocket.h"
#include "HHSharedUDT/hudtprotocolforfiletransmission.h"




namespace HEHUI {


class SHAREDMSLIB_API ResourcesManager : public QObject {
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

    TCPServer * getTCPServer(){return m_tcpServer;}
    TCPServer * startTCPServer(const QHostAddress &address = QHostAddress::Any, quint16 port = 0, bool tryOtherPort = true, QString *errorMessage = 0);

    bool sendReliableData(int socketID, const QByteArray *byteArray);


    FileManager *getFileManager();


private:
    UDTProtocolForFileTransmission * getUDTProtocolForFileTransmission();


private:

    UDPServer *ipmcServer;
    UDPServer *udpServer;
    UDTProtocol *udtProtocol;
    TCPServer *m_tcpServer;

    FileManager *m_fileManager;

    UDTProtocolForFileTransmission *m_udtProtocolForFileTransmission;
    

};

}

#endif /* NETWORKMANAGER_H_ */
