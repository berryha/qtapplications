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

#ifndef EXPANDLISTVIEW_H
#define EXPANDLISTVIEW_H


#include "shared/expandlistviewbase.h"
#include "treewidget.h"
//#include "expandlistviewmanager.h"
#include "shared/filterwidget_p.h"

#include <QTimer>

#include "../../../core/mysharedlib_global.h"


QT_BEGIN_NAMESPACE


namespace HEHUI {

class ExpandListViewManager;

class TreeWidget;

class MYSHAREDLIB_API ExpandListView : public ExpandListViewBase
{
    Q_OBJECT
public:

    explicit ExpandListView(ExpandListViewManager *core, QWidget *parent = 0, Qt::WindowFlags flags = 0);
    virtual ~ExpandListView();


    bool load(CategoryList *cat_list);
    bool load(Category *cat);

    ExpandListViewManager *core() const;

    virtual int categoryCount() const;
    virtual Category category(int cat_idx) const;
    virtual Category category(const QString &cat_name) const;
    virtual void addCategory(const Category &cat);
    virtual void removeCategory(int cat_idx);
    virtual void removeCategory(const QString &cat_name);
    virtual void updateCategoryName(const QString &old_cat_name, const QString &new_cat_name);


    virtual int objectItemCount(int cat_idx) const;
    virtual ObjectItem objectItem(int cat_idx, int item_idx) const;
    virtual ObjectItem objectItem(const QString &cat_name, const QString &item_id) const;
    virtual void addObjectItem(int cat_idx, const ObjectItem &item);
    virtual void addObjectItem(const QString &cat_name, const ObjectItem &item);
    virtual void removeObjectItem(int cat_idx, int item_idx);
    virtual void removeObjectItem(const QString &cat_name, const QString &item_id);
    virtual void moveObjectItem(int old_cat_idx, int new_cat_idx, const QString &item_id);
    virtual void moveObjectItem(const QString &old_cat_name, const QString &new_cat_name, const QString &item_id);

    void dropObjectItems(const QList<QDesignerDnDItemInterface*> &item_list, const QPoint &global_mouse_pos);

    virtual void setFileName(const QString &file_name);
    virtual QString fileName() const;

    virtual bool load();
    virtual bool save();

    virtual bool loadContents(const QString &contents);
    virtual QIcon iconForObjectItem(const QString &className, const QString &category = QString()) const;

    bool updateObjectItemName(const QString &cat_name, const QString &item_id, const QString &itemName);
    bool updateObjectItemIcon(const QString &cat_name, const QString &item_id, const QIcon &icon);
    bool updateObjectItemIcon(const QString &cat_name, const QString &item_id, const QString &iconName);


protected:
    bool eventFilter(QObject *o, QEvent *e);
    //virtual void dragEnterEvent (QDragEnterEvent * event);
    //virtual void dragMoveEvent(QDragMoveEvent * event);
    //virtual void dropEvent (QDropEvent * event);

private slots:
    //void handleMousePress(const QString &name, const QString &xml, const QPoint &global_mouse_pos);

private:
    ExpandListViewManager *m_core;

    //QList<QObject *> objectList;


    TreeWidget *m_treeWidget;

    FilterWidget *filterWidget;

    //QTimer *autoHideTimer;



};

}  // namespace HEHUI

QT_END_NAMESPACE

#endif // EXPANDLISTVIEW_H
