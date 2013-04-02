/*
 ****************************************************************************
 * bulletinboardplugin.h
 *
 * Created on: 2010-07-13
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
 * Last Modified on: 2010-07-13
 * Last Modified by: 贺辉
 ***************************************************************************
 */




#ifndef BULLETINBOARDPLUGIN_H_
#define BULLETINBOARDPLUGIN_H_

#include <QObject>

#include "systemsummaryinfo.h"

//#include "packetmanager/bulletinboardpacketsparser.h"
//#include "../../sharedms/networkmanager.h"

#include "HHSharedGUI/hguipluginbase.h"

//#include "remoteassistance/remoteassistance.h"
//#include "bulletinboard/bulletinboardwidget.h"
//#include "updatepassword/updatepasswordwidget.h"

#include "bulletinboardobject.h"

namespace HEHUI {

class BulletinBoardPlugin : public GUIPluginBase/*, public QObject*/   {
	Q_OBJECT

    //Export Plugin
    Q_PLUGIN_METADATA(IID AbstractPluginInterface_IID)
    Q_PLUGIN_METADATA(IID GUIInterface_IID)

    
public:
    BulletinBoardPlugin();
    virtual ~BulletinBoardPlugin();
    
    bool isSingle();
    QString name () const;
    QString version() const;
    QString description() const;
    
    QIcon icon () const;
    QString whatsThis () const;
    QString toolTip () const;
    
    QAction * mainActionForMenu();    
    
    bool unload();
    
signals:

    
public slots:
    void sloSystemSummaryInfoWidgetDestoryed(QObject * obj);
    
private slots:
    void retranslateUi();
    void slotMainActionForMenuTriggered();
    
//    void networkReady();

//    void adminRequestRemoteAssistancePacketReceived(const QString &adminAddress, quint16 adminPort, const QString &adminName);
//    void AdminInformUserNewPasswordPacketReceived(const QString &adminAddress, quint16 adminPort, const QString &adminName, const QString &oldPassword, const QString &newPassword );
//    void serverAnnouncementPacketReceived(const QString &adminName, const QString &announcement);
    
//    void test(const QString &msg);
     
private:
    QList<SystemSummaryInfo *> systemSummaryInfoWidgetList;
    
    QAction *actionMain;
    
//    bool m_networkReady;
//    NetworkManager *networkManager;
//    BulletinBoardPacketsParser *bulletinBoardPacketsParser;
    
//    RemoteAssistance *remoteAssistance;
//    BulletinBoardWidget *bulletinBoardWidget;
//    UpdatePasswordWidget *updatePasswordWidget;
    

    BulletinBoardObject *bulletinBoardObject;

};

} //namespace HEHUI

#endif /* BULLETINBOARDPLUGIN_H_ */
