#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QCoreApplication>

#include "app_constants.h"


#include "HHSharedCore/hsettingscore.h"
#include "HHSharedCore/hcryptography.h"



namespace HEHUI{

class Settings : public SettingsCore
{
	Q_OBJECT

public:
    Settings( const QString &appName = APP_NAME, const QString &appVersion = APP_VERSION, const QString fileBaseName = SERVICE_NAME, const QString fileDirPath = QCoreApplication::applicationDirPath(), QObject *parent = 0 );
    ~Settings();



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
        return value("Database/DatabaseName", REMOTE_SITOY_COMPUTERS_DB_NAME).toString();
    }


private:
    //void encData(const QString dataString);
    //QString decData(const QByteArray &dataArray);


private:
    QByteArray key;



};

} //namespace HEHUI

#endif // SETTINGS_H
