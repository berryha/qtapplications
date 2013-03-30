

#include <QMessageBox>
#include <QDateTime>
#include <QMimeData>
#include <QUrl>
#include <QDebug>

#include "filemanagement.h"


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

QString FileSystemModel::fileName(const QModelIndex &index){

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

    return info->name;;


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


//    m_udtProtocol = 0;
    m_peerSocket = INVALID_SOCK_ID;
//    m_udtProtocolForFileTransmission = 0;
//    m_peerFileTransmissionSocket = INVALID_SOCK_ID;

    m_fileManager = 0;



}

FileManagement::~FileManagement(){
    //TODO:
//    fileSavePathMap.clear();

    if(localFileSystemModel){
        delete localFileSystemModel;
        localFileSystemModel = 0;
    }

    if(localFilesCompleter){
        delete localFilesCompleter;
        localFilesCompleter = 0;
    }

    if(remoteFileSystemModel){
        delete remoteFileSystemModel;
        remoteFileSystemModel = 0;
    }

    if(m_fileManager){
        m_fileManager->exit();
        delete m_fileManager;
        m_fileManager = 0;
    }

    fileTXRequestList.clear();
    filesList.clear();


}

void FileManagement::setPacketsParser(ControlCenterPacketsParser *parser){

    //TODO:
    Q_ASSERT(parser);
    controlCenterPacketsParser = parser;

    ////////////////////
    connect(controlCenterPacketsParser, SIGNAL(signalFileSystemInfoReceived(int, const QString &, const QByteArray &)), this, SLOT(fileSystemInfoReceived(int, const QString &, const QByteArray &)));
    //File TX
    connect(controlCenterPacketsParser, SIGNAL(signalAdminRequestUploadFile(int, const QByteArray &, const QString &, quint64, const QString &)), this, SLOT(processPeerRequestUploadFilePacket(int, const QByteArray &, const QString &,quint64, const QString &)), Qt::QueuedConnection);
    connect(controlCenterPacketsParser, SIGNAL(signalAdminRequestDownloadFile(int, const QString &, const QString &, const QString &)), this, SLOT(processPeerRequestDownloadFilePacket(int, const QString &, const QString &, const QString &)), Qt::QueuedConnection);

    connect(controlCenterPacketsParser, SIGNAL(signalFileDownloadRequestAccepted(int, const QString &, const QByteArray &, quint64, const QString &)), this, SLOT(fileDownloadRequestAccepted(int, const QString &, const QByteArray &, quint64, const QString &)), Qt::QueuedConnection);
    connect(controlCenterPacketsParser, SIGNAL(signalFileDownloadRequestDenied(int , const QString &, const QString &)), this, SLOT(fileDownloadRequestDenied(int , const QString &, const QString &)), Qt::QueuedConnection);
    connect(controlCenterPacketsParser, SIGNAL(signalFileUploadRequestResponsed(int, const QByteArray &, bool, const QString &)), this, SLOT(fileUploadRequestResponsed(int, const QByteArray &, bool, const QString &)), Qt::QueuedConnection);

    connect(controlCenterPacketsParser, SIGNAL(signalFileDataRequested(int, const QByteArray &, int, int )), this, SLOT(processFileDataRequestPacket(int,const QByteArray &, int, int )), Qt::QueuedConnection);
    connect(controlCenterPacketsParser, SIGNAL(signalFileDataReceived(int, const QByteArray &, int, const QByteArray &, const QByteArray &)), this, SLOT(processFileDataReceivedPacket(int, const QByteArray &, int, const QByteArray &, const QByteArray &)), Qt::QueuedConnection);
    connect(controlCenterPacketsParser, SIGNAL(signalFileTXStatusChanged(int, const QByteArray &, quint8)), this, SLOT(processFileTXStatusChangedPacket(int, const QByteArray &, quint8)), Qt::QueuedConnection);
    connect(controlCenterPacketsParser, SIGNAL(signalFileTXError(int , const QByteArray &, quint8 , const QString &)), this, SLOT(processFileTXErrorFromPeer(int , const QByteArray &, quint8 , const QString &)), Qt::QueuedConnection);




}

void FileManagement::setPeerSocket(UDTSOCKET peerSocket){
    this->m_peerSocket = peerSocket;
    //TODO

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
//        emit signalUploadFilesToRemote(files, ui.comboBoxRemotePath->currentText());
        requestUploadFilesToRemote("", files, remoteFileSystemModel->currentDirPath());
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

//        emit signalShowRemoteFiles("");
        requestFileSystemInfo("");

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
//    emit signalShowRemoteFiles(newPath);
    requestFileSystemInfo(newPath);

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

//    emit signalShowRemoteFiles(newPath);
    requestFileSystemInfo(newPath);

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


void FileManagement::peerDisconnected(bool normalClose){

    if(normalClose){
        ui.textEditLogs->append(tr("Peer Closed!"));
    }else{
        ui.textEditLogs->append(tr("ERROR! Peer Closed Unexpectedly!"));
    }

    foreach (QByteArray fileMD5, filesList) {
        m_fileManager->closeFile(fileMD5);
    }
    fileTXRequestList.clear();
    filesList.clear();

    //TODO

}



////////////////////////////////////////

void FileManagement::requestFileSystemInfo(const QString &parentDirPath){

    if(!controlCenterPacketsParser->requestFileSystemInfo(m_peerSocket, parentDirPath)){
        ui.textEditLogs->append(tr("Error! Can not send request! %1").arg(controlCenterPacketsParser->lastErrorMessage()));
    }

}

void FileManagement::fileSystemInfoReceived(int socketID, const QString &parentDirPath, const QByteArray &fileSystemInfoData){

    if(socketID != m_peerSocket){
        return;
    }

//    FileManagement *fm = qobject_cast<FileManagement *>(ui.tabFileManagement);
//    if(!fm){return;}

    parseRemoteFilesInfo(parentDirPath, fileSystemInfoData);

}

void FileManagement::requestUploadFilesToRemote(const QString &localBaseDir, const QStringList &localFiles, const QString &remoteDir){

    if(m_peerSocket == INVALID_SOCK_ID){
//        on_toolButtonVerify_clicked();
        //TODO:
    }
    if(m_peerSocket == INVALID_SOCK_ID){
        QMessageBox::critical(this, tr("Error"), tr("Connection is not made!<br>Please connect to peer first!") );
        return;
    }

    startFileManager();

    foreach (QString localFileName, localFiles) {
        QFileInfo fi(localBaseDir, localFileName);
        if(fi.isDir()){
            QStringList newFiles;
            QDir dir(fi.absoluteFilePath());

            QStringList filters;
            filters << "*" << "*.*";
            foreach(QString file, dir.entryList(filters, QDir::Dirs | QDir::Files | QDir::System | QDir::Hidden | QDir::NoDotAndDotDot))
            {
                newFiles.append(file);
                //qApp->processEvents();
            }

            if(!newFiles.isEmpty()){
                QString newRemoteDir = remoteDir + "/" + localFileName;
                if(remoteDir.endsWith("/") || remoteDir.endsWith("\\") ){
                    newRemoteDir = remoteDir + localFileName;
                }
                requestUploadFilesToRemote(fi.absoluteFilePath(), newFiles, newRemoteDir);
            }
            continue;
        }

        QString errorString;
        const FileManager::FileMetaInfo *info = m_fileManager->tryToSendFile(fi.absoluteFilePath(), &errorString);
        if(!info){
            ui.textEditLogs->append(tr("Error! Can not send file! %1").arg(errorString));
            continue ;
        }

//        QMessageBox::information(this, "Upload", "Dir:"+localBaseDir+"<br>"+"Name:"+localFileName+"<br>"+fi.absoluteFilePath());
//        continue;

        bool ok = controlCenterPacketsParser->requestUploadFile(m_peerSocket, info->md5sum, fi.fileName(), info->size, remoteDir);
        if(!ok){
            m_fileManager->closeFile(info->md5sum);
            ui.textEditLogs->append(tr("Error! Can not send file! %1").arg(controlCenterPacketsParser->lastErrorMessage()));
            continue ;
        }else{
            if(!filesList.contains(info->md5sum)){
                filesList.append(info->md5sum);
            }
            ui.textEditLogs->append(tr("Request uploading file %1").arg(localFileName));
        }

    }

}

void FileManagement::requestDownloadFileFromRemote(const QString &remoteBaseDir, const QStringList &remoteFiles, const QString &localDir){

    if(m_peerSocket == INVALID_SOCK_ID){
        //TODO:
//        on_toolButtonVerify_clicked();
    }
    if(m_peerSocket == INVALID_SOCK_ID){
        QMessageBox::critical(this, tr("Error"), tr("Connection is not made!<br>Please connect to peer first!") );
        return;
    }


    startFileManager();

    foreach (QString remoteFileName, remoteFiles) {
        bool ok = controlCenterPacketsParser->requestDownloadFile(m_peerSocket, remoteBaseDir, remoteFileName, localDir);
        if(!ok){
            ui.textEditLogs->append(tr("Error! Can not send file download request! %1").arg(controlCenterPacketsParser->lastErrorMessage()) );
            continue ;
        }else{
            ui.textEditLogs->append(tr("Request downloading file %1").arg(remoteFileName));
        }
    }

}

void FileManagement::startFileManager(){

    if(!m_fileManager){
        m_fileManager = ResourcesManagerInstance::instance()->getFileManager();
        connect(m_fileManager, SIGNAL(dataRead(int , const QByteArray &, int , const QByteArray &, const QByteArray &)), this, SLOT(fileDataRead(int , const QByteArray &, int , const QByteArray &, const QByteArray &)), Qt::QueuedConnection);
        connect(m_fileManager, SIGNAL(error(int, const QByteArray &, quint8, const QString &)), this, SLOT(fileTXError(int, const QByteArray &, quint8, const QString &)), Qt::QueuedConnection);
        connect(m_fileManager, SIGNAL(pieceVerified(const QByteArray &, int , bool , int )), this, SLOT(pieceVerified(const QByteArray &, int , bool , int )), Qt::QueuedConnection);

    }

//    if(!m_udtProtocolForFileTransmission){
//        m_udtProtocolForFileTransmission = ResourcesManagerInstance::instance()->getUDTProtocolForFileTransmission();
//    }

}

void FileManagement::processPeerRequestUploadFilePacket(int socketID, const QByteArray &fileMD5Sum, const QString &fileName, quint64 size, const QString &localFileSaveDir){

    if(socketID != m_peerSocket){
        return;
    }

    startFileManager();

    QString localPath = localFileSaveDir + "/" + fileName;
    if(localFileSaveDir.endsWith('/')){
        localPath = localFileSaveDir + fileName;
    }

    QString errorString;
    const FileManager::FileMetaInfo *info = m_fileManager->tryToReceiveFile(fileMD5Sum, localPath, size, &errorString);
    if(!info){
        //TODO
        QMessageBox::critical(this, tr("Error"), tr("Failed to download file!"));
        return;
    }

    if(!filesList.contains(fileMD5Sum)){
        filesList.append(fileMD5Sum);
    }
    //controlCenterPacketsParser->requestFileData(m_peerSocket, fileMD5Sum, -1, -1);
    controlCenterPacketsParser->requestFileData(m_peerSocket, fileMD5Sum, 0, 0);


}

void FileManagement::processPeerRequestDownloadFilePacket(int socketID, const QString &localBaseDir, const QString &fileName, const QString &remoteFileSaveDir){

    if(socketID != m_peerSocket){
        return;
    }

    startFileManager();

    QString errorString;

    QFileInfo fi(localBaseDir, fileName);
    QString absoluteFilePath = fi.absoluteFilePath();
    if(fi.isDir()){
        QDir dir(absoluteFilePath);

        QStringList filters;
        filters << "*" << "*.*";

        foreach(QString file, dir.entryList(filters, QDir::Dirs | QDir::Files | QDir::System | QDir::Hidden | QDir::NoDotAndDotDot))
        {
            QString newRemoteDir = remoteFileSaveDir + "/" + fileName;
            if(remoteFileSaveDir.endsWith('/')){
                newRemoteDir = remoteFileSaveDir + fileName;
            }
            processPeerRequestDownloadFilePacket(socketID, absoluteFilePath, file, newRemoteDir);

            qApp->processEvents();
        }

        return;
    }

    const FileManager::FileMetaInfo *info = m_fileManager->tryToSendFile(absoluteFilePath, &errorString);
    if(!info){
        controlCenterPacketsParser->denyFileDownloadRequest(socketID, fileName, false, errorString);
    }

    if(controlCenterPacketsParser->acceptFileDownloadRequest(socketID, fileName, true, info->md5sum, info->size, remoteFileSaveDir)){
        if(!filesList.contains(info->md5sum)){
            filesList.append(info->md5sum);
        }
    }else{
        m_fileManager->closeFile(info->md5sum);
    }


}

void FileManagement::fileDownloadRequestAccepted(int socketID, const QString &remoteFileName, const QByteArray &fileMD5Sum, quint64 size, const QString &localFileSaveDir){

    if(socketID != m_peerSocket){
        return;
    }
    //TODO:

    ui.textEditLogs->append(tr("File download request accepted! %1").arg(remoteFileName));

    startFileManager();

    QString localPath = localFileSaveDir + "/" + remoteFileName;
    if(localFileSaveDir.endsWith('/') || localFileSaveDir.endsWith('\\')){
        localPath = localFileSaveDir + remoteFileName;
    }

    QString errorString;
    const FileManager::FileMetaInfo *info = m_fileManager->tryToReceiveFile(fileMD5Sum, localPath, size, &errorString);
    if(!info){
        ui.textEditLogs->append(tr("Error! Failed to download file '%1'! %2 ").arg(remoteFileName).arg(errorString));
        return;
    }

    if(!filesList.contains(fileMD5Sum)){
        filesList.append(fileMD5Sum);
    }
    //controlCenterPacketsParser->requestFileData(m_peerSocket, fileMD5Sum, -1, -1);
    if(!controlCenterPacketsParser->requestFileData(m_peerSocket, fileMD5Sum, 0, 0)){
        m_fileManager->closeFile(fileMD5Sum);
        ui.textEditLogs->append(tr("Error! Failed to download file '%1'! %2 ").arg(remoteFileName).arg(controlCenterPacketsParser->lastErrorMessage()));
    }


}

void FileManagement::fileDownloadRequestDenied(int socketID, const QString &remoteFileName, const QString &message){

    if(socketID != m_peerSocket){
        return;
    }
    //TODO:

    ui.textEditLogs->append(tr("Error! File download request denied! %1").arg(remoteFileName));

}

void FileManagement::fileUploadRequestResponsed(int socketID, const QByteArray &fileMD5Sum, bool accepted, const QString &message){

    if(socketID != m_peerSocket){
        return;
    }

    Q_ASSERT(m_fileManager);
    if(accepted){
        //fileTXRequestList.append(m_fileManager->readPiece(fileMD5Sum, 0));

//        QFileInfo fi("C:/3.dxf");
//        m_udtProtocolForFileTransmission->sendFileToPeer(m_peerFileTransmissionSocket, fi.absoluteFilePath(), 0, fi.size());
//        qDebug()<<"------------------------------------------1";

    }else{
        //QMessageBox::critical(this, tr("Error"), tr("Can not send file!<br>%1").arg(message) );
        m_fileManager->closeFile(fileMD5Sum);
        filesList.removeAll(fileMD5Sum);
        ui.textEditLogs->append(tr("Error! Can not send file!<br>%1").arg(message));
    }

}

void FileManagement::processFileDataRequestPacket(int socketID, const QByteArray &fileMD5, int startPieceIndex, int endPieceIndex){
    qDebug()<<"--FileManagement::processFileDataRequestPacket(...) "<<" startPieceIndex:"<<startPieceIndex<<" endPieceIndex:"<<endPieceIndex;

    if(socketID != m_peerSocket){
        return;
    }
    if(!filesList.contains(fileMD5)){
        return;
    }

    Q_ASSERT(m_fileManager);

    if( (startPieceIndex == -1) && (endPieceIndex == -1) ){
        QList<int> completedPieces = m_fileManager->completedPieces(fileMD5);
        qDebug()<<"completedPieces:"<<completedPieces;

        foreach (int pieceIndex, completedPieces) {
            fileTXRequestList.append(m_fileManager->readPiece(fileMD5, pieceIndex));
            //qApp->processEvents();
        }

    }else{
        Q_ASSERT(endPieceIndex >= startPieceIndex);
        for(int i=startPieceIndex; i<=endPieceIndex; i++){
            fileTXRequestList.append(m_fileManager->readPiece(fileMD5, i));
            //qApp->processEvents();
        }

    }


}

void FileManagement::processFileDataReceivedPacket(int socketID, const QByteArray &fileMD5, int pieceIndex, const QByteArray &data, const QByteArray &sha1){

    if(socketID != m_peerSocket){
        return;
    }
    if(!filesList.contains(fileMD5)){
        return;
    }

    Q_ASSERT(m_fileManager);
    m_fileManager->writePiece(fileMD5, pieceIndex, data, sha1);

}

void FileManagement::processFileTXStatusChangedPacket(int socketID, const QByteArray &fileMD5, quint8 status){

    if(socketID != m_peerSocket){
        return;
    }

    //MS::FileTXStatus status = MS::FileTXStatus(status);
    switch(status){
    case quint8(MS::File_TX_Preparing):
    {

    }
        break;
    case quint8(MS::File_TX_Receiving):
    {

    }
        break;
    case quint8(MS::File_TX_Sending):
    {

    }
        break;
    case quint8(MS::File_TX_Progress):
    {

    }
        break;
    case quint8(MS::File_TX_Paused):
    {

//        fileTXWithAdminStatus = MS::File_TX_Paused;
    }
        break;
    case quint8(MS::File_TX_Aborted):
    {
//        closeFileTXWithAdmin();

    }
        break;
    case quint8(MS::File_TX_Done):
    {

        ui.textEditLogs->append(tr("File Uploaded! '%1'").arg(m_fileManager->getFileLocalSavePath(fileMD5)));
        m_fileManager->closeFile(fileMD5);
    }
        break;
    default:
        break;
    }

}

void FileManagement::processFileTXErrorFromPeer(int socketID, const QByteArray &fileMD5, quint8 errorCode, const QString &errorMessage){
    qDebug()<<"--FileManagement::processFileTXErrorFromPeer(...) " <<" socketID:"<<socketID;
    qCritical()<<errorMessage;

    ui.textEditLogs->append(errorMessage);

}

void FileManagement::fileDataRead(int requestID, const QByteArray &fileMD5, int pieceIndex, const QByteArray &data, const QByteArray &dataSHA1SUM){
    qDebug()<<"--FileManagement::fileDataRead(...) "<<" pieceIndex:"<<pieceIndex<<" size:"<<data.size();


    if(!fileTXRequestList.contains(requestID)){
        return;
    }
    fileTXRequestList.removeAll(requestID);

    if(!filesList.contains(fileMD5)){
        return;
    }


    controlCenterPacketsParser->sendFileData(m_peerSocket, fileMD5, pieceIndex, &data, &dataSHA1SUM);

}

void FileManagement::fileTXError(int requestID, const QByteArray &fileMD5, quint8 errorCode, const QString &errorString){

    if(!fileTXRequestList.contains(requestID)){
        return;
    }
    fileTXRequestList.removeAll(requestID);

    if(!filesList.contains(fileMD5)){
        return;
    }

    qCritical()<<errorString;
    ui.textEditLogs->append(errorString);

    controlCenterPacketsParser->fileTXError(m_peerSocket, fileMD5, errorCode, errorString);

}

void FileManagement::pieceVerified(const QByteArray &fileMD5, int pieceIndex, bool verified, int verificationProgress){
    qDebug()<<"--FileManagement::pieceVerified(...) "<<" pieceIndex:"<<pieceIndex<<" verificationProgress:"<<verificationProgress;

    if(!filesList.contains(fileMD5)){
        return;
    }

    if(verified){

        if(verificationProgress == 100){
            //qWarning()<<"Done!";
            controlCenterPacketsParser->fileTXStatusChanged(m_peerSocket, fileMD5, quint8(MS::File_TX_Done));
            ui.textEditLogs->append(tr("File Downloaded! '%1'").arg(m_fileManager->getFileLocalSavePath(fileMD5)));
        }else{
            //TODO:
//            int uncompletedPieceIndex = m_fileManager->getOneUncompletedPiece(fileMD5);
//            qDebug()<<"uncompletedPieceIndex:"<<uncompletedPieceIndex;
//            if(uncompletedPieceIndex < 0){
//                return;
//            }
//            controlCenterPacketsParser->requestFileData(m_peerSocket, fileMD5, uncompletedPieceIndex);

            //if((pieceIndex % FILE_PIECES_IN_ONE_REQUEST) == 0){
            //    controlCenterPacketsParser->requestFileData(m_peerSocket, fileMD5, pieceIndex + 1, pieceIndex + FILE_PIECES_IN_ONE_REQUEST);
            //}

            if((pieceIndex % FILE_PIECES_IN_ONE_REQUEST) == 0){
                if(pieceIndex == 0 ){
                        controlCenterPacketsParser->requestFileData(m_peerSocket, fileMD5, 1, 2 * FILE_PIECES_IN_ONE_REQUEST);
                }else{
                    controlCenterPacketsParser->requestFileData(m_peerSocket, fileMD5, pieceIndex + FILE_PIECES_IN_ONE_REQUEST + 1, pieceIndex + 2 * FILE_PIECES_IN_ONE_REQUEST);
                }
            }


        }

    }else{

        controlCenterPacketsParser->requestFileData(m_peerSocket, fileMD5, pieceIndex, pieceIndex);
    }

}









//////////////////////////////////////////////

void FileManagement::on_pushButtonUploadToRemote_clicked(){

//    QModelIndex index = ui.tableViewLocalFiles->currentIndex();
//    if(!index.isValid() || localFileSystemModel->fileInfo(index).isRoot()){
//        QMessageBox::critical(this, tr("Error"), tr("Please select one file or folder to upload!"));
//        return;
//    }

    QString remoteDir = remoteFileSystemModel->currentDirPath();
    if(remoteDir.isEmpty()){
        QMessageBox::critical(this, tr("Error"), tr("Please select the remote path to save files!"));
        ui.tableViewRemoteFiles->setFocus();
        return;
    }

//    QString filePath = localFileSystemModel->fileInfo(index).absoluteFilePath();
//    QStringList files;
//    files.append(filePath);
////    emit signalUploadFilesToRemote(files, remoteFileSystemModel->currentDirPath());
//    requestUploadFilesToRemote(files, remoteFileSystemModel->currentDirPath());


    QStringList files;

    QModelIndexList selectedIndexes = ui.tableViewLocalFiles->selectionModel()->selectedIndexes();
    int selectedIndexesCount = selectedIndexes.count();

    for (int j = 0; j < selectedIndexesCount; ++j) {
        QModelIndex index = selectedIndexes.at(j);
        if (index.column() != 0){
            continue;
        }

        QString filePath = localFileSystemModel->fileInfo(index).fileName();
        if(m_localCurrentDir.startsWith(filePath)){
            continue;
        }
        files.append(filePath);
        qApp->processEvents();
    }

    if(files.isEmpty()){
        QMessageBox::critical(this, tr("Error"), tr("Please select at least one file or folder to upload!"));
        return;
    }

    int ret = QMessageBox::question(this, tr("Question"), tr("Are you sure to upload files?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
    if(ret == QMessageBox::No){
        return;
    }

    requestUploadFilesToRemote(m_localCurrentDir, files, remoteFileSystemModel->currentDirPath());


}

void FileManagement::on_pushButtonDownloadToLocal_clicked(){

//    QModelIndex index = ui.tableViewRemoteFiles->currentIndex();
//    if(!index.isValid() || remoteFileSystemModel->isDrive(index)){
//        QMessageBox::critical(this, tr("Error"), tr("Please select one file or folder to download!"));
//        return;
//    }

    if(m_localCurrentDir.isEmpty()){
        QMessageBox::critical(this, tr("Error"), tr("Please select the local path to save files!"));
        ui.tableViewLocalFiles->setFocus();
        return;
    }

//    QString filePath = remoteFileSystemModel->absoluteFilePath(index);
//    QStringList files;
//    files.append(filePath);
////    emit signalDownloadFileFromRemote(files, m_localCurrentDir);
//    requestDownloadFileFromRemote(remoteFileSystemModel->currentDirPath(), files, m_localCurrentDir);



    QStringList files;

    QModelIndexList selectedIndexes = ui.tableViewRemoteFiles->selectionModel()->selectedIndexes();
    int selectedIndexesCount = selectedIndexes.count();

    for (int j = 0; j < selectedIndexesCount; ++j) {
        QModelIndex index = selectedIndexes.at(j);
        if (index.column() != 0){
            continue;
        }

        QString filePath = remoteFileSystemModel->fileName(index);
        if(filePath == ".."){
            continue;
        }
        files.append(filePath);
        qApp->processEvents();
    }

    if(files.isEmpty()){
        QMessageBox::critical(this, tr("Error"), tr("Please select at least one file or folder to download!"));
        return;
    }

    int ret = QMessageBox::question(this, tr("Question"), tr("Are you sure to download files?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
    if(ret == QMessageBox::No){
        return;
    }


    requestDownloadFileFromRemote(remoteFileSystemModel->currentDirPath(), files, m_localCurrentDir);



}

















} //namespace HEHUI

