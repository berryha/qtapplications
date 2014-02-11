

#include <QMessageBox>
#include <QtConcurrentRun>


#include "HHSharedCore/hcryptography.h"

#include "udt.h"


namespace HEHUI {


UDTWidget::UDTWidget(QWidget *parent)
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
        serverSocket = peerSockeet = UDTProtocolBase::INVALID_UDT_SOCK;

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



        streamMode = true;


}

UDTWidget::~UDTWidget()
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
        udtProtocol->closeSocket(peerSockeet);
        udtProtocol->close();
    }
    udtProtocol->deleteLater();

}

void UDTWidget::listen(){

    if(isListening){
        udtProtocol->close();
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

void UDTWidget::connectToPeer(){

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

        peerSockeet = udtProtocol->connectToHost(m_peerAddress, m_peerPort, 0, false);

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
        connected(peerSockeet, m_peerAddress.toString(), m_peerPort);

    }


}

bool UDTWidget::startRUDPServer(quint16 port){

    if(!udtProtocol){
        UDTProtocolTest::SocketOptions opt;
//        opt.UDT_SNDSYN = false;
//        opt.UDT_RCVSYN = false;
//        opt.UDT_SNDTIMEO = -1;
//        opt.UDT_RCVTIMEO = -1;
//        opt.UDP_SNDBUF = 10240000;
//        opt.UDP_RCVBUF = 10240000;

        udtProtocol = new UDTProtocolTest(streamMode, &opt, this);
        connect(udtProtocol, SIGNAL(connected(int, const QString &, quint16)), this, SLOT(connected(int, const QString &, quint16)));
        connect(udtProtocol, SIGNAL(disconnected(int)), this, SLOT(disconnected(int)));

        connect(udtProtocol, SIGNAL(dataReceived(const QString &, quint16, const QByteArray &)), this, SLOT(dataReceived(const QString &, quint16, const QByteArray &)));

    }

    serverSocket = udtProtocol->listen(port);
    if(serverSocket == UDTProtocolBase::INVALID_UDT_SOCK){
        return false;
    }
    qDebug()<<"serverSocket:"<<serverSocket;

    udtProtocol->startWaitingForIOInOneThread(1);

    udtProtocol->getAddressInfoFromSocket(udtProtocol->getServerSocket(), 0, &localPort, false);
    ui.textBrowser->append("Listening on port:"+QString::number(localPort));
    ui.spinBoxLocalPort->setValue(localPort);

    return true;
    //return udtSocket->listen(port, QHostAddress("200.200.200.117"));


}

void UDTWidget::send(){

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
    udtProtocol->sendUDTStreamData(peerSockeet, &a);


    bool ok = false;
    for(int i=0;i<count;i++) {
        if(streamMode){
            ok = udtProtocol->sendUDTStreamData(peerSockeet, &data);
        }else{
            ok = udtProtocol->sendUDTMessageData(peerSockeet, &data);
        }
        if(ok){
            sent++;
        }else{
            ui.textBrowser->append(udtProtocol->getLastErrorMessage());

            failed++;
        }
        qApp->processEvents();
    }

    QByteArray b("0xFFFF");
    udtProtocol->sendUDTStreamData(peerSockeet, &b);

    ui.textBrowser->append("Sent:"+QString::number(sent)+" Failed:"+QString::number(failed)+" Data Sent To "+m_peerAddress.toString()+":"+QString::number(m_peerPort)+" Time:"+QDateTime::currentDateTime().toString("hh:mm:ss:zzz") );


}

void UDTWidget::clean(){

    ui.textBrowser->clear();
    m_receivedDataCount = 0;

}

void UDTWidget::connected(int socket, const QString &peerAddress, quint16 peerPort){

    ui.textBrowser->append("Connected! "+peerAddress+":"+QString::number(peerPort)+QString(" Socket:%1").arg(socket));

    //if(!isListening){
        ui.lineEditIP->setEnabled(false);
        ui.spinBoxRemotePort->setEnabled(false);
        ui.toolButtonConnect->setText("Disconnect");
        ui.toolButtonConnect->setEnabled(true);
        ui.toolButtonSend->setEnabled(true);
    //}
    isConnected = true;

    peerSockeet = socket;


}

void UDTWidget::signalConnectToPeerTimeout(const QHostAddress &peerAddress, quint16 peerPort){
    ui.textBrowser->append("Connecting Timeout! "+peerAddress.toString()+":"+QString::number(peerPort));

    ui.lineEditIP->setEnabled(true);
    ui.spinBoxRemotePort->setEnabled(true);
    ui.toolButtonConnect->setText("Connect");
    ui.toolButtonConnect->setEnabled(true);

    isConnected = false;

}

void UDTWidget::disconnected(const QHostAddress &peerAddress, quint16 peerPort){
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

void UDTWidget::disconnected(int socket){
    ui.textBrowser->append(QString("Disconnected! Socket:%1").arg(socket));

    if(!isListening){
        ui.lineEditIP->setEnabled(true);
        ui.spinBoxRemotePort->setEnabled(true);
        ui.toolButtonConnect->setText("Connect");
        ui.toolButtonConnect->setEnabled(true);
        ui.toolButtonSend->setEnabled(false);
    }

    isConnected = false;
    peerSockeet = UDTProtocolBase::INVALID_UDT_SOCK;

}

void UDTWidget::dataReceived(const QString &peerAddress, quint16 peerPort, const QByteArray &data){

    if(data == QByteArray("0x0000")){
        startTime = QDateTime::currentDateTime();
        ui.textBrowser->append("---START---" + startTime.toString("mm:ss:zzz"));
        totalDataSize = 0;
        m_receivedDataCount = 0;

        return;
    }
    if(data == QByteArray("0xFFFF")){
        endTime = QDateTime::currentDateTime();
        ui.textBrowser->append("---END---" + endTime.toString("mm:ss:zzz"));
        ui.textBrowser->append(QString("Total Data Size:%1 KB, Speed:%2 KB/S ").arg(totalDataSize/1024).arg((float)(totalDataSize*1000)/(startTime.msecsTo(endTime)*1024)) + endTime.toString(" mm:ss:zzz"));

        return;
    }

    totalDataSize += data.size();

    m_receivedDataCount++;
    QString md5 = Cryptography::MD5(data).toBase64();

    ui.textBrowser->append(QString::number(m_receivedDataCount)+" MD5:"+md5);


}



} //namespace HEHUI
