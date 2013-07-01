/*
 ****************************************************************************
 * usersmanager.cpp
 *
 * Created on: 2009-9-12
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
  * Last Modified on: 2010-05-22
  * Last Modified by: 贺辉
  ***************************************************************************
*/




#include <QDebug>
#include <QSqlError>

#include "usersmanager.h"

#include "../app_constants_server.h"
#include "../settings.h"
#include "../serverutilities.h"


//#include "../../shared/global_shared.h"

//#include "../../../shared/core/database/databaseutility.h"
//#include "../../../shared/gui/databaseconnecter/databaseconnecter.h"
//#include "../../../shared/core/user.h"

#include "HHSharedCore/hdatabaseutility.h"



namespace HEHUI {



//QHash<QString, IMUserBase*> UsersManager::imUsersHash = QHash<QString, IMUserBase*>();
//QTimer * UsersManager::timer = new QTimer();

QMutex * UsersManager::onlineUserMutex = new QMutex();
QHash<QString/*User ID*/, UserInfo*> * UsersManager::onlineUserInfoHash = new QHash<QString, UserInfo*> ();
QMutex * UsersManager::offlineUserMutex = new QMutex();
QHash<QString/*User ID*/, UserInfo*> * UsersManager::offlineUserInfoHash = new QHash<QString, UserInfo*> ();


QMutex * UsersManager::groupMutex = new QMutex();
QHash<quint32/*Group ID*/, InterestGroup*> * UsersManager::groupHash = new QHash<quint32, InterestGroup*> ();



UsersManager::UsersManager(QObject *parent)
        //:QObject(parent)
{


    openDatabase();
    

}

UsersManager::~UsersManager() {
	// TODO Auto-generated destructor stub
    
}



UserInfo* UsersManager::getUserInfo(const QString &imUserID){
    
    if(imUserID.trimmed().isEmpty()){
        return 0;
    }
    
    UserInfo * userInfo = getOnlineUserInfo(imUserID);
//    if(!userInfo){
//        userInfo = getOnlineUserInfo(imUserID);
//    }
    if(!userInfo){
        userInfo = getOfflineUserInfo(imUserID);
    }
    if(!userInfo){
        userInfo = queryUserInfo(imUserID);
    }
    return userInfo;

}

UserInfo* UsersManager::getOnlineUserInfo(const QString &imUserID){
    QMutexLocker locker(onlineUserMutex);
    UserInfo * userInfo = 0;
    if(onlineUserInfoHash->contains(imUserID)){
        userInfo = onlineUserInfoHash->value(imUserID);
    }
    return userInfo;
}

UserInfo* UsersManager::getOfflineUserInfo(const QString &imUserID){
    QMutexLocker locker(offlineUserMutex);
    UserInfo * userInfo = 0;
    if(offlineUserInfoHash->contains(imUserID)){
        userInfo = offlineUserInfoHash->value(imUserID);
    }
    return userInfo;
}

QList<UserInfo*> UsersManager::onlineUsers(){
        QMutexLocker locker(onlineUserMutex);
        return onlineUserInfoHash->values();
}

void UsersManager::userOnline(UserInfo* userInfo){

    if(userInfo){
        QString userID = userInfo->getUserID();
        userInfo->setOnline();

        QMutexLocker onlineLocker(onlineUserMutex);
        onlineUserInfoHash->insert(userID, userInfo);

        QMutexLocker offlineLocker(offlineUserMutex);
        offlineUserInfoHash->remove(userID);

    }

}

void UsersManager::userOffline(UserInfo* userInfo){

    if(userInfo){
        QString userID = userInfo->getUserID();
        userInfo->setOffline();

        QMutexLocker onlineLocker(onlineUserMutex);
        onlineUserInfoHash->remove(userID);

        QMutexLocker offlineLocker(offlineUserMutex);
        offlineUserInfoHash->insert(userID, userInfo);

        saveUserInfoToDatabase(userInfo);

        qWarning()<<QString("User %1 Offline!").arg(userID);
    }


}

bool UsersManager::canUserLogin(const QString &userID, const QString &clientVersion, IM::ErrorType *errorType){
//    qWarning()<<"--canUserLogin(...)";
    
    UserInfo *userInfo = getUserInfo(userID);
    //TODO:检查用户是否可以登陆及可登陆的服务器
    if(userInfo){
        if(userInfo->isBanned()){
            *errorType = IM::ERROR_IDBanned;
            return false;
        }

    }else{
        *errorType = IM::ERROR_IDNotExist;
        return false;
    }

    *errorType = IM::ERROR_NoError;
    return true;

}

void UsersManager::getUserLoginServer(const QString &userID, QHostAddress *serverAddress, quint16 *serverPort){
//    qWarning()<<"--getUserLoginServer(...)";
    
    //TODO:处理登陆服务器
    if(serverAddress){
//        serverAddress = QHostAddress::Null;
    }
    if(serverPort){
//        serverPort = 0;
    }

}

UserInfo* UsersManager::logUserIn(const QString &userID, const QByteArray &encryptedPassword, IM::OnlineState loginState, IM::ErrorType *errorType){
    //qWarning()<<"logUserIn(...)";

    UserInfo *userInfo = getUserInfo(userID);
    if(!userInfo){
        *errorType = IM::ERROR_IDNotExist;
        return 0;
    }

    //TODO:密码保存方式
    QByteArray decryptedPassword;
    Cryptography cryptography;
    cryptography.teaCrypto(&decryptedPassword, encryptedPassword, userInfo->encryptedPassword(), false); 
    
    if(decryptedPassword != QByteArray::fromBase64(userInfo->getPassword().toUtf8())){
        *errorType = IM::ERROR_PasswordIncorrect;
        userInfo->authenticationFailed();
        return 0;
    }else{
        *errorType = IM::ERROR_NoError;
        userInfo->setOnlineState(loginState);
        userInfo->setSessionEncryptionKey(ServerUtilities::generateSessionEncryptionKey());
        userOnline(userInfo);
        //qWarning()<<"getSessionEncryptionKey:"<<userInfo->getSessionEncryptionKey().toBase64();
    }

    return userInfo;
}

bool UsersManager::registerNewUser(const QString &userID, const QString &password, IM::ErrorType *errorType, quint32 *sysID, QString *message){
    
    
    UserInfo *userInfo = getUserInfo(userID);
    if(userInfo){
        *errorType = IM::ERROR_IDEXISTED;
        return false;
    }
    
    if(!db.isValid()){
        if(!openDatabase()){
            *errorType = IM::ERROR_UnKnownError;
            return false;
        }
    } 
    QSqlQuery query(db);
    //QString statement = QString("insert into users_detailed_info(UserID, UserPassword) values('%1', '%2') ").arg(userID).arg(password);
    QString statement = QString("call sp_CreateNewUser('%1', '%2', @sysID);").arg(userID).arg(password);
    if(!query.exec(statement)){
        QSqlError error = query.lastError();
        QString msg = QString("Can not add new user info to database! %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
        qCritical()<<msg;

        //TODO:数据库重启，重新连接
        //MySQL数据库重启，重新连接
        if(error.number() == 2006){
            query.clear();
            openDatabase(true);
        }
        
        *errorType = IM::ERROR_UnKnownError;
        return false;
    }

    statement = "select @sysID;";
    query.exec(statement);
    if(!query.first()){
        qCritical()<<QString("Can not query user SysID! Invalid record! User ID:%1").arg(userID);
        return false;
    }

    if(sysID){
        *sysID = query.value(0).toUInt();
       qDebug()<<"---------------------------------sysID:"<<*sysID;
       if(*sysID == 0){
           *errorType = IM::ERROR_UnKnownError;
           return false;
       }
    }


    
    userInfo = getUserInfo(userID);
    if(!userInfo){
        *errorType = IM::ERROR_UnKnownError;
        return false;
    }
    
    
    *errorType = IM::ERROR_NoError;
    return true;
    
}

void UsersManager::updateUserPassword(const QString &userID, const QString &newPassword, IM::ErrorType *errorType, QString *message){
    UserInfo *userInfo = getUserInfo(userID);
    if(!userInfo){
        return ;
    }

    userInfo->setPassword(newPassword);
    userInfo->addUpdatedProperty(IM::PI_Password, "'"+newPassword+"'");
    
    if(saveUserInfoToDatabase(userInfo)){
        *errorType = IM::ERROR_NoError;        
    }else{
        *errorType = IM::ERROR_UnKnownError;        
    }

}

QStringList UsersManager::searchContact(const QString &propertiesString, bool matchExactly, bool searchOnlineUsersOnly){
    //qWarning()<<"propertiesString:"<<propertiesString;
    
    if(propertiesString.trimmed().isEmpty()){
        return QStringList();
    }
    
    QStringList queryStringList;
    QStringList propertiesList = propertiesString.split(QString(CONTACT_INFO_SEPARATOR));
    if(propertiesList.isEmpty()){
        return QStringList();
    }
    
    UserInfo info("");
    
    foreach (QString property, propertiesList) {
        QStringList list = property.split("=");
        IM::PropertyIDOfUser pi = IM::PropertyIDOfUser(list.at(0).toUInt());
        QString value = list.at(1);
        
        if(value.trimmed().startsWith("'") && (!matchExactly)){
            queryStringList.append(info.databaseColumnName(pi)+" like "+value);
        }else{
            if(pi == IM::PI_Age){
                QString statement = "";
                IMUserBase::AgeSection as = IMUserBase::AgeSection(value.toUInt());
                switch(as){
                case IMUserBase::AS_16_22:
                    statement = " Between 16 And 22";
                    break;
                case IMUserBase::AS_23_30:
                    statement = " Between 23 And 30";
                    break;
                case IMUserBase::AS_31_40:
                    statement = " Between 31 And 40";
                    break;
                case IMUserBase::AS_40_:
                    statement = " > 40";
                    break;
                default:
                    statement = " > 0"; 
                }
                
                queryStringList.append(info.databaseColumnName(pi)+" "+statement);
                
            }else{
                queryStringList.append(info.databaseColumnName(pi)+" = "+value);
            }
            
        }

    }
    
    QString queryString = QString("Select %1, %2, %3, %4, %5, %6 From users_detailed_info where %7")
            .arg(info.databaseColumnName(IM::PI_UserID))
            .arg(info.databaseColumnName(IM::PI_NickName))
            .arg(info.databaseColumnName(IM::PI_Gender))
            .arg(info.databaseColumnName(IM::PI_Age))
            .arg(info.databaseColumnName(IM::PI_Face))
            .arg(info.databaseColumnName(IM::PI_FriendshipApply))
            .arg(queryStringList.join(" And "))
            ;
    if(!db.isValid()){
        if(!openDatabase()){
            return QStringList();
        }
    }
    QSqlQuery query(db);
    
    if(!query.exec(queryString)){
        QSqlError error = query.lastError();
        QString msg = QString("Can not query user info from database! %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
        qCritical()<<msg;
        
        //TODO:数据库重启，重新连接
        //MySQL数据库重启，重新连接
        if(error.number() == 2006){
            query.clear();
            openDatabase(true);
        }
        
        return QStringList();
    }
    
    QStringList usersInfoList;
    while(query.next()){  
        //TODO:query安全性
        QString userID = query.value(0).toString();
        UserInfo *info = getUserInfo(userID);
        
        if(searchOnlineUsersOnly){
            if(info->getOnlineState() != IM::ONLINESTATE_ONLINE){
                continue;
            }
        }
        
        QStringList userInfoList;
        for(int i=0; i<6; i++){
            userInfoList.append(query.value(i).toString());
//            qWarning()<<i<<":"<<query.value(i).toString();
        }
        userInfoList.append(QString::number(info->getOnlineState()));
        
        usersInfoList.append(userInfoList.join(QString(CONTACT_INFO_SEPARATOR)));
    }
    
    return usersInfoList;
    
}












bool UsersManager::saveCachedChatMessageFromIMUser(const QString &senderID, const QString &receiverID, const QString &message){

    if(!db.isValid()){
        if(!openDatabase()){
            return false;
        }
    }
    QSqlQuery query(db);

    QString msg = message;
    msg.replace("'", "\\'");
    msg.replace("\"", "\\\"");
    QString statement = QString("insert into cachedchatmessages(SenderID, RecieverID, Message) values('%1', '%2', '%3' )  ").arg(senderID).arg(receiverID).arg(msg);

    if(!query.exec(statement)){
        QSqlError error = query.lastError();
        QString msg = QString("Can not save data to database! %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
//        logMessage(msg, QtServiceBase::Error);
        qCritical()<<msg;

        //TODO:数据库重启，重新连接
        //MySQL数据库重启，重新连接
        if(error.number() == 2006){
            query.clear();
            openDatabase(true);
        }

        return false;
    }


    if((query.lastError().type() != QSqlError::NoError)){
        qCritical()<<QString("Can not save data to database! Error: %1").arg(query.lastError().text());
        return false;
    }

    return true;

}

QStringList UsersManager::cachedChatMessagesForIMUser(UserInfo* userInfo){
    //TODO:

    if(!userInfo){
        return QStringList();
    }

    if(!db.isValid()){
        if(!openDatabase()){
            return QStringList();
        }
    }
    QSqlQuery query(db);

    QString imUserID = userInfo->getUserID();

    //QString statement = QString("select SenderID, Message, TransmittingTime from cachedchatmessages where RecieverID='%1' and TransmittingTime>'%2'  ").arg(imUserID).arg(userInfo->getLastLoginTime().toString("yyyy-MM-dd hh:mm:ss"));
    QString statement = QString("select SenderID, Message, TransmittingTime from cachedchatmessages where RecieverID='%1' ").arg(imUserID);
    if(!query.exec(statement)){
        QSqlError error = query.lastError();
        QString msg = QString("Can not query chat messages from database! %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
        qCritical()<<msg;

        //TODO:数据库重启，重新连接
        //MySQL数据库重启，重新连接
        if(error.number() == 2006){
            query.clear();
            openDatabase(true);
        }

        return QStringList();
    }

    if((query.lastError().type() != QSqlError::NoError)){
        qCritical()<<QString("Can not query chat messages from database! Error: %1").arg(query.lastError().text());
        return QStringList();
    }

    QStringList messages;
    while(query.next()){
        QStringList message;
        message.append(QVariant(query.value(0)).toString());
        message.append(QVariant(query.value(1)).toString());
        message.append(QVariant(query.value(2)).toDateTime().toString("yyyy.MM.dd hh:mm:ss"));

        messages.append(message.join(QString(PACKET_DATA_SEPARTOR)));

    }

    statement = QString("delete from cachedchatmessages where RecieverID='%1' ").arg(imUserID);
    query.exec(statement);


    qWarning()<<"messages cached on server:"<<messages.size();

    return messages;

}

bool UsersManager::saveUserLastLoginInfo(UserInfo* userInfo, const QString &userHostAddress,quint16 userHostPort){
    qDebug()<<"--UsersManager::saveUserLoginInfo(...)";

    if(!userInfo){
        return false;
    }

    if(!db.isValid()){
        if(!openDatabase()){
            return false;
        }
    }
    QSqlQuery query(db);

    QString imUserID = userInfo->getUserID();

    userInfo->setLastLoginExternalHostAddress(userHostAddress);
    userInfo->setLastLoginExternalHostPort(userHostPort);


    //statement = QString("insert into loginhistories(UserID, IPAddress, LoginTime) values('%1', '%2', '%3') ").arg(imUserID).arg(userHostAddress).arg(curTime);
    QString statement = QString(" call sp_LogUserIn('%1', '%2', %3, @loginTme);  ").arg(imUserID).arg(userHostAddress).arg(userHostPort);
    statement += QString(" select @loginTme;");

    if(!query.exec(statement)){
        QSqlError error = query.lastError();
        QString msg = QString("Can not save user login info into database! %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
        qCritical()<<msg;
        return false;
    }

    if(!query.first()){
        qCritical()<<QString("Can not save user login info into database! Error: %1").arg(query.lastError().text());
        return false;
    }

    userInfo->setLastLoginTime(query.value(0).toDateTime());

    return true;

}

bool UsersManager::saveUserLastLogoutInfo(UserInfo* userInfo){
    qDebug()<<"--UsersManager::saveUserLastLogoutInfo(...)";

    if(!userInfo){
        return false;
    }

    if(!db.isValid()){
        if(!openDatabase()){
            return false;
        }
    }
    QSqlQuery query(db);

    QString imUserID = userInfo->getUserID();
    QString loginTime = userInfo->getLastLoginTime().toString("yyyy-MM-dd hh:mm:ss");

    QString statement = QString(" call sp_LogUserOut('%1', '%2') ; ").arg(imUserID).arg(loginTime);
    if(!query.exec(statement)){
        QSqlError error = query.lastError();
        QString msg = QString("Can not save user logout info into database! %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
        qCritical()<<msg;
        return false;
    }

    return true;

}

bool UsersManager::getUserLastLoginInfo(UserInfo *userInfo){

    if(!userInfo){
        return false;
    }

    if(!db.isValid()){
        if(!openDatabase()){
            return false;
        }
    }
    QSqlQuery query(db);

    QString imUserID = userInfo->getUserID();
    QString statement = QString("SELECT IPAddress, LoginTime FROM loginhistories where ID = (select max(ID) from loginhistories where UserID = '%1')  ").arg(imUserID);
    if(!query.exec(statement)){
        QSqlError error = query.lastError();
        QString msg = QString("Can not query user login info from database! %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
        qCritical()<<msg;

        //TODO:数据库重启，重新连接
        //MySQL数据库重启，重新连接
        if(error.number() == 2006){
            query.clear();
            openDatabase(true);
        }
        return false;
    }

    if(!query.first()){
        qCritical()<<QString("Can not query user login info from database! Error: %1").arg(query.lastError().text());
        return false;
    }


    userInfo->setLastLoginExternalHostAddress(query.value(0).toString());
    userInfo->setLastLoginTime(query.value(1).toDateTime());

    return true;

}

//void UsersManager::slotFetchIMUsersInfo(){

////	QString groupQueryString = QString("select * from contactgroups");

////		QSqlQueryModel *model = new QSqlQueryModel();
////		QSqlQueryModel *contactsModel = new QSqlQueryModel();

////		model->setQuery(queryDatabase(groupQueryString, true));
////		if(model->lastError().type() != QSqlError::NoError){
////			//QMessageBox::critical(0, "Query Error", model->lastError().text());
////			qCritical()<<"Query Error!"<<model->lastError().text();
////			return;
////		}

////		for (int i=0;i< model->rowCount();i++) {
////						int groupID = QVariant(model->record(i).value("ID")).toInt();
////						QString groupName = QVariant(model->record(i).value("GroupName")).toString();
////						QList<IMUserBase*> list;

////						QString contactsQueryString = QString("select * from contacts where ContactsGroup=%1") .arg(groupID);

////						contactsModel->setQuery(queryDatabase(contactsQueryString, true));
////						if(contactsModel->lastError().type() != QSqlError::NoError){
////							//QMessageBox::critical(0, "Query Error", contactsModel->lastError().text());
////							qCritical()<<"Query Error!"<<contactsModel->lastError().text();
////							return;
////						}

////						for (int i=0;i< contactsModel->rowCount();i++) {
////							QString contactUID = QVariant(contactsModel->record(i).value("UID")).toString();
////							int contactsGroupID = QVariant(contactsModel->record(i).value("ContactsGroup")).toInt();
////							QString contactDept = QVariant(contactsModel->record(i).value("Dept")).toString();
////							QString contactTrueName = QVariant(contactsModel->record(i).value("TrueName")).toString();
////							QString contactNickName = QVariant(contactsModel->record(i).value("NickName")).toString();
////							QString contactHeadPortrait = QVariant(contactsModel->record(i).value("HeadPortrait")).toString();
////							QString contactLastLoginIP = QVariant(contactsModel->record(i).value("LastLoginIP")).toString();

////							IMUserBase *contact = new IMUserBase();
////                                                        contact->setUserID(contactUID);
////							contact->setUserID(contactUID);
////							contact->setContactsGroupID(contactsGroupID);
////							contact->setDept(contactDept);
////							contact->setHeadPortrait(contactHeadPortrait);
////							contact->setTrueName(contactTrueName);
////							contact->setNickName(contactNickName);
////							contact->setLastLoginIP(contactLastLoginIP);

////                                                        imUsersHash.insert(contactUID, contact);
////							list.append(contact);
////							qApp->processEvents();
////						}

//////						UserGroup *group = new UserGroup(groupID, groupName, list, this);
//////						contactGroupHash.insert(groupID, group);



////					}


////		contactsModel->deleteLater();
////		model->deleteLater();


//}

////添加新的联系人
////Add new contact
//void UsersManager::slotAddNewIMUser(UserInfo *imUser){

//    //TODO:保存数据

////        QString contactUID = imUser->getUserID();
////        QString contactTrueName = imUser->getTrueName();
////        QString contactNickName = imUser->getNickName();
////        QString contactHeadPortrait = imUser->getFace();
////        QString contactLastLoginIP = imUser->getLastLoginHostAddress();

////        QString queryString = QString("Insert  Into [contacts] ([UID],[NickName],[TrueName],[LastLoginIP],[HeadPortrait]) Values('%1', '%2', '%3', '%4', '%5', '%6')")
////                                                                                                .arg(contactUID).arg(contactNickName).arg(contactTrueName).arg(contactLastLoginIP).arg(contactHeadPortrait) ;

////	QSqlQuery query = queryDatabase(queryString, true);
////	if(!query.exec()){
////		qDebug()<<"XXXXContactsManager::slotAddNewContact(...)~~Error:"<<query.lastError().text();
////	}


//}








/*

void UsersManager::slotUpdateContactsInfo2(){



	QSqlQuery query;
	QSqlRecord record;


	//Local
	QString queryString = QString("select ContactsListVersion from contacts where UID = '%1'") .arg(User::instance()->getUserID());

	int localContactsListVersion = -1;

	query = queryDatabase(queryString, true);
	if(query.lastError().type() == QSqlError::NoError && query.isValid()){
		query.first();
		localContactsListVersion = query.value(0).toInt();
		qDebug()<<"Local Contacts List Version:"<<localContactsListVersion;
	}else{
		//QMessageBox::critical(this, QObject::tr("Fatal Error"), tr("Can not query data from local database! <br> %1").arg(model.lastError().text()));
		qCritical()<<QString("Can not query data from local database!");
		qCritical()<<QString("Error: %1").arg(query.lastError().text());
		//statusBar()->showMessage(tr("Error! Can not query data from local database!"));
	}


	if(User::instance()->getRemoteServerContactsListVersion() != localContactsListVersion){
		qDebug()<<"Remote Server Contacts List Version:"<<User::instance()->getRemoteServerContactsListVersion();

		if(User::instance()->getRemoteServerContactsListVersion() > localContactsListVersion){
			queryString = QString("update contacts set ContactsList = '%1'  where UID = '%2'") .arg(User::instance()->getContactsList(), User::instance()->getUserID());
			query = queryDatabase(queryString, true);
			qDebug()<<"Update Local Contacts List Version";
		}else{
			queryString = QString("select ContactsList from contacts where UID = '%1'") .arg(User::instance()->getUserID());
			QString contactsList;
			query = queryDatabase(queryString, true);
			if(query.lastError().type() == QSqlError::NoError && query.isValid()){
				query.first();
				contactsList = query.value(0).toString();
			}else{
				//QMessageBox::critical(this, QObject::tr("Fatal Error"), tr("Can not query data from database server! <br> %1").arg(model.lastError().text()));
				qCritical()<<QString("Can not query data from database server!");
				qCritical()<<QString("Error: %1").arg(query.lastError().text());
				//statusBar()->showMessage(tr("Error! Can not query data from database server!"));
			}
			queryString = QString("update contacts set ContactsList = '%1' , ContactsListVersion = %2 where UID = '%3'") .arg(contactsList).arg(localContactsListVersion).arg(User::instance()->getUserID());
			query = queryDatabase(queryString, false);
			if(query.lastError().type() != QSqlError::NoError){qDebug()<<"Query Error:"<<query.lastError().text();}
			qDebug()<<"Update Remote Server Contacts List Version";
			User::instance()->setContactsList(contactsList);
			User::instance()->setRemoteServerContactsListVersion(localContactsListVersion);
			//qDebug()<<"contactsList~~~~"<<contactsList;
		}


	}


	qDebug()<<"-------------contactsList:"<<User::instance()->getContactsList();
	//queryString = QString("select UID, Role, Dept, TrueName, NickName, HeadPortrait, LastLoginIP from contacts where UID in (%1) ") .arg(User::instance()->getContactsList());
	queryString = QString("select UID, Role, Dept, TrueName, NickName, Icon, LastLoginIP from contacts where UID in (%1) ") .arg(User::instance()->getContactsList());
query.clear();
	query = queryDatabase(queryString, true);
	query.first();
	qDebug()<<"-------------queryString:"<<queryString;
	qDebug()<<"-------------query.size:"<<query.size();
	record = query.record();

	while (query.next()){
		QString contactUID = QVariant(query.value(record.indexOf("UID"))).toString();
		int contactsRoleID = QVariant(query.value(record.indexOf("Role"))).toInt();
		QString contactDept = QVariant(query.value(record.indexOf("Dept"))).toString();
		QString contactTrueName = QVariant(query.value(record.indexOf("TrueName"))).toString();
		QString contactNickName = QVariant(query.value(record.indexOf("NickName"))).toString();
		QString contactHeadPortrait = QVariant(query.value(record.indexOf("HeadPortrait"))).toString();
		QString contactLastLoginIP = QVariant(query.value(record.indexOf("LastLoginIP"))).toString();

		ContactBase *contact = new ContactBase();
		contact->setUID(contactUID);
		contact->setRoleID(contactsRoleID);
		contact->setDept(contactDept);
		contact->setHeadPortrait(contactHeadPortrait);
		contact->setTrueName(contactTrueName);
		contact->setNickName(contactNickName);
		contact->setLastLoginIP(contactLastLoginIP);

		contactHash.insert(contactUID, contact);

	}

	record.clearValues();
	queryString = QString("select * from group");
	query = queryDatabase(queryString, true);
	record = query.record();
	while (query.next()){
		int contactGroupID = QVariant(query.value(record.indexOf("ID"))).toInt();
		QString contactGroupName = QVariant(query.value(record.indexOf("Name"))).toString();
		int contactGroupParentID = QVariant(query.value(record.indexOf("ParentCategory"))).toInt();
		QString contactGroupRemark = QVariant(query.value(record.indexOf("Remark"))).toString();

		UserGroup *group = new UserGroup();
		group->setGroupID(contactGroupID);
		group->setGroupName(contactGroupName);
		group->setParentGroupID(contactGroupParentID);

		contactGroupHash.insert(contactGroupID, group);

	}



	qDebug()<<"UsersManager::slotUpdateContactsInfo()~~~~";

}

*/


/*
 void UsersManager::slotLoadContacts(ExpandListView *widgetBox, bool loadFromFile, const QString &string){

	QString file; // = QApplication::applicationDirPath()+QDir::separator()+"contacts.xml";

		if(string.isEmpty() || !QFile(string).exists()){
			file = ":/text/resources/text/default_contacts.xml";
		}


		if(loadFromFile){
			widgetBox->setFileName(file);
			widgetBox->load();
		}else{
			widgetBox->loadContents(string);
		}


		qDebug()<<"~~~~~~~~~~~~~ Load Contacts ~~~~~~~~~~~~~~";


}
*/

UserInfo * UsersManager::queryUserInfo(const QString &imUserID){
//    qWarning()<<"queryUserInfo(const QString &imUserID)";

    //TODO:查询错误处理
    UserInfo *info = new UserInfo(imUserID/*, this*/);
    if(!queryUserInfo(info)){
        delete info;
        info = 0;
    }else{
        QMutexLocker locker(offlineUserMutex);
        offlineUserInfoHash->insert(imUserID, info);
    }
    return info;

//    QSqlQuery query;
//    QSqlRecord record;
//    QString queryString = QString("select * from users where %1='%2' ").arg(UserInfo::databaseColumnName(IM::PI_UserID).arg(imUserID);

//    query = queryDatabase(queryString, false);
//    if((query.lastError().type() != QSqlError::NoError) || (!query.isValid())){
//        qCritical()<<QString("Can not query data from local database! Error: %1").arg(query.lastError().text());
//        return 0;
//    }

//    record = query.record();
//    query.first();

//    UserInfo *info = new UserInfo(imUserID, this);
//    info->setPassword(QVariant(query.value(record.indexOf(ServerUtilities::databaseColumnName(IM::PI_Password)))).toString());
//    info->setTrueName(QVariant(query.value(record.indexOf(ServerUtilities::databaseColumnName(IM::PI_TrueName)))).toString());
//    info->setNickName(QVariant(query.value(record.indexOf(ServerUtilities::databaseColumnName(IM::PI_NickName)))).toString());
//    info->setGender(IMUserBase::Gender(QVariant(query.value(record.indexOf(ServerUtilities::databaseColumnName(IM::PI_Gender)))).toUInt()));

//    info->setHomeAddress(QVariant(query.value(record.indexOf(ServerUtilities::databaseColumnName(IM::PI_HomeAddress)))).toString());
//    info->setHomePhoneNumber(QVariant(query.value(record.indexOf(ServerUtilities::databaseColumnName(IM::PI_HomePhoneNumber)))).toString());
//    info->setHomeZipCode(QVariant(query.value(record.indexOf(ServerUtilities::databaseColumnName(IM::PI_HomeZipCode)))).toString());
//    info->setPersonalHomepage(QVariant(query.value(record.indexOf(ServerUtilities::databaseColumnName(IM::PI_PersonalHomepage)))).toString());
//    info->setPersonalEmailAddress(QVariant(query.value(record.indexOf(ServerUtilities::databaseColumnName(IM::PI_PersonalEmailAddress)))).toString());

//    info->setLastLoginTime(QVariant(query.value(record.indexOf(ServerUtilities::databaseColumnName(IM::PI_LastLoginTime)))).toDateTime());
//    info->setLastLoginHostAddress(QVariant(query.value(record.indexOf(ServerUtilities::databaseColumnName(IM::PI_LastLoginHostAddress)))).toString());
//    info->setLastLoginHostPort(QVariant(query.value(record.indexOf(ServerUtilities::databaseColumnName(IM::PI_LastLoginHostAddress)))).toUInt());

//    info->setQuestionForSecurity(QVariant(query.value(record.indexOf(ServerUtilities::databaseColumnName(IM::PI_QuestionForSecurity)))).toString());
//    info->setAnswerForSecurity(QVariant(query.value(record.indexOf(ServerUtilities::databaseColumnName(IM::PI_AnswerForSecurity)))).toString());
//    info->setFace(QVariant(query.value(record.indexOf(ServerUtilities::databaseColumnName(IM::PI_Face)))).toString());
//    info->setCompanyName(QVariant(query.value(record.indexOf(ServerUtilities::databaseColumnName(IM::PI_CompanyName)))).toString());
//    info->setJobTitle(QVariant(query.value(record.indexOf(ServerUtilities::databaseColumnName(IM::PI_JobTitle)))).toString());

//    info->setBusinessAddress(QVariant(query.value(record.indexOf(ServerUtilities::databaseColumnName(IM::PI_BusinessAddress)))).toString());
//    info->setBusinessPhoneNumber(QVariant(query.value(record.indexOf(ServerUtilities::databaseColumnName(IM::PI_BusinessPhoneNumber)))).toString());
//    info->setBusinessZipCode(QVariant(query.value(record.indexOf(ServerUtilities::databaseColumnName(IM::PI_BusinessZipCode)))).toString());
//    info->setBusinessFaxNumber(QVariant(query.value(record.indexOf(ServerUtilities::databaseColumnName(IM::PI_BusinessFaxNumber)))).toString());
//    info->setBusinessHomepage(QVariant(query.value(record.indexOf(ServerUtilities::databaseColumnName(IM::PI_BusinessHomepage)))).toString());
//    info->setBusinessEmailAddress(QVariant(query.value(record.indexOf(ServerUtilities::databaseColumnName(IM::PI_BusinessEmailAddress)))).toString());

//    info->setRegistrationTime(QVariant(query.value(record.indexOf(ServerUtilities::databaseColumnName(IM::PI_RegistrationTime)))).toDateTime());
//    info->setLoginTimes(QVariant(query.value(record.indexOf(ServerUtilities::databaseColumnName(IM::PI_LoginTimes)))).toInt());


//    return info;



}

bool UsersManager::queryUserInfo(UserInfo *info){
    //qWarning()<<"queryUserInfo(UserInfo *info)";

    if(!info){
        return false;
    }
    
//    if(!query){
//        if(!openDatabase()){
//            return false;
//        }
//    }else{
//        query.clear();
//    }

    if(!db.isValid()){
        if(!openDatabase()){
            return false;
        }
    }
    QSqlQuery query(db);
    
    QSqlRecord record;
//    QString statement = QString("SELECT * FROM UsersSummaryInfo usi left join UsersDetailedInfo udi on usi.%1=udi.%1 where %2='%3' ").arg(info->databaseColumnName(IM::PI_SysID)).arg(info->databaseColumnName(IM::PI_UserID)).arg(info->getUserID());
    QString statement = QString("call sp_GetUserInfo('%1') ").arg(info->getUserID());

    if(!query.exec(statement)){
        QSqlError error = query.lastError();
        QString msg = QString("Can not query user info from database! %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
//        logMessage(msg, QtServiceBase::Error);
        qCritical()<<msg;

        //TODO:数据库重启，重新连接
        //MySQL数据库重启，重新连接
        if(error.number() == 2006){
            query.clear();
            openDatabase(true);
        }

        return false;
    }
    
    
//    query = queryDatabase(statement, false);
    
    if((query.lastError().type() != QSqlError::NoError)){
        qCritical()<<QString("Can not query data from local database! Error: %1").arg(query.lastError().text());
        return false;
    }
    
    
//    if(!query.isValid()){
//        qCritical()<<QString("Can not query data from local database! Error: %1").arg(query.lastError().text());
//        return false;
//    }
    
    if(!query.first()){
        qCritical()<<QString("Can not query user info! Invalid record! User ID:%1").arg(info->getUserID());
        return false;        
    }

    record = query.record();
    

    //UserInfo *info = new UserInfo(imUserID, this);
    info->setSysID(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PI_SysID)))).toUInt());
    info->setPassword(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PI_Password)))).toString());
    info->setTrueName(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PI_TrueName)))).toString());
    info->setNickName(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PI_NickName)))).toString());
    info->setGender(IMUserBase::Gender(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PI_Gender)))).toUInt()));
    info->setFace(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PI_Face)))).toString());

//    info->setContactGroupsInfoString(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PI_PersonalContactGroupsInfoString)))).toString());
    info->setPersonalContactGroupsVersion(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PI_PersonalContactGroupsInfoVersion)))).toInt());
//    info->setInterestGroupsStringFromDatabase(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PI_InterestGroupsInfoString)))).toString());
    info->setInterestGroupInfoVersion(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PI_InterestGroupsInfoVersion)))).toUInt());
    info->setBlacklistInfoVersion(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PI_BlacklistInfoVersion)))).toInt());
    info->setBlacklistInfoString(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PI_Blacklist)))).toString());
    info->setPersonalSummaryInfoVersion(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PI_PersonalSummaryInfoVersion)))).toInt());
    info->setPersonalDetailInfoVersion(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PI_PersonalDetailInfoVersion)))).toInt());
    info->setFriendshipApply(IMUserBase::FriendshipApply(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PI_FriendshipApply)))).toString().toUInt()));
    info->setShortTalk(IMUserBase::ShortTalk(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PI_ShortTalk)))).toString().toUInt()));
    info->setUserRole(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PI_Role)))).toInt());
    info->setAccountState(IMUserBase::AccountState(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PI_AccountState)))).toString().toUInt()));

    info->setAge(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PI_Age)))).toInt());
    info->setHomeAddress(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PI_HomeAddress)))).toString());
    info->setHomePhoneNumber(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PI_HomePhoneNumber)))).toString());
    info->setHomeZipCode(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PI_HomeZipCode)))).toString());
    info->setPersonalHomepage(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PI_PersonalHomepage)))).toString());
    info->setPersonalEmailAddress(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PI_PersonalEmailAddress)))).toString());

//    info->setLastLoginTime(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PI_LastLoginTime)))).toDateTime());
//    info->setLastLoginHostAddress(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PI_LastLoginHostAddress)))).toString());
//    info->setLastLoginHostPort(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PI_LastLoginHostAddress)))).toUInt());

    info->setQuestionForSecurity(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PI_QuestionForSecurity)))).toString());
    info->setAnswerForSecurity(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PI_AnswerForSecurity)))).toString());
    info->setEmailForSecurity(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PI_EmailForSecurity)))).toString());
    
    info->setCompanyName(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PI_CompanyName)))).toString());
    info->setJobTitle(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PI_JobTitle)))).toString());
    info->setBusinessAddress(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PI_BusinessAddress)))).toString());
    info->setBusinessPhoneNumber(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PI_BusinessPhoneNumber)))).toString());
    info->setBusinessZipCode(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PI_BusinessZipCode)))).toString());
    info->setBusinessFaxNumber(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PI_BusinessFaxNumber)))).toString());
    info->setBusinessHomepage(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PI_BusinessHomepage)))).toString());
    info->setBusinessEmailAddress(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PI_BusinessEmailAddress)))).toString());

    info->setRegistrationTime(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PI_RegistrationTime)))).toDateTime());
//    info->setLoginTimes(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PI_LoginTimes)))).toInt());
    info->setDescription(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PI_Description)))).toString());


    getUserInterestGroupsFromDatabase(info);
    getUserLastLoginInfo(info);

    info->clearUpdatedProperties();


    
    return true;
    

}

bool UsersManager::getUserInterestGroupsFromDatabase(UserInfo* info){

    if(!info){
        return false;
    }

    if(!db.isValid()){
        if(!openDatabase()){
            return false;
        }
    }
    QSqlQuery query(db);
    //QString statement = QString("select %1 from InterestGroupMembers where %2='%3' ").arg(InterestGroup::databaseColumnName(IM::PIG_GroupID).arg(InterestGroup::databaseColumnName(IM::PIG_MemberSysID)).arg(info->getUserID());
    QString statement = QString("call sp_GetUserInterestGroups('%1') ").arg(info->getUserID());

    if(!query.exec(statement)){
        QSqlError error = query.lastError();
        QString msg = QString("Can not query user interest groups info from database! %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
        qCritical()<<msg;

        //TODO:数据库重启，重新连接
        //MySQL数据库重启，重新连接
        if(error.number() == 2006){
            query.clear();
            openDatabase(true);
        }

        return false;
    }
    if(query.first()){
        info->setInterestGroups(query.value(0).toString().split(","));
    }

//    QStringList groups;
//     while(query.next()){
//        groups.append(query.value(0).toString());
//     }
//     info->setInterestGroups(groups);

    return true;

}

bool UsersManager::saveUserInfoToDatabase(UserInfo *info){
    //TODO:
    if(!info){
        return false;
    }
    
    QString updateSQLStatement = info->getUpdateSQLStatement();
    if(updateSQLStatement.trimmed().isEmpty()){
        return false;
    }
    
//    if(!query){
//        if(!openDatabase()){
//            return false;
//        }
//    }else{
//        query.clear();
//    }

    if(!db.isValid()){
        if(!openDatabase()){
            return false;
        }
    } 
    QSqlQuery query(db);
    QString statement = QString("update users_detailed_info set %1 where %2='%3' ").arg(updateSQLStatement).arg(info->databaseColumnName(IM::PI_UserID)).arg(info->getUserID());
    
//    query = queryDatabase(statement, false);
//    if((query.lastError().type() != QSqlError::NoError)){
//        qCritical()<<QString("Can not query data from local database! Error: %1").arg(query.lastError().text());
//        return false;
//    }
    
    if(!query.exec(statement)){
        QSqlError error = query.lastError();
        QString msg = QString("Can not write user info to database! %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
//        logMessage(msg, QtServiceBase::Error);
        qCritical()<<msg;

        //TODO:数据库重启，重新连接
        //MySQL数据库重启，重新连接
        if(error.number() == 2006){
            query.clear();
            openDatabase(true);
        }

        return false;
    }else{
        info->clearUpdatedProperties();
    }
    
    
    return true;
    
    
}

bool UsersManager::saveFriendshipApplyRequest(const QString &applicantID, const QString &contactID, const QString &message, quint8 resultCode, bool senderRead, bool receiverRead){


    if(!db.isValid()){
        if(!openDatabase()){
            return false;
        }
    }
    QSqlQuery query(db);
    QString statement = QString("insert into friendshipapply(SenderID, ReceiverID, ExtraMessage, Result, SenderRead, ReceiverRead) values('%1',  '%2', '%3', '%4', '%5', '%6' ) ")
            .arg(applicantID).arg(contactID).arg(message).arg(QString::number(resultCode))
            .arg(senderRead?"1":"0").arg(receiverRead?"1":"0")
            ;

    if(!query.exec(statement)){
        QSqlError error = query.lastError();
        QString msg = QString("Can not write friendship application info to database! %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
        qCritical()<<msg;

        //TODO:数据库重启，重新连接
        //MySQL数据库重启，重新连接
        if(error.number() == 2006){
            query.clear();
            openDatabase(true);
        }

        return false;
    }


    return true;

}

bool UsersManager::deleteFriendshipApplyRequest(const QString &applicantID, const QString &contactID){


    if(!db.isValid()){
        if(!openDatabase()){
            return false;
        }
    }
    QSqlQuery query(db);
    QString statement = QString("delete from friendshipapply where SenderID='%1' and ReceiverID='%2' ").arg(applicantID).arg(contactID);

    if(!query.exec(statement)){
        QSqlError error = query.lastError();
        QString msg = QString("Can not delete friendship application info from database! %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
        qCritical()<<msg;

        //TODO:数据库重启，重新连接
        //MySQL数据库重启，重新连接
        if(error.number() == 2006){
            query.clear();
            openDatabase(true);
        }

        return false;
    }


    return true;

}

bool UsersManager::getFriendshipApplyRequest(const QString &userID, QList<QStringList> *sentApplicationList, QList<QStringList> *receivedApplicationList){

    if(!sentApplicationList || !receivedApplicationList){
        return false;
    }

    if(!db.isValid()){
        if(!openDatabase()){
            return false;
        }
    }
    QSqlQuery query(db);

    QString statement = QString("select ReceiverID, Result, ExtraMessage from friendshipapply where SenderID='%1' and SenderRead = '0' ").arg(userID);
    if(!query.exec(statement)){
        QSqlError error = query.lastError();
        QString msg = QString("Can not get friendship application info from database! %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
        qCritical()<<msg;

        //TODO:数据库重启，重新连接
        //MySQL数据库重启，重新连接
        if(error.number() == 2006){
            query.clear();
            openDatabase(true);
        }

        return false;
    }
    while(query.next()){

        UserInfo::FriendshipApplyResult faResult = UserInfo::FriendshipApplyResult(query.value(1).toUInt());
        if(faResult == UserInfo::FAR_UNKNOWN){continue;}

        QStringList list;
        list << query.value(0).toString()
             <<QString::number(query.value(1).toUInt())
             <<query.value(2).toString()
                ;
        sentApplicationList->append(list);

    }

    statement = QString("select SenderID, Result, ExtraMessage from friendshipapply where ReceiverID='%1' and ReceiverRead = '0' ").arg(userID);
    query.clear();
    if(!query.exec(statement)){
        QSqlError error = query.lastError();
        QString msg = QString("Can not get friendship application info from database! %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
        qCritical()<<msg;

        //TODO:数据库重启，重新连接
        //MySQL数据库重启，重新连接
        if(error.number() == 2006){
            query.clear();
            openDatabase(true);
        }

        return false;
    }
    while(query.next()){
        QStringList list;
        list << query.value(0).toString()
             <<QString::number(query.value(1).toUInt())
             <<query.value(2).toString()
                ;
        receivedApplicationList->append(list);

    }

    //statement = QString("delete from friendshipapply where SenderID='%1' or ReceiverID='%1' ").arg(userID);
    statement = QString("update friendshipapply set SenderRead = '1' where SenderID='%1' and SenderRead = '0'  ").arg(userID);
    query.exec(statement);
    statement = QString("update friendshipapply set ReceiverRead = '1' where ReceiverID='%1' and ReceiverRead = '0' ").arg(userID);
    query.exec(statement);


    return true;

}


//Format: groupID:groupVersion,groupID:groupVersion,groupID:groupVersion
QString UsersManager::getInterestGroupsListForUser(UserInfo* userInfo){
    qDebug()<<"--UsersManager::getInterestGroupsListForUser(...)";

    if(!userInfo){
        qDebug()<<"invalid userInfo";
        return "";
    }

    QStringList infoList;
    QStringList groups = userInfo->getInterestGroups();
    foreach (QString groupID, groups) {
        InterestGroup *group = getInterestGroup(groupID.toUInt());
        if(!group){
            qDebug()<<"invalid group";
            return "";
        }
        infoList.append(groupID + ":" + QString::number(group->getGroupInfoVersion()) + ":" + QString::number(group->getMemberListInfoVersion()));
    }
    
    return infoList.join(",");

}

//Format: ?
QString UsersManager::getInterestGroupInfoStringForUser(UserInfo* userInfo, quint32 groupID){

    QString infoString = "";
    
    if(!userInfo){
        return infoString;
    }
    
    InterestGroup *group = getInterestGroup(groupID);
    if(!group){
        return infoString;
    }
    
    return group->getGroupInfoString();
    
   
}

//Format: ?
QString UsersManager::getInterestGroupMembersInfoStringForUser(UserInfo* userInfo, InterestGroup *group){

    QString infoString = "";
    
    if(!userInfo){
        return infoString;
    }
    
//    Group *group = getGroup(groupID);
    if(!group){
        return infoString;
    }
    
    QStringList infoList;
    QHash <QString/*Member's ID*/, quint32/*Member's Role*/> membersHash = group->getMembersHash();
    QStringList members = membersHash.keys();
    
    foreach (QString memberUserID, members) {
        UserInfo *memberInfo = getUserInfo(memberUserID);
        if(!memberInfo){continue;}
        QStringList memberInfoList;
        memberInfoList << memberInfo->getUserID() << QString::number(memberInfo->getPersonalDetailInfoVersion()) << QString::number(membersHash.value(memberUserID));
        infoList.append(memberInfoList.join(QString(CONTACT_INFO_SEPARATOR)));        
    }
    
    return infoList.join(QString(PACKET_DATA_SEPARTOR));
       
}


bool UsersManager::openDatabase(bool reopen){

    if(reopen){       
//        if(query){
//            query.clear();
//            delete query;
//            query = 0;
//        }
        DatabaseUtility::closeDBConnection(Server_DB_CONNECTION_NAME);
    }


    //QSqlDatabase db = QSqlDatabase::database(Server_DB_CONNECTION_NAME);
    db = QSqlDatabase::database(Server_DB_CONNECTION_NAME);
    if(!db.isValid()){
        QSqlError err;
//        err = databaseUtility->openDatabase(MYSQL_DB_CONNECTION_NAME,
//                                            REMOTE_SITOY_COMPUTERS_DB_DRIVER,
//                                            REMOTE_SITOY_COMPUTERS_DB_SERVER_HOST,
//                                            REMOTE_SITOY_COMPUTERS_DB_SERVER_PORT,
//                                            REMOTE_SITOY_COMPUTERS_DB_USER_NAME,
//                                            REMOTE_SITOY_COMPUTERS_DB_USER_PASSWORD,
//                                            REMOTE_SITOY_COMPUTERS_DB_NAME,
//                                            HEHUI::MYSQL);
        HEHUI::Settings settings(APP_NAME, APP_VERSION, SETTINGS_FILE_NAME, "./");
        DatabaseUtility databaseUtility;
        err = databaseUtility.openDatabase(Server_DB_CONNECTION_NAME,
                                            settings.getDBDriver(),
                                            settings.getDBServerHost(),
                                            settings.getDBServerPort(),
                                            settings.getDBServerUserName(),
                                            settings.getDBServerUserPassword(),
                                            settings.getDBName(),
                                            settings.getDBType());
        if (err.type() != QSqlError::NoError) {
            qCritical() << QString("An error occurred when opening the database on '%1'! %2").arg(REMOTE_SITOY_COMPUTERS_DB_SERVER_HOST).arg(err.text());
            return false;
        }

    }

    db = QSqlDatabase::database(Server_DB_CONNECTION_NAME);
    if(!db.isOpen()){
        qCritical()<<QString("Database is not open! %1").arg(db.lastError().text());
        return false;
    }

//    if(!query){
//        query = new QSqlQuery(db);
//    }


    return true;



}



QSqlQuery UsersManager::queryDatabase(const QString & queryString, bool localConfigDatabase) {

	QSqlQuery query;
	DatabaseUtility du;

        Settings settings;
        QString userPrivateDataFilePath = settings.getDataRootPath();

	if(localConfigDatabase){
		query = du.queryDatabase(queryString,
				LOCAL_SERVER_DB_CONNECTION_NAME,
				LOCAL_SERVER_DB_DRIVER,
				"",
				0,
				"",
				"",
				userPrivateDataFilePath,
				HEHUI::SQLITE);
	}else{
		query = du.queryDatabase(queryString,
				REMOTE_SITOY_COMPUTERS_DB_CONNECTION_NAME,
				REMOTE_SITOY_COMPUTERS_DB_DRIVER,
				REMOTE_SITOY_COMPUTERS_DB_SERVER_HOST,
				REMOTE_SITOY_COMPUTERS_DB_SERVER_PORT,
				REMOTE_SITOY_COMPUTERS_DB_USER_NAME,
				REMOTE_SITOY_COMPUTERS_DB_USER_PASSWORD,
				REMOTE_SITOY_COMPUTERS_DB_NAME,
				HEHUI::MYSQL);
	}

	return query;


}


QSqlQuery UsersManager::queryDatabase(const QString & queryString, const QString &connectionName, const QString &driver,
		const QString &host, int port, const QString &user, const QString &passwd,
		const QString &databaseName, HEHUI::DatabaseType databaseType) {


	QSqlQuery query;
        DatabaseUtility du;

	query = du.queryDatabase(queryString, connectionName, driver, host, port, user, passwd, databaseName, databaseType);

	return query;

}









/////////////////////////// Group Manager ////////////////////////////////////////    
  
InterestGroup* UsersManager::getInterestGroup(quint32 groupID){
    
    if(groupID < 1){
        return 0;
    }
    
    Group * Group = 0;
    if(groupHash->contains(groupID)){
        Group = groupHash->value(groupID);
    }else{
        Group = queryInterestGroup(groupID);
    }
    

    return Group;

}

bool UsersManager::createNewInterestGroup(quint32 groupTypeID, quint32 parentGroupID, quint32 creatorSystemID, const QString &groupName ){
    
    
    if(!db.isValid()){
        if(!openDatabase()){
            //*errorType = IM::ERROR_UnKnownError;
            return false;
        }
    } 
    QSqlQuery query(db);
    QString statement = QString("insert into groups(TypeID, ParentGroup, Creator, GroupName) values(%1, %2, %3, '%4') ").arg(groupTypeID).arg(parentGroupID).arg(creatorSystemID).arg(groupName);
    
    if(!query.exec(statement)){
        QSqlError error = query.lastError();
        QString msg = QString("Can not create new group! %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
//        logMessage(msg, QtServiceBase::Error);
        qCritical()<<msg;

        //TODO:数据库重启，重新连接
        //MySQL数据库重启，重新连接
        if(error.number() == 2006){
            query.clear();
            openDatabase(true);
        }
        
        return false;
    }
    
    
    statement = QString("select ID from groups where TypeID=%1 and ParentGroup=%2 and Creator=%3 and GroupName='%4' ").arg(groupTypeID).arg(parentGroupID).arg(creatorSystemID).arg(groupName);
    if(!query.exec(statement)){
        QSqlError error = query.lastError();
        QString msg = QString("Can not query group id from database! %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
//        logMessage(msg, QtServiceBase::Error);
        qCritical()<<msg;

        //TODO:数据库重启，重新连接
        //MySQL数据库重启，重新连接
        if(error.number() == 2006){
            query.clear();
            openDatabase(true);
        }
        
        return false;
    }
    
    if(!query.first()){
        return false;
    }
    
    quint32 groupID = query.value(0).toUInt();
    Group *Group = queryInterestGroup(groupID);
    if(!Group){
        return false;
    }
    
    
    return true;
    
}

QStringList UsersManager::searchInterestGroup(const QString &propertiesString, bool matchExactly, bool searchOnlineUsersOnly){

    
    if(propertiesString.trimmed().isEmpty()){
        return QStringList();
    }
    
    QStringList queryStringList;
    QStringList propertiesList = propertiesString.split(QString(CONTACT_INFO_SEPARATOR));
    if(propertiesList.isEmpty()){
        return QStringList();
    }
    
    InterestGroup info;
    
    foreach (QString property, propertiesList) {
        QStringList list = property.split("=");
        IM::PropertyIDOfGroup pi = IM::PropertyIDOfGroup(list.at(0).toUInt());
        QString value = list.at(1);
        
        if(value.trimmed().startsWith("'") && (!matchExactly)){
            queryStringList.append(info.databaseColumnName(pi)+" like "+value);
        }else{
//            if(pi == IM::PI_Age){
//                QString statement = "";
//                IMUserBase::AgeSection as = IMUserBase::AgeSection(value.toUInt());
//                switch(as){
//                case IMUserBase::AS_16_22:
//                    statement = " Between 16 And 22";
//                    break;
//                case IMUserBase::AS_23_30:
//                    statement = " Between 23 And 30";
//                    break;
//                case IMUserBase::AS_31_40:
//                    statement = " Between 31 And 40";
//                    break;
//                case IMUserBase::AS_40_:
//                    statement = " > 40";
//                    break;
//                default:
//                    statement = " > 0"; 
//                }
                
//                queryStringList.append(info.databaseColumnName(pi)+" "+statement);
                
//            }else{
                queryStringList.append(info.databaseColumnName(pi)+" = "+value);
//            }
            
        }

    }
    
    QString queryString = QString("Select %1, %2, %3, %4, %5, %6 From groups Where %7")
            .arg(info.databaseColumnName(IM::PIG_GroupID))
            .arg(info.databaseColumnName(IM::PIG_GroupTypeID))
            .arg(info.databaseColumnName(IM::PIG_ParentGroupID))
            .arg(info.databaseColumnName(IM::PIG_GroupName))
            .arg(info.databaseColumnName(IM::PIG_CreatorID))
            .arg(info.databaseColumnName(IM::PIG_Description))
            .arg(queryStringList.join(" And "))
            ;
    if(!db.isValid()){
        if(!openDatabase()){
            return QStringList();
        }
    }
    QSqlQuery query(db);
    
    if(!query.exec(queryString)){
        QSqlError error = query.lastError();
        QString msg = QString("Can not query user info from database! %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
        qCritical()<<msg;
        
        //TODO:数据库重启，重新连接
        //MySQL数据库重启，重新连接
        if(error.number() == 2006){
            query.clear();
            openDatabase(true);
        }
        
        return QStringList();
    }
    
    QStringList groupsInfoList;
    while(query.next()){  
        //TODO:query安全性
        //quint32 groupID = query.value(0).toUInt();
        //Group *info = getGroup(groupID);       
        
        QStringList groupList;
        for(int i=0; i<6; i++){
            groupList.append(query.value(i).toString());
//            qWarning()<<i<<":"<<query.value(i).toString();
        }
//        groupList.append(QString::number(info->getOnlineState()));
        
        groupsInfoList.append(groupList.join(QString(GROUP_INFO_SEPARATOR)));
    }
    
    return groupsInfoList;
    
}

bool UsersManager::saveInterestGroupToDatabase(InterestGroup *groupInfo){
    //TODO:
    if(!groupInfo){
        return false;
    }
    
    QString updateSQLStatement = groupInfo->getUpdateSQLStatement();
    if(updateSQLStatement.trimmed().isEmpty()){
        return false;
    }
    

    if(!db.isValid()){
        if(!openDatabase()){
            return false;
        }
    } 
    QSqlQuery query(db);
    QString statement = QString("update groups set %1 where %2='%3' ").arg(updateSQLStatement).arg(groupInfo->databaseColumnName(IM::PIG_GroupID)).arg(groupInfo->getGroupID());
     
    if(!query.exec(statement)){
        QSqlError error = query.lastError();
        QString msg = QString("Can not write group info to database! %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
//        logMessage(msg, QtServiceBase::Error);
        qCritical()<<msg;

        //TODO:数据库重启，重新连接
        //MySQL数据库重启，重新连接
        if(error.number() == 2006){
            query.clear();
            openDatabase(true);
        }

        return false;
    }else{
        groupInfo->clearUpdatedProperties();
    }
    
    
    return true;
    
    
}

bool UsersManager::saveMembershipApplyRequest(quint32 applicantID, quint32 groupID, const QString &message){


    if(!db.isValid()){
        if(!openDatabase()){
            return false;
        }
    }
    QSqlQuery query(db);
    QString statement = QString("insert into membershipapply(ApplicantSystemID, GroupID, ExtraMessage) values('%1',  '%2', '%3' ) ")
            .arg(applicantID).arg(groupID).arg(message);

    if(!query.exec(statement)){
        QSqlError error = query.lastError();
        QString msg = QString("Can not write membership application info to database! %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
        qCritical()<<msg;

        //TODO:数据库重启，重新连接
        //MySQL数据库重启，重新连接
        if(error.number() == 2006){
            query.clear();
            openDatabase(true);
        }

        return false;
    }

    return true;

}

bool UsersManager::deleteMembershipApplyRequest(quint32 applicantID, quint32 groupID){


    if(!db.isValid()){
        if(!openDatabase()){
            return false;
        }
    }
    QSqlQuery query(db);
    QString statement = QString("delete from membership where ApplicantSystemID='%1' and GroupID='%2' ").arg(applicantID).arg(groupID);

    if(!query.exec(statement)){
        QSqlError error = query.lastError();
        QString msg = QString("Can not delete membership application info from database! %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
        qCritical()<<msg;

        //TODO:数据库重启，重新连接
        //MySQL数据库重启，重新连接
        if(error.number() == 2006){
            query.clear();
            openDatabase(true);
        }

        return false;
    }

    return true;

}

bool UsersManager::getMembershipApplyRequest(quint32 groupID, QList<QStringList> *receivedApplicationList){

    if(!receivedApplicationList){
        return false;
    }

    if(!db.isValid()){
        if(!openDatabase()){
            return false;
        }
    }
    QSqlQuery query(db);

    QString statement = QString("select ApplicantSystemID, ExtraMessage from membershipapply where GroupID='%1' ").arg(groupID);
    if(!query.exec(statement)){
        QSqlError error = query.lastError();
        QString msg = QString("Can not get membership application info from database! %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
        qCritical()<<msg;

        //TODO:数据库重启，重新连接
        //MySQL数据库重启，重新连接
        if(error.number() == 2006){
            query.clear();
            openDatabase(true);
        }

        return false;
    }
    while(query.next()){
        QStringList list;
        list <<QString::number(query.value(0).toUInt())
             <<query.value(1).toString()
                ;
        receivedApplicationList->append(list);

    }

    statement = QString("delete from membership where GroupID='%1' ").arg(groupID);
    query.exec(statement);


    return true;

}

bool UsersManager::saveCachedInterestGroupChatMessageFromIMUser(const QString &senderID, quint32 interestGroupID, const QString &message){

    if(!db.isValid()){
        if(!openDatabase()){
            return false;
        }
    }
    QSqlQuery query(db);

    QString msg = message;
    msg.replace("'", "\\'");
    msg.replace("\"", "\\\"");
    QString statement = QString("insert into cachedinterestgroupchatmessages(SenderID, InterestGroupID, Message) values('%1', '%2', '%3' )  ").arg(senderID).arg(interestGroupID).arg(msg);

    if(!query.exec(statement)){
        QSqlError error = query.lastError();
        QString msg = QString("Can not save data to database! %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
//        logMessage(msg, QtServiceBase::Error);
        qCritical()<<msg;

        //TODO:数据库重启，重新连接
        //MySQL数据库重启，重新连接
        if(error.number() == 2006){
            query.clear();
            openDatabase(true);
        }

        return false;
    }


    if((query.lastError().type() != QSqlError::NoError)){
        qCritical()<<QString("Can not save data to database! Error: %1").arg(query.lastError().text());
        return false;
    }

    return true;

}

QStringList UsersManager::cachedInterestGroupChatMessagesForIMUser(UserInfo* userInfo){
    //TODO:

    if(!userInfo){
        return QStringList();
    }

    QStringList interestgroups = userInfo->getInterestGroups();
    if(interestgroups.isEmpty()){return QStringList();}

    if(!db.isValid()){
        if(!openDatabase()){
            return QStringList();
        }
    }
    QSqlQuery query(db);

//    QString imUserID = userInfo->getUserID();

    QString statement = QString("select InterestGroupID, SenderID,  Message, TransmittingTime from cachedinterestgroupchatmessages where InterestGroupID in (%1) and TransmittingTime>'%2'  ").arg(interestgroups.join(",")).arg(userInfo->getLastLoginTime().toString("yyyy-MM-dd hh:mm:ss"));
    if(!query.exec(statement)){
        QSqlError error = query.lastError();
        QString msg = QString("Can not query cached InterestGroup chat messages from database! %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
//        logMessage(msg, QtServiceBase::Error);
        qCritical()<<msg;

        //TODO:数据库重启，重新连接
        //MySQL数据库重启，重新连接
        if(error.number() == 2006){
            query.clear();
            openDatabase(true);
        }

        return QStringList();
    }

    if((query.lastError().type() != QSqlError::NoError)){
        qCritical()<<QString("Can not query cached InterestGroup chat messages from database! Error: %1").arg(query.lastError().text());
        return QStringList();
    }

    QStringList messages;
    while(query.next()){
        QStringList message;
        message.append(QVariant(query.value(0)).toString());
        message.append(QVariant(query.value(1)).toString());
        message.append(QVariant(query.value(2)).toString());
        message.append(QVariant(query.value(3)).toDateTime().toString("yyyy.MM.dd hh:mm:ss"));

        messages.append(message.join(QString(PACKET_DATA_SEPARTOR)));

    }

//    qWarning()<<"messages cached on server:"<<messages.size();

    return messages;

}





InterestGroup * UsersManager::queryInterestGroup(quint32 groupID){
    
    InterestGroup *group = new InterestGroup(groupID);
    if(!queryInterestGroup(group)){
        delete group;
        group = 0;
    }
    
    return group;
    
}

bool UsersManager::queryInterestGroup(InterestGroup *info){

    if(!info){
        return false;
    }
    
    if(!db.isValid()){
        if(!openDatabase()){
            return false;
        }
    }
    QSqlQuery query(db);
    
    QSqlRecord record;
    //QString statement = QString("select * from groups where %1='%2' ").arg(info->databaseColumnName(IM::PIG_GroupID)).arg(info->getGroupID());
    QString statement = QString("call sp_GetInterestGroupInfo(%1); ").arg(info->getGroupID());

    if(!query.exec(statement)){
        QSqlError error = query.lastError();
        QString msg = QString("Can not query group info from database! %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
        qCritical()<<msg;

        //TODO:数据库重启，重新连接
        //MySQL数据库重启，重新连接
        if(error.number() == 2006){
            query.clear();
            openDatabase(true);
        }

        return false;
    }
    
     
    if((query.lastError().type() != QSqlError::NoError)){
        qCritical()<<QString("Can not query data from database! Error: %1").arg(query.lastError().text());
        return false;
    }
       
    
    if(!query.first()){
        qCritical()<<QString("Can not query group info! Invalid record! Group ID:%1").arg(info->getGroupID());
        return false;        
    }

    record = query.record();
    
    info->setGroupTypeID(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PIG_GroupTypeID)))).toUInt());
    info->setParentGroupID(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PIG_ParentGroupID)))).toUInt());
    info->setCreatorID(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PIG_CreatorID)))).toString());
    info->setGroupName(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PIG_GroupName)))).toString());
    info->setCreationTime(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PIG_CreationTime)))).toDateTime());

    info->setGroupInfoVersion(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PIG_GroupInfoVersion)))).toUInt());
    info->setMemberListInfoVersion(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PIG_MemberListInfoVersion)))).toUInt());
    info->setDescription(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PIG_Description)))).toString());
    info->setAnnouncement(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PIG_Announcement)))).toString());
    info->setRemark(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PIG_Remark)))).toString());
//    info->setLastUpdateTime(QVariant(query.value(record.indexOf(info->databaseColumnName(IM::PIG_LastUpdateTime)))).toDateTime());

    
    info->clearUpdatedProperties();
    
    
    
    //TODO:Members

    //statement = QString("select MemberUserID, MemberRole from groupmembers where GroupID=%1 ").arg(info->getGroupID());
    statement = QString("call sp_GetInterestGroupMembers(%1);").arg(info->getGroupID());
    query.clear();
    if(!query.exec(statement)){
        QSqlError error = query.lastError();
        QString msg = QString("Can not query group members info from database! %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
        qCritical()<<msg;

        //TODO:数据库重启，重新连接
        //MySQL数据库重启，重新连接
        if(error.number() == 2006){
            query.clear();
            openDatabase(true);
        }

        return false;
    }
        
    if((query.lastError().type() != QSqlError::NoError)){
        qCritical()<<QString("Can not query data from database! Error: %1").arg(query.lastError().text());
        return false;
    }

    QHash <QString/*Member's ID*/, quint32/*Member's Role*/> membersHash;
    
    while (query.next()) {
        membersHash.insert(query.value(0).toString(), query.value(1).toUInt());
    }
    
    info->setMembersHash(membersHash);
    
    
    return true;
    
    
}











/////////////////////////// ---Group Manager--- ////////////////////////////////////////    













} //namespace HEHUI
