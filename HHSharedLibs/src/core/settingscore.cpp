/*
 ****************************************************************************
 * settingscore.cpp
 *
 * Created on: 2009-4-27
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
 * Last Modified on: 2010-05-07
 * Last Modified by: 贺辉
 ***************************************************************************
 */




#include <QDir>
#include <QFile>
#include <QLocale>

#include "settingscore.h"

SettingsCore::SettingsCore( const QString& pName, const QString& pVersion, const QString fileBaseName, const QString fileDirPath, QObject* parent )
            //: QSettings( QDir::convertSeparators( QString( "%1/.%2/%3.ini" ).arg( QDir::homePath(), pName, pName ) ), QSettings::IniFormat, parent )
            : QSettings( QDir::toNativeSeparators( QString( "%1/%2.ini" ).arg( fileDirPath, fileBaseName) ), QSettings::IniFormat, parent )

{

    mProgramName = pName;
    mProgramVersion = pVersion;
    //beginGroup( mProgramVersion );

}

SettingsCore::~SettingsCore()
{
    //endGroup();
}

QString SettingsCore::programName() const
{
    return mProgramName;
}

QString SettingsCore::programVersion() const
{
    return mProgramVersion;
}






void SettingsCore::setLanguage(const QString &language)
{
    setValue("MainWindow/Language", language);
}

QString SettingsCore::getLanguage() const
{
    //return value("MainWindow/Language",QString("en_US")).toString();
    return value("MainWindow/Language", QLocale::system().name()).toString();
}










