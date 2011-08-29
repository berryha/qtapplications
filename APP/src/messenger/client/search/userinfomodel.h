/*
 ****************************************************************************
 * userinfomodel.h
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

#ifndef USERINFOMODEL_H_
#define USERINFOMODEL_H_


#include <QObject>
#include <QAbstractTableModel>

//#include "../../sharedim/imuserbase.h"
#include "contactsmanager/contact.h"



namespace HEHUI {

class UserInfoModel : public QAbstractTableModel {
	Q_OBJECT

public:
	UserInfoModel(QObject *parent = 0);
	virtual ~UserInfoModel();

    void setUsersList(const QList<Contact*> &usersList);
//    void clear();

    int rowCount ( const QModelIndex & parent = QModelIndex() ) const ;
    int	columnCount ( const QModelIndex & parent = QModelIndex() ) const;
    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const ;
    QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

    

private:
	QList<Contact *> usersList;


};

}

#endif /* USERINFOMODEL_H_ */
