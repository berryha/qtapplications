/*
 * networkmanagerinstance.h
 *
 *  Created on: 2011-7-23
 *      Author: 贺辉
 */

#ifndef NETWORKMANAGERINSTANCE_H_
#define NETWORKMANAGERINSTANCE_H_

#include <QObject>

#include "../../../sharedms/networkmanager.h"

#include "HHSharedCore/hsingleton.h"

namespace HEHUI {

class NetworkManagerInstance: public NetworkManager, public Singleton<NetworkManagerInstance>
{
	Q_OBJECT

        friend class Singleton<NetworkManagerInstance> ;

public:

        NetworkManagerInstance(PacketHandlerBase *packetHandlerBase = 0, NetworkType type = LAN, CommunicationMode mode = P2P, QObject *parent = 0);
        virtual ~NetworkManagerInstance();



signals:



public slots:
        


private slots:


private:



};

} //namespace HEHUI

#endif /* NETWORKMANAGERINSTANCE_H_ */
