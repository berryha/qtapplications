/*
 ****************************************************************************
 * tcpsocketconnection.cpp
 *
 * Created on: 2009-11-6
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
 * Last Modified on: 2010-05-15
 * Last Modified by: 贺辉
 ***************************************************************************
 */




#include <QtNetwork>

#include "tcpsocketconnection.h"

namespace HEHUI {

TcpSocketConnection::TcpSocketConnection(PacketHandlerBase *packetHandlerBase, QObject *parent)
    :QTcpSocket(parent), m_packetHandlerBase(packetHandlerBase)
{

    Q_ASSERT_X(m_packetHandlerBase, "TcpSocketConnection::TcpSocketConnection(PacketHandlerBase *packetHandlerBase, QObject *parent)", "Invalid PacketHandlerBase!");
    //m_packetHandlerBase = new PacketHandlerBase(this);


    buffer = QByteArray();
    nextBlockSize = 0;

    QObject::connect(this, SIGNAL(readyRead()), this, SLOT(slotProcessReadyRead()));


}

TcpSocketConnection::~TcpSocketConnection(){
    buffer.clear();
    nextBlockSize = 0;


}

bool TcpSocketConnection::sendTCPDatagram(const QByteArray &data) {
    qWarning("----TcpSocketConnection::sendTCPDatagram(const QByteArray &data)");

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_7);
    out << quint16(0) << data;

    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));
    //write(block);
    return (write(block) == block.size());

}

void TcpSocketConnection::destory(){
    qWarning()<<"TCP Connection Closed!"<<" Target:"<<peerAddress().toString()<<" Port"<<peerPort();
    //abort();
    close();

    emit signlaCanBeDestroyed();

    //TODO:???????????
    deleteLater();


}

void TcpSocketConnection::abortConnection(){
    this->abort();
}

void TcpSocketConnection::slotProcessReadyRead() {
    qDebug("----TcpSocketConnection::slotProcessReadyRead()");

    QDataStream in(this);
    in.setVersion(QDataStream::Qt_4_7);

    forever {
        if (nextBlockSize == 0) {
            if (bytesAvailable() < sizeof(quint16)){
                break;
            }
            in >> nextBlockSize;
        }

        if (nextBlockSize == PACKET_TERMINATING_SYMBOL) {
            //TODO:是否需要关闭连接？
            close();
            break;
        }

        if (bytesAvailable() < nextBlockSize){
            break;
        }

        in >> buffer;
        qDebug()<<"----buffer.size():"<<buffer.size();
        QVariant v(buffer);
        if (v.canConvert<Packet>()){
            //Packet *packet = new Packet();
            Packet *packet = m_packetHandlerBase->getPacket();
            *packet = v.value<Packet>();
            packet->setTransmissionProtocol(TP_TCP);
            packet->setPeerHostAddress(peerAddress());
            packet->setPeerHostPort(peerPort());
            packet->setLocalHostAddress(localAddress());
            packet->setLocalHostPort(localPort());

            qWarning()<<"";
            qWarning()<<"~~packet->getPacketData().size():"<<packet->getPacketData().size();
            qWarning()<<"~~peerAddress.toString():"<<peerAddress().toString();
            qWarning()<<"~~peerPort:"<<peerPort();
            qWarning()<<"~~localAddress():"<<localAddress().toString();
            qWarning()<<"~~localPort():"<<localPort();
            qWarning()<<"";

            m_packetHandlerBase->appendIncomingPacket(packet);
            //emit signalNewTCPPacketReceived(packet);

        }

        //TODO:消息处理
        //qDebug()<< "----TcpSocketConnection::slotProcessReadyRead()~~Data Size:"<<buffer.size();



        //emit signalTCPDatagramsReceived(buffer);

        nextBlockSize = 0;
        buffer.clear();

    }


}






} //namespace HEHUI
