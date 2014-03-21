#include "filetransmissionlistwidget.h"
#include "ui_filetransmissionlistwidget.h"



namespace HEHUI {


FileTransmissionListWidget::FileTransmissionListWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileTransmissionListWidget)
{
    ui->setupUi(this);

    ui->frame->hide();

}

FileTransmissionListWidget::~FileTransmissionListWidget()
{
    delete ui;
}

void FileTransmissionListWidget::slotFileRequestReceivedFromContact(const QString &fileName, qint64 size, const QString &fileMD5){
    ProgressInfoWidget *wgt = createProgressInfoWidgetItem(fileMD5);
    wgt->requestToReceiveFile(fileName, size, fileMD5);
}

void FileTransmissionListWidget::slotSendFileRequestToContact(const QString &fileName, const QString &fileMD5){
    ProgressInfoWidget *wgt = createProgressInfoWidgetItem(fileMD5);
    wgt->requestToSendFile(fileName, fileMD5);
}

//void FileTransmissionListWidget::slotFileRequestCanceledByContact(const QString &fileMD5){
//    slotCloseProgressInfoWidget(fileMD5);
//}

//void FileTransmissionListWidget::slotFileTransmissionAbortedByContact(const QString &fileMD5){
//    slotCloseProgressInfoWidget(fileMD5);
//}

void FileTransmissionListWidget::slotCloseProgressInfoWidget(const QString &fileMD5){

    ProgressInfoWidget *wgt = progressInfoWidgetHash.take(fileMD5);
    if(!wgt){return;}

    int row = findListWidgetItemIndex(wgt);
    if(row < 0){return;}

    wgt->disconnect();
    delete wgt;
    wgt = 0;
    QListWidgetItem * item = ui->listWidget->takeItem(row);
    delete item;

    if(progressInfoWidgetHash.isEmpty()){
        emit signlaCloseWidget();
    }

}

void FileTransmissionListWidget::slotCancelSendingFileRequest(const QString &fileMD5){
    emit cancelSendingFileRequest(fileMD5);
    slotCloseProgressInfoWidget(fileMD5);
}

void FileTransmissionListWidget::slotAbortFileTransmission(const QString &fileMD5){
    emit abortFileTransmission(fileMD5);
    slotCloseProgressInfoWidget(fileMD5);
}

void FileTransmissionListWidget::slotAcceptFileRequest(const QString &fileMD5, const QString &localSavePath){
    emit acceptFileRequest(fileMD5, localSavePath);
}

void FileTransmissionListWidget::slotDeclineFileRequest(const QString &fileMD5){
    emit declineFileRequest(fileMD5);
    slotCloseProgressInfoWidget(fileMD5);
}

void FileTransmissionListWidget::updateFileTransmissionProgress(const QString &fileMD5, int percent){

    if(percent == 100){
        slotCloseProgressInfoWidget(fileMD5);
        return;
    }

    ProgressInfoWidget *wgt = progressInfoWidgetHash.value(fileMD5);
    if(!wgt){return;}
    wgt->updateProgress(percent);

}

int FileTransmissionListWidget::findListWidgetItemIndex(ProgressInfoWidget *wgt){

    if(!wgt){return -1;}

    for(int i=0; i<ui->listWidget->count(); i++){
        QListWidgetItem *item = ui->listWidget->item(i);
        if(!item){continue;}
        if(ui->listWidget->itemWidget(item) == wgt){
            return ui->listWidget->row(item);
        }
    }

    return -1;

}

ProgressInfoWidget * FileTransmissionListWidget::createProgressInfoWidgetItem(const QString &fileMD5){

    if(progressInfoWidgetHash.keys().contains(fileMD5)){
        return progressInfoWidgetHash.value(fileMD5);
    }

    ProgressInfoWidget *wgt = new ProgressInfoWidget(this);
    connect(wgt, SIGNAL(cancelSendingFileRequest(QString)), this, SLOT(slotCancelSendingFileRequest(QString)));
    connect(wgt, SIGNAL(abortFileTransmission(QString)), this, SLOT(slotAbortFileTransmission(QString)));
    connect(wgt, SIGNAL(acceptFileRequest(QString, QString)), this, SLOT(slotAcceptFileRequest(QString, QString)));
    connect(wgt, SIGNAL(declineFileRequest(QString)), this, SLOT(slotDeclineFileRequest(QString)));

    QListWidgetItem * item = new QListWidgetItem(ui->listWidget);
    item->setSizeHint(QSize(200, 100));
    ui->listWidget->addItem(item);
    ui->listWidget->setItemWidget(item, wgt);
    progressInfoWidgetHash.insert(fileMD5, wgt);


    return wgt;

}


} //namespace HEHUI
