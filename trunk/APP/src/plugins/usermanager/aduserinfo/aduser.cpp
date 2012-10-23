#include "aduser.h"


namespace HEHUI {


QHash<QString/*AD Attribute Name*/, QString/*Common Attribute Name*/> *ADUser::commonAttributeNameHash = 0;
QStringList ADUser::m_ouList = QStringList();
QString ADUser::m_ADDefaultNamingContext = "";


ADUser::ADUser(QObject *parent) :
    QObject(parent)
{

    if(!commonAttributeNameHash){
        commonAttributeNameHash = new QHash<QString, QString>();
        commonAttributeNameHash->insert("accountExpires", tr("Account Expires"));
        commonAttributeNameHash->insert("badPasswordTime", tr("Bad Password Time"));
        commonAttributeNameHash->insert("badPwdCount", tr("Bad Password Count"));
        commonAttributeNameHash->insert("cn", tr("Common Name"));
        commonAttributeNameHash->insert("codePage", tr("Code Page"));
        commonAttributeNameHash->insert("countryCode", tr("Country Code"));
        commonAttributeNameHash->insert("description", tr("Description"));
        commonAttributeNameHash->insert("displayName", tr("Display Name"));
        commonAttributeNameHash->insert("distinguishedName", tr("Distinguished Name"));
        commonAttributeNameHash->insert("dSCorePropagationData", tr("Core Propagation Data"));
        commonAttributeNameHash->insert("instanceType", tr("Instance Type"));
        commonAttributeNameHash->insert("lastLogoff", tr("Last Logoff"));
        commonAttributeNameHash->insert("lastLogon", tr("Last Logon"));
        commonAttributeNameHash->insert("logonCount", tr("Logon Count"));
        commonAttributeNameHash->insert("mail", tr("E-mail"));
        commonAttributeNameHash->insert("memberOf", tr("Member Of"));
        commonAttributeNameHash->insert("name", tr("Name"));
        commonAttributeNameHash->insert("nTSecurityDescriptor", tr("NT Security Descriptor"));
        commonAttributeNameHash->insert("objectCategory", tr("Object Category"));
        commonAttributeNameHash->insert("objectClass", tr("Object Class"));
        commonAttributeNameHash->insert("objectGUID", tr("Object GUID"));
        commonAttributeNameHash->insert("objectSid", tr("Object SID"));
        commonAttributeNameHash->insert("primaryGroupID", tr("Primary Group ID"));
        commonAttributeNameHash->insert("pwdLastSet", tr("Password Last Set"));
        commonAttributeNameHash->insert("sAMAccountName", tr("SAM Account Name"));
        commonAttributeNameHash->insert("sAMAccountType", tr("SAM Account Type"));
        commonAttributeNameHash->insert("telephoneNumber", tr("Telephone Number"));
        commonAttributeNameHash->insert("userAccountControl", tr("User Account Control"));
        commonAttributeNameHash->insert("userPrincipalName", tr("User Principal Name"));
        commonAttributeNameHash->insert("userWorkstations", tr("User Workstations"));
        commonAttributeNameHash->insert("uSNChanged", tr("USN Changed"));
        commonAttributeNameHash->insert("uSNCreated", tr("USN Created"));
        commonAttributeNameHash->insert("whenChanged", tr("When Changed"));
        commonAttributeNameHash->insert("whenCreated", tr("When Created"));


    }



}

void ADUser::setAttribute(const QString &attributeName, const QString &attributeValue){
    attributeHash.insert(attributeName, attributeValue);
}

QString ADUser::getAttribute(const QString &attributeName){
    return attributeHash.value(attributeName);
}


QString ADUser::CommonAttributeName(const QString &attributeName){
    QString name = commonAttributeNameHash->value(attributeName);
    if(name.isEmpty()){
        name = attributeName;
    }

    return name;
}

void ADUser::setOUList(const QStringList &ouList){
    m_ouList = ouList;
}

QStringList ADUser::getOUList(){
    return m_ouList;
}

void ADUser::setADDefaultNamingContext(const QString &adDefaultNamingContext){
    m_ADDefaultNamingContext = adDefaultNamingContext;
}

QString ADUser::getADDefaultNamingContext(){
    return m_ADDefaultNamingContext;
}







} //namespace HEHUI
