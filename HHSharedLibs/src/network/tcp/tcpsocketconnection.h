/*
 ****************************************************************************
 * tcpsocketconnection.h
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




#ifndef TCPSOCKETCONNECTION_H
#define TCPSOCKETCONNECTION_H

#include <QHostAddress>
#include <QString>
#include <QTcpSocket>
#include <QTime>
#include <QTimer>

#include "../global_network.h"

#include "../packethandler/packethandlerbase.h"



namespace HEHUI {

static const int MaxBufferSize = 1024000;

class TcpSocketConnection : public QTcpSocket
{
    Q_OBJECT

public:

    TcpSocketConnection(PacketHandlerBase *packetHandlerBase, QObject *parent = 0);
    ~TcpSocketConnection();

    bool sendTCPDatagram(const QByteArray &data);

    void destory();

signals:
    //    void signalNewTCPPacketReceived(Packet *packet);
    //    void signalTCPDatagramsReceived(const QByteArray &data);
    void signlaCanBeDestroyed();

public slots:
    void abortConnection();


private slots:
    void slotProcessReadyRead();


private:

    PacketHandlerBase *m_packetHandlerBase;

    QByteArray buffer;

    quint16 nextBlockSize;



};

} //namespace HEHUI

#endif
