/*
 ****************************************************************************
 * dnslookupplugin.h
 *
 * Created on: 2014-9-17
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
 * Last Modified on: 2014-9-17
 * Last Modified by: 贺辉
 ***************************************************************************
 */





#ifndef DNSLOOKUPPLUGIN_H_
#define DNSLOOKUPPLUGIN_H_

#include <QObject>

#include "dnslookup.h"
#include "HHSharedGUI/hguipluginbase.h"

namespace HEHUI {

class DnsLookupPlugin : public GUIPluginBase {
    Q_OBJECT

    //Export Plugin
    Q_PLUGIN_METADATA(IID AbstractPluginInterface_IID)
    Q_PLUGIN_METADATA(IID GUIInterface_IID)


public:
    DnsLookupPlugin();
    virtual ~DnsLookupPlugin();

    bool isSingle();
    QString name () const;
    QString version() const;
    QString description() const;

    QIcon icon () const;
    QString whatsThis () const;
    QString toolTip () const;

    bool unload();



private slots:
    void slotMainActionForMenuTriggered();
    void slotWidgetDestoryed(QObject * obj );

private:
    QList<QWidget *> widgetList;



};

} //namespace HEHUI

#endif /* DNSLOOKUPPLUGIN_H_ */
