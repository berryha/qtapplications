/*
 ****************************************************************************
 * widgetbase.h
 *
 * Created on: 2009-8-25
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
 * Last Modified on: 2010-05-10
 * Last Modified by: 贺辉
 ***************************************************************************
 */





#ifndef WIDGETBASE_H_
#define WIDGETBASE_H_


#include <QWidget>
#include <QSystemTrayIcon>

#include "../../core/global_core.h"
#include "HHSharedCore/hmysharedlib_global.h"

class MYSHAREDLIB_API WidgetBase :public QWidget {
    Q_OBJECT

public:
    WidgetBase(QWidget *parent = 0);
    virtual ~WidgetBase();

    virtual void initUI() = 0;
    virtual void moveWindow(HEHUI::WindowPosition positon);

    virtual QSystemTrayIcon *SystemTrayIcon() = 0;
    virtual void updateSystemTray(const QString &toolTip, const QIcon &icon, QMenu *menu = 0);
    virtual void showSystemTrayMsg( const QString & title, const QString & message, QSystemTrayIcon::MessageIcon iconType = QSystemTrayIcon::Information, int secondsTimeoutHint = 3 );


protected:
    void languageChange() = 0;
    void closeEvent ( QCloseEvent * event ) = 0;





};

#endif /* WIDGETBASE_H_ */
