/*
 ****************************************************************************
 * contact.cpp
 *
 * Created On: 2010-7-3
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
 * Last Modified On: 2010-7-3
 * Last Modified By: 贺辉
 ***************************************************************************
 */

#include "contact.h"

namespace HEHUI {

Contact::Contact(QObject *parent)
    :IMUserBase(parent)
{

    contactGroupID = 0;
    interestGroupID = 0;
    systemGroupID = 0;
    
}

Contact::Contact(const QString &contactID, QObject *parent)
    :IMUserBase(contactID, parent)
{

    contactGroupID = 0;
    interestGroupID = 0;
    systemGroupID = 0;
    
}

Contact::Contact(const QString &contactID, const QString &nickname, QObject *parent)
    :IMUserBase(contactID, parent)
{

    setNickName(nickname);
    
    contactGroupID = 0;
    interestGroupID = 0;
    systemGroupID = 0;

}

Contact::~Contact() {
	// TODO Auto-generated destructor stub
}

QString Contact::databaseColumnName(IM::PropertyIDOfUser propertyID) const{
    
//    qDebug()<<"--Contact::databaseColumnName(...)";
    
    QString columnName = "";
    switch(propertyID){
    case IM::PI_UserID:
        columnName = "UserID";
        break;
    case IM::PI_Password:
        columnName = "UserPassword";
        break;
    case IM::PI_TrueName:
        columnName = "TrueName";
        break;
    case IM::PI_NickName:
        columnName = "NickName";
        break;
    case IM::PI_Gender:
        columnName = "Gender";
        break;
    case IM::PI_Age:
        columnName = "Age";
        break;
    case IM::PI_Face:
        columnName = "Face";
        break;
        
//    case IM::PI_InterestGroupID :
//        columnName = "InterestGroupID";
//        break;         
    case IM::PI_ContactGroupID :
        columnName = "ContactGroupID";
        break;                 
    case IM::PI_PersonalInfoVersion:
        columnName = "PersonalInfoVersion";
        break; 
        

    case IM::PI_HomeAddress:
        columnName = "HomeAddress";
        break;
    case IM::PI_HomePhoneNumber:
        columnName = "HomePhoneNumber";
        break;
    case IM::PI_HomeZipCode:
        columnName = "HomeZipCode";
        break;
    case IM::PI_PersonalHomepage:
        columnName = "PersonalHomepage";
        break;
    case IM::PI_PersonalEmailAddress:
        columnName = "PersonalEmailAddress";
        break;
    case IM::PI_LastLoginTime:
        columnName = "LastLoginTime";
        break;
    case IM::PI_LastLoginHostAddress:
        columnName = "LastLoginHostAddress";
        break;
    case IM::PI_LastLoginHostPort:
        columnName = "LastLoginHostPort";
        break;
    case IM::PI_QuestionForSecurity:
        columnName = "QuestionForSecurity";
        break;
    case IM::PI_AnswerForSecurity:
        columnName = "AnswerForSecurity";
        break;

    case IM::PI_CompanyName:
        columnName = "CompanyName";
        break;
    case IM::PI_JobTitle:
        columnName = "JobTitle";
        break;
    case IM::PI_BusinessAddress:
        columnName = "BusinessAddress";
        break;
    case IM::PI_BusinessPhoneNumber:
        columnName = "BusinessPhoneNumber";
        break;
    case IM::PI_BusinessZipCode:
        columnName = "BusinessZipCode";
        break;
    case IM::PI_BusinessFaxNumber:
        columnName = "BusinessFaxNumber";
        break;
    case IM::PI_BusinessHomepage:
        columnName = "BusinessHomepage";
        break;
    case IM::PI_BusinessEmailAddress:
        columnName = "BusinessEmailAddress";
        break;
    case IM::PI_RegistrationTime:
        columnName = "RegistrationTime";
        break;
//    case IM::PI_LoginTimes:
//        columnName = "LoginTimes";
//        break;
//    case IM::PI_FriendshipApply:
//        columnName = "FriendshipApply";
//        break;    
//    case IM::PI_ShortTalk:
//        columnName = "ShortTalk";
//        break;  

    case IM::PI_Role:
        columnName = "Role";
        break;         
        
    default:
        columnName = "";




    }

    return columnName;



}






















}
