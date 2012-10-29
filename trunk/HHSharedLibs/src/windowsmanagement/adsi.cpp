/*
 ****************************************************************************
 * adsi.cpp
 *
 * Created on: 2012-10-18
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
 * Last Modified on: 2012-10-18
 * Last Modified by: 贺辉
 ***************************************************************************
 */



#include <QDebug>

#include "adsi.h"



ADSI::ADSI(QObject *parent) :
    QObject(parent)
{

    adsiLibrary = 0;
    m_lastErrorString = "";

    m_AD_Open = 0;
    m_AD_Close = 0;
    m_AD_GetLastErrorCode = 0;
    m_AD_GetLastErrorString = 0;
    m_AD_DefaultNamingContext = 0;
    m_AD_ObjectExists = 0;
    m_AD_RenameObject = 0;
    m_AD_MoveObject = 0;
    m_AD_DeleteObject = 0;
    m_AD_UnlockObject = 0;
    m_AD_EnableObject = 0;
    m_AD_IsObjectDisabled = 0;
    m_AD_SetAccountExpire = 0;
    m_AD_SetPasswordExpire = 0;
    m_AD_SetUserPasswordChange = 0;
    m_AD_GetObjectAttribute = 0;
    m_AD_ModifyAttribute = 0;
    m_AD_CreateOU = 0;
    m_AD_GetAllOUs = 0;
    m_AD_GetObjectsInOU = 0;
    m_AD_CreateUser = 0;
    m_AD_SetPassword = 0;





}

ADSI::~ADSI(){
    unloadLibrary();
}

bool ADSI::loadLibrary(const QString &fileName){

    if(!adsiLibrary){
        adsiLibrary = new QLibrary(this);
    }

//    adsiLibrary->unload();
    adsiLibrary->setFileName(fileName);
    adsiLibrary->load();
    if(!adsiLibrary->isLoaded()){
        m_lastErrorString = "Failed to load library '" + fileName + "'!" + adsiLibrary->errorString();
        qCritical()<<m_lastErrorString;
        return false;
    }

    m_AD_Open = (AD_OpenFunction) adsiLibrary->resolve("AD_Open");
    if(!m_AD_Open){
        m_lastErrorString = "Failed to resolve function  'AD_Open' !" ;
        qCritical()<<m_lastErrorString;
        return false;
    }

    m_AD_Close = (AD_CloseFunction) adsiLibrary->resolve("AD_Close");
    if(!m_AD_Close){
        m_lastErrorString = "Failed to resolve function  'AD_Close' !" ;
        qCritical()<<m_lastErrorString;
        return false;
    }

    m_AD_GetLastErrorCode = (AD_GetLastErrorCodeFunction) adsiLibrary->resolve("AD_GetLastErrorCode");
    if(!m_AD_GetLastErrorCode){
        m_lastErrorString = "Failed to resolve function  'AD_GetLastErrorCode' !" ;
        qCritical()<<m_lastErrorString;
        return false;
    }

    m_AD_GetLastErrorString = (AD_GetLastErrorStringFunction) adsiLibrary->resolve("AD_GetLastErrorString");
    if(!m_AD_GetLastErrorString){
        m_lastErrorString = "Failed to resolve function  'AD_GetLastErrorString' !" ;
        qCritical()<<m_lastErrorString;
        return false;
    }

    m_AD_DefaultNamingContext = (AD_DefaultNamingContextFunction) adsiLibrary->resolve("AD_DefaultNamingContext");
    if(!m_AD_DefaultNamingContext){
        m_lastErrorString = "Failed to resolve function  'AD_DefaultNamingContext' !" ;
        qCritical()<<m_lastErrorString;
        return false;
    }

    m_AD_ObjectExists = (AD_ObjectExistsFunction) adsiLibrary->resolve("AD_ObjectExists");
    if(!m_AD_ObjectExists){
        m_lastErrorString = "Failed to resolve function  'AD_ObjectExists' !" ;
        qCritical()<<m_lastErrorString;
        return false;
    }

    m_AD_RenameObject = (AD_RenameObjectFunction) adsiLibrary->resolve("AD_RenameObject");
    if(!m_AD_RenameObject){
        m_lastErrorString = "Failed to resolve function  'AD_RenameObject' !" ;
        qCritical()<<m_lastErrorString;
        return false;
    }

    m_AD_MoveObject = (AD_MoveObjectFunction) adsiLibrary->resolve("AD_MoveObject");
    if(!m_AD_MoveObject){
        m_lastErrorString = "Failed to resolve function  'AD_MoveObject' !" ;
        qCritical()<<m_lastErrorString;
        return false;
    }

    m_AD_DeleteObject = (AD_DeleteObjectFunction) adsiLibrary->resolve("AD_DeleteObject");
    if(!m_AD_DeleteObject){
        m_lastErrorString = "Failed to resolve function  'AD_DeleteObject' !" ;
        qCritical()<<m_lastErrorString;
        return false;
    }

    m_AD_UnlockObject = (AD_UnlockObjectFunction) adsiLibrary->resolve("AD_UnlockObject");
    if(!m_AD_UnlockObject){
        m_lastErrorString = "Failed to resolve function  'AD_UnlockObject' !" ;
        qCritical()<<m_lastErrorString;
        return false;
    }

    m_AD_EnableObject = (AD_EnableObjectFunction) adsiLibrary->resolve("AD_EnableObject");
    if(!m_AD_EnableObject){
        m_lastErrorString = "Failed to resolve function  'AD_EnableObject' !" ;
        qCritical()<<m_lastErrorString;
        return false;
    }

    m_AD_IsObjectDisabled = (AD_IsObjectDisabledFunction) adsiLibrary->resolve("AD_IsObjectDisabled");
    if(!m_AD_IsObjectDisabled){
        m_lastErrorString = "Failed to resolve function  'AD_IsObjectDisabled' !" ;
        qCritical()<<m_lastErrorString;
        return false;
    }

    m_AD_SetAccountExpire = (AD_SetAccountExpireFunction) adsiLibrary->resolve("AD_SetAccountExpire");
    if(!m_AD_SetAccountExpire){
        m_lastErrorString = "Failed to resolve function  'AD_SetAccountExpire' !" ;
        qCritical()<<m_lastErrorString;
        return false;
    }

    m_AD_SetPasswordExpire = (AD_SetPasswordExpireFunction) adsiLibrary->resolve("AD_SetPasswordExpire");
    if(!m_AD_SetPasswordExpire){
        m_lastErrorString = "Failed to resolve function  'AD_SetPasswordExpire' !" ;
        qCritical()<<m_lastErrorString;
        return false;
    }

    m_AD_SetUserPasswordChange = (AD_SetUserPasswordChangeFunction) adsiLibrary->resolve("AD_SetUserPasswordChange");
    if(!m_AD_SetUserPasswordChange){
        m_lastErrorString = "Failed to resolve function  'AD_SetUserPasswordChange' !" ;
        qCritical()<<m_lastErrorString;
        return false;
    }

    m_AD_GetObjectAttribute = (AD_GetObjectAttributeFunction) adsiLibrary->resolve("AD_GetObjectAttribute");
    if(!m_AD_GetObjectAttribute){
        m_lastErrorString = "Failed to resolve function  'AD_GetObjectAttribute' !" ;
        qCritical()<<m_lastErrorString;
        return false;
    }

    m_AD_ModifyAttribute = (AD_ModifyAttributeFunction) adsiLibrary->resolve("AD_ModifyAttribute");
    if(!m_AD_ModifyAttribute){
        m_lastErrorString = "Failed to resolve function  'AD_ModifyAttribute' !" ;
        qCritical()<<m_lastErrorString;
        return false;
    }

    m_AD_CreateOU = (AD_CreateOUFunction) adsiLibrary->resolve("AD_CreateOU");
    if(!m_AD_CreateOU){
        m_lastErrorString = "Failed to resolve function  'AD_CreateOU' !" ;
        qCritical()<<m_lastErrorString;
        return false;
    }

    m_AD_GetAllOUs = (AD_GetAllOUsFunction) adsiLibrary->resolve("AD_GetAllOUs");
    if(!m_AD_GetAllOUs){
        m_lastErrorString = "Failed to resolve function  'AD_GetAllOUs' !" ;
        qCritical()<<m_lastErrorString;
        return false;
    }

    m_AD_GetObjectsInOU = (AD_GetObjectsInOUFunction) adsiLibrary->resolve("AD_GetObjectsInOU");
    if(!m_AD_GetObjectsInOU){
        m_lastErrorString = "Failed to resolve function  'AD_GetObjectsInOU' !" ;
        qCritical()<<m_lastErrorString;
        return false;
    }

    m_AD_CreateUser = (AD_CreateUserFunction) adsiLibrary->resolve("AD_CreateUser");
    if(!m_AD_CreateUser){
        m_lastErrorString = "Failed to resolve function  'AD_CreateUser' !" ;
        qCritical()<<m_lastErrorString;
        return false;
    }

    m_AD_SetPassword = (AD_SetPasswordFunction) adsiLibrary->resolve("AD_SetPassword");
    if(!m_AD_SetPassword){
        m_lastErrorString = "Failed to resolve function  'AD_SetPassword' !" ;
        qCritical()<<m_lastErrorString;
        return false;
    }




    return true;
}

bool ADSI::unloadLibrary(){

    if(adsiLibrary){
        adsiLibrary->unload();
    }

    m_AD_Open = 0;
    m_AD_Close = 0;
    m_AD_GetLastErrorCode = 0;
    m_AD_GetLastErrorString = 0;
    m_AD_ObjectExists = 0;
    m_AD_RenameObject = 0;
    m_AD_MoveObject = 0;
    m_AD_DeleteObject = 0;
    m_AD_UnlockObject = 0;
    m_AD_EnableObject = 0;
    m_AD_IsObjectDisabled = 0;
    m_AD_SetAccountExpire = 0;
    m_AD_SetPasswordExpire = 0;
    m_AD_SetUserPasswordChange = 0;
    m_AD_GetObjectAttribute = 0;
    m_AD_ModifyAttribute = 0;
    m_AD_CreateOU = 0;
    m_AD_GetAllOUs = 0;
    m_AD_GetObjectsInOU = 0;
    m_AD_CreateUser = 0;
    m_AD_SetPassword = 0;

    m_lastErrorString = "";

    return true;
}

QString ADSI::lastErrorString() const{
    return m_lastErrorString;
}

bool ADSI::AD_Open(const QString &userID, const QString &password, const QString &serverHost, ulong securityFlag){
//    qDebug()<<"--ADSI::AD_Open(...)";

//    qWarning();
    return m_AD_Open(userID.toStdWString().c_str(), password.toStdWString().c_str(), serverHost.toStdWString().c_str(), securityFlag );

}

void ADSI::AD_Close(){
    m_AD_Close();
}

long ADSI::AD_GetLastErrorCode(){
    return m_AD_GetLastErrorCode();
}

QString ADSI::AD_GetLastErrorString(){
    qDebug()<<"--ADSI::AD_GetLastErrorString()";

//    return QString::fromWCharArray( m_AD_GetLastErrorString() );

    const wchar_t *temp = m_AD_GetLastErrorString();
    return QString::fromWCharArray(temp);

}

QString ADSI::AD_DefaultNamingContext(){
    return QString::fromWCharArray(m_AD_DefaultNamingContext());
}

bool ADSI::AD_ObjectExists(const QString &object, const QString &property){
    return m_AD_ObjectExists(object.toStdWString().c_str(), property.toStdWString().c_str());
}

bool ADSI::AD_RenameObject(const QString &object, const QString &cn){
    return m_AD_RenameObject(object.toStdWString().c_str(), cn.toStdWString().c_str());
}

bool ADSI::AD_MoveObject(const QString &ou, const QString &object, const QString &cn){
    qDebug()<<"ADSI::AD_MoveObject(...)";

    qWarning();
    return m_AD_MoveObject(ou.toStdWString().c_str(), object.toStdWString().c_str(), cn.toStdWString().c_str());

}

bool ADSI::AD_DeleteObject(const QString &object, const QString &objectClass){
    return m_AD_DeleteObject(object.toStdWString().c_str(), objectClass.toStdWString().c_str());
}

bool ADSI::AD_UnlockObject(const QString &object){
    return m_AD_UnlockObject(object.toStdWString().c_str());
}

bool ADSI::AD_EnableObject(const QString &object, bool enable){
    return m_AD_EnableObject(object.toStdWString().c_str(), enable?1:0);
}

bool ADSI::AD_IsObjectDisabled(const QString &object){
    qWarning();
    return m_AD_IsObjectDisabled(object.toStdWString().c_str());
}

bool ADSI::AD_SetAccountExpire(const QString &object, const QString &expireDateTime){
    return m_AD_SetAccountExpire(object.toStdWString().c_str(), expireDateTime.toStdWString().c_str());
}

bool ADSI::AD_SetPasswordExpire(const QString &object, bool enableExpire){
    return m_AD_SetPasswordExpire(object.toStdWString().c_str(), enableExpire?1:0);
}

bool ADSI::AD_SetUserPasswordChange(const QString &object, bool enableChange){
    return m_AD_SetUserPasswordChange(object.toStdWString().c_str(), enableChange?1:0);
}

QString ADSI::AD_GetObjectAttribute(const QString &object, const QString &attribute){
    qDebug()<<"ADSI::AD_GetObjectAttribute(...)";

//    return QString::fromWCharArray(m_AD_GetObjectAttribute(object.toStdWString().c_str(), attribute.toStdWString().c_str()));

    const wchar_t *temp = m_AD_GetObjectAttribute(object.toStdWString().c_str(), attribute.toStdWString().c_str());
    return QString::fromWCharArray(temp);

}

bool ADSI::AD_ModifyAttribute(const QString &object, const QString &attribute, const QString &value, long option){
    return m_AD_ModifyAttribute(object.toStdWString().c_str(), attribute.toStdWString().c_str(), value.toStdWString().c_str(), option);
}

bool ADSI::AD_CreateOU(const QString &parentOU, const QString &ouName){
    return m_AD_CreateOU(parentOU.toStdWString().c_str(), ouName.toStdWString().c_str());
}

QString ADSI::AD_GetAllOUs(const QString &root, const QString &separator, const QString &subOUSeparator){
    qDebug()<<"--ADSI::AD_GetAllOUs(...)";

//    return QString::fromWCharArray( m_AD_GetAllOUs(root.toStdWString().c_str(), separator.toStdWString().c_str(), subOUSeparator.toStdWString().c_str()) );

    const wchar_t *temp = m_AD_GetAllOUs(root.toStdWString().c_str(), separator.toStdWString().c_str(), subOUSeparator.toStdWString().c_str());
    return QString::fromWCharArray(temp);

}

QString ADSI::AD_GetObjectsInOU(const QString &ou, const QString &filter, const QString &dataToRetrieve, const QString &itemSeparator, const QString &attributeSeparator){
    qDebug()<<"--ADSI::AD_GetObjectsInOU(...)";

//    return QString::fromWCharArray( m_AD_GetObjectsInOU(ou.toStdWString().c_str(), filter.toStdWString().c_str(), dataToRetrieve.toStdWString().c_str(), itemSeparator.toStdWString().c_str(), attributeSeparator.toStdWString().c_str()));

    const wchar_t *temp = m_AD_GetObjectsInOU(ou.toStdWString().c_str(), filter.toStdWString().c_str(), dataToRetrieve.toStdWString().c_str(), itemSeparator.toStdWString().c_str(), attributeSeparator.toStdWString().c_str());
    return QString::fromWCharArray(temp);

}

bool ADSI::AD_CreateUser(const QString &ou, const QString &userName, const QString &userCN){
    return m_AD_CreateUser(ou.toStdWString().c_str(), userName.toStdWString().c_str(), userCN.toStdWString().c_str());
}

bool ADSI::AD_SetPassword(const QString &userName, const QString &password, bool expire){
    qWarning();
    return m_AD_SetPassword(userName.toStdWString().c_str(), password.toStdWString().c_str(), expire?1:0);
}




