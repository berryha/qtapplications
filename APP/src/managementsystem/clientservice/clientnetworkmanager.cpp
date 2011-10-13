/*
 * clientnetworkmanager.cpp
 *
 *  Created on: 2009-9-14
 *      Author: 贺辉
 */

#include <QDebug>
#include <QHostInfo>

#include <QNetworkInterface>
#include <QHostAddress>

#include "clientnetworkmanager.h"


#include "HHSharedCore/huser.h"
#include "HHSharedCore/hutilities.h"


namespace HEHUI {


ClientNetworkManager::ClientNetworkManager(PacketHandlerBase *packetHandlerBase, NetworkType type, CommunicationMode mode, QObject *parent)
    :NetworkManager(packetHandlerBase, type, mode, parent)
{

    setParent(parent);

}

ClientNetworkManager::~ClientNetworkManager() {
    // TODO Auto-generated destructor stub
}





















} //namespace HEHUI