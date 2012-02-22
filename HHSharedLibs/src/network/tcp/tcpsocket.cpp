/*
 ****************************************************************************
 * tcpsocket.cpp
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

#include "tcpsocket.h"

namespace HEHUI {

TcpSocket::TcpSocket(QObject *parent)
    :QTcpSocket(parent)
{

    //buffer = new QByteArray();

    nextBlockSize = 0;

    QObject::connect(this, SIGNAL(readyRead()), this, SLOT(slotProcessReadyRead()));


}

TcpSocket::~TcpSocket(){

//    buffer->clear();
//    delete buffer;
//    buffer = 0;

    nextBlockSize = 0;


}

bool TcpSocket::sendTCPDatagram(const QByteArray *data) {
    qDebug()<<"--TcpSocket::sendTCPDatagram(...) "<<" data->size():"<<data->size();

    QMutexLocker locker(&writeMutex);

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_7);
    out << quint16(0) << *data;

    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));
    //write(block);
    return (write(block) == block.size());

}


void TcpSocket::abortConnection(){
    this->abort();
}

void TcpSocket::slotProcessReadyRead() {

    QtConcurrent::run(this, &TcpSocket::readData);

}

void TcpSocket::readData(){

    QMutexLocker locker(&readMutex);

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

        emit dataReceived(buffer);

        qDebug()<<"--1--buffer.size():"<<buffer.size();


        nextBlockSize = 0;
//        buffer.clear();
//        qDebug()<<"--2--buffer.size():"<<buffer.size();

    }

}






} //namespace HEHUI
