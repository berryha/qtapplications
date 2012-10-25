#ifndef ADUSERINFOMODEL_H
#define ADUSERINFOMODEL_H

#include <QObject>
#include <QStringList>
#include <QAbstractTableModel>
#include <QSortFilterProxyModel>

#include "aduser.h"



namespace HEHUI {

class ADUserInfoModel : public QAbstractTableModel {
    Q_OBJECT

public:
    ADUserInfoModel(QObject *parent = 0);
    virtual ~ADUserInfoModel();


    int rowCount ( const QModelIndex & parent = QModelIndex() ) const ;
    int	columnCount ( const QModelIndex & parent = QModelIndex() ) const;
    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const ;
    QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

    void setADUserItems(const QStringList &attributeNames, const QList<QStringList/*Attribute Values*/> &userItems);
    ADUser * getADUser(const QModelIndex & index);

private:
    void clear();


private:
    QList<ADUser *> usersList;
    QStringList m_attributeNames;


};

class ADUserInfoSortFilterProxyModel : public QSortFilterProxyModel{
    Q_OBJECT

public:
    ADUserInfoSortFilterProxyModel(QObject *parent);

//    void cleanFilters();
//    void setFilters(const QRegExp &computerName, const QRegExp &userName, const QRegExp &workgroup, const QRegExp &usbSD, const QRegExp &mac, const QRegExp &ip, const QRegExp &os, const QRegExp &programs);


protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
//    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

private:
//    QRegExp computerName;
//    QRegExp userName;
//    QRegExp workgroup;
//    QRegExp usbSD;
//    QRegExp mac;
//    QRegExp ip;
//    QRegExp os;
//    QRegExp programs;



};



}
#endif // ADUSERINFOMODEL_H
