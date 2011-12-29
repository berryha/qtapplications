#include "filemanagement.h"





namespace HEHUI {


FileManagement::FileManagement(QWidget *parent) :
    QWidget(parent)
{
    ui.setupUi(this);


    localFileSystemModel = 0;


    connect(ui.tableViewLocalFiles, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(localFileItemDoubleClicked(const QModelIndex &)))


}

void FileManagement::on_groupBoxLocal_toggled( bool on ){

    if(on){
        if(!localFileSystemModel){
            localFileSystemModel = new QFileSystemModel(this);
            localFileSystemModel->setFilter(QDir::AllEntries | QDir::NoDot);
            localFileSystemModel->setRootPath(QDir::currentPath());
            ui.tableViewLocalFiles->setModel(localFileSystemModel);
            ui.tableViewLocalFiles->setRootIndex(localFileSystemModel->index(QDir::currentPath()));
        }
    }
}

void FileManagement::localFileItemDoubleClicked(const QModelIndex &index){

}

void FileManagement::on_groupBoxRemote_toggled( bool on ){

}

void FileManagement::on_toolButtonShowLocalFiles_clicked(){

    QString path = ui.comboBoxLocalPath->currentText();
    if(path.isEmpty()){

    }

}














} //namespace HEHUI

