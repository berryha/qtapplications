

#include <QString>
#include <QDir>
#include <QCloseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QSqlQuery>


#include "./systeminfo.h"
//#include "../../shared/core/global_core.h"
#include "HHSharedCore/hglobal_core.h"


bool SystemInfo::running = false;
qint64 SystemInfo::failedFilesTotalSize = 0;

#ifndef SITOY_MSSQLSERVER_DB_CONNECTION_NAME
#define SITOY_MSSQLSERVER_DB_CONNECTION_NAME "200.200.200.2/MIS/PC"
#endif

#ifndef MYSQL_DB_CONNECTION_NAME
#define MYSQL_DB_CONNECTION_NAME "200.200.200.17/sitoycomputers/systeminfo"
#endif



SystemInfo::SystemInfo(bool isYDAdmin, QWidget *parent) :
        QMainWindow(parent), isYDAdmin(isYDAdmin)
{

    ui.setupUi(this);


    if(!isYDAdmin){
        //ui.comboBoxIDPart1->setCurrentIndex(ui.comboBoxIDPart1->findText("SDG"));
        ui.comboBoxIDPart1->setCurrentIndex(0);
    }else{
        //ui.comboBoxIDPart1->setCurrentIndex(ui.comboBoxIDPart1->findText("SYD"));
        ui.comboBoxIDPart1->setCurrentIndex(1);
    }


    ui.dateEditMonitorDate->setMaximumDate(QDate::currentDate());
    ui.dateEditMotherboardDate->setMaximumDate(QDate::currentDate());

    ui.lineEditServiceNumber->setEnabled(false);

    validator = new QRegExpValidator(this);
    //QRegExp rx("\\bS(DG|YD)-[ABC]-\\d{4}");
    QRegExp rx("\\d{4}");
    validator->setRegExp(rx);
    ui.idLineEdit->setValidator(validator);

    ui.idLineEdit->setFocus();

    dc = new DatabaseConnecter(this);

    connect(ui.cleanTempFilesToolButton, SIGNAL(clicked()), this, SLOT(slotCleaner()));
    connect(ui.backupMyDocsToolButton, SIGNAL(clicked()), this, SLOT(slotBackupMyDocs()));
    connect(ui.backupEmailToolButton, SIGNAL(clicked()), this, SLOT(slotBackupEmails()));
    //connect(ui.actionUploadSystemInfo, SIGNAL(triggered()), this, SLOT(slotUploadSystemInfo()));


    installEventFilter(this);





    //TODO:为何不可
    //setWindowFlags(Qt::WindowMinimizeButtonHint);

    // Get the size of screen
    //QDesktopWidget* desktop = QApplication::desktop();
    //int desktopWidth = desktop->width();
    //int desktopHeight = desktop->height();

    //move the dialog
    //move((desktopWidth - width()) / 2, (desktopHeight - height()) / 2);

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

    //if(process){
        delete process;
        process = 0;
    //}

    //if(validator){
        delete validator;
        validator = 0;
    //}

    //if(dc){
        delete dc;
        dc = 0;
    //}


        QSqlDatabase msSQLServerDB = QSqlDatabase::database(SITOY_MSSQLSERVER_DB_CONNECTION_NAME);
        if(msSQLServerDB.isOpen()){
            msSQLServerDB.close();
        }
        QSqlDatabase::removeDatabase(SITOY_MSSQLSERVER_DB_CONNECTION_NAME);

        QSqlDatabase mySQLDB = QSqlDatabase::database(MYSQL_DB_CONNECTION_NAME);
        if(mySQLDB.isOpen()){
            mySQLDB.close();
        }
        QSqlDatabase::removeDatabase(MYSQL_DB_CONNECTION_NAME);


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



void SystemInfo::slotCleaner() {
    qDebug()<<"SystemInfo::slotCleaner()";

#if defined(Q_WS_WIN)
    ui.cleanTempFilesToolButton->setEnabled(false);
    QString tempPath = QDir::tempPath();
    QString tempIEPath = QDir::homePath () + QDir::separator() + "Local Settings/Temporary Internet Files/Content.IE5";

    slotCleanTempFiles(tempPath);
    ui.tempDirLineEdit->setText(QVariant(failedFilesTotalSize/(1024*1024)).toString());
    failedFilesTotalSize = 0;

    slotCleanTempFiles(tempIEPath);
    ui.ieTempDirLineEdit->setText(QVariant(failedFilesTotalSize/(1024*1024)).toString());
    failedFilesTotalSize = 0;

    ui.cleanTempFilesToolButton->setEnabled(true);
#else
    QMessageBox::critical(this, tr("Operation Canceled"), tr(
            "This Function Is For Fucking M$ Windows Only!"));
#endif

}

void SystemInfo::slotCleanTempFiles(const QString &path) {
    qDebug()<<"SystemInfo::slotCleanTempFiles(const QString &path)";

    QDir dir(path);

    slotResetStatusBar(true);
    progressBar->setRange(0,dir.count());

    //qlonglong size = 0;
    //QStringList filters;
    //filters << "*" << "*.*";
    int i = 0;
    foreach(QString file, dir.entryList(/*filters,*/QDir::Files|QDir::System|QDir::Hidden))
    {
        if(!dir.remove(file)){
            failedFilesTotalSize += QFileInfo(dir.filePath(file)).size();
            qDebug() << "Delete Failed:" + path + QDir::separator() + file ;
        }

        progressBar->setValue(++i);
        statusBar()->showMessage(tr("Deleting %1").arg(file));
        qApp->processEvents();
    }

    foreach(QString subDir, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
        slotCleanTempFiles(path + QDir::separator() + subDir);
    }

    if (path != QDir::tempPath()) {
        dir.rmdir(path);
    }


    slotResetStatusBar(false);
    statusBar()->showMessage(tr("Temporary files deleted."),3000);

}

void SystemInfo::slotScanSystem() {
    qDebug()<<"SystemInfo::slotScanSystem()";

    //#ifndef Q_OS_WIN32
    //    QMessageBox::critical(this, tr("Error"), tr("This function is only available for M$ Windows!"));
    //    return;
    //#endif

    QDir::setCurrent(QCoreApplication::applicationDirPath ());
    //QString everestDirPath = QCoreApplication::applicationDirPath () + QString("/everest");
    QString everestDirPath = QString("./everest");

    QString systemInfoFilePath = everestDirPath + QString("/systeminfo.ini");

    if (QFile(systemInfoFilePath).exists()) {
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


    //QProcess *process = new QProcess(this);
    if(!process){
        qDebug()<<"!process";
        process = new QProcess(this);
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
        return;
    }

    slotReadReport();

}

void SystemInfo::slotReadReport(){
    qDebug()<<"SystemInfo::slotReadReport()";

    statusBar()->showMessage(tr("Reading Reports...."));

    isScanning = false;

    QString systemInfoFilePath = QCoreApplication::applicationDirPath () + QDir::separator()
                                 + QString("everest")+QDir::separator()+QString("systeminfo.ini");


    QSettings systemInfo(systemInfoFilePath, QSettings::IniFormat, this);

    if(!QFile(systemInfoFilePath).exists()){
        QMessageBox::critical(this, QString(tr("Error")), QString(tr("File '")+systemInfoFilePath+tr("' Missing!")));

        slotResetStatusBar(false);
        statusBar()->showMessage(tr("Error! File '%1' missing!").arg(systemInfoFilePath));
        //TODO:Close the window
        removeEventFilter(this);
        //close()
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
    partitionsTotalSize = systemInfo.value("PartitionsTotalSize").toString();

    QString adapter1Name = systemInfo.value("Adapter1Name").toString();
    QString adapter1HDAddress = systemInfo.value("Adapter1HDAddress").toString();
    QString adapter1IPAddress = systemInfo.value("Adapter1IPAddress").toString();
    network1Info<<adapter1Name<<adapter1HDAddress<<adapter1IPAddress;

    QString adapter2Name = systemInfo.value("Adapter2Name").toString();
    QString adapter2HDAddress = systemInfo.value("Adapter2HDAddress").toString();
    QString adapter2IPAddress = systemInfo.value("Adapter2IPAddress").toString();
    network2Info<<adapter2Name<<adapter2HDAddress<<adapter2IPAddress;

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
    os = systemInfo.value("OS").toString();
    installationDate = systemInfo.value("InstallationDate").toString();
    workgroup = systemInfo.value("Workgroup").toString();
    computerName = systemInfo.value("ComputerName").toString();
    windowsDir = systemInfo.value("WindowsDir").toString();


    ui.osVersionLineEdit->setText(os);
    ui.installationDateLineEdit->setText(installationDate);
    ui.computerNameLineEdit->setText(computerName);
    ui.workgroupLineEdit->setText(workgroup);

    ui.logicalDrivesComboBox->clear();
    for (int i = 1; i < 10; i++) {
        QString logicalDrivesKey,driveInfo;
        logicalDrivesKey = QString("Drive" + QString::number(i));
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

    systemInfo.beginGroup("UserInfo");
    userName = systemInfo.value("UserName").toString();
    isAdmin = systemInfo.value("IsAdmin").toBool();
    myDocuments = systemInfo.value("MyDocuments").toString();

    emailStoreRoot = systemInfo.value("EmailStoreRoot").toString();
    //emailFolderSize = systemInfo.value("EmailFolderSize").toInt();
    emailFolderSize = 0;
    float biggestDBXFileSize = 0;
    if(!emailStoreRoot.isEmpty()){
        QDir dir(emailStoreRoot);
        biggestDBXFile = dir.path()+"/Folders.dbx";

        foreach(QString file, dir.entryList(QDir::Files))
        {
            int curFileSize = QFileInfo(dir.filePath(file)).size()/(1024*1024);
            if(curFileSize > biggestDBXFileSize){
                biggestDBXFile = file;
                biggestDBXFileSize = QFileInfo(dir.filePath(file)).size()/(1024*1024);
            }
            emailFolderSize += curFileSize;
            qApp->processEvents();
        }
    }



    //TODO: Use Iterator to get the directory size on startup
    tempDirSize = systemInfo.value("TempDirSize").toInt();
    ieTempDirSize = systemInfo.value("IETempDirSize").toInt();

    systemInfo.endGroup();

    ui.userNamelineEdit->setText(userName);
    if(isAdmin){
        ui.isAdminlineEdit->setText(QString(tr("Yes")));
    }else{
        ui.isAdminlineEdit->setText(QString(tr("No")));
    }
    ui.myDocumentsLineEdit->setText(myDocuments);
    ui.backupMyDocsToolButton->setEnabled(true);

    ui.emailStoreRootLineEdit->setText(emailStoreRoot);
    ui.backupEmailToolButton->setEnabled(true);
    ui.emailFolderSizeLineEdit->setText(QString::number(emailFolderSize));
    if(!emailStoreRoot.isEmpty()){
	ui.biggestDBXLineEdit->setText(QFileInfo(biggestDBXFile).fileName()+": "+QVariant(biggestDBXFileSize).toString());
    }

    ui.tempDirLineEdit->setText(QString::number(tempDirSize));
    ui.ieTempDirLineEdit->setText(QString::number(ieTempDirSize));
    ui.cleanTempFilesToolButton->setEnabled(true);

    ui.userGroupBox->setEnabled(true);

    ui.idLineEdit->setFocus();
    unsigned int no = createPropertyNO();
//    if(no < 1000){
//        ui.idLineEdit->setText("0" + QString::number(no));
//    }else{
//        ui.idLineEdit->setText(QString::number(no));
//    }
    ui.idLineEdit->setText(QString::number(no).rightJustified(4, '0'));

    ui.toolButtonUpload->setEnabled(true);
    ui.remarkLineEdit->setEnabled(true);
    ui.deptComboBox->setEnabled(true);
    ui.locationComboBox->setEnabled(true);
    ui.dateEditMotherboardDate->setEnabled(true);
    ui.dateEditMonitorDate->setEnabled(true);

    if(os.toUpper() == "WIN_7"){
        ui.lineEditServiceNumber->setEnabled(true);
    }


    slotResetStatusBar(false);
    statusBar()->showMessage(tr("Done. Press 'Ctrl+S' to upload the data to server!"));


}


void SystemInfo::slotBackupMyDocs(){
    qDebug()<<"SystemInfo::slotBackupMyDocs()";

    QString openFilesPath = ui.myDocumentsLineEdit->text();
    QString selectedFilter;
    QFileDialog::Options options;
    //options |= QFileDialog::DontUseNativeDialog;

    QStringList files = QFileDialog::getOpenFileNames(
            this, tr("Select files to be backup"),
            openFilesPath,
            tr("All Files (*);;Excel Files (*.xls);;Word Files (*.doc)"),
            &selectedFilter,
            options);
    if (files.count()) {
        copyFiles(files);
    }

}

void SystemInfo::slotBackupEmails(){
    qDebug()<<"SystemInfo::slotBackupEmails()";

    QString openFilesPath = QDir(ui.emailStoreRootLineEdit->text()).path();
    QString selectedFilter;
    QFileDialog::Options options;
    //options |= QFileDialog::DontUseNativeDialog;

    QStringList files = QFileDialog::getOpenFileNames(
            this, tr("Select files to be backup"),
            openFilesPath,
            tr("All Files (*);;DBX Files (*.dbx)"),
            &selectedFilter,
            options);
    if (files.count()) {
        copyFiles(files);
    }

}

void SystemInfo::copyFiles(const QStringList &list){
    qDebug()<<"SystemInfo::copyFiles(const QStringList &list)";

    QFileDialog::Options options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
    //options |= QFileDialog::DontUseNativeDialog;
    QString directory = QFileDialog::getExistingDirectory(this,
                                                          tr("Select target directory"),
                                                          QDir::rootPath(),
                                                          options);
    if (directory.isEmpty()){
    	statusBar()->showMessage(tr("Operation 'My Documents Backup' canceled!"));
        return;
    }else if(directory == QFileInfo(list[0]).dir().path()){
    	QMessageBox::critical(this,tr("Error"),tr("Source and target directory should not be the same!"));
    	statusBar()->showMessage(tr("Source and target directory should not be the same!"));
    	return;
    }

    int i = 0;
    slotResetStatusBar(true);
    progressBar->setRange(0,list.count());

    foreach(QString file, list){
    	statusBar()->showMessage(tr("Copying %1").arg(file));

    	copyFile(file, directory + "/" + QFileInfo(file).fileName(), true);

    	progressBar->setValue(++i);
    	qApp->processEvents();
    }

    slotResetStatusBar(false);
    statusBar()->showMessage(tr("Operation 'My Documents Backup' Done."));

}


bool SystemInfo::copyFile(const QString &source, const QString &target, bool confirmFileExists){
    qDebug()<<"SystemInfo::copyFile(....)";

    if(QFile(target).exists()){
        if(confirmFileExists){
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, tr("File Already Exists!"), tr("File '%1' Already Exists!<br>Overwrite it?").arg(target),
                                          QMessageBox::Yes | QMessageBox::No);
            if (reply == QMessageBox::No) {
                return false;
            }
        }


        if(!QFile::remove(target)){
            QMessageBox::StandardButton reply;
            reply = QMessageBox::critical(this, tr("Fatal Error"),
                                          tr("Can not remove the target file %1").arg(target),
                                          QMessageBox::Retry | QMessageBox::Ignore);
            if (reply == QMessageBox::Retry){
                copyFile(source, target, false);
            }else{
                return false;
            }
        }

    }


    return QFile::copy(source, target);

}

void SystemInfo::slotUploadSystemInfo(){
    qDebug()<<"SystemInfo::slotUploadSystemInfo()";

    if(!slotIsPropertyNOValid()){
        QMessageBox::critical(this, tr("Error"), tr("Invalid Property NO.!"));
        return;
    }


    remark = ui.remarkLineEdit->text();
    dept = ui.deptComboBox->currentText();
    area = ui.locationComboBox->currentText();
    motherboardMDate = ui.dateEditMotherboardDate->date();
    monitorMDate = ui.dateEditMonitorDate->date();

    if(pNo.isEmpty()){
        QMessageBox::critical(this, tr("Error"), tr("Property NO. Required! "));
        ui.idLineEdit->setFocus();
        return;
    }

    if(dept.isEmpty()){
        QMessageBox::critical(this, tr("Error"), tr("Department Information Required!"));
        ui.deptComboBox->setFocus();
        return;
    }

    if(ui.osVersionLineEdit->text().trimmed().toUpper() == "WIN_7"){
        if(!ui.lineEditServiceNumber->isEnabled()){
            ui.lineEditServiceNumber->setEnabled(true);
        }
        if(ui.lineEditServiceNumber->text().trimmed().isEmpty()){
            QMessageBox::critical(this, tr("Error"), tr("Service Number Required!"));
            ui.lineEditServiceNumber->setFocus();
            return;
        }
    }


    if(ui.cpuLineEdit->text().trimmed().isEmpty() ||ui.motherboardLineEdit->text().trimmed().isEmpty()){
        QMessageBox::critical(this, tr("Error"), tr("Devices Information Required! "));
        return;
    }



    slotGetAllInfo();

    slotUploadSystemInfoToSitoyDBServer();


    //if(isUploaded){
    //    QMessageBox::critical(this,tr("Operation Canceled"),tr("Data had been uploaded to server!"));
    //    return;
    //}

    //QApplication::setOverrideCursor(Qt::WaitCursor);

    //DatabaseConnecter dc(this);
    if(!dc->isDatabaseOpened(MYSQL_DB_CONNECTION_NAME,
                             REMOTE_SITOY_COMPUTERS_DB_DRIVER,
                             REMOTE_SITOY_COMPUTERS_DB_SERVER_HOST,
                             REMOTE_SITOY_COMPUTERS_DB_SERVER_PORT,
                             REMOTE_SITOY_COMPUTERS_DB_USER_NAME,
                             REMOTE_SITOY_COMPUTERS_DB_USER_PASSWORD,
                             REMOTE_SITOY_COMPUTERS_DB_NAME,
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


    if(recordExists){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr("Record Already Exists!"), tr("Record '%1' Already Exists!<br>Update it?").arg(pNo),
                                      QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::No) {
            return;
        }

        query.prepare(QString("UPDATE systeminfo SET OS = :OS, InstallationDate = :InstallationDate, Workgroup = :Workgroup, ComputerName = :ComputerName, WindowsDir = :WindowsDir, DrivesInfo = :DrivesInfo, UserName = :UserName, IsAdmin = :IsAdmin, MyDocuments = :MyDocuments, EmailStoreRoot = :EmailStoreRoot,"
                              "CPU = :CPU, DMIUUID = :DMIUUID, MotherboardName = :MotherboardName, Chipset = :Chipset, Memory = :Memory, Video = :Video, Monitor = :Monitor, Audio = :Audio, Network1 = :Network1, Network2 = :Network2, Remark = :Remark, PropertyNO = :PropertyNO, Dept = :Dept, Location = :Location, MotherboardMDate = :MotherboardMDate, MonitorMDate = :MonitorMDate, SystemUsers = :SystemUsers, ServiceNumber = :ServiceNumber "
                              " WHERE PropertyNO LIKE '%1'").arg(pNo));

    }else{
        query.prepare("INSERT INTO systeminfo (OS, InstallationDate, Workgroup, ComputerName, WindowsDir, DrivesInfo, UserName, IsAdmin, MyDocuments, EmailStoreRoot, CPU, DMIUUID, MotherboardName, Chipset ,Memory, Video, Monitor, Audio, Network1, Network2, Remark, PropertyNO, Dept, Location, MotherboardMDate, MonitorMDate, SystemUsers, ServiceNumber) "
                      "VALUES (:OS, :InstallationDate, :Workgroup, :ComputerName, :WindowsDir, :DrivesInfo, :UserName, :IsAdmin, :MyDocuments, :EmailStoreRoot, :CPU, :DMIUUID, :MotherboardName, :Chipset , :Memory, :Video, :Monitor, :Audio, :Network1, :Network2, :Remark, :PropertyNO, :Dept, :Location, :MotherboardMDate, :MonitorMDate, :SystemUsers, :ServiceNumber)");

    }

    //	     query.prepare("INSERT INTO systeminfo (OS, InstallationDate, Workgroup, ComputerName, WindowsDir, DrivesInfo, UserName, IsAdmin, MyDocuments, EmailStoreRoot, CPU, DMIUUID, MotherboardName, Chipset ,Memory, Video, Monitor, Audio, Network1, Network2, Remark, PropertyNO, Dept, Location) "
    //	                   "VALUES (:OS, :InstallationDate, :Workgroup, :ComputerName, :WindowsDir, :DrivesInfo, :UserName, :IsAdmin, :MyDocuments, :EmailStoreRoot, :CPU, :DMIUUID, :MotherboardName, :Chipset , :Memory, :Video, :Monitor, :Audio, :Network1, :Network2, :Remark, :PropertyNO, :Dept, :Location)");
    query.bindValue(":OS", os);
    query.bindValue(":InstallationDate", installationDate);
    query.bindValue(":Workgroup", workgroup);
    query.bindValue(":ComputerName", computerName);
    query.bindValue(":WindowsDir", windowsDir);
    query.bindValue(":DrivesInfo", drivesInfo.join(";*;"));
    query.bindValue(":UserName", userName);
    query.bindValue(":IsAdmin", (isAdmin?"1":"0"));
    query.bindValue(":MyDocuments", myDocuments);
    query.bindValue(":EmailStoreRoot", emailStoreRoot);
    query.bindValue(":CPU", cpu);
    query.bindValue(":DMIUUID", dmiUUID);
    query.bindValue(":MotherboardName", motherboardName);
    query.bindValue(":Chipset", chipset);
    query.bindValue(":Memory", memory);
    query.bindValue(":Video", video);
    query.bindValue(":Monitor", monitor);
    query.bindValue(":Audio", audio);
    query.bindValue(":Network1", network1Info.join(";*;"));
    query.bindValue(":Network2", network2Info.join(";*;"));
    query.bindValue(":Remark", remark );
    query.bindValue(":PropertyNO", pNo);
    query.bindValue(":Dept", dept);
    query.bindValue(":Location", area);
    query.bindValue(":MotherboardMDate", motherboardMDate.toString("yyyy-MM-dd"));
    query.bindValue(":MonitorMDate", monitorMDate.toString("yyyy-MM-dd"));
    query.bindValue(":SystemUsers", usersInfo.join(";*;"));
    query.bindValue(":ServiceNumber", serviceNumber);


    if(!query.exec()){
        QApplication::restoreOverrideCursor();
        QMessageBox::critical(this, QObject::tr("Fatal Error"), tr("Can not upload data to server! <br> %1").arg(query.lastError().text()));
        qCritical()<<QString("Can not upload data to server!");
        qCritical()<<QString("Error: %1").arg(query.lastError().text());
        statusBar()->showMessage(tr("Error! Can not upload data to server!"));

        return;
    }

    query.clear();

    QApplication::restoreOverrideCursor();

    //QSqlDatabase::removeDatabase(MYSQL_DB_CONNECTION_NAME);

    QMessageBox::information(this, tr("Done"), tr("Data has been uploaded to server!"));

    //isUploaded = true;
    statusBar()->showMessage(tr("Done! Data has been uploaded to server!"));


}



void SystemInfo::retranslateUi() {
    qDebug()<<"SystemInfo::retranslateUi()";

    ui.retranslateUi(this);

}




void SystemInfo::slotUploadSystemInfoToSitoyDBServer(){
    qDebug()<<"SystemInfo::slotUploadSystemInfoToSitoyDBServer()";


    //if(isUploadedToSitoyDB){
    //    QMessageBox::critical(this,tr("Operation Canceled"),tr("Data had been uploaded to server 200.200.200.2!"));
    //    return;
    //}

    if(!slotIsPropertyNOValid()){
        QMessageBox::critical(this, tr("Error"), tr("Invalid Property NO.!"));
        return;
    }

    //QApplication::setOverrideCursor(Qt::WaitCursor);

    //DatabaseConnecter dc(this);
    //QString connectionName = "200.200.200.2/MIS/PC";
    if(!dc->isDatabaseOpened(SITOY_MSSQLSERVER_DB_CONNECTION_NAME,
                             "QODBC",
                             "200.200.200.2",
                             1433,
                             "sa",
                             "sitoydb",
                             "mis",
                             HEHUI::M$SQLSERVER
                             )){
        QApplication::restoreOverrideCursor();
        QMessageBox::critical(this, tr("Fatal Error"), tr("Database Connection Failed! Upload Failed!"));
        qCritical() << QString("Error: Database Connection Failed! Upload Failed!");
        return;
    }


    QSqlDatabase db;
    db = QSqlDatabase::database(SITOY_MSSQLSERVER_DB_CONNECTION_NAME);
    QSqlQuery query(db);

    bool recordExistsInSitoyDB = false;
    QString queryString = QString("select _No  from PC where _No = '%1'").arg(pNo);

    if(!query.exec(queryString)){
        QMessageBox::information(this, "Error", query.lastError().text());
        return ;
    }

    if(query.first()){
        recordExistsInSitoyDB = true;
    }

//    if(query.size() > 1){
//        QMessageBox::critical(this, tr("Fatal Error"), tr("Duplicate Property No.!"));
//        statusBar()->showMessage(tr("Error! Duplicate No.!"));
//        qCritical()<<"Error! Duplicate No.!";
//        recordExistsInSitoyDB = true;
//    }else if(query.size() == 1){
//        statusBar()->showMessage(tr("Record exists in server 200.200.200.2!"));
//        recordExistsInSitoyDB = true;
//    }else{
//        recordExistsInSitoyDB = false;
//    }


    query.clear();

    if(recordExistsInSitoyDB){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr("Record Already Exists!"), tr("Record '%1' already exists in server 200.200.200.2!<br>Update it?").arg(pNo),
                                      QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::No) {
            return;
        }

        query.prepare(QString("UPDATE PC SET _No = :_No, CPU = :CPU, RAM = :RAM, HDD = :HDD, Remark = :Remark, Depart = :Depart, Area = :Area, Monitor = :Monitor, MainBoard = :MainBoard WHERE _No LIKE '%1'").arg(pNo));

    }else{
        query.prepare("INSERT INTO PC (_No, CPU, RAM, HDD, Remark, Depart, Area, Monitor, MainBoard) "
                      "VALUES (:_No, :CPU, :RAM, :HDD, :Remark, :Depart, :Area, :Monitor, :MainBoard)");

    }

    query.bindValue(":_No", pNo.left(20));
    query.bindValue(":CPU", cpu.left(20));
    query.bindValue(":RAM", memory.left(20));
    query.bindValue(":HDD", partitionsTotalSize.left(20));
    query.bindValue(":Remark", remark.left(200));
    query.bindValue(":Depart", dept.left(20));
    query.bindValue(":Area", area.left(20));
    query.bindValue(":Monitor", monitor.left(20));
    query.bindValue(":MainBoard", motherboardName.left(20));


    if(!query.exec()){
        QApplication::restoreOverrideCursor();
        QMessageBox::critical(this, QObject::tr("Fatal Error"), tr("Can not upload data to server 200.200.200.2! <br> %1").arg(query.lastError().text()));
        qCritical()<<QString("Can not upload data to server 200.200.200.2!");
        qCritical()<<QString("Error: %1").arg(query.lastError().text());
        statusBar()->showMessage(tr("Error! Can not upload data to server 200.200.200.2!"));

        return;
    }

    QApplication::restoreOverrideCursor();


    QMessageBox::information(this, tr("Done"), tr("Data has been uploaded to server 200.200.200.2!"));

    //isUploadedToSitoyDB = true;
    statusBar()->showMessage(tr("Done! Data has been uploaded to server 200.200.200.2!"));


}

void SystemInfo::slotQuerySystemInfo(){
    qDebug()<<"SystemInfo::slotQuerySystemInfo()";


    if(!slotIsPropertyNOValid()){
        QMessageBox::critical(this, tr("Error"), tr("Invalid Property NO.!"));
        return;
    }

    slotResetAllInfo();

    QApplication::setOverrideCursor(Qt::WaitCursor);


    //DatabaseConnecter dc(this);

    /*
        //QString connectionName = "200.200.200.2/MIS/PC";
        if(!dc.isDatabaseOpened(SITOY_MSSQLSERVER_DB_CONNECTION_NAME,
                                                "QODBC",
                                                "200.200.200.2",
                                                1433,
                                                "sa",
                                                "sitoydb",
                                                "mis",
                                                HEHUI::M$SQLSERVER
                                                )){
                QApplication::restoreOverrideCursor();
                QMessageBox::critical(this, tr("Fatal Error"), tr("Database Connection Failed! Upload Failed!"));
                qCritical() << QString("Error: Database Connection Failed! Upload Failed!");
                return;
        }


                QSqlDatabase db;
                        db = QSqlDatabase::database(SITOY_MSSQLSERVER_DB_CONNECTION_NAME);
*/

    if(!dc->isDatabaseOpened(MYSQL_DB_CONNECTION_NAME,
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
        return;
    }


    QSqlDatabase db;
    db = QSqlDatabase::database(MYSQL_DB_CONNECTION_NAME);



    QSqlQuery query(db);

    //QString queryString = QString("select _No, Remark, Depart, Area  from PC where _No like '%1'").arg(pNo);
    //QString queryString = QString("select PropertyNO, Remark, Dept, Location, MotherboardMDate, MonitorMDate  from systeminfo where PropertyNO like '%1'").arg(pNo);
    QString queryString = QString("select OS, InstallationDate, Workgroup, ComputerName, WindowsDir, DrivesInfo, UserName, IsAdmin, MyDocuments, EmailStoreRoot, CPU, DMIUUID, MotherboardName, Chipset ,Memory, Video, Monitor, Audio, Network1, Network2, Remark, Dept, Location, MotherboardMDate, MonitorMDate, SystemUsers, ServiceNumber  from systeminfo where PropertyNO like '%1'").arg(pNo);


    if(!query.exec(queryString)){
        QApplication::restoreOverrideCursor();
        QMessageBox::information(this, "Error", query.lastError().text());
        return;
    }

    QApplication::restoreOverrideCursor();

    if(query.size() > 1){
        QMessageBox::critical(this, tr("Fatal Error"), tr("Duplicate No.!"));
        statusBar()->showMessage(tr("Error! Duplicate No.!"));
        qCritical()<<"Error! Duplicate No.!";
        return;
    }else if(query.size() == 1){
        recordExists = true;
        query.first();

        os = query.value(0).toString();
        installationDate = query.value(1).toString();
        workgroup = query.value(2).toString();
        computerName = query.value(3).toString();
        windowsDir = query.value(4).toString();
        QStringList drivesInfoList = query.value(5).toString().split(";*;");


        userName = query.value(6).toString();
        isAdmin = query.value(7).toBool();
        myDocuments = query.value(8).toString();
        emailStoreRoot = query.value(9).toString();

        cpu = query.value(10).toString();
        dmiUUID = query.value(11).toString();
        motherboardName = query.value(12).toString();
        chipset = query.value(13).toString();
        memory = query.value(14).toString();

        video = query.value(15).toString();
        monitor = query.value(16).toString();
        audio = query.value(17).toString();

        network1Info = query.value(18).toString().split(";*;");
        network2Info = query.value(19).toString().split(";*;");

        remark = query.value(20).toString();
        dept = query.value(21).toString();
        area = query.value(22).toString();
        motherboardMDate = query.value(23).toDate();
        monitorMDate = query.value(24).toDate();

        QStringList usersInfoList = query.value(25).toString().split(";*;");
        serviceNumber = query.value(26).toString();



        ui.osVersionLineEdit->setText(os);
        ui.installationDateLineEdit->setText(installationDate);
        ui.computerNameLineEdit->setText(computerName);
        ui.workgroupLineEdit->setText(workgroup);

        ui.logicalDrivesComboBox->clear();
        ui.logicalDrivesComboBox->addItems(drivesInfoList);

        ui.comboBoxSystemUsers->clear();
        ui.comboBoxSystemUsers->addItems(usersInfoList);

        ui.osGroupBox->setEnabled(true);



        ui.userNamelineEdit->setText(userName);
        if(isAdmin){
            ui.isAdminlineEdit->setText(QString(tr("Yes")));
        }else{
            ui.isAdminlineEdit->setText(QString(tr("No")));
        }
        ui.myDocumentsLineEdit->setText(myDocuments);

        ui.emailStoreRootLineEdit->setText(emailStoreRoot);

        ui.userGroupBox->setEnabled(true);


        ui.cpuLineEdit->setText(cpu);
        ui.memoryLineEdit->setText(memory);
        ui.motherboardLineEdit->setText(motherboardName);
        ui.dmiUUIDLineEdit->setText(dmiUUID);
        ui.chipsetLineEdit->setText(chipset);
        ui.videoCardLineEdit->setText(video);
        ui.monitorLineEdit->setText(monitor);
        ui.audioLineEdit->setText(audio);

        if(network1Info.size() == 3){
            ui.adapter1NameLineEdit->setText(network1Info[0]);
            ui.adapter1HDAddressLineEdit->setText(network1Info[1]);
            ui.adapter1IPAddressLineEdit->setText(network1Info[2]);
        }

        if(network2Info.size() == 3){
            ui.adapter2NameLineEdit->setText(network2Info[0]);
            ui.adapter2HDAddressLineEdit->setText(network2Info[1]);
            ui.adapter2IPAddressLineEdit->setText(network2Info[2]);
        }

        ui.devicesInfoGroupBox->setEnabled(true);


        ui.toolButtonUpload->setEnabled(true);
        //ui.idLineEdit->setText(pNo);
        ui.remarkLineEdit->setText(remark);
        ui.remarkLineEdit->setEnabled(true);
        ui.deptComboBox->setCurrentIndex(ui.deptComboBox->findText(dept));
        ui.deptComboBox->setEnabled(true);
        //ui.locationComboBox->setCurrentIndex(ui.locationComboBox->findText(area));
        ui.locationComboBox->setEditText(area);
        ui.locationComboBox->setEnabled(true);

        ui.dateEditMotherboardDate->setDate(motherboardMDate);
        ui.dateEditMotherboardDate->setEnabled(true);
        ui.dateEditMonitorDate->setDate(monitorMDate);
        ui.dateEditMonitorDate->setEnabled(true);

        if(os.toUpper() == "WIN_7"){
            ui.lineEditServiceNumber->setEnabled(true);
            ui.lineEditServiceNumber->setText(serviceNumber);
        }


        //ui.idLineEdit->setText(pNo);

    }else{
        QMessageBox::critical(this, tr("Error"), tr("Record does not exist!"));
        recordExists = false;

    }



}

unsigned int SystemInfo::createPropertyNO(){

    QApplication::setOverrideCursor(Qt::WaitCursor);

    if(!dc->isDatabaseOpened(MYSQL_DB_CONNECTION_NAME,
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
        return 0;
    }


    QSqlDatabase db;
    db = QSqlDatabase::database(MYSQL_DB_CONNECTION_NAME);

    QSqlQuery query(db);

    QString queryString = QString("select PropertyNO  from systeminfo where ID = (%1)").arg("select max(ID)  from systeminfo");


    if(!query.exec(queryString)){
        QApplication::restoreOverrideCursor();
        QMessageBox::information(this, "Error", query.lastError().text());
        return 0;
    }

    QApplication::restoreOverrideCursor();

    if(query.size() > 1){
        QMessageBox::critical(this, tr("Fatal Error"), tr("Duplicate No.!"));
        statusBar()->showMessage(tr("Error! Duplicate No.!"));
        qCritical()<<"Error! Duplicate No.!";
        return 0;
    }else if(query.size() == 1){
        recordExists = true;
        query.first();
        QString noStr = query.value(0).toString();
        unsigned int no = noStr.right(4).toUInt();

        return no + 1;
    }

    return 0;
}

bool SystemInfo::slotIsPropertyNOValid(){
    qDebug()<<"SystemInfo::slotIsIDValid()";

    pNo = ui.comboBoxIDPart1->currentText() + "-"
    		+ ui.comboBoxIDPart2->currentText() + "-"
    		+ui.idLineEdit->text().trimmed();

    int pos = 0;
    QRegExpValidator rxValidator(this);
    QRegExp rx("\\bS(DG|YD)-[ABC]-\\d{4}");
    rxValidator.setRegExp(rx);
    //if(validator->validate(pNo, pos) != QValidator::Acceptable){
    if(rxValidator.validate(pNo, pos) != QValidator::Acceptable){
        //QMessageBox::critical(this, tr("Error"), tr("Invalid Property NO.!"));
        qDebug()<<"Invalid Property NO.!";
        ui.idLineEdit->setFocus();
        ui.idLineEdit->end(false);
        return false;
    }

    return true;

}

void SystemInfo::slotResetAllInfo()
{

    ui.osVersionLineEdit->clear();
    ui.installationDateLineEdit->clear();
    ui.computerNameLineEdit->clear();
    ui.workgroupLineEdit->clear();
    ui.logicalDrivesComboBox->clear();
    ui.osGroupBox->setEnabled(false);



    ui.userNamelineEdit->clear();
    ui.isAdminlineEdit->clear();
    ui.myDocumentsLineEdit->clear();
    ui.backupMyDocsToolButton->setEnabled(false);
    ui.emailStoreRootLineEdit->clear();
    ui.backupEmailToolButton->setEnabled(false);
    ui.emailFolderSizeLineEdit->clear();
    ui.biggestDBXLineEdit->clear();
    ui.tempDirLineEdit->clear();
    ui.ieTempDirLineEdit->clear();
    ui.cleanTempFilesToolButton->setEnabled(false);
    ui.userGroupBox->setEnabled(false);


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

    ui.toolButtonUpload->setEnabled(false);
    //ui.idLineEdit->setText("");
    ui.remarkLineEdit->clear();
    ui.deptComboBox->setCurrentIndex(0);
    ui.locationComboBox->setCurrentIndex(-1);
    ui.locationComboBox->clear();

    ui.dateEditMotherboardDate->setDate(ui.dateEditMotherboardDate->minimumDate());
    ui.dateEditMonitorDate->setDate(ui.dateEditMonitorDate->minimumDate());

    ui.lineEditServiceNumber->clear();
    ui.lineEditServiceNumber->setEnabled(false);


    ////////////////////////////////
    //OS Info
    os.clear();
    installationDate.clear();;
    workgroup.clear();
    computerName.clear();
    windowsDir.clear();

    drivesInfo.clear();

    //User Info
    userName.clear();
    isAdmin = false;

    myDocuments.clear();

    emailStoreRoot.clear();
    emailFolderSize = 0;
    biggestDBXFile.clear();

    tempDirSize = 0;
    ieTempDirSize = 0;

    //Devices Info
    cpu.clear();
    dmiUUID.clear();
    motherboardName.clear();
    chipset.clear();
    memory.clear();

    video.clear();
    monitor.clear();
    audio.clear();
    partitionsTotalSize.clear();

    network1Info.clear();
    network2Info.clear();


    //Other Info
    //pNo = ;
    remark.clear();
    dept.clear();
    area.clear();

    motherboardMDate = QDate();
    monitorMDate = QDate();

    usersInfo.clear();
    serviceNumber.clear();

    recordExists = false;

    updateGeometry();


}

void SystemInfo::slotGetAllInfo()
{

    //OS Info
    os = ui.osVersionLineEdit->text();
    installationDate = ui.installationDateLineEdit->text();
    workgroup = ui.workgroupLineEdit->text();
    computerName = ui.computerNameLineEdit->text();
    //windowsDir = ;

    drivesInfo.clear();
    for(int i = 0; i < ui.logicalDrivesComboBox->count(); i++){
        drivesInfo.append(ui.logicalDrivesComboBox->itemText(i));
    }

    //User Info
    userName = ui.userNamelineEdit->text();
    isAdmin = (ui.isAdminlineEdit->text() == QString(tr("Yes")))?true:false;

    myDocuments = ui.myDocumentsLineEdit->text();

    emailStoreRoot = ui.emailStoreRootLineEdit->text();
    emailFolderSize = ui.emailFolderSizeLineEdit->text().toInt();
    biggestDBXFile = ui.biggestDBXLineEdit->text();

    tempDirSize = ui.tempDirLineEdit->text().toInt();
    ieTempDirSize = ui.ieTempDirLineEdit->text().toInt();

    //Devices Info
    cpu = ui.cpuLineEdit->text();
    dmiUUID = ui.dmiUUIDLineEdit->text();
    motherboardName = ui.motherboardLineEdit->text();
    chipset = ui.chipsetLineEdit->text();
    memory = ui.memoryLineEdit->text();

    video = ui.videoCardLineEdit->text();
    monitor = ui.monitorLineEdit->text();
    audio = ui.audioLineEdit->text();
    //partitionsTotalSize = ;

    network1Info.clear();
    network2Info.clear();
    network1Info.append(ui.adapter1NameLineEdit->text());
    network1Info.append(ui.adapter1HDAddressLineEdit->text());
    network1Info.append(ui.adapter1IPAddressLineEdit->text());
    network2Info.append(ui.adapter2NameLineEdit->text());
    network2Info.append(ui.adapter2HDAddressLineEdit->text());
    network2Info.append(ui.adapter2IPAddressLineEdit->text());


    //Other Info
    //pNo = ;
    remark = ui.remarkLineEdit->text();
    dept = ui.deptComboBox->currentText();
    area = ui.locationComboBox->currentText();

    motherboardMDate = ui.dateEditMotherboardDate->date();
    monitorMDate = ui.dateEditMonitorDate->date();

    usersInfo.clear();
    for(int i = 0; i < ui.comboBoxSystemUsers->count(); i++){
        usersInfo.append(ui.comboBoxSystemUsers->itemText(i));
    }

    serviceNumber = ui.lineEditServiceNumber->text();


}

void SystemInfo::on_idLineEdit_editingFinished (){
    qDebug()<<"SystemInfo::on_idLineEdit_editingFinished()";

    if(!slotIsPropertyNOValid()){
        //QMessageBox::critical(this, tr("Error"), tr("Invalid Property NO.!"));
        return;
    }

    focusNextChild();

    QApplication::setOverrideCursor(Qt::WaitCursor);


    //DatabaseConnecter dc(this);

    /*
	//QString connectionName = "200.200.200.2/MIS/PC";
        if(!dc.isDatabaseOpened(SITOY_MSSQLSERVER_DB_CONNECTION_NAME,
						"QODBC",
						"200.200.200.2",
						1433,
						"sa",
						"sitoydb",
						"mis",
						HEHUI::M$SQLSERVER
						)){
		QApplication::restoreOverrideCursor();
		QMessageBox::critical(this, tr("Fatal Error"), tr("Database Connection Failed! Upload Failed!"));
		qCritical() << QString("Error: Database Connection Failed! Upload Failed!");
		return;
	}


		QSqlDatabase db;
                        db = QSqlDatabase::database(SITOY_MSSQLSERVER_DB_CONNECTION_NAME);
*/

    if(!dc->isDatabaseOpened(MYSQL_DB_CONNECTION_NAME,
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
        return;
    }


    QSqlDatabase db;
    db = QSqlDatabase::database(MYSQL_DB_CONNECTION_NAME);



    QSqlQuery query(db);

    //QString queryString = QString("select _No, Remark, Depart, Area  from PC where _No like '%1'").arg(pNo);
    QString queryString = QString("select PropertyNO, Remark, Dept, Location, MotherboardMDate, MonitorMDate  from systeminfo where PropertyNO like '%1'").arg(pNo);


    if(!query.exec(queryString)){
        QApplication::restoreOverrideCursor();
        QMessageBox::information(this, "Error", query.lastError().text());
        return;
    }

    QApplication::restoreOverrideCursor();

    if(query.size() > 1){
        QMessageBox::critical(this, tr("Fatal Error"), tr("Duplicate No.!"));
        statusBar()->showMessage(tr("Error! Duplicate No.!"));
        qCritical()<<"Error! Duplicate No.!";
        return;
    }else if(query.size() == 1){
        recordExists = true;
        statusBar()->showMessage(tr("Record '%1' already exists!").arg(pNo));


        //        query.first();
        //        //pNo = query.value(0).toString();
        //        remark = query.value(1).toString();
        //        dept = query.value(2).toString();
        //        area = query.value(3).toString();
        //        motherboardMDate = query.value(4).toDate();
        //        monitorMDate = query.value(5).toDate();
        //
        //        //ui.idLineEdit->setText(pNo);
        //        ui.remarkLineEdit->setText(remark);
        //        ui.deptComboBox->setCurrentIndex(ui.deptComboBox->findText(dept));
        //        //ui.locationComboBox->setCurrentIndex(ui.locationComboBox->findText(area));
        //        ui.locationComboBox->setEditText(area);
        //
        //        ui.dateEditMotherboardDate->setDate(motherboardMDate);
        //        ui.dateEditMonitorDate->setDate(monitorMDate);
        //
        //        ui.idLineEdit->setText(pNo);

    }else{
        recordExists = false;
        statusBar()->showMessage(tr("Record '%1' does not exist!").arg(pNo));

    }


    //QSqlDatabase::removeDatabase(SITOY_MSSQLSERVER_DB_CONNECTION_NAME);


}

void SystemInfo::on_deptComboBox_currentIndexChanged(const QString & text){
    qDebug()<<"SystemInfo::on_deptComboBox_currentIndexChanged(const QString & text)";

    ui.locationComboBox->clear();

    if(text == "Sales"){  
        QStringList locations;
        locations<<"Coach"<<"PS"<<"Ben"<<"LN"<<"Fossil";
        ui.locationComboBox->addItems(locations);
    }

    if(text == "Shop"){
        QStringList locations;
        locations<<"A"<<"B"<<"C"<<"D"<<"E"<<"F"<<"G";
        ui.locationComboBox->addItems(locations);
    }

    //QMessageBox::information(this, "", text);


}

void SystemInfo::on_dateEditMotherboardDate_dateChanged(const QDate &date){
    qDebug()<<"SystemInfo::on_dateEditMotherboardDate_dateChanged(const QDate &date)";

    if(slotIsPropertyNOValid()){
        return;
    }


    if(date.year() >= 2009){
    	ui.comboBoxIDPart2->setCurrentIndex(0);
      //ui.idLineEdit->setText("SDG-A-0");
    }else if(date.year()<2006){
    	ui.comboBoxIDPart2->setCurrentIndex(1);
        //ui.idLineEdit->setText("SDG-C-0");
    }else{
    	ui.comboBoxIDPart2->setCurrentIndex(2);
        //ui.idLineEdit->setText("SDG-B-0");
    }

    //ui.idLineEdit->setText("0");

    //ui.idLineEdit->end(false);
}


void SystemInfo::on_toolButtonQuery_clicked()
{
    ui.otherInfoGroupBox->setEnabled(false);

    slotQuerySystemInfo();

    ui.otherInfoGroupBox->setEnabled(true);

    ui.idLineEdit->setFocus();

}

void SystemInfo::on_toolButtonUpload_clicked()
{
    ui.otherInfoGroupBox->setEnabled(false);

    slotUploadSystemInfo();

    ui.otherInfoGroupBox->setEnabled(true);

}

void SystemInfo::on_toolButtonScan_clicked()
{
    ui.otherInfoGroupBox->setEnabled(false);

    slotScanSystem();

    ui.otherInfoGroupBox->setEnabled(true);

    ui.idLineEdit->setFocus();

}











