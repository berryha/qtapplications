/*
 ****************************************************************************
 * clientinfomodel.cpp
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

#include "clientinfomodel.h"

namespace HEHUI {

ClientInfoModel::ClientInfoModel(QObject *parent)
	:QAbstractTableModel(parent)
{
	// TODO Auto-generated constructor stub

}

ClientInfoModel::~ClientInfoModel() {
	clientsList.clear();
}

void ClientInfoModel::setClientList(QList<ClientInfo*> clientsList)
{
    beginResetModel();
    this->clientsList = clientsList;
    endResetModel();
    
}

void ClientInfoModel::addClientInfo(ClientInfo *clientInfo){
    
    if(!clientsList.contains(clientInfo)){
        this->clientsList.append(clientInfo);
    
    }
    
    beginResetModel();
    this->clientsList = clientsList;
    endResetModel();
    
}

int ClientInfoModel::rowCount ( const QModelIndex & parent) const {
	if(parent.isValid()){
		return 0;
	}
	return clientsList.size();

}

int	 ClientInfoModel::columnCount ( const QModelIndex & parent) const{
	if(parent.isValid()){
		return 0;
	}

	return 10;


}

QVariant ClientInfoModel::data ( const QModelIndex & index, int role) const{
	if(!index.isValid()){
		return QVariant();
	}

	int row = index.row();
	if((row < 0) || (row >= clientsList.size())){
		return QVariant();
	}

	if(role == Qt::DisplayRole || role == Qt::EditRole){
            ClientInfo *info = static_cast<ClientInfo *> (clientsList.at(row));
            switch (index.column()) {
            case 0:
                return info->getComputerName();
                break;
            case 1:
                return info->getWorkgroup();
                break;
            case 2:
                return info->getNetwork();
                break;
            case 3:
                return info->getUsers();
                break;
            case 4:
                return info->getOs();
                break;
            case 5:
                return info->getUsbSDEnabled()?"1":"0";
                break;
            case 6:
                return info->getProgramsEnabled()?"1":"0";
                break;
            case 7:
                return info->getAdministrators();
                break;
            case 8:
                return info->getLastOnlineTime().toString("yyyy.MM.dd hh:mm:ss");
                break;
            case 9:
                return info->getClientVersion();
                break;
            

            default:
                return QVariant();
                break;
            }
	}

	return QVariant();

}

QVariant ClientInfoModel::headerData ( int section, Qt::Orientation orientation, int role) const{
	if(role != Qt::DisplayRole){
		return QVariant();
	}

	if(orientation ==  Qt::Horizontal){
            switch (section) {
            case 0:
                return QString(tr("Computer Name"));
                break;
            case 1:
                return QString(tr("Workgroup"));
                break;
            case 2:
                return QString(tr("Network"));
                break;
            case 3:
                return QString(tr("Users"));
                break;                         
            case 4:
                return QString(tr("OS"));
                break;   
            case 5:
                return QString(tr("USBSD"));
                break; 
            case 6:
                return QString(tr("Programes"));
                break; 
            case 7:
                return QString(tr("Administrators"));
                break; 
            case 8:
                return QString(tr("LastOnlineTime"));
                break;       
            case 9:
                return QString(tr("ClientVersion"));
                break; 
                
            default:
                return QVariant();
                break;
            }
            
	}

	return QVariant();

}








}
