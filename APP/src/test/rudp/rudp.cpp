

#include <QMessageBox>
#include <QtConcurrentRun>


#include "HHSharedCore/hcryptography.h"

#include "rudp.h"



namespace HEHUI {


RUDPWidget::RUDPWidget(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);



    m_packetHandlerBase = new PacketHandlerBase(this);
    networkManager = new NetworkManagerBase(m_packetHandlerBase);
    clientPacketsParser = new ClientPacketsParser(networkManager, this);
    connect(clientPacketsParser, SIGNAL(dataReceived(const QHostAddress &, quint16, const QByteArray &)), this, SLOT(dataReceived(const QHostAddress &, quint16, const QByteArray &)));

    rudpSocket = 0;
//    rudpSocket = new RUDPSocket(m_packetHandlerBase, this);
//    connect(rudpSocket, SIGNAL(peerConnected(const QHostAddress &, quint16)), this, SLOT(connected(const QHostAddress &, quint16)));
//    connect(rudpSocket, SIGNAL(signalConnectToPeerTimeout(const QHostAddress &, quint16)), this, SLOT(signalConnectToPeerTimeout(const QHostAddress &, quint16)));
//    connect(rudpSocket, SIGNAL(peerDisconnected(const QHostAddress &, quint16)), this, SLOT(disconnected(const QHostAddress &, quint16)));

    //Single Process Thread
    QtConcurrent::run(clientPacketsParser, &ClientPacketsParser::run);


    isListening = false;
    isConnected = false;

    m_receivedDataCount = 0;

    m_peerAddress = QHostAddress(ui.lineEditIP->text());
    m_peerPort = ui.spinBoxRemotePort->value();

    localPort = 0;

}

RUDPWidget::~RUDPWidget()
{

    on_toolButtonConnect_clicked();
    clientPacketsParser->aboutToQuit();
    delete clientPacketsParser;
    clientPacketsParser = 0;

    networkManager->closeRUDPServer(0);
    delete networkManager;
    networkManager = 0;

    m_packetHandlerBase->clean();
    delete m_packetHandlerBase;
    m_packetHandlerBase = 0;



}

void RUDPWidget::listen(){

    if(isListening){
        if(isConnected){
            ui.toolButtonConnect->setText("Disconnecting...");
            rudpSocket->disconnectFromPeer(m_peerAddress, m_peerPort);
            disconnected(m_peerAddress, m_peerPort);
        }

        networkManager->closeRUDPServer(0);
        rudpSocket = 0;
        ui.toolButtonListen->setText("Listen");
        isListening = false;
        ui.textBrowser->append("RUDP Server Closed!");

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
        rudpSocket->disconnectFromPeer(m_peerAddress, m_peerPort);
        disconnected(m_peerAddress, m_peerPort);
    }else{

        if(!isListening){
            listen();
        }
        if(!isListening){
            return;
        }

        ui.toolButtonConnect->setText("Connecting...");

        rudpSocket->connectToPeer(m_peerAddress, m_peerPort);

    }


}

bool RUDPWidget::startRUDPServer(quint16 port){


    rudpSocket = networkManager->startRUDPServerListening(QHostAddress::Any, port);
    if(!rudpSocket){
        isListening = false;
        ui.textBrowser->append("Failed to start RUDP server listening on port:"+QString::number(port));

        return false;
    }
    isListening = true;

    connect(rudpSocket, SIGNAL(peerConnected(const QHostAddress &, quint16)), this, SLOT(connected(const QHostAddress &, quint16)));
    connect(rudpSocket, SIGNAL(signalConnectToPeerTimeout(const QHostAddress &, quint16)), this, SLOT(signalConnectToPeerTimeout(const QHostAddress &, quint16)));
    connect(rudpSocket, SIGNAL(peerDisconnected(const QHostAddress &, quint16, bool)), this, SLOT(disconnected(const QHostAddress &, quint16, bool)));


    ui.textBrowser->append("Listening on port:"+QString::number(port));
    ui.spinBoxLocalPort->setValue(port);

    return true;


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
        rudpSocket->sendDatagram(m_peerAddress, m_peerPort, &data);

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

void RUDPWidget::disconnected(const QHostAddress &peerAddress, quint16 peerPort, bool normalClose){
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
