


#include <QObject>
#include <QtSql>
#include <QString>
#include <QDir>
#include <QDebug>
#include <QCloseEvent>

#include <QMessageBox>
#include <QInputDialog>
#include <QMenu>
#include <QHostInfo>
//#include <QDesktopWidget>



#include "constants.h"
#include "controlcenter.h"
#include "announcement/announcement.h"


#include "HHSharedCore/hglobal_core.h"
#include "HHSharedCore/hutilities.h"
#include "HHSharedGUI/hdatabaseconnecter.h"
#include "HHSharedGUI/hdataoutputdialog.h"
#include "HHSharedNetwork/hnetworkutilities.h"


namespace HEHUI {


bool ControlCenter::running = false;

ControlCenter::ControlCenter(const QString &adminName, QWidget *parent)
    : QMainWindow(parent), m_adminName(adminName)
{
    ui.setupUi(this);

    //setWindowFlags(Qt::Dialog);


    localSystemManagementWidget = 0;
    localComputerName = QHostInfo::localHostName().toLower();



    //databaseConnectionName = QString(REMOTE_SITOY_COMPUTERS_DB_CONNECTION_NAME) + "-ControlCenter";
    databaseConnectionName = QString(DB_CONNECTION_NAME);
    queryModel = new QSqlQueryModel(this);

    clientInfoModel = new ClientInfoModel(this);
    //proxyModel = new QSortFilterProxyModel(this);
    proxyModel = new ClientInfoSortFilterProxyModel(this);
    proxyModel->setSourceModel(queryModel);
    proxyModel->setDynamicSortFilter(true);

    ui.tableViewClientList->setModel(proxyModel);


    connect(ui.toolButtonManage, SIGNAL(clicked()), this, SLOT(slotRemoteManagement()));

    connect(ui.actionQueryDatabase, SIGNAL(triggered()), this, SLOT(slotQueryDatabase()));
    connect(ui.actionSearchNetwork, SIGNAL(triggered()), this, SLOT(slotSearchNetwork()));
    connect(ui.actionFilter, SIGNAL(triggered()), this, SLOT(filter()));


    connect(ui.actionUpdatePassword, SIGNAL(triggered()), this, SLOT(slotUpdateUserLogonPassword()));
    connect(ui.actionInformNewPassword, SIGNAL(triggered()), this, SLOT(slotInformUserNewLogonPassword()));
    connect(ui.actionAnnouncement, SIGNAL(triggered()), this, SLOT(slotSendAnnouncement()));


    connect(ui.tableViewClientList, SIGNAL(clicked(const QModelIndex &)), this, SLOT(slotShowClientInfo(const QModelIndex &)));
    //connect(ui.tableViewClientList->selectionModel(), SIGNAL(currentRowChanged(QModelIndex &,QModelIndex &)), this, SLOT(slotShowUserInfo(const QModelIndex &)));
    connect(ui.tableViewClientList, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(slotRemoteManagement()));

    connect(ui.tableViewClientList, SIGNAL(customContextMenuRequested(QPoint)), this,
            SLOT(slotShowCustomContextMenu(QPoint)));

    connect(ui.actionRemoteDesktop, SIGNAL(triggered()), this, SLOT(slotRemoteDesktop()));


    connect(ui.actionExport, SIGNAL(triggered()), this, SLOT(slotExportQueryResult()));
    connect(ui.actionPrint, SIGNAL(triggered()), this, SLOT(slotPrintQueryResult()));




    //ui.lineEditComputerName->setText(localComputerName);
    //QString currentUserNameOfOS = Utilities::currentUserNameOfOS();
    //if(currentUserNameOfOS.toLower() != "administrator"){
    //    ui.lineEditUserName->setText(currentUserNameOfOS);
    //}

    ui.lineEditUserName->setFocus();


    slotInitTabWidget();




    ui.lineEditComputerName->setFocus();

    this->installEventFilter(this);

    running = true;


    resourcesManager = ResourcesManagerInstance::instance();
    controlCenterPacketsParser = 0;

    m_networkReady = false;
    m_udpServer = 0;
    m_localUDPListeningPort = IP_MULTICAST_GROUP_PORT + 10;

    m_rtp = 0;
    m_localRTPListeningPort = UDT_LISTENING_PORT + 10;
    m_socketConnectedToServer = INVALID_SOCK_ID;


    startNetwork();

    m_administrators = "";

    vncProcess = 0;
    
    searchClientsMenu = new QMenu();
    searchClientsMenu->addAction(ui.actionQueryDatabase);
    searchClientsMenu->addAction(ui.actionSearchNetwork);
    ui.toolButtonQuery->setMenu(searchClientsMenu);
    ui.toolButtonQuery->setDefaultAction(ui.actionQueryDatabase);

    ui.toolButtonfilter->setDefaultAction(ui.actionFilter);
    
    updatePasswordMenu = new QMenu;
    updatePasswordMenu->addAction(ui.actionInformNewPassword);
    updatePasswordMenu->addAction(ui.actionUpdatePassword);
    ui.toolButtonUpdatePassword->setMenu(updatePasswordMenu);
    ui.toolButtonUpdatePassword->addAction(ui.actionInformNewPassword);
    ui.toolButtonUpdatePassword->setDefaultAction(ui.actionInformNewPassword);
    
    ui.toolButtonAnnouncement->setDefaultAction(ui.actionAnnouncement);

    if(m_adminName != "kiwa" && m_adminName != "hehui"){
        ui.toolButtonUpdatePassword->setEnabled(false);
    }
    
    if(m_adminName != "hehui"){
        ui.toolButtonAnnouncement->setEnabled(false);
    }
    
    //    ui.toolButtonUpdatePassword->setEnabled(false);
    //    ui.toolButtonAnnouncement->setEnabled(false);
    
    
//    localUDPListeningPort = 0;
//    localRUDPListeningPort = 0;
    

//    m_serverInstanceID = 0;


    
}

ControlCenter::~ControlCenter()
{
    qDebug()<<"--ControlCenter::~ControlCenter()";

    if(vncProcess){
        vncProcess->terminate();
    }

    if(queryModel){
        queryModel->clear();
        delete queryModel;
        queryModel = 0;
    }

    QSqlDatabase db = QSqlDatabase::database(databaseConnectionName);
    if(db.isOpen()){
        db.close();
    }
    QSqlDatabase::removeDatabase(databaseConnectionName);

    delete controlCenterPacketsParser;
    controlCenterPacketsParser = 0;

//    resourcesManager->cleanInstance();
    delete resourcesManager;
    resourcesManager = 0;

    PacketHandlerBase::clean();

    running = false;   

}

bool ControlCenter::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::KeyRelease ) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *> (event);
        if(keyEvent->key() == Qt::Key_Up || keyEvent->key() == Qt::Key_Down){
            slotShowClientInfo(ui.tableViewClientList->currentIndex());
        }

        if(keyEvent->key() == Qt::Key_Escape){
            if(ui.lineEditComputerName->isReadOnly()){

                ui.lineEditComputerName->setReadOnly(false);
                ui.lineEditUserName->setReadOnly(false);

                ui.comboBoxWorkgroup->setEnabled(true);
                ui.comboBoxUSBSD->setEnabled(true);

                ui.lineEditMACAddress->setReadOnly(false);
                ui.lineEditIPAddress->setReadOnly(false);

                ui.comboBoxOSVersion->setEnabled(true);
                ui.comboBoxPrograms->setEnabled(true);

                //ui.lineEditComputerName->setFocus();
                ui.lineEditUserName->setFocus();

            }else{
                ui.lineEditComputerName->clear();
                ui.lineEditUserName->clear();

                ui.comboBoxWorkgroup->setCurrentIndex(0);
                ui.comboBoxUSBSD->setCurrentIndex(0);

                ui.lineEditMACAddress->clear();
                ui.lineEditIPAddress->clear();

                ui.comboBoxOSVersion->setCurrentIndex(0);
                ui.comboBoxPrograms->setCurrentIndex(0);

                //ui.lineEditComputerName->setFocus();
                ui.lineEditUserName->setFocus();

            }

            ui.toolButtonManage->setEnabled(false);
        }

        if(QApplication::keyboardModifiers() == Qt::ControlModifier && keyEvent->key() == Qt::Key_O){
            slotExportQueryResult();
        }
        if(QApplication::keyboardModifiers() == Qt::ControlModifier && keyEvent->key() == Qt::Key_P){
            slotPrintQueryResult();
        }

        return true;
    }else{
        // standard event processing
        return QObject::eventFilter(obj, event);
    }

}

void ControlCenter::languageChange() {
    retranslateUi();
}

void ControlCenter::closeEvent(QCloseEvent *e) {



    //关闭所有相关的TabPage
    //Close all related TabPage
    int tabPages = ui.tabWidget->count();
    for(int i = tabPages; i >= 0; --i){
        //SystemManagementWidget *systemManagementWidget = qobject_cast<SystemManagementWidget *>(ui.tabWidget->widget(i));
        //if(!systemManagementWidget){continue;}
        //systemManagementWidget->close();

        QWidget *widget = qobject_cast<QWidget *>(ui.tabWidget->widget(i));
        if(!widget){continue;}
        widget->close();

        ui.tabWidget->removeTab(i);
        //systemManagementWidget->deleteLater();
    }


    if(controlCenterPacketsParser){
        controlCenterPacketsParser->sendAdminOnlineStatusChangedPacket(m_socketConnectedToServer, localComputerName, m_adminName, false);
        m_rtp->closeSocket(m_socketConnectedToServer);
    }
    
    clientInfoModel->setClientList(QList<ClientInfo*>());
    clientInfoHash.clear();
    
//    clientSocketsHash.clear();


    e->accept();
    //deleteLater();

}


void ControlCenter::retranslateUi() {

    ui.retranslateUi(this);

}

void ControlCenter::slotInitTabWidget(){

    connect(ui.tabWidget, SIGNAL(currentChanged(int)),
            this, SLOT(slotTabPageChanged()));

    QTabBar *tabBar = ui.tabWidget->tabBar();
    QStyleOptionTab opt;
    if (tabBar) {
        opt.init(tabBar);
        opt.shape = tabBar->shape();
    }

    // workaround for sgi style
    //QPalette pal = palette();
    // pal.setColor(QPalette::Active, QPalette::Button, pal.color(QPalette::Active, QPalette::Window));
    // pal.setColor(QPalette::Disabled, QPalette::Button, pal.color(QPalette::Disabled, QPalette::Window));
    // pal.setColor(QPalette::Inactive, QPalette::Button, pal.color(QPalette::Inactive, QPalette::Window));

    QToolButton *newTabButton = new QToolButton(this);
    ui.tabWidget->setCornerWidget(newTabButton, Qt::TopLeftCorner);
    newTabButton->setCursor(Qt::ArrowCursor);
    newTabButton->setAutoRaise(true);
    newTabButton->setIcon(QIcon(":/resources/images/addtab.png"));
    QObject::connect(newTabButton, SIGNAL(clicked()), this, SLOT(slotNewTab()));
    newTabButton->setToolTip(tr("Add Page"));
    newTabButton->setEnabled(false);

    QToolButton *closeTabButton = new QToolButton(this);
    //closeTabButton->setPalette(pal);
    ui.tabWidget->setCornerWidget(closeTabButton, Qt::TopRightCorner);
    closeTabButton->setCursor(Qt::ArrowCursor);
    closeTabButton->setAutoRaise(true);
    closeTabButton->setIcon(QIcon(":/resources/images/closetab.png"));
    QObject::connect(closeTabButton, SIGNAL(clicked()), this, SLOT(slotcloseTab()));
    closeTabButton->setToolTip(tr("Close Page"));
    closeTabButton->setEnabled(false);


    //    QString localComputerName = QHostInfo::localHostName();
    //    SystemManagementWidget *systemManagementWidget = new SystemManagementWidget(m_adminName, 0, localComputerName, "", "127.0.0.1", "");
    //    ui.gridLayoutTabLocalComputer->addWidget(systemManagementWidget);
    //    //ui.tabWidget->addTab(systemManagementWidget, tr("Local Computer: %1").arg(localComputerName));
    //    //ui.tabWidget->setCurrentWidget(systemManagementWidget);
    //    ui.tabWidget->setCurrentWidget(ui.tabLocalComputer);



    localSystemManagementWidget = new SystemManagementWidget(0, 0, m_adminName, localComputerName, "", "127.0.0.1", "");
    localSystemManagementWidget->setParent(this);
    ui.tabWidget->addTab(localSystemManagementWidget, tr("Local Computer"));




}

void ControlCenter::slotTabPageChanged(){

    SystemManagementWidget *systemManagementWidget = qobject_cast<SystemManagementWidget *>(ui.tabWidget->currentWidget());
    if(systemManagementWidget){
        if(systemManagementWidget == localSystemManagementWidget){
            ui.tabWidget->cornerWidget(Qt::TopRightCorner)->setEnabled(false);
        }else{
            ui.tabWidget->cornerWidget(Qt::TopRightCorner)->setEnabled(true);
        }
    }else{
        ui.tabWidget->cornerWidget(Qt::TopRightCorner)->setEnabled(false);
    }


}

void ControlCenter::slotNewTab(){

    //    slotRemoteManagement();
    //    updateGeometry();

    //    QString localComputerName = QHostInfo::localHostName();
    //    SystemManagementWidget *systemManagementWidget = new SystemManagementWidget(m_adminName, controlCenterPacketsParser, localComputerName, "", "127.0.0.1", "");
    //    ui.tabWidget->addTab(systemManagementWidget, tr("Computer"));
    //    ui.tabWidget->setCurrentWidget(systemManagementWidget);

}

void ControlCenter::slotcloseTab(){

    //如果只有一页，则返回
    //   if(ui.tabWidget->count()==1){
    //        return;
    //   }

    //QWidget *page = qobject_cast<QWidget *>(ui.tabWidget->currentWidget());
    //ui.tabWidget->removeTab(ui.tabWidget->currentIndex());
    //delete page;

    SystemManagementWidget *systemManagementWidget = qobject_cast<SystemManagementWidget *>(ui.tabWidget->currentWidget());
    if(systemManagementWidget){
        if(systemManagementWidget == localSystemManagementWidget){
            return;
        }
        ui.tabWidget->removeTab(ui.tabWidget->currentIndex());
        systemManagementWidget->close();
        systemManagementWidget->deleteLater();
    }


    //如果只有一页，则关闭按钮不可用
    //ui.tabWidget->cornerWidget(Qt::TopRightCorner)->setEnabled(ui.tabWidget->count() > 1);


}

void ControlCenter::slotRemoteManagement(){

//    if(!m_networkReady && (computerName() != localComputerName)){
//        QMessageBox::critical(this, tr("Error"), tr("The Network is not available! Remote Management is not available!"));
//        return;
//    }


    QString targetComputerName = computerName();
    if(targetComputerName == localComputerName){
        ui.tabWidget->setCurrentWidget(localSystemManagementWidget);
        return;
    }

    int tabPages = ui.tabWidget->count();
    for(int i = tabPages; i >= 0; --i){
        if(ui.tabWidget->tabText(i) == targetComputerName){
            ui.tabWidget->setCurrentIndex(i);
            return;
        }
        //QWidget *widget = qobject_cast<QWidget *>(ui.tabWidget->widget(i));
        //if(!widget){continue;}

    }


    SystemManagementWidget *systemManagementWidget = new SystemManagementWidget(m_rtp, controlCenterPacketsParser, m_adminName, targetComputerName, userName(), ipAddress(), macAddress(), (usbsdEnabled() == "1"?true:false), (programesEnabled() == "1"?true:false), m_administrators, this);
    connect(systemManagementWidget, SIGNAL(requestRemoteAssistance()), this, SLOT(slotVNC()));
    
    ui.tabWidget->addTab(systemManagementWidget, targetComputerName);
    //ui.tabWidget->cornerWidget(Qt::TopRightCorner)->setEnabled(ui.tabWidget->count() > 1);
    ui.tabWidget->setCurrentWidget(systemManagementWidget);


    //    QDialog dlg(this);
    //    QVBoxLayout layout;
    //    SystemManagementWidget systemManagementWidget(m_adminName, controlCenterPacketsParser, computerName(), userName(), ipAddress(), macAddress(), (usbsdEnabled() == "1"?true:false), (programesEnabled() == "1"?true:false), m_administrators, &dlg);
    //    layout.addWidget(&systemManagementWidget);
    //    layout.setSizeConstraint(QLayout::SetFixedSize);
    //    layout.setContentsMargins(3,3,3,3);

    //    dlg.setLayout(&layout);
    //    dlg.setWindowTitle("System Management");
    //    dlg.exec();

    //    ui.comboBoxUSBSD->setCurrentIndex(0);
    //    ui.comboBoxPrograms->setCurrentIndex(0);
    //    slotQueryButtonClicked();
    //    ui.tableViewClientList->setFocus();


}


QString ControlCenter::computerName() const {
    return ui.lineEditComputerName->text().trimmed();
}

QString ControlCenter::userName() const {
    return ui.lineEditUserName->text().trimmed();
}

QString ControlCenter::workgroup() const {
    return ui.comboBoxWorkgroup->currentText();
}

QString ControlCenter::usbsdEnabled(){
    int index = ui.comboBoxUSBSD->currentIndex();
    if(index == 1){
        return "1";
    }else if(index == 2){
        return "0";
    }else{
        return "";
    }

    //    QString text = ui.comboBoxUSBSD->currentText();
    //    if(text == tr("Enabled")){
    //        return "1";
    //    }else if(text == tr("Disabled")){
    //        return "0";
    //    }else{
    //        return "";
    //    }


}

QString ControlCenter::macAddress() const {
    return ui.lineEditMACAddress->text().trimmed();

}

QString ControlCenter::ipAddress() const {
    return ui.lineEditIPAddress->text().trimmed();

}

QString ControlCenter::osVersion() const{
    return ui.comboBoxOSVersion->currentText();

}

QString ControlCenter::programesEnabled() const{
    int index = ui.comboBoxPrograms->currentIndex();
    if(index == 1){
        return "1";
    }else if(index == 2){
        return "0";
    }else{
        return "";
    }

    //QString text = ui.comboBoxPrograms->currentText();
    //    if(text == tr("Enabled")){
    //        return "1";
    //    }else if(text == tr("Disabled")){
    //        return "0";
    //    }else{
    //        return "";
    //    }

}


void ControlCenter::slotQueryDatabase() {
    
    ui.toolButtonQuery->setDefaultAction(ui.actionQueryDatabase);  
//    ui.tableViewClientList->setModel(queryModel);
    proxyModel->setSourceModel(queryModel);
    proxyModel->cleanFilters();
    
    slotQueryClient(computerName(), userName(), workgroup(), macAddress(), ipAddress(), osVersion(), usbsdEnabled(), programesEnabled());
    
    //statusBar()->showMessage(tr("Matched In Database:%1").arg(QString::number(queryModel->rowCount())));
    
}

void ControlCenter::slotSearchNetwork() {

    ui.toolButtonQuery->setDefaultAction(ui.actionSearchNetwork);
    
    proxyModel->setSourceModel(clientInfoModel);
    filter();

    //Broadcast
    QList<QHostAddress> broadcastAddresses = NetworkUtilities::broadcastAddresses();
    foreach (QHostAddress address, broadcastAddresses) {
        controlCenterPacketsParser->sendAdminSearchClientPacket(address, computerName(), userName(), workgroup(), macAddress(), ipAddress(), osVersion(), m_adminName);        
    }
    

    //statusBar()->showMessage(tr("Matched:%1 Total:%2").arg(QString::number(proxyModel->rowCount())).arg(clientInfoHash.size()));
    statusBar()->showMessage(tr("Matched In Network:%1").arg(QString::number(proxyModel->rowCount())));

}

void ControlCenter::filter(){

    QRegExp computerNameRegExp = QRegExp(".*", Qt::CaseInsensitive);
    QRegExp userNameRegExp = QRegExp(".*", Qt::CaseInsensitive);
    QRegExp workgroupRegExp = QRegExp(".*", Qt::CaseInsensitive);
    QRegExp usbSDRegExp = QRegExp(".*", Qt::CaseInsensitive);
    QRegExp macRegExp = QRegExp(".*", Qt::CaseInsensitive);
    QRegExp ipRegExp = QRegExp(".*", Qt::CaseInsensitive);
    QRegExp osRegExp = QRegExp(".*", Qt::CaseInsensitive);
    QRegExp programsRegExp = QRegExp(".*", Qt::CaseInsensitive);


    QString filterString = computerName();
    if(!filterString.trimmed().isEmpty()){
        computerNameRegExp = QRegExp(filterString, Qt::CaseInsensitive);
    }

    filterString = workgroup();
    if(!filterString.trimmed().isEmpty()){
        workgroupRegExp = QRegExp(filterString, Qt::CaseInsensitive);
    }

    filterString = usbsdEnabled();
    if(!filterString.trimmed().isEmpty()){
        usbSDRegExp = QRegExp(filterString, Qt::CaseInsensitive);
    }

    filterString = macAddress();
    if(!filterString.trimmed().isEmpty()){
        macRegExp = QRegExp(filterString, Qt::CaseInsensitive);
    }

    filterString = ipAddress();
    if(!filterString.trimmed().isEmpty()){
        ipRegExp = QRegExp(filterString, Qt::CaseInsensitive);
    }

    filterString = userName();
    if(!filterString.trimmed().isEmpty()){
        userNameRegExp = QRegExp(filterString, Qt::CaseInsensitive);
    }

    filterString = osVersion();
    if(!filterString.trimmed().isEmpty()){
        osRegExp = QRegExp(filterString, Qt::CaseInsensitive);
    }

    filterString = programesEnabled();
    if(!filterString.trimmed().isEmpty()){
        programsRegExp = QRegExp(filterString, Qt::CaseInsensitive);
    }

    proxyModel->setFilters(computerNameRegExp, userNameRegExp, workgroupRegExp, usbSDRegExp, macRegExp, ipRegExp, osRegExp, programsRegExp);

    statusBar()->showMessage(tr("Matched:%1").arg(QString::number(proxyModel->rowCount())));

}

void ControlCenter::slotQueryClient(const QString &computerName, const QString &userName, const QString &workgroup, const QString &macAddress, const QString &ipAddress, const QString &osVersion, const QString &usbsd, const QString &programes) {

    QString queryString = QString("select * from summaryinfo where ComputerName like '%%1%' and Workgroup like '%%2%' ") .arg(
                computerName) .arg(workgroup);

    QString network = "";
    if(!ipAddress.isEmpty()){
        network += ipAddress;
    }
    if(!macAddress.isEmpty()){
        network += QString("%" + macAddress);
    }
    queryString += QString(" and Network like '%%1%' ").arg(network);

    if(!userName.isEmpty()){
        queryString += QString(" and Users like '%%1%' ").arg(userName);
    }

    if(!osVersion.isEmpty()){
        queryString += QString(" and OS like '%%1%' ").arg(osVersion);
    }

    if(!usbsd.isEmpty()){
        queryString += QString(" and USBSD = %1 ").arg(usbsd == "1"?1:0);
    }

    if(!programes.isEmpty()){
        queryString += QString(" and Programes = %1 ").arg(programes == "1"?1:0);
    }

    querySitoyClientInfo(queryString);

}

void ControlCenter::querySitoyClientInfo(const QString &queryString){

    QApplication::setOverrideCursor(Qt::WaitCursor);

    DatabaseConnecter dc(this);
    if(!dc.isDatabaseOpened(databaseConnectionName,
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
    db = QSqlDatabase::database(databaseConnectionName);



    queryModel->setQuery(QSqlQuery(queryString, db));

    QSqlError error = queryModel->lastError();
    if (error.type() != QSqlError::NoError) {
        QString msg = QString("Can not query client info from database! %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
        QApplication::restoreOverrideCursor();
        QMessageBox::critical(this, tr("Fatal Error"), msg);

        //MySQL数据库重启，重新连接
        if(error.number() == 2006){
            db.close();
            QSqlDatabase::removeDatabase(databaseConnectionName);
            return;
        }

    }

    while(queryModel->canFetchMore()){
        queryModel->fetchMore();
    }

    QApplication::restoreOverrideCursor();

}

//void ControlCenter::slotUpdateButtonClicked() {


//}


void ControlCenter::slotShowClientInfo(const QModelIndex &index) {

    if(!index.isValid()){
        return;
    }

    int row = index.row();
    QStringList list;


    for(int i = 0; i < 8; i++){
        QModelIndex idx =  index.sibling(row,i);
        list << idx.data().toString();
        //QMessageBox::information(this,QString(row),idx.data().toString());
    }

    ui.lineEditComputerName->setText(list.at(0));
    ui.comboBoxWorkgroup->setCurrentIndex( ui.comboBoxWorkgroup->findText(list.at(1),Qt::MatchStartsWith));

    QStringList networkInfoList = list.at(2).split(",").at(0).split("/");
    ui.lineEditIPAddress->setText(networkInfoList.at(0));
    ui.lineEditMACAddress->setText(networkInfoList.at(1).toLower());

    ui.lineEditUserName->setText(list.at(3));
    ui.comboBoxOSVersion->setCurrentIndex(ui.comboBoxOSVersion->findText(list.at(4),Qt::MatchStartsWith));


    ui.comboBoxUSBSD->setCurrentIndex( ui.comboBoxUSBSD->findText((list.at(5) == "1"?tr("Enabled"):tr("Disabled")),Qt::MatchStartsWith));
    ui.comboBoxPrograms->setCurrentIndex( ui.comboBoxPrograms->findText((list.at(6) == "1"?tr("Enabled"):tr("Disabled")),Qt::MatchStartsWith));

    m_administrators = list.at(7);


    if(!ui.lineEditComputerName->isReadOnly()){
        ui.lineEditComputerName->setReadOnly(true);
        ui.lineEditUserName->setReadOnly(true);

        ui.comboBoxWorkgroup->setEnabled(false);
        ui.comboBoxUSBSD->setEnabled(false);

        ui.lineEditMACAddress->setReadOnly(true);
        ui.lineEditIPAddress->setReadOnly(true);

        ui.comboBoxOSVersion->setEnabled(false);
        ui.comboBoxPrograms->setEnabled(false);
    }

    ui.toolButtonManage->setEnabled(true);


}

void ControlCenter::slotExportQueryResult(){

    DataOutputDialog dlg(ui.tableViewClientList, DataOutputDialog::EXPORT, this);
    dlg.exec();

}

void ControlCenter::slotPrintQueryResult(){

#ifndef QT_NO_PRINTER
    //TODO
    DataOutputDialog dlg(ui.tableViewClientList, DataOutputDialog::PRINT, this);
    dlg.exec();
#endif

}

void ControlCenter::slotRemoteDesktop(){

    //    QDesktopWidget *desktop = QApplication::desktop();
    //    QRect rect = desktop->screenGeometry();
    
    
    //    QProcess *rdpProcess = new QProcess(this);
    //    connect(rdpProcess, SIGNAL(finished(int)), rdpProcess, SLOT(deleteLater()));

    QString cmdString = ""; 

#ifdef Q_OS_WIN32

    cmdString = QString("mstsc.exe /v:%1 /f").arg(ipAddress());

#else

    cmdString = QString("rdesktop %1 -f").arg(ipAddress());

#endif

    //    rdpProcess->start(cmdString);

    if(!QProcess::startDetached(cmdString)){
        QMessageBox::critical(this, tr("Error"), tr("Can not start RDP client process!"));
    }

}

void ControlCenter::slotVNC(){

    if(!vncProcess){
        vncProcess = new QProcess(this);
    }
    
    if(vncProcess->state() == QProcess::Running){
        return;
    }
    
    
    QString cmdString = ""; 

#ifdef Q_OS_WIN32

    //cmdString = QString("vncviewer.exe %1").arg(ipAddress());
    cmdString = QString("vncviewer.exe -listen");

#else

    cmdString = QString("vnc %1").arg(ipAddress());

#endif

    //    if(!QProcess::startDetached(cmdString)){
    //        QMessageBox::critical(this, tr("Error"), tr("Can not start VNC client process!"));
    //    }
    

    vncProcess->start(cmdString);
    if(!vncProcess->waitForStarted(5000)){
        QMessageBox::critical(this, tr("Error"), tr("Can not start VNC client process!"));
    }
    
    
    //TODO:
    
}

void ControlCenter::slotUpdateUserLogonPassword(){
    
    if(m_adminName != "kiwa" && m_adminName != "hehui"){
        QMessageBox::critical(this, tr("Error"), tr("You dont have the access permissions!"));
        return;
    }

    if(ui.comboBoxWorkgroup->currentText().isEmpty()){
        QMessageBox::critical(this, tr("Error"), tr("Please select the workgroup!"));
        ui.comboBoxWorkgroup->setFocus();
        return;
    }
    
    int ret = QMessageBox::question(this, tr("Confirm"), tr("Do you really want to inform the clients to update the password?"), QMessageBox::Yes|QMessageBox::No);
    if(ret == QMessageBox::No){
        return;
    }
    
    ui.toolButtonUpdatePassword->setDefaultAction(ui.actionUpdatePassword);
    
    controlCenterPacketsParser->sendUpdateMSUserPasswordPacket("", 0, workgroup(), m_adminName);
}

void ControlCenter::slotInformUserNewLogonPassword(){
    
    if(m_adminName != "kiwa" && m_adminName != "hehui"){
        QMessageBox::critical(this, tr("Error"), tr("You dont have the access permissions!"));
        return;
    }

    if(ui.comboBoxWorkgroup->currentText().isEmpty()){
        QMessageBox::critical(this, tr("Error"), tr("Please select the workgroup!"));
        ui.comboBoxWorkgroup->setFocus();
        return;
    }
    
    int ret = QMessageBox::question(this, tr("Confirm"), tr("Do you really want to inform the clients to retrieve the new password?"), QMessageBox::Yes|QMessageBox::No);
    if(ret == QMessageBox::No){
        return;
    }
    
    ui.toolButtonUpdatePassword->setDefaultAction(ui.actionInformNewPassword);
    
    controlCenterPacketsParser->sendInformUpdatePasswordPacket("", 0, workgroup(), m_adminName);
    
}

void ControlCenter::slotSendAnnouncement(){

    if(m_adminName != "hehui"){
        QMessageBox::critical(this, tr("Error"), tr("You dont have the access permissions!"));
        return;
    }

    
    QDialog dlg(this);
    QVBoxLayout layout;
    Announcement announcement(ui.tableViewClientList, m_adminName, controlCenterPacketsParser, &dlg);
    layout.addWidget(&announcement);
    layout.setSizeConstraint(QLayout::SetFixedSize);
    layout.setContentsMargins(3,3,3,3);
    
    dlg.setLayout(&layout);
    dlg.setWindowTitle("Announcement");
    dlg.exec();

    
    
} 

void ControlCenter::slotShowCustomContextMenu(const QPoint & pos){

    QTableView *tableView = qobject_cast<QTableView*> (sender());

    if (!tableView){
        return;
    }

    updateActions();

    QMenu menu(this);

    menu.addAction(ui.actionRemoteDesktop);
    menu.addSeparator();

    menu.addAction(ui.actionExport);

#ifndef QT_NO_PRINTER

    menu.addSeparator();

    ui.actionPrint->setShortcut(QKeySequence::Print);
    menu.addAction(ui.actionPrint);

#endif

    //        menu.addSeparator();
    //        menu.addAction(ui.actionUpdateUserLogonPassword);

    menu.exec(tableView->viewport()->mapToGlobal(pos));

}

void ControlCenter::updateActions() {

    bool selected = ui.tableViewClientList->currentIndex().isValid() && ui.tableViewClientList->selectionModel()->selectedIndexes().size();

    if(osVersion().contains("2000", Qt::CaseInsensitive)){
        ui.actionRemoteDesktop->setEnabled(false);
    }else{
        ui.actionRemoteDesktop->setEnabled(selected);
    }

    ui.actionExport->setEnabled(selected);
    ui.actionPrint->setEnabled(selected);

}

void ControlCenter::startNetwork(){

    QString errorMessage = "";
    m_udpServer = resourcesManager->startUDPServer(QHostAddress::Any, m_localUDPListeningPort, true, &errorMessage);
    if(!m_udpServer){
        QMessageBox::critical(this, tr("Error"), tr("Can not start UDP listening on port %1! %2").arg(m_localUDPListeningPort).arg(errorMessage));
    }else{
        qWarning()<<QString("UDP listening on port %1!").arg(m_localUDPListeningPort);
    }

    m_rtp = resourcesManager->startRTP(QHostAddress::Any, m_localRTPListeningPort, true, &errorMessage);

//    m_udtProtocol = m_rtp->getUDTProtocol();
////    if(!m_udtProtocol){
////        QString error = tr("Can not start UDT listening on port %1! %2").arg(m_localUDTListeningPort).arg(errorMessage);
////        QMessageBox::critical(this, tr("Error"), error);
////        close();
////        return;
////    }
//    m_localUDTListeningPort = m_udtProtocol->getUDTListeningPort();
//    //connect(m_udtProtocol, SIGNAL(disconnected(int)), this, SLOT(peerDisconnected(int)));
//    m_udtProtocol->startWaitingForIOInOneThread(1);
//    //m_udtProtocol->startWaitingForIOInSeparateThread();




    controlCenterPacketsParser = new ControlCenterPacketsParser(resourcesManager, this);

    connect(controlCenterPacketsParser, SIGNAL(signalServerDeclarePacketReceived(const QString&, quint16, quint16, const QString&, const QString&, int)), this, SLOT(serverFound(const QString&, quint16, quint16, const QString&, const QString&, int)));
    connect(controlCenterPacketsParser, SIGNAL(signalClientResponseClientSummaryInfoPacketReceived(const QString&, const QString&, const QString&, const QString&, const QString&, bool, bool, const QString&, bool, const QString&)), this, SLOT(updateOrSaveClientInfo(const QString&, const QString&, const QString&, const QString&, const QString&, bool, bool, const QString&, bool, const QString&)), Qt::QueuedConnection);
    //connect(controlCenterPacketsParser, SIGNAL(signalClientOnlineStatusChanged(int, const QString&, bool)), this, SLOT(processClientOnlineStatusChangedPacket(int, const QString&, bool)), Qt::QueuedConnection);

    if(localSystemManagementWidget){
        localSystemManagementWidget->setRTP(m_rtp);
        localSystemManagementWidget->setControlCenterPacketsParser(controlCenterPacketsParser);
    }

    m_networkReady = true;

    controlCenterPacketsParser->sendClientLookForServerPacket("255.255.255.255");


}

void ControlCenter::serverFound(const QString &serverAddress, quint16 serverUDTListeningPort, quint16 serverTCPListeningPort, const QString &serverName, const QString &version, int serverInstanceID){
    qDebug()<<"----ControlCenter::serverFound(...)";


    m_rtp->closeSocket(m_socketConnectedToServer);
    QString errorMessage;
    m_socketConnectedToServer = m_rtp->connectToHost(QHostAddress(serverAddress), serverUDTListeningPort, 10000, &errorMessage);
    if(m_socketConnectedToServer == INVALID_SOCK_ID){
        qCritical()<<tr("ERROR! Can not connect to server %1:%2 ! %3").arg(serverAddress).arg(serverUDTListeningPort).arg(errorMessage);
        return;
    }

    controlCenterPacketsParser->sendAdminOnlineStatusChangedPacket(m_socketConnectedToServer, localComputerName, m_adminName, true);





//    if(m_serverInstanceID != 0 && serverInstanceID != m_serverInstanceID){
//        controlCenterPacketsParser->sendClientOnlinePacket(networkManager->localRUDPListeningAddress(), networkManager->localRUDPListeningPort(), m_adminName+"@"+localComputerName, true);
//    }

    qWarning()<<"Server Found!"<<" Address:"<<serverAddress<<" UDT Port:"<<serverUDTListeningPort<<" Name:"<<serverName;

    //controlCenterPacketsParser->sendClientOnlinePacket(networkManager->localRUDPListeningAddress(), networkManager->localRUDPListeningPort(), m_adminName+"@"+localComputerName, true);


    QString msg = tr("IP: %1<br>UDT Port: %2<br>Name: %3<br>Version: %4").arg(serverAddress).arg(serverUDTListeningPort).arg(serverName).arg(version);

#ifdef Q_OS_WIN

    if(Utilities::versionCompare(version, QString(APP_VERSION)) == 1){
        //QMessageBox::warning(this, tr("Update Needed"), tr("New version available! Please update!"));
        msg += tr("<p><font color = 'red'><b>New version available! Please check for update!</b></font></p>");
        //QMessageBox::warning(this, tr("Server Found"), msg);
    }


#endif

    QMessageBox::information(this, tr("Server Found"), msg);



}

void ControlCenter::updateOrSaveClientInfo(const QString &computerName, const QString &workgroupName, const QString &networkInfo, const QString &usersInfo, const QString &osInfo, bool usbsdEnabled, bool programesEnabled, const QString &admins, bool isJoinedToDomain, const QString &clientVersion){

    qDebug()<<"updateOrSaveClientInfo(...) "<<computerName<<" "<<workgroupName<<" "<<networkInfo<<" "<<usersInfo;
    
    if(computerName.trimmed().isEmpty()){
        qCritical()<<"Invalid Computer Name!";
        return;
    }


    ClientInfo *info = 0;
    if(clientInfoHash.contains(computerName)){
        info = clientInfoHash.value(computerName);
        //qWarning()<<QString("Client Info of '%1' Exists In:  Memory:YES  DB:%2").arg(computerName).arg(isRecordExistInDB(computerName)?"YES":"NO")<<"\n";
    }else{
        info = new ClientInfo(computerName, this);
        clientInfoHash.insert(computerName, info);                      
    }


    info->setWorkgroup(workgroupName);
    info->setNetwork(networkInfo);
    info->setUsers(usersInfo);
    info->setOs(osInfo);
    info->setUsbSDEnabled(usbsdEnabled);
    info->setProgramsEnabled(programesEnabled);
    info->setAdministrators(admins);
    info->setClientVersion(clientVersion);
    info->setLastOnlineTime(QDateTime::currentDateTime());
    info->setIsJoinedToDomain(isJoinedToDomain);

    clientInfoModel->addClientInfo(info);
    

    //if(ui.tableViewClientList->model() == proxyModel){
        statusBar()->showMessage(tr("Matched:%1 Total:%2").arg(QString::number(ui.tableViewClientList->model()->rowCount())).arg(clientInfoHash.size()));
    //}


    qApp->processEvents();

}

void ControlCenter::processClientOnlineStatusChangedPacket(int socketID, const QString &clientName, bool online){
    qDebug()<<"--ControlCenter::processClientOnlineStatusChangedPacket(...)";

//    QString ip = "";
//    quint16 port = 0;

//    if(!m_udtProtocol->getAddressInfoFromSocket(socketID, &ip, &port)){
//        qCritical()<<m_udtProtocol->getLastErrorMessage();
//        return;
//    }

//    qWarning()<<QString("Client %1 %2!").arg(clientName).arg(online?"Online":"Offline");

//    if(online){
//        clientSocketsHash.insert(socketID, QHostAddress(ip));
//    }else{

//        int tabPages = ui.tabWidget->count();
//        for(int i = tabPages; i >= 0; --i){
//            SystemManagementWidget *widget = qobject_cast<SystemManagementWidget *>(ui.tabWidget->widget(i));
//            if(!widget){continue;}
//            if(widget->peerIPAddress() == clientSocketsHash.value(socketID)){
//                widget->peerDisconnected(true);
//            }

//        }

//        clientSocketsHash.remove(socketID);
//    }

}

void ControlCenter::peerConnected(const QHostAddress &peerAddress, quint16 peerPort){
    qWarning()<<QString("Connected! "+peerAddress.toString()+":"+QString::number(peerPort));

}

void ControlCenter::signalConnectToPeerTimeout(const QHostAddress &peerAddress, quint16 peerPort){
    qCritical()<<QString("Connecting Timeout! "+peerAddress.toString()+":"+QString::number(peerPort));

}

void ControlCenter::peerDisconnected(const QHostAddress &peerAddress, quint16 peerPort, bool normalClose){
    qDebug()<<QString("Disconnected! "+peerAddress.toString()+":"+QString::number(peerPort));

//    if(!normalClose){
//        qCritical()<<QString("ERROR! Peer %1:%2 Closed Unexpectedly!").arg(peerAddress.toString()).arg(peerPort);
//    }

//    int tabPages = ui.tabWidget->count();
//    for(int i = tabPages; i >= 0; --i){
//        SystemManagementWidget *widget = qobject_cast<SystemManagementWidget *>(ui.tabWidget->widget(i));
//        if(!widget){continue;}
//        if(widget->peerIPAddress() == peerAddress){
//            widget->peerDisconnected(normalClose);
//        }

//    }

}

void ControlCenter::peerDisconnected(int socketID){
    qDebug()<<"--ControlCenter::peerDisconnected(...) socketID:"<<socketID;

//    if(clientSocketsHash.contains(socketID)){
//        qCritical()<<QString("ERROR! Peer %1 Closed Unexpectedly!").arg(clientSocketsHash.value(socketID).toString());

//        int tabPages = ui.tabWidget->count();
//        for(int i = tabPages; i >= 0; --i){
//            SystemManagementWidget *widget = qobject_cast<SystemManagementWidget *>(ui.tabWidget->widget(i));
//            if(!widget){continue;}
//            if(widget->peerIPAddress() == clientSocketsHash.value(socketID)){
//                widget->peerDisconnected(false);
//            }
//        }

//        clientSocketsHash.remove(socketID);
//    }

}




















} //namespace HEHUI
