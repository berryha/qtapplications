#include "udtprotocol.h"



namespace HEHUI {


UDTProtocol::UDTProtocol(PacketHandlerBase *packetHandlerBase, QObject *parent) :
    UDTProtocolBase(parent), m_packetHandlerBase(packetHandlerBase)
{

    Q_ASSERT(m_packetHandlerBase);

}

void UDTProtocol::streamDataReceived(UDTSOCKET socket, QByteArray *data){
    qDebug()<<"--UDTProtocol::streamDataReceived(...) "<<"socket:"<<socket;

    convertDataToPacket(socket, data);

}

void UDTProtocol::messageDataReceived(UDTSOCKET socket, QByteArray *data){
    qDebug()<<"--UDTProtocol::messageDataReceived(...) "<<"socket:"<<socket;

    convertDataToPacket(socket, data);

}

void UDTProtocol::convertDataToPacket(UDTSOCKET socket, QByteArray *data){

    QString ip = "";
    quint16 port = 0;
    getAddressInfoFromSocket(socket, &ip, &port);

    QDataStream in(data, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_7);
    QVariant v;
    in >> v;
    if (v.canConvert<Packet>()){
        Packet *packet = m_packetHandlerBase->getPacket();
        *packet = v.value<Packet>();
        packet->setTransmissionProtocol(TP_RUDP);
        packet->setSocketID(socket);


//        packet->setPeerHostAddress(m_peerAddress);
//        packet->setPeerHostPort(m_peerPort);
//        packet->setLocalHostAddress(m_udpSocket->localAddress());
//        packet->setLocalHostPort(m_udpSocket->localPort());

        m_packetHandlerBase->appendIncomingPacket(packet);
    }

}














} //namespace HEHUI
