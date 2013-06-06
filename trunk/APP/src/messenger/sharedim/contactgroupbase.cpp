#include "contactgroupbase.h"



namespace HEHUI {

QString ContactGroupBase::Group_Blacklist_Name = tr("Blacklist");
QString ContactGroupBase::Group_Friends_Name = tr("Friends");
QString ContactGroupBase::Group_Strangers_Name = tr("Strangers");

ContactGroupBase::ContactGroupBase(quint32 groupID, const QString &name)
{

    m_groupID = groupID;
    m_creatorName = "";
    m_groupName = name;

}


} //namespace HEHUI
