/*
 ****************************************************************************
 * filename
 *
 * Created on: 2013-10-28
 *     Author: 贺辉
 *    License: LGPL
 *    Comment:
 *
 *
 *    =============================  Usage  =============================
 *|
 *|
 *    ===================================================================
 *
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 ****************************************************************************
 */

/*
 ***************************************************************************
 * Last Modified on: 2013-10-28
 * Last Modified by: 贺辉
 ***************************************************************************
 */





#include "contactbox.h"

#include "sheet_delegate_p.h"


namespace HEHUI{

ContactBox::ContactBox(QWidget *parent) :
    QTreeWidget(parent)
{

    //setFocusPolicy(Qt::NoFocus);
    setIndentation(0);
    setRootIsDecorated(false);
    setColumnCount(1);
    header()->hide();
    header()->setSectionResizeMode(QHeaderView::Stretch);
    setTextElideMode(Qt::ElideMiddle);
    setVerticalScrollMode(ScrollPerPixel);

    setItemDelegate(new SheetDelegate(this, this));

    connect(this, SIGNAL(itemPressed(QTreeWidgetItem*,int)), this, SLOT(handleMousePress(QTreeWidgetItem*)));


    m_contactsManager = ContactsManager::instance();
    m_myself = IMUser::instance();

}

void ContactBox::loadAllContacts(){

    QList<ContactGroupBase *> groups = m_myself->getContactGroups(false, false);
    ContactGroupBase * strangersGroup = m_myself->strangersGroup();
    groups.removeAll(strangersGroup);

    QHash<QString/*Contact ID*/, Contact*> users = m_contactsManager->getAllUsers();


    foreach (ContactGroupBase *contactGroup, groups) {
        addOrRemoveContactGroupItem(contactGroup, true);

        QStringList members = contactGroup->members();
        foreach (QString contactID, members) {
            Contact *contact = users.take(contactID);
            Q_ASSERT(contact);
            if(!contact){
                continue;
            }

            addOrRemoveContactItem(contact, true);


        }

    }


}

void ContactBox::addOrRemoveContactItem(Contact *contact, bool add){

    QTreeWidgetItem *parentItem = contactGroupsHash.value(m_myself->getContactGroup(contact->getContactGroupID()));
    if(!parentItem){return;}

    QTreeWidgetItem *item = contactsHash.value(contact);


    if(add){
        if(item){return;}

        item = new QTreeWidgetItem(parentItem);
        setItemWidget(item, 0, new ContactWidget(contact, this));

        contactsHash.insert(contact, item);
    }else{
        if(!item){return;}

        contactsHash.remove(contact);

        parentItem->removeChild(item);
        delete item;
        item = 0;

    }


}

void ContactBox::updateContactItemInfo(Contact *contact){

    QTreeWidgetItem *item = contactsHash.value(contact);
    if(!item){return;}


}

void ContactBox::addOrRemoveContactGroupItem(ContactGroupBase *contactGroup, bool add){

    if(!contactGroup){return;}

//    quint32 groupID = contactGroup->getGroupID();


    QTreeWidgetItem *item = contactGroupsHash.value(contactGroup);
    if(add){
        if(item){return;}

        item = new QTreeWidgetItem();
        item->setText(0, contactGroup->getGroupName());
        addTopLevelItem(item);

        contactGroupsHash.insert(contactGroup, item);

    }else{

        contactGroupsHash.remove(contactGroup);

        takeTopLevelItem(indexOfTopLevelItem((item)));

        delete item;
        item = 0;

    }


}

void ContactBox::updateContactGroupItemInfo(ContactGroupBase *contactGroup){

    QTreeWidgetItem *item = contactGroupsHash.value(contactGroup);
    if(!item){
        return;
    }

    //TODO
    item->setText(0, contactGroup->getGroupName());


}


void ContactBox::contextMenuEvent(QContextMenuEvent *e)
{
    QTreeWidgetItem *item = itemAt(e->pos());
    if(!item){return;}

//    const bool scratchpad_menu = item != 0
//                            && item->parent() != 0
//                          ;

    QMenu menu;
    menu.addAction(tr("Expand all"), this, SLOT(expandAll()));
    menu.addAction(tr("Collapse all"), this, SLOT(collapseAll()));
    menu.addSeparator();


//    if (scratchpad_menu) {
//        menu.addSeparator();
//        menu.addAction(tr("Remove"), itemWidget(item, 0), SLOT(removeCurrentItem()));
//        if (!m_iconMode)
//            menu.addAction(tr("Edit name"), itemWidget(item, 0), SLOT(editCurrentItem()));
//    }

    e->accept();

    menu.exec(mapToGlobal(e->pos()));

}

void ContactBox::resizeEvent(QResizeEvent *e)
{
    QTreeWidget::resizeEvent(e);
    if (const int numTopLevels = topLevelItemCount()) {
        for (int i = 0; i < numTopLevels; i++){
//            adjustSubListSize(topLevelItem(i));
        }
    }

}




void ContactBox::handleMousePress(QTreeWidgetItem* item){

    if (item == 0)
        return;

    if (QApplication::mouseButtons() != Qt::LeftButton)
        return;

    if (item->parent() == 0) {
        setItemExpanded(item, !isItemExpanded(item));
        return;
    }

}




} //namespace HEHUI
