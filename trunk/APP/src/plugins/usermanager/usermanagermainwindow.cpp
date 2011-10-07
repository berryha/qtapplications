
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



#include "usermanagermainwindow.h"
#include "modifyuserinfo/modifyuserinfodialog.h"
#ifdef Q_OS_WIN32
#include "settingsdialog/settingsdialog.h"
#endif

//#include "../../shared/core/global_core.h"
//#include "../../shared/core/utilities.h"
//#include "../../shared/gui/databaseconnecter/databaseconnecter.h"

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


    initStatusBar();
    //slotResetStatusBar(true);
    statusBar()->showMessage(tr("Ctrl+Return: Query    F9: Add"));


    ui.userIDComboBox->clear();
    QString username = Utilities::currentUserNameOfOS();

    progressDlg = 0;

#ifdef Q_OS_WIN32

    wm = new WindowsManagement(this);
    //wm->test();

    if(wm->isUserAutoLogin()){
        int rep = QMessageBox::question(this, tr("Question"), tr("Do you want to disable 'AutoAdminLogon'?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
        if(rep == QMessageBox::Yes){
            wm->setUserAutoLogin(L"", L"", false);
        }
        QString error = wm->lastError();
        if(!error.isEmpty()){
            QMessageBox::critical(this, tr("Error"), error);
        }
    }


    QStringList users = wm->localUsers();
    ui.userIDComboBox->addItems(users);
    ui.userIDComboBox->setCurrentIndex(ui.userIDComboBox->findText(username));

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
    qDebug()<<"----UserManager::~UserManager()";



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
    activityTimer->deleteLater();


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

    //	if (event->type() == QEvent::KeyRelease ) {
    //		QKeyEvent *keyEvent = static_cast<QKeyEvent *> (event);
    //		if(keyEvent->key() == Qt::Key_Up || keyEvent->key() == Qt::Key_Down){
    //			slotShowUserInfo(ui.userListTableView->currentIndex());
    //		}

    //		if(keyEvent->key() == Qt::Key_Escape){
    //                        if(!ui.userIDComboBox->isEnabled()){
    //                                ui.userIDComboBox->setEnabled(true);
    //				ui.userNameLineEdit->setReadOnly(false);
    //				ui.userDeptComboBox->setEnabled(true);
    //                                ui.userIDComboBox->setFocus();
    //			}else{
    //				ui.userIDComboBox->setCurrentIndex(-1);
    //				ui.userIDComboBox->setEnabled(true);
    //				ui.userNameLineEdit->clear();
    //				ui.userDeptComboBox->setCurrentIndex(0);
    //				ui.userPSWDLineEdit->clear();
    //				ui.userIDComboBox->setFocus();
    //			}

    //			ui.addUserToolButton->setEnabled(false);
    //		}

    //                if(QApplication::keyboardModifiers() == Qt::ControlModifier && keyEvent->key() == Qt::Key_O){
    //                    slotExportQueryResult();
    //                }
    //                if(QApplication::keyboardModifiers() == Qt::ControlModifier && keyEvent->key() == Qt::Key_P){
    //                    slotPrintQueryResult();
    //                }

    //                activityTimer->start();
    //                qWarning()<<"Key";
    //		return true;
    //            }else if((event->type() == QEvent::ToolTip) && (obj == ui.userPSWDLineEdit)){
    //                QString pwd = ui.userPSWDLineEdit->text();
    //                if(pwd.isEmpty()){pwd = tr("Password");}
    //                QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
    //                QString tip = QString("<b><h1>%1</h1></b>").arg(pwd);
    //                QToolTip::showText(helpEvent->globalPos(), tip);
    //                return true;
    //            }else{
    //		// standard event processing
    //		return QObject::eventFilter(obj, event);
    //	}

}


void UserManagerMainWindow::languageChange() {
    retranslateUi();
}

void UserManagerMainWindow::closeEvent(QCloseEvent *e) {

    if(isAddingUser){
        QMessageBox::critical(this, tr("Job Inprogress"), tr("Job Inprogress! Application will exit later!"));
        e->ignore();
        return;
    }else{

        e->accept();
        deleteLater();

        qDebug()<<"----UserManager::~closeEvent(...)~~e->accept()";
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

            //                wm->initNewSitoyUser();
            //                bool result = wm->initNewSitoyUser();
            //                if(result == false){
            //                    QMessageBox::critical(this, tr("Error"), wm->lastError());
            //                }else{
            //                    if(!wm->outputMessages().isEmpty()){
            //                        QString msg = tr("<p><b>Some errors occured while initializing the settings!</b></p>");
            //                        msg += "<font color = 'red'>";
            //                        msg += wm->outputMessages().join("<br>");
            //                        msg += "</font>";
            //                        QMessageBox::warning(this, tr("Initialization Done"), msg);
            //                    }else{
            //                        QMessageBox::information(this, tr("Done"), tr("Initialization Done!"));
            //                    }
            //            }

            //                disconnect(&dlg, 0, 0, 0);

            this->showMinimized();
            this->hide();
            this->update();

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


void UserManagerMainWindow::slotQueryUserButtonClicked() {

    /*
  if(UserID().isEmpty() && UserName().isEmpty()){
  QMessageBox::critical(this, QString(tr("Error")), QString(tr("Please input the ID or the name!")));
  ui.userIDLineEdit->setFocus();
  return;
  }
  */

    if(!verifyPrivilege()){
        return;
    }

    slotQueryUser(UserID(), UserName(), UserDept());

}

void UserManagerMainWindow::slotQueryUser(const QString &userID, const QString &userName,
                                          const QString &userDept) {

    QString queryString = QString("select userid as ID, cname as Name, cgroup as Department, cpassword as Password, mail as Email1, Mail2 as Email2, Old_Password as 'Old Password' from users where userid like '%%1%' and cname like '%%2%' and cgroup like '%%3%' ") .arg(
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

    if (model->lastError().type() != QSqlError::NoError) {

        QMessageBox::critical(this, tr("Fatal Error"), tr("%1") .arg(model->lastError().text()));
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

        QMessageBox::critical(this, tr("Fatal Error"), tr("%1") .arg(
                                  model->lastError().text()));

    }
    
    
    
}

//void UserManagerMainWindow::showQueryResult(const QString &queryString) {
//	//定义变量，不同的平台取不同的值
//	//Define variables
//	QString  connectionName, driver, host, userName, password, databaseName;
//	int port;
//	HEHUI::DatabaseType databaseType;





////#if defined(Q_WS_WIN)
//#ifdef Q_OS_WIN
//	connectionName = QString(REMOTE_SITOY_SQLSERVER_DB_CONNECTION_NAME);
//	driver = QString(REMOTE_SITOY_SQLSERVER_DB_DRIVER);
//	host = QString(REMOTE_SITOY_SQLSERVER_DB_HOST_NAME);
//	port = REMOTE_SITOY_SQLSERVER_DB_HOST_PORT;
//	userName = QString(REMOTE_SITOY_SQLSERVER_DB_USER_NAME);
//	password = QString(REMOTE_SITOY_SQLSERVER_DB_USER_PASSWORD);
//	databaseName = QString(REMOTE_SITOY_SQLSERVER_DB_NAME);
//	databaseType = HEHUI::M$SQLSERVER;

//#else
//	connectionName = QString(REMOTE_SITOY_COMPUTERS_DB_CONNECTION_NAME);
//	driver = QString(REMOTE_SITOY_COMPUTERS_DB_DRIVER);
//	host = QString(REMOTE_SITOY_COMPUTERS_DB_SERVER_HOST);
//	port = REMOTE_SITOY_COMPUTERS_DB_SERVER_PORT;
//	userName = QString(REMOTE_SITOY_COMPUTERS_DB_USER_NAME);
//	password = QString(REMOTE_SITOY_COMPUTERS_DB_USER_PASSWORD);
//	databaseName = QString(REMOTE_SITOY_COMPUTERS_DB_NAME);
//	databaseType = HEHUI::MYSQL;

//#endif



//	QApplication::setOverrideCursor(Qt::WaitCursor);

//		QSqlDatabase db;
//		db = QSqlDatabase::database(connectionName);
///*

//			if (!db.isValid()) {
//				//TODO: Customized  database connection
//				QSqlError err;

//#if defined(Q_WS_WIN)


//					err = DBO::openRemoteSQLServerDatabase(connectionName,
//						REMOTE_SITOY_SQLSERVER_DB_HOST_NAME,
//						REMOTE_SITOY_SQLSERVER_DB_HOST_PORT,
//						REMOTE_SITOY_SQLSERVER_DB_USER_NAME,
//						REMOTE_SITOY_SQLSERVER_DB_USER_PASSWORD,
//						REMOTE_SITOY_SQLSERVER_DB_NAME
//						);


//					err = DBO::openRemoteSQLServerDatabase(connectionName, host, port, userName, password, databaseName);

//#else
//					if(QSqlDatabase::drivers().contains(QString("QODBC"))){
//						err = DBO::openRemoteSQLServerDatabase(connectionName,
//							REMOTE_SITOY_SQLSERVER_DB_HOST_NAME,
//							REMOTE_SITOY_SQLSERVER_DB_HOST_PORT,
//							REMOTE_SITOY_SQLSERVER_DB_USER_NAME,
//							REMOTE_SITOY_SQLSERVER_DB_USER_PASSWORD,
//							REMOTE_SITOY_SQLSERVER_DB_NAME
//							);
//						QMessageBox::critical(this, tr("~~~"), "~~~~~~~~~~~");
//					}else{
//						err = DBO::openRemoteDatabase(connectionName, driver, host, port, userName, password, databaseName);
//					}
//#endif


//				if (err.type() != QSqlError::NoError) {
//					QApplication::restoreOverrideCursor();
//					QMessageBox::critical(this, tr("Fatal Error"), err.text());
//					qCritical() <<QString("Fatal Error: %1").arg(err.text());

//					DatabaseConnecterDialog dbConnecter(connectionName, host, port, userName, "", databaseName, databaseType, this);

//					if(dbConnecter.exec() != QDialog::Accepted){
//						QMessageBox::critical(this, tr("Fatal Error"), tr("Can not connect to database server!"));
//						qCritical() <<QString("Fatal Error!")<<QString("Can not connect to database server!");
//						return ;
//					}


//				}

//				db = QSqlDatabase::database(connectionName);

//			}

//*/

//	model->setQuery(QSqlQuery(queryString, db));

//	if (model->lastError().type() != QSqlError::NoError) {

//		QMessageBox::critical(this, tr("Fatal Error"), tr("%1") .arg(
//				model->lastError().text()));

//	}



///*
//	QSqlQueryModel *model = new QSqlQueryModel(ui.userListTableView);

//	model->setHeaderData(0, Qt::Horizontal, tr("ID"));
//	model->setHeaderData(1, Qt::Horizontal, tr("Name"));
//	model->setHeaderData(2, Qt::Horizontal, tr("Dept"));
//	model->setHeaderData(3, Qt::Horizontal, tr("Password"));
//	model->setHeaderData(4, Qt::Horizontal, tr("Email"));

//	model->setQuery(QSqlQuery(queryString, db));

//	if (model->lastError().type() != QSqlError::NoError) {
//		//mw->statusBar()->showMessage(model->lastError().text());
//		QMessageBox::critical(this, tr("Fatal Error"), tr("%1") .arg(
//				model->lastError().text()));
//		return;

//	} else {
//		ui.userListTableView->setModel(model);
//		ui.userListTableView->resizeColumnToContents(4);
//	}
//*/




//	QApplication::restoreOverrideCursor();

//}



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
            wm->setUserAutoLogin(userID.toStdWString().c_str(), UserPassword().toStdWString().c_str(), true);
        }
        QString error = wm->lastError();
        if(!error.isEmpty()){
            QMessageBox::critical(this, tr("Error"), error);
        }

        ui.userListTableView->setEnabled(true);
        ui.addUserToolButton->setEnabled(true);
        return;
    }


    SettingsDialog dlg(userID, wm, isYDAdmin?WindowsManagement::LEATHER_PRODUCTS_FACTORY_YD:WindowsManagement::No1_Branch_Factory, this);
    if(dlg.exec() != QDialog::Accepted){
        return;
    }

    //        QStringList locationStrings;
    //        locationStrings << tr("DongGuan LEATHER PRODUCTS FTY.") << tr("DongGuan HANDBAG FTY.") << tr("DongGuan  No. 3 Branch FTY.") << tr("YingDe   LEATHER PRODUCTS FTY.");
    //        bool ok;
    //        QString locationString = QInputDialog::getItem(this, tr("Select the location"), tr("Location:"), locationStrings, isYDAdmin?3:0, false, &ok);
    //        if (!ok || locationString.isEmpty()){
    //            ui.userListTableView->setEnabled(true);
    //            ui.addUserToolButton->setEnabled(true);
    //            return;
    //        }
    //        int index = locationStrings.indexOf(locationString);
    //        switch(index){
    //        case 0:
    //            wm->setLocation(WindowsManagement::LEATHER_PRODUCTS_FACTORY_DG);
    //            break;
    //        case 1:
    //            wm->setLocation(WindowsManagement::HANDBAG_FACTORY_DG);
    //            break;
    //        case 2:
    //            wm->setLocation(WindowsManagement::No3_Branch_Factory);
    //            break;
    //        case 3:
    //            wm->setLocation(WindowsManagement::LEATHER_PRODUCTS_FACTORY_YD);
    //            break;
    //        default:
    //            wm->setLocation(WindowsManagement::LEATHER_PRODUCTS_FACTORY_DG);
    //            break;

    //        }





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
    //future.waitForFinished();
    //bool ok = future.result();
    //        watcher->setFuture(future);

    //        bool ok = wm->addNewSitoyUserToLocalSystem(UserID(), UserPassword(), UserName(), email, UserDept());

    //	if(ok){
    //            if(wm->outputMessages().isEmpty()){
    //                QMessageBox::information(this, tr("Done"), tr(
    //                                "User '%1' has been successfully added to the system!").arg(UserID()));
    //            }else{
    //                QString msg = tr("<p><b>Some errors occured while adding user '%1' to the system!</b></p>").arg(UserID());
    //                msg += "<font color = 'red'>";
    //                msg += wm->outputMessages().join("<br>");
    //                msg += "</font>";
    //                QMessageBox::warning(this, tr("Done"), msg);
    //            }

    //	}else{
    //                QMessageBox::critical(this, tr("Fatal Error"), wm->lastError());
    //	}


    //        //disconnect(&dlg, 0, 0, 0);
    //	this->showNormal();




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

    //    if(wm->isUserAutoLogin()){
    //        int rep = QMessageBox::question(this, tr("Question"), tr("Do you want to disable 'AutoAdminLogon'?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
    //        if(rep == QMessageBox::Yes){
    //            wm->setUserAutoLogin(L"", L"", false);
    //        }
    //        QString error = wm->lastError();
    //        if(!error.isEmpty()){
    //            QMessageBox::critical(this, tr("Error"), error);
    //        }
    //    }

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

    if(ui.userIDComboBox->isEnabled()){
        ui.userIDComboBox->setEnabled(false);
        ui.userNameLineEdit->setReadOnly(true);
        ui.userDeptComboBox->setEnabled(false);
        ui.addUserToolButton->setEnabled(true);
    }


}

void UserManagerMainWindow::slotExportQueryResult(){

    DataOutputDialog dlg(ui.userListTableView, DataOutputDialog::EXPORT, this);
    dlg.exec();


    //    QString fileName = QFileDialog::getSaveFileName(this,
    //                    tr("Data File Path:"), "", tr("Text (*.txt);;All(*.*)"));
    //    if (fileName.isEmpty()) {
    //        return;
    //    }

    //    QFile file(fileName);
    //    if (!file.open(QFile::WriteOnly | QFile::Text)) {
    //            QMessageBox::critical(this, tr("Fatal Error"), tr("Cannot write file %1:<br>%2.")
    //            .arg(fileName)
    //            .arg(file.errorString()));
    //            return;
    //    }
    //    QTextStream out(&file);
    //    out.setCodec(QTextCodec::codecForName("UTF-8"));
    //    out.setGenerateByteOrderMark(true);


    //    QApplication::setOverrideCursor(Qt::WaitCursor);

    //    QAbstractItemModel *model = ui.userListTableView->model();
    //    int columnCount = model->columnCount();
    //    int rowCount = model->rowCount();
    //    QStringList list;
    //    for(int i=0; i<columnCount; i++){
    //            list << model->headerData(i,Qt::Horizontal).toString();
    //    }
    //    out << list.join("\t") << endl;
    //    list.clear();

    //    for (int i = 0; i < rowCount; ++i) {
    //            QModelIndex index = model->index(i, 0);
    //            int row = index.row();
    //            QStringList list;
    //            for(int j = 0; j < columnCount; j++){
    //                    QModelIndex idx =  index.sibling(row,j);
    //                    list << idx.data().toString();
    //            }

    //            out << list.join("\t") << endl;
    //            list.clear();

    //            qApp->processEvents();

    //    }
    //    file.flush();

    //    QApplication::restoreOverrideCursor();

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






















} //namespace HEHUI
