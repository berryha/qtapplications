

#include <QDesktopWidget>
#include <QMessageBox>

#include "contactinfowidget.h"

#include "../imuser.h"

#include "../imageresource.h"




namespace HEHUI {

ContactInfoWidget::ContactInfoWidget(IMUserBase *user, QWidget *parent, Qt::WindowFlags fl)
    : WidgetBase(parent, fl), m_user(user)
{
    ui.setupUi(this);

    Q_ASSERT_X(m_user, "ContactInfoWidget::ContactInfoWidget(...)", "Invalid IMUserBase!");


    initUI();

    moveWindow(HEHUI::Center);

}

ContactInfoWidget::~ContactInfoWidget()
{

    //QMessageBox::information(this, "~ContactInfoWidget()", "~ContactInfoWidget()");

}

void ContactInfoWidget::languageChange(){

    ui.retranslateUi(this);

    setWindowTitle(tr("%1's Information").arg(m_user->getNickName()));

}

void ContactInfoWidget::closeEvent(QCloseEvent *){
    //QMessageBox::information(this, "closeEvent()", "closeEvent()");

    deleteLater();

}

void ContactInfoWidget::initUI(){
    QString userID = m_user->getUserID();

    ui.lineEditUserID->setText(userID);
    ui.lineEditNickName->setText(m_user->getNickName());
    ui.plainTextEditSignature->appendPlainText("");

    ui.comboBoxGender->setCurrentIndex(m_user->getGender());
    ui.spinBoxAge->setValue(m_user->getAge());
    ui.dateEditBirthday->setDate(m_user->getBirthday());


    //TODO

    ui.pushButtonApply->setEnabled(false);


    if(userID == IMUser::instance()->getUserID()){
        setWindowTitle(tr("My Information"));


    }else{
        setWindowTitle(tr("%1's Information").arg(m_user->getNickName()));

        ui.lineEditNickName->setReadOnly(true);
        ui.plainTextEditSignature->setReadOnly(true);
        ui.comboBoxGender->setEnabled(false);
        ui.spinBoxAge->setReadOnly(true);
        ui.dateEditBirthday->setReadOnly(true);


        ui.toolButtonChangeFace->setEnabled(false);
        ui.pushButtonSettings->hide();
        ui.pushButtonApply->hide();


    }

    setWindowIcon(ImageResource::createIconForContact(m_user->getFace(), IM::ONLINESTATE_ONLINE));

    on_pushButtonGeneralInformation_clicked();


}

void ContactInfoWidget::on_toolButtonUserFace_clicked(){
    //TODO

}

void ContactInfoWidget::on_pushButtonGeneralInformation_clicked(){
    ui.pushButtonGeneralInformation->setChecked(true);
    ui.pushButtonMoreInformation->setChecked(false);

    ui.stackedWidget->setCurrentWidget(ui.pageGeneralInfo);

}

void ContactInfoWidget::on_pushButtonMoreInformation_clicked(){
    ui.pushButtonGeneralInformation->setChecked(false);
    ui.pushButtonMoreInformation->setChecked(true);

    ui.stackedWidget->setCurrentWidget(ui.pageMoreInfo);

}

void ContactInfoWidget::on_pushButtonSettings_clicked(){
    //TODO

}

void ContactInfoWidget::on_pushButtonUpdate_clicked(){
    //TODO

}

void ContactInfoWidget::on_pushButtonOK_clicked(){
    //TODO

}

void ContactInfoWidget::on_pushButtonCancel_clicked(){
    close();
}

void ContactInfoWidget::on_pushButtonApply_clicked(){
    //TODO

}
























}
