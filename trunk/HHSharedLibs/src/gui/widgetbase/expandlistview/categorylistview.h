/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Designer of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef CATEGORYLISTVIEW_H
#define CATEGORYLISTVIEW_H

//#include "widgetbox.h"
#include "shared/abstractexpandlistviewInterface.h"
#include "expandlistviewmanager.h"

#include <QtGui/QListWidget>
#include <QtCore/QList>

QT_BEGIN_NAMESPACE

//class ExpandListView;


class QSortFilterProxyModel;
class QRegExp;

namespace HEHUI {

class ExpandListViewCategoryModel;


// List view of a category, switchable between icon and list mode.
// Provides a filtered view.
class CategoryListView : public QListView
{
    Q_OBJECT
public:
    // Whether to access the filtered or unfiltered view
    enum AccessMode { FilteredAccess, UnfilteredAccess };

    explicit CategoryListView(ExpandListViewManager *core, QWidget *parent = 0);
    void setViewMode(ViewMode vm);

    void dropObjectItems(const QList<QDesignerDnDItemInterface*> &item_list);

    using QListView::contentsSize;

    // These methods operate on the filtered/unfiltered model according to accessmode
    int count(AccessMode am) const;
    ExpandListViewInterface::ObjectItem objectItemAt(AccessMode am, const QModelIndex &index) const;
    ExpandListViewInterface::ObjectItem objectItemAt(AccessMode am, int row) const;
    ExpandListViewInterface::ObjectItem objectItem(const QString &objectItemID);

    int indexOfObjectItem(const QString &item_id);

    void removeRow(AccessMode am, int row);
    void setCurrentItem(AccessMode am, int row);

    // These methods operate on the unfiltered model and are used for serialization
    void addObjectItem(const ExpandListViewInterface::ObjectItem &item, const QIcon &icon, bool editable);
    //void removeObjectItem(const QString &item_id);
    bool containsObjectItem(const QString &item_id);
    ExpandListViewInterface::Category category() const;
    bool removeCustomObjectItems();

    // Helper: Ensure a <ui> tag in the case of empty XML
    static QString objectItemDomXml(const ExpandListViewInterface::ObjectItem &item);

    bool updateObjectItemName(const QString &item_id, const QString &name);
    bool updateObjectItemIcon(const QString &item_id, const QIcon &icon);
    bool updateObjectItemIcon(const QString &item_id, const QString &iconName);

signals:
    void scratchPadChanged();
    void pressed(const QString &name, const QPoint &globalPos);
    //void contextMenuEventOnObjectItemOccurs(const QString item_id, const QPoint &global_mouse_pos);
    void itemRemoved();
    void lastItemRemoved();

public slots:
    void filter(const QRegExp &re);

private slots:
    void slotPressed(const QModelIndex &index);
    void removeCurrentItem();
    void editCurrentItem();

    void slotItemActivated(const QModelIndex &index);

protected:
    bool event(QEvent *event);
//    void focusOutEvent ( QFocusEvent * event );
//    void contextMenuEvent(QContextMenuEvent *e);


private:
    int mapRowToSource(int filterRow) const;

    ExpandListViewManager *m_core;

    QSortFilterProxyModel *m_proxyModel;
    ExpandListViewCategoryModel *m_model;

    static CategoryListView *lastFocusedCategoryListView;



};

}  // namespace HEHUI

QT_END_NAMESPACE

#endif // CATEGORYLISTVIEW_H
