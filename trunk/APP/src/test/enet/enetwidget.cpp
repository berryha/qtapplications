

#include <QMessageBox>
#include <QtConcurrentRun>


#include "HHSharedCore/hcryptography.h"

#include "enetwidget.h"


namespace HEHUI {


ENETWidget::ENETWidget(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);


    enetProtocol = 0;
    m_peerID = 0;

    isListening = false;
    isConnected = false;

    m_receivedDataCount = 0;

    m_peerAddress = QHostAddress(ui.lineEditIP->text());
    m_peerPort = ui.spinBoxRemotePort->value();

    localPort = 0;

}

ENETWidget::~ENETWidget()
{


    if(enetProtocol){
        enetProtocol->close();
    }
    delete enetProtocol;

}

void ENETWidget::listen(){

    if(isListening){
        enetProtocol->close();
        ui.toolButtonListen->setText("Listen");
        isListening = false;
        return;
    }


    if(!startRUDPServer(ui.spinBoxLocalPort->value())){
        return;
    }

    ui.toolButtonListen->setText("Close");
    isListening = true;

}

void ENETWidget::connectToPeer(){

    m_peerAddress = QHostAddress(ui.lineEditIP->text());
    m_peerPort = ui.spinBoxRemotePort->value();

    if(m_peerAddress.isNull()){
        QMessageBox::critical(this, "Error", "Invalid IP!");
        ui.lineEditIP->setFocus();
    }

//    ui.toolButtonConnect->setEnabled(false);


    if(isConnected){
        ui.toolButtonConnect->setText("Disconnecting...");
//        enetProtocol->disconnectNow(m_peerID);
        enetProtocol->disconnectNow(m_peerID);
//        disconnected(m_peerID, m_peerAddress.toString(), m_peerPort);
    }else{

        if(!isListening){
            ui.spinBoxLocalPort->setValue(ui.spinBoxLocalPort->value() + 1);
            listen();
        }

        ui.toolButtonConnect->setText("Connecting...");

        if(!enetProtocol->connectToHost(m_peerAddress, m_peerPort, &m_peerID, 10000) ){

            QMessageBox::critical(this, "Error", "Failed to connect to peer!");
            ui.toolButtonConnect->setText("Connect");
            return;
        }

        enetProtocol->getPeerAddressInfo(m_peerID, 0, &localPort);
        ui.textBrowser->append("Listening on port:"+QString::number(localPort));
        ui.spinBoxLocalPort->setValue(localPort);


//        connected(m_peerID, m_peerAddress.toString(), m_peerPort);

    }


}

bool ENETWidget::startRUDPServer(quint16 port){

    if(!enetProtocol){
        enetProtocol = new ENETProtocolTest(this);
        connect(enetProtocol, SIGNAL(connected(quint32, const QString &, quint16)), this, SLOT(connected(quint32, const QString &, quint16)));
        connect(enetProtocol, SIGNAL(disconnected(quint32, const QString &, quint16)), this, SLOT(disconnected(quint32, const QString &, quint16)));

        connect(enetProtocol, SIGNAL(dataReceived(const QString &, quint16, const QByteArray &)), this, SLOT(dataReceived(const QString &, quint16, const QByteArray &)));

    }

    if( !enetProtocol->listen(port) ){
        return false;
    }

    enetProtocol->startWaitingForIOInAnotherThread(20);

    enetProtocol->getLocalListeningAddressInfo(0, &localPort);

    ui.textBrowser->append("Listening on port:"+QString::number(localPort));
    ui.spinBoxLocalPort->setValue(localPort);

    return true;

}

void ENETWidget::send(){

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


    ui.textBrowser->append("----"+QDateTime::currentDateTime().toString("hh:mm:ss:zzz"));
    ui.textBrowser->append("Start Sending Data To "+m_peerAddress.toString()+":"+QString::number(m_peerPort)+" MD5:"+md5 );




    QByteArray a("0x0000");
    enetProtocol->sendData(m_peerID, &a);

    for(int i=0;i<count;i++){
        if(enetProtocol->sendData(m_peerID, &data)){
            sent++;
        }else{
            failed++;
        }
        qApp->processEvents();
    }
    QByteArray b("0xFFFF");
    enetProtocol->sendData(m_peerID, &b);

    ui.textBrowser->append("Sent:"+QString::number(sent)+" Failed:"+QString::number(failed)+" Data Sent To "+m_peerAddress.toString()+":"+QString::number(m_peerPort) );
    ui.textBrowser->append("----"+QDateTime::currentDateTime().toString("hh:mm:ss:zzz"));


}

void ENETWidget::clean(){

    ui.textBrowser->clear();
    m_receivedDataCount = 0;
    totalDataSize = 0;

}

void ENETWidget::connected(quint32 peerID, const QString &address, quint16 peerPort){

    ui.textBrowser->append("Connected! "+address+":"+QString::number(peerPort)+" peerID:"+QString::number(peerID));

    //if(!isListening){
    ui.lineEditIP->setEnabled(false);
    ui.spinBoxRemotePort->setEnabled(false);
    ui.toolButtonConnect->setText("Disconnect");
    ui.toolButtonConnect->setEnabled(true);
    ui.toolButtonSend->setEnabled(true);
    //}
    isConnected = true;


}

void ENETWidget::signalConnectToPeerTimeout(const QHostAddress &peerAddress, quint16 peerPort){
    ui.textBrowser->append("Connecting Timeout! "+peerAddress.toString()+":"+QString::number(peerPort));

    ui.lineEditIP->setEnabled(true);
    ui.spinBoxRemotePort->setEnabled(true);
    ui.toolButtonConnect->setText("Connect");
    ui.toolButtonConnect->setEnabled(true);

    isConnected = false;

}

void ENETWidget::disconnected(quint32 peerID, const QString &address, quint16 peerPort){
    ui.textBrowser->append("Disconnected! "+address+":"+QString::number(peerPort)+" peerID:"+QString::number(peerID));

    if(!isListening){
        ui.lineEditIP->setEnabled(true);
        ui.spinBoxRemotePort->setEnabled(true);
        ui.toolButtonConnect->setText("Connect");
        ui.toolButtonConnect->setEnabled(true);
        ui.toolButtonSend->setEnabled(false);
    }

    isConnected = false;

}

void ENETWidget::dataReceived(const QString &peerAddress, quint16 peerPort, const QByteArray &data){


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
