/*
 * networkmanager.h
 *
 *  Created on: 2010-7-21
 *      Author: HeHui
 */

#ifndef NETWORKMANAGER_H_
#define NETWORKMANAGER_H_

#include <QObject>
#include <QCoreApplication>

#include "global_shared.h"
#include "HHSharedNetwork/hnetworkmanagerbase.h"

#include "HHSharedCore/hmysharedlib_global.h"

namespace HEHUI {


class MYSHAREDLIB_API NetworkManager : public NetworkManagerBase {
    Q_OBJECT

public:
    NetworkManager(PacketHandlerBase *packetHandlerBase, NetworkType type = LAN, CommunicationMode mode = CS, QObject *parent = 0);
    virtual ~NetworkManager();


    //QString hostName();
//    QHostAddress localTCPListeningAddress();
//    void setLocalTCPListeningAddress(const QHostAddress &address);
//    quint16 localTCPListeningPort();
//    void setLocalTCPListeningPort(quint16 port);

    QHostAddress localIPMCListeningAddress();
    void setLocalIPMCListeningAddress(const QHostAddress &address);
    quint16 localIPMCListeningPort();
    void setLocalIPMCListeningPort(quint16 port);
    QHostAddress ipMCGroupAddress();
    void setIPMCGroupAddress(const QHostAddress &address);

    QHostAddress localRUDPListeningAddress();
    quint16 localRUDPListeningPort();

    QString hardwareAddress() const;


    bool startTCPServer();
    bool startIPMCServer();
    quint16 startUDPServer(const QHostAddress &address = QHostAddress::Any, quint16 port = 0, bool tryOtherPort = true);

    RUDPSocket * startRUDPServer(const QHostAddress &address = QHostAddress::Any, quint16 port = 0, bool tryOtherPort = true);


private:



signals:
    void signalNetworkReady();

public slots:
    bool isNetworkReady();
    void startWaitingNetworkReady();


private slots:



private:


    //QString m_hostName;
    QHostAddress m_localTCPListeningAddress;
    quint16 m_localTCPListeningPort;

    QHostAddress m_localIPMCListeningAddress;
    quint16 m_localIPMCListeningPort;
    QHostAddress m_ipMCGroupAddress;

    QHostAddress m_localRUDPListeningAddress;
    quint16 m_localRUDPListeningPort;

    QTimer *checkNetworkStatusTimer;

    QList<QHostAddress> localIPAddresses ;
    QString m_hardwareAddress;
    
    

};

}

#endif /* NETWORKMANAGER_H_ */
