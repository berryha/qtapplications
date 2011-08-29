/*
 ****************************************************************************
 * tcpsocketthread.cpp
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

#include <QtNetwork>

#include "tcpsocketthread.h"
#include "tcpsocketconnection.h"

namespace HEHUI {

TcpSocketThread::TcpSocketThread(int socketDescriptor, PacketHandlerBase *packetHandlerBase, QObject *parent)
    :QThread(parent), socketDescriptor(socketDescriptor), m_packetHandlerBase(packetHandlerBase)
{

    Q_ASSERT_X(m_packetHandlerBase, "TcpSocketThread::TcpSocketThread(...)", "Invalid PacketHandlerBase!");

    connection = 0;
    address = QHostAddress::Null;
    port = 0;

}

TcpSocketThread::TcpSocketThread(const QHostAddress & address, quint16 port, PacketHandlerBase *packetHandlerBase, QObject *parent)
    :QThread(parent), address(address), port(port), m_packetHandlerBase(packetHandlerBase)
{

    Q_ASSERT_X(m_packetHandlerBase, "TcpSocketThread::TcpSocketThread(...)", "Invalid PacketHandlerBase!");

    connection = 0;
    socketDescriptor = 0;

}

TcpSocketThread::~TcpSocketThread() {
    delete connection;
    connection = 0;
    //     connection->deleteLater();
    socketDescriptor = 0;
    address = QHostAddress::Null;
    port = 0;

}

void TcpSocketThread::run() {
    qDebug("----TcpSocketThread::run()");


    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));

    if (!connection) {
        connection = new TcpSocketConnection(m_packetHandlerBase);
    }

    connect(connection, SIGNAL(error ( QAbstractSocket::SocketError)), this, SIGNAL(error(QAbstractSocket::SocketError)));

    //TcpSocketConnection *connection = new TcpSocketConnection();
    if (port > 0 && port < 65535 && (!address.isNull())) {
        connection->connectToHost(address, port);
        if (!connection->waitForConnected(5000)) {
            emit error(QAbstractSocket::SocketTimeoutError);
            qCritical("ERROR! Can not connect to host '%s' on port %d in 5000 milliseconds!", qPrintable(address.toString()), port);
            return;
        }
    } else if (!connection->setSocketDescriptor(socketDescriptor)) {
        emit error(QAbstractSocket::UnknownSocketError);
        qCritical("ERROR! Can not initialize the socket with the native socket descriptor %d!", socketDescriptor);
        //emit error(connection->error());
        return;
    }

    connect(connection, SIGNAL(signlaCanBeDestroyed()), this, SLOT(quit()));
    connect(this, SIGNAL(terminated()), connection, SLOT(abortConnection()));

    emit signalNewTCPConnectionConnected(connection);

    //        qWarning()<<"Peer Address:"<<connection->peerAddress().toString()<<" Port:"<<connection->peerPort();

    exec();

}

} //namespace HEHUI

