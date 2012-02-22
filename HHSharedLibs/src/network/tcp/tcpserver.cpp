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



namespace HEHUI {

TcpServer::TcpServer(QObject *parent)
    : QTcpServer(parent)
{


}

void TcpServer::incomingConnection(int socketDescriptor)
{

    TcpSocket *socket = new TcpSocket();
    if(socket->setSocketDescriptor(socketDescriptor)){
        emit newIncomingTCPConnection(socket);
    }else{
        delete socket;
    }

}










} //namespace HEHUI

