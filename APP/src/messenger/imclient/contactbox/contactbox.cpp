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


#include <QMessageBox>


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
    connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(handleMouseDoubleClick(QTreeWidgetItem*)));

    m_contactsManager = ContactsManager::instance();
    m_myself = IMUser::instance();


    m_userInfoTipWindow = new UserInfoTipWindow(this);
    //connect(m_userInfoTipWindow, SIGNAL(showUserInfoRequested(IMUserBase*)), this, SLOT(showUserInfo(IMUserBase*)));




}

ContactBox::~ContactBox(){

    m_userInfoTipWindow->hide();
    delete m_userInfoTipWindow;

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


    if(item->parent() == 0){
        handleContextMenuEventOnContactGroup(item, mapToGlobal(e->pos()));
    }else{
        handleContextMenuEventOnContact(item, mapToGlobal(e->pos()));
    }

    e->accept();

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

void ContactBox::handleMouseDoubleClick(QTreeWidgetItem* item){

    if (item == 0 || item->parent() == 0){return;}


    QMessageBox::information(this, tr(""), item->data(0, Qt::DisplayRole).toString());

}

void ContactBox::handleContextMenuEventOnContactGroup(QTreeWidgetItem* item, const QPoint &global_mouse_pos){
    qDebug()<<"--ContactBox::handleContextMenuEventOnContactGroup(...)";


    ContactGroupBase *group = contactGroupsHash.key(item);
    if(!group){return;}

    int groupID = group->getGroupID();
    //    ContactGroupBase *contactGroup = m_myself->getContactGroup(groupID);
    //    if(!contactGroup){return;}

    m_userInfoTipWindow->hideUserInfoTip();


    QMenu contextMenu;

    contextMenu.addAction(tr("Expand all"), this, SLOT(expandAll()));
    contextMenu.addAction(tr("Collapse all"), this, SLOT(collapseAll()));
    contextMenu.addSeparator();

    QAction actionRenameGroupName(tr("Rename Group"), contextMenu);
    QAction actionDeleteGroup(tr("Delete Group"), contextMenu);
    QAction actionCreateNewGroup(tr("Create New Group"), contextMenu);

    if(ContactGroupBase::isUserCreatedGroup(groupID)){
        contextMenu->addAction(&actionRenameGroupName);
        if(m_myself->countOfContactGroupMembers(groupID) == 0){
            contextMenu->addAction(&actionDeleteGroup);
        }
    }

    contextMenu->addAction(&actionCreateNewGroup);

    QAction *action = contextMenu->exec(global_mouse_pos);
    if(action == &actionRenameGroupName){
        bool ok = false;
        QString labelText = tr("New Name:\n(Only word-characters up to 16 are acceptable!)");
        QString newGroupName = QInputDialog::getText(this, tr("Rename Group"),
                                                     labelText, QLineEdit::Normal,
                                                     contactGroup->getGroupName(), &ok);
        if (ok && !newGroupName.isEmpty()){
            int pos = 0;
            QRegExpValidator rxValidator(this);
            QRegExp rx("\\b\\w{0,16}\\b");
            rxValidator.setRegExp(rx);
            if(rxValidator.validate(newGroupName, pos) != QValidator::Acceptable){
                QMessageBox::critical(this, tr("Error"), tr("Invalid Group Name!"));
                return ;
            }

            if(m_myself->hasContactGroup(newGroupName)){
                QMessageBox::critical(this, tr("Error"), tr("Group with the same name already exists!"));
                return;
            }

            emit signalRenameContactGroup(groupID, newGroupName);
            //showProgressDialog();

//            clientPacketsParser->renameContactGroup(m_socketConnectedToServer, groupID, newGroupName);

//            m_contactsManager->renameContactGroupToDatabase(groupID, newGroupName);
//            m_contactsManager->renameContactGroupToUI(friendBox, groupID, newGroupName);
//            m_myself->renameContactGroup(groupID, newGroupName);

        }


    }else if(action == &actionDeleteGroup){
        QString groupName = m_myself->getContactGroupName(groupID);
        int ret = QMessageBox::question(this, tr("Delete Contact Group"), tr("Are you sure you want to delete the the group '%1' ?").arg(groupName), QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
        if(ret == QMessageBox::No){return;}

        emit signalCreateOrDeleteContactGroup(groupID, "", false);
//        showProgressDialog();
//        clientPacketsParser->createOrDeleteContactGroup(m_socketConnectedToServer, groupID, "", false);


    }else if(action == &actionCreateNewGroup){

        bool ok = false;
        QString labelText = tr("Group Name:\n(Only word-characters up to 16 are acceptable!)");
        QString newGroupName = QInputDialog::getText(this, tr("Create New Group"),
                                                     labelText, QLineEdit::Normal,
                                                     "", &ok);
        if (ok && !newGroupName.isEmpty()){
            int pos = 0;
            QRegExpValidator rxValidator(this);
            QRegExp rx("\\b\\w{0,16}\\b");
            rxValidator.setRegExp(rx);
            if(rxValidator.validate(newGroupName, pos) != QValidator::Acceptable){
                QMessageBox::critical(this, tr("Error"), tr("Invalid Group Name!"));
                return ;
            }
            //TODO:
            m_myself->saveMyInfoToLocalDatabase();
            if(m_myself->hasContactGroup(newGroupName)){
                QMessageBox::critical(this, tr("Error"), tr("Group already exists!"));
                return;
            }

            emit signalCreateOrDeleteContactGroup(0, newGroupName, true);
//            showProgressDialog();
//            clientPacketsParser->createOrDeleteContactGroup(m_socketConnectedToServer, 0, newGroupName, true);

        }

    }

}

void ContactBox::handleContextMenuEventOnContact(QTreeWidgetItem* item, const QPoint &global_mouse_pos){
    qDebug()<<"--ContactBox::handleContextMenuEventOnContact(...)";


    m_userInfoTipWindow->hideUserInfoTip();


    Contact *contact = contactsHash.key(item);
    if(!contact){
        QMessageBox::critical(this, tr("Error"), tr("Contact '%1' does not exist!").arg(contactID));
        return;
    }

    QMenu menu;

    int oldGroupID = contact->getContactGroupID();
    QList<ContactGroupBase *> groups = m_myself->getContactGroups();
    groups.removeAll(m_myself->getContactGroup(oldGroupID));

    if(!groups.isEmpty()){
        QMenu *menuMoveContactToGroup = menu.addMenu(tr("Move To"));
        foreach (ContactGroupBase *group, groups) {
            QAction *action = menuMoveContactToGroup->addAction(group->getGroupName());
            action->setData(contactID);
            connect(action, SIGNAL(triggered()), this, SLOT(slotMoveContactToGroup()));
        }
        menuMoveContactToGroup->addSeparator();
    }

    QAction actionDeleteContact(tr("Delete Contact"), &menu);
    menu.addAction(&actionDeleteContact);

    QAction actionBlockContact(tr("Block Contact"), &menu);
    menu.addAction(&actionBlockContact);

    QAction *executedAction = 0;

    executedAction = menu->exec(global_mouse_pos);

    if(executedAction == &actionDeleteContact){
        if(oldGroupID == ContactGroupBase::Group_Strangers_ID){
            //TODO:Close chat window
            if(!chatWindowManager->closeContactChatWindow(contact)){
                return;
            }
            m_contactsManager->slotdeleteContactFromDatabase(contact);
            m_contactsManager->deleteContactFromUI(friendBox, oldGroupID, contactID);
        }else if(oldGroupID == ContactGroupBase::Group_Blacklist_ID){
            slotRequestDeleteContact(contactID);
            showProgressDialog();
        }else{
            showDeleteContactDialog(contact, false);
        }

    }else if(executedAction == &actionBlockContact){
        showDeleteContactDialog(contact, true);
    }


}

void ContactBox::handleTooltipEventOnContact(const QString &contactID, const QPoint &global_item_topLeft_pos, const QPoint &global_mouse_pos){

    qDebug()<<"--MainWindow::handleTooltipEventOnItem()--contactID:"<<contactID;


    Contact *contact = m_contactsManager->getUser(contactID);
    if(!contact){
        m_userInfoTipWindow->hideUserInfoTip();
        return;
    }


    //QString tip = QString("<b><h4>ID:%1</h4></b>").arg(contactID);
    //QToolTip::showText(global_mouse_pos, tip);


    QSize userInfoTipWindowSize = m_userInfoTipWindow->size();
    QPoint p = ui.contactsToolBox->mapToGlobal(QPoint(0,0));
    int x = p.x()-userInfoTipWindowSize.width();
    if(x < 0){
        x = p.x() + ui.contactsToolBox->width();
    }

    //    qDebug()<<"global_item_topLeft_pos:x:"<<global_item_topLeft_pos.x()<<" y:"<<global_item_topLeft_pos.y();
    //    qDebug()<<"global_mouse_pos:x:"<<global_mouse_pos.x()<<" y:"<<global_mouse_pos.y();
    //    qDebug()<<"x:"<<x<<" y:"<<global_item_topLeft_pos.y();

    m_userInfoTipWindow->showUserInfoTip(contact, mapTo(this, QPoint(x, global_item_topLeft_pos.y())) );

    activateWindow();
    raise();


}





} //namespace HEHUI
