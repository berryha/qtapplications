#include "udtsocket.h"





namespace HEHUI {


UDTSocket::UDTSocket(QObject *parent) :
    UDTProtocolBase()
{


    m_packetHandlerBase = new PacketHandlerBase(this);
    networkManager = new NetworkManagerBase(m_packetHandlerBase);
    clientPacketsParser = new ClientPacketsParser(networkManager, this);

    connect(clientPacketsParser, SIGNAL(dataReceived(const QString &, quint16, const QByteArray &)), this, SIGNAL(dataReceived(const QString &, quint16, const QByteArray &)));


    QtConcurrent::run(clientPacketsParser, &ClientPacketsParser::run);

}

UDTSocket::~UDTSocket(){

    if(clientPacketsParser){
        clientPacketsParser->aboutToQuit();
    }

    delete clientPacketsParser;
    clientPacketsParser = 0;

    delete networkManager;
    networkManager = 0;

    m_packetHandlerBase->clean();
    delete m_packetHandlerBase;
    m_packetHandlerBase = 0;

}

void UDTSocket::streamDataReceived(int udtSocketID, const QByteArray &data){
    QString ip = "";
    quint16 port = 0;
    getAddressInfoFromSocket(udtSocketID, &ip, &port);

    emit dataReceived(ip, port, data);

    //        QDataStream in(&data, QIODevice::ReadOnly);
    //        in.setVersion(QDataStream::Qt_4_7);
    ////    QVariant v;
    ////    in >> v;
    ////    if (v.canConvert<Packet>()){
    ////        Packet *packet = m_packetHandlerBase->getPacket();
    ////        *packet = v.value<Packet>();
    ////        packet->setTransmissionProtocol(TP_RUDP);
    ////        packet->setPeerHostAddress(m_peerAddress);
    ////        packet->setPeerHostPort(m_peerPort);
    ////        packet->setLocalHostAddress(m_udpSocket->localAddress());
    ////        packet->setLocalHostPort(m_udpSocket->localPort());

    ////        m_packetHandlerBase->appendIncomingPacket(packet);
    ////    }



}

void UDTSocket::messageDataReceived(int udtSocketID, const QByteArray &data){

    QString ip = "";
    quint16 port = 0;
    getAddressInfoFromSocket(udtSocketID, &ip, &port);

    emit dataReceived(ip, port, data);

}

//void UDTSocket::dataReceived(const QString &address, quint16 port, char *data){

//    QByteArray byteArray(data);

//    if(isStreamMode()){

//        QString key = address+":"+QString::number(port);
//        CachedDataInfo *info = m_cachedDataInfo.value(key);
//        int blockSize = info?info->blockSize:0;
//        QByteArray *cachedData = 0;

//        if(0 != blockSize){
//            cachedData = info->data;
//            Q_ASSERT(cachedData);

//            byteArray.prepend(cachedData);
//            cachedData->clear();

//            if(-1 == blockSize){
//                blockSize = 0;
//            }
//        }

//        QDataStream in(&byteArray, QIODevice::ReadOnly);
//        in.setVersion(QDataStream::Qt_4_7);
//        QIODevice *dev = in.device();
//        //dev->seek(offset);

//        QByteArray temp;
//        qint64 readSize = 0;
//        forever{

//            if(0 == blockSize){
//                blockSize = sizeof(int);
//                temp.clear();
//                temp.resize(blockSize);
//                readSize = dev->read(temp.data(), blockSize);
//                if(readSize == blockSize){
//                    blockSize = temp.toInt();
//                    //continue;
//                }else{
//                    info->blockSize = -1;
//                    info->data = temp;
//                    break;
//                }
//            }


//            temp.clear();
//            temp.resize(blockSize);
//            readSize = dev->read(temp.data(), blockSize);
//            if(readSize == blockSize){
//                emit dataReceived(address, port, temp);
//                blockSize = 0;
//                continue;
//            }else{
//                info->blockSize = blockSize;
//                info->data = temp;
//                break;
//            }

//        }

//    }else{
//        emit dataReceived(address, port, byteArray);
//    }




////    QVariant v;
////    in >> v;
////    if (v.canConvert<Packet>()){
////        Packet *packet = m_packetHandlerBase->getPacket();
////        *packet = v.value<Packet>();
////        packet->setTransmissionProtocol(TP_RUDP);
////        packet->setPeerHostAddress(m_peerAddress);
////        packet->setPeerHostPort(m_peerPort);
////        packet->setLocalHostAddress(m_udpSocket->localAddress());
////        packet->setLocalHostPort(m_udpSocket->localPort());

////        m_packetHandlerBase->appendIncomingPacket(packet);
////    }




//}














} //namespace HEHUI
