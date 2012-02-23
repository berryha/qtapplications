#include "tcp.h"

#include "HHSharedNetwork/hpackethandlerbase.h"


namespace HEHUI {


TCP::TCP(QObject *parent) :
    TCPBase(parent)
{

}

void TCP::processData(int socketID, QByteArray *data){

    QHostAddress address;
    quint16 port;
    socketPeerAddressInfo(socketID, &address, &port);


    QDataStream in(data, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_7);
    QVariant v;
    in >> v;
    if (v.canConvert<Packet>()){
        Packet *packet = PacketHandlerBase::getPacket(socketID);
        *packet = v.value<Packet>();
        packet->setTransmissionProtocol(TP_UDT);
        //packet->setSocketID(socketID);


        packet->setPeerHostAddress(address);
        packet->setPeerHostPort(port);
//        packet->setLocalHostAddress(m_udpSocket->localAddress());
//        packet->setLocalHostPort(m_udpSocket->localPort());

//        m_packetHandlerBase->appendIncomingPacket(packet);

        emit packetReceived(packet);
    }


}













}
