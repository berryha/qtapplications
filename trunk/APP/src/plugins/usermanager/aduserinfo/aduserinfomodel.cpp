#include "aduserinfomodel.h"

namespace HEHUI {

ADUserInfoModel::ADUserInfoModel(QObject *parent)
    :QAbstractTableModel(parent)
{
    // TODO Auto-generated constructor stub

}

ADUserInfoModel::~ClientInfoModel() {
    usersList.clear();
}

void ADUserInfoModel::setADUserList(QList<ADUser*> usersList)
{
    beginResetModel();
    this->usersList = usersList;
    endResetModel();

}

void ADUserInfoModel::addADUser(ADUser *adUser){

    beginResetModel();

    if(!usersList.contains(adUser)){
        this->usersList.append(adUser);
    }

    //this->clientsList = clientsList;
    endResetModel();

}

int ADUserInfoModel::rowCount ( const QModelIndex & parent) const {
    if(parent.isValid()){
        return 0;
    }
    return usersList.size();

}

int	 ADUserInfoModel::columnCount ( const QModelIndex & parent) const{
    if(parent.isValid()){
        return 0;
    }

    return 11;


}

QVariant ADUserInfoModel::data ( const QModelIndex & index, int role) const{
    if(!index.isValid()){
        return QVariant();
    }

    int row = index.row();
    if((row < 0) || (row >= usersList.size())){
        return QVariant();
    }

    if(role == Qt::DisplayRole || role == Qt::EditRole){
            ADUser *info = static_cast<ADUser *> (usersList.at(row));
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
                return info->isJoinedToDomain()?"1":"0";
                break;
            case 8:
                return info->getAdministrators();
                break;
            case 9:
                return info->getLastOnlineTime().toString("yyyy.MM.dd hh:mm:ss");
                break;
            case 10:
                return info->getClientVersion();
                break;


            default:
                return QVariant();
                break;
            }
    }

    return QVariant();

}

QVariant ADUserInfoModel::headerData ( int section, Qt::Orientation orientation, int role) const{
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
                return QString(tr("JoinedToDomain"));
                break;
            case 8:
                return QString(tr("Administrators"));
                break;
            case 9:
                return QString(tr("LastOnlineTime"));
                break;
            case 10:
                return QString(tr("ClientVersion"));
                break;

            default:
                return QVariant();
                break;
            }

    }

    return QVariant();

}

////////////////////////////////////////////////////////////

ADUserInfoSortFilterProxyModel::ADUserInfoSortFilterProxyModel(QObject *parent)
    :QSortFilterProxyModel(parent)
{

    computerName = QRegExp(".*", Qt::CaseInsensitive);
    userName = QRegExp(".*", Qt::CaseInsensitive);
    workgroup = QRegExp(".*", Qt::CaseInsensitive);
    usbSD = QRegExp(".*", Qt::CaseInsensitive);
    mac = QRegExp(".*", Qt::CaseInsensitive);
    ip = QRegExp(".*", Qt::CaseInsensitive);
    os = QRegExp(".*", Qt::CaseInsensitive);

}

void ADUserInfoSortFilterProxyModel::cleanFilters(){

    computerName = QRegExp(".*", Qt::CaseInsensitive);
    userName = QRegExp(".*", Qt::CaseInsensitive);
    workgroup = QRegExp(".*", Qt::CaseInsensitive);
    usbSD = QRegExp(".*", Qt::CaseInsensitive);
    mac = QRegExp(".*", Qt::CaseInsensitive);
    ip = QRegExp(".*", Qt::CaseInsensitive);
    os = QRegExp(".*", Qt::CaseInsensitive);

    invalidateFilter();
}

void ADUserInfoSortFilterProxyModel::setFilters(const QRegExp &computerName, const QRegExp &userName, const QRegExp &workgroup, const QRegExp &usbSD, const QRegExp &mac, const QRegExp &ip, const QRegExp &os, const QRegExp &programs){

    this->computerName = computerName;
    this->userName = userName;
    this->workgroup = workgroup;
    this->usbSD = usbSD;
    this->mac = mac;
    this->ip = ip;
    this->os = os;
    this->programs = programs;

    invalidateFilter();

}

bool ADUserInfoSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index0 = sourceModel()->index(sourceRow, 0, sourceParent);
    QModelIndex index1 = sourceModel()->index(sourceRow, 1, sourceParent);
    QModelIndex index2 = sourceModel()->index(sourceRow, 2, sourceParent);
    QModelIndex index3 = sourceModel()->index(sourceRow, 3, sourceParent);
    QModelIndex index4 = sourceModel()->index(sourceRow, 4, sourceParent);
    QModelIndex index5 = sourceModel()->index(sourceRow, 5, sourceParent);
    QModelIndex index6 = sourceModel()->index(sourceRow, 6, sourceParent);


//    return (sourceModel()->data(index0).toString().contains(computerName)
//            && sourceModel()->data(index1).toString().contains(workgroup)
//            && ( sourceModel()->data(index2).toString().contains(ip) && sourceModel()->data(index2).toString().contains(mac) )
//            && sourceModel()->data(index3).toString().contains(userName)
//            && sourceModel()->data(index4).toString().contains(os)
//            && sourceModel()->data(index5).toString().contains(usbSD)
//            && sourceModel()->data(index6).toString().contains(programs)

//            );

    return (index0.data().toString().contains(computerName)
            && index1.data().toString().contains(workgroup)
            && ( index2.data().toString().contains(ip) && index2.data().toString().contains(mac) )
            && index3.data().toString().contains(userName)
            && index4.data().toString().contains(os)
            && index5.data().toString().contains(usbSD)
            && index6.data().toString().contains(programs)

            );

}








} //namespace HEHUI
