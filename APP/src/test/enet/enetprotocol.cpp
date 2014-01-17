
#include <QDebug>

#include "enetprotocol.h"

#include "HHSharedNetwork/hpackethandlerbase.h"

namespace HEHUI {


ENETProtocolTest::ENETProtocolTest(QObject *parent) :
    ENETProtocolBase(parent)
{


    //注册自定义类型，必须重载“<<”和“>>”
    //qRegisterMetaTypeStreamOperators<HEHUI::Packet>("HEHUI::Packet");
    Packet::registerMetaTypeStreamOperators();

}




void ENETProtocolTest::processReceivedData(quint32 peerID, const QByteArray &data){
    //qDebug()<<"--ENETProtocolTest::processReceivedData(...) "<<"peerID:"<<peerID;

    convertDataToPacket(peerID, data);

}

inline void ENETProtocolTest::convertDataToPacket(quint32 peerID, const QByteArray &data){

    QString ip = "";
    quint16 port = 0;
//    getPeerAddressInfo(peerID, &ip, &port);

    emit dataReceived(ip, port, data);

}














} //namespace HEHUI
