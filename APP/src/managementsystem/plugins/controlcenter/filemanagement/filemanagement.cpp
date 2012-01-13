#include "filemanagement.h"


#include <QMessageBox>
#include <QDateTime>
#include <QUrl>
#include <QDebug>



namespace HEHUI {


FileSystemModel::FileSystemModel(QFileIconProvider *fileIconProvider, QObject *parent)
    :QAbstractTableModel(parent), m_fileIconProvider(fileIconProvider)
{
    // TODO Auto-generated constructor stub

    Q_ASSERT(m_fileIconProvider);

    m_currentDirPath = "";

//    setHeaderData(1, Qt::Horizontal, Qt::AlignRight, Qt::TextAlignmentRole);



}

FileSystemModel::~FileSystemModel() {

    beginResetModel();
    foreach (FileItemInfo *info, fileItems) {
        delete info;
    }
    this->fileItems.clear();
    endResetModel();

}

//void FileSystemModel::setFileItems(QList<FileItemInfo> fileItems)
//{
//    beginResetModel();
//    this->fileItems = fileItems;
//    endResetModel();

//}

void FileSystemModel::addFileItem(const QString &name, const QString &size, quint8 type, const QString &dateModified){

    foreach (FileItemInfo *info, fileItems) {
        if(info->name == name){
            return;
        }
    }

    FileItemInfo *info = new FileItemInfo();
    info->name = name;
    info->size = size;
    info->type = type;
    info->dateModified = dateModified;

    beginResetModel();
    this->fileItems.append(info);
    endResetModel();

}

void FileSystemModel::deleteFileItem(const QString &name){

//    for(int i=0; i<fileItems.size(); i++){
//        FileItemInfo *info = fileItems.at(i);
//        if(info->name == name){
//            beginResetModel();
//            fileItems.removeAll(info);
//            endResetModel();
//        }
//    }

    foreach (FileItemInfo *info, fileItems) {
        if(info->name == name){
            beginResetModel();
            delete info;
            fileItems.removeAll(info);
            endResetModel();
        }
    }

}
void FileSystemModel::deleteFileItem(const QModelIndex &index){

    if(!index.isValid()){
        return;
    }

    int row = index.row();
    if((row < 0) || (row >= fileItems.size())){
        return;
    }

    FileItemInfo *info = fileItems.at(row);

    beginResetModel();
    delete info;
    this->fileItems.removeAll(info);
    endResetModel();


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

    FileItemInfo *info = fileItems.at(row);
    //FileItemInfo *info = static_cast<FileItemInfo *> (fileItems.at(row));
    if(role == Qt::DisplayRole || role == Qt::EditRole){
            switch (index.column()) {
            case 0:
                return info->name;
                break;
            case 1:
                return info->size;
                break;
            case 2:
            {
                quint8 type = info->type;
                if(type == quint8(MS::DRIVE)){
                    return tr("Drive");
                }else if(type == quint8(MS::FOLDER)){
                    return tr("Folder");
                }else{
                    return tr("File");
                }
            }
                break;
            case 3:
                return info->dateModified;
                break;

            default:
                return QVariant();
                break;
            }
    }else if(role == Qt::DecorationRole){
        if(index.column() == 0){
            QIcon icon;
            quint8 type = info->type;
            if(type == quint8(MS::DRIVE)){
                icon = m_fileIconProvider->icon(QFileIconProvider::Drive);
            }else if(type == quint8(MS::FOLDER)){
                icon = m_fileIconProvider->icon(QFileIconProvider::Folder);
            }else{
                icon = m_fileIconProvider->icon(QFileIconProvider::File);
            }
            return icon;
        }
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


//    if(role == Qt::DisplayRole){
//        if(orientation ==  Qt::Horizontal){
//                switch (section) {
//                case 0:
//                    return QString(tr("Name"));
//                    break;
//                case 1:
//                    return QString(tr("Size"));
//                    break;
//                case 2:
//                    return QString(tr("Type"));
//                    break;
//                case 3:
//                    return QString(tr("Date Modified"));
//                    break;
//                default:
//                    return QVariant();
//                    break;
//                }
//        }
//    }else if(role == Qt::TextAlignmentRole){
//        if(orientation ==  Qt::Horizontal){
//                switch (section) {
//                case 0:
//                    return Qt::AlignRight;
//                    break;
//                default:
//                    return Qt::AlignLeft;
//                    break;
//                }
//        }

//    }


    return QVariant();

}

bool FileSystemModel::parseRemoteFilesInfo(const QString &remoteParentDirPath, const QByteArray &data){

    if(m_currentDirPath != remoteParentDirPath){
        return false;
    }

    QList<FileItemInfo *> items;

    QDataStream in(data);
    in.setVersion(QDataStream::Qt_4_8);

    QString parentDirPath = "";


    in >> parentDirPath;

    while (!in.atEnd()) {
        QString name = "";
        qint64 size = 0;
        quint8 type = quint8(MS::FILE);
        uint lastModified = 0;

        in >> name >> size >> type >> lastModified;
        if(name.isEmpty()){
            continue;
        }
        FileItemInfo *info = new FileItemInfo();
        info->name = name;

        QString sizeString = QString::number(size);
        if(size <= 0){
            sizeString = "";
        }else if(size < 1024){
            sizeString = QString::number(size) + "Byte";
        }else if(size < 1024*1024){
            sizeString = QString::number(size/1024) + "KB";
        }else{
            sizeString = QString::number(size/(1024*1024)) + "MB";
        }
        info->size = sizeString;

//        if(type == quint8(DRIVE)){
//            info.type = tr("Drive");
//        }else if(type == quint8(FOLDER)){
//            info.type = tr("Folder");
//        }else{
//            info.type = tr("File");
//        }
        info->type = type;

        QDateTime time = QDateTime::fromTime_t(lastModified);
        info->dateModified = time.toString("yyyy/MM/dd hh:mm:ss");

        items.append(info);
        qDebug()<<"name:"<<name<<" size:"<<size<<" type:"<<type<<" lastModified:"<<lastModified;

    }

//    setFileItems(items);


    beginResetModel();
    foreach (FileItemInfo *info, fileItems) {
        delete info;
    }
    this->fileItems.clear();
    this->fileItems = items;
    endResetModel();


    return true;
}

bool FileSystemModel::isDrive(const QModelIndex & index){

    if(!index.isValid()){
        return false;
    }

    int row = index.row();
    if((row < 0) || (row >= fileItems.size())){
        return false;
    }

    FileItemInfo *info = fileItems.at(row);
    //FileItemInfo *info = static_cast<FileItemInfo *> (fileItems.at(row));


    return (info->type == quint8(MS::DRIVE));

//    int column = index.column();
//    if(column != 0){
//        QModelIndex idx = index.sibling(index.row(), 0);
//    }

}

bool FileSystemModel::isDir(const QModelIndex & index){

    if(!index.isValid()){
        return false;
    }

    int row = index.row();
    if((row < 0) || (row >= fileItems.size())){
        return false;
    }

    FileItemInfo *info = fileItems.at(row);
    //FileItemInfo *info = static_cast<FileItemInfo *> (fileItems.at(row));


    return (info->type != quint8(MS::FILE));

//    int column = index.column();
//    if(column != 0){
//        QModelIndex idx = index.sibling(index.row(), 0);
//    }

}

QString FileSystemModel::absoluteFilePath(const QModelIndex &index){

    if(!index.isValid()){
        return "";
    }

    int row = index.row();
    if((row < 0) || (row >= fileItems.size())){
        return "";
    }

    FileItemInfo *info = fileItems.at(row);
    if(!info){
        return "";
    }



    QString path;
    if(m_currentDirPath.isEmpty() || m_currentDirPath.endsWith("/") || m_currentDirPath.endsWith("\\")){
        path = m_currentDirPath + info->name;
    }else{
        path = m_currentDirPath + "/" + info->name;
    }
    QDir dir(path);
    return dir.absolutePath();

}

void FileSystemModel::changePath(const QString &newPath){

    m_currentDirPath = newPath;

    beginResetModel();
    this->fileItems.clear();
    endResetModel();

}

QString FileSystemModel::currentDirPath() const{
    return m_currentDirPath;
}






/////////////////////////////////////////////
FileManagement::FileManagement(QWidget *parent) :
    QWidget(parent)
{
    ui.setupUi(this);


    localFileSystemModel = 0;
    localFilesCompleter = 0;
    m_localCurrentDir = "";

    remoteFileSystemModel = 0;

    connect(ui.tableViewLocalFiles, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(localFileItemDoubleClicked(const QModelIndex &)));
    connect(ui.tableViewRemoteFiles, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(tableViewRemoteFileItemDoubleClicked(const QModelIndex &)));

//    QHeaderView *header = ui.tableViewRemoteFiles->horizontalHeader();


}

void FileManagement::dragEnterEvent(QDragEnterEvent *event){

    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void FileManagement::dragMoveEvent(QDragMoveEvent *event)
{
    // Accept file actions with all extensions.

//    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
//    }

}

void FileManagement::dropEvent(QDropEvent *event)
{

    // Accept drops if the file exists.

    const QMimeData *mimeData = event->mimeData();
    QStringList files;
    if (mimeData->hasUrls()) {
        QList<QUrl> urlList = mimeData->urls();
        foreach (QUrl url, urlList) {
            if (url.isValid() && url.scheme().toLower() == "file" ){
                QString fileName = url.path().remove(0, 1);
                //QFileInfo fi(fileName);
                //QMessageBox::information(this, fileName, fileName);
                if (QFile::exists(fileName)){
                    files.append(fileName);
                }
            }
        }

    }

    event->acceptProposedAction();

    if (!files.isEmpty()){
        emit signalUploadFilesToRemote(files, ui.comboBoxRemotePath->currentText());
    }

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

        if(ui.groupBoxRemote->isChecked()){
            ui.pushButtonDownloadToLocal->setEnabled(true);
            ui.pushButtonUploadToRemote->setEnabled(true);
        }

    }else{
        ui.pushButtonDownloadToLocal->setEnabled(false);
        ui.pushButtonUploadToRemote->setEnabled(false);
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
    ui.tableViewLocalFiles->clearSelection();

    m_localCurrentDir = path;

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
    ui.tableViewLocalFiles->clearSelection();

    m_localCurrentDir = newPath;


}

void FileManagement::on_groupBoxRemote_toggled( bool on ){

    if(on){
        if(!remoteFileSystemModel){
            if(!ui.groupBoxLocal->isChecked()){
                ui.groupBoxLocal->setChecked(true);
                //on_groupBoxLocal_toggled();
            }

            remoteFileSystemModel = new FileSystemModel(localFileSystemModel->iconProvider(), this);
            ui.tableViewRemoteFiles->setModel(remoteFileSystemModel);
        }

        emit signalShowRemoteFiles("");

        if(ui.groupBoxLocal->isChecked()){
            ui.pushButtonDownloadToLocal->setEnabled(true);
            ui.pushButtonUploadToRemote->setEnabled(true);
        }

    }else{
        ui.pushButtonDownloadToLocal->setEnabled(false);
        ui.pushButtonUploadToRemote->setEnabled(false);
    }

}

void FileManagement::on_toolButtonShowRemoteFiles_clicked(){

    QString newPath = ui.comboBoxRemotePath->currentText();
    emit signalShowRemoteFiles(newPath);
    remoteFileSystemModel->changePath(newPath);
    ui.tableViewRemoteFiles->clearSelection();
}

void FileManagement::tableViewRemoteFileItemDoubleClicked(const QModelIndex &index){

    if(!index.isValid()){
        return;
    }

    if(!remoteFileSystemModel->isDir(index)){
        return;
    }

    QString newPath = remoteFileSystemModel->absoluteFilePath(index);

    emit signalShowRemoteFiles(newPath);

    ui.comboBoxRemotePath->setEditText(newPath);

    remoteFileSystemModel->changePath(newPath);

    ui.tableViewRemoteFiles->clearSelection();

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
    QDataStream out(result, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);
    out << parentDirPath;

    foreach (QFileInfo info, infoList) {
        QString name = info.baseName();
        qint64 size = info.size();
        quint8 type = quint8(MS::FILE);
        if(isDrives){
            type = quint8(MS::DRIVE);
        }else{
            if(info.isDir()){
                type = quint8(MS::FOLDER);
            }
        }
        uint lastModified = info.lastModified().toTime_t();

        out << name << size << type << lastModified;

    }

    return true;

}

bool FileManagement::parseRemoteFilesInfo(const QString &remoteParentDirPath, const QByteArray &data){

    return remoteFileSystemModel->parseRemoteFilesInfo(remoteParentDirPath, data);
}

void FileManagement::on_pushButtonUploadToRemote_clicked(){

    QModelIndex index = ui.tableViewLocalFiles->currentIndex();
    if(!index.isValid() || localFileSystemModel->fileInfo(index).isRoot()){
        QMessageBox::critical(this, tr("Error"), tr("Please select one file or folder to upload!"));
        return;
    }

    QString remoteDir = remoteFileSystemModel->currentDirPath();
    if(remoteDir.isEmpty()){
        QMessageBox::critical(this, tr("Error"), tr("Please select the remote path to save files!"));
        return;
    }

    QString filePath = localFileSystemModel->fileInfo(index).absoluteFilePath();
    QStringList files;
    files.append(filePath);
    emit signalUploadFilesToRemote(files, remoteFileSystemModel->currentDirPath());


}

void FileManagement::on_pushButtonDownloadToLocal_clicked(){

    QModelIndex index = ui.tableViewRemoteFiles->currentIndex();
    if(!index.isValid() || remoteFileSystemModel->isDrive(index)){
        QMessageBox::critical(this, tr("Error"), tr("Please select one file or folder to download!"));
        return;
    }

    if(m_localCurrentDir.isEmpty()){
        QMessageBox::critical(this, tr("Error"), tr("Please select the local path to save files!"));
        return;
    }

    QString filePath = remoteFileSystemModel->absoluteFilePath(index);
    QStringList files;
    files.append(filePath);
    emit signalDownloadFileFromRemote(files, m_localCurrentDir);

}

















} //namespace HEHUI

