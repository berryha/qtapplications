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

#ifndef TREEWIDGET_H
#define TREEWIDGET_H

#include "shared/expandlistviewbase.h"
//#include "expandlistviewmanager.h"

#include <QTreeWidget>
#include <QIcon>
#include <QtCore/QList>
#include <QtCore/QHash>
#include <QtCore/QXmlStreamReader> // Cannot forward declare them on Mac
#include <QtCore/QXmlStreamWriter>



//class QDesignerDnDItemInterface;

class QTimer;

namespace HEHUI {

class ExpandListViewManager;
class CategoryListView;

// TreeWidget: A tree of categories

class TreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    typedef ExpandListViewInterface::ObjectItem ObjectItem;
    typedef ExpandListViewInterface::ObjectItemList ObjectItemList;
    typedef ExpandListViewInterface::Category Category;
    typedef ExpandListViewInterface::CategoryList CategoryList;

    explicit TreeWidget(ExpandListViewManager *core, QWidget *parent = 0);
    ~TreeWidget();

    bool load(CategoryList *cat_list);
    bool load(Category *cat);

    int categoryCount() const;
    Category category(int cat_idx) const;
    Category category(const QString &cat_name) const;
    void addCategory(const Category &cat);
    /*static*/ int findCategory(const QString &name, const TreeWidget::CategoryList &list);
    void removeCategory(int cat_idx);
    void removeCategory(const QString &cat_name);
    
    void updateCategoryName(const QString &old_cat_name, const QString &new_cat_name);

    int objectItemCount(int cat_idx) const;
    ObjectItem objectItem(int cat_idx, int item_idx) const;
    ObjectItem objectItem(const QString &cat_name, const QString &item_id) const;
    void addObjectItem(int cat_idx, const ObjectItem &item);
    void addObjectItem(const QString &cat_name, const ObjectItem &item);

    void removeObjectItem(int cat_idx, int item_idx);
    void removeObjectItem(const QString &cat_name, const QString &item_id);

    void moveObjectItem(int old_cat_idx, int new_cat_idx, const QString &item_id);
    void moveObjectItem(const QString &old_cat_name, const QString &new_cat_name, const QString &item_id);


    void dropObjectItems(const QList<QDesignerDnDItemInterface*> &item_list);

    void setFileName(const QString &file_name);
    QString fileName() const;
    bool load(ExpandListViewBase::LoadMode loadMode);
    bool loadContents(const QString &contents);
    bool save();
    QIcon iconForObjectItem(QString iconName, QIcon::Mode iconMode = QIcon::Disabled) const;

    bool updateObjectItemName(const QString &cat_name, const QString &item_id, const QString &itemName);
    bool updateObjectItemIcon(const QString &cat_name, const QString &item_id, const QIcon &icon);
    bool updateObjectItemIcon(const QString &cat_name, const QString &item_id, const QString &iconName);

signals:
    void pressed(const QString name, const QPoint &global_mouse_pos);
    //void contextMenuEventOnCategoryOccurs(const QString cat_name, const QPoint &global_mouse_pos);
    //void contextMenuEventOnObjectItemOccurs(const QString item_id, const QPoint &global_mouse_pos);

public slots:
    void filter(const QString &);

protected:
    void contextMenuEvent(QContextMenuEvent *e);
    void resizeEvent(QResizeEvent *e);

private slots:
    void slotSave();
    void slotScratchPadItemDeleted();
    void slotLastScratchPadItemDeleted();

    void handleMousePress(QTreeWidgetItem *item);
    void deleteScratchpad();
    void slotListMode();
    void slotIconMode();

private:
    CategoryListView *addCategoryView(QTreeWidgetItem *parent, bool iconMode);
    CategoryListView *categoryViewAt(int idx) const;
    CategoryListView *categoryView(const QString &cat_name) const;
    void adjustSubListSize(QTreeWidgetItem *cat_item);

    static bool readCategories(const QString &fileName, const QString &xml, CategoryList *cats, QString *errorMessage);
    static bool readObjectItem(ObjectItem *w, const QString &xml, QXmlStreamReader &r);

    CategoryList loadCustomCategoryList() const;
    void writeCategories(QXmlStreamWriter &writer, const CategoryList &cat_list) const;

    int indexOfCategory(const QString &cat_name) const;
    int indexOfScratchpad() const;
    int ensureScratchpad();
    void addCustomCategories(bool replace);

    void saveExpandedState() const;
    void restoreExpandedState();
    void updateViewMode();

private:
    ExpandListViewManager *m_core;
    QString m_file_name;
    typedef QHash<QString, QIcon> IconCache;
    mutable IconCache m_pluginIcons;
    bool m_iconMode;
    QTimer *m_scratchPadDeleteTimer;



};

}  // namespace HEHUI


#endif // TREEWIDGET_H
