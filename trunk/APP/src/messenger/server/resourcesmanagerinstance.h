/*
 * networkmanagerinstance.h
 *
 *  Created on: 2011-7-23
 *      Author: 贺辉
 */

#ifndef RESOURCEMANAGERINSTANCE_H_
#define RESOURCEMANAGERINSTANCE_H_

#include <QObject>

#include "../sharedim/resourcesmanager.h"

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

#endif /* NETWORKMANAGERINSTANCE_H_ */