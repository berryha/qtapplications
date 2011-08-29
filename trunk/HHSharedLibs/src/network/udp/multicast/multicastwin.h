/*
 ****************************************************************************
 * multicastwin.h
 *
 * Created on: 2009-9-3
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





#ifndef RECEIVER_H
#define RECEIVER_H

 #include <QtCore>
 #include <QObject>
 #include <QUdpSocket>

//#include "../shared/global"
#include "ipmulticastsocketbase.h"

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>

//#include "CBase.h"
//#define MCASTADDR "224.0.0.1" //本例使用的多播组地址。
//#define MCASTPORT 3838
//#define PORT 7838 //绑定的本地端口号。
#define BUFSIZE 5000 //接收数据缓冲大小。

//class QUdpSocket;

class IPMultiCastWin : public IPMulticastSocketBase{
	Q_OBJECT;
public:

        IPMultiCastWin(QObject *parent);
        virtual ~IPMultiCastWin();


//signals:
//	void signalUDPDatagramsReceived(const QByteArray &data);

public slots:
        bool startIPMulticastListening(const QHostAddress &ipMulticastGroupAddress, quint16 ipMulticastGroupPort);
	void leaveGroup();
        bool slotSendUDPDatagramViaBoundSocket(const QString &ip, quint16 port, const QByteArray &data);

private slots:
//	void readPendingDatagrams();


private:
        //QUdpSocket * udpSocket;
	long count,Receivedpackets;
	bool bFlag;
	WSADATA wsd;
	struct sockaddr_in local,remote,from;
	SOCKET sock,sockM;
	char recvbuf[BUFSIZE];
};

#endif // RECEIVER_H
