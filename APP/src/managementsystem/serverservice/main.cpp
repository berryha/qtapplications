

#include <QtCore/QDir>
#include <QtCore/QSettings>

#include "app_constants.h"
#include "serverservice/serverservice.h"

#include "HHSharedCore/hlogdebug.h"
#include "HHSharedCore/hglobal_core.h"

#include "settings.h"

#include <iostream>
using namespace std;

int main(int argc, char **argv)
{
    Q_INIT_RESOURCE(resources);

//#if !defined(Q_WS_WIN)
//    // QtService stores service settings in SystemScope, which normally require root privileges.
//    // To allow testing this example as non-root, we change the directory of the SystemScope settings file.
//    QSettings::setPath(QSettings::NativeFormat, QSettings::SystemScope, QDir::tempPath());
//    qWarning("(Example uses dummy settings file: %s/QtSoftware.conf)", QDir::tempPath().toLatin1().constData());
//#endif

    //HEHUI::ServerService service(argc, argv, "ServerService", "Server Service For Computer Management System");
    HEHUI::ServerService service(argc, argv, SERVICE_NAME, APP_NAME);

    cout<<qPrintable(QString(APP_NAME) + " Build " + QString(APP_VERSION))<<endl<<endl;

    QStringList arguments;
    for(int i = 0; i < argc; i++){
        arguments.append(QString(argv[i]));
    }

    //reset the message handler
    qInstallMessageHandler(0);

    if(arguments.contains("-log", Qt::CaseInsensitive)){
        qInstallMessageHandler(logDebug);
        qAddPostRoutine(closeDebugLog);
    }

    if(arguments.contains("-setup", Qt::CaseInsensitive)){
        QStringList databaseTypes;
        databaseTypes<<"Other"<<"MYSQL"<<"SQLITE"<<"POSTGRESQL"<<"FIREBIRD"<<"DB2"<<"ORACLE"<<"M$ SQLSERVER"<<"M$ ACCESS";

        HEHUI::Settings settings(APP_NAME, APP_VERSION, SERVICE_NAME, "./");
        qWarning()<<"Current Database Info:";
        qWarning()<<"\tDatabase type: "<<databaseTypes.at(settings.getDBType());
        qWarning()<<"\tDriver: "<<settings.getDBDriver();
        qWarning()<<"\tDatabase server address: "<<settings.getDBServerHost();
        qWarning()<<"\tDatabase server port: "<<settings.getDBServerPort();
        qWarning()<<"\tUser name: "<<settings.getDBServerUserName();
        //qWarning()<<"\tPassword: "<<settings.getDBServerUserPassword();
        qWarning()<<"\tDatabase name: "<<settings.getDBName();
        qWarning();

        string input = "";
        bool ok = false;

        cout<<"Database type:"<<endl;
        for(int i = 0; i<databaseTypes.size(); i++){
            QString type = databaseTypes.at(i);
            cout<<"\t"<<i<<":"<<qPrintable(type)<<endl;
        }
        //cout<<" 0:Other  1:MYSQL  2:SQLITE  3:POSTGRESQL  4:FIREBIRD  5:DB2  6:ORACLE  7:M$SQLSERVER  8:M$ACCESS"<<endl;
        int type = -1;
        while(1){
            cout<<"Please select database type number: ";
            cin>>input;
            type = QString::fromStdString(input).toInt(&ok);
            if(ok && type >= 0 && type < databaseTypes.size()){
                break;
            }else{
                cout<<"Invalid type number!"<<endl;
            }
        }
        cout<<endl;
        input = "";

        cout<<"Available Database Drivers:"<<endl;
        QStringList drivers = DatabaseUtility::availableDrivers();
        for(int i = 0; i<drivers.size(); i++){
            QString driver = drivers.at(i);
            cout<<"\t"<<i<<":"<<qPrintable(driver)<<endl;
        }
        QString driver = "";
        while(1){
            cout<<"Please select database driver number: ";
            cin>>input;
            int driverNO = QString::fromStdString(input).toInt(&ok);
            if(ok && driverNO >= 0 && driverNO < drivers.size()){
                driver = drivers.at(driverNO);
                break;
            }else{
                cout<<"Invalid driver number!"<<endl;
            }
        }
        cout<<endl;
        input = "";

        QString host = "";
        while(1){
            cout<<"Please input database server host name or IP address: ";
            cin>>input;
            host = QString::fromStdString(input);
            if(QHostAddress(host).isNull()){
                cout<<"Invalid database server host name or IP address!"<<endl;
            }else{
                break;
            }
        }
        cout<<endl;
        input = "";

        int port = 0;
        while(1){
            cout<<"Please input database server port: ";
            cin>>input;
            port = QString::fromStdString(input).toInt(&ok);
            if(ok && port > 0 && port < 65535){
                break;
            }else{
                cout<<"Invalid database server port!"<<endl;
            }

        }
        cout<<endl;
        input = "";

        QString databaseName = "";
        while(1){
            cout<<"Please input database name: ";
            cin>>input;
            databaseName = QString::fromStdString(input).trimmed();
            if(databaseName.isEmpty()){
                cout<<"Invalid database name!"<<endl;
            }else{
                break;
            }

        }
        cout<<endl;
        input = "";

        QString userName = "";
        while(1){
            cout<<"Please input user name: ";
            cin>>input;
            userName = QString::fromStdString(input).trimmed();
            if(userName.isEmpty()){
                cout<<"Invalid user name!"<<endl;
            }else{
                break;
            }

        }
        cout<<endl;
        input = "";

        QString userPassword = "";
        while(1){
            cout<<"Please input password: ";
            cin>>input;
            userPassword = QString::fromStdString(input);

            string userPassword2 = "";
            cout<<"Please type your password again to confirm it: ";
            cin>>userPassword2;
            if(userPassword == QString::fromStdString(userPassword2)){
                break;
            }else{
                cout<<"The two passwords you entered did not match!"<<endl;
            }

        }
        cout<<endl;
        input = "";


        qWarning()<<"New Database Info:";
        qWarning()<<"\tDatabase type: "<<databaseTypes.at(type);
        qWarning()<<"\tDatabase driver: "<<driver;
        qWarning()<<"\tDatabase server address: "<<host;
        qWarning()<<"\tDatabase server port: "<<port;
        qWarning()<<"\tDatabase name: "<<databaseName;
        qWarning()<<"\tUser name: "<<userName;
        qWarning()<<"\tPassword: "<<userPassword;
        qWarning();

        qWarning()<<"Testing database connection...";
        DatabaseUtility databaseUtility;
        QSqlError err = databaseUtility.openDatabase(SERVERSERVICE_DB_CONNECTION_NAME,
                                            driver,
                                            host,
                                            port,
                                            userName,
                                            userPassword,
                                            databaseName,
                                            HEHUI::DatabaseType(type));
        if (err.type() != QSqlError::NoError) {
            qCritical() << QString("Database connection failed! An error occurred when opening the database: %1").arg(err.text());
        }else{
            qWarning()<<"Database connection succeeded!";
        }
        //databaseUtility.closeDBConnection(SERVERSERVICE_DB_CONNECTION_NAME);
        qWarning();


        QString confirmString = "";
        cout<<"Do you wnat to save the changes? Y:Yes  N:No"<<endl;
        cin>>input;
        confirmString = QString::fromStdString(input).toLower();
        input = "";
        if(confirmString == "y" || confirmString == "yes"){
            settings.setDBType(type);
            settings.setDBDriver(driver);
            settings.setDBServerHost(host);
            settings.setDBServerPort(port);
            settings.setDBName(databaseName);
            settings.setDBServerUserName(userName);
            settings.setDBServerUserPassword(userPassword);
            cout<<"Database Info Saved!"<<endl;
        }else{
            cout<<"Operation canceled! Nothing changes!"<<endl;
        }


    }

    //reset the message handler     
    //qInstallMsgHandler(0);   

    return service.exec();
}

