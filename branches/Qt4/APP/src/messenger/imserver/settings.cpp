


#include "settings.h"


namespace HEHUI {

Settings::Settings( const QString& pName, const QString& pVersion, const QString fileBaseName, const QString fileDirPath, QObject* parent )
    : SettingsCore(pName, pVersion, fileBaseName, fileDirPath, parent )
{

    key = QString("HEHUI").toUtf8();

}

Settings::~Settings()
{



}









} //namespace HEHUI

