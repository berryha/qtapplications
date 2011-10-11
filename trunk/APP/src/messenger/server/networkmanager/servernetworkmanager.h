/*
 ****************************************************************************
 * servernetworkmanager.h
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





#ifndef SERVERNETWORKMANAGER_H_
#define SERVERNETWORKMANAGER_H_


#include <QObject>


#include "../../sharedim/networkmanager/networkmanager.h"



namespace HEHUI {


class ServerNetworkManager :public NetworkManager{
    Q_OBJECT
public:
    static ServerNetworkManager * instance();
    static void cleanInstance();

    virtual ~ServerNetworkManager();


private:
    ServerNetworkManager(PacketHandlerBase *packetHandlerBase = 0, NetworkType type = LAN, CommunicationMode mode = P2P, QObject *parent = 0);



private slots:


private:
    static ServerNetworkManager *serverNetworkManager;


};

} //namespace HEHUI

#endif /* SERVERNETWORKMANAGER_H_ */
