
#include <QDebug>

#include "udtsocket.h"

#include "HHSharedNetwork/hpackethandlerbase.h"

namespace HEHUI {


UDTProtocolTest::UDTProtocolTest(bool stream, const SocketOptions *options, QObject *parent) :
    UDTProtocolBase(stream, options, parent)
{


    //注册自定义类型，必须重载“<<”和“>>”, 见"packetstreamoperator.h"
    //qRegisterMetaTypeStreamOperators<HEHUI::Packet>("HEHUI::Packet");
    Packet::registerMetaTypeStreamOperators();

}

bool UDTProtocolTest::sendData(UDTSOCKET socket, const QByteArray *byteArray){

    if(isStreamMode()){
        return sendUDTStreamData(socket, byteArray);
    }else{
        return sendUDTMessageData(socket, byteArray);
    }

}

void UDTProtocolTest::streamDataReceived(UDTSOCKET socket, QByteArray *data){
    //qDebug()<<"--UDTProtocolTest::streamDataReceived(...) "<<"socket:"<<socket;

    convertDataToPacket(socket, data);

}

void UDTProtocolTest::messageDataReceived(UDTSOCKET socket, QByteArray *data){
    //qDebug()<<"--UDTProtocolTest::messageDataReceived(...) "<<"socket:"<<socket;

    convertDataToPacket(socket, data);

}

void UDTProtocolTest::convertDataToPacket(UDTSOCKET socket, QByteArray *data){

    QString ip = "";
    quint16 port = 0;
    getAddressInfoFromSocket(socket, &ip, &port);

    emit dataReceived(ip, port, *data);



}














} //namespace HEHUI
