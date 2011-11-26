/*
 * clientnetworkmanager.h
 *
 *  Created on: 2009-9-14
 *      Author: 贺辉
 */

#ifndef CLIENTNETWORKMANAGER_H_
#define CLIENTNETWORKMANAGER_H_

#include <QObject>

#include "../../sharedim/networkmanager/networkmanager.h"
//#include "../../shared/impacket.h"

//#include "../../../shared/core/user.h"
//#include "../../../shared/core/singleton.h"
#include "HHSharedCore/huser.h"
#include "HHSharedCore/hsingleton.h"

namespace HEHUI {


class ClientResourcesManager: public NetworkManager, public Singleton<ClientResourcesManager>
{
    Q_OBJECT

    friend class Singleton<ClientResourcesManager> ;

public:

    ClientResourcesManager(PacketHandlerBase *packetHandlerBase = 0, NetworkType type = LAN, CommunicationMode mode = P2P, QObject *parent = 0);
    virtual ~ClientResourcesManager();



signals:



public slots:



private slots:

private:



};

} //namespace HEHUI

#endif /* CLIENTNETWORKMANAGER_H_ */
