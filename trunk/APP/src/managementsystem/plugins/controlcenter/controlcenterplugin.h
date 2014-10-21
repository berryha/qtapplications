/*
 ****************************************************************************
 * controlcenterplugin.h
 *
 * Created on: 2010-07-13
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
 * Last Modified on: 2010-07-13
 * Last Modified by: 贺辉
 ***************************************************************************
 */







#ifndef CONTROLCENTERPLUGIN_H_
#define CONTROLCENTERPLUGIN_H_

#include <QObject>

#include "controlcenter.h"

//#include "../../shared/gui/plugin/guipluginbase.h"
#include "HHSharedGUI/hguipluginbase.h"

namespace HEHUI {


class ControlCenterPlugin : public GUIPluginBase{
    Q_OBJECT

    //Export Plugin
    Q_PLUGIN_METADATA(IID AbstractPluginInterface_IID)
    Q_PLUGIN_METADATA(IID GUIInterface_IID)


public:
    ControlCenterPlugin();
    virtual ~ControlCenterPlugin();

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

#endif /* CONTROLCENTERPLUGIN_H_ */
