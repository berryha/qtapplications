/*
 ****************************************************************************
 * systeminfoplugin.h
 *
 * Created on: 2009-8-24
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







#ifndef SYSTEMINFOPLUGIN_H_
#define SYSTEMINFOPLUGIN_H_

#include <QObject>

#include "systeminfo.h"

#include "HHSharedGUI/hguipluginbase.h"

namespace HEHUI {

class SystemInfoPlugin : public GUIPluginBase{
    Q_OBJECT

    //Export Plugin
    Q_PLUGIN_METADATA(IID AbstractPluginInterface_IID)
    Q_PLUGIN_METADATA(IID GUIInterface_IID)

public:
    SystemInfoPlugin();
    virtual ~SystemInfoPlugin();

    bool isSingle();
    QString name () const;
    QString version() const;
    QString description() const;

    QIcon icon () const;
    QString whatsThis () const;
    QString toolTip () const;

    bool unload();


public slots:
    void slotMainActionForMenuTriggered();
    void slotWidgetDestoryed(QObject * obj);

private:
    QList<QWidget *> widgetList;




};

} //namespace HEHUI

#endif /* SYSTEMINFOPLUGIN_H_ */
