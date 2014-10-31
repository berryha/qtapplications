#include <QDir>
#include <QFile>

#include "settings.h"


namespace HEHUI {

Settings::Settings( const QString& pName, const QString& pVersion, const QString fileBaseName, const QString fileDirPath, QObject* parent )
    : SettingsBase(pName, pVersion, fileBaseName, fileDirPath, parent )
{

    //beginGroup( mProgramVersion );
}

Settings::~Settings()
{
    //endGroup();


}









} //namespace HEHUI

