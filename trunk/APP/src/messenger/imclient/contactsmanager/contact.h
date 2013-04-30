/*
 ****************************************************************************
 * contact.h
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



#ifndef CONTACT_H_
#define CONTACT_H_

#include <QObject>

#include "../../sharedim/constants_global_shared.h"
#include "../../sharedim/imuserbase.h"



namespace HEHUI {

class Contact: public HEHUI::IMUserBase {
    Q_OBJECT
public:
    Contact(QObject *parent = 0);
    Contact(const QString &contactID = "", QObject *parent = 0);
    Contact(const QString &contactID = "", const QString &nickname = "", QObject *parent = 0);
    virtual ~Contact();

    QString databaseColumnName(IM::PropertyIDOfUser propertyID) const;




    quint32 getContactGroupID() const
    {
        return contactGroupID;
    }
    void setContactGroupID(quint32 contactGroupID)
    {
        this->contactGroupID = contactGroupID;
        addUpdatedProperty(IM::PI_ContactGroupID, QString::number(contactGroupID));
    }

    //    quint32 getInterestGroupID() const
    //    {
    //    	return interestGroupID;
    //    }
    //    void setInterestGroupID(quint32 interestGroupID)
    //    {
    //    	this->interestGroupID = interestGroupID;
    //        addUpdatedProperty(IM::PI_InterestGroupID, QString::number(interestGroupID));

    //    }

    QString getRemarkName(){
        return m_remarkName;
    }
    void setRemarkName(const QString &remarkName){
        qDebug()<<"---setRemarkName()";
        this->m_remarkName = remarkName;
        addUpdatedProperty(IM::PI_RemarkName, "'" + m_remarkName + "'");
    }


    void setShowRemarkName(bool show){m_showRemarkName = show;}
    QString displayName() const{
        if(m_showRemarkName && !m_remarkName.trimmed().isEmpty()){
            return m_remarkName;
        }
        QString nickName = getNickName();
        if(nickName.trimmed().isEmpty()){
            return getUserID();
        }else{
            return nickName;
        }
    }


private:

    quint32 contactGroupID;
    quint32 interestGroupID;
    quint32 systemGroupID;
    
    QString m_remarkName;
    bool m_showRemarkName;





};

}

#endif /* CONTACT_H_ */
