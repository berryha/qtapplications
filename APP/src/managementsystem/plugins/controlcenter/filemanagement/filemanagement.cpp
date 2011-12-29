#include "filemanagement.h"


#include <QMessageBox>


namespace HEHUI {


FileManagement::FileManagement(QWidget *parent) :
    QWidget(parent)
{
    ui.setupUi(this);


    localFileSystemModel = 0;
    localFilesCompleter = 0;


    connect(ui.tableViewLocalFiles, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(localFileItemDoubleClicked(const QModelIndex &)));


}

void FileManagement::on_groupBoxLocal_toggled( bool on ){

    if(on){
        if(!localFileSystemModel){
            localFileSystemModel = new QFileSystemModel(this);
            localFileSystemModel->setFilter(QDir::AllEntries | QDir::NoDot);
//            localFileSystemModel->setRootPath(QDir::currentPath());
            localFileSystemModel->setRootPath("");

            ui.tableViewLocalFiles->setModel(localFileSystemModel);
//            ui.tableViewLocalFiles->setRootIndex(localFileSystemModel->index(QDir::currentPath()));

            localFilesCompleter = new QCompleter(this);
            localFilesCompleter->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
            localFilesCompleter->setModel(localFileSystemModel);
            ui.comboBoxLocalPath->setCompleter(localFilesCompleter);
        }
    }
}

void FileManagement::on_toolButtonShowLocalFiles_clicked(){

    QString path = ui.comboBoxLocalPath->currentText();

    if(!path.isEmpty() && !QFile::exists(path)){
        QMessageBox::critical(this, tr("Error"), tr("Target path does not exist!"));
        ui.comboBoxLocalPath->setEditText(localFileSystemModel->fileInfo(ui.tableViewLocalFiles->rootIndex()).absoluteFilePath() );
        return;
    }

    ui.tableViewLocalFiles->setRootIndex(localFileSystemModel->index(path));

}

void FileManagement::localFileItemDoubleClicked(const QModelIndex &index){

    if(!index.isValid()){
        return;
    }

    if(!localFileSystemModel->isDir(index)){
        return;
    }
    QString newPath = localFileSystemModel->fileInfo(index).absoluteFilePath();
    ui.comboBoxLocalPath->setEditText(newPath);

    ui.tableViewLocalFiles->setRootIndex(localFileSystemModel->index(newPath));

}

void FileManagement::on_groupBoxRemote_toggled( bool on ){

}
















} //namespace HEHUI

