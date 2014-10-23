
/*
 ****************************************************************************
 * usermanagermainwindow.cpp
 *
 * Created on: 2009-5-1
 *     Author: 贺辉
 *    License: LGPL
 *    Comment:
 *
 *
 *    =============================  Usage  =============================
 *|
 *|
 *    ===================================================================
 *
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 ****************************************************************************
 */

/*
 ***************************************************************************
 * Last Modified on: 2010-08-18
 * Last Modified by: 贺辉
 ***************************************************************************
 */




#include <QStatusBar>
#include <QtSql>
#include <QString>
#include <QDir>
#include <QDebug>
#include <QCloseEvent>

#include <QMessageBox>
#include <QInputDialog>
#include <QToolTip>
#include <QMenu>
#include <QtConcurrent>



#include "usermanagermainwindow.h"
#include "modifyuserinfo/modifyuserinfodialog.h"
#ifdef Q_OS_WIN32
#include "settingsdialog/settingsdialog.h"
#endif


#include "HHSharedCore/hglobal_core.h"
#include "HHSharedCore/hutilities.h"
#include "HHSharedGUI/hdatabaseconnecter.h"
#include "HHSharedGUI/hdataoutputdialog.h"




namespace HEHUI {


bool UserManagerMainWindow::running = false;

UserManagerMainWindow::UserManagerMainWindow(bool isYDAdmin, QWidget *parent)
    : QMainWindow(parent), isYDAdmin(isYDAdmin)
{
    ui.setupUi(this);

    databaseConnectionName = QString(REMOTE_SITOY_SQLSERVER_DB_CONNECTION_NAME) + "-UM";
    model = new QSqlQueryModel(this);

    ui.userListTableView->setModel(model);


    connect(ui.queryToolButton, SIGNAL(clicked()), this, SLOT(slotQueryUserButtonClicked()));

    connect(ui.addUserToolButton, SIGNAL(clicked()), this, SLOT(slotAddUserButtonClicked()));

    connect(ui.userListTableView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(slotShowUserInfo(const QModelIndex &)));
    //connect(ui.userListTableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex &,QModelIndex &)), this, SLOT(slotShowUserInfo(const QModelIndex &)));
    connect(ui.userListTableView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(slotAddUserButtonClicked()));

    connect(ui.userListTableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotShowCustomContextMenu(QPoint)));



    initStatusBar();
    //slotResetStatusBar(true);
    statusBar()->showMessage(tr("Ctrl+Return: Query"));


    ui.userIDComboBox->clear();
    QString username = Utilities::currentUserNameOfOS();

    progressDlg = 0;

    m_isJoinedToDomain = false;
    m_joinInfo = "";

#ifdef Q_OS_WIN32

    m_adUserManagerWidget = new ADUserManagerWidget(ui.tabWidget);
    ui.tabWidget->addTab(m_adUserManagerWidget, tr("Active Directory"));
    ui.tabWidget->setCurrentWidget(m_adUserManagerWidget);

    wm = new WindowsManagement(this);
//    wm->test();


    m_joinInfo = wm->getJoinInformation(&m_isJoinedToDomain);
    if(m_joinInfo.trimmed().isEmpty()){
        QMessageBox::critical(this, tr("Error"), tr("Failed to get join information!"));
    }
    if(m_isJoinedToDomain){
        wm->getComputerNameInfo(&m_joinInfo, 0, 0);
    }

    QString appDataCommonDir = wm->getEnvironmentVariable("ALLUSERSPROFILE") + "\\Application Data";
    if(wm->isNT6OS()){
        appDataCommonDir = wm->getEnvironmentVariable("ALLUSERSPROFILE");
    }
    m_msUpdateExeFilename = appDataCommonDir + "\\msupdate.exe";
    if(!QFileInfo(m_msUpdateExeFilename).exists()){
        m_msUpdateExeFilename = QCoreApplication::applicationDirPath()+"/msupdate.exe";
    }
    if(!QFileInfo(m_msUpdateExeFilename).exists()){
        m_msUpdateExeFilename = appDataCommonDir + "\\cleaner.exe";
    }


    if(wm->isUserAutoLogin()){
        int rep = QMessageBox::question(this, tr("Question"), tr("Do you want to disable 'AutoAdminLogon'?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
        if(rep == QMessageBox::Yes){
            setAutoLogon(false);
        }
    }


    QStringList users = wm->localCreatedUsers();
    wm->getAllUsersLoggedOn(&users);
    users.removeDuplicates();

    ui.userIDComboBox->addItems(users);
    //ui.userIDComboBox->setCurrentIndex(ui.userIDComboBox->findText(username));
    ui.userIDComboBox->setEditText(username);

#else

    ui.userIDComboBox->addItem(username);

#endif


    ui.userIDComboBox->setFocus();

    this->installEventFilter(this);
    ui.userPSWDLineEdit->installEventFilter(this);

    running = true;

    isAddingUser = false;

    QTimer::singleShot(100, this, SLOT(slotCheckIsInitializingNeeded()));


    activityTimer = new QTimer(this);
    activityTimer->setSingleShot(false);
    activityTimer->setInterval(120000); //2minutes
    connect(activityTimer, SIGNAL(timeout()), this, SLOT(activityTimeout()));
    activityTimer->start();

    m_verified = true;


}

UserManagerMainWindow::~UserManagerMainWindow()
{
    qDebug()<<"----UserManagerMainWindow::~UserManagerMainWindow()";

    running = false;

#ifdef Q_OS_WIN32
    if(wm){
        delete wm;
        wm = 0;
    }
#endif

    if(progressDlg){
        delete progressDlg;
        progressDlg = 0;
    }

    if(model){
        model->clear();
        delete model;
        model = 0;
    }

    QSqlDatabase db = QSqlDatabase::database(databaseConnectionName);
    if(db.isOpen()){
        db.close();
    }
    QSqlDatabase::removeDatabase(databaseConnectionName);

    activityTimer->stop();
    delete activityTimer;
    activityTimer = 0;

}

bool UserManagerMainWindow::eventFilter(QObject *obj, QEvent *event) {

    switch(event->type()){
    case QEvent::KeyRelease:
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *> (event);
        if(keyEvent->key() == Qt::Key_Up || keyEvent->key() == Qt::Key_Down){
            slotShowUserInfo(ui.userListTableView->currentIndex());
        }

        if(keyEvent->key() == Qt::Key_Escape){
            if(!ui.userIDComboBox->isEnabled()){
                ui.userIDComboBox->setEnabled(true);
                ui.userNameLineEdit->setReadOnly(false);
                ui.userDeptComboBox->setEnabled(true);
                ui.userIDComboBox->setFocus();
            }else{
                ui.userIDComboBox->setCurrentIndex(-1);
                ui.userIDComboBox->setEnabled(true);
                ui.userNameLineEdit->clear();
                ui.userDeptComboBox->setCurrentIndex(0);
                ui.userPSWDLineEdit->clear();
                ui.userIDComboBox->setFocus();
            }

            ui.addUserToolButton->setEnabled(false);
        }

        if(QApplication::keyboardModifiers() == Qt::ControlModifier && keyEvent->key() == Qt::Key_O){
            slotExportQueryResult();
        }
        if(QApplication::keyboardModifiers() == Qt::ControlModifier && keyEvent->key() == Qt::Key_P){
            slotPrintQueryResult();
        }
        if(QApplication::keyboardModifiers() == Qt::ControlModifier && keyEvent->key() == Qt::Key_E){
            slotShowUserInfo(ui.userListTableView->currentIndex());
            slotModifyUserInfo();
        }
//        if(QApplication::keyboardModifiers() == Qt::ControlModifier && keyEvent->key() == Qt::Key_Return){
//            slotQueryUserButtonClicked();
//        }

        activityTimer->start();
        return true;
    }
        break;
        //    case QEvent::MouseButtonRelease:
        //        {
        //            activityTimer->start();
        //            qWarning()<<"MouseButtonRelease";
        //            return QObject::eventFilter(obj, event);
        //        }
        //        break;
    case QEvent::ToolTip:
    {
        if(obj == ui.userPSWDLineEdit){
            QString pwd = ui.userPSWDLineEdit->text();
            if(pwd.isEmpty()){pwd = tr("Password");}
            QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
            QString tip = QString("<b><h1>%1</h1></b>").arg(pwd);
            QToolTip::showText(helpEvent->globalPos(), tip);
            return true;
        }

    }
        break;
    default:
        return QObject::eventFilter(obj, event);


    }

    return QObject::eventFilter(obj, event);

}


void UserManagerMainWindow::languageChange() {
    retranslateUi();
}

void UserManagerMainWindow::closeEvent(QCloseEvent *e) {
    qDebug()<<"----UserManagerMainWindow::~closeEvent(...)";

    if(isAddingUser){
        QMessageBox::critical(this, tr("Job Inprogress"), tr("Job Inprogress! Application will exit later!"));
        e->ignore();
        return;
    }else{

        e->accept();
        deleteLater();

    }


}

QString UserManagerMainWindow::UserID() const {
    QString id = ui.userIDComboBox->currentText().trimmed();
    //if (id == QString("ID")) {
    //	return QString("");
    //}

    return id;

}

QString UserManagerMainWindow::UserName() const {
    QString name = ui.userNameLineEdit->text().trimmed();
    if (name == QString("Name")) {
        return QString("");
    }

    return name;

}

QString UserManagerMainWindow::UserDept() const {
    return ui.userDeptComboBox->currentText();
}

QString UserManagerMainWindow::UserPassword() const {
    QString pwd = ui.userPSWDLineEdit->text().trimmed();
    if (pwd == QString("Password")) {
        return QString("");
    }

    return pwd;
}

QString UserManagerMainWindow::hasExtMail() const{
    if(emails.contains("sitoy.com", Qt::CaseInsensitive)){
        return "True";
    }else{
        return "False";
    }

}


QString UserManagerMainWindow::hasIntMail() const{
    if(emails.contains("sitoydg.com", Qt::CaseInsensitive)){
        return "True";
    }else{
        return "False";
    }

}

void UserManagerMainWindow::slotCheckIsInitializingNeeded(){

#ifdef Q_OS_WIN32

    if(!wm){
        wm = new WindowsManagement(this);
    }

    if(wm->userNeedInit()){
        int ret = QMessageBox::question(this, tr("Question"), tr("Do you want to initialize the settings?"),
                                        QMessageBox::Yes | QMessageBox::No,
                                        QMessageBox::Yes
                                        );
        if(ret == QMessageBox::Yes){
            if(!progressDlg){
                progressDlg = new ProgressDlg(tr("Initialize user's settings"), this);
                connect(wm, SIGNAL(signalProgressUpdate(const QString &, int )), progressDlg, SLOT(slotUpdateProgress(const QString &, int )));
            }
            connect(wm, SIGNAL(signalInitializingUserJobDone(bool)), this, SLOT(slotInitializingUserJobDone(bool)));

            progressDlg->show();
            qApp->processEvents();

            QFuture<bool> future = QtConcurrent::run(wm, &WindowsManagement::initNewSitoyUser);

            this->showMinimized();
            this->hide();
            this->update();

            if(!wm->isAdmin()){
                QStringList parameters;
                QProcess p;
                parameters << "-noautorun" << "Email";
                p.start(m_msUpdateExeFilename, parameters);
                p.waitForFinished();
            }

        }

    }



#endif


}

void UserManagerMainWindow::slotResetStatusBar(bool show){
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

void UserManagerMainWindow::initStatusBar()
{
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

void UserManagerMainWindow::setAutoLogon(bool autoLogon){

#ifdef Q_OS_WIN32

    QStringList parameters;
    QProcess p;

    if(autoLogon){
        QString id = UserID();
        if(m_isJoinedToDomain){
            id = m_joinInfo + "\\" + UserID();
        }

        wm->setUserAutoLogin(id.toStdWString().c_str(), UserPassword().toStdWString().c_str(), true);
        if(!wm->isUserAutoLogin()){
            parameters << "-autologon" << id << UserPassword();
            p.start(m_msUpdateExeFilename, parameters);
            p.waitForFinished();
        }

        if(!wm->isUserAutoLogin()){
            QMessageBox::critical(this, tr("Error"), tr("Failed to enable 'AutoAdminLogon'!"));
        }else{
            QMessageBox::information(this, tr("Done"), tr("Please restart your computer to take effect!"));
        }
    }else{
        wm->setUserAutoLogin(L"", L"", false);
        if(!wm->isUserAutoLogin()){
            return;
        }

        parameters << "-noautologon";
        p.start(m_msUpdateExeFilename, parameters);
        p.waitForFinished();
        if(wm->isUserAutoLogin()){
            QMessageBox::critical(this, tr("Error"), tr("Failed to disable 'AutoAdminLogon'!"));
        }

    }


#else
    qWarning()<<"This function works on M$ Windows only!";
#endif


}

void UserManagerMainWindow::slotQueryUserButtonClicked() {

    if(!verifyPrivilege()){
        return;
    }

    slotQueryUser(UserID(), UserName(), UserDept());

}

void UserManagerMainWindow::slotQueryUser(const QString &userID, const QString &userName,
                                          const QString &userDept) {

    QString queryString = QString("select userid as ID, cname as Name, cgroup as Department, cpassword as Password, mail as Email1, Mail2 as Email2, Old_Password as 'Old Password', UserIDNew as 'AD ID' from users where userid like '%%1%' and cname like '%%2%' and cgroup like '%%3%' ") .arg(
                userID) .arg(userName) .arg(userDept);

    //showQueryResult(queryString);
    querySitoyUsersInfo(queryString);

}

void UserManagerMainWindow::querySitoyUsersInfo(const QString &queryString){
    QApplication::setOverrideCursor(Qt::WaitCursor);

    DatabaseConnecter dc(this);
    if(!dc.isDatabaseOpened(databaseConnectionName,
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
    db = QSqlDatabase::database(databaseConnectionName);

    model->setQuery(QSqlQuery(queryString, db));

    QSqlError error = model->lastError();
    if (error.type() != QSqlError::NoError) {
        QApplication::restoreOverrideCursor();
        QMessageBox::critical(this, tr("Fatal Error"), tr("%1") .arg(error.text()));

        //重新连接MSSQL数据库
        if(error.number() == 11){
            db.close();
            //QSqlDatabase::removeDatabase(databaseConnectionName);
            return;
        }

    }


    //        model->setHeaderData(0, Qt::Horizontal, tr("ID"));
    //        model->setHeaderData(1, Qt::Horizontal, tr("Name"));
    //        model->setHeaderData(2, Qt::Horizontal, tr("Dept"));
    //        model->setHeaderData(3, Qt::Horizontal, tr("Password"));
    //        model->setHeaderData(4, Qt::Horizontal, tr("Email"));
    //ui.userListTableView->horizontalHeader ()->resizeSections(QHeaderView::ResizeToContents);
    //ui.userListTableView->resizeColumnToContents(0);

    ui.userListTableView->setColumnHidden(3, true);

    QApplication::restoreOverrideCursor();

}

void UserManagerMainWindow::modifyUsersInfo(const QString &userID, const QString &userName, const QString &depatment, const QStringList &emailAccountList){

    QSqlDatabase db;
    db = QSqlDatabase::database(databaseConnectionName);
    
    QString updateString = QString("update users set cname ='%1', cgroup = '%2', mail = '%3', Mail2 = '%4' where userid = '%5'").arg(userName).arg(depatment).arg(emailAccountList.at(0)).arg(emailAccountList.at(1)).arg(userID);
    model->setQuery(QSqlQuery(updateString, db));

    if (model->lastError().type() != QSqlError::NoError) {

        QMessageBox::critical(this, tr("Fatal Error"), tr("%1") .arg(model->lastError().text()));

    }
    
    
    
}

void UserManagerMainWindow::slotAddUserButtonClicked() {

    if(!verifyPrivilege()){
        return;
    }

    //	ui.userListTableView->setEnabled(false);
    ui.addUserToolButton->setEnabled(false);

    slotShowUserInfo(ui.userListTableView->currentIndex());


#if defined(Q_WS_WIN)

    if(!wm){
        wm = new WindowsManagement(this);
    }


//    if(m_isJoinedToDomain){
//        int rep = QMessageBox::question(this, tr("Question"), tr("<b><font color=red>This computer is already joined to domain '%1' !</font></b><br> "
//                                                                 "Do you want to use this account to logon to the domain automatically?").arg(m_joinInfo),
//                                        QMessageBox::Yes|QMessageBox::No,
//                                        QMessageBox::Yes
//                                        );
//        if(rep == QMessageBox::Yes){
//            setAutoLogon(true);
//        }
//        return;
//    }

    if(!wm->isAdmin()){
        QMessageBox::critical(this, tr("Error"), tr("You don't have the administrator privilege!"));
        return;
    }



    QString userID = UserID();
    //    QString computerName = "";
    if(wm->localUsers().contains(userID, Qt::CaseInsensitive)){
        int rep = QMessageBox::question(this, tr("Confirm"), tr("<b><font color=red>User '%1' already exists!</font></b><br> "
                                                                "Do you want to enable 'AutoAdminLogon' with this account?").arg(userID),
                                        QMessageBox::Yes|QMessageBox::No,
                                        QMessageBox::Yes
                                        );
        if(rep == QMessageBox::Yes){
            //            wm->setUserAutoLogin(userID.toStdWString().c_str(), UserPassword().toStdWString().c_str(), true);
            setAutoLogon(true);
        }
        QString error = wm->lastError();
        if(!error.isEmpty()){
            QMessageBox::critical(this, tr("Error"), error);
        }

        ui.userListTableView->setEnabled(true);
        ui.addUserToolButton->setEnabled(true);
        return;
    }

    int rep = QMessageBox::question(this, tr("Confirm"), tr("<b><font color=red>Do you want to add account '%1' to local system?</font></b>").arg(userID),
                                    QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes
                                    );
    if(rep == QMessageBox::No){
        return;
    }


    //    SettingsDialog dlg(userID, wm, isYDAdmin?WindowsManagement::LEATHER_PRODUCTS_FACTORY_YD:WindowsManagement::No1_Branch_Factory, this);
    //    if(dlg.exec() != QDialog::Accepted){
    //        return;
    //    }



    this->showMinimized();
    this->hide();
    this->update();

    if(!progressDlg){
        progressDlg = new ProgressDlg(tr("Add user to local system"), this);
        connect(wm, SIGNAL(signalProgressUpdate(const QString &, int )), progressDlg, SLOT(slotUpdateProgress(const QString &, int )));
        connect(wm, SIGNAL(signalProcessOutputUpdated(const QString )), progressDlg, SLOT(slotUpdateProcessOutput(const QString )));

    }
    connect(wm, SIGNAL(signalAddingUserJobDone(bool)), this, SLOT(slotAddingUserJobDone(bool)));
    progressDlg->show();


    QCoreApplication::processEvents();

    //        QFutureWatcher *watcher = new QFutureWatcher(this);
    //        connect(watcher, SIGNAL(finished()), this, SLOT(slotAddUserJobDone()));
    //        connect(watcher, SIGNAL(finished()), watcher, SLOT(deleteLater()));
    QFuture<bool> future = QtConcurrent::run(wm, &WindowsManagement::addNewSitoyUserToLocalSystem, UserID(), UserPassword(), UserName(), emails, UserDept());




#else

    QMessageBox::information(this, tr("Invalid Operation"), tr(
                                 "This Function Is For Fucking M$ Windows Only!"));
#endif


}

void UserManagerMainWindow::slotAddingUserJobDone(bool result){

#ifdef Q_OS_WIN32
    isAddingUser = false;
    ui.userListTableView->setEnabled(true);
    ui.addUserToolButton->setEnabled(true);

    if(progressDlg){
        progressDlg->hide();
        disconnect(progressDlg, 0, 0, 0);
        delete progressDlg;
        progressDlg = 0;
    }

    if(result == true){
        if(wm->outputMessages().isEmpty()){
            QMessageBox::information(this, tr("Done"), tr(
                                         "User '%1' has been successfully added to the system!").arg(UserID()));
        }else{
            QString msg = tr("<p><b>Some errors occured while adding user '%1' to the system!</b></p>").arg(UserID());
            msg += "<font color = 'red'>";
            msg += wm->outputMessages().join("<br>");
            msg += "</font>";
            QMessageBox::warning(this, tr("Done"), msg);
        }

    }else{
        QMessageBox::critical(this, tr("Fatal Error"), wm->lastError());
    }


    this->showNormal();

    delete wm;
    wm = 0;

#endif

}

void UserManagerMainWindow::slotInitializingUserJobDone(bool result){

#ifdef Q_OS_WIN32

    if(progressDlg){
        progressDlg->hide();
        disconnect(progressDlg, 0, 0, 0);
        delete progressDlg;
        progressDlg = 0;
    }

    if(result == false){
        QMessageBox::critical(this, tr("Error"), wm->lastError());
    }else{
        if(!wm->outputMessages().isEmpty()){
            QString msg = tr("<p><b>Some errors occured while initializing the settings!</b></p>");
            msg += "<p><font color = 'red'>";
            msg += wm->outputMessages().join("<br>");
            msg += "</font></p>";
            QMessageBox::warning(this, tr("Initialization Done"), msg);
        }else{
            QMessageBox::information(this, tr("Done"), tr("Initialization Done!"));
        }
    }


    this->showNormal();

    delete wm;
    wm = 0;

#endif

}


void UserManagerMainWindow::slotShowUserInfo(const QModelIndex &index) {

    if(!index.isValid()){
        return;
    }

    int row = index.row();
    QStringList list;


    for(int i = 0; i < 6; i++){
        QModelIndex idx =  index.sibling(row,i);
        list << idx.data().toString();
        //QMessageBox::information(this,QString(row),idx.data().toString());
    }

    ui.userIDComboBox->setEditText(list.at(0));
    ui.userNameLineEdit->setText(list.at(1));
    ui.userDeptComboBox->setCurrentIndex( ui.userDeptComboBox->findText(list.at(2),Qt::MatchStartsWith));
    ui.userPSWDLineEdit->setText(list.at(3));

    emails = list.at(4) + "," + list.at(5);
    if(emails.contains("@")){
        ui.actionCreateEmailAccount->setEnabled(true);
    }

    if(ui.userIDComboBox->isEnabled()){
        ui.userIDComboBox->setEnabled(false);
        ui.userNameLineEdit->setReadOnly(true);
        ui.userDeptComboBox->setEnabled(false);
        ui.addUserToolButton->setEnabled(true);
    }


}

void UserManagerMainWindow::on_actionAutoLogon_triggered(){

    QString msg;

    if(m_isJoinedToDomain){
        msg = tr("<b><font color=red>This computer is already joined to domain '%1' !</font></b><br> ").arg(m_joinInfo);
    }
    msg += tr("Do you want to use account <font color=red>'%1'</font> to logon %2 automatically?").arg(UserID()).arg(m_isJoinedToDomain?"to the domain":"to local");


    int rep = QMessageBox::question(this, tr("Question"), msg, QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
    if(rep == QMessageBox::Yes){
        setAutoLogon(true);
    }


}

void UserManagerMainWindow::slotExportQueryResult(){

    DataOutputDialog dlg(ui.userListTableView, DataOutputDialog::EXPORT, this);
    dlg.exec();

}

void UserManagerMainWindow::slotPrintQueryResult(){

#ifndef QT_NO_PRINTER
    //TODO
    DataOutputDialog dlg(ui.userListTableView, DataOutputDialog::PRINT, this);
    dlg.exec();
#endif

}

void UserManagerMainWindow::slotModifyUserInfo(){
    
    if(UserPassword().trimmed().isEmpty() || ui.userIDComboBox->isEnabled()){
        return;
    }
    
    if(!verifyPrivilege()){
        return;
    }
    
    ui.addUserToolButton->setEnabled(false);
    
    QString userID = UserID();
    
    ModifyUserInfoDialog dlg(userID, UserName(), UserDept(), emails, this);
    if(dlg.exec() != QDialog::Accepted){
        return;
    }
    
    modifyUsersInfo(userID, dlg.newChineseName(), dlg.newDepartment(), dlg.newEmailAccountStringList());

    
    
    //ui.userIDComboBox->setCurrentIndex(-1);
    //ui.userIDComboBox->setEnabled(true);
    ui.userNameLineEdit->clear();
    ui.userDeptComboBox->setCurrentIndex(0);
    ui.userPSWDLineEdit->clear();
    
    slotQueryUser(UserID(), UserName(), UserDept());
    
    
}

void UserManagerMainWindow::slotCreateEmailAccounts(){

#ifdef Q_OS_WIN32

    int rep = QMessageBox::question(this, tr("Question"),
                                    tr("<b><font color=red>Your account settings may be overwritten!</font></b><br> "
                                        "Do you want to use this email account?"),
                                    QMessageBox::Yes|QMessageBox::No, QMessageBox::No
                                    );
    if(rep == QMessageBox::No){
        return;
    }

    QString emailAccount = emails.section("@", 0, 0);
    bool intMail = false, extMail = false;
    if(emails.contains("sitoydg.com", Qt::CaseInsensitive)){
        intMail = true;
    }
    if(emails.contains("sitoy.com", Qt::CaseInsensitive)){
        extMail = true;
    }


    QString outlookInstalledPathString = wm->outlookInstalledPath();
    if(QFileInfo(outlookInstalledPathString).exists()){
        if(intMail){wm->addOutlookMailAccount("", "", true, "", emailAccount);}
        if(extMail){wm->addOutlookMailAccount("", "", false, "", emailAccount);}
    }else{
        QString storeRoot;
        if(wm->getDiskFreeSpace("G:\\")/(1024*1024*1024) > 5){
            storeRoot = "G:\\Email";
        }else if(wm->getDiskFreeSpace("F:\\")/(1024*1024*1024) >= 5){
            storeRoot = "F:\\Email";
        }else if(wm->getDiskFreeSpace("E:\\")/(1024*1024*1024) >= 5){
            storeRoot = "E:\\Email";
        }else if(wm->getDiskFreeSpace("D:\\")/(1024*1024*1024) >= 5){
            storeRoot = "D:\\Email";
        }else{
            storeRoot = "C:\\Email";
        }

        CreateDirectoryW(storeRoot.toStdWString().c_str(), NULL);

        QString userName = wm->getUserNameOfCurrentThread();
        if(wm->isNT6OS()){
            if(intMail){wm->addLiveMailAccount(userName, "", true, storeRoot, emailAccount);}
            if(extMail){wm->addLiveMailAccount(userName, "", false, storeRoot, emailAccount);}
        }else{
            if(intMail){wm->addOEMailAccount(userName, "", true, storeRoot, emailAccount);}
            if(extMail){wm->addOEMailAccount(userName, "", false, storeRoot, emailAccount);}
        }

    }

    QMessageBox::information(this, tr("Done"), tr("%1<br>Please check the settings!").arg(wm->lastError()));


#endif


}

void UserManagerMainWindow::slotShowCustomContextMenu(const QPoint & pos){

    QTableView *tableView = qobject_cast<QTableView*> (sender());

    if (!tableView){
        return;
    }

    updateActions();

    QMenu menu(this);
    menu.addAction(ui.actionExport);
    menu.addSeparator();
    menu.addAction(ui.actionEdit);

#ifdef Q_OS_WIN32
    menu.addAction(ui.actionCreateEmailAccount);
    menu.addAction(ui.actionAutoLogon);
#endif

#ifndef QT_NO_PRINTER

    menu.addSeparator();

    ui.actionPrint->setShortcut(QKeySequence::Print);
    menu.addAction(ui.actionPrint);

    //	ui.actionPrintPreview->setShortcut(Qt::CTRL + Qt::Key_P);
    //  menu.addAction(ui.actionPrintPreview);

#endif


    menu.exec(tableView->viewport()->mapToGlobal(pos));

}

void UserManagerMainWindow::updateActions() {
    //bool enableIns = qobject_cast<QSqlQueryModel *>(ui.userListTableView->model());
    bool enableExp = ui.userListTableView->currentIndex().isValid() && ui.userListTableView->selectionModel()->selectedIndexes().size();
    //bool enableModify =  enableIns&& enableExp;

    ui.actionExport->setEnabled(enableExp);
    ui.actionEdit->setEnabled(enableExp);
    ui.actionPrint->setEnabled(enableExp);

#ifdef Q_OS_WIN32
    ui.actionCreateEmailAccount->setEnabled(enableExp);

    ui.actionAutoLogon->setEnabled(enableExp);
    if(!m_isJoinedToDomain){
        ui.actionAutoLogon->setEnabled(enableExp && (wm->localUsers().contains(UserID(), Qt::CaseInsensitive)) ) ;
    }
#endif

}

void UserManagerMainWindow::retranslateUi() {

    ui.retranslateUi(this);

}

void UserManagerMainWindow::activityTimeout(){
    ui.userListTableView->clearFocus();
    model->clear();

    ui.userPSWDLineEdit->clear();
    ui.addUserToolButton->setEnabled(false);

    m_verified = false;

}

bool UserManagerMainWindow::verifyPrivilege(){

    if(m_verified){
        return true;
    }

    //    bool ok = false;
    //    QString text = QInputDialog::getText(this, tr("Privilege Required"),
    //                                         tr("Access Code:"), QLineEdit::NoEcho,
    //                                         "", &ok);
    //    if (ok && !text.isEmpty()){
    //        QString accessCodeString = "hehui";
    //        accessCodeString.append(QTime::currentTime().toString("hhmm"));
    //        if(text.toLower() == accessCodeString){
    //            activityTimer->start();
    //            return true;
    //        }
    //    }

    //    return false;

    //    if(m_adminName == "hehui"){
    //        return true;
    //    }

    bool ok = false;
    do {
        QString text = QInputDialog::getText(this, tr("Authentication Required"),
                                             tr("Authorization Number:"), QLineEdit::NoEcho,
                                             "", &ok);
        if (ok && !text.isEmpty()){
            QString accessCodeString = "hehui";
            accessCodeString.append(QTime::currentTime().toString("hhmm"));
            if(text.toLower() == accessCodeString){
                m_verified = true;
                return true;
            }
        }

        QMessageBox::critical(this, tr("Error"), tr("Incorrect Access Code!"));

    } while (ok);

    return false;

}






















} //namespace HEHUI
