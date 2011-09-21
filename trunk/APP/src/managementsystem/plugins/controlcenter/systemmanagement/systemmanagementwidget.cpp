
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


namespace HEHUI {


SystemManagementWidget::SystemManagementWidget(const QString &adminName, ControlCenterPacketsParser *parser, const QString &computerName, const QString &users, const QString &peerIPAddress, const QString &peerMACAddress, bool usbsdEnabled, bool programesEnabled, const QString &admins, QWidget *parent)
    : QWidget(parent), m_adminName(adminName), m_computerName(computerName), m_users(users), m_peerIPAddress(QHostAddress(peerIPAddress)), m_peerMACAddress(peerMACAddress), m_usbsdEnabled(usbsdEnabled), m_programesEnabled(programesEnabled)
{
    ui.setupUi(this);

    ui.lineEditComputerName->setText(computerName);
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

    m_winDirPath = "";


#ifdef Q_OS_WIN32

    if(localComputer){
        ui.groupBoxAdministrationTools->show();
    }else{
        int index = ui.tabWidget->indexOf(ui.tabLocalManagement);
        ui.tabWidget->removeTab(index);

        ui.tabLocalManagement->setEnabled(false);
        //            ui.groupBoxAdministrationTools->setEnabled(false);;
        //            ui.groupBoxAdministrationTools->hide();
    }

    WindowsManagement wm;
    m_winDirPath = wm.getEnvironmentVariable("windir");



#else
    ui.pushButtonShowAdmin->setEnabled(false);
    ui.pushButtonShowAdmin->hide();

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


    ui.groupBoxSettings->setEnabled(false);
    //ui.groupBoxSettings->hide();

    ui.horizontalLayoutCommand->setEnabled(false);


    ui.tabSystemInfo->setEnabled(false);
    ui.tabRemoteManagement->setEnabled(false);


    setControlCenterPacketsParser(parser);
    //    connect(controlCenterPacketsParser, SIGNAL(signalClientResponseAdminConnectionResultPacketReceived(const QString &, bool, const QString &)), this, SLOT(processClientResponseAdminConnectionResultPacket(const QString &, bool, const QString &)));
    //    connect(controlCenterPacketsParser, SIGNAL(signalClientMessagePacketReceived(const QString &, const QString &)), this, SLOT(clientMessageReceived(const QString &, const QString &)));
    //    connect(controlCenterPacketsParser, SIGNAL(signalClientResponseClientSummaryInfoPacketReceived(const QString&, const QString&, const QString&, const QString&, const QString&, bool, bool, const QString&, const QString&)), this, SLOT(clientResponseClientSummaryInfoPacketReceived(const QString&, const QString&, const QString&, const QString&, const QString&, bool, bool, const QString&, const QString&)));

    //    connect(controlCenterPacketsParser, SIGNAL(signalClientResponseClientInfoPacketReceived(const QString &, const QString &)), this, SLOT(clientInfoPacketReceived(const QString &, const QString &)));



    clientResponseAdminConnectionResultPacketReceived = false;

    remoteConsoleRunning = false;

    //    requestConnectionToClientTimer = 0;



    //    info = new ClientInfo(computerName, this);
    //    info->setUsers(usersInfo);
    //    info->setUsbSDEnabled(usbsdEnabled);
    //    info->setProgramsEnabled(programesEnabled);



}

SystemManagementWidget::~SystemManagementWidget()
{

    qDebug()<<"~SystemManagementWidget()";



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

#ifdef Q_OS_WIN32
    if(!adminProcesses.isEmpty()){
        QString msg = tr("<p>Please make sure that these processes run as admin are closed! Or that raises the issue of security!</p>");
        msg += adminProcesses.join(", ");
        QMessageBox::warning(this, tr("Warning"), msg);
    }

#endif

    controlCenterPacketsParser->disconnect(this);

    event->accept();

}

void SystemManagementWidget::setControlCenterPacketsParser(ControlCenterPacketsParser *parser){

    if(!parser){
        ui.toolButtonVerify->setEnabled(false);
        return;
    }

    this->controlCenterPacketsParser = parser;
    connect(controlCenterPacketsParser, SIGNAL(signalClientResponseAdminConnectionResultPacketReceived(const QString &, bool, const QString &)), this, SLOT(processClientResponseAdminConnectionResultPacket(const QString &, bool, const QString &)));
    connect(controlCenterPacketsParser, SIGNAL(signalClientMessagePacketReceived(const QString &, const QString &)), this, SLOT(clientMessageReceived(const QString &, const QString &)));
    connect(controlCenterPacketsParser, SIGNAL(signalClientResponseClientSummaryInfoPacketReceived(const QString&, const QString&, const QString&, const QString&, const QString&, bool, bool, const QString&, const QString&)), this, SLOT(clientResponseClientSummaryInfoPacketReceived(const QString&, const QString&, const QString&, const QString&, const QString&, bool, bool, const QString&, const QString&)));

    connect(controlCenterPacketsParser, SIGNAL(signalClientResponseClientDetailedInfoPacketReceived(const QString &, const QString &)), this, SLOT(clientDetailedInfoPacketReceived(const QString &, const QString &)));

    connect(controlCenterPacketsParser, SIGNAL(signalClientResponseRemoteConsoleStatusPacketReceived(const QString &, bool, const QString &)), this, SLOT(clientResponseRemoteConsoleStatusPacketReceived(const QString &, bool, const QString &)));
    connect(controlCenterPacketsParser, SIGNAL(signalRemoteConsoleCMDResultFromClientPacketReceived(const QString &, const QString &)), this, SLOT(remoteConsoleCMDResultFromClientPacketReceived(const QString &, const QString &)));

    connect(controlCenterPacketsParser, SIGNAL(signalUserResponseRemoteAssistancePacketReceived(const QString &, const QString &, bool)), this, SLOT(userResponseRemoteAssistancePacketReceived(const QString &, const QString &, bool)));
    

    ui.toolButtonVerify->setEnabled(true);

}

void SystemManagementWidget::on_toolButtonVerify_clicked(){

    if(!verifyPrivilege()){
        return;
    }

    clientResponseAdminConnectionResultPacketReceived = false;

    ui.toolButtonVerify->setEnabled(false);

    //    QHostAddress address = QHostAddress(peerAddress);
    //    if(localComputer){
    //        address = QHostAddress(QHostAddress::LocalHost);
    //    }


    controlCenterPacketsParser->sendAdminRequestConnectionToClientPacket(m_peerIPAddress, RUDP_LISTENING_PORT, this->m_computerName, this->m_users);


    //    if(!requestConnectionToClientTimer){
    //        requestConnectionToClientTimer =new QTimer(this);
    //        requestConnectionToClientTimer->setInterval(5000);
    //        requestConnectionToClientTimer->setSingleShot(true);
    //        connect(requestConnectionToClientTimer, SIGNAL(timeout()), this, SLOT(requestConnectionToClientTimeout()));
    //    }
    //    requestConnectionToClientTimer->start();

    QTimer::singleShot(30000, this, SLOT(requestConnectionToClientTimeout()));


}

void SystemManagementWidget::on_pushButtonUSBSD_clicked(){

    if(!verifyPrivilege()){
        return;
    }

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

    controlCenterPacketsParser->sendSetupUSBSDPacket(m_peerIPAddress, RUDP_LISTENING_PORT, m_computerName, m_users, !m_usbsdEnabled, m_temporarilyAllowed, m_adminName);

    ui.pushButtonUSBSD->setEnabled(false);

}

void SystemManagementWidget::on_pushButtonPrograms_clicked(){

    if(!verifyPrivilege()){
        return;
    }

    QString text = tr("Do you really want to <font color = 'red'><b>%1</b></font> the Programes on the computer?").arg(m_programesEnabled?tr("disable"):tr("enable"));

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
    if(!m_programesEnabled){
        m_temporarilyAllowed = temporarilyAllowed();
    }

    controlCenterPacketsParser->sendSetupProgramesPacket(m_peerIPAddress, RUDP_LISTENING_PORT, m_computerName, m_users, !m_programesEnabled, m_temporarilyAllowed, m_adminName);

    ui.pushButtonPrograms->setEnabled(false);

}

void SystemManagementWidget::on_pushButtonShowAdmin_clicked(){

    //    QHostAddress address = QHostAddress(peerAddress);
    //    if(localComputer){
    //        address = QHostAddress(QHostAddress::LocalHost);
    //    }

    controlCenterPacketsParser->sendShowAdminPacket(m_peerIPAddress, RUDP_LISTENING_PORT, m_computerName, m_users, true);

    ui.pushButtonShowAdmin->setEnabled(false);

}

void SystemManagementWidget::on_pushButtonRemoteAssistance_clicked(){
    
    if(!verifyPrivilege()){
        return;
    }
    
    //ui.pushButtonRemoteAssistance->setEnabled(false);
    emit requestRemoteAssistance();
    
    controlCenterPacketsParser->sendRemoteAssistancePacket(m_peerIPAddress, RUDP_LISTENING_PORT, m_computerName, m_adminName);
    

}

void SystemManagementWidget::on_actionAddAdmin_triggered(){

    if(!verifyPrivilege()){
        return;
    }

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

    //    QHostAddress address = QHostAddress(peerAddress);
    //    if(localComputer){
    //        address = QHostAddress(QHostAddress::LocalHost);
    //    }

    controlCenterPacketsParser->sendModifyAdminGroupUserPacket(m_peerIPAddress, RUDP_LISTENING_PORT, m_computerName, item, true, m_adminName);
    ui.pushButtonAdminsManagement->setEnabled(false);


}

void SystemManagementWidget::on_actionDeleteAdmin_triggered(){

    if(!verifyPrivilege()){
        return;
    }

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

    controlCenterPacketsParser->sendModifyAdminGroupUserPacket(m_peerIPAddress, RUDP_LISTENING_PORT, m_computerName, item, false, m_adminName);
    ui.pushButtonAdminsManagement->setEnabled(false);


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

void SystemManagementWidget::on_toolButtonRequestSystemInfo_clicked(){

    if(!verifyPrivilege()){
        return;
    }

    //    QHostAddress address = QHostAddress(peerAddress);
    //    if(localComputer){
    //        address = QHostAddress(QHostAddress::LocalHost);
    //    }

    controlCenterPacketsParser->sendRequestClientDetailedInfoPacket(m_peerIPAddress, RUDP_LISTENING_PORT, m_computerName, false);

    QTimer::singleShot(60000, this, SLOT(requestClientInfoTimeout()));

    ui.toolButtonRequestSystemInfo->setEnabled(false);
    ui.toolButtonRescanSystemInfo->setEnabled(false);
    ui.toolButtonSaveAs->setEnabled(false);

    ui.osGroupBox->setEnabled(false);
    ui.devicesInfoGroupBox->setEnabled(false);

}

void SystemManagementWidget::on_toolButtonRescanSystemInfo_clicked(){

    if(!verifyPrivilege()){
        return;
    }

    //    QHostAddress address = QHostAddress(peerAddress);
    //    if(localComputer){
    //        address = QHostAddress(QHostAddress::LocalHost);
    //    }

    controlCenterPacketsParser->sendRequestClientDetailedInfoPacket(m_peerIPAddress, RUDP_LISTENING_PORT, m_computerName, true);


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

    if(!verifyPrivilege()){
        return;
    }
    
    if(remoteConsoleRunning){
        int rep = QMessageBox::question(this, tr("Confirm"), tr("Do you really want to terminate the process?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if(rep == QMessageBox::Yes){
            controlCenterPacketsParser->sendAdminRequestRemoteConsolePacket(m_peerIPAddress, RUDP_LISTENING_PORT, m_computerName, "", m_adminName, false);
            //ui.toolButtonRunRemoteApplication->setEnabled(false);
        }

    }else{
        QString remoteAPPPath = ui.comboBoxRemoteApplicationPath->currentText();
        if(!remoteAPPPath.trimmed().isEmpty()){
            controlCenterPacketsParser->sendAdminRequestRemoteConsolePacket(m_peerIPAddress, RUDP_LISTENING_PORT, m_computerName, remoteAPPPath, m_adminName, true);
        }

        ui.comboBoxRemoteApplicationPath->setEnabled(false);
        ui.toolButtonRunRemoteApplication->setEnabled(false);
        

        ui.toolButtonSendCommand->setEnabled(false);
    }


    

    //QTimer::singleShot(10000, this, SLOT(requestRemoteConsoleTimeout()));




}

void SystemManagementWidget::on_toolButtonSendCommand_clicked(){

    if(!verifyPrivilege()){
        return;
    }

    QString cmd = ui.comboBoxCommand->currentText();
    controlCenterPacketsParser->sendRemoteConsoleCMDFromAdminPacket(m_peerIPAddress, RUDP_LISTENING_PORT, m_computerName, cmd);

    ui.comboBoxCommand->insertItem(0, cmd);

    ui.comboBoxCommand->setEditText("");
    ui.comboBoxCommand->setFocus();

    //ui.textBrowserRemoteApplicationOutput->append(cmd + "\n");

}


void SystemManagementWidget::processClientResponseAdminConnectionResultPacket(const QString &computerName, bool result, const QString &message){
    //    qWarning()<<"SystemManagementWidget::processClientResponseVerifyInfoResultPacket:"<<"computerName:"<<computerName<<" result:"<<result;

    if(computerName != this->m_computerName){
        return;
    }

    clientResponseAdminConnectionResultPacketReceived = true;

    if(result == true){
        //ui.groupBoxSettings->setEnabled(true);
        //ui.groupBoxSettings->show();
        ui.tabSystemInfo->setEnabled(true);
        if(!message.trimmed().isEmpty()){
            QMessageBox::warning(this, tr("Warning"), message);
        }
    }else{
        ui.tabSystemInfo->setEnabled(false);
        ui.tabRemoteManagement->setEnabled(false);
        ui.toolButtonVerify->setEnabled(true);

        QMessageBox::critical(this, tr("Connection Error"), message);
    }




}

void SystemManagementWidget::requestConnectionToClientTimeout(){

    if(!clientResponseAdminConnectionResultPacketReceived){
        QMessageBox::critical(this, tr("Error"), tr("Timeout! No response from client!"));
        ui.toolButtonVerify->setEnabled(true);
    }
}

void SystemManagementWidget::clientMessageReceived(const QString &computerName, const QString &message){

    if(computerName != this->m_computerName){
        return;
    }

    QString msg = QString(tr("<p>Message From Computer <b>%1</b> :</p>").arg(computerName));
    msg += message;
    QMessageBox::information(this, tr("Message"), msg);



}


void SystemManagementWidget::clientResponseClientSummaryInfoPacketReceived(const QString &computerName, const QString &workgroupName, const QString &networkInfo, const QString &usersInfo, const QString &osInfo, bool usbsdEnabled, bool programesEnabled, const QString &admins, const QString &clientVersion){

    if(this->m_computerName.toLower() != computerName.toLower()){
        return;
    }


    m_users = usersInfo;
    m_usbsdEnabled = usbsdEnabled;
    m_programesEnabled = programesEnabled;
    m_administrators = admins.split(",");;

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


    ui.toolButtonRequestSystemInfo->setEnabled(true);

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


    if(!admins.trimmed().isEmpty()){
        m_administrators = admins.split(",");
    }
    administratorsManagementMenu = new QMenu(this);
    if(!m_users.isEmpty()){
        administratorsManagementMenu->addAction(ui.actionAddAdmin);
    }
    if(!m_administrators.isEmpty()){
        administratorsManagementMenu->addAction(ui.actionDeleteAdmin);
    }
    ui.pushButtonAdminsManagement->setMenu(administratorsManagementMenu);
    ui.pushButtonAdminsManagement->setEnabled(true);


    ui.tabSystemInfo->setEnabled(true);
    ui.tabRemoteManagement->setEnabled(true);

    ui.groupBoxSettings->setEnabled(true);
    //ui.groupBoxSettings->show();

    ui.groupBoxRemoteConsole->setEnabled(true);





}


void SystemManagementWidget::clientDetailedInfoPacketReceived(const QString &computerName, const QString &clientInfo){

    //qWarning()<<"Client Info from "<< computerName;
    //qWarning()<<clientInfo;

    if(this->m_computerName != computerName){
        return;
    }

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

void SystemManagementWidget::clientResponseRemoteConsoleStatusPacketReceived(const QString &computerName, bool running, const QString &extraMessage){


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


        if(!extraMessage.trimmed().isEmpty()){
            clientMessageReceived(computerName, extraMessage);
        }



    }else{
        remoteConsoleRunning = false;
        //        ui.horizontalLayoutRemoteApplication->setEnabled(true);
        //        ui.horizontalLayoutCommand->setEnabled(false);
        ui.comboBoxRemoteApplicationPath->setEnabled(true);
        ui.comboBoxRemoteApplicationPath->setFocus();
        ui.toolButtonRunRemoteApplication->setEnabled(true);
        ui.toolButtonRunRemoteApplication->setIcon(QIcon(":/icon/resources/images/start.png"));
        

        ui.toolButtonSendCommand->setEnabled(false);

        QMessageBox::critical(this, tr("Error"), tr("<p>Message From Computer <b>%1</b> :</p>").arg(computerName) + extraMessage);


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

bool SystemManagementWidget::verifyPrivilege(){

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
    bool ok = wm.runAs("administrator", QString(WIN_ADMIN_PASSWORD), exeFilePath, parameters, show);
    if(!ok){
        QMessageBox::critical(this, tr("Error"), wm.lastError());
        return;
    }

    adminProcesses.append(QFileInfo(exeFilePath).fileName());
#endif

}














} //namespace HEHUI


