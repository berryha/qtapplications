/*
 * networkmanagerinstance.h
 *
 *  Created on: 2011-7-23
 *      Author: 贺辉
 */

#ifndef CONTROLCENTERRESOURCEMANAGERINSTANCE_H_
#define CONTROLCENTERRESOURCEMANAGERINSTANCE_H_

#include <QObject>

#include "../../../sharedms/resourcesmanager.h"

#include "HHSharedCore/hsingleton.h"

namespace HEHUI {


class ResourcesManagerInstance: public ResourcesManager, public Singleton<ResourcesManagerInstance>
{
    Q_OBJECT

    friend class Singleton<ResourcesManagerInstance> ;

public:

    ResourcesManagerInstance(QObject *parent = 0);
    virtual ~ResourcesManagerInstance();



signals:



public slots:



private slots:


private:



};

} //namespace HEHUI

#endif /* CONTROLCENTERRESOURCEMANAGERINSTANCE_H_ */
