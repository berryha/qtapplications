/*
 ****************************************************************************
 * bulletinboardpacketsparser.cpp.h
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

#ifndef BULLETINBOARDPACKETSPARSER_H_
#define BULLETINBOARDPACKETSPARSER_H_


#include <QDataStream>
#include <QHostAddress>
#include <QDebug>

#include "../../sharedms/global_shared.h"
#include "resourcesmanagerinstance.h"


#include "HHSharedCore/hcryptography.h"
//#include "HHSharedNetwork/hpacketparserbase.h"
#include "HHSharedCore/hutilities.h"
#include "HHSharedNetwork/hpackethandlerbase.h"

namespace HEHUI {


class BulletinBoardPacketsParser : public QObject{
    Q_OBJECT

public:
    BulletinBoardPacketsParser(ResourcesManagerInstance *resourcesManager, QObject *parent = 0);
    virtual ~BulletinBoardPacketsParser();


public slots:
    void parseIncomingPacketData(Packet *packet);


    bool sendLocalUserOnlinePacket(int socketID){
        qDebug()<<"--sendLocalUserOnlinePacket(...)";

        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::LocalUserOnline));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_localID << m_userName << m_localComputerName;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        //return m_udtProtocol->sendUDTMessageData(socketID, &ba);
        return m_rtp->sendReliableData(socketID, &ba);
    }

//    void sendUserOfflinePacket(){
//        qDebug()<<"----sendUserOfflinePacket(...)";

//        Packet *packet = m_packetHandlerBase->getPacket(QHostAddress::LocalHost, localServerRUDPListeningPort, localRUDPListeningAddress, localRUDPListeningPort);

//        packet->setPacketType(quint8(MS::UserOffline));
//        packet->setTransmissionProtocol(TP_UDT);
//        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
//        QByteArray ba;
//        QDataStream out(&ba, QIODevice::WriteOnly);
//        out.setVersion(QDataStream::Qt_4_7);
//        out << m_localID << m_userName << m_localComputerName;
//        packet->setPacketData(ba);
//        m_packetHandlerBase->appendOutgoingPacket(packet);

//    }

    bool sendUserResponseRemoteAssistancePacket(int adminSocketID, bool accept){

        Packet *packet = m_packetHandlerBase->getPacket(adminSocketID);
        
        packet->setPacketType(quint8(MS::UserResponseRemoteAssistance));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_localID << m_userName << m_localComputerName << accept;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(adminSocketID, &ba);
    }

    bool sendNewPasswordRetrevedByUserPacket(int adminSocketID){

        Packet *packet = m_packetHandlerBase->getPacket(adminSocketID);
        
        packet->setPacketType(quint8(MS::NewPasswordRetrevedByUser));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_localID << m_userName << m_localComputerName;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(adminSocketID, &ba);
    }

    
    

    /////////////////////////////////////////////////////




signals:
    void  signalHeartbeatPacketReceived(const QString &computerName);
    void  signalConfirmationOfReceiptPacketReceived(quint16 packetSerialNumber1, quint16 packetSerialNumber2);

    void signalLocalServiceServerDeclarePacketReceived(const QString &localComputerName);

    void signalAdminRequestRemoteAssistancePacketReceived(const QString &adminAddress, quint16 adminPort, const QString &adminName );
    void signalAdminInformUserNewPasswordPacketReceived(const QString &adminAddress, quint16 adminPort, const QString &adminName, const QString &oldPassword, const QString &newPassword);
    void signalAnnouncementPacketReceived(const QString &adminName, quint32 announcementID, const QString &announcement);


private:


    ResourcesManagerInstance *m_resourcesManager;
    PacketHandlerBase *m_packetHandlerBase;
    //NetworkManagerInstance *networkManager;


    QString m_userName;
    QString m_localComputerName;
    QString m_localID;

    RTP *m_rtp;
    UDTProtocol *m_udtProtocol;
    TCPServer *m_tcpServer;



};

}

#endif /* BULLETINBOARDPACKETSPARSER_H_ */
