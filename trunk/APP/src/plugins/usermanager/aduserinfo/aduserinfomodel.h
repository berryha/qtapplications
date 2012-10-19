#ifndef ADUSERINFOMODEL_H
#define ADUSERINFOMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include <QSortFilterProxyModel>

#include "aduser.h"



namespace HEHUI {

class ADUserInfoModel : public QAbstractTableModel {
    Q_OBJECT

public:
    ADUserInfoModel(QObject *parent = 0);
    virtual ~ADUserInfoModel();

    void setADUserList(QList<ADUser*> usersList);
    void addADUser(ADUser *adUser);

    int rowCount ( const QModelIndex & parent = QModelIndex() ) const ;
    int	columnCount ( const QModelIndex & parent = QModelIndex() ) const;
    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const ;
    QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;


private:
    QList<ADUser *> usersList;


};

class ADUserInfoSortFilterProxyModel : public QSortFilterProxyModel{
    Q_OBJECT

public:
    ADUserInfoSortFilterProxyModel(QObject *parent);

    void cleanFilters();
    void setFilters(const QRegExp &computerName, const QRegExp &userName, const QRegExp &workgroup, const QRegExp &usbSD, const QRegExp &mac, const QRegExp &ip, const QRegExp &os, const QRegExp &programs);

//    void setFilterComputerName(const QString &computerName);
//    void setFilterUserName(const QString &userName);
//    void setFilterWorkgroup(const QString &workgroup);
//    void setFilterUSBSD(const QString &usbSD);
//    void setFilterMAC(const QString &mac);
//    void setFilterIP(const QString &ip);
//    void setFilterOS(const QString &os);
//    void setFilterPrograms(const QString &computerName);


protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
//    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

private:
    QRegExp computerName;
    QRegExp userName;
    QRegExp workgroup;
    QRegExp usbSD;
    QRegExp mac;
    QRegExp ip;
    QRegExp os;
    QRegExp programs;



};



}
#endif // ADUSERINFOMODEL_H
