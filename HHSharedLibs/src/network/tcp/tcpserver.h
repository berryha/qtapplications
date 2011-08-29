/*
 ****************************************************************************
 * tcpserver.h
 *
 * Created on: 2009-11-6
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
 * Last Modified on: 2010-05-15
 * Last Modified by: 贺辉
 ***************************************************************************
 */



#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QStringList>
#include <QTcpServer>

#include "tcpsocketconnection.h"

namespace HEHUI {

class TcpServer : public QTcpServer
{
    Q_OBJECT

public:
    TcpServer(const QHostAddress & address = QHostAddress::Any, quint16 port = 0 , QObject *parent = 0);

signals:
        //void signalNewIncomingTCPConnection(TcpSocketConnection *connection);
    void signalNewIncomingTCPConnection(int socketDescriptor);

protected:
    void incomingConnection(int socketDescriptor);

private:





};

} //namespace HEHUI

#endif
