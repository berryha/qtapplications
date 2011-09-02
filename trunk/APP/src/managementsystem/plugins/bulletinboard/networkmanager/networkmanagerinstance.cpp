/*
 * networkmanagerinstance.cpp
 *
 *  Created on: 2009-9-14
 *      Author: 贺辉
 */

#include <QDebug>
#include <QHostInfo>

#include <QNetworkInterface>
#include <QHostAddress>

#include "networkmanagerinstance.h"


#include "HHSharedCore/huser.h"
#include "HHSharedCore/hutilities.h"


namespace HEHUI {

NetworkManagerInstance::NetworkManagerInstance(PacketHandlerBase *packetHandlerBase, NetworkType type, CommunicationMode mode, QObject *parent)
        :NetworkManager(packetHandlerBase, type, mode, parent)//, mainWindow(mainWindow)
{

	setParent(parent);

}

NetworkManagerInstance::~NetworkManagerInstance() {
	// TODO Auto-generated destructor stub
}











} //namespace HEHUI
