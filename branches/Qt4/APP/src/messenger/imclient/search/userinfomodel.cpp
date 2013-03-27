/*
 ****************************************************************************
 * userinfomodel.cpp
 *
 * Created On: 2010-5-24
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
 * Last Modified On: 2010-5-24
 * Last Modified By: 贺辉
 ***************************************************************************
 */




#include "userinfomodel.h"
#include "../imageresource.h"




namespace HEHUI {

UserInfoModel::UserInfoModel(QObject *parent)
	:QAbstractTableModel(parent)
{
	// TODO Auto-generated constructor stub

}

UserInfoModel::~UserInfoModel() {
	usersList.clear();
}

void UserInfoModel::setUsersList(const QList<Contact*> &usersList)
{
    beginResetModel();
    this->usersList = usersList;
    endResetModel();
}

//void UserInfoModel::clear(){
//    beginResetModel();
//    this->usersList = QList<Contact*>();
//    endResetModel();
//}

int UserInfoModel::rowCount ( const QModelIndex & parent) const {
    if(parent.isValid()){
        return 0;
    }
    return usersList.size();
    
}

int UserInfoModel::columnCount ( const QModelIndex & parent) const{
    if(parent.isValid()){
        return 0;
    }
    
    return 3;
    
}

QVariant UserInfoModel::data ( const QModelIndex & index, int role) const{
    if(!index.isValid()){
        return QVariant();
    }
    
    int row = index.row();
    if((row < 0) || (row >= usersList.size())){
        return QVariant();
    }
    
    Contact *info = static_cast<Contact *> (usersList.at(row));
    
    if(role == Qt::DisplayRole || role == Qt::EditRole){
        switch (index.column()) {
        case 0:
            return info->getUserID();
            break;
        case 1:
            return info->getNickName();
            break;
        case 2:
            return info->getAge();
            break;
//        case 3:
//            return info->getAge();
//            break;
        default:
            return QVariant();
        }
    }else if(role == Qt::DecorationRole){
        switch (index.column()) {
        case 0:
            return ImageResource::createIconForContact(info->getFace(), info->getOnlineState());
            break;
        default:
            return QVariant();
        }
    
    }
    
    
    return QVariant();



}

QVariant UserInfoModel::headerData ( int section, Qt::Orientation orientation, int role) const{
    if(role != Qt::DisplayRole){
        return QVariant();
    }
    
    if(orientation ==  Qt::Horizontal){
        switch (section) {
        case 0:
            return QString(tr("User ID"));
            break;
        case 1:
            return QString(tr("Nick Name"));
            break;
        case 2:
            return QString(tr("Age"));
            break;
        case 3:
            return QString(tr(""));
            break;
        default:
            return QVariant();
            break;
        }
        
    }
    
    return QVariant();

}








}
