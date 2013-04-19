/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Designer of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef ITEMBOXWIDGET_H
#define ITEMBOXWIDGET_H


#include "itemboxlib.h"
#include "itemboxwidgetbase_p.h"
#include "itemboxeventhandler.h"


namespace HEHUI {


class ItemBoxTreeWidget;
class FilterWidget;

class ITEMBOX_LIB_API ItemBoxWidget : public ItemBoxWidgetBase
{
    Q_OBJECT
public:
    explicit ItemBoxWidget(ItemBoxEventHandler *core, QWidget *parent = 0, Qt::WindowFlags flags = 0);
    virtual ~ItemBoxWidget();

    ItemBoxEventHandler *core() const;

    virtual int categoryCount() const;
    virtual Category category(int cat_idx) const;
    virtual void addCategory(const Category &cat);
    virtual void removeCategory(int cat_idx);
    virtual void removeCategory(const QString &cat_name);
    virtual void updateCategoryName(const QString &old_cat_name, const QString &new_cat_name);


    virtual int itemCount(int cat_idx) const;
    virtual Item item(int cat_idx, int wgt_idx) const;
    virtual Item item(const QString &cat_name, const QString &item_id) const;

    virtual void addItem(int cat_idx, const Item &itm);
    virtual void addItem(const QString &cat_name, const Item &item);

    virtual void removeItem(int cat_idx, int wgt_idx);
    virtual void removeItem(const QString &cat_name, const QString &item_id);

    virtual void moveItem(int old_cat_idx, int new_cat_idx, const QString &item_id);
    virtual void moveItem(const QString &old_cat_name, const QString &new_cat_name, const QString &item_id);

//    void dropItems(const QList<ItemBoxWidgetInterface*> &item_list, const QPoint &global_mouse_pos);

//    virtual void setFileName(const QString &file_name);
//    virtual QString fileName() const;
//    virtual bool load();
//    virtual bool save();

//    virtual bool loadContents(const QString &contents);
    virtual QIcon iconForItem(const QString &itemID, const QString &category = QString()) const;

    bool updateObjectItemName(const QString &cat_name, const QString &item_id, const QString &itemName);
    bool updateObjectItemIcon(const QString &cat_name, const QString &item_id, const QIcon &icon);
    bool updateObjectItemIcon(const QString &cat_name, const QString &item_id, const QString &iconName);

protected:
    bool eventFilter(QObject *o, QEvent *e);

    virtual void dragEnterEvent (QDragEnterEvent * event);
    virtual void dragMoveEvent(QDragMoveEvent * event);
    virtual void dropEvent (QDropEvent * event);

private slots:
//    void handleMousePress(const QString &name, const QString &xml, const QPoint &global_mouse_pos);

private:
    ItemBoxEventHandler *m_core;
    ItemBoxTreeWidget *m_treeWidget;

    FilterWidget *filterWidget;
};



} //namespace HEHUI

#endif // WIDGETBOX_H
