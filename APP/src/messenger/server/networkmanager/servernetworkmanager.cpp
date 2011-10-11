/*
 ****************************************************************************
 * servernetworkmanager.cpp
 *
 * Created On: 2009-9-14
 *     Author: 贺辉
 *    License: LGPL
 *    Comment:
 *
 *
 *    =============================  Usage  =============================
 *|
 *|
 *    ===================================================================
 *
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 ****************************************************************************
 */

/*
 ***************************************************************************
 * Last Modified On: 2010-5-25
 * Last Modified By: 贺辉
 ***************************************************************************
 */



#include <QHostInfo>

#include <QNetworkInterface>
#include <QHostAddress>
#include <QDebug>

#include "servernetworkmanager.h"

//#include "../../shared/core/user.h"
//#include "../../shared/core/utilities.h"



namespace HEHUI {


ServerNetworkManager * ServerNetworkManager::serverNetworkManager = 0;

ServerNetworkManager * ServerNetworkManager::instance(){
    if(!serverNetworkManager){
        serverNetworkManager = new ServerNetworkManager();
    }

    return serverNetworkManager;

}

void ServerNetworkManager::cleanInstance(){
    delete serverNetworkManager;
    serverNetworkManager = 0;
}

ServerNetworkManager::ServerNetworkManager(PacketHandlerBase *packetHandlerBase, NetworkType type, CommunicationMode mode, QObject *parent)
    :NetworkManager(packetHandlerBase, type, mode, parent)//, mainWindow(mainWindow)
{

    setParent(parent);

}

ServerNetworkManager::~ServerNetworkManager() {
    // TODO Auto-generated destructor stub


}










} //namespace HEHUI
