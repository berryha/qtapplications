


#include "settings.h"


namespace HEHUI {

Settings::Settings(const QString & fileName, Format format, QObject * parent)
    : QSettings(fileName, format, parent )
{

    key = QString("HEHUI").toUtf8();

}

Settings::~Settings()
{



}









} //namespace HEHUI

