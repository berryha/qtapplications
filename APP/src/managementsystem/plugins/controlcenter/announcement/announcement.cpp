


#include <QMessageBox>


#include "announcement.h"




namespace HEHUI {


Announcement::Announcement(QTableView *tableView, const QString &adminName, ControlCenterPacketsParser *controlCenterPacketsParser, QWidget *parent)
    : QWidget(parent), m_tableView(tableView), m_adminName(adminName), m_controlCenterPacketsParser(controlCenterPacketsParser)
{
    ui.setupUi(this);

    ui.progressBar->hide();
    ui.comboBoxWorkgroup->hide();

    ui.lineEditAnnouncementID->setText(QDateTime::currentDateTime().toString("Mddhhmmss"));

    if(!tableView->model()->rowCount()){
        ui.comboBoxTargetType->setCurrentIndex(2);
    }else if (!tableView->selectionModel()->selectedRows().count()) {
        ui.comboBoxTargetType->setCurrentIndex(1);

    }

}

Announcement::~Announcement()
{

} 

void Announcement::on_pushButtonSend_clicked(){
    
    QString announcementString = ui.textEdit->toPlainText();
    if(announcementString.trimmed().isEmpty()){
        QMessageBox::critical(this, tr("Error"), tr("Invalid Content!"));
        return;
    }
    
    int ret = QMessageBox::question(this, tr("Confirm"), tr("Do you really want to send the announcement?"), QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Cancel);
    if(ret == QMessageBox::Cancel){
        return;
    }
    
    ui.pushButtonSend->setEnabled(false);
    
    
    QApplication::setOverrideCursor(Qt::WaitCursor);


    int targetTypeIndex = ui.comboBoxTargetType->currentIndex();
    
    if(targetTypeIndex == 1){
        m_tableView->selectAll();
    }
    
    quint32 announcementID = ui.lineEditAnnouncementID->text().toUInt();
    
    if(targetTypeIndex != 2){
        QModelIndexList selectedIndexes = m_tableView->selectionModel()->selectedRows();
        
        int selectedIndexesCount = selectedIndexes.count();
        
        ui.progressBar->show();
        ui.progressBar->setRange(0, selectedIndexesCount);
        
        for (int j = 0; j < selectedIndexesCount; j++) {
            QModelIndex index = selectedIndexes.at(j);
            int row = index.row();

            QString computerName = index.sibling(row,0).data().toString();
            QString workgroup = index.sibling(row,1).data().toString();
            QStringList networkInfoList = index.sibling(row,2).data().toString().split(",");
            foreach (QString info, networkInfoList) {
                if(info.trimmed().isEmpty()){continue;}
                m_controlCenterPacketsParser->sendAnnouncementPacket(info.split("/").at(0), IP_MULTICAST_GROUP_PORT, workgroup, computerName, announcementID, announcementString, m_adminName);
            }
            
            
            ui.progressBar->setValue(j);
            qApp->processEvents();
            
        }
        
        
        
        //    }else if(targetTypeIndes == 1){
        
        //        int rowCount = model->rowCount();
        
        //        ui.progressBar->show();
        //        ui.progressBar->setRange(0, rowCount);
        
        //        for (int i = 0; i < rowCount; ++i) {
        //            QModelIndex index = model->index(i, 0);
        //            int row = index.row();
        //            QString computerName = index.sibling(row,0).data().toString();
        //            QString workgroup = index.sibling(row,1).data().toString();
        //            QString networkInfoList = index.sibling(row,2).data().toString().split(",");
        //            foreach (QString info, networkInfoList) {
        //                if(info.trimmed().isEmpty()){continue;}
        //                m_controlCenterPacketsParser->sendAnnouncementPacket(info.split("/").at(0), workgroup, computerName, announcementString, m_adminName);
        //            }


        //            ui.progressBar->setValue(i);
        //            qApp->processEvents();

        //        }
        
        
    }else{
        m_controlCenterPacketsParser->sendAnnouncementPacket("", IP_MULTICAST_GROUP_PORT, ui.comboBoxWorkgroup->currentText(), "", announcementID, announcementString, m_adminName);
        
    }



    ui.progressBar->reset();
    ui.progressBar->hide();
    QApplication::restoreOverrideCursor();
    
    ui.pushButtonSend->setEnabled(true);
    
    
    

    
}


void Announcement::on_comboBoxTargetType_currentIndexChanged ( int index ){
    
    if(index == 2){
        ui.comboBoxWorkgroup->show();
    }else{
        ui.comboBoxWorkgroup->hide();
    }
    
}











} //namespace HEHUI
