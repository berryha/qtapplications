#ifndef ADUSER_H
#define ADUSER_H

#include <QObject>
#include <QHash>



namespace HEHUI {


class ADUser : public QObject
{
    Q_OBJECT
public:
    explicit ADUser(QObject *parent = 0);

//    void setSAMAccountName(const QString &sAMAccountName){this->m_sAMAccountName = sAMAccountName;}
//    QString getSAMAccountName() const{return m_sAMAccountName;}

//    void setDisplayName(const QString &displayName){this->m_displayName = displayName;}
//    QString getDisplayName() const{return m_displayName;}

//    void setUserWorkstations(const QString &userWorkstations){this->m_userWorkstations = userWorkstations;}
//    QString getUserWorkstations() const{return m_userWorkstations;}

//    void setTelephoneNumber(const QString &telephoneNumber){this->m_telephoneNumber = telephoneNumber;}
//    QString getTelephoneNumber() const{return m_telephoneNumber;}

//    void setDescription(const QString &description){this->m_description = description;}
//    QString getDescription() const{return m_description;}

//    void setMemberOf(const QString &memberOf){this->m_memberOf = memberOf;}
//    QString getMemberOf() const{return m_memberOf;}


//    void setObjectGUID(const QString &objectGUID){this->m_objectGUID = objectGUID;}
//    QString getObjectGUID() const{return m_objectGUID;}

//    void setObjectSid(const QString &objectSid){this->m_objectSid = objectSid;}
//    QString getObjectSid() const{return m_objectSid;}


    void setAttribute(const QString &attributeName, const QString &attributeValue);
    QString getAttribute(const QString &attributeName);

    static QString CommonAttributeName(const QString &attributeName);

signals:


public slots:


private:
    static QHash<QString/*AD Attribute Name*/, QString/*Common Attribute Name*/> *commonAttributeNameHash;

//    QString m_sAMAccountName;
//    QString m_displayName;
//    QString m_userWorkstations;
//    QString m_telephoneNumber;
//    QString m_description;
//    QString m_memberOf;

//    QString m_objectGUID;
//    QString m_objectSid;

    QHash<QString/*AD Attribute Name*/, QString/*Attribute Value*/> attributeHash;
    


};

} //namespace HEHUI

#endif // ADUSER_H
