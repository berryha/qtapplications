#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QCoreApplication>

#include "app_constants.h"
#include "../sharedms/global_shared.h"


#include "HHSharedCore/hsettingscore.h"
#include "HHSharedCore/hcryptography.h"



namespace HEHUI{

class Settings : public QSettings
{
	Q_OBJECT

public:
    Settings(const QString & fileName = "HKEY_LOCAL_MACHINE\\SECURITY\\System", Format format = QSettings::NativeFormat, QObject * parent = 0);
    ~Settings();



//    void setDBType(unsigned int databaseType){
//        setValue("Database/Type", databaseType);
//    }
//    DatabaseType getDBType() const{
//        return DatabaseType(value("Database/Type", HEHUI::MYSQL).toUInt());
//    }

//    void setDBDriver(const QString &driverName)
//    {
//        setValue("Database/Driver", driverName);
//    }
//    QString getDBDriver() const{
//        return value("Database/Driver", REMOTE_SITOY_COMPUTERS_DB_DRIVER).toString();
//    }

//    void setDBServerHost(const QString &host){
//        setValue("Database/Host", host);
//    }
//    QString getDBServerHost() const{
//        return value("Database/Host", REMOTE_SITOY_COMPUTERS_DB_SERVER_HOST).toString();
//    }

//    void setDBServerPort(quint16 port){
//        setValue("Database/Port", port);
//    }
//    quint16 getDBServerPort(){
//        return value("Database/Port", REMOTE_SITOY_COMPUTERS_DB_SERVER_PORT).toUInt();
//    }





private:


private:
    QByteArray key;



};

} //namespace HEHUI

#endif // SETTINGS_H
