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

#include "treewidget.h"
#include "shared/abstractexpandlistviewInterface.h"
#include "categorylistview.h"

// shared
#include "./shared/iconloader_p.h"
#include "./shared/sheet_delegate_p.h"

//#include <ui4_p.h>
//#include <qdesigner_utils_p.h>
//#include <pluginmanager_p.h>




#include <QtGui/QHeaderView>
#include <QtGui/QApplication>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QAction>
#include <QtGui/QActionGroup>
#include <QtGui/QMenu>

#include <QtCore/QFile>
#include <QtCore/QTimer>
#include <QtCore/QDebug>
#include <QSettings>

static const char *widgetBoxRootElementC = "widgetbox";
static const char *widgetElementC = "widget";
static const char *uiElementC = "ui";
static const char *categoryElementC = "category";
static const char *categoryEntryElementC = "categoryentry";
static const char *idAttributeC = "id";
static const char *nameAttributeC = "name";
static const char *typeAttributeC = "type";
static const char *iconAttributeC = "icon";
static const char *defaultTypeValueC = "default";
static const char *customValueC = "custom";
static const char *iconPrefixC = "__def_icon__";
static const char *scratchPadValueC = "scratchpad";
static const char *defaultIconC = ":/resources/images/default_icon.png";
static const char *invisibleNameC = "[invisible]";

enum TopLevelRole  { NORMAL_ITEM, SCRATCHPAD_ITEM, CUSTOM_ITEM };

QT_BEGIN_NAMESPACE

static void setTopLevelRole(TopLevelRole tlr, QTreeWidgetItem *item)
{
    item->setData(0, Qt::UserRole, QVariant(tlr));
}

static TopLevelRole topLevelRole(const  QTreeWidgetItem *item)
{
    return static_cast<TopLevelRole>(item->data(0, Qt::UserRole).toInt());
}

namespace HEHUI {

TreeWidget::TreeWidget(ExpandListViewManager *core, QWidget *parent) :
    QTreeWidget(parent),
    m_core(core),
    m_iconMode(false),
    m_scratchPadDeleteTimer(0)
{
    setFocusPolicy(Qt::NoFocus);
    setIndentation(0);
    setRootIsDecorated(false);
    setColumnCount(1);
    header()->hide();
    header()->setResizeMode(QHeaderView::Stretch);
    setTextElideMode(Qt::ElideMiddle);
    setVerticalScrollMode(ScrollPerPixel);

    setItemDelegate(new SheetDelegate(this, this));

    //connect(this, SIGNAL(itemPressed(QTreeWidgetItem*,int)), this, SLOT(handleMousePress(QTreeWidgetItem*)));
}

QIcon TreeWidget::iconForObjectItem(QString iconName, QIcon::Mode iconMode) const
{
    if (iconName.isEmpty())
        iconName = QLatin1String(defaultIconC);

    if (iconName.startsWith(QLatin1String(iconPrefixC))) {
        const IconCache::const_iterator it = m_pluginIcons.constFind(iconName);
        if (it != m_pluginIcons.constEnd())
            return it.value();
    }
    return createIconSet(iconName,"",iconMode);
}

CategoryListView *TreeWidget::categoryViewAt(int idx) const
{
    CategoryListView *rc = 0;
    if (QTreeWidgetItem *cat_item = topLevelItem(idx))
        if (QTreeWidgetItem *embedItem = cat_item->child(0))
            rc = qobject_cast<CategoryListView*>(itemWidget(embedItem, 0));
    Q_ASSERT(rc);
    return rc;
}

CategoryListView *TreeWidget::categoryView(const QString &cat_name) const{
    qDebug()<<"----TreeWidget::categoryView(...)~~cat_name:"<<cat_name;

    CategoryListView *rc = 0;
    if(cat_name.isEmpty()){
        qCritical()<<"Empty cat_name!";
        return rc;
    }

    int topItemCount = topLevelItemCount();
    for (int i = 0;  i< topItemCount; i++) {
        QTreeWidgetItem *cat_item = topLevelItem(i);
        if(cat_item && cat_item->text(0) == cat_name){
            if(QTreeWidgetItem *embedItem = cat_item->child(0)){
                rc = qobject_cast<CategoryListView*>(itemWidget(embedItem, 0));
                qDebug()<<"----TreeWidget::categoryView(...)~~cat_item->text(0):"<<cat_item->text(0);
                break;
            }

        }
    }

    Q_ASSERT(rc);

    return rc;

}

void TreeWidget::saveExpandedState() const
{
    QStringList closedCategories;
    if (const int numCategories = categoryCount()) {
        for (int i = 0; i < numCategories; ++i) {
            const QTreeWidgetItem *cat_item = topLevelItem(i);
            if (!isItemExpanded(cat_item))
                closedCategories.append(cat_item->text(0));
        }
    }
    //QDesignerSettingsInterface *settings = m_core->settingsManager();
    QSettings *settings = new QSettings();
    settings->beginGroup(QLatin1String(widgetBoxRootElementC));
    settings->setValue(QLatin1String("Closed categories"), closedCategories);
    settings->setValue(QLatin1String("View mode"), m_iconMode);
    settings->endGroup();
}

void  TreeWidget::restoreExpandedState()
{
    typedef QSet<QString> StringSet;
    //QDesignerSettingsInterface *settings = m_core->settingsManager();
    QSettings *settings = new QSettings();
    m_iconMode = settings->value(QLatin1String("WidgetBox/View mode")).toBool();
    updateViewMode();
    const StringSet closedCategories = settings->value(QLatin1String("WidgetBox/Closed categories"), QStringList()).toStringList().toSet();
    expandAll();
    if (closedCategories.empty())
        return;

    if (const int numCategories = categoryCount()) {
        for (int i = 0; i < numCategories; ++i) {
            QTreeWidgetItem *item = topLevelItem(i);
            if (closedCategories.contains(item->text(0)))
                item->setExpanded(false);
            }
    }
}

TreeWidget::~TreeWidget()
{
    saveExpandedState();
}

void TreeWidget::setFileName(const QString &file_name)
{
    m_file_name = file_name;
}

QString TreeWidget::fileName() const
{
    return m_file_name;
}

bool TreeWidget::save()
{
    if (fileName().isEmpty())
        return false;

    QFile file(fileName());
    if (!file.open(QIODevice::WriteOnly))
        return false;

    CategoryList cat_list;
    const int count = categoryCount();
    for (int i = 0; i < count; ++i)
        cat_list.append(category(i));

    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.setAutoFormattingIndent(1);
    writer.writeStartDocument();
    writeCategories(writer, cat_list);
    writer.writeEndDocument();

    return true;
}

void TreeWidget::slotSave()
{
    save();
}

void TreeWidget::handleMousePress(QTreeWidgetItem *item)
{
    if (item == 0)
        return;

    if (QApplication::mouseButtons() != Qt::LeftButton)
        return;

    if (item->parent() == 0) {
        setItemExpanded(item, !isItemExpanded(item));
        return;
    }
}

int TreeWidget::ensureScratchpad()
{
    const int existingIndex = indexOfScratchpad();
    if (existingIndex != -1)
         return existingIndex;

    QTreeWidgetItem *scratch_item = new QTreeWidgetItem(this);
    scratch_item->setText(0, tr("Scratchpad"));
    setTopLevelRole(SCRATCHPAD_ITEM, scratch_item);
    addCategoryView(scratch_item, false); // Scratchpad in list mode.
    return categoryCount() - 1;
}

CategoryListView *TreeWidget::addCategoryView(QTreeWidgetItem *parent, bool iconMode)
{
    QTreeWidgetItem *embed_item = new QTreeWidgetItem(parent);
    embed_item->setFlags(Qt::ItemIsEnabled);

    CategoryListView *categoryView = new CategoryListView(m_core, this);
    categoryView->setViewMode(iconMode ? QListView::IconMode : QListView::ListMode);
    categoryView->setResizeMode(QListView::Adjust);
    categoryView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    categoryView->setFlow(QListView::TopToBottom);

    connect(categoryView, SIGNAL(scratchPadChanged()), this, SLOT(slotSave()));
    connect(categoryView, SIGNAL(pressed(const QString &, const QPoint &)), this, SIGNAL(pressed(const QString &, const QPoint &)));
    //connect(categoryView, SIGNAL(contextMenuEventOnObjectItemOccurs(QString,QPoint)), this, SIGNAL(contextMenuEventOnObjectItemOccurs(QString,QPoint)));
    connect(categoryView, SIGNAL(itemRemoved()), this, SLOT(slotScratchPadItemDeleted()));
    connect(categoryView, SIGNAL(lastItemRemoved()), this, SLOT(slotLastScratchPadItemDeleted()));

    setItemWidget(embed_item, 0, categoryView);
    return categoryView;
}

int TreeWidget::indexOfScratchpad() const
{
    if (const int numTopLevels =  topLevelItemCount()) {
        for (int i = numTopLevels - 1; i >= 0; --i) {
            if (topLevelRole(topLevelItem(i)) == SCRATCHPAD_ITEM)
                return i;
        }
    }
    return -1;
}

int TreeWidget::indexOfCategory(const QString &cat_name) const
{
    const int topLevelCount = topLevelItemCount();
    for (int i = 0; i < topLevelCount; ++i) {
        if (topLevelItem(i)->text(0) == cat_name)
            return i;
    }
    return -1;
}

bool TreeWidget::load(ExpandListViewBase::LoadMode loadMode)
{
    switch (loadMode) {
    case ExpandListViewBase::LoadReplace:
        clear();
        break;
    case ExpandListViewBase::LoadCustomWidgetsOnly:
        addCustomCategories(true);
        updateGeometries();
        return true;
    default:
        break;
    }

    const QString name = fileName();

    QFile f(name);
    if (!f.open(QIODevice::ReadOnly)) // Might not exist at first startup
        return false;

    const QString contents = QString::fromUtf8(f.readAll());
    return loadContents(contents);
}

bool TreeWidget::load(CategoryList *cat_list)
{

	if(!cat_list->size()){
		return false;
	}


    foreach(const Category &cat, *cat_list)
        addCategory(cat);

    addCustomCategories(false);
    // Restore which items are expanded
    restoreExpandedState();
    return true;
}

bool TreeWidget::load(Category *cat)
{

        addCategory(*cat);
    //restoreExpandedState();
    return true;
}

bool TreeWidget::loadContents(const QString &contents)
{
    QString errorMessage;
    CategoryList cat_list;
    if (!readCategories(m_file_name, contents, &cat_list, &errorMessage)) {
        qWarning()<<errorMessage;
        return false;
    }

    foreach(const Category &cat, cat_list)
        addCategory(cat);

    addCustomCategories(false);
    // Restore which items are expanded
    restoreExpandedState();
    return true;
}

void TreeWidget::addCustomCategories(bool replace)
{
    if (replace) {
        // clear out all existing custom widgets
        if (const int numTopLevels =  topLevelItemCount()) {
            for (int t = 0; t < numTopLevels ; ++t)
                categoryViewAt(t)->removeCustomObjectItems();
        }
    }
    // re-add
    const CategoryList customList = loadCustomCategoryList();
    const CategoryList::const_iterator cend = customList.constEnd();
    for (CategoryList::const_iterator it = customList.constBegin(); it != cend; ++it)
        addCategory(*it);
}

static inline QString msgXmlError(const QString &fileName, const QXmlStreamReader &r)
{
    return ExpandListViewBase::tr("An error has been encountered at line %1 of %2: %3")
            .arg(r.lineNumber()).arg(fileName, r.errorString());
}

bool TreeWidget::readCategories(const QString &fileName, const QString &contents,
                                       CategoryList *cats, QString *errorMessage)
{
    // Read objectItem box XML:
    //
    //<widgetbox version="4.5">
    // <category name="Layouts">
    //  <categoryentry name="Vertical Layout" icon="win/editvlayout.png" type="default">
    //   <objectItem class="QListWidget" ...>
    // ...

    QXmlStreamReader reader(contents);


    // Entries of category with name="invisible" should be ignored
    bool ignoreEntries = false;

    while (!reader.atEnd()) {
        switch (reader.readNext()) {
        case QXmlStreamReader::StartElement: {
            const QStringRef tag = reader.name();
            if (tag == QLatin1String(widgetBoxRootElementC)) {
                //<widgetbox version="4.5">
                continue;
            }
            if (tag == QLatin1String(categoryElementC)) {
                // <category name="Layouts">
                const QXmlStreamAttributes attributes = reader.attributes();
                const QString categoryID = attributes.value(QLatin1String(idAttributeC)).toString();
                const QString categoryName = attributes.value(QLatin1String(nameAttributeC)).toString();
                if (categoryName == QLatin1String(invisibleNameC)) {
                    ignoreEntries = true;
                } else {
                    Category category(categoryID, categoryName);
                    if (attributes.value(QLatin1String(typeAttributeC)) == QLatin1String(scratchPadValueC))
                        category.setType(Category::Scratchpad);
                    cats->push_back(category);
                }
                continue;
            }
            if (tag == QLatin1String(categoryEntryElementC)) {
                //  <categoryentry name="Vertical Layout" icon="win/editvlayout.png" type="default">
                if (!ignoreEntries) {
                    QXmlStreamAttributes attr = reader.attributes();
                    const QString widgetID = attr.value(QLatin1String(idAttributeC)).toString();
                    const QString widgetName = attr.value(QLatin1String(nameAttributeC)).toString();
                    const QString widgetIcon = attr.value(QLatin1String(iconAttributeC)).toString();
                    const TreeWidget::ObjectItem::Type widgetType =
                        attr.value(QLatin1String(typeAttributeC)).toString()
                            == QLatin1String(customValueC) ?
                        TreeWidget::ObjectItem::Custom :
                        TreeWidget::ObjectItem::Default;

                    ObjectItem w;
                    w.setID(widgetID);
                    w.setName(widgetName);
                    w.setIconName(widgetIcon);
                    w.setCategoryID(cats->back().id());
                    w.setType(widgetType);

                    //ContactBase contactBase(widgetID, "", widgetIcon, "");
                    //w.setContactBase(contactBase);


                    //TODO:???
                    //if (!readObjectItem(&w, contents, reader))
                    //    continue;
                    readObjectItem(&w, contents, reader);

                    cats->back().addObjectItem(w);
                } // ignoreEntries
                continue;
            }
            break;
        }
        case QXmlStreamReader::EndElement: {
           const QStringRef tag = reader.name();
           if (tag == QLatin1String(widgetBoxRootElementC)) {
               continue;
           }
           if (tag == QLatin1String(categoryElementC)) {
               ignoreEntries = false;
               continue;
           }
           if (tag == QLatin1String(categoryEntryElementC)) {
               continue;
           }
           break;
        }
        default: break;
        }
    }

    if (reader.hasError()) {
        *errorMessage = msgXmlError(fileName, reader);
        return false;
    }

    return true;
}

/*!
 * Read out a objectItem within a category. This can either be
 * enclosed in a <ui> element or a (legacy) <objectItem> element which may
 * contain nested <objectItem> elements.
 *
 * Examples:
 *
 * <ui language="c++">
 *  <objectItem class="MultiPageWidget" name="multipagewidget"> ... </objectItem>
 *  <customwidgets>...</customwidgets>
 * <ui>
 *
 * or
 *
 * <objectItem>
 *   <objectItem> ... </objectItem>
 *   ...
 * <objectItem>
 *
 * Returns true on success, false if end was reached or an error has been encountered
 * in which case the reader has its error flag set. If successful, the current item
 * of the reader will be the closing element (</ui> or </objectItem>)
 */
bool TreeWidget::readObjectItem(ObjectItem *w, const QString &xml, QXmlStreamReader &r)
{
    qint64 startTagPosition =0, endTagPosition = 0;

    int nesting = 0;
    bool endEncountered = false;
    bool parsedWidgetTag = false;
    QString outmostElement;
    while (!endEncountered) {
        const qint64 currentPosition = r.characterOffset();
        switch(r.readNext()) {
        case QXmlStreamReader::StartElement:
            if (nesting++ == 0) {
                // First element must be <ui> or (legacy) <objectItem>
                const QStringRef name = r.name();
                if (name == QLatin1String(uiElementC)) {
                    startTagPosition = currentPosition;
                } else {
                    if (name == QLatin1String(widgetElementC)) {
                        startTagPosition = currentPosition;
                        parsedWidgetTag = true;
                    } else {
                        r.raiseError(ExpandListViewBase::tr("Unexpected element <%1> encountered when parsing for <widget> or <ui>").arg(name.toString()));
                        return false;
                    }
                }
            } else {
                // We are within <ui> looking for the first <objectItem> tag
                if (!parsedWidgetTag && r.name() == QLatin1String(widgetElementC)) {
                    parsedWidgetTag = true;
                }
            }
            break;
        case QXmlStreamReader::EndElement:
            // Reached end of objectItem?
            if (--nesting == 0)  {
                endTagPosition = r.characterOffset();
                endEncountered = true;
            }
            break;
        case QXmlStreamReader::EndDocument:
            r.raiseError(ExpandListViewBase::tr("Unexpected end of file encountered when parsing widgets."));
            return false;
        case QXmlStreamReader::Invalid:
            return false;
        default:
            break;
        }
    }
    if (!parsedWidgetTag) {
        r.raiseError(ExpandListViewBase::tr("A widget element could not be found."));
        return false;
    }
    // Oddity: Startposition is 1 off
    QString widgetXml = xml.mid(startTagPosition, endTagPosition - startTagPosition);
    const QChar lessThan = QLatin1Char('<');
    if (!widgetXml.startsWith(lessThan))
        widgetXml.prepend(lessThan);
    w->setDomXml(widgetXml);
    return true;
}

void TreeWidget::writeCategories(QXmlStreamWriter &writer, const CategoryList &cat_list) const
{
    const QString widgetbox = QLatin1String(widgetBoxRootElementC);
    const QString id = QLatin1String(idAttributeC);
    const QString name = QLatin1String(nameAttributeC);
    const QString type = QLatin1String(typeAttributeC);
    const QString icon = QLatin1String(iconAttributeC);
    const QString defaultType = QLatin1String(defaultTypeValueC);
    const QString category = QLatin1String(categoryElementC);
    const QString categoryEntry = QLatin1String(categoryEntryElementC);
    const QString iconPrefix = QLatin1String(iconPrefixC);
    const QString widgetTag = QLatin1String(widgetElementC);

    //
    // <widgetbox>
    //   <category name="Layouts">
    //     <categoryEntry name="Vertical Layout" type="default" icon="win/editvlayout.png">
    //       <ui>
    //        ...
    //       </ui>
    //     </categoryEntry>
    //     ...
    //   </category>
    //   ...
    // </widgetbox>
    //

    writer.writeStartElement(widgetbox);

    foreach (const Category &cat, cat_list) {
        writer.writeStartElement(category);
        writer.writeAttribute(id, cat.id());
        writer.writeAttribute(name, cat.name());
        if (cat.type() == Category::Scratchpad)
            writer.writeAttribute(type, QLatin1String(scratchPadValueC));

        const int widgetCount = cat.objectItemCount();
        for (int i = 0; i < widgetCount; ++i) {
           const  ObjectItem wgt = cat.objectItem(i);
            if (wgt.type() == ObjectItem::Custom)
                continue;

            writer.writeStartElement(categoryEntry);
            writer.writeAttribute(id, wgt.id());
            writer.writeAttribute(name, wgt.name());
            if (!wgt.iconName().startsWith(iconPrefix))
                writer.writeAttribute(icon, wgt.iconName());
            writer.writeAttribute(type, defaultType);

            //const DomUI *domUI = ExpandListViewBase::xmlToUi(wgt.name(), CategoryListView::objectItemDomXml(wgt), false);
            //if (domUI) {
            //    domUI->write(writer);
            //    delete domUI;
								//}

            writer.writeEndElement(); // categoryEntry
        }
        writer.writeEndElement(); // categoryEntry
    }

    writer.writeEndElement(); // widgetBox
}

int TreeWidget::findCategory(const QString &name, const TreeWidget::CategoryList &list)
{
    int idx = 0;
    foreach (const TreeWidget::Category &cat, list) {
        if (cat.name() == name)
            return idx;
        ++idx;
    }
    return -1;
}

static inline bool isValidIcon(const QIcon &icon)
{
    if (!icon.isNull()) {
        const QList<QSize> availableSizes = icon.availableSizes();
        if (!availableSizes.empty())
            return !availableSizes.front().isEmpty();
    }
    return false;
}

TreeWidget::CategoryList TreeWidget::loadCustomCategoryList() const
{
    CategoryList result;
/*

    const QDesignerPluginManager *pm = m_core->pluginManager();
    const QDesignerPluginManager::CustomWidgetList customWidgets = pm->registeredCustomWidgets();
    if (customWidgets.empty())
        return result;

    static const QString customCatName = tr("Custom Widgets");

    const QString invisible = QLatin1String(invisibleNameC);
    const QString iconPrefix = QLatin1String(iconPrefixC);

    foreach(QDesignerCustomWidgetInterface *c, customWidgets) {
        const QString dom_xml = c->domXml();
        if (dom_xml.isEmpty())
            continue;

        const QString pluginName = c->name();
        const QDesignerCustomWidgetData data = pm->customWidgetData(c);
        QString displayName = data.xmlDisplayName();
        if (displayName.isEmpty())
            displayName = pluginName;

        QString cat_name = c->group();
        if (cat_name.isEmpty())
            cat_name = customCatName;
        else if (cat_name == invisible)
            continue;

        int idx = findCategory(cat_name, result);
        if (idx == -1) {
            result.append(Category(cat_name));
            idx = result.size() - 1;
        }
        Category &cat = result[idx];

        const QIcon icon = c->icon();

        QString icon_name;
        if (isValidIcon(icon)) {
            icon_name = iconPrefix;
            icon_name += pluginName;
            m_pluginIcons.insert(icon_name, icon);
        } else {
            icon_name = QLatin1String(qtLogoC);
        }

        cat.addObjectItem(ObjectItem(displayName, dom_xml, icon_name, ObjectItem::Custom));
    }
*/

    return result;
}

void TreeWidget::adjustSubListSize(QTreeWidgetItem *cat_item)
{
    qDebug()<<"--TreeWidget::adjustSubListSize(...)";


    QTreeWidgetItem *embedItem = cat_item->child(0);
    CategoryListView *list_widget = static_cast<CategoryListView*>(itemWidget(embedItem, 0));
    list_widget->setFixedWidth(header()->width());
    //qDebug()<<"------header()->width():"<<header()->width();
    list_widget->doItemsLayout();
    const int height = qMax(list_widget->contentsSize().height(), 1);
    list_widget->setFixedHeight(height);
    list_widget->setFlow(QListView::TopToBottom);


    embedItem->setSizeHint(0, QSize(-1, height - 1));


}

int TreeWidget::categoryCount() const
{
    return topLevelItemCount();
}

TreeWidget::Category TreeWidget::category(int cat_idx) const
{
    if (cat_idx >= topLevelItemCount())
        return Category();

    QTreeWidgetItem *cat_item = topLevelItem(cat_idx);

    QTreeWidgetItem *embedItem = cat_item->child(0);
    CategoryListView *categoryView = static_cast<CategoryListView*>(itemWidget(embedItem, 0));

    Category result = categoryView->category();
    result.setName(cat_item->text(0));

    switch (topLevelRole(cat_item)) {
    case SCRATCHPAD_ITEM:
        result.setType(Category::Scratchpad);
        break;
    default:
        result.setType(Category::Default);
        break;
    }
    return result;
}

TreeWidget::Category TreeWidget::category(const QString &cat_name) const{
    return category(indexOfCategory(cat_name));

}


void TreeWidget::addCategory(const Category &cat)
{


//    if (cat.objectItemCount() == 0){
//        return;
//    }


    const bool isScratchPad = cat.type() == Category::Scratchpad;
    CategoryListView *categoryView;
    QTreeWidgetItem *cat_item;

    if (isScratchPad) {
        const int idx = ensureScratchpad();
        categoryView = categoryViewAt(idx);
        cat_item = topLevelItem(idx);
    } else {

        const int existingIndex = indexOfCategory(cat.id());
        if (existingIndex == -1) {
            cat_item = new QTreeWidgetItem();
            cat_item->setText(0, cat.name());
            setTopLevelRole(NORMAL_ITEM, cat_item);
            // insert before scratchpad
            const int scratchPadIndex = indexOfScratchpad();
            if (scratchPadIndex == -1) {
                addTopLevelItem(cat_item);
            } else {
                insertTopLevelItem(scratchPadIndex, cat_item);
            }
            setItemExpanded(cat_item, true);
            categoryView = addCategoryView(cat_item, m_iconMode);
        } else {
            categoryView = categoryViewAt(existingIndex);
            cat_item = topLevelItem(existingIndex);
        }
    }
    // The same categories are read from the file $HOME, avoid duplicates
    const int widgetCount = cat.objectItemCount();
    for (int i = 0; i < widgetCount; ++i) {
        const ObjectItem w = cat.objectItem(i);
        if (!categoryView->containsObjectItem(w.id()))
            categoryView->addObjectItem(w, iconForObjectItem(w.iconName(), w.iconMode()), isScratchPad);
    }

    adjustSubListSize(cat_item);

    qDebug()<<"-----------------------0-cat_item:"<<cat_item->text(0);


}

void TreeWidget::removeCategory(int cat_idx)
{
    if (cat_idx >= topLevelItemCount())
        return;
    delete takeTopLevelItem(cat_idx);

    update();
    //updateGeometry();
}

void TreeWidget::removeCategory(const QString &cat_name){

    int cat_idx = indexOfCategory(cat_name);
    if (cat_idx >= topLevelItemCount() || cat_idx < 0){return;}
    
    removeCategory(cat_idx);
    
}

void TreeWidget::updateCategoryName(const QString &old_cat_name, const QString &new_cat_name){

    int cat_idx = indexOfCategory(old_cat_name);
    if (cat_idx >= topLevelItemCount() || cat_idx < 0){return;}

    QTreeWidgetItem *cat_item = topLevelItem(cat_idx);
    cat_item->setText(0, new_cat_name);

//    QTreeWidgetItem *embedItem = cat_item->child(0);
//    embedItem->setText(0, new_cat_name);

}

int TreeWidget::objectItemCount(int cat_idx) const
{
    if (cat_idx >= topLevelItemCount())
        return 0;
    // SDK functions want unfiltered access
    return categoryViewAt(cat_idx)->count(CategoryListView::UnfilteredAccess);
}

TreeWidget::ObjectItem TreeWidget::objectItem(int cat_idx, int item_idx) const
{
    if (cat_idx >= topLevelItemCount())
        return ObjectItem();
    // SDK functions want unfiltered access
    CategoryListView *categoryView = categoryViewAt(cat_idx);
    return categoryView->objectItemAt(CategoryListView::UnfilteredAccess, item_idx);
}

TreeWidget::ObjectItem TreeWidget::objectItem(const QString &cat_name, const QString &item_id) const
{
    if (cat_name.trimmed().isEmpty())
        return ObjectItem();
    // SDK functions want unfiltered access
    CategoryListView *cv = categoryView(cat_name);
    return cv->objectItem(item_id);
}

void TreeWidget::addObjectItem(int cat_idx, const ObjectItem &item)
{
    qDebug()<<"--TreeWidget::addObjectItem(...) cat_idx:"<<cat_idx<<" item.id():"<<item.id();

    if (cat_idx >= topLevelItemCount() || cat_idx < 0){
        qCritical()<<"ERROR! Invalid category index!";
        return;
    }

    QTreeWidgetItem *cat_item = topLevelItem(cat_idx);
    CategoryListView *categoryView = categoryViewAt(cat_idx);

    const bool scratch = topLevelRole(cat_item) == SCRATCHPAD_ITEM;
    //const bool scratch = topLevelRole(cat_item) == NORMAL_ITEM;

    // The same categories are read from the file $HOME, avoid duplicates
    if (!categoryView->containsObjectItem(item.id())){
        categoryView->addObjectItem(item, iconForObjectItem(item.iconName(), item.iconMode()), scratch);
    }else{
        qWarning()<<"ERROR! Item "<<item.id()<<" already exists!";
    }

    adjustSubListSize(cat_item);

    updateGeometries();

}

void TreeWidget::addObjectItem(const QString &cat_name, const ObjectItem &item){
    qDebug()<<"--TreeWidget::addObjectItem(...) cat_name:"<<cat_name<<" item.id():"<<item.id();

    if(cat_name.trimmed().isEmpty()){
        qDebug()<<"ERROR! Empty cat_name!";
        return;
    }

    addObjectItem(indexOfCategory(cat_name), item);

}

void TreeWidget::removeObjectItem(int cat_idx, int item_idx)
{
    qDebug()<<"--TreeWidget::removeObjectItem(...)";

    if (cat_idx >= topLevelItemCount())
        return;

    CategoryListView *categoryView = categoryViewAt(cat_idx);

    // SDK functions want unfiltered access
    const CategoryListView::AccessMode am = CategoryListView::UnfilteredAccess;
    if (item_idx >= categoryView->count(am))
        return;

    categoryView->removeRow(am, item_idx);
    adjustSubListSize(topLevelItem(cat_idx));

    updateGeometries();

}

void TreeWidget::removeObjectItem(const QString &cat_name, const QString &item_id){

    qDebug()<<"--TreeWidget::removeObjectItem()";

    int cat_Idx = indexOfCategory(cat_name);
    if (cat_Idx >= topLevelItemCount() || cat_Idx < 0){return ;}

    // unfiltered access
    const CategoryListView::AccessMode am = CategoryListView::UnfilteredAccess;

    CategoryListView *categoryview = categoryViewAt(cat_Idx);
    int item_idx = categoryview->indexOfObjectItem(item_id);
    //TreeWidget::ObjectItem objItem = old_categoryView->objectItemAt(am, item_idx);
    //if(objItem.isNull()){return;}

    if (item_idx >= categoryview->count(am)){return;}
    categoryview->removeRow(am, item_idx);
    adjustSubListSize(topLevelItem(cat_Idx));

    updateGeometries();

}

void TreeWidget::moveObjectItem(int old_cat_idx, int new_cat_idx, const QString &item_id){

    qDebug()<<"--TreeWidget::moveObjectItem()";

    if (old_cat_idx >= topLevelItemCount() || new_cat_idx >= topLevelItemCount()){
        qCritical()<<"ERROR! Invalid old_cat_idx or new_cat_idx!";
        return ;
    }

    // unfiltered access
    const CategoryListView::AccessMode am = CategoryListView::UnfilteredAccess;

    CategoryListView *old_categoryView = categoryViewAt(old_cat_idx);
    int item_idx = old_categoryView->indexOfObjectItem(item_id);
    TreeWidget::ObjectItem objItem = old_categoryView->objectItemAt(am, item_idx);
    if(objItem.isNull()){return;}

    //if (item_idx >= old_categoryView->count(am)){return;}
    old_categoryView->removeRow(am, item_idx);
    adjustSubListSize(topLevelItem(old_cat_idx));

    addObjectItem(new_cat_idx, objItem);




//    QTreeWidgetItem *cat_item = topLevelItem(new_cat_idx);
//    CategoryListView *categoryView = categoryViewAt(new_cat_idx);

//    const bool scratch = topLevelRole(cat_item) == SCRATCHPAD_ITEM;
//    //const bool scratch = topLevelRole(cat_item) == NORMAL_ITEM;

//    // The same categories are read from the file $HOME, avoid duplicates
//    if (!categoryView->containsObjectItem(objItem.id())){
//        categoryView->addObjectItem(objItem, iconForObjectItem(objItem.iconName(), objItem.iconMode()), scratch);
//    }else{
//        qWarning()<<"ERROR! Item "<<objItem.id()<<" already exists!";
//    }

//    adjustSubListSize(cat_item);

    updateGeometries();



}

void TreeWidget::moveObjectItem(const QString &old_cat_name, const QString &new_cat_name, const QString &item_id){

    int old_cat_idx = indexOfCategory(old_cat_name);
    int new_cat_idx = indexOfCategory(new_cat_name);

    moveObjectItem(old_cat_idx, new_cat_idx, item_id);

}


void TreeWidget::slotScratchPadItemDeleted()
{
    qDebug()<<"--TreeWidget::slotScratchPadItemDeleted()";

    const int scratch_idx = indexOfScratchpad();
    QTreeWidgetItem *scratch_item = topLevelItem(scratch_idx);
    adjustSubListSize(scratch_item);
    save();
}

void TreeWidget::slotLastScratchPadItemDeleted()
{
    // Remove the scratchpad in the next idle loop
    if (!m_scratchPadDeleteTimer) {
        m_scratchPadDeleteTimer = new QTimer(this);
        m_scratchPadDeleteTimer->setSingleShot(true);
        m_scratchPadDeleteTimer->setInterval(0);
        connect(m_scratchPadDeleteTimer, SIGNAL(timeout()), this, SLOT(deleteScratchpad()));
    }
    if (!m_scratchPadDeleteTimer->isActive())
        m_scratchPadDeleteTimer->start();
}

void TreeWidget::deleteScratchpad()
{
    const int idx = indexOfScratchpad();
    if (idx == -1)
        return;
    delete takeTopLevelItem(idx);
    save();
}


void TreeWidget::slotListMode()
{
    m_iconMode = false;
    updateViewMode();
}

void TreeWidget::slotIconMode()
{
    m_iconMode = true;
    updateViewMode();
}

void TreeWidget::updateViewMode()
{
    qDebug()<<"--TreeWidget::updateViewMode()";

    if (const int numTopLevels = topLevelItemCount()) {
        for (int i = numTopLevels - 1; i >= 0; --i) {
            QTreeWidgetItem *topLevel = topLevelItem(i);
            // Scratch pad stays in list mode.
            const QListView::ViewMode viewMode  = m_iconMode && (topLevelRole(topLevel) != SCRATCHPAD_ITEM) ? QListView::IconMode : QListView::ListMode;
            CategoryListView *categoryView = categoryViewAt(i);
            if (viewMode != categoryView->viewMode()) {
                categoryView->setViewMode(viewMode);
                adjustSubListSize(topLevelItem(i));
            }
        }
    }

    updateGeometries();
}

void TreeWidget::resizeEvent(QResizeEvent *e)
{
    qDebug()<<"--TreeWidget::resizeEvent()";

    QTreeWidget::resizeEvent(e);
    if (const int numTopLevels = topLevelItemCount()) {
//        for (int i = numTopLevels - 1; i >= 0; --i)
//            adjustSubListSize(topLevelItem(i));

        for (int i = 0; i < numTopLevels; i++){
            adjustSubListSize(topLevelItem(i));
        }


    }


}

void TreeWidget::contextMenuEvent(QContextMenuEvent *e)
{
    QTreeWidgetItem *item = itemAt(e->pos());

    const bool scratchpad_menu = item != 0
                            && item->parent() != 0
                            && topLevelRole(item->parent()) ==  SCRATCHPAD_ITEM;

    if(!item || item->parent()){
        return;
    }

    //emit contextMenuEventOnCategoryOccurs(item->text(0), mapToGlobal(e->pos()));


    QMenu menu;
    menu.addAction(tr("Expand all"), this, SLOT(expandAll()));
    menu.addAction(tr("Collapse all"), this, SLOT(collapseAll()));
    menu.addSeparator();

    QAction *listModeAction = menu.addAction(tr("List View"));
    QAction *iconModeAction = menu.addAction(tr("Icon View"));
    listModeAction->setCheckable(true);
    iconModeAction->setCheckable(true);
    QActionGroup *viewModeGroup = new QActionGroup(&menu);
    viewModeGroup->addAction(listModeAction);
    viewModeGroup->addAction(iconModeAction);
    if (m_iconMode)
        iconModeAction->setChecked(true);
    else
        listModeAction->setChecked(true);
    connect(listModeAction, SIGNAL(triggered()), SLOT(slotListMode()));
    connect(iconModeAction, SIGNAL(triggered()), SLOT(slotIconMode()));

    if (scratchpad_menu) {
        menu.addSeparator();
        menu.addAction(tr("Remove"), itemWidget(item, 0), SLOT(removeCurrentItem()));
        if (!m_iconMode)
            menu.addAction(tr("Edit name"), itemWidget(item, 0), SLOT(editCurrentItem()));
    }

    e->accept();

    //menu.exec(mapToGlobal(e->pos()));

    //menu.addSeparator();
    //CategoryListView *categoryListView = qobject_cast<CategoryListView*>(itemWidget(embedItem, 0));
    //menu.addAction(tr("Rename"), this, SLOT(editItem(item)));

    m_core->slotContextMenuEventOnCategoryOccurs(item->text(0), mapToGlobal(e->pos()), &menu);


}

void TreeWidget::dropObjectItems(const QList<QDesignerDnDItemInterface*> &item_list)
{
/*
    QTreeWidgetItem *scratch_item = 0;
    CategoryListView *categoryView = 0;
    bool added = false;

    foreach (QDesignerDnDItemInterface *item, item_list) {
        QWidget *w = item->objectItem();
        if (w == 0)
            continue;

        DomUI *dom_ui = item->domUi();
        if (dom_ui == 0)
            continue;

        const int scratch_idx = ensureScratchpad();
        scratch_item = topLevelItem(scratch_idx);
        categoryView = categoryViewAt(scratch_idx);

        // Temporarily remove the fake toplevel in-between
        DomWidget *fakeTopLevel = dom_ui->takeElementWidget();
        DomWidget *firstWidget = 0;
        if (fakeTopLevel && !fakeTopLevel->elementWidget().isEmpty()) {
            firstWidget = fakeTopLevel->elementWidget().first();
            dom_ui->setElementWidget(firstWidget);
        } else {
            dom_ui->setElementWidget(fakeTopLevel);
            continue;
        }

        // Serialize to XML
        QString xml;
        {
            QXmlStreamWriter writer(&xml);
            writer.setAutoFormatting(true);
            writer.setAutoFormattingIndent(1);
            writer.writeStartDocument();
            dom_ui->write(writer);
            writer.writeEndDocument();
        }

        // Insert fake toplevel again
        dom_ui->takeElementWidget();
        dom_ui->setElementWidget(fakeTopLevel);

        const ObjectItem wgt = ObjectItem(w->objectName(), xml);
        categoryView->addObjectItem(wgt, iconForObjectItem(wgt.iconName()), true);
        setItemExpanded(scratch_item, true);
        added = true;
    }

    if (added) {
        save();
        QApplication::setActiveWindow(this);
        // Is the new item visible in filtered mode?
        const CategoryListView::AccessMode am = CategoryListView::FilteredAccess;
        if (const int count = categoryView->count(am))
            categoryView->setCurrentItem(am, count - 1);
        categoryView->adjustSize(); // XXX
        adjustSubListSize(scratch_item);
    }
  */

}

void TreeWidget::filter(const QString &f)
{
    qDebug()<<"--TreeWidget::filter()";

    const bool empty = f.isEmpty();
    const QRegExp re = empty ? QRegExp() : QRegExp(f, Qt::CaseInsensitive, QRegExp::FixedString);
    const int numTopLevels = topLevelItemCount();
    bool changed = false;
    for (int i = 0; i < numTopLevels; i++) {
        QTreeWidgetItem *tl = topLevelItem(i);
        CategoryListView *categoryView = categoryViewAt(i);
        // Anything changed? -> Enable the category
        const int oldCount = categoryView->count(CategoryListView::FilteredAccess);
        categoryView->filter(re);
        const int newCount = categoryView->count(CategoryListView::FilteredAccess);
        if (oldCount != newCount) {
            changed = true;
            const bool categoryEnabled = newCount > 0 || empty;
            if (categoryEnabled) {
                categoryView->adjustSize();
                adjustSubListSize(tl);
            }
            setRowHidden (i, QModelIndex(), !categoryEnabled);
        }
    }
    if (changed)
        updateGeometries();
}

bool TreeWidget::updateObjectItemName(const QString &cat_name, const QString &item_id, const QString &itemName){
    CategoryListView *cat = categoryView(cat_name);
    if(!cat){
        return false;
    }

    return cat->updateObjectItemName(item_id, itemName);
}

bool TreeWidget::updateObjectItemIcon(const QString &cat_name, const QString &item_id, const QIcon &icon){
	qDebug()<<"----TreeWidget::updateObjectItemIcon(const QString &cat_name, const QString &item_id, const QIcon &icon)";

	CategoryListView *cat = categoryView(cat_name);

        return cat->updateObjectItemIcon(item_id, icon);

}

bool TreeWidget::updateObjectItemIcon(const QString &cat_name, const QString &item_id, const QString &iconName){
	qDebug()<<"----TreeWidget::updateObjectItemIcon(const QString &cat_name, const QString &item_id, const QString &iconName)";

	CategoryListView *cat = categoryView(cat_name);

        return cat->updateObjectItemIcon(item_id, iconName);

}









}  // namespace HEHUI

QT_END_NAMESPACE
