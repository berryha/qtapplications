/*
 * widgetmanager.cpp
 *
 *  Created on: 2009-9-1
 *      Author: 贺辉
 */

#include "expandlistviewmanager.h"

#include <QMessageBox>
#include <QDebug>


QT_BEGIN_NAMESPACE


namespace HEHUI {

ExpandListViewManager::ExpandListViewManager(QObject *parent)
	:QObject(parent)
{
	// TODO Auto-generated constructor stub

}

ExpandListViewManager::~ExpandListViewManager()
{
	// TODO Auto-generated destructor stub
}


ExpandListView *ExpandListViewManager::createExpandListView(QWidget *parent){
	ExpandListView *expandListView = new ExpandListView(this, parent);
	return expandListView;

}



void ExpandListViewManager::slotItemActivated(const QModelIndex & index){


	QMessageBox::information(0, "slotItemActivated", "WidgetBoxManager::slotItemActivated(const QModelIndex & index)\n"+index.data(Qt::DisplayRole	).toString());
	qDebug()<<"WidgetBoxManager::slotItemActivated(const QModelIndex & index)~~~";

}

void ExpandListViewManager::slotContactItemActivated(const QString &id){

	emit signalContactItemActivated(id);

	qDebug()<<"WidgetBoxManager::slotItemActivated(WidgetBoxInterface::Widget *wgt)~~~";

}


void ExpandListViewManager::slotItemDoubleClicked(const QModelIndex & index){
QMessageBox::information(0, "slotItemDoubleClicked", "slotItemDoubleClicked\n"+index.data(Qt::DisplayRole	).toString());

}

void ExpandListViewManager::slotItemEntered(const QModelIndex & index ){
	QMessageBox::information(0, "slotItemEntered", "slotItemEntered\n"+index.data(Qt::DisplayRole	).toString());

}

void ExpandListViewManager::slotContextMenuEventOnCategoryOccurs(const QString &cat_name, const QPoint &global_mouse_pos, QMenu *contextMenu){

    emit contextMenuEventOnCategoryOccurs(cat_name, global_mouse_pos, contextMenu);

//    if(contextMenu){
//        contextMenu->addSeparator();
//        contextMenu->addAction("AAAAAAA");
//        contextMenu->exec(global_mouse_pos);
//    }

}

void ExpandListViewManager::slotContextMenuEventOnObjectItemOccurs(const QString &item_id, const QPoint &global_mouse_pos, QMenu *contextMenu){

    emit contextMenuEventOnObjectItemOccurs(item_id, global_mouse_pos, contextMenu);

//    if(contextMenu){
//        contextMenu->exec(global_mouse_pos);
//    }

}

void ExpandListViewManager::slotTooltipEventOnObjectItemOccurs(const QString &item_id, const QPoint &global_item_topLeft_pos, const QPoint &global_mouse_pos){
    emit signalTooltipEventOnObjectItemOccurs(item_id, global_item_topLeft_pos, global_mouse_pos);
}










}  // namespace HEHUI

QT_END_NAMESPACE
