/****************************************************************************
* utilities.cpp
*
* Created on: 2009-11-9
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
  * Last Modified on: 2010-08-19
  * Last Modified by: 贺辉
  ***************************************************************************
*/





#include <QCoreApplication>
#include <QStringList>
#include <QProcess>
#include <QDir>
#include <QMutexLocker>
#include <QDebug>

#include "utilities.h"

#ifdef Q_OS_WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif



namespace HEHUI {


QList<QTranslator *> Utilities::translators = QList<QTranslator *>();
QMutex * Utilities::translatorsMutex = new QMutex();

Utilities::Utilities() {
    // TODO Auto-generated constructor stub


}

Utilities::~Utilities() {
    // TODO Auto-generated destructor stub
}


const QString Utilities::currentUserNameOfOS() {
    QString username;
    QStringList envVariables;
    envVariables << "USERNAME.*" << "USER.*" << "USERDOMAIN.*" << "HOSTNAME.*"
            << "DOMAINNAME.*";

    QStringList environment = QProcess::systemEnvironment();
    foreach (QString string, envVariables)
    {
        int index = environment.indexOf(QRegExp(string));
        if (index != -1) {
            QStringList stringList = environment.at(index).split("=");
            if (stringList.size() == 2) {
                username = stringList.at(1).toUtf8();
                break;
            }
        }
    }

    if (username.isEmpty()) {
        username = "";
    }

    return username;


}

QStringList Utilities::availableTranslationLanguages(const QString &translationFilesDir){

    //查找语言文件
    //Search language files
    QDir dir(translationFilesDir);
    QStringList fileNames = dir.entryList(QStringList("*.qm"));
    qDebug()<<"~~ Language Files: "<<fileNames.join(",");

    if (fileNames.isEmpty()) {
        return QStringList();
    }

    QStringList translationLanguages;
    foreach(QString file, fileNames){
        file.truncate(file.lastIndexOf(".qm", -1, Qt::CaseInsensitive));
        QString translationLanguageName = file.right(5);
        qDebug()<<"~~translationLanguageName:"<<translationLanguageName;
        if((translationLanguageName.size() == 5) && (!translationLanguages.contains(translationLanguageName))){
            translationLanguages.append(translationLanguageName);
        }
    }

    return translationLanguages;

}


bool Utilities::changeLangeuage(const QString &translationFilesDir, const QString &qmLocale){

    qDebug()<<"~~ Locale System Name:"<< QLocale::system().name();

    QMutexLocker locker(translatorsMutex);

    if(qmLocale.size() != 5){
        qCritical()<<"Invalid local name! It should be a string of the form 'language_country', where language is a lowercase, two-letter ISO 639 language code, and country is an uppercase, two-letter ISO 3166 country code.";
        return false;
    }

//    if(!availableTranslationLanguages(translationFilesDir).contains(qmLocale)){
//        return false;
//    }

    foreach(QTranslator *translator, translators){
        qApp->removeTranslator(translator);
        delete translator;
        translator = 0;
    }
    translators.clear();


    QStringList filters;
    filters << QString("*" + qmLocale + ".qm");
    foreach(QString file, QDir(translationFilesDir).entryList(filters, QDir::Files|QDir::System|QDir::Hidden))
    {
        qDebug()<<"~~Loading language file:"<<file;
        QTranslator *translator = new QTranslator();
        if(translator->load(file, translationFilesDir)){
            qApp->installTranslator(translator);
            translators.append(translator);
        }else{
            delete translator;
            translator = 0;
            qDebug()<<"XXLoading language file failed:"<<file;
        }

    }


    return translators.size();

}

int Utilities::versionCompare(const QString &exeFile1Version, const QString &exeFile2Version){


    QStringList exeFile1VersionInfoList = exeFile1Version.split(".");
    QStringList exeFile2VersionInfoList = exeFile2Version.split(".");
    if((exeFile1VersionInfoList.size() != 4) || (exeFile2VersionInfoList.size() != 4)){
        qCritical()<<"Invalid version string!";
        return -2;
    }

    if(exeFile1VersionInfoList[0].toUInt() > exeFile2VersionInfoList[0].toUInt()){
        return 1;
    }else if(exeFile1VersionInfoList[0].toUInt() < exeFile2VersionInfoList[0].toUInt()){
        return -1;
    }

    if(exeFile1VersionInfoList[1].toUInt() > exeFile2VersionInfoList[1].toUInt()){
        return 1;
    }else if(exeFile1VersionInfoList[1].toUInt() < exeFile2VersionInfoList[1].toUInt()){
        return -1;
    }

    if(exeFile1VersionInfoList[2].toUInt() > exeFile2VersionInfoList[2].toUInt()){
        return 1;
    }else if(exeFile1VersionInfoList[2].toUInt() < exeFile2VersionInfoList[2].toUInt()){
        return -1;
    }

    if(exeFile1VersionInfoList[3].toUInt() > exeFile2VersionInfoList[3].toUInt()){
        return 1;
    }else if(exeFile1VersionInfoList[3].toUInt() < exeFile2VersionInfoList[3].toUInt()){
        return -1;
    }


    return 0;

}

void Utilities::msleep(int msec){

#ifdef Q_OS_WIN32
    Sleep(msec);
#else
    usleep(msec*1000);
#endif

}























} //namespace HEHUI


