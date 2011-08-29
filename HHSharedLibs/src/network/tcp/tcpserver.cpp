/*
 ****************************************************************************
 * tcpserver.cpp
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



#include "tcpserver.h"
#include "tcpsocketthread.h"

namespace HEHUI {

TcpServer::TcpServer(const QHostAddress & address, quint16 port, QObject *parent)
    : QTcpServer(parent)
{

//        if(port > 0 && port < 65535){
		listen(address, port);
//	}

}



void TcpServer::incomingConnection(int socketDescriptor)
{
    qDebug("----TcpServer::incomingConnection(int socketDescriptor)");
    emit signalNewIncomingTCPConnection(socketDescriptor);

//    TcpSocketThread *thread = new TcpSocketThread(socketDescriptor, this);
//    connect(thread, SIGNAL(signalNewTCPConnection(TcpSocketConnection *)), this, SIGNAL(signalNewIncomingTCPConnection(TcpSocketConnection *)));
//    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
//    thread->start();
}










} //namespace HEHUI

