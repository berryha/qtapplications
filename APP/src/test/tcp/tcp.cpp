#include "tcp.h"




namespace HEHUI {


TCP::TCP(QObject *parent) :
    TCPBase(parent)
{

}

void TCP::processData(int socketID, const QByteArray &data){

    QHostAddress address;
    quint16 port;
    socketPeerAddressInfo(socketID, &address, &port);

    emit dataReceived(address.toString(), port, data);


}













}
