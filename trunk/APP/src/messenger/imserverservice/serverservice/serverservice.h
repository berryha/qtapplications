/*
 ****************************************************************************
 * serverservice.h
 *
 * Created on: 2010-7-12
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
 * Last Modified on: 2010-9-3
 * Last Modified by: 贺辉
 ***************************************************************************
 */



#ifndef SERVERSERVICE_H
#define SERVERSERVICE_H

#include "../../server/server.h"

#include "HHSharedService/hservice.h"



namespace HEHUI {


class ServerService : public Service
{
    Q_OBJECT
public:
    ServerService(int argc, char **argv, const QString &serviceName = "ServerService", const QString &description = "Server service for management system.");
    ~ServerService();



signals:


private slots:
    bool startMainService();


private:

protected:
    void start();
    void stop();
    void pause();
    void resume();
    void processCommand(int code);



private:


    bool mainServiceStarted;
    
    Server *m_server;



};

} //namespace HEHUI

#endif // SERVERSERVICE_H
