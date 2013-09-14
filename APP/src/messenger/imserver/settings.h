#ifndef SERVERSETTINGS_H
#define SERVERSETTINGS_H

#include <QSettings>
#include <QCoreApplication>
#include <QObject>
#include <QDir>

#include "app_constants_server.h"


#include "HHSharedCore/hsettingscore.h"
#include "HHSharedCore/hcryptography.h"

#include "serverlib.h"
 

namespace HEHUI{


class SERVERLIB_API Settings : public SettingsCore
{
	Q_OBJECT

public:
    Settings( const QString &appName = APP_NAME, const QString &appVersion = APP_VERSION, const QString fileBaseName = APP_NAME, const QString fileDirPath = QCoreApplication::applicationDirPath(), QObject *parent = 0 );
    ~Settings();


    void setDataRootPath(const QString &dataRootPath){
    	setValue("Server/DataRootPath", dataRootPath);

    }

    QString getDataRootPath() const{
        QString dataRootPath = QCoreApplication::applicationDirPath() + QDir::separator() + QString(LOCAL_SERVER_DATA_DIR_NAME);
    	return value("Server/DataRootPath",dataRootPath).toString();

    }

    void setLocalDatabaseFilePath(const QString &databaseFilePath){
    	setValue("Server/DatabaseFilePath", databaseFilePath);

    }

    QString getLocalDatabaseFilePath() const{
    	QString databaseFilePath = getDataRootPath() + QDir::separator() + QString(LOCAL_SERVER_DB_NAME);
    	return value("Server/DatabaseFilePath", databaseFilePath).toString();

    }






    void setDBType(unsigned int databaseType){
        setValue("Database/Type", databaseType);
    }
    DatabaseType getDBType() const{
        return DatabaseType(value("Database/Type", HEHUI::MYSQL).toUInt());
    }

    void setDBDriver(const QString &driverName)
    {
        setValue("Database/Driver", driverName);
    }
    QString getDBDriver() const{
        return value("Database/Driver", REMOTE_SITOY_COMPUTERS_DB_DRIVER).toString();
    }

    void setDBServerHost(const QString &host){
        setValue("Database/Host", host);
    }
    QString getDBServerHost() const{
        return value("Database/Host", REMOTE_SITOY_COMPUTERS_DB_SERVER_HOST).toString();
    }

    void setDBServerPort(quint16 port){
        setValue("Database/Port", port);
    }
    quint16 getDBServerPort(){
        return value("Database/Port", REMOTE_SITOY_COMPUTERS_DB_SERVER_PORT).toUInt();
    }

    void setDBServerUserName(const QString &userName){
        QByteArray *destination = new QByteArray();
        Cryptography cryptography;
        cryptography.teaCrypto(destination, userName.toUtf8(), key, true);
        setValue("Database/UserName", *destination);
        delete destination;
    }
    QString getDBServerUserName() const{
        QString userName = "";
        QByteArray userNameArray = value("Database/UserName").toByteArray();
        if(userNameArray.isEmpty()){
            userName = REMOTE_SITOY_COMPUTERS_DB_USER_NAME;
        }else{
            QByteArray *destination = new QByteArray();
            Cryptography cryptography;
            cryptography.teaCrypto(destination, userNameArray, key, false);
            userName = QString(*destination);
            delete destination;

        }
        return userName;
    }

    void setDBServerUserPassword(const QString &userPassword){
        QByteArray *destination = new QByteArray();
        Cryptography cryptography;
        cryptography.teaCrypto(destination, userPassword.toUtf8(), key, true);
        setValue("Database/Password", *destination);
        delete destination;
    }
    QString getDBServerUserPassword() const{
        QString password = "";
        QByteArray passwordArray = value("Database/Password").toByteArray();
        if(passwordArray.isEmpty()){
            password = REMOTE_SITOY_COMPUTERS_DB_USER_PASSWORD;
        }else{
            QByteArray *destination = new QByteArray();
            Cryptography cryptography;
            cryptography.teaCrypto(destination, passwordArray, key, false);
            password = QString(*destination);
            delete destination;

        }
        return password;
    }

    void setDBName(const QString &databaseName){
        setValue("Database/DatabaseName", databaseName);
    }
    QString getDBName() const{
        return value("Database/DatabaseName", "im").toString();
    }


    QString getChatImageCacheDir(){
        QString pictureCacheDir = m_imagesDir + QString("/cache");
        if(!QFile::exists(pictureCacheDir)){
            QDir dir;
            dir.mkpath(pictureCacheDir);
        }
        return pictureCacheDir;
    }
    QString getUserFaceDir(){
        QString pictureCacheDir = m_imagesDir + QString("/face");
        if(!QFile::exists(pictureCacheDir)){
            QDir dir;
            dir.mkpath(pictureCacheDir);
        }
        return pictureCacheDir;
    }



private:
    //void encData(const QString dataString);
    //QString decData(const QByteArray &dataArray);


private:
    QByteArray key;

    QString m_imagesDir;



};

} //namespace HEHUI

#endif // SERVERSETTINGS_H
