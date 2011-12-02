

#include <QMessageBox>
#include <QtConcurrentRun>


#include "HHSharedCore/hcryptography.h"

#include "rudp.h"

//#ifdef Q_CC_MSVC
//#include <windows.h>
//#include "HHSharedWindowsManagement/hwindowsmanagement.h"
//#define msleep(x) Sleep(x)
//#endif

//#ifdef Q_CC_GNU
//#include <unistd.h>
//#define msleep(x) usleep(x*1000)
//#endif

namespace HEHUI {


RUDPWidget::RUDPWidget(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);


////        rudpSocket = 0;
//        m_packetHandlerBase = new PacketHandlerBase(this);
////        //rudpSocket = new RUDPSocket(m_packetHandlerBase, this);
//        networkManager = new NetworkManagerBase(m_packetHandlerBase);
//        clientPacketsParser = new ClientPacketsParser(networkManager, this);

////        //Single Process Thread
//        QtConcurrent::run(clientPacketsParser, &ClientPacketsParser::run);

        udtProtocol = 0;

        isListening = false;
        isConnected = false;

        m_receivedDataCount = 0;

        m_peerAddress = QHostAddress(ui.lineEditIP->text());
        m_peerPort = ui.spinBoxRemotePort->value();

        localPort = 0;
//        connect(rudpSocket, SIGNAL(peerConnected(const QHostAddress &, quint16)), this, SLOT(connected(const QHostAddress &, quint16)));
//        connect(rudpSocket, SIGNAL(signalConnectToPeerTimeout(const QHostAddress &, quint16)), this, SLOT(signalConnectToPeerTimeout(const QHostAddress &, quint16)));
//        connect(rudpSocket, SIGNAL(peerDisconnected(const QHostAddress &, quint16)), this, SLOT(disconnected(const QHostAddress &, quint16)));
//        connect(rudpSocket, SIGNAL(dataReceived(const QHostAddress &, quint16, const QByteArray &)), this, SLOT(dataReceived(const QHostAddress &, quint16, const QByteArray &)));

}

RUDPWidget::~RUDPWidget()
{

//    if(clientPacketsParser){
//        on_toolButtonConnect_clicked();
//        clientPacketsParser->aboutToQuit();
//    }

//    delete clientPacketsParser;
//    clientPacketsParser = 0;

////    networkManager->closeRUDPServer(0);
////    delete networkManager;
////    networkManager = 0;

//    m_packetHandlerBase->clean();
//    delete m_packetHandlerBase;
//    m_packetHandlerBase = 0;

    if(udtProtocol){
        udtProtocol->closeUDTProtocol();
    }
    delete udtProtocol;

}

void RUDPWidget::listen(){

    if(isListening){
        udtProtocol->closeUDTProtocol();
        ui.toolButtonListen->setText("Listen");
        isListening = false;
        return;
    }


    if(!startRUDPServer(ui.spinBoxLocalPort->value())){
        return;
    }

    ui.toolButtonListen->setText("Close");
    isListening = true;


    //ui.textBrowser->append("Listening on port:"+QString::number(ui.spinBoxLocalPort->value()));

}

void RUDPWidget::connectToPeer(){

    m_peerAddress = QHostAddress(ui.lineEditIP->text());
    m_peerPort = ui.spinBoxRemotePort->value();

    if(m_peerAddress.isNull()){
        QMessageBox::critical(this, "Error", "Invalid IP!");
        ui.lineEditIP->setFocus();
    }

    ui.toolButtonConnect->setEnabled(false);


    if(isConnected){
        ui.toolButtonConnect->setText("Disconnecting...");
        udtProtocol->closeSocket(peerSockeet);
        disconnected(m_peerAddress, m_peerPort);
    }else{

        if(!isListening){
            listen();
        }

//        if(!startRUDPServer(0)){
//            ui.toolButtonConnect->setEnabled(true);
//            return;
//        }

        ui.toolButtonConnect->setText("Connecting...");

        peerSockeet = udtProtocol->connectToHost(m_peerAddress, m_peerPort, false);
        UDTSocketStatus status = udtProtocol->getUDTSocketStatus(peerSockeet);
        qDebug();
        qDebug()<<"peerSockeet:"<<peerSockeet<<" status:"<<status;
        switch(status){
        case INIT: //1
        {
            //qDebug()<<"INIT";
        }
            break;
        case OPENED: //2
        {
            //qDebug()<<"OPENED";
        }
            break;
        case LISTENING: //3
        {
            //qDebug()<<"LISTENING";

        }
            break;
        case CONNECTING: //4
        {
            //qDebug()<<"CONNECTING";
        }
            break;
        case CONNECTED: //5
        {
            qDebug()<<"CONNECTED";

        }
            break;
        case BROKEN: //6
        {
            qDebug()<<"BROKEN";
        }
            break;
        case CLOSING: //7
        {
            //qDebug()<<"CLOSING";
        }
            break;
        case CLOSED: //8
        {
            qDebug()<<"CLOSED";
        }
            break;
        case NONEXIST: //9
        {
            qDebug()<<"NONEXIST";
        }
            break;
        default:
            break;


        }

        qDebug();









        if(peerSockeet == UDTProtocolBase::INVALID_UDT_SOCK){
            qDebug()<<"Can not connect to peer!";
            disconnected(m_peerAddress, m_peerPort);
            return;
        }
        qDebug()<<"peerSockeet:"<<peerSockeet;
        udtProtocol->getAddressInfoFromSocket(udtProtocol->getServerSocket(), 0, &localPort, false);
        ui.textBrowser->append("Listening on port:"+QString::number(localPort));
        ui.spinBoxLocalPort->setValue(localPort);

        //UDTSTATUS status = UDT::getsockstate(peerSockeet);
        //qDebug()<<"status:"<<status;
        connected(m_peerAddress, m_peerPort);

    }


}

bool RUDPWidget::startRUDPServer(quint16 port){

    if(!udtProtocol){
        udtProtocol = new UDTProtocolTest(true, 0, this);
        connect(udtProtocol, SIGNAL(connected(const QHostAddress &, quint16)), this, SLOT(connected(const QHostAddress &, quint16)));
        connect(udtProtocol, SIGNAL(disconnected(const QHostAddress &, quint16)), this, SLOT(disconnected(const QHostAddress &, quint16)));

        connect(udtProtocol, SIGNAL(dataReceived(const QString &, quint16, const QByteArray &)), this, SLOT(dataReceived(const QString &, quint16, const QByteArray &)));

    }

    serverSocket = udtProtocol->listen(port);
    if(serverSocket == UDTProtocolBase::INVALID_UDT_SOCK){
        return false;
    }
    qDebug()<<"serverSocket:"<<serverSocket;

    udtProtocol->startWaitingForIOInSeparateThread();

    udtProtocol->getAddressInfoFromSocket(udtProtocol->getServerSocket(), 0, &localPort, false);
    ui.textBrowser->append("Listening on port:"+QString::number(localPort));
    ui.spinBoxLocalPort->setValue(localPort);

    return true;
    //return udtSocket->listen(port, QHostAddress("200.200.200.117"));


}

void RUDPWidget::send(){

    QByteArray data;
    int size = ui.spinBoxDataSize->value();
    //data.resize(ui.spinBoxDataSize->value());
    for(int i=0; i<size; i++){
        data.append(QString::number(i).toAscii());
        if(data.size() >= size){break;}
    }
    data.resize(size);

    QString md5 = Cryptography::MD5(data).toBase64();


    ui.textBrowser->append("----"+QDateTime::currentDateTime().toString("hh:mm:ss:zzz"));
    ui.textBrowser->append("Start Sending Data To "+m_peerAddress.toString()+":"+QString::number(m_peerPort)+" MD5:"+md5 );

    int count = ui.spinBoxSendCount->value();
    int sent= 0;
    int failed = 0;

    int i = 0;
    while (i<count) {
        udtProtocol->sendUDTStreamData(peerSockeet, &data);

        i++;
    }


    ui.textBrowser->append("Sent:"+QString::number(sent)+" Failed:"+QString::number(failed)+" Data Sent To "+m_peerAddress.toString()+":"+QString::number(m_peerPort) );
    ui.textBrowser->append("----"+QDateTime::currentDateTime().toString("hh:mm:ss:zzz"));


}

void RUDPWidget::clean(){

    ui.textBrowser->clear();
    m_receivedDataCount = 0;

}

void RUDPWidget::connected(const QHostAddress &peerAddress, quint16 peerPort){

    ui.textBrowser->append("Connected! "+peerAddress.toString()+":"+QString::number(peerPort));

    //if(!isListening){
        ui.lineEditIP->setEnabled(false);
        ui.spinBoxRemotePort->setEnabled(false);
        ui.toolButtonConnect->setText("Disconnect");
        ui.toolButtonConnect->setEnabled(true);
        ui.toolButtonSend->setEnabled(true);
    //}
    isConnected = true;

}

void RUDPWidget::signalConnectToPeerTimeout(const QHostAddress &peerAddress, quint16 peerPort){
    ui.textBrowser->append("Connecting Timeout! "+peerAddress.toString()+":"+QString::number(peerPort));

    ui.lineEditIP->setEnabled(true);
    ui.spinBoxRemotePort->setEnabled(true);
    ui.toolButtonConnect->setText("Connect");
    ui.toolButtonConnect->setEnabled(true);

    isConnected = false;

}

void RUDPWidget::disconnected(const QHostAddress &peerAddress, quint16 peerPort){
    ui.textBrowser->append("Disconnected! "+peerAddress.toString()+":"+QString::number(peerPort));

    if(!isListening){
        ui.lineEditIP->setEnabled(true);
        ui.spinBoxRemotePort->setEnabled(true);
        ui.toolButtonConnect->setText("Connect");
        ui.toolButtonConnect->setEnabled(true);
        ui.toolButtonSend->setEnabled(false);
    }

    isConnected = false;

}

void RUDPWidget::dataReceived(const QString &peerAddress, quint16 peerPort, const QByteArray &data){

    m_receivedDataCount++;
    QString md5 = Cryptography::MD5(data).toBase64();

    ui.textBrowser->append(QString::number(m_receivedDataCount)+" Data Received From "+peerAddress+":"+QString::number(peerPort)+" MD5:"+md5+" Time:"+QDateTime::currentDateTime().toString("hh:mm:ss:zzz"));

}



} //namespace HEHUI
