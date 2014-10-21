/*
 ****************************************************************************
 * sqlexplorerplugin.h
 *
 * Created on: 2009-8-22
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
 * Last Modified on: 2010-05-19
 * Last Modified by: 贺辉
 ***************************************************************************
 */





#ifndef SQLEXPLORERPLUGIN_H_
#define SQLEXPLORERPLUGIN_H_

#include <QObject>

#include "sqlexplorer.h"
//#include "../../shared/gui/plugin/guipluginbase.h"
#include "HHSharedGUI/hguipluginbase.h"

namespace HEHUI {

class SqlExplorerPlugin : public GUIPluginBase {
    Q_OBJECT

    //Export Plugin
    Q_PLUGIN_METADATA(IID AbstractPluginInterface_IID)
    Q_PLUGIN_METADATA(IID GUIInterface_IID)


public:
    SqlExplorerPlugin();
    virtual ~SqlExplorerPlugin();

    bool isSingle();
    QString name () const;
    QString version() const;
    QString description() const;

    QIcon icon () const;
    QString whatsThis () const;
    QString toolTip () const;

    bool unload();

    bool verifyUser();


private slots:
    void slotMainActionForMenuTriggered();
    void slotWidgetDestoryed(QObject * obj );

private:
    QList<QWidget *> widgetList;



};

} //namespace HEHUI

#endif /* SQLEXPLORERPLUGIN_H_ */
