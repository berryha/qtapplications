/*
 ****************************************************************************
 * clientinfomodel.h
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

#ifndef CLIENTINFOMODEL_H_
#define CLIENTINFOMODEL_H_


#include <QObject>
#include <QAbstractTableModel>
#include <QSortFilterProxyModel>

#include "../../sharedms/clientinfo.h"



namespace HEHUI {

class ClientInfoModel : public QAbstractTableModel {
	Q_OBJECT

public:
	ClientInfoModel(QObject *parent = 0);
	virtual ~ClientInfoModel();

    void setClientList(QList<ClientInfo*> clientsList);
    void addClientInfo(ClientInfo *clientInfo);

    int rowCount ( const QModelIndex & parent = QModelIndex() ) const ;
    int	columnCount ( const QModelIndex & parent = QModelIndex() ) const;
    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const ;
    QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;


private:
	QList<ClientInfo *> clientsList;


};

class ClientInfoSortFilterProxyModel : public QSortFilterProxyModel{
    Q_OBJECT

public:
    ClientInfoSortFilterProxyModel(QObject *parent);

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

#endif /* CLIENTINFOMODEL_H_ */
