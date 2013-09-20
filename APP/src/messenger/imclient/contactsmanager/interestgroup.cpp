/*
 * groupfromserver.cpp
 *
 *  Created on: 2011-2-28
 *      Author: hehui
 */

#include "interestgroup.h"

namespace HEHUI {

InterestGroup::InterestGroup(quint32 groupID, const QString &groupName, QObject *parent)
    :InterestGroupBase(groupID, groupName, parent)
{
	

    

}

InterestGroup::~InterestGroup() {
	// TODO Auto-generated destructor stub
}

QString InterestGroup::databaseColumnName(IM::PropertyIDOfGroup propertyID) const{
    
    QString columnName = "";
    switch(propertyID){
    case IM::PIG_GroupID:
        columnName = "ID";
        break;
    case IM::PIG_GroupTypeID:
        columnName = "TypeID";
        break;
    case IM::PIG_ParentGroupID:
        columnName = "ParentGroup";
        break;
    case IM::PIG_CreatorID:
        columnName = "Creator";
        break;
    case IM::PIG_GroupName:
        columnName = "GroupName";
        break;
    case IM::PIG_CreationTime:
        columnName = "CreationTime";
        break;
    case IM::PIG_GroupInfoVersion:
        columnName = "GroupInfoVersion";
        break;
        
    case IM::PIG_MemberListInfoVersion:
        columnName = "MemberListVersion";
        break;  
    case IM::PIG_Description:
        columnName = "Description";
        break;         
    case IM::PIG_Announcement:
        columnName = "Announcement";
        break;  
    case IM::PIG_Remark :
        columnName = "Remark";
        break; 
    case IM::PIG_State :
        columnName = "State";
        break;
    case IM::PIG_Privacy :
        columnName = "Privacy";
        break;

        
    default:
        columnName = "";




    }

    return columnName;



}


















}
