/*
 * clientnetworkmanager.h
 *
 *  Created on: 2009-9-14
 *      Author: 贺辉
 */

#ifndef CLIENTNETWORKMANAGER_H_
#define CLIENTNETWORKMANAGER_H_

#include <QObject>

#include "../sharedms/networkmanager.h"


#include "HHSharedCore/huser.h"
#include "HHSharedCore/hsingleton.h"

namespace HEHUI {


class ClientNetworkManager: public NetworkManager, public Singleton<ClientNetworkManager>
{
    Q_OBJECT

    friend class Singleton<ClientNetworkManager> ;

public:

    ClientNetworkManager(PacketHandlerBase *packetHandlerBase = 0, NetworkType type = LAN, CommunicationMode mode = P2P, QObject *parent = 0);
    virtual ~ClientNetworkManager();



private:

signals:



public slots:




private slots:

private:



};

} //namespace HEHUI

#endif /* CLIENTNETWORKMANAGER_H_ */
