

//#include <QCoreApplication>

//#include <QThreadPool>
#include <QDebug>


#include "rudpsocket.h"

#include "rudppacket.h"
#include "rudppacketstreamoperator.h"





namespace HEHUI{


RUDPSocket::RUDPSocket(QObject *parent) :
    QUdpSocket(parent)
{

    peers.clear();

    datagram = new QByteArray();
    datagram->resize(0);

    connect(this, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));

    //注册自定义类型，必须重载“<<”和“>>”, 见"rudppacketstreamoperator.h"
    qRegisterMetaTypeStreamOperators<HEHUI::RUDPPacket>("HEHUI::RUDPPacket");

}

RUDPSocket::~RUDPSocket(){

    foreach (RUDPChannel *channel, peers.values()) {
        if(!channel){continue;}
        channel->closeChannel();
        recyleRUDPChannel(channel);
    }
    close();
    peers.clear();

    foreach (RUDPChannel *channel, m_unusedRUDPChannels) {
        channel->quit();
        delete channel;
        channel = 0;
    }
    m_unusedRUDPChannels.clear();

    datagram->clear();
    datagram->resize(0);
    delete datagram;
    datagram = 0;


}

void RUDPSocket::connectToPeer(const QString &peerAddressString, quint16 peerPort, int msecTimeout){

    connectToPeer(QHostAddress(peerAddressString), peerPort, msecTimeout);
}

void RUDPSocket::connectToPeer(const QHostAddress &peerAddress, quint16 peerPort, int msecTimeout){

    if( 0 == localPort()){
        bind();
    }

    RUDPChannel *channel = getRUDPChannel(peerAddress, peerPort);
    channel->connectToPeer(msecTimeout);

}

void RUDPSocket::disconnectFromPeer(const QHostAddress &peerAddress, quint16 peerPort){

    RUDPChannel *channel = getRUDPChannel(peerAddress, peerPort);
    channel->disconnectFromPeer();
}

//bool RUDPSocket::canSendData(quint64 size){
//    return RUDPChannel::canSendData(size);
//}

quint16 RUDPSocket::beginDataTransmission(const QHostAddress &peerAddress, quint16 peerPort){
    RUDPChannel *channel = getRUDPChannel(peerAddress, peerPort);
    return channel->beginDataTransmission();
}

bool RUDPSocket::sendData(const QHostAddress &peerAddress, quint16 peerPort, QByteArray &data){

    RUDPChannel *channel = getRUDPChannel(peerAddress, peerPort);
    return channel->sendData(data);

}

//quint64 RUDPSocket::sendData(const QHostAddress &peerAddress, quint16 peerPort, QByteArray *data, quint64 offset){

//    RUDPChannel *channel = getRUDPChannel(peerAddress, peerPort);
//    return channel->sendDatagram(data);

//}

quint64 RUDPSocket::sendDatagram(const QHostAddress &peerAddress, quint16 peerPort, QByteArray *data, quint64 offset, bool fragment){

    RUDPChannel *channel = getRUDPChannel(peerAddress, peerPort);
    return channel->sendDatagram(data, offset, fragment);

}

//quint64 RUDPSocket::sendDatagram(const QHostAddress &peerAddress, quint16 peerPort, QByteArray *data){

//    RUDPChannel *channel = getRUDPChannel(peerAddress, peerPort);
//    return channel->sendDatagram(data);
//}

void RUDPSocket::endDataTransmission(const QHostAddress &peerAddress, quint16 peerPort, quint16 fragmentDataID){
    RUDPChannel *channel = getRUDPChannel(peerAddress, peerPort);
    return channel->endDataTransmission(fragmentDataID);
}

void RUDPSocket::readPendingDatagrams() {
    //qDebug()<<"----RUDPSocket::readPendingDatagrams()";

        while (hasPendingDatagrams()) {
                datagram->clear();
                qint64 datagramSize = pendingDatagramSize();
                datagram->resize(datagramSize);
                QHostAddress peerAddress;
                quint16 peerPort;

                qint64 readSize = readDatagram(datagram->data(), datagramSize, &peerAddress, &peerPort);
                if(readSize == -1){
                        qWarning()<<"Can not read datagram!";
                        break;
                }

                //qWarning()<<"Datagram Received From " <<peerAddress.toString()<<" Port:"<<peerPort<<" Size:"<<datagramSize;

                //qDebug()<<"~~datagramSize:"<<datagramSize;

//                QDataStream in(datagram, QIODevice::ReadOnly);
//                in.setVersion(QDataStream::Qt_4_6);
//                QVariant v;
//                in >> v;
//                if (v.canConvert<Packet>()){
//                    //Packet *packet = new Packet();
//                    RUDPSocket *packet = packetHandlerBase->getPacket();
//                    *packet = v.value<RUDPSocket>();
//                    packet->setTransmissionProtocol(TP_UDP);
//                    packet->setPeerHostAddress(peerAddress);
//                    packet->setPeerHostPort(peerPort);
//                    packet->setLocalHostAddress(localAddress());
//                    packet->setLocalHostPort(localPort());

//                }


                RUDPChannel *channel = getRUDPChannel(peerAddress, peerPort);
                channel->datagramReceived(*datagram);

        }


        datagram->clear();
        datagram->resize(0);

}

void RUDPSocket::channelclosed(){
    qDebug()<<"--RUDPSocket::channelclosed()";

    RUDPChannel *channel = qobject_cast<RUDPChannel *>(sender());
    if(channel){
        QHostAddress address = channel->getPeerHostAddress();
        quint16 port = channel->getPeerHostPort();
        QString channelID = address.toString() + ":" + QString::number(port);
        peers.remove(channelID);
        emit peerDisconnected(address, port);

        recyleRUDPChannel(channel);
    }

}

RUDPChannel * RUDPSocket::getRUDPChannel(const QHostAddress &hostAddress, quint16 port){

    QString channelID = hostAddress.toString() + ":" + QString::number(port);

    RUDPChannel *channel = 0;

    if(!peers.contains(channelID)){
        if(m_unusedRUDPChannels.isEmpty()){
            qWarning()<<"Create new channel:"<<channelID;
            channel = new RUDPChannel(this, hostAddress, port, this);
            connect(channel, SIGNAL(finished()), this, SLOT(channelclosed()));
            connect(channel, SIGNAL(terminated()), this, SLOT(channelclosed()));
            connect(channel, SIGNAL(peerConnected(const QHostAddress &, quint16)), this, SIGNAL(peerConnected(const QHostAddress &, quint16)));
            connect(channel, SIGNAL(signalConnectToPeerTimeout(const QHostAddress &, quint16)), this, SIGNAL(signalConnectToPeerTimeout(const QHostAddress &, quint16)));
            connect(channel, SIGNAL(peerDisconnected(const QHostAddress &, quint16)), this, SIGNAL(peerDisconnected(const QHostAddress &, quint16)));

            connect(channel, SIGNAL(dataReceived(const QHostAddress &, quint16, const QByteArray &)), this, SIGNAL(dataReceived(const QHostAddress &, quint16, const QByteArray &)));

            channel->start();
            peers.insert(channelID, channel);
        }else{
            qWarning()<<"Use idle channel:"<<channelID;
            channel = m_unusedRUDPChannels.takeFirst();
        }

    }else{
//        QList<QHostAddress> addresses = peers.keys();
//        foreach (QHostAddress address, addresses) {
//            RUDPChannel *c = peers.value(address);
//            if(c->getPeerHostPort() == port){
//                channel = c;
//                break;
//            }
//        }

        channel = peers.value(channelID);

    }

    return channel;

}

inline void RUDPSocket::recyleRUDPChannel(RUDPChannel *channel){

    m_unusedRUDPChannels.append(channel);

}


























} //namespace HEHUI
