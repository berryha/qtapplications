/*
 ****************************************************************************
 * tcpsocket.h
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




#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <QHostAddress>
#include <QString>
#include <QTcpSocket>
#include <QTime>
#include <QTimer>
#include <QMutex>

#include "../global_network.h"

//#include "../packethandler/packethandlerbase.h"



namespace HEHUI {

static const int MaxBufferSize = 1024000;

class TcpSocket : public QTcpSocket
{
    Q_OBJECT

public:

    TcpSocket(QObject *parent = 0);
    ~TcpSocket();

    bool sendTCPDatagram(const QByteArray *data);


signals:
    void dataReceived(const QByteArray &data);

public slots:
    void abortConnection();


private slots:
    void slotProcessReadyRead();

    void readData();


private:

    //int m_socketID;

//    PacketHandlerBase *m_packetHandlerBase;

    QByteArray buffer;

    quint16 nextBlockSize;

    QMutex writeMutex;
    QMutex readMutex;



};

} //namespace HEHUI

#endif
