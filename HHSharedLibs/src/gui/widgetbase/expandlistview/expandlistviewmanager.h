/*
 * widgetmanager.h
 *
 *  Created on: 2009-9-1
 *      Author: 贺辉
 */

#ifndef EXPANDLISTVIEWMANAGER_H
#define EXPANDLISTVIEWMANAGER_H

#include <QObject>
#include <QModelIndex>
#include <QMenu>

#include "expandlistview.h"

#include "../../guilib.h"


QT_BEGIN_NAMESPACE

namespace HEHUI {

class ExpandListView;

class GUI_LIB_API ExpandListViewManager: public QObject {
    Q_OBJECT

public:
    typedef ExpandListViewInterface::CategoryList CategoryList;

    ExpandListViewManager(QObject *parent);
    virtual ~ExpandListViewManager();

    ExpandListView *createExpandListView(QWidget *parent);

private:

signals:

    void itemActivated(const QModelIndex & index );
    //void itemActivated(ExpandListViewInterface::ObjectItem *wgt);
    void signalContactItemActivated(const QString &id);

    void itemDoubleClicked(const QModelIndex & index);
    void itemEntered(const QModelIndex & index );

    void contextMenuEventOnCategoryOccurs(const QString &cat_name, const QPoint &global_mouse_pos, QMenu *contextMenu);
    void contextMenuEventOnObjectItemOccurs(const QString &item_id, const QPoint &global_mouse_pos, QMenu *contextMenu);
    void signalTooltipEventOnObjectItemOccurs(const QString &item_id, const QPoint &global_item_topLeft_pos, const QPoint &global_mouse_pos);




public slots:
    void slotItemActivated(const QModelIndex & index);
    void slotContactItemActivated(const QString &id);
    void slotItemDoubleClicked(const QModelIndex & index);
    void slotItemEntered(const QModelIndex & index );

    void slotContextMenuEventOnCategoryOccurs(const QString &cat_name, const QPoint &global_mouse_pos, QMenu *contextMenu);
    void slotContextMenuEventOnObjectItemOccurs(const QString &item_id, const QPoint &global_mouse_pos, QMenu *contextMenu);
    void slotTooltipEventOnObjectItemOccurs(const QString &item_id, const QPoint &global_item_topLeft_pos, const QPoint &global_mouse_pos);


private slots:





};

} // namespace HEHUI

QT_END_NAMESPACE

#endif /* EXPANDLISTVIEWMANAGER_H */
