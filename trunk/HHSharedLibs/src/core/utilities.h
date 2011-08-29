/****************************************************************************
* utilities.h
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





#ifndef UTILITIES_H_
#define UTILITIES_H_




#include <QString>
#include <QTranslator>
#include <QMutex>

#include "mysharedlib_global.h"


class MYSHAREDLIB_API Utilities {
public:
	Utilities();
	virtual ~Utilities();

	static const QString currentUserNameOfOS();

    static QStringList availableTranslationLanguages(const QString &translationFilesDir);

    //载入语言文件
    //Load translation
    static bool changeLangeuage(const QString &translationFilesDir, const QString &qmLocale);

    static int versionCompare(const QString &exeFile1Version, const QString &exeFile2Version);


private:
    static QList<QTranslator *>translators;
    static QMutex *translatorsMutex;


};

#endif /* UTILITIES_H_ */
