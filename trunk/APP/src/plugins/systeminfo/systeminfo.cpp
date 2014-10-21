

#include <QString>
#include <QDir>
#include <QCloseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QSqlQuery>
#include <QStatusBar>
#include <QSettings>

#include "./systeminfo.h"
#include "HHSharedCore/hglobal_core.h"

#ifdef Q_OS_WIN32
    #include <windows.h>
    #include <Lm.h>
    #include <Lmjoin.h>

#endif

#ifndef SITOY_MSSQLSERVER_DB_CONNECTION_NAME
#define SITOY_MSSQLSERVER_DB_CONNECTION_NAME "200.200.200.2/MIS/AssetsInfo"
#endif

#ifndef MYSQL_DB_CONNECTION_NAME
#define MYSQL_DB_CONNECTION_NAME "200.200.200.17/sitoycomputers/systeminfo"
#endif

#ifndef REMOTE_SITOY_ASSETS_DB_NAME
#define REMOTE_SITOY_ASSETS_DB_NAME "sitoyassets"
#endif


namespace HEHUI {

bool SystemInfo::running = false;
QMap<QString/*Short Name*/, QString/*Department*/> SystemInfo::departments = QMap<QString, QString>();

SystemInfo::SystemInfo(const QString &adminName, QWidget *parent)
    :QMainWindow(parent), m_adminName(adminName)
{

    ui.setupUi(this);

    ui.lineEditRegistrant->setText(m_adminName);
    m_computerName = getComputerName().toLower();
    ui.lineEditComputerName->setText(m_computerName.toUpper());

    m_workgroup = getJoinInformation();

    ui.comboBoxLocation->addItem(tr("Dong Guan"), "dg");
    ui.comboBoxLocation->addItem(tr("Ying De"), "yd");
    ui.comboBoxLocation->addItem(tr("Hong Kong"), "hk");
    QString location = m_computerName.left(2).toLower();
    if("yd" == location){
        ui.comboBoxLocation->setCurrentIndex(1);
    }else if("hk" == location){
        ui.comboBoxLocation->setCurrentIndex(2);
    }else{
        ui.comboBoxLocation->setCurrentIndex(0);
    }
    connect(ui.comboBoxLocation, SIGNAL(currentIndexChanged(int)), this, SLOT(getNewComputerName()));

    if(departments.isEmpty()){
        departments.insert("it", "IT");
        departments.insert("ac", "Account");
        departments.insert("ad", "AdminDept");
        departments.insert("co", "Cost");
        departments.insert("cu", "Custom");
        departments.insert("gm", "GMO");
        departments.insert("hr", "HR");
        departments.insert("ma", "Marker");
        departments.insert("pd", "PDS");
        departments.insert("pg", "PG");
        departments.insert("pl", "Plan");
        departments.insert("pm", "PMC");
        departments.insert("pu", "Purchase");
        departments.insert("qc", "QC");
        departments.insert("re", "Retail");
        departments.insert("sa", "Sales");
        //departmentsHash.insert("", "Sample");
        departments.insert("se", "Secretary");
        departments.insert("sh", "Ship");
        departments.insert("sp", "Shop");
        departments.insert("wh", "WHouse");
    }
    QString department = m_computerName.mid(2, 2).toLower();
    foreach (QString key, departments.keys()) {
        ui.comboBoxDepartment->addItem(departments.value(key), key);
    }
    ui.comboBoxDepartment->setCurrentIndex(-1);
    for(int i=0; i<ui.comboBoxDepartment->count(); i++){
        if(ui.comboBoxDepartment->itemData(i) == department){
            ui.comboBoxDepartment->setCurrentIndex(i);
            break;
        }
    }
    connect(ui.comboBoxDepartment, SIGNAL(currentIndexChanged(int)), this, SLOT(getNewComputerName()));

    m_sn = m_computerName.right(5).toInt();
    ui.spinBoxSN->setValue(m_sn);
    connect(ui.spinBoxSN, SIGNAL(valueChanged(int)), this, SLOT(getNewComputerName()));


    //No editing possible.
    ui.tableWidgetSoftware->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QHeaderView *view = ui.tableWidgetSoftware->horizontalHeader();
    view->resizeSection(0, 200);
    view->setVisible(true);


//    validator = 0;
    dc = new DatabaseConnecter(this);
    queryModel = 0;



    installEventFilter(this);


    initStatusBar();

    running = true;
    isScanning = false;

    //isUploaded = false;
    //isUploadedToSitoyDB = false;

    recordExists = false;

    process = new QProcess(this);
    process->setProcessChannelMode(QProcess::MergedChannels);
    connect(process, SIGNAL(finished( int , QProcess::ExitStatus)), this, SLOT(slotScannerExit( int , QProcess::ExitStatus )));

    slotScanSystem();

    statusBar()->showMessage(tr("Ctrl+S: Upload    F5: Scan    Ctrl+Return: Query"));

}

SystemInfo::~SystemInfo() {
    qDebug()<<"SystemInfo::~SystemInfo()";

    running = false;

    delete m_progressWidget;
    m_progressWidget = 0;

    if(process){
        delete process;
        process = 0;
    }

//    if(validator){
//        delete validator;
//        validator = 0;
//    }

    if(dc){
        delete dc;
        dc = 0;
    }


//    QSqlDatabase msSQLServerDB = QSqlDatabase::database(SITOY_MSSQLSERVER_DB_CONNECTION_NAME);
//    if(msSQLServerDB.isOpen()){
//        msSQLServerDB.close();
//    }
//    QSqlDatabase::removeDatabase(SITOY_MSSQLSERVER_DB_CONNECTION_NAME);

    QSqlDatabase mySQLDB = QSqlDatabase::database(MYSQL_DB_CONNECTION_NAME);
    if(mySQLDB.isValid() && mySQLDB.isOpen()){
        mySQLDB.close();
    }
    QSqlDatabase::removeDatabase(MYSQL_DB_CONNECTION_NAME);

    QSqlDatabase sitoyDB = QSqlDatabase::database(SITOY_MSSQLSERVER_DB_CONNECTION_NAME);
    if(sitoyDB.isValid() && sitoyDB.isOpen()){
        sitoyDB.close();
    }
    QSqlDatabase::removeDatabase(SITOY_MSSQLSERVER_DB_CONNECTION_NAME);


}


void SystemInfo::closeEvent(QCloseEvent *e) {
    qDebug()<<"SystemInfo::closeEvent(QCloseEvent *e)";

    //如果正在扫描,则提示是否退出
    //If scanning, interrogate user whether quit or not
    if(isScanning){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr("Scanning...."), tr("Scanner is running! <br> Exit the system?"),
                                      QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (reply == QMessageBox::No) {
            e->ignore();
            return;
        }else{
            process->disconnect();
            if(process->state() != QProcess::NotRunning){
                process->terminate();
            }
        }
    }


    e->accept();
    deleteLater();

}


bool SystemInfo::eventFilter(QObject *obj, QEvent *event) {
    //qDebug()<<"SystemInfo::eventFilter(QObject *obj, QEvent *event)";


    if (event->type() == QEvent::KeyRelease ) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *> (event);
        //        if(keyEvent->key() == Qt::Key_F8){
        //            slotScanSystem();
        //        }
        //        if(keyEvent->key() == Qt::Key_F5){
        //            slotUploadSystemInfo();
        //        }
        //        if(keyEvent->key() == Qt::Key_F9){
        //            slotQuerySystemInfo();
        //        }
        if(keyEvent->key() == Qt::Key_Return){
            focusNextChild();
        }
        if(keyEvent->key() == Qt::Key_Escape){
            slotResetAllInfo();
        }
        return true;
    }


    return QObject::eventFilter(obj, event);

}

void SystemInfo::languageChange() {
    qDebug()<<"SystemInfo::languageChange()";

    retranslateUi();
}

void SystemInfo::initStatusBar()
{
    qDebug()<<"SystemInfo::initStatusBar()";

    m_progressWidget = new QWidget();
    hlayout = new QHBoxLayout(m_progressWidget);

    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

    //label = new QLabel(tr("Updating search index"));
    //label->setSizePolicy(sizePolicy);
    //hlayout->addWidget(label);

    progressBar = new QProgressBar(m_progressWidget);
    progressBar->setRange(0, 0);
    progressBar->setTextVisible(false);
    progressBar->setSizePolicy(sizePolicy);

    //hlayout->setSpacing(6);
    hlayout->setMargin(0);
    hlayout->addWidget(progressBar);

    statusBar()->addPermanentWidget(m_progressWidget);
    m_progressWidget->hide();
}

QString SystemInfo::getEnvironmentVariable(const QString &environmentVariable){

    QString variableValueString = "";

    DWORD nSize = 512;
    LPWSTR variableValueArray = new wchar_t[nSize];

    int result = GetEnvironmentVariableW (environmentVariable.toStdWString().c_str(), variableValueArray, nSize);
    if(result == 0){
        return variableValueString;
    }

    variableValueString = QString::fromWCharArray(variableValueArray);

    delete [] variableValueArray;

    return variableValueString;

}

QString SystemInfo::getJoinInformation(bool *isJoinedToDomain, const QString &serverName){
    qDebug()<<"--SystemInfo::getJoinInformation()";


    QString workgroupName = "";
    NET_API_STATUS err;
    LPWSTR lpNameBuffer = new wchar_t[256];
    NETSETUP_JOIN_STATUS bufferType;
    LPCWSTR lpServer = NULL; // The server is the default local computer.
    if(!serverName.trimmed().isEmpty()){
        lpServer = serverName.toStdWString().c_str();
    }

    err = NetGetJoinInformation(lpServer, &lpNameBuffer, &bufferType);
    if(err == NERR_Success){
        workgroupName = QString::fromWCharArray(lpNameBuffer);
    }else{
        QMessageBox::critical(this, tr("Error"), tr("Can not get join status information!"));
    }

    NetApiBufferFree(lpNameBuffer);
    if(isJoinedToDomain){
        *isJoinedToDomain = (bufferType == NetSetupDomainName)?true:false;
    }

    return workgroupName;

}

QString SystemInfo::getComputerName(){
    qDebug()<<"--SystemInfo::getComputerName()";

    QString computerName = "";
    DWORD size = MAX_COMPUTERNAME_LENGTH + 1;
    LPWSTR name = new wchar_t[size];

    if(GetComputerNameW(name, &size)){
        computerName = QString::fromWCharArray(name);
    }else{
        QMessageBox::critical(this, tr("Error"), tr("Can not get computer name! Error: %1").arg(GetLastError()));
    }

    delete [] name;

    return computerName.toLower();

}

bool SystemInfo::setComputerNameWithAPI(const QString &computerName) {

    QSettings settings("HKEY_LOCAL_MACHINE\\SYSTEM\\ControlSet001\\Services\\Tcpip\\Parameters", QSettings::NativeFormat, this);
    settings.setValue("NV Hostname", computerName);

    //if (SetComputerNameExW(ComputerNamePhysicalDnsHostname, computerName)){
    if (SetComputerNameW(computerName.toStdWString().c_str())){
        return true;
    }else{
        qWarning()<< "Can not set computer name to " << computerName;
        QMessageBox::critical(this, tr("Error"), tr("Can not set computer name to '%1'").arg(computerName));
        return false;
    }

}

bool SystemInfo::isNT6OS()
{

    OSVERSIONINFO  osvi;
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx (&osvi);
    if(osvi.dwMajorVersion > 5){
        return true;
    }

    return false;

}

void SystemInfo::slotResetStatusBar(bool show){

    qDebug()<<"SystemInfo::slotResetStatusBar(bool show)";

    //statusBar()->removeWidget(m_progressWidget);
    //delete m_progressWidget;
    //m_progressWidget = 0;

    if(show){
        m_progressWidget->show();
    }else{
        progressBar->reset();
        m_progressWidget->hide();
    }

}




void SystemInfo::slotScanSystem() {
    qDebug()<<"SystemInfo::slotScanSystem()";


    QDir::setCurrent(QCoreApplication::applicationDirPath ());
    QString everestDirPath = QString("./aida64business");
    m_systemInfoFilePath = everestDirPath + QString("/systeminfo.ini");

    if (QFile(m_systemInfoFilePath).exists()) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr("System Infomation File Already Exists!"), tr("Rescan the system?"),
                                      QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::No) {
            slotReadReport();
            return;
        }

    }

    QString exeFilePath = everestDirPath + QString("/sysinfo.exe");

    if (!QFile(exeFilePath).exists()) {
        QMessageBox::critical(this, QString(tr("Error")), QString(tr("File '")+exeFilePath+tr("' Missing!")));
        return;

    }

    if(!process){
        process = new QProcess(this);
        process->setProcessChannelMode(QProcess::MergedChannels);
        connect(process, SIGNAL(finished( int , QProcess::ExitStatus)), this, SLOT(slotScannerExit( int , QProcess::ExitStatus )));
    }

    //process->setProcessChannelMode(QProcess::MergedChannels);
    //connect(process, SIGNAL(finished(int , QProcess::ExitStatus )), this, SLOT(slotReadReport()));
    //connect(process, SIGNAL(finished( int , QProcess::ExitStatus)), this, SLOT(slotScannerExit( int , QProcess::ExitStatus )));

    process->start(exeFilePath);


    isScanning = true;
    //isUploaded = false;

    slotResetStatusBar(true);
    statusBar()->showMessage(tr("Scanning...."));



}

void SystemInfo::slotScannerExit( int exitCode, QProcess::ExitStatus exitStatus){
    if((exitCode != 0) || (exitStatus == QProcess::CrashExit)){
        QMessageBox::critical(this, tr("Error"), tr("Scanner Error!"));
        return;
    }

    QFile file(m_systemInfoFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox::critical(this, QString(tr("Error")), QString(tr("Can not open system info file '")+m_systemInfoFilePath+tr("'!")));
        return;
    }
    QTextStream in(&file);
    QString sysinfo = in.readAll();
    file.close();

    if(!file.open(QIODevice::WriteOnly | QFile::Truncate | QIODevice::Text)){
        QMessageBox::critical(this, QString(tr("Error")), QString(tr("Can not write system info file '")+m_systemInfoFilePath+tr("'!")));
        return;
    }
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << sysinfo;
    file.flush();
    file.close();

    slotReadReport();

}

void SystemInfo::slotReadReport(){
    qDebug()<<"SystemInfo::slotReadReport()";

    statusBar()->showMessage(tr("Reading Reports...."));

    isScanning = false;


    slotResetAllInfo();

    QSettings systemInfo(m_systemInfoFilePath, QSettings::IniFormat, this);
    systemInfo.setIniCodec("UTF-8");

    if(!QFile(m_systemInfoFilePath).exists()){
        QMessageBox::critical(this, QString(tr("Error")), QString(tr("System Info File '")+m_systemInfoFilePath+tr("' Missing!")));

        //slotResetStatusBar(false);
        statusBar()->showMessage(tr("Error! Client Info File '%1' missing!").arg(m_systemInfoFilePath));
        return;
    }


    systemInfo.beginGroup("DevicesInfo");

    cpu = systemInfo.value("CPU").toString();
    memory = systemInfo.value("Memory").toString();
    motherboardName = systemInfo.value("MotherboardName").toString();
    dmiUUID = systemInfo.value("DMIUUID").toString();
    chipset = systemInfo.value("Chipset").toString();
    video = systemInfo.value("Video").toString();
    monitor = systemInfo.value("Monitor").toString();
    audio = systemInfo.value("Audio").toString();
    audio.replace("'", "\\'");

    adapter1Name = systemInfo.value("Adapter1Name").toString();
    adapter1HDAddress = systemInfo.value("Adapter1HDAddress").toString();
    adapter1IPAddress = systemInfo.value("Adapter1IPAddress").toString();
    nic1Info = adapter1Name + "|" + adapter1HDAddress + "|" + adapter1IPAddress;

    adapter2Name = systemInfo.value("Adapter2Name").toString();
    adapter2HDAddress = systemInfo.value("Adapter2HDAddress").toString();
    adapter2IPAddress = systemInfo.value("Adapter2IPAddress").toString();
    nic2Info = adapter2Name + "|" + adapter2HDAddress + "|" + adapter2IPAddress;

    ui.logicalDrivesComboBox->clear();
    storages.clear();
    for (int i = 1; i < 6; i++) {
        //QString diskKey,diskInfo;
        QString diskKey = QString("Disk" + QString::number(i));
        QString diskInfo = systemInfo.value(diskKey).toString();
        if (diskInfo.isEmpty()) {
            break;
        }
        storages.append(diskInfo);
        ui.logicalDrivesComboBox->addItem(diskInfo);
    }
    storagesInfo = storages.join(" | ");

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
    m_os = systemInfo.value("OS").toString();
    installationDate = systemInfo.value("InstallationDate").toString();
    //m_workgroup = systemInfo.value("Workgroup").toString();
    //QString computerName = systemInfo.value("ComputerName").toString();
    windowsDir = systemInfo.value("WindowsDir").toString();
    osKey = systemInfo.value("Key").toString();


    ui.osVersionLineEdit->setText(m_os);
    ui.installationDateLineEdit->setText(installationDate);
    ui.computerNameLineEdit->setText(m_computerName);
    ui.workgroupLineEdit->setText(m_workgroup);

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
    m_users = usersInfo.join(" | ");
    systemInfo.endGroup();

    ui.osGroupBox->setEnabled(true);


    systemInfo.beginGroup("Other");
    QString creationTime = systemInfo.value("CreationTime").toString();
    ui.labelReportCreationTime->setText("Report Creation Time: " + creationTime);
    //ui.labelReportCreationTime->show();
    systemInfo.endGroup();

    systemInfo.beginGroup("InstalledSoftwareInfo");
    softwares.clear();
    for(int k = 1; k < 400; k++){
        QString info = systemInfo.value(QString::number(k)).toString();
        if(info.isEmpty()){break;}
        softwares.append(info);
    }
    int rowCount = softwares.size();
    ui.tableWidgetSoftware->setRowCount(rowCount);
    for(int m=0; m<rowCount; m++){
        QStringList info = softwares.at(m).split(" | ");
        if(info.size() != 5){break;}
        for(int n=0; n<5; n++){
            ui.tableWidgetSoftware->setItem(m, n, new QTableWidgetItem(info.at(n)));
        }
    }
    systemInfo.endGroup();

    if(motherboardName.startsWith("Dell", Qt::CaseInsensitive)){
        ui.comboBoxVender->setCurrentIndex(1);
    }


    ui.toolButtonScan->setEnabled(true);
    ui.toolButtonUpload->setEnabled(true);

    ///////////////////////



    slotResetStatusBar(false);
    statusBar()->showMessage(tr("Done. Press 'Ctrl+S' to upload the data to server!"));


}





void SystemInfo::slotUploadSystemInfo(){
    qDebug()<<"SystemInfo::slotUploadSystemInfo()";




    slotGetAllInfo();

    //    slotUploadSystemInfoToSitoyDBServer();


    //QApplication::setOverrideCursor(Qt::WaitCursor);

    //DatabaseConnecter dc(this);
    if(!dc->isDatabaseOpened(MYSQL_DB_CONNECTION_NAME,
                             REMOTE_SITOY_COMPUTERS_DB_DRIVER,
                             REMOTE_SITOY_COMPUTERS_DB_SERVER_HOST,
                             REMOTE_SITOY_COMPUTERS_DB_SERVER_PORT,
                             REMOTE_SITOY_COMPUTERS_DB_USER_NAME,
                             REMOTE_SITOY_COMPUTERS_DB_USER_PASSWORD,
                             REMOTE_SITOY_ASSETS_DB_NAME,
                             HEHUI::MYSQL
                             )){
        QApplication::restoreOverrideCursor();
        QMessageBox::critical(this, tr("Fatal Error"), tr("Database Connection Failed! Upload Failed!"));
        qCritical() << QString("Error: Database Connection Failed! Upload Failed!");
        return;
    }


    QSqlDatabase db;
    db = QSqlDatabase::database(MYSQL_DB_CONNECTION_NAME);

    QSqlQuery query(db);

    QString queryString = QString("SELECT SN FROM assetsinfo s WHERE s.SN = '%1'  ").arg(m_sn);
    if(!query.exec(queryString)){
        QMessageBox::critical(this, tr("Error"), tr("Failed to query info! <br>%1").arg(query.lastError().text()));
        return;
    }
    if(query.first()){
        recordExists = true;
    }else{
        recordExists = false;
    }
    query.clear();


    if(recordExists){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr("Record Already Exists!"), tr("Record Already Exists!<br>Update it?"), QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::No) {
            return;
        }

        bool ok = query.prepare(QString("UPDATE assetsinfo SET ComputerName = :ComputerName, Workgroup = :Workgroup, Users = :Users, OS = :OS, Vender = :Vender, DateOfPurchase = :DateOfPurchase, Warranty = :Warranty, ServiceNumber = :ServiceNumber, Registrant = :Registrant, "
                              " InstallationDate = :InstallationDate, OSKey = :OSKey, CPU = :CPU, MotherboardName = :MotherboardName, Chipset = :Chipset, Memory = :Memory, Storage = :Storage, Video = :Video, Audio = :Audio, "
                              " NIC1 = :NIC1, NIC2 = :NIC2, Monitor = :Monitor, UpdateTime = NULL, Remark = :Remark "
                              " WHERE SN = '%1'").arg(m_sn));
        if(!ok){
            QMessageBox::critical(this, tr("Error"), tr("Failed to prepare query! <br>%1").arg(query.lastError().text()));
            return;
        }

    }else{
         bool ok = query.prepare("INSERT INTO assetsinfo (ComputerName, Workgroup, Users, OS, SN, Vender, DateOfPurchase, Warranty, ServiceNumber, Registrant, "
                      " InstallationDate, OSKey, CPU, MotherboardName, Chipset, Memory, Storage, Video, Audio, NIC1, NIC2, Monitor, "
                      " UpdateTime, Remark)"
                      " VALUES (:ComputerName, :Workgroup, :Users, :OS, :SN,  :Vender, :DateOfPurchase, :Warranty, :ServiceNumber, :Registrant, "
                      " :InstallationDate, :OSKey, :CPU, :MotherboardName, :Chipset, :Memory, :Storage, :Video, :Audio, :NIC1, :NIC2, :Monitor, "
                      " NULL, :Remark"
                      ")"
                      );
        if(!ok){
            QMessageBox::critical(this, tr("Error"), tr("Failed to prepare query! <br>%1").arg(query.lastError().text()));
            return;
        }

    }
    query.bindValue(":ComputerName", m_computerName);
    query.bindValue(":Workgroup", m_workgroup);
    query.bindValue(":Users", m_users);
    query.bindValue(":OS", m_os);
    query.bindValue(":SN", m_sn);
    query.bindValue(":Vender", vender);
    query.bindValue(":DateOfPurchase", dateOfPurchase.toString("yyyy-MM-dd"));
    query.bindValue(":Warranty", warranty);
    query.bindValue(":ServiceNumber", serviceNumber);
    query.bindValue(":Registrant", m_adminName);

    query.bindValue(":InstallationDate", installationDate);
    query.bindValue(":OSKey", osKey);
    query.bindValue(":CPU", cpu);
    query.bindValue(":MotherboardName", motherboardName);
    query.bindValue(":Chipset", chipset);
    query.bindValue(":Memory", memory);
    query.bindValue(":Storage", storagesInfo);
    query.bindValue(":Video", video);
    query.bindValue(":Audio", audio);
    query.bindValue(":NIC1", nic1Info);
    query.bindValue(":NIC2", nic2Info);
    query.bindValue(":Monitor", monitor);
    query.bindValue(":Remark", remark );


    if(!query.exec()){
        QApplication::restoreOverrideCursor();
        QMessageBox::critical(this, QObject::tr("Error"), tr("Can not upload data to server! <br> %1").arg(query.lastError().text()));
        qCritical()<<QString("Can not upload data to server!");
        qCritical()<<QString("Error: %1").arg(query.lastError().text());
        statusBar()->showMessage(tr("Error! Can not upload data to server!"));
        return;
    }
    query.clear();



    if(softwares.size()){
        QString updateInstalledSoftwaresInfoStatement = QString("START TRANSACTION; delete from installedsoftware where ComputerName = '%1'; ").arg(m_computerName);
        foreach (QString info, softwares) {
            QStringList values = info.split(" | ");
            if(values.size() != 5){continue;}
            updateInstalledSoftwaresInfoStatement += QString(" insert into installedsoftware(ComputerName, SoftwareName, SoftwareVersion, Size, InstallationDate, Publisher) values('%1', '%2', '%3', '%4', '%5', '%6'); ").arg(m_computerName).arg(values.at(0)).arg(values.at(1)).arg(values.at(2)).arg(values.at(3)).arg(values.at(4));
        }
        updateInstalledSoftwaresInfoStatement += "COMMIT;";

        if(!query.exec(updateInstalledSoftwaresInfoStatement)){
            QApplication::restoreOverrideCursor();
            QMessageBox::critical(this, tr("Error"), tr("Failed to upload installed software info to database! <br>%1").arg(query.lastError().text()));
        }

    }



    QApplication::restoreOverrideCursor();


    QMessageBox::information(this, tr("Done"), tr("Data has been uploaded to server!"));

    //isUploaded = true;
    statusBar()->showMessage(tr("Done! Data has been uploaded to server!"));


}

void SystemInfo::slotUploadSystemInfoToSitoyDB(){
    qDebug()<<"SystemInfo::slotUploadSystemInfoToSitoyDB()";


    slotGetAllInfo();

    //QApplication::setOverrideCursor(Qt::WaitCursor);

    //DatabaseConnecter dc(this);
    if(!dc->isDatabaseOpened(SITOY_MSSQLSERVER_DB_CONNECTION_NAME,
                             REMOTE_SITOY_SQLSERVER_DB_DRIVER,
                             REMOTE_SITOY_SQLSERVER_DB_HOST_NAME,
                             REMOTE_SITOY_SQLSERVER_DB_HOST_PORT,
                             REMOTE_SITOY_SQLSERVER_DB_USER_NAME,
                             REMOTE_SITOY_SQLSERVER_DB_USER_PASSWORD,
                             REMOTE_SITOY_SQLSERVER_DB_NAME,
                             HEHUI::M$SQLSERVER
                             )){
        QApplication::restoreOverrideCursor();
        QMessageBox::critical(this, tr("Error"), tr("Failed to open database!"));
        qCritical() << QString("Error!Failed to open database!");
        return;
    }


    QSqlDatabase db;
    db = QSqlDatabase::database(SITOY_MSSQLSERVER_DB_CONNECTION_NAME);

    QSqlQuery query(db);

    QString queryString = QString("SELECT SN FROM AssetsInfo s WHERE s.SN = '%1'  ").arg(m_sn);
    if(!query.exec(queryString)){
        QMessageBox::critical(this, tr("Error"), tr("Failed to query info! <br>%1").arg(query.lastError().text()));
        return;
    }
    if(query.first()){
        recordExists = true;
    }else{
        recordExists = false;
    }
    query.clear();


    if(recordExists){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr("Record Already Exists!"), tr("Record Already Exists!<br>Update it?"), QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::No) {
            return;
        }

        bool ok = query.prepare(QString("UPDATE AssetsInfo SET ComputerName = :ComputerName, Workgroup = :Workgroup, Users = :Users, OS = :OS, Vender = :Vender, BuyDate = :BuyDate, Warranty = :Warranty, ServiceNumber = :ServiceNumber, Registrant = :Registrant, "
                              " InstallationDate = :InstallationDate, OSKey = :OSKey, CPU = :CPU, MotherboardName = :MotherboardName, Chipset = :Chipset, Memory = :Memory, Storage = :Storage, Video = :Video, Audio = :Audio, "
                              " NIC1 = :NIC1, NIC2 = :NIC2, Monitor = :Monitor, UpdateTime = GETDATE(), Remark = :Remark "
                              " WHERE SN = '%1'").arg(m_sn));
        if(!ok){
            QMessageBox::critical(this, tr("Error"), tr("Failed to prepare query! <br>%1").arg(query.lastError().text()));
            return;
        }

    }else{
         bool ok = query.prepare("INSERT INTO AssetsInfo (ComputerName, Workgroup, Users, OS, SN, Vender, BuyDate, Warranty, ServiceNumber, Registrant, "
                      " InstallationDate, OSKey, CPU, MotherboardName, Chipset, Memory, Storage, Video, Audio, NIC1, NIC2, Monitor, "
                      " UpdateTime, Remark)"
                      " VALUES (:ComputerName, :Workgroup, :Users, :OS, :SN,  :Vender, :BuyDate, :Warranty, :ServiceNumber, :Registrant, "
                      " :InstallationDate, :OSKey, :CPU, :MotherboardName, :Chipset, :Memory, :Storage, :Video, :Audio, :NIC1, :NIC2, :Monitor, "
                      " GETDATE(), :Remark"
                      ")"
                      );
        if(!ok){
            QMessageBox::critical(this, tr("Error"), tr("Failed to prepare query! <br>%1").arg(query.lastError().text()));
            return;
        }

    }
    query.bindValue(":ComputerName", m_computerName);
    query.bindValue(":Workgroup", m_workgroup);
    query.bindValue(":Users", m_users);
    query.bindValue(":OS", m_os);
    query.bindValue(":SN", m_sn);
    query.bindValue(":Vender", vender);
    query.bindValue(":BuyDate", dateOfPurchase.toString("yyyy-MM-dd"));
    query.bindValue(":Warranty", warranty);
    query.bindValue(":ServiceNumber", serviceNumber);
    query.bindValue(":Registrant", m_adminName);

    query.bindValue(":InstallationDate", installationDate);
    query.bindValue(":OSKey", osKey);
    query.bindValue(":CPU", cpu);
    query.bindValue(":MotherboardName", motherboardName);
    query.bindValue(":Chipset", chipset);
    query.bindValue(":Memory", memory);
    query.bindValue(":Storage", storagesInfo);
    query.bindValue(":Video", video);
    query.bindValue(":Audio", audio);
    query.bindValue(":NIC1", nic1Info);
    query.bindValue(":NIC2", nic2Info);
    query.bindValue(":Monitor", monitor);
    query.bindValue(":Remark", remark );


    if(!query.exec()){
        QApplication::restoreOverrideCursor();
        QMessageBox::critical(this, QObject::tr("Error"), tr("Can not upload data to server! <br> %1").arg(query.lastError().text()));
        qCritical()<<QString("Can not upload data to server!");
        qCritical()<<QString("Error: %1").arg(query.lastError().text());
        statusBar()->showMessage(tr("Error! Can not upload data to server!"));
        return;
    }
    query.clear();



//    if(softwares.size()){
//        QString updateInstalledSoftwaresInfoStatement = QString("START TRANSACTION; delete from AssetsInfoSoftware where ComputerName = '%1'; ").arg(m_computerName);
//        foreach (QString info, softwares) {
//            QStringList values = info.split(" | ");
//            if(values.size() != 5){continue;}
//            updateInstalledSoftwaresInfoStatement += QString(" insert into AssetsInfoSoftware(ComputerName, SoftwareName, SoftwareVersion, Size, InstallationDate, Publisher) values('%1', '%2', '%3', '%4', '%5', '%6'); ").arg(m_computerName).arg(values.at(0)).arg(values.at(1)).arg(values.at(2)).arg(values.at(3)).arg(values.at(4));
//        }
//        updateInstalledSoftwaresInfoStatement += "COMMIT;";

//        if(!query.exec(updateInstalledSoftwaresInfoStatement)){
//            QApplication::restoreOverrideCursor();
//            QMessageBox::critical(this, tr("Error"), tr("Failed to upload installed software info to database! <br>%1").arg(query.lastError().text()));
//        }

//    }



    QApplication::restoreOverrideCursor();


    QMessageBox::information(this, tr("Done"), tr("Data has been uploaded to server!"));

    //isUploaded = true;
    statusBar()->showMessage(tr("Done! Data has been uploaded to server!"));


}


void SystemInfo::retranslateUi() {
    qDebug()<<"SystemInfo::retranslateUi()";

    ui.retranslateUi(this);

}




//void SystemInfo::slotUploadSystemInfoToSitoyDBServer(){
//    qDebug()<<"SystemInfo::slotUploadSystemInfoToSitoyDBServer()";


//    //if(isUploadedToSitoyDB){
//    //    QMessageBox::critical(this,tr("Operation Canceled"),tr("Data had been uploaded to server 200.200.200.2!"));
//    //    return;
//    //}

//    if(!slotIsPropertyNOValid()){
//        QMessageBox::critical(this, tr("Error"), tr("Invalid Property NO.!"));
//        return;
//    }

//    //QApplication::setOverrideCursor(Qt::WaitCursor);

//    //DatabaseConnecter dc(this);
//    //QString connectionName = "200.200.200.2/MIS/PC";
//    if(!dc->isDatabaseOpened(SITOY_MSSQLSERVER_DB_CONNECTION_NAME,
//                             "QODBC",
//                             "200.200.200.2",
//                             1433,
//                             "sa",
//                             "sitoydb",
//                             "mis",
//                             HEHUI::M$SQLSERVER
//                             )){
//        QApplication::restoreOverrideCursor();
//        QMessageBox::critical(this, tr("Fatal Error"), tr("Database Connection Failed! Upload Failed!"));
//        qCritical() << QString("Error: Database Connection Failed! Upload Failed!");
//        return;
//    }


//    QSqlDatabase db;
//    db = QSqlDatabase::database(SITOY_MSSQLSERVER_DB_CONNECTION_NAME);
//    QSqlQuery query(db);

//    bool recordExistsInSitoyDB = false;
//    QString queryString = QString("select _No  from PC where _No = '%1'").arg(pNo);

//    if(!query.exec(queryString)){
//        QMessageBox::information(this, "Error", query.lastError().text());
//        return ;
//    }

//    if(query.first()){
//        recordExistsInSitoyDB = true;
//    }

////    if(query.size() > 1){
////        QMessageBox::critical(this, tr("Fatal Error"), tr("Duplicate Property No.!"));
////        statusBar()->showMessage(tr("Error! Duplicate No.!"));
////        qCritical()<<"Error! Duplicate No.!";
////        recordExistsInSitoyDB = true;
////    }else if(query.size() == 1){
////        statusBar()->showMessage(tr("Record exists in server 200.200.200.2!"));
////        recordExistsInSitoyDB = true;
////    }else{
////        recordExistsInSitoyDB = false;
////    }


//    query.clear();

//    if(recordExistsInSitoyDB){
//        QMessageBox::StandardButton reply;
//        reply = QMessageBox::question(this, tr("Record Already Exists!"), tr("Record '%1' already exists in server 200.200.200.2!<br>Update it?").arg(pNo),
//                                      QMessageBox::Yes | QMessageBox::No);
//        if (reply == QMessageBox::No) {
//            return;
//        }

//        query.prepare(QString("UPDATE PC SET _No = :_No, CPU = :CPU, RAM = :RAM, HDD = :HDD, Remark = :Remark, Depart = :Depart, Area = :Area, Monitor = :Monitor, MainBoard = :MainBoard WHERE _No LIKE '%1'").arg(pNo));

//    }else{
//        query.prepare("INSERT INTO PC (_No, CPU, RAM, HDD, Remark, Depart, Area, Monitor, MainBoard) "
//                      "VALUES (:_No, :CPU, :RAM, :HDD, :Remark, :Depart, :Area, :Monitor, :MainBoard)");

//    }

//    query.bindValue(":_No", pNo.left(20));
//    query.bindValue(":CPU", cpu.left(20));
//    query.bindValue(":RAM", memory.left(20));
//    query.bindValue(":HDD", partitionsTotalSize.left(20));
//    query.bindValue(":Remark", remark.left(200));
//    query.bindValue(":Depart", dept.left(20));
//    query.bindValue(":Area", area.left(20));
//    query.bindValue(":Monitor", monitor.left(20));
//    query.bindValue(":MainBoard", motherboardName.left(20));


//    if(!query.exec()){
//        QApplication::restoreOverrideCursor();
//        QMessageBox::critical(this, QObject::tr("Fatal Error"), tr("Can not upload data to server 200.200.200.2! <br> %1").arg(query.lastError().text()));
//        qCritical()<<QString("Can not upload data to server 200.200.200.2!");
//        qCritical()<<QString("Error: %1").arg(query.lastError().text());
//        statusBar()->showMessage(tr("Error! Can not upload data to server 200.200.200.2!"));

//        return;
//    }

//    QApplication::restoreOverrideCursor();


//    QMessageBox::information(this, tr("Done"), tr("Data has been uploaded to server 200.200.200.2!"));

//    //isUploadedToSitoyDB = true;
//    statusBar()->showMessage(tr("Done! Data has been uploaded to server 200.200.200.2!"));


//}

void SystemInfo::slotQuerySystemInfo(){
    qDebug()<<"SystemInfo::slotQuerySystemInfo()";


    slotResetAllInfo();

    ////////////////////////////

    QApplication::setOverrideCursor(Qt::WaitCursor);

//    DatabaseConnecter dc(this);
//    if(!dc.isDatabaseOpened(MYSQL_DB_CONNECTION_NAME,
//                            REMOTE_SITOY_COMPUTERS_DB_DRIVER,
//                            REMOTE_SITOY_COMPUTERS_DB_SERVER_HOST,
//                            REMOTE_SITOY_COMPUTERS_DB_SERVER_PORT,
//                            REMOTE_SITOY_COMPUTERS_DB_USER_NAME,
//                            REMOTE_SITOY_COMPUTERS_DB_USER_PASSWORD,
//                            REMOTE_SITOY_ASSETS_DB_NAME,
//                            HEHUI::MYSQL
//                            )){
    if(!dc->isDatabaseOpened(SITOY_MSSQLSERVER_DB_CONNECTION_NAME,
                             REMOTE_SITOY_SQLSERVER_DB_DRIVER,
                             REMOTE_SITOY_SQLSERVER_DB_HOST_NAME,
                             REMOTE_SITOY_SQLSERVER_DB_HOST_PORT,
                             REMOTE_SITOY_SQLSERVER_DB_USER_NAME,
                             REMOTE_SITOY_SQLSERVER_DB_USER_PASSWORD,
                             REMOTE_SITOY_SQLSERVER_DB_NAME,
                             HEHUI::M$SQLSERVER
                             )){

        QApplication::restoreOverrideCursor();
        QMessageBox::critical(this, tr("Fatal Error"), tr("Database Connection Failed! Query Failed!"));
        qCritical() << QString("Error: Database Connection Failed! Query Failed!");
        return ;
    }


    QSqlDatabase db;
    db = QSqlDatabase::database(SITOY_MSSQLSERVER_DB_CONNECTION_NAME);


    if(!queryModel){
        queryModel = new QSqlQueryModel(this);
    }
    queryModel->clear();

    QString queryString = QString("SELECT * FROM AssetsInfo s WHERE s.ComputerName = '%1'  ").arg(m_computerName);
    queryModel->setQuery(QSqlQuery(queryString, db));
    QApplication::restoreOverrideCursor();

    QSqlError error = queryModel->lastError();
    if (error.type() != QSqlError::NoError) {
        QString msg = QString("Can not query system info from database! %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
        //QApplication::restoreOverrideCursor();
        QMessageBox::critical(this, tr("Fatal Error"), msg);

        //MySQL数据库重启，重新连接
        if(error.number() == 2006){
            db.close();
            QSqlDatabase::removeDatabase(MYSQL_DB_CONNECTION_NAME);
            return;
        }

    }

    queryModel->fetchMore();

//    QMessageBox::information(this, tr("rowCount"), QString::number(queryModel->rowCount()));
    QSqlRecord record = queryModel->record(0);
    if(!queryModel->rowCount() || record.isEmpty()){
        QMessageBox::critical(this, tr("Error"), tr("No Record Found!"));
        recordExists = false;
        return;
    }
    recordExists = true;

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

    ui.labelReportCreationTime->setText(tr("Last Update Time: %1").arg(record.value("UpdateTime").toString()));
    ui.spinBoxSN->setValue(record.value("SN").toUInt());
    ui.comboBoxVender->setCurrentIndex(ui.comboBoxVender->findText(record.value("Vender").toString()));
    ui.dateEditDateOfPurchase->setDate(record.value("BuyDate").toDate());
    ui.lineEditWarranty->setText(record.value("Warranty").toString());
    ui.lineEditDellServiceNumber->setText(record.value("ServiceNumber").toString());
    ui.lineEditRegistrant->setText(record.value("Registrant").toString());
    ui.lineEditRemark->setText(record.value("Remark").toString());


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


    //QApplication::restoreOverrideCursor();



}


void SystemInfo::slotResetAllInfo()
{

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
    ui.toolButtonUpload->setEnabled(false);

    recordExists = false;

    updateGeometry();


}


void SystemInfo::slotGetAllInfo()
{

    monitor = ui.monitorLineEdit->text();


    serviceNumber = ui.lineEditDellServiceNumber->text().trimmed();
    vender = ui.comboBoxVender->currentText();
    dateOfPurchase = ui.dateEditDateOfPurchase->date();
    warranty = ui.lineEditWarranty->text();
    remark = ui.lineEditRemark->text();


}


void SystemInfo::on_toolButtonQuerySystemInfo_clicked()
{

    slotQuerySystemInfo();

}

void SystemInfo::on_toolButtonUpload_clicked()
{

    int pos = 0;
    QRegExpValidator rxValidator(this);
    QRegExp rx("\\b(DG|YD|HK)[A-Z]{2}\\d{5}");
    rxValidator.setRegExp(rx);
    //if(validator->validate(pNo, pos) != QValidator::Acceptable){
    if(rxValidator.validate(m_computerName, pos) != QValidator::Acceptable){
        QMessageBox::critical(this, tr("Error"), tr("Invalid Computer Name!"));
        qDebug()<<"Invalid Computer Name!";
        ui.comboBoxLocation->setFocus();
        return ;
    }


    if(ui.comboBoxVender->currentText().startsWith("DELL", Qt::CaseInsensitive)){
        if(ui.lineEditDellServiceNumber->text().trimmed().isEmpty()){
            QMessageBox::critical(this, tr("Error"), tr("Dell Service Number Required!"));
            ui.lineEditDellServiceNumber->setFocus();
            return;
        }
    }

    dateOfPurchase = ui.dateEditDateOfPurchase->date();
    if(dateOfPurchase > QDate::currentDate() ){
        QMessageBox::critical(this, tr("Error"), tr("Invalid Date Of Purchase!"));
        ui.dateEditDateOfPurchase->setFocus();
        return;
    }


    slotUploadSystemInfoToSitoyDB();
    //slotUploadSystemInfo();

}

void SystemInfo::on_toolButtonScan_clicked()
{
    slotScanSystem();

}

void SystemInfo::getNewComputerName(){
    QString location = ui.comboBoxLocation->itemData(ui.comboBoxLocation->currentIndex()).toString();
    QString dept = ui.comboBoxDepartment->itemData(ui.comboBoxDepartment->currentIndex()).toString();
    QString sn = QString::number(ui.spinBoxSN->value()).rightJustified(5, '0');

    QString newName = location + dept + sn;
    ui.lineEditComputerName->setText(newName.toUpper());

    ui.pushButtonRenameComputer->setEnabled((!dept.trimmed().isNull()) && (newName.size() == 9) && (m_computerName != newName));

}

void SystemInfo::on_pushButtonRenameComputer_clicked(){



    QString text = tr("Do you really want to <b><font color = 'red'>rename</font></b> the computer? ");
    int ret = QMessageBox::question(this, tr("Question"), text, QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if(ret == QMessageBox::No){
        return;
    }

    bool ok = false;
    QString newComputerName = ui.lineEditComputerName->text();
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

    ok = setComputerNameWithAPI(newComputerName);
    if(!ok){
        setComputerName(newComputerName);
        //QMessageBox::critical(this, tr("Error"), tr("Can not rename computer to '%1'!<br>%2").arg(newComputerName).arg(m_rtp->lastErrorString()));
    }

    ui.pushButtonRenameComputer->setEnabled(false);

    m_computerName = ui.lineEditComputerName->text().trimmed();
    m_workgroup = ui.comboBoxDepartment->currentText();
    m_sn = ui.spinBoxSN->value();

    ui.computerNameLineEdit->setText(m_computerName);

    QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\HEHUI", QSettings::NativeFormat, this);
    settings.setValue("ComputerName", m_computerName);
    settings.setValue("Department", m_workgroup);
    settings.setValue("SN", m_sn);

}

void SystemInfo::setComputerName(const QString &newName){

#ifdef Q_OS_WIN32

    QStringList parameters;
     QProcess p;

     QString appDataCommonDir = getEnvironmentVariable("ALLUSERSPROFILE") + "\\Application Data";
     if(isNT6OS()){
         appDataCommonDir = getEnvironmentVariable("ALLUSERSPROFILE");
     }
     QString m_msUpdateExeFilename = appDataCommonDir + "\\msupdate.exe";
     if(!QFileInfo(m_msUpdateExeFilename).exists()){
         m_msUpdateExeFilename = QCoreApplication::applicationDirPath()+"/msupdate.exe";
     }
//     if(!QFileInfo(m_msUpdateExeFilename).exists()){
//         m_msUpdateExeFilename = appDataCommonDir + "\\cleaner.exe";
//     }


     parameters << "-setcomputername" << newName;
     p.start(m_msUpdateExeFilename, parameters);
     p.waitForFinished();


//     if(!wm->isUserAutoLogin()){
//         QMessageBox::critical(this, tr("Error"), tr("Failed to enable 'AutoAdminLogon'!"));
//     }else{
//         QMessageBox::information(this, tr("Done"), tr("Please restart your computer to take effect!"));
//     }



#else
    qWarning()<<"This function works on M$ Windows only!";
#endif


}

















}






