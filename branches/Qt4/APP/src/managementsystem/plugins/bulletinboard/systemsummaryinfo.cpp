



#include <QtSql>
#include <QString>
#include <QDir>
#include <QDebug>
#include <QCloseEvent>

#include <QMessageBox>
#include <QInputDialog>
#include <QMenu>
#include <QHostInfo>





#include "systemsummaryinfo.h" 


#include "HHSharedCore/hglobal_core.h"
#include "HHSharedCore/hutilities.h"
#include "HHSharedNetwork/hnetworkutilities.h"

#ifdef Q_OS_WIN
#include "HHSharedWindowsManagement/hwindowsmanagement.h"
#endif



namespace HEHUI {



bool SystemSummaryInfo::running = false;

SystemSummaryInfo::SystemSummaryInfo(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    setWindowFlags(Qt::Dialog);


    localComputerName = QHostInfo::localHostName().toLower();
    ui.lineEditComputerName->setText(localComputerName);

    m_currentUserNameOfOS = Utilities::currentUserNameOfOS();
    ui.lineEditUserName->setText(m_currentUserNameOfOS);



    QString osInfo = "";

#ifdef Q_OS_WIN

    WindowsManagement wm;
    ui.lineEditWorkgroup->setText(wm.getJoinInformation());

    switch(QSysInfo::windowsVersion()){
    case QSysInfo::WV_2000:
        osInfo = "WIN_2000";
        break;
    case QSysInfo::WV_XP:
        osInfo = "WIN_XP";
        break;
    case QSysInfo::WV_2003:
        osInfo = "WIN_2003";
        break;
    case QSysInfo::WV_VISTA:
        osInfo = "WIN_VISTA";
        break;
    case QSysInfo::WV_WINDOWS7:
        osInfo = "WIN_7";
        break;
    default:
        osInfo = "WIN";
        break;
    }
#elif defined(Q_OS_UNIX)
    osInfo = "UNIX";
#elif defined(Q_OS_LINUX)
    osInfo = "LINUX";
#elif defined(Q_OS_DARWIN)
    osInfo = "MAC OS";
#else
    osInfo = "Unknown OS";
#endif

    ui.lineEditOSVersion->setText(osInfo);




    QList<QHostAddress> ips = NetworkUtilities::validIPAddresses();
    int count = ips.count();


    if(count>=1){
        QHostAddress ip = ips.at(0);
        ui.lineEditPhysicalAddress1->setText(NetworkUtilities::hardwareAddress(ip));
        ui.lineEditIPAddress1->setText(ip.toString());
    }

    if(count>=2){
        QHostAddress ip = ips.at(1);
        ui.lineEditPhysicalAddress2->setText(NetworkUtilities::hardwareAddress(ip));
        ui.lineEditIPAddress2->setText(ip.toString());
    }

    if(count>=3){
        QHostAddress ip = ips.at(2);
        ui.lineEditPhysicalAddress3->setText(NetworkUtilities::hardwareAddress(ip));
        ui.lineEditIPAddress3->setText(ip.toString());
    }

    if(count>=4){
        QHostAddress ip = ips.at(3);
        ui.lineEditPhysicalAddress4->setText(NetworkUtilities::hardwareAddress(ip));
        ui.lineEditIPAddress4->setText(ip.toString());
    }




    running = true;


}

SystemSummaryInfo::~SystemSummaryInfo()
{

    running = false;

}


void SystemSummaryInfo::languageChange() {
    retranslateUi();
}

void SystemSummaryInfo::closeEvent(QCloseEvent *e) {

    e->accept();
    deleteLater();

}


void SystemSummaryInfo::retranslateUi() {

    ui.retranslateUi(this);

}























} //namespace HEHUI
