

#include <QMessageBox>
#include <QtConcurrentRun>
#include <QDateTime>

#include "HHSharedCore/hcryptography.h"

#include "tcpwidget.h"


namespace HEHUI {


TCPWidget::TCPWidget(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);


    tcpProtocol = new TCP(this);
    connect(tcpProtocol, SIGNAL(connected(int, const QString &, quint16)), this, SLOT(connected(int, const QString &, quint16)));
    connect(tcpProtocol, SIGNAL(disconnected(int, const QString &, quint16)), this, SLOT(disconnected(int, const QString &, quint16)));
    connect(tcpProtocol, SIGNAL(dataReceived(const QString &, quint16, QByteArray *)), this, SLOT(dataReceived(const QString &, quint16, QByteArray *)));

    peerSockeet = -1;

    isListening = false;
    isConnected = false;

    m_receivedDataCount = 0;

    startTime = endTime = QDateTime();
    totalDataSize = 0;


    m_peerAddress = QHostAddress(ui.lineEditIP->text());
    m_peerPort = ui.spinBoxRemotePort->value();

    localPort = 0;

}

TCPWidget::~TCPWidget()
{

    if(tcpProtocol){
        tcpProtocol->closeServer();
    }
    delete tcpProtocol;

}

void TCPWidget::listen(){

    if(isListening){
        tcpProtocol->closeServer();
        ui.toolButtonListen->setText("Listen");
        isListening = false;
        return;
    }


    if(!startServer(ui.spinBoxLocalPort->value())){
        return;
    }

    ui.toolButtonListen->setText("Close");
    isListening = true;


}

void TCPWidget::connectToPeer(){

    m_peerAddress = QHostAddress(ui.lineEditIP->text());
    m_peerPort = ui.spinBoxRemotePort->value();

    if(m_peerAddress.isNull()){
        QMessageBox::critical(this, "Error", "Invalid IP!");
        ui.lineEditIP->setFocus();
    }

    ui.toolButtonConnect->setEnabled(false);


    if(isConnected){
        ui.toolButtonConnect->setText("Disconnecting...");
        tcpProtocol->disconnectFromHost(peerSockeet);
        //disconnected(peerSockeet, m_peerAddress.toString(), m_peerPort);
    }else{

//        if(!isListening){
//            listen();
//        }

        ui.toolButtonConnect->setText("Connecting...");

        peerSockeet = tcpProtocol->connectToHost(m_peerAddress, m_peerPort, 30000);

//        UDTSocketStatus status = tcpProtocol->getUDTSocketStatus(peerSockeet);
//        qDebug();
//        qDebug()<<"peerSockeet:"<<peerSockeet<<" status:"<<status;
//        switch(status){
//        case INIT: //1
//        {
//            //qDebug()<<"INIT";
//        }
//            break;
//        case OPENED: //2
//        {
//            //qDebug()<<"OPENED";
//        }
//            break;
//        case LISTENING: //3
//        {
//            //qDebug()<<"LISTENING";

//        }
//            break;
//        case CONNECTING: //4
//        {
//            //qDebug()<<"CONNECTING";
//        }
//            break;
//        case CONNECTED: //5
//        {
//            qDebug()<<"CONNECTED";

//        }
//            break;
//        case BROKEN: //6
//        {
//            qDebug()<<"BROKEN";
//        }
//            break;
//        case CLOSING: //7
//        {
//            //qDebug()<<"CLOSING";
//        }
//            break;
//        case CLOSED: //8
//        {
//            qDebug()<<"CLOSED";
//        }
//            break;
//        case NONEXIST: //9
//        {
//            qDebug()<<"NONEXIST";
//        }
//            break;
//        default:
//            break;


//        }

//        qDebug();









        if(!tcpProtocol->isConnected(peerSockeet)){
            qDebug()<<"Can not connect to peer!";
            disconnected(peerSockeet, m_peerAddress.toString(), m_peerPort);
            return;
        }
        qDebug()<<"peerSockeet:"<<peerSockeet;
        tcpProtocol->getAddressInfoFromSocket(peerSockeet, 0, &localPort);

        ui.textBrowser->append("Local port:"+QString::number(localPort));
        ui.spinBoxLocalPort->setValue(localPort);

        connected(peerSockeet, m_peerAddress.toString(), m_peerPort);

    }


}

bool TCPWidget::startServer(quint16 port){



    if(!tcpProtocol->listen(QHostAddress::Any, port)){
        QMessageBox::critical(this, "Error", tcpProtocol->serverErrorString());
        return false;
    }


    tcpProtocol->serverAddressInfo(0, &localPort);


    ui.textBrowser->append("Listening on port:"+QString::number(localPort));
    ui.spinBoxLocalPort->setValue(localPort);

    return true;


}

void TCPWidget::send(){

    QByteArray data;
    int size = ui.spinBoxDataSize->value() * 1024;
    int msec = QDateTime::currentDateTime().time().msec();
    for(int i=0; i<size; i++){
        data.append(QString::number(i+msec).toLatin1());
        if(data.size() >= size){break;}
    }
    data.resize(size);
    QString md5 = Cryptography::MD5(data).toBase64();


    int count = ui.spinBoxSendCount->value();
    int sent= 0;
    int failed = 0;

    ui.textBrowser->append("Start Sending Data To "+m_peerAddress.toString()+":"+QString::number(m_peerPort)+" MD5:"+md5 +" Time:"+QDateTime::currentDateTime().toString("hh:mm:ss:zzz") );

    QByteArray a("0x0000");
    tcpProtocol->sendData(peerSockeet, &a);

    bool ok = false;
    for(int i=0;i<count;i++) {
        ok = tcpProtocol->sendData(peerSockeet, &data);
        if(ok){
            sent++;
        }else{
            ui.textBrowser->append(tcpProtocol->socketErrorString(peerSockeet));

            failed++;
        }
        qApp->processEvents();
    }

    QByteArray b("0xFFFF");
    tcpProtocol->sendData(peerSockeet, &b);

    ui.textBrowser->append("Sent:"+QString::number(sent)+" Failed:"+QString::number(failed)+" Data Sent To "+m_peerAddress.toString()+":"+QString::number(m_peerPort)+" Time:"+QDateTime::currentDateTime().toString("hh:mm:ss:zzz") );


}

void TCPWidget::clean(){

    ui.textBrowser->clear();
    m_receivedDataCount = 0;

}

void TCPWidget::connected(int socketID, const QString &peerAddress, quint16 peerPort){

    ui.textBrowser->append("Connected! "+peerAddress+":"+QString::number(peerPort)+" Socket ID:"+QString::number(socketID));

    //if(!isListening){
        ui.lineEditIP->setEnabled(false);
        ui.spinBoxRemotePort->setEnabled(false);
        ui.toolButtonConnect->setText("Disconnect");
        ui.toolButtonConnect->setEnabled(true);
        ui.toolButtonSend->setEnabled(true);
    //}
    isConnected = true;


}

void TCPWidget::signalConnectToPeerTimeout(const QString &peerAddress, quint16 peerPort){
    ui.textBrowser->append("Connecting Timeout! "+peerAddress+":"+QString::number(peerPort));

    ui.lineEditIP->setEnabled(true);
    ui.spinBoxRemotePort->setEnabled(true);
    ui.toolButtonConnect->setText("Connect");
    ui.toolButtonConnect->setEnabled(true);

    isConnected = false;

}

void TCPWidget::disconnected(int socketID, const QString &peerAddress, quint16 peerPort){
    ui.textBrowser->append("Disconnected! "+peerAddress+":"+QString::number(peerPort)+" Socket ID:"+QString::number(socketID));

    isConnected = false;

        ui.lineEditIP->setEnabled(true);
        ui.spinBoxRemotePort->setEnabled(true);
        ui.toolButtonConnect->setText("Connect");
        ui.toolButtonConnect->setEnabled(true);
        ui.toolButtonSend->setEnabled(false);



//    tcpProtocol->closeSocket(socketID);


}

void TCPWidget::dataReceived(const QString &peerAddress, quint16 peerPort, QByteArray *data){


    if(*data == QByteArray("0x0000")){
        startTime = QDateTime::currentDateTime();
        ui.textBrowser->append("---START---" + startTime.toString("mm:ss:zzz"));
        totalDataSize = 0;
        m_receivedDataCount = 0;

        return;
    }
    if(*data == QByteArray("0xFFFF")){
        endTime = QDateTime::currentDateTime();
        ui.textBrowser->append("---END---" + endTime.toString("mm:ss:zzz"));
        ui.textBrowser->append(QString("Total Data Size:%1 KB, Speed:%2 KB/S ").arg(totalDataSize/1024).arg((float)(totalDataSize*1000)/(startTime.msecsTo(endTime)*1024)) + endTime.toString(" mm:ss:zzz"));

        return;
    }

    totalDataSize += data->size();

    m_receivedDataCount++;
    QString md5 = Cryptography::MD5(*data).toBase64();

    ui.textBrowser->append(QString::number(m_receivedDataCount)+" MD5:"+md5);


}



} //namespace HEHUI
