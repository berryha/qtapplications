/*
 ****************************************************************************
 * tcpsocketthread.h
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



#ifndef TCPSOCKETTHREAD_H
#define TCPSOCKETTHREAD_H

#include <QThread>
#include <QTcpSocket>

#include "tcpsocketconnection.h"
#include "packethandler/packethandlerbase.h"

namespace HEHUI {

class TcpSocketThread : public QThread
{
    Q_OBJECT

public:
    TcpSocketThread(int socketDescriptor, PacketHandlerBase *packetHandlerBase, QObject *parent = 0);
    TcpSocketThread(const QHostAddress & address, quint16 port, PacketHandlerBase *packetHandlerBase, QObject *parent = 0);
    ~TcpSocketThread();

    void run();

signals:
    void error(QAbstractSocket::SocketError socketError);
    void signalNewTCPConnectionConnected(TcpSocketConnection *connection);

private:
    TcpSocketConnection *connection;
    int socketDescriptor;

    QHostAddress address;
    quint16 port;

    PacketHandlerBase *m_packetHandlerBase;



};

} //namespace HEHUI



#endif
