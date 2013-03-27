/*
 * clientnetworkmanager.h
 *
 *  Created on: 2009-9-14
 *      Author: 贺辉
 */

#ifndef CLIENTRESOURCESMANAGER_H_
#define CLIENTRESOURCESMANAGER_H_

#include <QObject>

#include "../sharedms/resourcesmanager.h"


//#include "HHSharedCore/huser.h"
#include "HHSharedCore/hsingleton.h"

namespace HEHUI {


class ClientResourcesManager: public ResourcesManager, public Singleton<ClientResourcesManager>
{
    Q_OBJECT

    friend class Singleton<ClientResourcesManager> ;

public:

    ClientResourcesManager(QObject *parent = 0);
    virtual ~ClientResourcesManager();



private:

signals:



public slots:




private slots:

private:



};

} //namespace HEHUI

#endif /* CLIENTNETWORKMANAGER_H_ */
