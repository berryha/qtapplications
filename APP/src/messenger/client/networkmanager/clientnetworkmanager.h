/*
 * clientnetworkmanager.h
 *
 *  Created on: 2009-9-14
 *      Author: 贺辉
 */

#ifndef CLIENTNETWORKMANAGER_H_
#define CLIENTNETWORKMANAGER_H_

#include <QObject>

#include "../../sharedim/resourcesmanager.h"


#include "HHSharedCore/huser.h"
#include "HHSharedCore/hsingleton.h"

namespace HEHUI {


class ClientResourcesManager: public ResourcesManager, public Singleton<ClientResourcesManager>
{
    Q_OBJECT

    friend class Singleton<ClientResourcesManager> ;

public:

    ClientResourcesManager(NetworkType type = LAN, CommunicationMode mode = P2P, QObject *parent = 0);
    virtual ~ClientResourcesManager();



signals:



public slots:



private slots:

private:



};

} //namespace HEHUI

#endif /* CLIENTNETWORKMANAGER_H_ */
