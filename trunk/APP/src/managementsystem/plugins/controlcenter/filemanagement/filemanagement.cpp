#include "filemanagement.h"


#include <QMessageBox>


namespace HEHUI {


FileSystemModel::FileSystemModel(QFileIconProvider *fileIconProvider, QObject *parent)
    :QAbstractTableModel(parent), m_fileIconProvider(fileIconProvider)
{
    // TODO Auto-generated constructor stub

    Q_ASSERT(m_fileIconProvider);

    currentDirPath = "";

}

FileSystemModel::~FileSystemModel() {
    fileItems.clear();
}

void FileSystemModel::setFileItems(QList<FileItemInfo> fileItems)
{
    beginResetModel();
    this->fileItems = fileItems;
    endResetModel();

}

void FileSystemModel::addFileItem(const QString &name, const QString &size, const QString &type, const QString &dateModified){

    FileItemInfo info;
    info.name = name;
    info.size = size;
    info.type = type;
    info.dateModified = dateModified;

    if(!fileItems.contains(info)){
        beginResetModel();
        this->fileItems.append(clientInfo);
        endResetModel();
    }

}

void FileSystemModel::deleteFileItem(const QString &name){

    foreach (FileItemInfo info, fileItems) {
        if(info.name == name){
            beginResetModel();
            this->fileItems.removeAll(info);
            endResetModel();
        }
    }

}

int FileSystemModel::rowCount ( const QModelIndex & parent) const {
    if(parent.isValid()){
        return 0;
    }
    return fileItems.size();

}

int FileSystemModel::columnCount ( const QModelIndex & parent) const{
    if(parent.isValid()){
        return 0;
    }

    return 4;


}

QVariant FileSystemModel::data ( const QModelIndex & index, int role) const{
    if(!index.isValid()){
        return QVariant();
    }

    int row = index.row();
    if((row < 0) || (row >= fileItems.size())){
        return QVariant();
    }

    FileItemInfo info = static_cast<FileItemInfo> (fileItems.at(row));
    if(role == Qt::DisplayRole || role == Qt::EditRole){
            switch (index.column()) {
            case 0:
                return info.name;
                break;
            case 1:
                return info.size;
                break;
            case 2:
                return info.type;
                break;
            case 3:
                return info.dateModified;
                break;

            default:
                return QVariant();
                break;
            }
    }else if(role == Qt::DecorationRole){
        QIcon icon;
        QString type = info.type;
        if(type == "Drive"){
            icon = m_fileIconProvider->icon(QFileIconProvider::Drive);
        }else if(type == "File Folder"){
            icon = m_fileIconProvider->icon(QFileIconProvider::Folder);
        }else{
            icon = m_fileIconProvider->icon(QFileIconProvider::File);
        }
        reurrn icon;

    }

    return QVariant();

}

QVariant FileSystemModel::headerData ( int section, Qt::Orientation orientation, int role) const{
    if(role != Qt::DisplayRole){
        return QVariant();
    }

    if(orientation ==  Qt::Horizontal){
            switch (section) {
            case 0:
                return QString(tr("Name"));
                break;
            case 1:
                return QString(tr("Size"));
                break;
            case 2:
                return QString(tr("Type"));
                break;
            case 3:
                return QString(tr("Date Modified"));
                break;
            default:
                return QVariant();
                break;
            }

    }

    return QVariant();

}


/////////////////////////////////////////////
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
            //localFileSystemModel->setRootPath(QDir::currentPath());
            localFileSystemModel->setRootPath("");

            ui.tableViewLocalFiles->setModel(localFileSystemModel);
            //ui.tableViewLocalFiles->setRootIndex(localFileSystemModel->index(QDir::currentPath()));

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

    if(on){
        emit signalShowRemoteFiles("");
    }

}

void FileManagement::on_toolButtonShowRemoteFiles_clicked(){
    emit signalShowRemoteFiles(ui.comboBoxRemotePath->currentText());
}

void FileManagement::tableViewRemoteFileItemDoubleClicked(const QModelIndex &index){

    if(!index.isValid()){
        return;
    }
    //TODO:

//    if(!localFileSystemModel->isDir(index)){
//        return;
//    }
//    QString newPath = localFileSystemModel->fileInfo(index).absoluteFilePath();
//    ui.comboBoxRemotePath->setEditText(newPath);

//    ui.tableViewRemoteFiles->setRootIndex(localFileSystemModel->index(newPath));

}

bool FileManagement::getLocalFilesInfo(const QString &parentDirPath, QByteArray *result, QString *errorMessage){

    Q_ASSERT(result);
    Q_ASSERT(errorMessage);

    QFileInfoList infoList;
    bool isDrives = false;

    if(parentDirPath.isEmpty()){
        infoList = QDir::drives();
        isDrives = true;
    }else{
        QFileInfo fi(parentDirPath);
        if(!fi.isDir()){
            *errorMessage = tr("'%1' is not a directorie!").arg(parentDirPath);
            return false;
        }
        QDir dir(parentDirPath);
        if(!dir.exists()){
            *errorMessage = tr("Directorie '%1' does not exist!").arg(parentDirPath);
            return false;
        }
        dir.setFilter(QDir::AllEntries | QDir::NoDot | QDir::Hidden | QDir::NoSymLinks);
        infoList = dir.entryInfoList();
    }

    if(infoList.isEmpty()){
        //TODO
        return false;
    }

    result->clear();
    QDataStream out(result);
    out.setVersion(QDataStream::Qt_4_8);
    out << parentDirPath;

    foreach (QFileInfo info, infoList) {
        QString name = info.baseName();
        qint64 size = info.size();
        quint8 type = quint8(FILE);
        if(isDrives){
            type = quint8(DRIVE);
        }else{
            if(info.isDir()){
                type = quint8(FOLDER);
            }
        }
        uint lastModified = info.lastModified();

        out << name << size << type << lastModified;

    }

    return true;



}

bool FileManagement::parseRemoteFilesInfo(const QString &remoteParentDirPath, const QByteArray &data){

    QDataStream in(&data);
    in.setVersion(QDataStream::Qt_4_8);

    QString parentDirPath = "";
    QString name = "";
    qint64 size = 0;
    quint8 type = quint8(FILE);
    uint lastModified = 0;

    in >> parentDirPath;

    while (!in.atEnd()) {
        in >> name >> size >> type >> lastModified;
        if(name.isEmpty()){
            continue;
        }

    }


}















} //namespace HEHUI

