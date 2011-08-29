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

#include <QDebug>

#include "expandlistview.h"

//#include "widgetbox_dnditem.h"



#include "./shared/iconloader_p.h"
#include "shared/abstractexpandlistviewInterface.h"
//#include <shared/filterwidget_p.h>

#include <QtGui/QDropEvent>
#include <QtGui/QVBoxLayout>
#include <QtGui/QApplication>
#include <QtGui/QToolBar>
#include <QtGui/QIcon>
#include <QAction>
#include <QMessageBox>



namespace HEHUI {

ExpandListView::ExpandListView(ExpandListViewManager *core, QWidget *parent, Qt::WindowFlags flags)
    : ExpandListViewBase(parent, flags),
      m_core(core),
      m_treeWidget(new TreeWidget(m_core, this))
{

    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->setMargin(0);
    vBoxLayout->setSpacing(0);

    // Prevent the filter from grabbing focus since Our view has Qt::NoFocus
    /*FilterWidget **/filterWidget = new FilterWidget(0, FilterWidget::LayoutAlignNone);
    filterWidget->setRefuseFocus(true);
    connect(filterWidget, SIGNAL(filterChanged(QString)), m_treeWidget, SLOT(filter(QString)));
    vBoxLayout->addWidget(filterWidget);
    filterWidget->hide();

//    QAction *actionShowFilterWidget = new QAction(tr("Open Filter"), this);
//    actionShowFilterWidget->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F));
//		connect(actionShowFilterWidget, SIGNAL(triggered()), filterWidget, SLOT(show()));


    // View
    //m_treeWidget = new TreeWidget(this);
    connect(m_treeWidget, SIGNAL(pressed(QString,QString,QPoint)),
            this, SLOT(handleMousePress(QString,QString,QPoint)));
    vBoxLayout->addWidget(m_treeWidget);

    setAcceptDrops (true);



//	autoHideTimer = new QTimer(this);
//	autoHideTimer->setInterval(5000);
//	autoHideTimer->setSingleShot(true);
//	QObject::connect(autoHideTimer, SIGNAL(timeout()), filterWidget, SLOT(hide()));
//	autoHideTimer->start();

    installEventFilter(this);
}

ExpandListView::~ExpandListView()
{
}


bool ExpandListView::eventFilter(QObject *o, QEvent *e)
{

	if (e->type() == QEvent::KeyPress) {
    			QKeyEvent *ke = static_cast<QKeyEvent *>(e);
    			if ((ke->modifiers() & Qt::ControlModifier) && ke->key() == Qt::Key_F) {
							if(!filterWidget->isVisible()){
								filterWidget->show();
							}
							filterWidget->setFocusToEditor();
							//autoHideTimer->start();
    				//keyPressEvent(ke);
    				return true;
					}else if(ke->key() == Qt::Key_Escape){
						filterWidget->reset();
							filterWidget->hide();
							return true;
					}


	}

	return QWidget::eventFilter(o, e);

}


/*
void ExpandListView::setInput(QList<QObject *> list){
	objectList = list;

	ContentProvider *provider = getContentProvider();
	if(!provider){
		return;
	}

	CategoryList *cat_list = provider->getCategoryList(list);
	m_treeWidget->load(cat_list);



}

QList<QObject *> ExpandListView::getInput(){
	return objectList;

}
*/



ExpandListViewManager *ExpandListView::core() const
{
    return m_core;
}




/*

void ExpandListView::handleMousePress(const QString &name, const QString &xml, const QPoint &global_mouse_pos)
{
    if (QApplication::mouseButtons() != Qt::LeftButton)
        return;

    DomUI *ui = xmlToUi(name, xml, true);
    if (ui == 0)
        return;
    QList<QDesignerDnDItemInterface*> item_list;
    item_list.append(new WidgetBoxDnDItem(core(), ui, global_mouse_pos));
    m_core->formWindowManager()->dragItems(item_list);
}
*/


int ExpandListView::categoryCount() const
{
    return m_treeWidget->categoryCount();
}

ExpandListViewInterface::Category ExpandListView::category(int cat_idx) const
{
    return m_treeWidget->category(cat_idx);
}

ExpandListViewInterface::Category ExpandListView::category(const QString &cat_name) const
{
    return m_treeWidget->category(cat_name);
}

void ExpandListView::addCategory(const Category &cat)
{
    m_treeWidget->addCategory(cat);
}

void ExpandListView::removeCategory(int cat_idx)
{
    m_treeWidget->removeCategory(cat_idx);
}

void ExpandListView::removeCategory(const QString &cat_name){
     m_treeWidget->removeCategory(cat_name);
}

void ExpandListView::updateCategoryName(const QString &old_cat_name, const QString &new_cat_name){
    m_treeWidget->updateCategoryName(old_cat_name, new_cat_name);
}

int ExpandListView::objectItemCount(int cat_idx) const
{
    return m_treeWidget->objectItemCount(cat_idx);
}

ExpandListViewInterface::ObjectItem ExpandListView::objectItem(int cat_idx, int item_idx) const
{
    return m_treeWidget->objectItem(cat_idx, item_idx);
}

ExpandListViewInterface::ObjectItem ExpandListView::objectItem(const QString &cat_name, const QString &item_id) const{
	return m_treeWidget->objectItem(cat_name, item_id);

}

void ExpandListView::addObjectItem(int cat_idx, const ObjectItem &item)
{
    m_treeWidget->addObjectItem(cat_idx, item);
}

void ExpandListView::addObjectItem(const QString &cat_name, const ObjectItem &item){
    m_treeWidget->addObjectItem(cat_name, item);
}

void ExpandListView::removeObjectItem(int cat_idx, int item_idx)
{
    m_treeWidget->removeObjectItem(cat_idx, item_idx);
}

void ExpandListView::removeObjectItem(const QString &cat_name, const QString &item_id){
    m_treeWidget->removeObjectItem(cat_name, item_id);
}

void ExpandListView::moveObjectItem(int old_cat_idx, int new_cat_idx, const QString &item_id){
    m_treeWidget->moveObjectItem(old_cat_idx, new_cat_idx, item_id);
}

void ExpandListView::moveObjectItem(const QString &old_cat_name, const QString &new_cat_name, const QString &item_id){
    m_treeWidget->moveObjectItem(old_cat_name, new_cat_name, item_id);
}

void ExpandListView::dropObjectItems(const QList<QDesignerDnDItemInterface*> &item_list, const QPoint&)
{
    m_treeWidget->dropObjectItems(item_list);
}

void ExpandListView::setFileName(const QString &file_name)
{
    m_treeWidget->setFileName(file_name);
}

QString ExpandListView::fileName() const
{
    return m_treeWidget->fileName();
}

bool ExpandListView::load()
{
    return m_treeWidget->load(loadMode());
}

bool ExpandListView::load(Category *cat)
{
    return m_treeWidget->load(cat);
}

bool ExpandListView::load(CategoryList *cat_list)
{
    return m_treeWidget->load(cat_list);
}


bool ExpandListView::loadContents(const QString &contents)
{
    return m_treeWidget->loadContents(contents);
}

bool ExpandListView::save()
{
    return m_treeWidget->save();
}


/*

static const QDesignerMimeData *checkDragEvent(QDropEvent * event,
                                               bool acceptEventsFromWidgetBox)
{
    const QDesignerMimeData *mimeData = qobject_cast<const QDesignerMimeData *>(event->mimeData());
    if (!mimeData) {
        event->ignore();
        return 0;
    }
    // If desired, ignore a objectItem box drag and drop, where objectItem==0.
    if (!acceptEventsFromWidgetBox) {
        const bool fromWidgetBox = !mimeData->items().first()->objectItem();
        if (fromWidgetBox) {
            event->ignore();
            return 0;
        }
    }

    mimeData->acceptEvent(event);
    return mimeData;
}




void ExpandListView::dragEnterEvent (QDragEnterEvent * event)
{
    // We accept event originating from the objectItem box also here,
    // because otherwise Windows will not show the DnD pixmap.
    checkDragEvent(event, true);
}

void ExpandListView::dragMoveEvent(QDragMoveEvent * event)
{
    checkDragEvent(event, true);
}

void ExpandListView::dropEvent(QDropEvent * event)
{
    const QDesignerMimeData *mimeData = checkDragEvent(event, false);
    if (!mimeData)
        return;

    dropObjectItems(mimeData->items(), event->pos());
    QDesignerMimeData::removeMovedWidgetsFromSourceForm(mimeData->items());
}

*/



QIcon ExpandListView::iconForObjectItem(const QString &className, const QString &category) const
{
    ObjectItem widgetData;
    if (!findObjectItem(this, className, category, &widgetData))
        return QIcon();
    return m_treeWidget->iconForObjectItem(widgetData.iconName(), widgetData.iconMode());
}

bool ExpandListView::updateObjectItemName(const QString &cat_name, const QString &item_id, const QString &itemName){
    return m_treeWidget->updateObjectItemName(cat_name, item_id, itemName);

}

bool ExpandListView::updateObjectItemIcon(const QString &cat_name, const QString &item_id, const QIcon &icon){
	qDebug()<<"----ExpandListView::updateObjectItemIcon(const QString &cat_name, const QString &item_id, const QIcon &icon)";

	return m_treeWidget->updateObjectItemIcon(cat_name, item_id, icon);

}

bool ExpandListView::updateObjectItemIcon(const QString &cat_name, const QString &item_id, const QString &iconName){
	qDebug()<<"----ExpandListView::updateObjectItemIcon(const QString &cat_name, const QString &item_id, const QString &iconName)";

	return m_treeWidget->updateObjectItemIcon(cat_name, item_id, iconName);

}









}  // namespace HEHUI


