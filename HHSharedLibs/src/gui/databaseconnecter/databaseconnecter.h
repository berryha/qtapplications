/*
 ****************************************************************************
 * databaseconnecter.h
 *
 * Created on: 2009-8-5
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
 * Last Modified on: 2010-05-08
 * Last Modified by: 贺辉
 ***************************************************************************
 */


#ifndef DATABASECONNECTER_H_
#define DATABASECONNECTER_H_

#include <QObject>
#include <QtSql>

#include "../../core/global_core.h"
#include "../guilib.h"

class GUI_LIB_API DatabaseConnecter: public QObject {

    Q_OBJECT

public:
    DatabaseConnecter(QObject *parent = 0);
    virtual ~DatabaseConnecter();

    bool isDatabaseOpened(const QString &connectionName, const QString &driver,
                          const QString &host, int port, const QString &user,
                          const QString &passwd, const QString &databaseName,
                          HEHUI::DatabaseType databaseType);

    bool connectToNewDatabase(QString *connectionName = NULL);

private:
    QSqlDatabase getDatabase(const QString &connectionName,
                             const QString &driver, const QString &host, int port,
                             const QString &user, const QString &passwd,
                             const QString &databaseName, HEHUI::DatabaseType databaseType);

signals:
    void signalNewDatabaseConnected(const QString &connectionName);

private slots:

private:
    QWidget *parentWidget;

};

#endif /* DATABASECONNECTER_H_ */
