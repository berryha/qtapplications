/*
 ****************************************************************************
 * clientpacketsparser.cpp
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


#include <QCoreApplication>

#include "clientpacketsparser.h"

#ifdef Q_CC_MSVC
#include <windows.h>
//#include "HHSharedWindowsManagement/hwindowsmanagement.h"
#define msleep(x) Sleep(x)
#endif

#ifdef Q_CC_GNU
#include <unistd.h>
#define msleep(x) usleep(x*1000)
#endif

//#include "HHSharedCore/hutilities.h"


namespace HEHUI {


ClientPacketsParser::ClientPacketsParser(NetworkManagerBase *networkManager, QObject *parent)
    :PacketsParserBase(networkManager, parent)
{

    Q_ASSERT_X(networkManager, "ClientPacketsParser::ClientPacketsParser(...)", "Invalid ClientNetworkManager!");

    m_packetHandlerBase = networkManager->getPacketHandler();
    Q_ASSERT_X(m_packetHandlerBase, "ClientPacketsParser::ClientPacketsParser(...)", "Invalid PacketHandlerBase!");


    m_localComputerName = QHostInfo::localHostName().toLower();



}

ClientPacketsParser::~ClientPacketsParser() {
    // TODO Auto-generated destructor stub

    qDebug()<<"ClientPacketsParser::~ClientPacketsParser() ";


}

void ClientPacketsParser::run(){

    QMutexLocker locker(&mutex);

//    QTimer processWaitingForReplyPacketsTimer;
//    processWaitingForReplyPacketsTimer.setSingleShot(false);
//    processWaitingForReplyPacketsTimer.setInterval(UDP_PACKET_WAITING_FOR_REPLY_TIMEOUT);
//    connect(&processWaitingForReplyPacketsTimer, SIGNAL(timeout()), this, SLOT(processWaitingForReplyPackets()));
//    connect(this, SIGNAL(signalAboutToQuit()), &processWaitingForReplyPacketsTimer, SLOT(stop()));
//    processWaitingForReplyPacketsTimer.start();

    while(1){

        QCoreApplication::processEvents();
        parseIncomingPackets();
        processOutgoingPackets();

        if(isAboutToQuit()){
            break;
        }
        msleep(500);
    }


//    processWaitingForReplyPacketsTimer.stop();

    processOutgoingPackets();


}

void ClientPacketsParser::startparseIncomingPackets(){

    while(!isAboutToQuit()){
        QCoreApplication::processEvents();
        parseIncomingPackets();
        msleep(500);
    }

}

void ClientPacketsParser::startprocessOutgoingPackets(){

    QTimer processWaitingForReplyPacketsTimer;
    processWaitingForReplyPacketsTimer.setSingleShot(false);
    processWaitingForReplyPacketsTimer.setInterval(UDP_PACKET_WAITING_FOR_REPLY_TIMEOUT/2);
    connect(&processWaitingForReplyPacketsTimer, SIGNAL(timeout()), this, SLOT(processWaitingForReplyPackets()));
    connect(this, SIGNAL(signalAboutToQuit()), &processWaitingForReplyPacketsTimer, SLOT(stop()));
    processWaitingForReplyPacketsTimer.start();

    while(!isAboutToQuit()){
        //QCoreApplication::processEvents();
        processOutgoingPackets();
        msleep(500);
    }

    processWaitingForReplyPacketsTimer.stop();

    processOutgoingPackets();

}

void ClientPacketsParser::parseIncomingPacketData(Packet *packet){
    qDebug()<<"--ClientPacketsParser::parseIncomingPacketData(Packet *packet)";

    QByteArray packetData = packet->getPacketData();
    QDataStream in(&packetData, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_7);

    QString peerName = "";
    QByteArray data;
    in >> peerName >> data;

    emit dataReceived(packet->getPeerHostAddress(), packet->getPeerHostPort(), data);


}





















} //namespace HEHUI
