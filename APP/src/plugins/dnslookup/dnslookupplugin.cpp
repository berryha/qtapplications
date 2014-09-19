/*
 ****************************************************************************
 * dnslookupplugin.cpp
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
 * Last Modified on: 2014-09-17
 * Last Modified by: 贺辉
 ***************************************************************************
 */






#include <QWidget>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QDebug>

#include "dnslookupplugin.h"


namespace HEHUI {

DnsLookupPlugin::DnsLookupPlugin() {
    dnslookupWidgetList = QList<DnsLookupWidget *> ();

}

DnsLookupPlugin::~DnsLookupPlugin() {
    unload();
}

bool DnsLookupPlugin::isSingle(){
    return false;
}

QString DnsLookupPlugin::name () const{
    return QString(tr("DNS Lookup"));
}

QString DnsLookupPlugin::version() const{
    return "2014.09.17";
}

QString DnsLookupPlugin::description() const{
    return QString(tr("DNS Lookup Plugin"));
}

QIcon DnsLookupPlugin::icon () const{
    return QIcon(":/resources/images/find.png");
}

QString DnsLookupPlugin::whatsThis () const{
    return QString(tr("DNS Lookup"));
}

QString DnsLookupPlugin::toolTip () const{
    return QString(tr("DNS Lookup"));
}

bool DnsLookupPlugin::unload(){
    qDebug("----DnsLookupPlugin::unload()");

    emit signalPluginToBeUnloaded();

    if(dnslookupWidgetList.isEmpty()){
        return true;
    }

    foreach(DnsLookupWidget *wgt, dnslookupWidgetList){
        if(!wgt){break;}
        if(wgt->close()){
            dnslookupWidgetList.removeAll(wgt);
            delete wgt;
            wgt = 0;
        }
    }

    return dnslookupWidgetList.isEmpty();

}

void DnsLookupPlugin::slotMainActionForMenuTriggered(){

    QWidget *parentWidget = qobject_cast<QWidget *> (parent());
    DnsLookupWidget *wgt = new DnsLookupWidget(parentWidget);
    connect(wgt, SIGNAL(destroyed(QObject *)), this, SLOT(slotWidgetDestoryed(QObject *)));

    if(parentWidget){
        if(QMdiArea *mdiArea = qobject_cast<QMdiArea *>(parentWidget)){
            QMdiSubWindow *subWindow = new QMdiSubWindow;
            subWindow->setWidget(wgt);
            subWindow->setAttribute(Qt::WA_DeleteOnClose);

            QLayout *layout = subWindow->layout();
            if(layout){
                layout->setSizeConstraint(QLayout::SetFixedSize);
            }

            mdiArea->addSubWindow(subWindow);
            connect(this, SIGNAL(signalPluginToBeUnloaded()), subWindow, SLOT(close()));
        }
    }

    wgt->show();
    dnslookupWidgetList.append(wgt);
}

void DnsLookupPlugin::slotWidgetDestoryed(QObject * obj){
    qDebug("--DnsLookupPlugin::slotWidgetDestoryed(QObject * obj )");

    DnsLookupWidget *sqlExplorer = static_cast<DnsLookupWidget *> (sender());
    if(sqlExplorer){
        dnslookupWidgetList.removeAll(sqlExplorer);
    }
}



//Q_EXPORT_PLUGIN2(dnsLookupPlugin, DnsLookupPlugin)


} //namespace HEHUI {
