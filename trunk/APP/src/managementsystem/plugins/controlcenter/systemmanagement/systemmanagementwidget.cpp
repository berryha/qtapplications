
#include "systemmanagementwidget.h"

#include <QHostInfo>
#include <QHostAddress>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include <QTime>
#include <QSettings>

#ifdef Q_OS_WIN32
    #include "HHSharedWindowsManagement/hwindowsmanagement.h"
    #include "../../sharedms/global_shared.h"
#endif

#include "constants.h"

#include "HHSharedGUI/hdatabaseconnecter.h"


namespace HEHUI {

QMap<QString/*Short Name*/, QString/*Department*/> SystemManagementWidget::departments = QMap<QString, QString>();

SystemManagementWidget::SystemManagementWidget(RTP *rtp, ControlCenterPacketsParser *parser, const QString &adminName, const QString &computerName, const QString &users, const QString &peerIPAddress, const QString &peerMACAddress, bool usbsdEnabled, bool programesEnabled, const QString &admins, bool isJoinedToDomain, QWidget *parent)
    : QWidget(parent), m_adminName(adminName), m_computerName(computerName), m_users(users), m_peerIPAddress(QHostAddress(peerIPAddress)), m_peerMACAddress(peerMACAddress), m_usbsdEnabled(usbsdEnabled), m_programesEnabled(programesEnabled), m_isJoinedToDomain(isJoinedToDomain)
{
    ui.setupUi(this);

    ui.lineEditComputerName->setText(computerName.toUpper());
    ui.lineEditUsers->setText(users);
    ui.lineEditIPAddress->setText(peerIPAddress);
    ui.lineEditMACAddress->setText(peerMACAddress);

    if(computerName.toLower() == QHostInfo::localHostName().toLower()){
        localComputer = true;
    }else{
        localComputer = false;
    }

    if(localComputer){
        this->m_peerIPAddress = QHostAddress::LocalHost;
    }

    ui.comboBoxProtocol->addItem(tr("Auto"), quint8(RTP::AUTO));
    ui.comboBoxProtocol->addItem("UDT", quint8(RTP::UDT));
    ui.comboBoxProtocol->addItem("TCP", quint8(RTP::TCP));



    m_winDirPath = "";




#ifdef Q_OS_WIN32

    if(localComputer){
        ui.groupBoxAdministrationTools->show();
        ui.tabWidget->removeTab(ui.tabWidget->indexOf(ui.tabFileManagement));
    }else{
        int index = ui.tabWidget->indexOf(ui.tabLocalManagement);
        ui.tabWidget->removeTab(index);

        ui.tabLocalManagement->setEnabled(false);
        //            ui.groupBoxAdministrationTools->setEnabled(false);;
        //            ui.groupBoxAdministrationTools->hide();
    }

    WindowsManagement wm;
    m_winDirPath = wm.getEnvironmentVariable("windir");


    if(m_isJoinedToDomain){
        ui.pushButtonAdminsManagement->hide();
        ui.pushButtonDomain->setText(tr("Unjoin The Domain"));
    }else{
        ui.pushButtonAdminsManagement->show();
        ui.pushButtonDomain->setText(tr("Join A Domain"));
    }



#else
    ui.pushButtonShowAdmin->setEnabled(false);
    ui.pushButtonShowAdmin->hide();
//    ui.pushButtonRenameComputer->setEnabled(false);
//    ui.pushButtonRenameComputer->hide();
//    ui.pushButtonDomain->setEnabled(false);
//    ui.pushButtonDomain->hide();

    int index = ui.tabWidget->indexOf(ui.tabLocalManagement);
    ui.tabWidget->removeTab(index);
    ui.tabLocalManagement->setEnabled(false);
    //        ui.groupBoxAdministrationTools->setEnabled(false);;
    //        ui.groupBoxAdministrationTools->hide();



#endif


    //        if(usbsdEnabled){
    //            ui.pushButtonUSBSD->setText(tr("Disable USB SD"));
    //        }else{

    //            ui.pushButtonUSBSD->setText(tr("Enable USB SD"));
    //        }

    //        if(programesEnabled){
    //            ui.pushButtonPrograms->setText(tr("Disable Programs"));
    //        }else{
    //            ui.pushButtonPrograms->setText(tr("Enable Programs"));
    //        }


    administratorsManagementMenu = 0;

    if(!admins.trimmed().isEmpty()){
        m_administrators = admins.split(",");
    }
    //        administratorsManagementMenu = new QMenu(this);
    //        if(!users.isEmpty()){
    //            administratorsManagementMenu->addAction(ui.actionAddAdmin);
    //        }
    //        if(!administrators.isEmpty()){
    //            administratorsManagementMenu->addAction(ui.actionDeleteAdmin);
    //        }
    //        ui.pushButtonAdminsManagement->setMenu(administratorsManagementMenu);


    ui.comboBoxLocation->addItem(tr("Dong Guan"), "dg");
    ui.comboBoxLocation->addItem(tr("Ying De"), "yd");
    ui.comboBoxLocation->addItem(tr("Hong Kong"), "hk");
    QString location = computerName.left(2).toLower();
    if("yd" == location){
        ui.comboBoxLocation->setCurrentIndex(1);
    }else if("hk" == location){
        ui.comboBoxLocation->setCurrentIndex(2);
    }else{
        ui.comboBoxLocation->setCurrentIndex(0);
    }
    connect(ui.comboBoxLocation, SIGNAL(currentIndexChanged(int)), this, SLOT(getNewComputerName()));

    if(departments.isEmpty()){
        departments.insert("it", tr("IT"));
        departments.insert("ac", tr("Account"));
        departments.insert("ad", tr("AdminDept"));
        departments.insert("co", tr("Cost"));
        departments.insert("cu", tr("Custom"));
        departments.insert("gm", tr("GMO"));
        departments.insert("hr", tr("HR"));
        departments.insert("ma", tr("Marker"));
        departments.insert("pd", tr("PDS"));
        departments.insert("pg", tr("PG"));
        departments.insert("pl", tr("Plan"));
        departments.insert("pm", tr("PMC"));
        departments.insert("qc", tr("QC"));
        departments.insert("re", tr("Retail"));
        departments.insert("sa", tr("Sales"));
        //departmentsHash.insert("", tr("Sample"));
        departments.insert("se", tr("Secretary"));
        departments.insert("sh", tr("Ship"));
        departments.insert("sp", tr("Shop"));
        departments.insert("wh", tr("WHouse"));
    }
    QString department = computerName.mid(2, 2).toLower();
//    int index = -1;
    foreach (QString key, departments.keys()) {
        ui.comboBoxDepartment->addItem(departments.value(key), key);
//        if(key == department){
//            index = ui.comboBoxDepartment->currentIndex();
//        }
    }
    for(int i=0; i<ui.comboBoxDepartment->count(); i++){
        if(ui.comboBoxDepartment->itemData(i) == department){
            ui.comboBoxDepartment->setCurrentIndex(i);
            break;
        }
    }
    connect(ui.comboBoxDepartment, SIGNAL(currentIndexChanged(int)), this, SLOT(getNewComputerName()));

    int sn = computerName.right(5).toInt();
    ui.spinBoxSN->setValue(sn);
    connect(ui.spinBoxSN, SIGNAL(valueChanged(int)), this, SLOT(getNewComputerName()));


    ui.groupBoxBasicSettings->setEnabled(false);
    ui.groupBoxOtherSettings->setEnabled(false);
    //ui.groupBoxSettings->hide();

    ui.horizontalLayoutCommand->setEnabled(false);


    //ui.tabSystemInfo->setEnabled(false);
    ui.toolButtonRequestSystemInfo->setEnabled(false);
    ui.toolButtonRescanSystemInfo->setEnabled(false);
    ui.toolButtonSaveAs->setEnabled(false);

    QHeaderView *view = ui.tableWidgetSoftware->horizontalHeader();
    view->resizeSection(0, 200);
    view->setVisible(true);


    ui.tabRemoteManagement->setEnabled(false);


    queryModel = 0;


    setRTP(rtp);
    m_peerSocket = INVALID_SOCK_ID;

    m_aboutToCloseSocket = false;


//    m_udtProtocol = 0;
//    setUDTProtocol(udtProtocol);

//    m_tcpServer = tcpServer;
//    m_usingTCP = false;

    setControlCenterPacketsParser(parser);

    //    connect(controlCenterPacketsParser, SIGNAL(signalClientResponseAdminConnectionResultPacketReceived(const QString &, bool, const QString &)), this, SLOT(processClientResponseAdminConnectionResultPacket(const QString &, bool, const QString &)));
    //    connect(controlCenterPacketsParser, SIGNAL(signalClientMessagePacketReceived(const QString &, const QString &)), this, SLOT(clientMessageReceived(const QString &, const QString &)));
    //    connect(controlCenterPacketsParser, SIGNAL(signalClientResponseClientSummaryInfoPacketReceived(const QString&, const QString&, const QString&, const QString&, const QString&, bool, bool, const QString&, const QString&)), this, SLOT(clientResponseClientSummaryInfoPacketReceived(const QString&, const QString&, const QString&, const QString&, const QString&, bool, bool, const QString&, const QString&)));

    //    connect(controlCenterPacketsParser, SIGNAL(signalClientResponseClientInfoPacketReceived(const QString &, const QString &)), this, SLOT(clientInfoPacketReceived(const QString &, const QString &)));


//    m_udtProtocolForFileTransmission = 0;
//    m_peerFileTransmissionSocket = INVALID_SOCK_ID;

    clientResponseAdminConnectionResultPacketReceived = false;

    remoteConsoleRunning = false;

    //    requestConnectionToClientTimer = 0;

    //    info = new ClientInfo(computerName, this);
    //    info->setUsers(usersInfo);
    //    info->setUsbSDEnabled(usbsdEnabled);
    //    info->setProgramsEnabled(programesEnabled);

    //No editing possible.
    ui.tableWidgetSoftware->setEditTriggers(QAbstractItemView::NoEditTriggers);

//    m_fileManager = 0;


    m_fileManagementWidget = qobject_cast<FileManagement *>(ui.tabFileManagement);
    Q_ASSERT(m_fileManagementWidget);
    if(!localComputer){
        m_fileManagementWidget->setPacketsParser(parser);
    }


//    connect(m_fileManagementWidget, SIGNAL(signalShowRemoteFiles(const QString &)), this, SLOT(requestFileSystemInfo(const QString &)));
//    connect(m_fileManagementWidget, SIGNAL(signalUploadFilesToRemote(const QStringList &, const QString &)), this, SLOT(requestUploadFilesToRemote(const QStringList &, const QString &)));
//    connect(m_fileManagementWidget, SIGNAL(signalDownloadFileFromRemote(const QStringList &, const QString &)), this, SLOT(requestDownloadFileFromRemote(const QStringList &, const QString &)));



}

SystemManagementWidget::~SystemManagementWidget()
{

    qDebug()<<"~SystemManagementWidget()";

//    if(queryModel){
//        queryModel->clear();
//        delete queryModel;
//        queryModel = 0;
//    }


    //#ifdef Q_OS_WIN32
    //    if(!adminProcesses.isEmpty()){
    //        QString msg = tr("<p>Please make sure that these processes run as admin are closed! Or that raises the issue of security!</p>");
    //        msg += adminProcesses.join(", ");
    //        QMessageBox::warning(this, tr("Warning"), msg);
    //    }


    //#endif





    //    if(requestConnectionToClientTimer){
    //        requestConnectionToClientTimer->stop();
    //        delete requestConnectionToClientTimer;
    //        requestConnectionToClientTimer = 0;
    //    }



    //    delete ClientInfo;
    //    ClientInfo = 0;


}

void SystemManagementWidget::closeEvent(QCloseEvent *event){
    qDebug()<<"--SystemManagementWidget::closeEvent(...)";

#ifdef Q_OS_WIN32
    if(!adminProcesses.isEmpty()){
        QString msg = tr("<p>Please make sure that these processes run as admin are closed! Or that raises the issue of security!</p>");
        msg += adminProcesses.join(", ");
        QMessageBox::warning(this, tr("Warning"), msg);
    }

#endif

    controlCenterPacketsParser->disconnect(this);

    if(queryModel){
        queryModel->clear();
        delete queryModel;
        queryModel = 0;
    }

    m_aboutToCloseSocket = true;
    m_rtp->closeSocket(m_peerSocket);
    m_peerSocket = INVALID_SOCK_ID;

    event->accept();

}

//void SystemManagementWidget::dragEnterEvent(QDragEnterEvent *event){

//    if (event->mimeData()->hasUrls()) {
//        event->acceptProposedAction();
//    }
//}

//void SystemManagementWidget::dragMoveEvent(QDragMoveEvent *event)
//{
//    // Accept file actions with all extensions.

////    if (event->mimeData()->hasUrls()) {
//        event->acceptProposedAction();
////    }

//}

//void SystemManagementWidget::dropEvent(QDropEvent *event)
//{

//    // Accept drops if the file exists.

//    const QMimeData *mimeData = event->mimeData();
//    QStringList files;
//    if (mimeData->hasUrls()) {
//        QList<QUrl> urlList = mimeData->urls();
//        foreach (QUrl url, urlList) {
//            if (url.isValid() && url.scheme().toLower() == "file" ){
//                QString fileName = url.path().remove(0, 1);
//                //QFileInfo fi(fileName);
//                //QMessageBox::information(this, fileName, fileName);
//                if (QFile::exists(fileName)){
//                    files.append(fileName);
//                }
//            }
//        }

//    }

//    event->acceptProposedAction();

//    if (!files.isEmpty()){
//        filesDropped(files);
//    }

//}


void SystemManagementWidget::setRTP(RTP *rtp){

    if(!rtp){
        ui.toolButtonVerify->setEnabled(false);
        return;
    }

    this->m_rtp = rtp;
    connect(m_rtp, SIGNAL(disconnected(int)), this, SLOT(peerDisconnected(int)));
}

//void SystemManagementWidget::setUDTProtocol(UDTProtocol *udtProtocol){

//    if(!udtProtocol){
//        ui.toolButtonVerify->setEnabled(false);
//        return;
//    }

//    this->m_udtProtocol = udtProtocol;
//    connect(m_udtProtocol, SIGNAL(disconnected(int)), this, SLOT(peerDisconnected(int)));

//}

//void SystemManagementWidget::setTCPServer(TCPServer *tcpServer){
//    if(!tcpServer){
//        ui.toolButtonVerify->setEnabled(false);
//        return;
//    }

//    this->m_tcpServer = tcpServer;
//    connect(m_tcpServer, SIGNAL(disconnected(int)), this, SLOT(peerDisconnected(int)));


//}

void SystemManagementWidget::setControlCenterPacketsParser(ControlCenterPacketsParser *parser){

    if(!parser){
        ui.toolButtonVerify->setEnabled(false);
        return;
    }

    this->controlCenterPacketsParser = parser;
    connect(controlCenterPacketsParser, SIGNAL(signalClientOnlineStatusChanged(int, const QString&, bool)), this, SLOT(processClientOnlineStatusChangedPacket(int, const QString&, bool)), Qt::QueuedConnection);
    connect(controlCenterPacketsParser, SIGNAL(signalClientResponseAdminConnectionResultPacketReceived(int, const QString &, bool, const QString &)), this, SLOT(processClientResponseAdminConnectionResultPacket(int, const QString &, bool, const QString &)));
    connect(controlCenterPacketsParser, SIGNAL(signalClientMessagePacketReceived(const QString &, const QString &, quint8)), this, SLOT(clientMessageReceived(const QString &, const QString &, quint8)));
    connect(controlCenterPacketsParser, SIGNAL(signalClientResponseClientSummaryInfoPacketReceived(const QString&, const QString&, const QString&, const QString&, const QString&, bool, bool, const QString&, bool, const QString&)), this, SLOT(clientResponseClientSummaryInfoPacketReceived(const QString&, const QString&, const QString&, const QString&, const QString&, bool, bool, const QString&, bool, const QString&)));

    connect(controlCenterPacketsParser, SIGNAL(signalClientResponseClientDetailedInfoPacketReceived(const QString &, const QString &)), this, SLOT(clientDetailedInfoPacketReceived(const QString &, const QString &)));

    connect(controlCenterPacketsParser, SIGNAL(signalClientResponseRemoteConsoleStatusPacketReceived(const QString &, bool, const QString &, quint8)), this, SLOT(clientResponseRemoteConsoleStatusPacketReceived(const QString &, bool, const QString &, quint8)));
    connect(controlCenterPacketsParser, SIGNAL(signalRemoteConsoleCMDResultFromClientPacketReceived(const QString &, const QString &)), this, SLOT(remoteConsoleCMDResultFromClientPacketReceived(const QString &, const QString &)));

    connect(controlCenterPacketsParser, SIGNAL(signalUserResponseRemoteAssistancePacketReceived(const QString &, const QString &, bool)), this, SLOT(userResponseRemoteAssistancePacketReceived(const QString &, const QString &, bool)));
    
//    ////////////////////
//    connect(controlCenterPacketsParser, SIGNAL(signalFileSystemInfoReceived(int, const QString &, const QByteArray &)), this, SLOT(fileSystemInfoReceived(int, const QString &, const QByteArray &)));
//    //File TX
//    connect(controlCenterPacketsParser, SIGNAL(signalAdminRequestUploadFile(int, const QByteArray &, const QString &, quint64, const QString &)), this, SLOT(processPeerRequestUploadFilePacket(int, const QByteArray &, const QString &,quint64, const QString &)), Qt::QueuedConnection);
//    connect(controlCenterPacketsParser, SIGNAL(signalAdminRequestDownloadFile(int,QString)), this, SLOT(processPeerRequestDownloadFilePacket(int,QString)), Qt::QueuedConnection);

//    connect(controlCenterPacketsParser, SIGNAL(signalFileDownloadRequestAccepted(int, const QString &, const QByteArray &, quint64)), this, SLOT(fileDownloadRequestAccepted(int, const QString &, const QByteArray &, quint64)), Qt::QueuedConnection);
//    connect(controlCenterPacketsParser, SIGNAL(signalFileDownloadRequestDenied(int , const QString &, const QString &)), this, SLOT(fileDownloadRequestDenied(int , const QString &, const QString &)), Qt::QueuedConnection);
//    connect(controlCenterPacketsParser, SIGNAL(signalFileUploadRequestResponsed(int, const QByteArray &, bool, const QString &)), this, SLOT(fileUploadRequestResponsed(int, const QByteArray &, bool, const QString &)), Qt::QueuedConnection);

//    connect(controlCenterPacketsParser, SIGNAL(signalFileDataRequested(int, const QByteArray &, int, int )), this, SLOT(processFileDataRequestPacket(int,const QByteArray &, int, int )), Qt::QueuedConnection);
//    connect(controlCenterPacketsParser, SIGNAL(signalFileDataReceived(int, const QByteArray &, int, const QByteArray &, const QByteArray &)), this, SLOT(processFileDataReceivedPacket(int, const QByteArray &, int, const QByteArray &, const QByteArray &)), Qt::QueuedConnection);
//    connect(controlCenterPacketsParser, SIGNAL(signalFileTXStatusChanged(int, const QByteArray &, quint8)), this, SLOT(processFileTXStatusChangedPacket(int, const QByteArray &, quint8)), Qt::QueuedConnection);
//    connect(controlCenterPacketsParser, SIGNAL(signalFileTXError(int , const QByteArray &, quint8 , const QString &)), this, SLOT(processFileTXErrorFromPeer(int , const QByteArray &, quint8 , const QString &)), Qt::QueuedConnection);


    ui.toolButtonVerify->setEnabled(true);

}

void SystemManagementWidget::on_toolButtonVerify_clicked(){

    if(!verifyPrivilege()){
        return;
    }

    m_peerIPAddress = QHostAddress(ui.lineEditIPAddress->text().trimmed());
    if(localComputer){
        this->m_peerIPAddress = QHostAddress::LocalHost;
    }
    if(m_peerIPAddress.isNull()){
        QMessageBox::critical(this, tr("Error"), tr("Invalid IP Address!"));
        ui.lineEditIPAddress->setFocus();
        return;
    }

    clientResponseAdminConnectionResultPacketReceived = false;

    ui.toolButtonVerify->setEnabled(false);

    QString errorMessage;
    if(m_peerSocket == INVALID_SOCK_ID){
        m_peerSocket = m_rtp->connectToHost(m_peerIPAddress, UDT_LISTENING_PORT, 5000, &errorMessage, RTP::Protocol(ui.comboBoxProtocol->itemData(ui.comboBoxProtocol->currentIndex()).toUInt()));
    }
    if(m_peerSocket == INVALID_SOCK_ID){
        QMessageBox::critical(this, tr("Error"), tr("Can not connect to host!<br>%1").arg(errorMessage));
        ui.toolButtonVerify->setEnabled(true);
        return;
    }

    bool ok = controlCenterPacketsParser->sendAdminRequestConnectionToClientPacket(m_peerSocket, this->m_computerName, this->m_users);
    if(!ok){
        QString err = m_rtp->lastErrorString();
        m_rtp->closeSocket(m_peerSocket);
        m_peerSocket = INVALID_SOCK_ID;
        QMessageBox::critical(this, tr("Error"), tr("Can not send connection request to host!<br>%1").arg(err));
        ui.toolButtonVerify->setEnabled(true);
        return;
    }

    QTimer::singleShot(60000, this, SLOT(requestConnectionToClientTimeout()));

}

void SystemManagementWidget::on_pushButtonUSBSD_clicked(){

//    if(!verifyPrivilege()){
//        return;
//    }

    QString text = tr("Do you really want to <font color = 'red'><b>%1</b></font> the USB SD on the computer?").arg(m_usbsdEnabled?tr("disable"):tr("enable"));
    int ret = QMessageBox::question(this, tr("Question"), text,
                                    QMessageBox::Yes | QMessageBox::No, QMessageBox::No
                                    );
    if(ret == QMessageBox::No){
        return;
    }

    //    QHostAddress address = QHostAddress(peerAddress);
    //    if(localComputer){
    //        address = QHostAddress(QHostAddress::LocalHost);
    //    }

    bool m_temporarilyAllowed = true;
    if(!m_usbsdEnabled){
        m_temporarilyAllowed = temporarilyAllowed();
    }

    bool ok = controlCenterPacketsParser->sendSetupUSBSDPacket(m_peerSocket, m_computerName, m_users, !m_usbsdEnabled, m_temporarilyAllowed, m_adminName);
    if(!ok){
        QMessageBox::critical(this, tr("Error"), tr("Can not send data to peer!<br>%1").arg(m_rtp->lastErrorString()));
        return;
    }
    ui.pushButtonUSBSD->setEnabled(false);

}

void SystemManagementWidget::on_pushButtonPrograms_clicked(){

//    if(!verifyPrivilege()){
//        return;
//    }

    QString text = tr("Do you really want to <font color = 'red'><b>%1</b></font> the Programes on the computer?").arg(m_programesEnabled?tr("disable"):tr("enable"));

    int ret = QMessageBox::question(this, tr("Question"), text,QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if(ret == QMessageBox::No){
        return;
    }

    //    QHostAddress address = QHostAddress(peerAddress);
    //    if(localComputer){
    //        address = QHostAddress(QHostAddress::LocalHost);
    //    }

    bool m_temporarilyAllowed = true;
    if(!m_programesEnabled){
        m_temporarilyAllowed = temporarilyAllowed();
    }

    bool ok = controlCenterPacketsParser->sendSetupProgramesPacket(m_peerSocket, m_computerName, m_users, !m_programesEnabled, m_temporarilyAllowed, m_adminName);
    if(!ok){
        QMessageBox::critical(this, tr("Error"), tr("Can not send data to peer!<br>%1").arg(m_rtp->lastErrorString()));
        return;
    }
    ui.pushButtonPrograms->setEnabled(false);

}

void SystemManagementWidget::on_pushButtonShowAdmin_clicked(){

    bool ok = controlCenterPacketsParser->sendShowAdminPacket(m_peerSocket, m_computerName, m_users, true);
    if(!ok){
        QMessageBox::critical(this, tr("Error"), tr("Can not send data to peer!<br>%1").arg(m_rtp->lastErrorString()));
        return;
    }

    ui.pushButtonShowAdmin->setEnabled(false);

}

void SystemManagementWidget::on_pushButtonRemoteAssistance_clicked(){
    
//    if(!verifyPrivilege()){
//        return;
//    }
    
    //ui.pushButtonRemoteAssistance->setEnabled(false);
    emit requestRemoteAssistance();
    
    bool ok = controlCenterPacketsParser->sendRemoteAssistancePacket(m_peerSocket, m_computerName, m_adminName);
    if(!ok){
        QMessageBox::critical(this, tr("Error"), tr("Can not send data to peer!<br>%1").arg(m_rtp->lastErrorString()));
        return;
    }

}

void SystemManagementWidget::on_actionAddAdmin_triggered(){

//    if(!verifyPrivilege()){
//        return;
//    }

    bool ok = false;
    QString item = QInputDialog::getItem(this, tr("Select The User"), tr("User:"), m_users.split(","), 0, false, &ok);
    if(!ok || item.isEmpty()){
        return;
    }

    QString text = tr("Do you really want to <font color = 'red'>add <b>%1</b></font> to local administrators group? ").arg(item);
    int ret = QMessageBox::question(this, tr("Question"), text,
                                    QMessageBox::Yes | QMessageBox::No, QMessageBox::No
                                    );
    if(ret == QMessageBox::No){
        return;
    }

    ok = controlCenterPacketsParser->sendModifyAdminGroupUserPacket(m_peerSocket, m_computerName, item, true, m_adminName);
    if(!ok){
        QMessageBox::critical(this, tr("Error"), tr("Can not send data to peer!<br>%1").arg(m_rtp->lastErrorString()));
        return;
    }

    ui.pushButtonAdminsManagement->setEnabled(false);

}

void SystemManagementWidget::on_actionDeleteAdmin_triggered(){

//    if(!verifyPrivilege()){
//        return;
//    }

    bool ok = false;
    QString item = QInputDialog::getItem(this, tr("Select The User"), tr("User:"), m_administrators, 0, false, &ok);
    if(!ok || item.isEmpty()){
        return;
    }

    QString text = tr("Do you really want to <font color = 'red'>delete <b>%1</b></font> from local administrators group? ").arg(item);
    int ret = QMessageBox::question(this, tr("Question"), text,
                                    QMessageBox::Yes | QMessageBox::No, QMessageBox::No
                                    );
    if(ret == QMessageBox::No){
        return;
    }

    //    QHostAddress address = QHostAddress(peerAddress);
    //    if(localComputer){
    //        address = QHostAddress(QHostAddress::LocalHost);
    //    }

    ok = controlCenterPacketsParser->sendModifyAdminGroupUserPacket(m_peerSocket, m_computerName, item, false, m_adminName);
    if(!ok){
        QMessageBox::critical(this, tr("Error"), tr("Can not send data to peer!<br>%1").arg(m_rtp->lastErrorString()));
        return;
    }

    ui.pushButtonAdminsManagement->setEnabled(false);

}

void SystemManagementWidget::getNewComputerName(){
    QString location = ui.comboBoxLocation->itemData(ui.comboBoxLocation->currentIndex()).toString();
    QString dept = ui.comboBoxDepartment->itemData(ui.comboBoxDepartment->currentIndex()).toString();
    QString sn = QString::number(ui.spinBoxSN->value()).rightJustified(5, '0');

    QString newName = location + dept + sn;
    ui.lineEditNewComputerName->setText(newName.toUpper());

    ui.pushButtonRenameComputer->setEnabled((newName.size() == 9) && (m_computerName != newName));

}

void SystemManagementWidget::on_pushButtonRenameComputer_clicked(){

//    if(!verifyPrivilege()){
//        return;
//    }

    QString text = tr("Do you really want to <b><font color = 'red'>rename</font></b> the computer? ");
    int ret = QMessageBox::question(this, tr("Question"), text, QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if(ret == QMessageBox::No){
        return;
    }

    bool ok = false;
    QString newComputerName = ui.lineEditNewComputerName->text();
//    do {
//        newComputerName = QInputDialog::getText(this, tr("Rename Computer"), tr("New Computer Name:"), QLineEdit::Normal, m_computerName, &ok).trimmed();
//        if (ok){
//            if(newComputerName.isEmpty()){
//                QMessageBox::critical(this, tr("Error"), tr("Incorrect Computer Name!"));
//            }else{
//                break;
//            }
//        }

//    } while (ok);

    if(newComputerName.isEmpty()){
        return;
    }

    ok = controlCenterPacketsParser->sendRenameComputerPacket(m_peerSocket, m_computerName, newComputerName, m_adminName);
    if(!ok){
        QMessageBox::critical(this, tr("Error"), tr("Can not send data to peer!<br>%1").arg(m_rtp->lastErrorString()));
        return;
    }

    ui.pushButtonRenameComputer->setEnabled(false);

}

void SystemManagementWidget::on_pushButtonDomain_clicked(){

    //    if(!verifyPrivilege()){
    //        return;
    //    }

    QString text = tr("Do you really want to <b><font color = 'red'>join</font></b> the computer to a domain? ");
    if(m_isJoinedToDomain){
        text = tr("Do you really want to <b><font color = 'red'>unjoin</font></b> the computer from the domain? ");
    }
    int ret = QMessageBox::question(this, tr("Question"), text, QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if(ret == QMessageBox::No){
        return;
    }

    bool ok = false;
    QString domainOrWorkgroupName = "";
    QString joinType = m_isJoinedToDomain?tr("Workgroup"):tr("Domain");
//    if(!m_isJoinedToDomain){
        do {
            domainOrWorkgroupName = QInputDialog::getText(this, tr("Join To %1").arg(joinType), tr("%1 Name:").arg(joinType), QLineEdit::Normal, m_isJoinedToDomain?"WORKGROUP":DOMAIN_NAME, &ok).trimmed();
            if (ok){
                if(domainOrWorkgroupName.isEmpty()){
                    QMessageBox::critical(this, tr("Error"), tr("Incorrect %1 Name!").arg(joinType));
                }else{
                    break;
                }
            }

        } while (ok);
//    }

    if(domainOrWorkgroupName.isEmpty()){
        return;
    }

    ok = controlCenterPacketsParser->sendJoinOrUnjoinDomainPacket(m_peerSocket, m_computerName, m_adminName, !m_isJoinedToDomain, domainOrWorkgroupName);
    if(!ok){
        QMessageBox::critical(this, tr("Error"), tr("Can not send data to peer!<br>%1").arg(m_rtp->lastErrorString()));
        return;
    }

    ui.pushButtonDomain->setEnabled(false);

}

void SystemManagementWidget::on_pushButtonMMC_clicked(){

    //runProgrameAsAdmin("mmc.exe");
    runProgrameAsAdmin(m_winDirPath + "/system32/mmc.exe", "compmgmt.msc");
}

void SystemManagementWidget::on_pushButtonCMD_clicked(){

    runProgrameAsAdmin("cmd.exe");

}

void SystemManagementWidget::on_pushButtonRegedit_clicked(){

    runProgrameAsAdmin(m_winDirPath + "/regedit.exe");
}

void SystemManagementWidget::on_pushButtonOtherEXE_clicked(){

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Select an EXE File to run as admin:"), "", tr("EXE (*.exe);;All(*.*)"));

    if (fileName.isEmpty()) {
        return;
    }

    runProgrameAsAdmin(fileName);

}

void SystemManagementWidget::on_toolButtonQuerySystemInfo_clicked(){

    resetSystemInfo();

    QApplication::setOverrideCursor(Qt::WaitCursor);

    DatabaseConnecter dc(this);
    if(!dc.isDatabaseOpened(DB_CONNECTION_NAME,
                            REMOTE_SITOY_COMPUTERS_DB_DRIVER,
                            REMOTE_SITOY_COMPUTERS_DB_SERVER_HOST,
                            REMOTE_SITOY_COMPUTERS_DB_SERVER_PORT,
                            REMOTE_SITOY_COMPUTERS_DB_USER_NAME,
                            REMOTE_SITOY_COMPUTERS_DB_USER_PASSWORD,
                            REMOTE_SITOY_COMPUTERS_DB_NAME,
                            HEHUI::MYSQL
                            )){
        QApplication::restoreOverrideCursor();
        QMessageBox::critical(this, tr("Fatal Error"), tr("Database Connection Failed! Query Failed!"));
        qCritical() << QString("Error: Database Connection Failed! Query Failed!");
        return ;
    }


    QSqlDatabase db;
    db = QSqlDatabase::database(DB_CONNECTION_NAME);


    if(!queryModel){
        queryModel = new QSqlQueryModel(this);
    }
    queryModel->clear();

    QString queryString = QString("SELECT s.OS, s.Workgroup, s.Users, d.* FROM summaryinfo s, detailedinfo d WHERE s.ComputerName = '%1' AND d.ComputerName = '%1' ").arg(m_computerName);
    queryModel->setQuery(QSqlQuery(queryString, db));
    QApplication::restoreOverrideCursor();

    QSqlError error = queryModel->lastError();
    if (error.type() != QSqlError::NoError) {
        QString msg = QString("Can not query client info from database! %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
        //QApplication::restoreOverrideCursor();
        QMessageBox::critical(this, tr("Fatal Error"), msg);

        //MySQL数据库重启，重新连接
        if(error.number() == 2006){
            db.close();
            QSqlDatabase::removeDatabase(DB_CONNECTION_NAME);
            return;
        }

    }

    QSqlRecord record = queryModel->record(0);
    if(record.isEmpty()){
        QMessageBox::critical(this, tr("Fatal Error"), tr("No Record Found!"));
        return;
    }

    ui.osVersionLineEdit->setText(record.value("OS").toString());
    ui.installationDateLineEdit->setText(record.value("InstallationDate").toString());
    ui.computerNameLineEdit->setText(m_computerName);
    ui.workgroupLineEdit->setText(record.value("Workgroup").toString());

    QStringList storageInfo = record.value("Storage").toString().split("|");
    foreach (QString info, storageInfo) {
        ui.logicalDrivesComboBox->addItem(info);
    }

    ui.comboBoxSystemUsers->addItem(record.value("Users").toString());

    ui.osGroupBox->setEnabled(true);


    ui.cpuLineEdit->setText(record.value("CPU").toString());
    ui.motherboardLineEdit->setText(record.value("MotherboardName").toString());
    ui.dmiUUIDLineEdit->setText(record.value("DMIUUID").toString());
    ui.chipsetLineEdit->setText(record.value("Chipset").toString());
    ui.memoryLineEdit->setText(record.value("Memory").toString());
    ui.videoCardLineEdit->setText(record.value("Video").toString());
    ui.monitorLineEdit->setText(record.value("Monitor").toString());
    ui.audioLineEdit->setText(record.value("Audio").toString());

    QStringList nic1Info = record.value("NIC1").toString().split("|");
    if(nic1Info.size() == 3){
        ui.adapter1NameLineEdit->setText(nic1Info.at(0));
        ui.adapter1HDAddressLineEdit->setText(nic1Info.at(1));
        ui.adapter1IPAddressLineEdit->setText(nic1Info.at(2));
    }


    QStringList nic2Info = record.value("NIC2").toString().split("|");
    if(nic2Info.size() == 3){
        ui.adapter2NameLineEdit->setText(nic2Info.at(0));
        ui.adapter2HDAddressLineEdit->setText(nic2Info.at(1));
        ui.adapter2IPAddressLineEdit->setText(nic2Info.at(2));
    }


    ui.devicesInfoGroupBox->setEnabled(true);


    queryModel->clear();
    queryString = QString("SELECT SoftwareName, SoftwareVersion, Size, InstallationDate, Publisher FROM installedsoftware WHERE ComputerName = '%1' ").arg(m_computerName);
    queryModel->setQuery(QSqlQuery(queryString, db));
    while (queryModel->canFetchMore()){
        queryModel->fetchMore();
    }
    int rows = queryModel->rowCount();
    ui.tableWidgetSoftware->setRowCount(rows);
    for(int i=0; i<rows; i++){
        QSqlRecord record = queryModel->record(i);
        for(int j=0; j<5; j++){
            ui.tableWidgetSoftware->setItem(i, j, new QTableWidgetItem(record.value(j).toString()));
        }
    }
    queryModel->clear();

    //ui.tableWidgetSoftware->setModel(queryModel);
    ui.labelReportCreationTime->setText(tr("Last Update Time: %1").arg(record.value("UpdateTime").toString()));


    //QApplication::restoreOverrideCursor();

}

void SystemManagementWidget::on_toolButtonRequestSystemInfo_clicked(){

//    if(!verifyPrivilege()){
//        return;
//    }

    bool ok = controlCenterPacketsParser->sendRequestClientDetailedInfoPacket(m_peerSocket, m_computerName, false);
    if(!ok){
        QMessageBox::critical(this, tr("Error"), tr("Can not send data to peer!\n%1").arg(m_rtp->lastErrorString()));
        return;
    }

    QTimer::singleShot(60000, this, SLOT(requestClientInfoTimeout()));

    ui.toolButtonRequestSystemInfo->setEnabled(false);
    ui.toolButtonRescanSystemInfo->setEnabled(false);
    ui.toolButtonSaveAs->setEnabled(false);

    ui.osGroupBox->setEnabled(false);
    ui.devicesInfoGroupBox->setEnabled(false);

}

void SystemManagementWidget::on_toolButtonRescanSystemInfo_clicked(){

//    if(!verifyPrivilege()){
//        return;
//    }


    bool ok = controlCenterPacketsParser->sendRequestClientDetailedInfoPacket(m_peerSocket, m_computerName, true);
    if(!ok){
        QMessageBox::critical(this, tr("Error"), tr("Can not send data to peer!\n%1").arg(m_rtp->lastErrorString()));
        return;
    }

    QTimer::singleShot(60000, this, SLOT(requestClientInfoTimeout()));


    ui.toolButtonRequestSystemInfo->setEnabled(false);
    ui.toolButtonRescanSystemInfo->setEnabled(false);
    ui.toolButtonSaveAs->setEnabled(false);

    ui.osGroupBox->setEnabled(false);
    ui.devicesInfoGroupBox->setEnabled(false);


}

void SystemManagementWidget::on_toolButtonSaveAs_clicked(){



    QString path = QFileDialog::getSaveFileName(this, tr("File Save Path:"), QDir::homePath() + "/" + m_computerName, tr("INI (*.ini);;All(*.*)"));
    if(path.isEmpty()){
        return;
    }

    QFileInfo fileInfo = QFileInfo(path);
    if(fileInfo.exists()){
        QDir dir = fileInfo.dir();
        dir.remove(fileInfo.fileName());
    }

    QDir::setCurrent(QDir::tempPath());
    QString clientInfoFilePath = QString("./%1.ini").arg(m_computerName);
    if(!QFile::copy(clientInfoFilePath, path)){
        QMessageBox::critical(this, tr("Error"), tr("Can not save file!"));
    }

}

void SystemManagementWidget::on_toolButtonRunRemoteApplication_clicked(){

//    if(!verifyPrivilege()){
//        return;
//    }
    
    if(remoteConsoleRunning){
        int rep = QMessageBox::question(this, tr("Confirm"), tr("Do you really want to terminate the process?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if(rep == QMessageBox::Yes){
            bool ok = controlCenterPacketsParser->sendAdminRequestRemoteConsolePacket(m_peerSocket, m_computerName, "", m_adminName, false);
            if(!ok){
                QMessageBox::critical(this, tr("Error"), tr("Can not send data to peer!\n%1").arg(m_rtp->lastErrorString()));
                return;
            }
            //ui.toolButtonRunRemoteApplication->setEnabled(false);
        }

    }else{
        if(!verifyPrivilege()){
            return;
        }

        QString remoteAPPPath = ui.comboBoxRemoteApplicationPath->currentText();
        if(!remoteAPPPath.trimmed().isEmpty()){
            bool ok = controlCenterPacketsParser->sendAdminRequestRemoteConsolePacket(m_peerSocket, m_computerName, remoteAPPPath, m_adminName, true);
            if(!ok){
                QMessageBox::critical(this, tr("Error"), tr("Can not send data to peer!\n%1").arg(m_rtp->lastErrorString()));
                return;
            }
        }

        ui.comboBoxRemoteApplicationPath->setEnabled(false);
        ui.toolButtonRunRemoteApplication->setEnabled(false);
        

        ui.toolButtonSendCommand->setEnabled(false);
    }


    

    //QTimer::singleShot(10000, this, SLOT(requestRemoteConsoleTimeout()));




}

void SystemManagementWidget::on_toolButtonSendCommand_clicked(){

//    if(!verifyPrivilege()){
//        return;
//    }

    QString cmd = ui.comboBoxCommand->currentText();
    bool ok = controlCenterPacketsParser->sendRemoteConsoleCMDFromAdminPacket(m_peerSocket, m_computerName, cmd);
    if(!ok){
        QMessageBox::critical(this, tr("Error"), tr("Can not send data to peer!\n%1").arg(m_rtp->lastErrorString()));
        return;
    }

    ui.comboBoxCommand->insertItem(0, cmd);

    ui.comboBoxCommand->setEditText("");
    ui.comboBoxCommand->setFocus();

    //ui.textBrowserRemoteApplicationOutput->append(cmd + "\n");

}

void SystemManagementWidget::processClientOnlineStatusChangedPacket(int socketID, const QString &computerName, bool online){
    qDebug()<<"--SystemManagementWidget::processClientOnlineStatusChangedPacket(...)";

    if(socketID != m_peerSocket || computerName != this->m_computerName){
        return;
    }

//    QString ip = "";
//    quint16 port = 0;
//    if(!m_udtProtocol->getAddressInfoFromSocket(socketID, &ip, &port)){
//        qCritical()<<m_rtp->lastErrorString();
//        return;
//    }

    if(!online){
        peerDisconnected(true);
    }

    //qWarning()<<QString("Client %1 %2!").arg(clientName).arg(online?"Online":"Offline");

}

void SystemManagementWidget::processClientResponseAdminConnectionResultPacket(int socketID, const QString &computerName, bool result, const QString &message){
    qDebug()<<"SystemManagementWidget::processClientResponseVerifyInfoResultPacket:"<<"computerName:"<<computerName<<" result:"<<result;

    if(socketID != m_peerSocket || computerName != this->m_computerName){
        return;
    }

    clientResponseAdminConnectionResultPacketReceived = true;

    if(result == true){
        //ui.tabSystemInfo->setEnabled(true);
        ui.tabRemoteManagement->setEnabled(true);
        ui.groupBoxBasicSettings->setEnabled(true);
        ui.groupBoxOtherSettings->setEnabled(true);
        ui.groupBoxRemoteConsole->setEnabled(true);

        ui.toolButtonRequestSystemInfo->setEnabled(true);
        ui.toolButtonRescanSystemInfo->setEnabled(true);

        m_fileManagementWidget->setPeerSocket(m_peerSocket);
        ui.tabFileManagement->setEnabled(true);

        if(!message.trimmed().isEmpty()){
            QMessageBox::warning(this, tr("Warning"), message);
        }
    }else{
        //ui.tabSystemInfo->setEnabled(false);
        ui.tabRemoteManagement->setEnabled(false);
        ui.groupBoxBasicSettings->setEnabled(false);
        ui.groupBoxOtherSettings->setEnabled(false);
        ui.groupBoxRemoteConsole->setEnabled(false);

        ui.toolButtonRequestSystemInfo->setEnabled(false);
        ui.toolButtonRescanSystemInfo->setEnabled(false);
        ui.toolButtonVerify->setEnabled(true);

        //m_fileManagementWidget->setPeerSocket(m_peerSocket);
        ui.tabFileManagement->setEnabled(false);

        QMessageBox::critical(this, tr("Connection Error"), message);
    }




}

void SystemManagementWidget::requestConnectionToClientTimeout(){

    if(!clientResponseAdminConnectionResultPacketReceived){
        QMessageBox::critical(this, tr("Error"), tr("Timeout! No response received from client!"));
        ui.toolButtonVerify->setEnabled(true);
    }

}

void SystemManagementWidget::clientMessageReceived(const QString &computerName, const QString &message, quint8 clientMessageType){

    if(computerName != this->m_computerName){
        return;
    }

    QString msg = QString(tr("<p>Message From Computer <b>%1</b> :</p>").arg(computerName));
    msg += message;
    switch(clientMessageType){
    case quint8(MS::MSG_Information):
        QMessageBox::information(this, tr("Message"), msg);
        break;
    case quint8(MS::MSG_Warning):
        QMessageBox::warning(this, tr("Warning"), msg);
        break;
    case quint8(MS::MSG_Critical):
        QMessageBox::critical(this, tr("Error"), msg);
        break;
    default:
        QMessageBox::information(this, tr("Message"), msg);
    }



}


void SystemManagementWidget::clientResponseClientSummaryInfoPacketReceived(const QString &computerName, const QString &workgroupName, const QString &networkInfo, const QString &usersInfo, const QString &osInfo, bool usbsdEnabled, bool programesEnabled, const QString &admins, bool isJoinedToDomain, const QString &clientVersion){

    if(this->m_computerName.toLower() != computerName.toLower()){
        return;
    }

    m_users = usersInfo;
    m_usbsdEnabled = usbsdEnabled;
    m_programesEnabled = programesEnabled;
    m_administrators.clear();
    if(!admins.trimmed().isEmpty()){
        m_administrators = admins.split(",");
    }

    QStringList networkInfoList = networkInfo.split(",").at(0).split("/");
    //    m_peerIPAddress = QHostAddress(networkInfoList.at(0));
    //    if(localComputer){
    //        this->m_peerIPAddress = QHostAddress(QHostAddress::LocalHost);
    //    }
    m_peerMACAddress = networkInfoList.at(1);

    ui.lineEditUsers->setText(m_users);
    ui.lineEditUsers->setReadOnly(true);
    //    ui.lineEditIPAddress->setText(m_peerIPAddress.toString());
    ui.lineEditIPAddress->setText(networkInfoList.at(0));
    ui.lineEditIPAddress->setReadOnly(true);
    ui.lineEditMACAddress->setText(m_peerMACAddress);
    ui.lineEditMACAddress->setReadOnly(true);


    //    info->setWorkgroup(workgroupName);
    //    info->setNetwork(networkInfo);
    //    info->setUsers(usersInfo);
    //    info->setOs(osInfo);
    //    info->setUsbSDEnabled(usbsdEnabled);
    //    info->setProgramsEnabled(programesEnabled);
    //    info->setAdministrators(admins);
    //    info->setClientVersion(clientVersion);


    //ui.toolButtonRequestSystemInfo->setEnabled(true);

    if(usbsdEnabled){
        ui.pushButtonUSBSD->setText(tr("Disable USB SD"));
    }else{

        ui.pushButtonUSBSD->setText(tr("Enable USB SD"));
    }
    ui.pushButtonUSBSD->setEnabled(true);

    if(programesEnabled){
        ui.pushButtonPrograms->setText(tr("Disable Programs"));
    }else{
        ui.pushButtonPrograms->setText(tr("Enable Programs"));
    }
    ui.pushButtonPrograms->setEnabled(true);


    if(!administratorsManagementMenu){
        administratorsManagementMenu = new QMenu(this);
        administratorsManagementMenu->addAction(ui.actionAddAdmin);
        administratorsManagementMenu->addAction(ui.actionDeleteAdmin);
    }
    ui.actionAddAdmin->setEnabled(!m_users.isEmpty());
    ui.actionDeleteAdmin->setEnabled(!m_administrators.isEmpty());

    ui.pushButtonAdminsManagement->setMenu(administratorsManagementMenu);
    ui.pushButtonAdminsManagement->setEnabled(true);

    //ui.pushButtonRenameComputer->setEnabled(true);
    ui.pushButtonDomain->setEnabled(true);
    m_isJoinedToDomain = isJoinedToDomain;
    if(m_isJoinedToDomain){
        ui.pushButtonAdminsManagement->hide();
        ui.pushButtonDomain->setText(tr("Unjoin The Domain"));
    }else{
        ui.pushButtonAdminsManagement->show();
        ui.pushButtonDomain->setText(tr("Join A Domain"));
    }


    //ui.tabSystemInfo->setEnabled(true);
    ui.toolButtonRequestSystemInfo->setEnabled(true);
    ui.toolButtonRescanSystemInfo->setEnabled(true);
    //ui.toolButtonSaveAs->setEnabled(false);

    ui.tabRemoteManagement->setEnabled(true);

    ui.groupBoxBasicSettings->setEnabled(true);
    ui.groupBoxOtherSettings->setEnabled(true);
    //ui.groupBoxSettings->show();

    ui.groupBoxRemoteConsole->setEnabled(true);


}


void SystemManagementWidget::clientDetailedInfoPacketReceived(const QString &computerName, const QString &clientInfo){

    //qWarning()<<"Client Info from "<< computerName;
    //qWarning()<<clientInfo;

    if(this->m_computerName != computerName){
        return;
    }

    resetSystemInfo();

    QDir::setCurrent(QDir::tempPath());
    QString clientInfoFilePath = QString("./%1.ini").arg(computerName);

    QFile file(clientInfoFilePath);
    if(!file.open(QIODevice::WriteOnly | QFile::Truncate | QIODevice::Text)){
        QMessageBox::critical(this, QString(tr("Error")), QString(tr("Can not write client info file '")+clientInfoFilePath+tr("'!")));
        return;
    }
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << clientInfo;
    file.flush();
    file.close();

    QSettings systemInfo(clientInfoFilePath, QSettings::IniFormat, this);
    systemInfo.setIniCodec("UTF-8");

    if(!QFile(clientInfoFilePath).exists()){
        QMessageBox::critical(this, QString(tr("Error")), QString(tr("Client Info File '")+clientInfoFilePath+tr("' Missing!")));

        //slotResetStatusBar(false);
        //statusBar()->showMessage(tr("Error! Client Info File '%1' missing!").arg(clientInfoFilePath));
        return;
    }


    systemInfo.beginGroup("DevicesInfo");

    QString cpu = systemInfo.value("CPU").toString();
    QString memory = systemInfo.value("Memory").toString();
    QString motherboardName = systemInfo.value("MotherboardName").toString();
    QString dmiUUID = systemInfo.value("DMIUUID").toString();
    QString chipset = systemInfo.value("Chipset").toString();
    QString video = systemInfo.value("Video").toString();
    QString monitor = systemInfo.value("Monitor").toString();
    QString audio = systemInfo.value("Audio").toString();
    QString partitionsTotalSize = systemInfo.value("PartitionsTotalSize").toString();

    QString adapter1Name = systemInfo.value("Adapter1Name").toString();
    QString adapter1HDAddress = systemInfo.value("Adapter1HDAddress").toString();
    QString adapter1IPAddress = systemInfo.value("Adapter1IPAddress").toString();
    //network1Info<<adapter1Name<<adapter1HDAddress<<adapter1IPAddress;

    QString adapter2Name = systemInfo.value("Adapter2Name").toString();
    QString adapter2HDAddress = systemInfo.value("Adapter2HDAddress").toString();
    QString adapter2IPAddress = systemInfo.value("Adapter2IPAddress").toString();
    //network2Info<<adapter2Name<<adapter2HDAddress<<adapter2IPAddress;

    systemInfo.endGroup();

    ui.cpuLineEdit->setText(cpu);
    ui.memoryLineEdit->setText(memory);
    ui.motherboardLineEdit->setText(motherboardName);
    ui.dmiUUIDLineEdit->setText(dmiUUID);
    ui.chipsetLineEdit->setText(chipset);
    ui.videoCardLineEdit->setText(video);
    ui.monitorLineEdit->setText(monitor);
    ui.audioLineEdit->setText(audio);

    ui.adapter1NameLineEdit->setText(adapter1Name);
    ui.adapter1HDAddressLineEdit->setText(adapter1HDAddress);
    ui.adapter1IPAddressLineEdit->setText(adapter1IPAddress);
    ui.adapter2NameLineEdit->setText(adapter2Name);
    ui.adapter2HDAddressLineEdit->setText(adapter2HDAddress);
    ui.adapter2IPAddressLineEdit->setText(adapter2IPAddress);

    ui.devicesInfoGroupBox->setEnabled(true);


    systemInfo.beginGroup("OSInfo");
    QString os = systemInfo.value("OS").toString();
    QString installationDate = systemInfo.value("InstallationDate").toString();
    QString workgroup = systemInfo.value("Workgroup").toString();
    //QString computerName = systemInfo.value("ComputerName").toString();
    QString windowsDir = systemInfo.value("WindowsDir").toString();


    ui.osVersionLineEdit->setText(os);
    ui.installationDateLineEdit->setText(installationDate);
    ui.computerNameLineEdit->setText(computerName);
    ui.workgroupLineEdit->setText(workgroup);

    ui.logicalDrivesComboBox->clear();
    QStringList drivesInfo;
    for (int i = 1; i < 10; i++) {
        QString logicalDrivesKey,driveInfo;
        logicalDrivesKey = QString("Partition" + QString::number(i));
        driveInfo = systemInfo.value(logicalDrivesKey).toString();
        if (driveInfo.isEmpty()) {
            continue;
        }

        drivesInfo.append(driveInfo);
        ui.logicalDrivesComboBox->addItem(driveInfo);
    }
    systemInfo.endGroup();

    systemInfo.beginGroup("Users");
    ui.comboBoxSystemUsers->clear();
    QStringList usersInfo;
    for(int j = 0; j < 10; j++){
        QString userKey, userInfo;
        userKey = QString("User" + QString::number(j));
        userInfo = systemInfo.value(userKey).toString();
        if(userInfo.isEmpty()){
            continue;
        }
        usersInfo.append(userInfo);
        ui.comboBoxSystemUsers->addItem(userInfo);
    }
    systemInfo.endGroup();

    ui.osGroupBox->setEnabled(true);


    systemInfo.beginGroup("Other");
    QString creationTime = systemInfo.value("CreationTime").toString();
    ui.labelReportCreationTime->setText("Report Creation Time: " + creationTime);
    //ui.labelReportCreationTime->show();
    systemInfo.endGroup();

    systemInfo.beginGroup("InstalledSoftwareInfo");
    QStringList infoList;
    for(int k = 1; k < 400; k++){
        QString info = systemInfo.value(QString::number(k)).toString();
        if(info.isEmpty()){break;}
        infoList.append(info);
    }
    int rowCount = infoList.size();
    ui.tableWidgetSoftware->setRowCount(rowCount);
    for(int m=0; m<rowCount; m++){
        QStringList info = infoList.at(m).split(" | ");
        if(info.size() != 5){break;}
        for(int n=0; n<5; n++){
            ui.tableWidgetSoftware->setItem(m, n, new QTableWidgetItem(info.at(n)));
        }
    }
    systemInfo.endGroup();


    ui.toolButtonRequestSystemInfo->setEnabled(true);
    ui.toolButtonRescanSystemInfo->setEnabled(true);
    ui.toolButtonSaveAs->setEnabled(true);


    //slotResetStatusBar(false);
    //statusBar()->showMessage(tr("Done. Press 'Ctrl+S' to upload the data to server!"));

}

void SystemManagementWidget::requestClientInfoTimeout(){

    if(!ui.toolButtonRequestSystemInfo->isEnabled()){
        QMessageBox::critical(this, tr("Error"), tr("Timeout! No response from client!"));
        ui.toolButtonRequestSystemInfo->setEnabled(true);
    }

}

void SystemManagementWidget::clientResponseRemoteConsoleStatusPacketReceived(const QString &computerName, bool running, const QString &extraMessage, quint8 messageType){


    if(computerName != this->m_computerName){
        return;
    }

    ui.textBrowserRemoteApplicationOutput->append(extraMessage);

    if(running){
        remoteConsoleRunning = true;
        //        ui.horizontalLayoutRemoteApplication->setEnabled(false);
        //        ui.horizontalLayoutCommand->setEnabled(true);
        ui.comboBoxRemoteApplicationPath->setEnabled(false);
        ui.toolButtonRunRemoteApplication->setEnabled(true);
        ui.toolButtonRunRemoteApplication->setIcon(QIcon(":/icon/resources/images/stop.png"));

        ui.comboBoxCommand->setFocus();
        ui.toolButtonSendCommand->setEnabled(true);

    }else{
        remoteConsoleRunning = false;
        //        ui.horizontalLayoutRemoteApplication->setEnabled(true);
        //        ui.horizontalLayoutCommand->setEnabled(false);
        ui.comboBoxRemoteApplicationPath->setEnabled(true);
        ui.comboBoxRemoteApplicationPath->setFocus();
        ui.toolButtonRunRemoteApplication->setEnabled(true);
        ui.toolButtonRunRemoteApplication->setIcon(QIcon(":/icon/resources/images/start.png"));
        
        ui.toolButtonSendCommand->setEnabled(false);

        //QMessageBox::critical(this, tr("Error"), tr("<p>Message From Computer <b>%1</b> :</p>").arg(computerName) + extraMessage);

    }

    if(!extraMessage.trimmed().isEmpty()){
        clientMessageReceived(computerName, extraMessage, messageType);
    }

}

void SystemManagementWidget::remoteConsoleCMDResultFromClientPacketReceived(const QString &computerName, const QString &result){


    if(computerName != this->m_computerName){
        return;
    }

    ui.textBrowserRemoteApplicationOutput->append(result);


}

void SystemManagementWidget::requestRemoteConsoleTimeout(){

    if(!remoteConsoleRunning){
        QMessageBox::critical(this, tr("Error"), tr("Timeout! No response from client!"));

        ui.comboBoxRemoteApplicationPath->setEnabled(true);
        ui.toolButtonRunRemoteApplication->setEnabled(true);
    }

}

void SystemManagementWidget::userResponseRemoteAssistancePacketReceived(const QString &userName, const QString &computerName, bool accept){

    if(!accept){
        QMessageBox::critical(this, tr("Error"), tr("Remote Assistance Rejected By %1!").arg(userName+"@"+computerName));
    }
}

void SystemManagementWidget::peerDisconnected(int socketID){

    if(socketID != m_peerSocket){
        return;
    }

    peerDisconnected(m_aboutToCloseSocket);

}

void SystemManagementWidget::peerDisconnected(bool normalClose){
    qDebug()<<"--SystemManagementWidget::peerDisconnected(...) "<<" normalClose:"<<normalClose;

    m_peerSocket = INVALID_SOCK_ID;

    //ui.tabSystemInfo->setEnabled(false);
    ui.toolButtonRequestSystemInfo->setEnabled(false);
    ui.toolButtonRescanSystemInfo->setEnabled(false);
    if(ui.osVersionLineEdit->text().trimmed().isEmpty()){
        ui.toolButtonSaveAs->setEnabled(false);
    }

    ui.tabRemoteManagement->setEnabled(false);
    ui.toolButtonVerify->setEnabled(true);

    m_fileManagementWidget->peerDisconnected(normalClose);
    m_fileManagementWidget->setPeerSocket(INVALID_SOCK_ID);
    ui.tabFileManagement->setEnabled(false);

    if(!normalClose){
        QMessageBox::critical(this, tr("Error"), QString("ERROR! Peer %1 Closed Unexpectedly!").arg(m_peerIPAddress.toString()));
    }else{
        QMessageBox::warning(this, tr("Warning"), QString("Warning! Peer %1 Closed!").arg(m_peerIPAddress.toString()));
    }



//    foreach (QByteArray fileMD5, filesList) {
//        m_fileManager->closeFile(fileMD5);
//    }
//    fileTXRequestList.clear();
//    filesList.clear();


}


bool SystemManagementWidget::verifyPrivilege(){

    return true;

    if(m_adminName == "hehui"){
        return true;
    }
    
    bool ok = false;
    do {
        QString text = QInputDialog::getText(this, tr("Privilege Required"),
                                             tr("Access Code:"), QLineEdit::NoEcho,
                                             "", &ok);
        if (ok && !text.isEmpty()){
            QString accessCodeString = "hehui";
            accessCodeString.append(QTime::currentTime().toString("hhmm"));
            if(text.toLower() == accessCodeString){
                return true;
            }
        }

        QMessageBox::critical(this, tr("Error"), tr("Incorrect Access Code!"));

    } while (ok);

    return false;

}

bool SystemManagementWidget::temporarilyAllowed(){

    QStringList items;
    items << tr("Provisional Licence") << tr("Perpetual License");

    bool ok = false;
    QString item = QInputDialog::getItem(this, tr("Select Licence Type"),
                                         tr("Licence Type:"), items, 0, false, &ok);
    if (!ok || item.isEmpty()){
        return true;
    }

    if(item == tr("Provisional Licence")){
        return true;
    }else{
        return false;
    }

}

void SystemManagementWidget::runProgrameAsAdmin(const QString &exeFilePath, const QString &parameters, bool show){

#ifdef Q_OS_WIN32
    if(!verifyPrivilege()){
        return;
    }
    WindowsManagement wm;
    bool ok = wm.runAs("administrator", ".", QString(WIN_ADMIN_PASSWORD), exeFilePath, parameters, show);
    if(!ok){
        QMessageBox::critical(this, tr("Error"), wm.lastError());
        return;
    }

    adminProcesses.append(QFileInfo(exeFilePath).fileName());
#endif

}

void SystemManagementWidget::resetSystemInfo(){


    ui.osVersionLineEdit->clear();
    ui.installationDateLineEdit->clear();
    ui.computerNameLineEdit->clear();
    ui.workgroupLineEdit->clear();
    ui.logicalDrivesComboBox->clear();
    ui.comboBoxSystemUsers->clear();

    ui.osGroupBox->setEnabled(false);


    ui.cpuLineEdit->clear();
    ui.memoryLineEdit->clear();
    ui.motherboardLineEdit->clear();
    ui.dmiUUIDLineEdit->clear();
    ui.chipsetLineEdit->clear();
    ui.videoCardLineEdit->clear();
    ui.monitorLineEdit->clear();
    ui.audioLineEdit->clear();

    ui.adapter1NameLineEdit->clear();
    ui.adapter1HDAddressLineEdit->clear();
    ui.adapter1IPAddressLineEdit->clear();
    ui.adapter2NameLineEdit->clear();
    ui.adapter2HDAddressLineEdit->clear();
    ui.adapter2IPAddressLineEdit->clear();

    ui.devicesInfoGroupBox->setEnabled(false);

    ui.tableWidgetSoftware->clearContents();


    ui.labelReportCreationTime->clear();
    ui.toolButtonSaveAs->setEnabled(false);


}














} //namespace HEHUI


