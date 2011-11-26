/*
 ****************************************************************************
 * clientpacketsparser.h
 *
 * Created On: 2010-7-13
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
 * Last Modified On: 2010-7-13
 * Last Modified By: 贺辉
 ***************************************************************************
 */

#ifndef CLIENTPACKETSPARSER_H_
#define CLIENTPACKETSPARSER_H_



#include <QHostAddress>
#include <QHostInfo>
#include <QDebug>
#include <QFile>




#include "HHSharedCore/hcryptography.h"
#include "HHSharedNetwork/hpacketparserbase.h"
#include "HHSharedNetwork/hnetworkutilities.h"



namespace HEHUI {


class ClientPacketsParser : public PacketsParserBase{
    Q_OBJECT
public:
    ClientPacketsParser(NetworkManagerBase *networkManager, QObject *parent = 0);
    virtual ~ClientPacketsParser();


    void parseIncomingPacketData(Packet *packet);






public slots:

    void sendTestData(const QHostAddress peerListeningAddress, quint16 peerListeningPort, const QHostAddress localListeningAddress, quint16 localListeningPort, QByteArray *data){

        Packet *packet = m_packetHandlerBase->getPacket(peerListeningAddress, peerListeningPort, localListeningAddress, localListeningPort);

        packet->setPacketType(quint8(0));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_localComputerName << *data;
        packet->setPacketData(ba);
        m_packetHandlerBase->appendOutgoingPacket(packet);

    }


signals:
    void dataReceived(const QString &peerAddress, quint16 peerPort, const QByteArray &data);



private:


private:


    PacketHandlerBase *m_packetHandlerBase;


    QMutex mutex;

    QString m_localComputerName;




};

}

#endif /* CLIENTPACKETSPARSER_H_ */
