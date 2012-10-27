#include "aduserinfomodel.h"



namespace HEHUI {

ADUserInfoModel::ADUserInfoModel(QObject *parent)
    :QAbstractTableModel(parent)
{
    // TODO Auto-generated constructor stub

}

ADUserInfoModel::~ADUserInfoModel() {
    usersList.clear();
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

    return m_attributeNames.size();

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
            return info->getAttribute(m_attributeNames.at(index.column()));
    }
    if(role == Qt::UserRole){
        return row;
    }

    return QVariant();

}

QVariant ADUserInfoModel::headerData ( int section, Qt::Orientation orientation, int role) const{
    if(role != Qt::DisplayRole){
        return QVariant();
    }

    if(orientation ==  Qt::Horizontal){
        return ADUser::CommonAttributeName(m_attributeNames.at(section));
    }

    return QVariant();

}

void ADUserInfoModel::setADUserItems(const QStringList &attributeNames, const QList<QStringList/*Attribute Values*/> &userItems){

    clear();

    if(attributeNames.isEmpty() || userItems.isEmpty()){
        return;
    }

    m_attributeNames =attributeNames;

    beginResetModel();

    foreach (QStringList attributeValues, userItems) {
        Q_ASSERT(attributeNames.size() == attributeValues.size());

        ADUser *user = new ADUser();
        for(int i=0;i<attributeNames.size();i++){
            user->setAttribute(attributeNames.at(i), attributeValues.at(i));
        }
        usersList.append(user);
    }

    //beginResetModel();
    //this->usersList = usersList;
    endResetModel();


}

void ADUserInfoModel::clear(){

    if(usersList.isEmpty()){
        return;
    }

    beginResetModel();

    m_attributeNames.clear();

    foreach (ADUser *user, usersList) {
        delete user;
        user = 0;
    }
    usersList.clear();

    endResetModel();

}

ADUser * ADUserInfoModel::getADUser(const QModelIndex & index){
    if(!index.isValid()){
        return 0;
    }

    return usersList.at(index.data(Qt::UserRole).toInt());
}

////////////////////////////////////////////////////////////

ADUserInfoSortFilterProxyModel::ADUserInfoSortFilterProxyModel(QObject *parent)
    :QSortFilterProxyModel(parent)
{

//    computerName = QRegExp(".*", Qt::CaseInsensitive);
//    userName = QRegExp(".*", Qt::CaseInsensitive);
//    workgroup = QRegExp(".*", Qt::CaseInsensitive);
//    usbSD = QRegExp(".*", Qt::CaseInsensitive);
//    mac = QRegExp(".*", Qt::CaseInsensitive);
//    ip = QRegExp(".*", Qt::CaseInsensitive);
//    os = QRegExp(".*", Qt::CaseInsensitive);

}

//void ADUserInfoSortFilterProxyModel::cleanFilters(){

//    computerName = QRegExp(".*", Qt::CaseInsensitive);
//    userName = QRegExp(".*", Qt::CaseInsensitive);
//    workgroup = QRegExp(".*", Qt::CaseInsensitive);
//    usbSD = QRegExp(".*", Qt::CaseInsensitive);
//    mac = QRegExp(".*", Qt::CaseInsensitive);
//    ip = QRegExp(".*", Qt::CaseInsensitive);
//    os = QRegExp(".*", Qt::CaseInsensitive);

//    invalidateFilter();
//}

//void ADUserInfoSortFilterProxyModel::setFilters(const QRegExp &computerName, const QRegExp &userName, const QRegExp &workgroup, const QRegExp &usbSD, const QRegExp &mac, const QRegExp &ip, const QRegExp &os, const QRegExp &programs){

//    this->computerName = computerName;
//    this->userName = userName;
//    this->workgroup = workgroup;
//    this->usbSD = usbSD;
//    this->mac = mac;
//    this->ip = ip;
//    this->os = os;
//    this->programs = programs;

//    invalidateFilter();

//}

bool ADUserInfoSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
//    QModelIndex index0 = sourceModel()->index(sourceRow, 0, sourceParent);
//    QModelIndex index1 = sourceModel()->index(sourceRow, 1, sourceParent);
//    QModelIndex index2 = sourceModel()->index(sourceRow, 2, sourceParent);
//    QModelIndex index3 = sourceModel()->index(sourceRow, 3, sourceParent);
//    QModelIndex index4 = sourceModel()->index(sourceRow, 4, sourceParent);
//    QModelIndex index5 = sourceModel()->index(sourceRow, 5, sourceParent);
//    QModelIndex index6 = sourceModel()->index(sourceRow, 6, sourceParent);


//    return (index0.data().toString().contains(computerName)
//            && index1.data().toString().contains(workgroup)
//            && ( index2.data().toString().contains(ip) && index2.data().toString().contains(mac) )
//            && index3.data().toString().contains(userName)
//            && index4.data().toString().contains(os)
//            && index5.data().toString().contains(usbSD)
//            && index6.data().toString().contains(programs)

//            );

    return true;

}








} //namespace HEHUI
