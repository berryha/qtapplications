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

#include "categorylistview.h"
#include "shared/abstractexpandlistviewInterface.h"


#include <QtXml/QDomDocument>

#include <QtGui/QIcon>
#include <QtGui/QListView>
#include <QtGui/QLineEdit>
#include <QtGui/QItemDelegate>
#include <QtGui/QSortFilterProxyModel>

#include <QtCore/QAbstractListModel>
#include <QtCore/QList>
#include <QtCore/QTextStream>
#include <QtCore/QRegExp>
#include <QFocusEvent>
#include <QDebug>

static const char *widgetElementC = "widget";
static const char *nameAttributeC = "name";
static const char *uiOpeningTagC = "<ui>";
static const char *uiClosingTagC = "</ui>";

QT_BEGIN_NAMESPACE

enum { FilterRole = Qt::UserRole + 11 };

static QString domToString(const QDomElement &elt)
{
    QString result;
    QTextStream stream(&result, QIODevice::WriteOnly);
    elt.save(stream, 2);
    stream.flush();
    return result;
}

static QDomDocument stringToDom(const QString &xml)
{
    QDomDocument result;
    result.setContent(xml);
    return result;
}

namespace HEHUI {

// Entry of the model list

struct ExpandListViewCategoryEntry {
    ExpandListViewCategoryEntry();
    explicit ExpandListViewCategoryEntry(const ExpandListViewInterface::ObjectItem &widget, const QIcon &icon, bool editable);

    ExpandListViewInterface::ObjectItem objectItem;
    QString toolTip;
    QString whatsThis;
    QIcon icon;
    bool editable;
};


ExpandListViewCategoryEntry::ExpandListViewCategoryEntry() :
    editable(false)
{
}

ExpandListViewCategoryEntry::ExpandListViewCategoryEntry(const ExpandListViewInterface::ObjectItem &w, const QIcon &i, bool e) :
    objectItem(w),
    icon(i),
    editable(e)
{
}

/* ExpandListViewCategoryModel, representing a list of category entries. Uses a
 * QAbstractListModel since the behaviour depends on the view mode of the list
 * view, it does not return text in the case of IconMode. */

class ExpandListViewCategoryModel : public QAbstractListModel {
public:
    explicit ExpandListViewCategoryModel(/*ExpandListView *core,*/ QObject *parent = 0);

    // QAbstractListModel
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
    virtual Qt::ItemFlags flags (const QModelIndex & index ) const;
    virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

    // The model returns no text in icon mode, so, it also needs to know it
    QListView::ViewMode viewMode() const;
    void setViewMode(QListView::ViewMode vm);

    void addObjectItem(const ExpandListViewInterface::ObjectItem &objectItem, const QIcon &icon, bool editable);
    //void removeObjectItem(const QString &item_id);

    ExpandListViewInterface::ObjectItem objectItemAt(const QModelIndex & index) const;
    ExpandListViewInterface::ObjectItem objectItemAt(int row) const;
    ExpandListViewInterface::ObjectItem objectItem(const QString &id) const;

    int indexOfObjectItem(const QString &item_id);

    ExpandListViewInterface::Category category() const;
    bool removeCustomObjectItems();

    bool updateObjectItemName(const QString &item_id, const QString &name);
    bool updateObjectItemIcon(const QString &item_id, const QIcon &icon);
    bool updateObjectItemIcon(const QString &item_id, const QString &iconName);

private:
    typedef QList<ExpandListViewCategoryEntry> ExpandListViewCategoryEntries;

    //ExpandListView *m_core;
    ExpandListViewCategoryEntries m_categoryEntries;
    QListView::ViewMode m_viewMode;
};

ExpandListViewCategoryModel::ExpandListViewCategoryModel(/*ExpandListView *core,*/ QObject *parent) :
    QAbstractListModel(parent),
    //m_core(core),
    m_viewMode(QListView::ListMode)
{
}

QListView::ViewMode ExpandListViewCategoryModel::viewMode() const
{
    return m_viewMode;
}

void ExpandListViewCategoryModel::setViewMode(QListView::ViewMode vm)
{
    if (m_viewMode == vm)
        return;
    m_viewMode = vm;
    if (!m_categoryEntries.empty())
        reset();
}

int ExpandListViewCategoryModel::indexOfObjectItem(const QString &item_id)
{
    const int count = m_categoryEntries.size();
    for (int  i = 0; i < count; i++)
        if (m_categoryEntries.at(i).objectItem.id() == item_id)
            return i;
    return -1;
}

ExpandListViewInterface::Category ExpandListViewCategoryModel::category() const
{
    ExpandListViewInterface::Category rc;
    const ExpandListViewCategoryEntries::const_iterator cend = m_categoryEntries.constEnd();
    for (ExpandListViewCategoryEntries::const_iterator it = m_categoryEntries.constBegin(); it != cend; ++it)
        rc.addObjectItem(it->objectItem);
    return rc;
}

bool ExpandListViewCategoryModel::removeCustomObjectItems()
{
    // Typically, we are a whole category of custom widgets, so, remove all
    // and do reset.
    bool changed = false;
    for (ExpandListViewCategoryEntries::iterator it = m_categoryEntries.begin(); it != m_categoryEntries.end(); )
        if (it->objectItem.type() == ExpandListViewInterface::ObjectItem::Custom) {
            it = m_categoryEntries.erase(it);
            changed = true;
        } else {
            ++it;
        }
    if (changed)
        reset();
    return changed;
}

void ExpandListViewCategoryModel::addObjectItem(const ExpandListViewInterface::ObjectItem &objectItem, const QIcon &icon, bool editable)
{

    qDebug()<<"--ExpandListViewCategoryModel::addObjectItem(...)";

    // build item
    ExpandListViewCategoryEntry categoryEntry(objectItem, icon, editable);
    //const QDesignerWidgetDataBaseInterface *db = m_core->widgetDataBase();
    //const int dbIndex = db->indexOfClassName(objectItem.name());
    const int dbIndex = 1;

    if (dbIndex != -1) {
        //const QDesignerWidgetDataBaseItemInterface *dbItem = db->item(dbIndex);
        //const QString toolTip = dbItem->toolTip();
        const QString toolTip = objectItem.name();
        if (!toolTip.isEmpty())
            categoryEntry.toolTip = toolTip;
        //const QString whatsThis = dbItem->whatsThis();
        const QString whatsThis = objectItem.name();
        if (!whatsThis.isEmpty())
            categoryEntry.whatsThis = whatsThis;
    }
    // insert
    const int row = m_categoryEntries.size();
    beginInsertRows(QModelIndex(), row, row);
    m_categoryEntries.push_back(categoryEntry);
    endInsertRows();

}

//void ExpandListViewCategoryModel::removeObjectItem(const QString &item_id){
//    removeRow(indexOfObjectItem(item_id));
//}

QVariant ExpandListViewCategoryModel::data(const QModelIndex &index, int role) const
{
    const int row = index.row();
    if (row < 0 || row >=  m_categoryEntries.size())
        return QVariant();

    const ExpandListViewCategoryEntry &categoryEntry = m_categoryEntries.at(row);
    switch (role) {
    case Qt::DisplayRole:
        // No text in icon mode
        return QVariant(m_viewMode == QListView::ListMode ? categoryEntry.objectItem.name() : QString());
    case Qt::DecorationRole:
        return QVariant(categoryEntry.icon);
    case Qt::EditRole:
        return QVariant(categoryEntry.objectItem.name());
        //    case Qt::ToolTipRole: {
        //        if (m_viewMode == QListView::ListMode)
        //            return QVariant(categoryEntry.toolTip);
        //        // Icon mode tooltip should contain the  class name
        //        QString tt =  categoryEntry.objectItem.name();
        //        if (!categoryEntry.toolTip.isEmpty()) {
        //            tt += QLatin1Char('\n');
        //            tt += categoryEntry.toolTip;
        //        }
        //        return QVariant(tt);

        //    }
    case Qt::WhatsThisRole:
        return QVariant(categoryEntry.whatsThis);
    case FilterRole:
        return categoryEntry.objectItem.name();
    }
    return QVariant();
}

bool ExpandListViewCategoryModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    const int row = index.row();

    if ( row < 0 || row >=  m_categoryEntries.size() ){
        return false;
    }


    //if (role != Qt::EditRole || row < 0 || row >=  m_categoryEntries.size() || value.type() != QVariant::String)
    //   return false;




    ExpandListViewCategoryEntry &categoryEntry = m_categoryEntries[row];

    if(role == Qt::DisplayRole && value.type() == QVariant::String){
        const QString newName = value.toString();
        categoryEntry.objectItem.setName(newName);

        const QDomDocument doc = stringToDom(CategoryListView::objectItemDomXml(categoryEntry.objectItem));
        QDomElement widget_elt = doc.firstChildElement(QLatin1String(widgetElementC));
        if (!widget_elt.isNull()) {
            widget_elt.setAttribute(QLatin1String(nameAttributeC), newName);
            categoryEntry.objectItem.setDomXml(domToString(widget_elt));
        }
    }

    if(role == Qt::DecorationRole){
        if( value.type() == QVariant::Icon){
            categoryEntry.icon = value.value<QIcon>();
        }else if(value.type() == QVariant::String){
            const QString newIconName = value.toString();
            categoryEntry.objectItem.setIconName(newIconName);
            categoryEntry.icon = QIcon(newIconName);
        }


    }



    emit dataChanged(index, index);
    return true;
}

Qt::ItemFlags ExpandListViewCategoryModel::flags(const QModelIndex &index) const
{
    //Qt::ItemFlags rc = Qt::ItemIsEnabled;
    Qt::ItemFlags rc = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    const int row = index.row();
    if (row >= 0 && row <  m_categoryEntries.size())
        if (m_categoryEntries.at(row).editable) {
            rc |= Qt::ItemIsSelectable;
            // Can change name in list mode only
            if (m_viewMode == QListView::ListMode)
                rc |= Qt::ItemIsEditable;
        }
    return rc;
}

int ExpandListViewCategoryModel::rowCount(const QModelIndex & /*parent*/) const
{
    return m_categoryEntries.size();
}

bool ExpandListViewCategoryModel::removeRows(int row, int count, const QModelIndex & parent)
{
    if (row < 0 || count < 1)
        return false;
    const int size = m_categoryEntries.size();
    const int last =  row + count - 1;
    if (row >= size || last >= size)
        return false;
    beginRemoveRows(parent, row, last);
    for (int r = last; r >= row; r--)
        m_categoryEntries.removeAt(r);
    endRemoveRows();
    return true;
}

ExpandListViewInterface::ObjectItem ExpandListViewCategoryModel::objectItemAt(const QModelIndex & index) const
{
    return objectItemAt(index.row());
}

ExpandListViewInterface::ObjectItem ExpandListViewCategoryModel::objectItemAt(int row) const
{
    if (row < 0 || row >=  m_categoryEntries.size())
        return ExpandListViewInterface::ObjectItem();
    return m_categoryEntries.at(row).objectItem;
}

ExpandListViewInterface::ObjectItem ExpandListViewCategoryModel::objectItem(const QString &id) const{
    foreach(ExpandListViewCategoryEntry entry, m_categoryEntries){
        if(entry.objectItem.id() == id){
            return entry.objectItem;
        }
    }

    return ExpandListViewInterface::ObjectItem();

}

bool ExpandListViewCategoryModel::updateObjectItemName(const QString &item_id, const QString &name){
    qDebug()<<"----ExpandListViewCategoryModel::updateObjectItemIcon(const QString &item_id, const QIcon &icon)";

    QModelIndex idx = index(indexOfObjectItem(item_id));
    if(!idx.isValid()){
        return false;
    }

    setData(idx, name, Qt::DisplayRole);
    return true;

}

bool ExpandListViewCategoryModel::updateObjectItemIcon(const QString &item_id, const QIcon &icon){
    qDebug()<<"----ExpandListViewCategoryModel::updateObjectItemIcon(const QString &item_id, const QIcon &icon)";

    QModelIndex idx = index(indexOfObjectItem(item_id));
    if(!idx.isValid()){
        return false;
    }

    setData(idx, icon, Qt::DecorationRole);

    return true;

}

bool ExpandListViewCategoryModel::updateObjectItemIcon(const QString &item_id, const QString &iconName){
    qDebug()<<"----ExpandListViewCategoryModel::updateObjectItemIcon(const QString &item_id, const QString &iconName)";

    QModelIndex idx = index(indexOfObjectItem(item_id));
    if(!idx.isValid()){
        return false;
    }

    setData(idx, iconName, Qt::DecorationRole);

    return true;

}






/* WidgetSubBoxItemDelegate, ensures a valid name using a regexp validator */

class ExpandListViewCategoryEntryDelegate : public QItemDelegate
{
public:
    explicit ExpandListViewCategoryEntryDelegate(QWidget *parent = 0) : QItemDelegate(parent) {}
    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;
};

QWidget *ExpandListViewCategoryEntryDelegate::createEditor(QWidget *parent,
                                                           const QStyleOptionViewItem &option,
                                                           const QModelIndex &index) const
{
    QWidget *result = QItemDelegate::createEditor(parent, option, index);
    if (QLineEdit *line_edit = qobject_cast<QLineEdit*>(result)) {
        const QRegExp re = QRegExp(QLatin1String("[_a-zA-Z][_a-zA-Z0-9]*"));
        Q_ASSERT(re.isValid());
        line_edit->setValidator(new QRegExpValidator(re, line_edit));
    }
    qDebug()<<"WidgetBoxCategoryEntryDelegate::createEditor(...)~~~~";

    return result;
}

// ----------------------  CategoryListView

CategoryListView *CategoryListView::lastFocusedCategoryListView = 0;

CategoryListView::CategoryListView(ExpandListViewManager *core, QWidget *parent)
    :QListView(parent), m_core(core),
      m_proxyModel(new QSortFilterProxyModel(this)),
      m_model(new ExpandListViewCategoryModel(/*core, */this))
{
    //setFocusPolicy(Qt::NoFocus);
    setFrameShape(QFrame::NoFrame);
    setIconSize(QSize(22, 22));
    setSpacing(1);
    setTextElideMode(Qt::ElideMiddle);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setResizeMode(QListView::Adjust);
    setUniformItemSizes(true);
    //    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    //setItemDelegate(new ExpandListViewCategoryEntryDelegate(this));
    //setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);

    connect(this, SIGNAL(pressed(QModelIndex)), this, SLOT(slotPressed(QModelIndex)));
    //connect(this, SIGNAL(activated(QModelIndex)), m_core, SLOT(slotItemActivated(QModelIndex)));
    connect(this, SIGNAL(activated(QModelIndex)), this, SLOT(slotItemActivated(QModelIndex)));
    //connect(this, SIGNAL(doubleClicked(QModelIndex)), m_core, SLOT(slotItemDoubleClicked(QModelIndex)));
    //connect(this, SIGNAL(entered(QModelIndex)), m_core, SLOT(slotItemEntered(QModelIndex)));



    m_proxyModel->setSourceModel(m_model);
    m_proxyModel->setFilterRole(FilterRole);
    setModel(m_proxyModel);
    connect(m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SIGNAL(scratchPadChanged()));


}

bool CategoryListView::event(QEvent *event){

    switch(event->type()){
    case QEvent::FocusIn:
    {
        if(lastFocusedCategoryListView != this){
            if(lastFocusedCategoryListView){
                lastFocusedCategoryListView->clearSelection();
            }
            lastFocusedCategoryListView = this;
        }

    }
        break;
        //    case QEvent::FocusOut:
        //    {
        //        QPoint cursorpos = QCursor::pos();
        //        QPoint topleft = mapToGlobal(pos());
        //        if(cursorpos.x() < topleft.x() || cursorpos.x() > (topleft.x() + width()) || cursorpos.y() < topleft.y() || cursorpos.y() > (topleft.y() + height()) ){
        //            clearSelection();
        //        }

        //        qDebug()<<"--QEvent::FocusOut";
        //        //return true;
        //    }
        //    break;
    case QEvent::ContextMenu:
    {
        QContextMenuEvent *e = static_cast<QContextMenuEvent *> (event);
        if(!e){return false;}

        QString itemID = "";
        QModelIndex index = indexAt(e->pos());
        ExpandListViewInterface::ObjectItem item = objectItemAt(UnfilteredAccess, index);
        if(!item.isNull()){
            itemID = item.id();
        }

        m_core->slotContextMenuEventOnObjectItemOccurs(itemID, mapToGlobal(e->pos()), 0);

        return true;

    }
        break;
    case QEvent::ToolTip:
    {
        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
        if(!helpEvent){return false;}

        QString itemID = "";
        QModelIndex index = indexAt(helpEvent->pos());
        ExpandListViewInterface::ObjectItem item = objectItemAt(UnfilteredAccess, index);
        if(item.isNull()){
            return false;
        }
        itemID = item.id();

        QRect rect = visualRect(index);
        QPoint globalvisualRectTopLeft = mapToGlobal(rect.topLeft());
        QPoint globalMousePos = helpEvent->globalPos();

        QSize iconsize = iconSize();
        if(globalMousePos.x() < globalvisualRectTopLeft.x() || globalMousePos.x() > (globalvisualRectTopLeft.x() + iconsize.width()) || globalMousePos.y() < globalvisualRectTopLeft.y() || globalMousePos.y() > (globalvisualRectTopLeft.y() + iconsize.height()) ){
            //m_core->slotTooltipEventOnObjectItemOccurs("", globalvisualRectTopLeft, globalMousePos);
            return false;
        }

        //m_core->slotTooltipEventOnObjectItemOccurs(itemID, helpEvent->globalPos());
        m_core->slotTooltipEventOnObjectItemOccurs(itemID, globalvisualRectTopLeft, globalMousePos);

        return true;
    }
        break;
    case QEvent::Resize:
    {
        QResizeEvent *e = static_cast<QResizeEvent *>(event);
        QListView::resizeEvent(e);
        updateGeometries();
    }

        break;
    default:
        break;

    }


    return QListView::event(event);

}

//void CategoryListView::focusOutEvent ( QFocusEvent * event ){
//	clearSelection();
//	event->accept();
//}

//void CategoryListView::contextMenuEvent(QContextMenuEvent *e)
//{
//    QString itemID = "";
//    QModelIndex index = indexAt(e->pos());
//    ExpandListViewInterface::ObjectItem item = objectItemAt(UnfilteredAccess, index);
//    if(!item.isNull()){
//        itemID = item.id();
//    }

//    e->accept();
//    //emit contextMenuEventOnObjectItemOccurs(itemID, mapToGlobal(e->pos()));
//    m_core->slotContextMenuEventOnObjectItemOccurs(itemID, mapToGlobal(e->pos()), 0);

//}



void CategoryListView::setViewMode(ViewMode vm)
{
    QListView::setViewMode(vm);
    m_model->setViewMode(vm);
}

void CategoryListView::setCurrentItem(AccessMode am, int row)
{
    const QModelIndex index = am == FilteredAccess ?
                m_proxyModel->index(row, 0) :
                m_proxyModel->mapFromSource(m_model->index(row, 0));

    if (index.isValid())
        setCurrentIndex(index);
}

void CategoryListView::slotItemActivated(const QModelIndex &index)
{
    /*const*/ ExpandListViewInterface::ObjectItem wgt = m_model->objectItemAt(m_proxyModel->mapToSource(index));
    if (wgt.isNull())
        return;

    QString id = wgt.id();
    m_core->slotContactItemActivated(id);
}

void CategoryListView::slotPressed(const QModelIndex &index)
{

    //TODO
    m_core->slotTooltipEventOnObjectItemOccurs("", QPoint(0,0), QPoint(0,0));

    const ExpandListViewInterface::ObjectItem item = m_model->objectItemAt(m_proxyModel->mapToSource(index));
    if (item.isNull())
        return;
    emit pressed(item.name(), QCursor::pos());
}

void CategoryListView::removeCurrentItem()
{
    const QModelIndex index = currentIndex();
    if (!index.isValid() || !m_proxyModel->removeRow(index.row()))
        return;

    // We check the unfiltered item count here, we don't want to get removed if the
    // filtered view is empty
    if (m_model->rowCount()) {
        emit itemRemoved();
    } else {
        emit lastItemRemoved();
    }
}

void CategoryListView::editCurrentItem()
{
    const QModelIndex index = currentIndex();
    if (index.isValid())
        edit(index);
}

int CategoryListView::count(AccessMode am) const
{
    return am == FilteredAccess ? m_proxyModel->rowCount() : m_model->rowCount();
}

int CategoryListView::mapRowToSource(int filterRow) const
{
    const QModelIndex filterIndex = m_proxyModel->index(filterRow, 0);
    return m_proxyModel->mapToSource(filterIndex).row();
}

ExpandListViewInterface::ObjectItem CategoryListView::objectItemAt(AccessMode am, const QModelIndex & index) const
{
    const QModelIndex unfilteredIndex = am == FilteredAccess ? m_proxyModel->mapToSource(index) : index;
    return m_model->objectItemAt(unfilteredIndex);
}

ExpandListViewInterface::ObjectItem CategoryListView::objectItemAt(AccessMode am, int row) const
{
    return m_model->objectItemAt(am == UnfilteredAccess ? row : mapRowToSource(row));
}


ExpandListViewInterface::ObjectItem CategoryListView::objectItem(const QString &objectItemID){
    ExpandListViewInterface::ObjectItem item = m_model->objectItem(objectItemID);
    if (item.isNull()){
        return ExpandListViewInterface::ObjectItem();
    }

    //    ExpandListViewInterface::ObjectItem *i = new ExpandListViewInterface::ObjectItem();
    //    i = &item;
    //    return i;

    return item;
}

int CategoryListView::indexOfObjectItem(const QString &item_id){
    return m_model->indexOfObjectItem(item_id);
}

void CategoryListView::removeRow(AccessMode am, int row)
{
    m_model->removeRow(am == UnfilteredAccess ? row : mapRowToSource(row));
    viewport()->update();

}

bool CategoryListView::containsObjectItem(const QString &item_id)
{
    return m_model->indexOfObjectItem(item_id) != -1;
}

void CategoryListView::addObjectItem(const ExpandListViewInterface::ObjectItem &item, const QIcon &icon, bool editable)
{
    m_model->addObjectItem(item, icon, editable);
    viewport()->update();

}

//void CategoryListView::removeObjectItem(const QString &item_id){
//    m_model->removeObjectItem(item_id);
//}

QString CategoryListView::objectItemDomXml(const ExpandListViewInterface::ObjectItem &item)
{
    QString domXml = item.domXml();

    if (domXml.isEmpty()) {
        domXml = QLatin1String(uiOpeningTagC);
        domXml += QLatin1String("<widget class=\"");
        domXml += item.name();
        domXml += QLatin1String("\"/>");
        domXml += QLatin1String(uiClosingTagC);
    }
    return domXml;
}

void CategoryListView::filter(const QRegExp &re)
{
    m_proxyModel->setFilterRegExp(re);
}

ExpandListViewInterface::Category CategoryListView::category() const
{
    return m_model->category();
}

bool CategoryListView::removeCustomObjectItems()
{
    return m_model->removeCustomObjectItems();
}

bool CategoryListView::updateObjectItemName(const QString &item_id, const QString &name){
    qDebug()<<"----CategoryListView::updateObjectItemName(const QString &item_id, const QIcon &icon)";

    bool ok = m_model->updateObjectItemName(item_id, name);
    viewport()->update();
    return ok;

}

bool CategoryListView::updateObjectItemIcon(const QString &item_id, const QIcon &icon){
    qDebug()<<"----CategoryListView::updateObjectItemIcon(const QString &item_id, const QIcon &icon)";

    bool ok =  m_model->updateObjectItemIcon(item_id, icon);
    viewport()->update();
    return ok;

}

bool CategoryListView::updateObjectItemIcon(const QString &item_id, const QString &iconName){
    qDebug()<<"----CategoryListView::updateObjectItemIcon(const QString &item_id, const QString &iconName)";

    bool ok = m_model->updateObjectItemIcon(item_id, iconName);
    viewport()->update();
    return ok;

}




}  // namespace HEHUI

QT_END_NAMESPACE
