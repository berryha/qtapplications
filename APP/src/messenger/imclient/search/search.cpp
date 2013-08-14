

#include <QMessageBox>

#include "search.h"

#include "../../sharedim/constants_global_shared.h"
#include "../imuser.h"
#include "addcontactdialog.h"


namespace HEHUI{

//Search * Search::m_searchInstance = 0;

Search::Search(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);

        ui.comboBoxGender->addItem(tr("Any"), QVariant(IMUser::GENDER_UNKNOWN));
        ui.comboBoxGender->addItem(tr("Male"), QVariant(IMUser::MALE));
        ui.comboBoxGender->addItem(tr("Female"), QVariant(IMUser::FEMALE));
        ui.comboBoxGender->setCurrentIndex(0);
        
        ui.comboBoxAge->addItem(tr("Any"), QVariant(IMUser::Age_Any));
        ui.comboBoxAge->addItem("18-", QVariant(IMUser::Age_1_18));
        ui.comboBoxAge->addItem("19-30", QVariant(IMUser::Age_19_30));
        ui.comboBoxAge->addItem("31-40", QVariant(IMUser::Age_31_40));
        ui.comboBoxAge->addItem("40+", QVariant(IMUser::Age_40_));
        ui.comboBoxAge->setCurrentIndex(0);
        
        ui.pushButtonCondition->setVisible(false);
        
        
//        ui.radioButtonUsersMatchWildcard->click();
//        ui.radioButtonGroupsMatchWildcard->click();
        
        ui.radioButtonUsersMatchExactly->click();
        ui.labelUsersResult->hide();
        ui.lineEditUserID->setFocus();
        ui.radioButtonGroupsMatchExactly->click();
        ui.labelGroupsResult->hide();
        
        connect(this, SIGNAL(accepted()), this, SLOT(reset()));
        connect(this, SIGNAL(rejected()), this, SLOT(reset()));
        
        
        userInfoModel = new UserInfoModel(this);
        ui.tableViewUsersResult->setModel(userInfoModel);
        ui.tableViewUsersResult->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
        
        
        connect(ui.tableViewUsersResult, SIGNAL(clicked(const QModelIndex &)), this, SLOT(slotUserSelected(const QModelIndex &)));
        connect(ui.tableViewUsersResult, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(slotAddUserAsAContact(const QModelIndex &)));
        
        
        
        
}

Search::~Search()
{
    
    reset();
    
    this->disconnect();
    
    
}

void Search::slotSearchContactsResultPacketReceived(const QStringList &users){

    resetUsersInfo();

    //TODO
    //FORMAT:UserID,NickName,Gender,Age,Face,FriendshipApply,BusinessAddress,OnlineState
    
    foreach (QString userInfo, users) {
        QStringList userInfoList = userInfo.split(QString(CONTACT_INFO_SEPARATOR));
        QString userID = userInfoList.at(0);
        Contact *user = new Contact(userID, "", this);
        user->setNickName(userInfoList.at(1));
        user->setGender(Contact::Gender(userInfoList.at(2).toUInt()));
        user->setAge(userInfoList.at(3).toUInt());
        user->setFace(userInfoList.at(4));
        user->setFriendshipApply(IMUserBase::FriendshipApply(userInfoList.at(5).toUInt()));
        user->setBusinessAddress(userInfoList.at(6));
        user->setOnlineState(IM::OnlineState(userInfoList.at(7).toUInt()));
        
        usersHash.insert(userID, user);
    }
    
    userInfoModel->setUsersList(usersHash.values());
    //QMessageBox::information(this, "Result", users.join("|"));
}

void Search::reset(){
    
    resetUsersInfo();
    
    ui.pushButtonCondition->click();
    
    ui.radioButtonUsersMatchExactly->click();
    ui.labelUsersResult->hide();
    ui.lineEditUserID->setFocus();
    ui.radioButtonGroupsMatchExactly->click();
    ui.labelGroupsResult->hide();
    
    ui.lineEditUserID->clear();
    ui.lineEditNickname->clear();
    
    ui.comboBoxGender->setCurrentIndex(0);
    ui.comboBoxAge->setCurrentIndex(0);
    ui.tableViewUsersResult->reset();
    
    //TODO:重置
    ui.tableViewGroupsResult->reset();
    
    
}

void Search::resetUsersInfo(){
    userInfoModel->setUsersList(QList<Contact*>());
    //userInfoModel->clear();
    
    QList<Contact *> users = usersHash.values();
    usersHash.clear();
    foreach (Contact *user, users) {
        delete user;
    }
}

void Search::slotUserSelected(const QModelIndex &index){
    if(!index.isValid()){
        ui.pushButtonUserDetails->setEnabled(false);
        ui.pushButtonAddAsContact->setEnabled(false);
    }else{
        ui.pushButtonUserDetails->setEnabled(true);
        ui.pushButtonAddAsContact->setEnabled(true);
    }
    
}

void Search::slotAddUserAsAContact(const QModelIndex &index){

    if(!index.isValid()){
        return;
    }
    
    ui.pushButtonAddAsContact->click();
    
//    int row = index.row();
//    Contact *user = usersHash.values().at(row);
//    QMessageBox::information(this,"", user->getUserID());
    
}

void Search::on_tabWidget_currentChanged( int index ){
    if(ui.tabWidget->currentWidget() == ui.tabUsers){
        if(ui.stackedWidgetUsers->currentWidget() == ui.pageUsersSearchCondition){
            ui.pushButtonCondition->setEnabled(false);
            ui.pushButtonSearch->setEnabled(true);
        }else{
            ui.pushButtonCondition->setEnabled(true);
            ui.pushButtonSearch->setEnabled(false);
        }
    }else{
        if(ui.stackedWidgetGroups->currentWidget() == ui.pageGroupsSearchCondition){
            ui.pushButtonCondition->setEnabled(false);
            ui.pushButtonSearch->setEnabled(true);
        }else{
            ui.pushButtonCondition->setEnabled(true);
            ui.pushButtonSearch->setEnabled(false);
        }
    }

}

void Search::on_pushButtonCondition_clicked(){
    if(ui.tabWidget->currentWidget() == ui.tabUsers){
        ui.stackedWidgetUsers->setCurrentWidget(ui.pageUsersSearchCondition);
        ui.pushButtonCondition->setEnabled(false);
        ui.pushButtonCondition->setVisible(false);
        ui.pushButtonSearch->setEnabled(true);
        ui.pushButtonSearch->setVisible(true);
        resetUsersInfo();
    }else{
        
        ui.stackedWidgetGroups->setCurrentWidget(ui.pageGroupsSearchCondition);
        ui.pushButtonCondition->setEnabled(false);
        ui.pushButtonCondition->setVisible(false);
        ui.pushButtonSearch->setEnabled(true);
        ui.pushButtonSearch->setVisible(true);
       
    }
}

void Search::on_pushButtonSearch_clicked(){
    if(ui.tabWidget->currentWidget() == ui.tabUsers){

        QStringList propertiesList;
        QString userID = ui.lineEditUserID->text().trimmed();
        QString nickName = ui.lineEditNickname->text();
        quint8 age = IMUserBase::Age_Any;
        if(ui.comboBoxAge->currentIndex() != 0){
            age = ui.comboBoxAge->itemData(ui.comboBoxAge->currentIndex()).toUInt();
            qDebug()<<"----------age"<<age;
        }
        quint8 gender = IMUser::GENDER_UNKNOWN;
        if(ui.comboBoxGender->currentIndex() != 0){
            gender = ui.comboBoxGender->itemData(ui.comboBoxGender->currentIndex()).toUInt();
        }
        QString hometown = ui.lineEditHomeAddress->text().trimmed();
        QString businessAddress = ui.lineEditBusinessAddress->text().trimmed();

        bool matchExactly = ui.radioButtonUsersMatchExactly->isChecked();
        bool searchOnlineUsersOnly = ui.checkBoxUserSearchOnlineUsers->isChecked();
        bool searchWebcamUsersOnly = ui.checkBoxUserSearchWebcamUsers->isChecked();


        if(matchExactly){
            if(!userID.isEmpty()){
                if(userID == IMUser::instance()->getUserID()){
                    //QMessageBox::critical(this, tr("Error"), tr("Please input a valid user id!"));
                    ui.lineEditUserID->clear();
                    return;
                }
                if(IMUser::instance()->isFriendContact(userID)){
                    QMessageBox::critical(this, tr("Error"), tr("'%1' is already your contact!").arg(userID));
                    ui.lineEditUserID->clear();
                    return;
                }
            }
            searchOnlineUsersOnly = searchWebcamUsersOnly = false;
        }

        propertiesList << userID << nickName << QString::number(age) << QString::number(gender) <<hometown << businessAddress;

        
//        if(propertiesList.isEmpty()){
//            QMessageBox::critical(this, tr("Error"), tr("Search Condition Required!"));
//            return;
//        }else{
            emit signalSearchContact(propertiesList.join(QString(CONTACT_INFO_SEPARATOR)), matchExactly, searchOnlineUsersOnly, searchWebcamUsersOnly);
//        }
        
        
        ui.stackedWidgetUsers->setCurrentWidget(ui.pageUsersSearchResult);
        ui.pushButtonCondition->setEnabled(true);
        ui.pushButtonCondition->setVisible(true);
        ui.pushButtonSearch->setEnabled(false); 
        ui.pushButtonSearch->setVisible(false);
        
    }else{
        ui.stackedWidgetGroups->setCurrentWidget(ui.pageGroupsSearchResult);
        ui.pushButtonCondition->setEnabled(true);
        ui.pushButtonCondition->setVisible(true);
        ui.pushButtonSearch->setEnabled(false); 
        ui.pushButtonSearch->setVisible(false);
       
    }

}

void Search::on_radioButtonUsersMatchWildcard_clicked(){
    ui.stackedWidgetUsersSearchCondition->setCurrentWidget(ui.pageUsersSearchMatchWildcard);
}

void Search::on_radioButtonUsersMatchExactly_clicked(){
    ui.stackedWidgetUsersSearchCondition->setCurrentWidget(ui.pageUsersSearchMatchExactly);

}

void Search::on_radioButtonGroupsMatchWildcard_clicked(){
    ui.stackedWidgetGroupsSearchCondition->setCurrentWidget(ui.pageGroupsSearchMatchWildcard);
}

void Search::on_radioButtonGroupsMatchExactly_clicked(){
    ui.stackedWidgetGroupsSearchCondition->setCurrentWidget(ui.pageGroupsSearchMatchExactly);
}

void Search::on_pushButtonUserDetails_clicked(){
    QModelIndex index = ui.tableViewUsersResult->currentIndex();
    if(!index.isValid()){
        return;
    }
    int row = index.row();
    
    Contact *user = usersHash.values().at(row);
    QMessageBox::information(this,"", user->getUserID());
    
    
}

void Search::on_pushButtonAddAsContact_clicked(){
    QModelIndex index = ui.tableViewUsersResult->currentIndex();
    if(!index.isValid()){
        return;
    }
    int row = index.row();
    
    Contact *user = usersHash.values().at(row);
    QString userID = user->getUserID();
    if(userID == IMUser::instance()->getUserID()){
        QMessageBox::critical(this, tr("Error"), tr("You can't add yourself as a contact!"));
        return;
    }
    if(IMUser::instance()->isFriendContact(userID)){
        QMessageBox::critical(this, tr("Error"), tr("'%1' was already your contact!").arg(userID));
        return;
    }
    
    AddContactDialog dlg(user, true, this);
    if(dlg.exec()  == QDialog::Accepted){
        emit signalAddContact(user->getUserID(), dlg.getMessage());
    }
    
    
//    if(user->getFriendshipApply() == Contact::FA_AUTO_ACCEPT){
    
//    }
    
    
//    QMessageBox::information(this,"", user->getUserID());
    
    
}

















} //namespace HEHUI



