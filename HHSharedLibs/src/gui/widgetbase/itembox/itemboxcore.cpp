#include "itemboxcore.h"

#include <QDebug>

namespace HEHUI {


ItemBoxCore::ItemBoxCore(QObject *parent) :
    QObject(parent)
{

}

void ItemBoxCore::handleItemPressed(const QString &item_id, const QPoint &globalPos){
    qDebug()<<"ItemBoxCore::handleItemPressed(...)"<<" item_id:"<<item_id;
}

void ItemBoxCore::handleItemActivated(const QModelIndex & index){
    qDebug()<<"ItemBoxCore::handleItemActivated(...)"<<" Data:"<<index.data().toString();
}

void ItemBoxCore::handleItemActivated(const QString &id){
    qDebug()<<"ItemBoxCore::handleItemActivated(...)"<<" id:"<<id;

}

void ItemBoxCore::handleItemDoubleClicked(const QModelIndex & index){
    qDebug()<<"ItemBoxCore::handleItemDoubleClicked(...)"<<" Data:"<<index.data().toString();

}

void ItemBoxCore::handleItemEntered(const QModelIndex & index ){
    qDebug()<<"ItemBoxCore::handleItemEntered(...)"<<" Data:"<<index.data().toString();

}

void ItemBoxCore::handleContextMenuEventOnCategory(const QString &cat_name, const QPoint &global_mouse_pos, QMenu *contextMenu){
    qDebug()<<"ItemBoxCore::handleContextMenuEventOnCategory(...)"<<" cat_name:"<<cat_name;

}

void ItemBoxCore::handleContextMenuEventOnItem(const QString &item_id, const QPoint &global_mouse_pos, QMenu *contextMenu){
    qDebug()<<"ItemBoxCore::handleContextMenuEventOnCategory(...)"<<" item_id:"<<item_id;

}

void ItemBoxCore::handleTooltipEventOnItem(const QString &item_id, const QPoint &global_item_topLeft_pos, const QPoint &global_mouse_pos){
    qDebug()<<"ItemBoxCore::handleTooltipEventOnItem(...)"<<" item_id:"<<item_id;

}



} //namespace HEHUI
