

#include <QMessageBox>
#include <QtConcurrentRun>


#include "HHSharedCore/hcryptography.h"

#include "rudp.h"

#ifdef Q_CC_MSVC
#include <windows.h>
#include "HHSharedWindowsManagement/hwindowsmanagement.h"
#define msleep(x) Sleep(x)
#endif

#ifdef Q_CC_GNU
#include <unistd.h>
#define msleep(x) usleep(x*1000)
#endif

namespace HEHUI {


RUDPWidget::RUDPWidget(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);


        rudpSocket = 0;
        m_packetHandlerBase = new PacketHandlerBase(this);
        //rudpSocket = new RUDPSocket(m_packetHandlerBase, this);
        networkManager = new NetworkManagerBase(m_packetHandlerBase);
        clientPacketsParser = new ClientPacketsParser(networkManager, this);

        //Single Process Thread
        QtConcurrent::run(clientPacketsParser, &ClientPacketsParser::run);

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

    if(clientPacketsParser){
        on_toolButtonConnect_clicked();
        clientPacketsParser->aboutToQuit();
    }

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
        networkManager->closeRUDPServer(0);
        //rudpSocket->close();
        ui.toolButtonListen->setText("Listen");
        isListening = false;
        return;
    }

//    if(!rudpSocket){
//        rudpSocket = networkManager->startRUDPServerListening(QHostAddress::Null, ui.spinBoxLocalPort->value());
//    }
//    if(0 == rudpSocket){
//        QMessageBox::critical(this, "Bind Error", rudpSocket->errorString());
//        return;
//    }
//    localPort = rudpSocket->localPort();

//    connect(rudpSocket, SIGNAL(peerConnected(const QHostAddress &, quint16)), this, SLOT(connected(const QHostAddress &, quint16)));
//    connect(rudpSocket, SIGNAL(signalConnectToPeerTimeout(const QHostAddress &, quint16)), this, SLOT(signalConnectToPeerTimeout(const QHostAddress &, quint16)));
//    connect(rudpSocket, SIGNAL(peerDisconnected(const QHostAddress &, quint16)), this, SLOT(disconnected(const QHostAddress &, quint16)));
//    connect(clientPacketsParser, SIGNAL(dataReceived(const QHostAddress &, quint16, const QByteArray &)), this, SLOT(dataReceived(const QHostAddress &, quint16, const QByteArray &)));
    if(!startRUDPServer(ui.spinBoxLocalPort->value())){
        return;
    }

//    if(!rudpSocket->bind(ui.spinBoxLocalPort->value())){
//        QMessageBox::critical(this, "Bind Error", rudpSocket->errorString());
//        return;
//    }

    ui.toolButtonListen->setText("Close");
    isListening = true;

    ui.textBrowser->append("Listening on port:"+QString::number(ui.spinBoxLocalPort->value()));

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
        rudpSocket->disconnectFromPeer(m_peerAddress, m_peerPort);
        ui.toolButtonConnect->setText("Disconnecting...");
    }else{
//        if(!rudpSocket){
//            rudpSocket = networkManager->startRUDPServerListening(QHostAddress::Null, 0);
//        }
//        if(0 == rudpSocket){
//            QMessageBox::critical(this, "Bind Error", rudpSocket->errorString());
//            ui.toolButtonConnect->setEnabled(true);
//            return;
//        }
        if(!startRUDPServer(0)){
            ui.toolButtonConnect->setEnabled(true);
            return;
        }
        ui.textBrowser->append("Listening on port:"+QString::number(localPort));

        rudpSocket->connectToPeer(m_peerAddress, m_peerPort);
        ui.toolButtonConnect->setText("Connecting...");

    }


}

bool RUDPWidget::startRUDPServer(quint16 port){

    if(!rudpSocket){
        rudpSocket = networkManager->startRUDPServerListening(QHostAddress::Null, port);
    }
    if(0 == rudpSocket){
        QMessageBox::critical(this, "Bind Error", networkManager->errorString());
        return false;
    }
    localPort = rudpSocket->localPort();

    connect(rudpSocket, SIGNAL(peerConnected(const QHostAddress &, quint16)), this, SLOT(connected(const QHostAddress &, quint16)));
    connect(rudpSocket, SIGNAL(signalConnectToPeerTimeout(const QHostAddress &, quint16)), this, SLOT(signalConnectToPeerTimeout(const QHostAddress &, quint16)));
    connect(rudpSocket, SIGNAL(peerDisconnected(const QHostAddress &, quint16)), this, SLOT(disconnected(const QHostAddress &, quint16)));
    connect(clientPacketsParser, SIGNAL(dataReceived(const QHostAddress &, quint16, const QByteArray &)), this, SLOT(dataReceived(const QHostAddress &, quint16, const QByteArray &)));

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

clientPacketsParser->sendTestData(m_peerAddress, m_peerPort, QHostAddress::Null, localPort, &data);

        //Method 1
//        quint16 fragmentDataID = rudpSocket->beginDataTransmission(m_peerAddress, m_peerPort);
//        int totalSent = 0;
//        int sent;
//        while (totalSent < size)
//        {
//            qApp->processEvents();
//            if (0 == (sent = rudpSocket->sendDatagram(m_peerAddress, m_peerPort, &data, totalSent, true)) )
//            {
//                //msleep(10);
//                //cout << "Send Error!" << endl;
//                //break;
//            }

//            totalSent += sent;
//        }

        i++;

//        rudpSocket->endDataTransmission(m_peerAddress, m_peerPort, fragmentDataID);




            //Method 2
//        qApp->processEvents();
//        //if(rudpSocket->canSendData(size)){
//            if(!rudpSocket->sendDatagram(m_peerAddress, m_peerPort, &data)){
//                failed++;
//                ui.textBrowser->append(QString::number(i)+" Data Sent Failed!");
//                msleep(50);
//            }else{
//                sent++;
//                i++;
//            }
//        //}



            //Method 3
//        if (size == rudpSocket->sendDatagram(m_peerAddress, m_peerPort, &data) )
//        {
//            sent++;
//        }else{
//            cout << "Send Error!" << endl;
//            failed++;
//        }
//        i++;


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

    if(!isListening){
        ui.lineEditIP->setEnabled(false);
        ui.spinBoxRemotePort->setEnabled(false);
        ui.toolButtonConnect->setText("Disconnect");
        ui.toolButtonConnect->setEnabled(true);
        ui.toolButtonSend->setEnabled(true);
    }
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

void RUDPWidget::dataReceived(const QHostAddress &peerAddress, quint16 peerPort, const QByteArray &data){

    m_receivedDataCount++;
    QString md5 = Cryptography::MD5(data).toBase64();

    ui.textBrowser->append(QString::number(m_receivedDataCount)+" Data Received From "+peerAddress.toString()+":"+QString::number(peerPort)+" MD5:"+md5+" Time:"+QDateTime::currentDateTime().toString("hh:mm:ss:zzz"));

}



} //namespace HEHUI
