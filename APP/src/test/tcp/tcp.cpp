#include "tcp.h"


#include <QThread>

namespace HEHUI {


TCP::TCP(QObject *parent) :
    TCPBase(parent)
{

}

void TCP::processData(int socketID, const QByteArray &data){

    QHostAddress address;
    quint16 port;
    socketPeerAddressInfo(socketID, &address, &port);

    qDebug()<<"-------------1-------Thread ID:"<<QThread::currentThreadId ();
    emit dataReceived(address.toString(), port, data);



}













}
