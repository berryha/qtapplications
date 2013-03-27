#ifndef FILEMANAGEMENT_H
#define FILEMANAGEMENT_H

#include <QWidget>
#include <QObject>
#include <QFileSystemModel>
#include <QCompleter>
#include <QFileIconProvider>
#include <QList>
#include <QDragEnterEvent>
#include <QMap>

#include "ui_filemanagement.h"

#include "../../sharedms/global_shared.h"

#include "networkmanager/controlcenterpacketsparser.h"


namespace HEHUI {


class FileSystemModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    FileSystemModel(QFileIconProvider *fileIconProvider, QObject *parent = 0);
    virtual ~FileSystemModel();

//    void setFileItems(QList<FileItemInfo> fileItems);
    void addFileItem(const QString &name, const QString &size, quint8 type, const QString &dateModified);
    void deleteFileItem(const QString &name);
    void deleteFileItem(const QModelIndex & index);


    int rowCount ( const QModelIndex & parent = QModelIndex() ) const ;
    int	columnCount ( const QModelIndex & parent = QModelIndex() ) const;
    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const ;
    QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

    bool parseRemoteFilesInfo(const QString &remoteParentDirPath, const QByteArray &data);

    bool isDrive(const QModelIndex &index);
    bool isDir(const QModelIndex &index);
    QString absoluteFilePath(const QModelIndex &index);
    QString fileName(const QModelIndex &index);

    void changePath(const QString &newPath);

    QString currentDirPath() const;

private:
    struct FileItemInfo{
        QString name;
        QString size;
        quint8 type;
        QString dateModified;

    };

    QList<FileItemInfo *> fileItems;

    QString m_currentDirPath;

    QFileIconProvider *m_fileIconProvider;

};



class FileManagement : public QWidget
{
    Q_OBJECT
    
public:
    explicit FileManagement(QWidget *parent = 0);
    ~FileManagement();

    void setPacketsParser(ControlCenterPacketsParser *parser);
    void setPeerSocket(UDTSOCKET peerSocket);
    

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);


signals:
//    void signalShowRemoteFiles(const QString &path);
//    void signalUploadFilesToRemote(const QStringList &localFiles, const QString &remoteDir);
//    void signalDownloadFileFromRemote(const QStringList &remoteFiles, const QString &localDir);

public slots:
    void peerDisconnected(bool normalClose);

    ///////////////////
    void requestFileSystemInfo(const QString &parentDirPath);
    void fileSystemInfoReceived(int socketID, const QString &parentDirPath, const QByteArray &fileSystemInfoData);
    void requestUploadFilesToRemote(const QString &localBaseDir, const QStringList &localFiles, const QString &remoteDir);
    void requestDownloadFileFromRemote(const QString &remoteBaseDir, const QStringList &remoteFiles, const QString &localDir);

    //File TX
    void startFileManager();
    void processPeerRequestUploadFilePacket(int socketID, const QByteArray &fileMD5Sum, const QString &fileName, quint64 size, const QString &localFileSaveDir);
    void processPeerRequestDownloadFilePacket(int socketID, const QString &localBaseDir, const QString &fileName, const QString &remoteFileSaveDir);

    void fileDownloadRequestAccepted(int socketID, const QString &remoteFileName, const QByteArray &fileMD5Sum, quint64 size, const QString &localFileSaveDir);
    void fileDownloadRequestDenied(int socketID, const QString &remoteFileName, const QString &message);
    void fileUploadRequestResponsed(int socketID, const QByteArray &fileMD5Sum, bool accepted, const QString &message);

    void processFileDataRequestPacket(int socketID, const QByteArray &fileMD5, int startPieceIndex, int endPieceIndex);
    void processFileDataReceivedPacket(int socketID, const QByteArray &fileMD5, int pieceIndex, const QByteArray &data, const QByteArray &sha1);
    void processFileTXStatusChangedPacket(int socketID, const QByteArray &fileMD5, quint8 status);
    void processFileTXErrorFromPeer(int socketID, const QByteArray &fileMD5, quint8 errorCode, const QString &errorMessage);

    void fileDataRead(int requestID, const QByteArray &fileMD5, int pieceIndex, const QByteArray &data, const QByteArray &dataSHA1SUM);
    void fileTXError(int requestID, const QByteArray &fileMD5, quint8 errorCode, const QString &errorString);
    void pieceVerified(const QByteArray &fileMD5, int pieceIndex, bool verified, int verificationProgress);


private slots:

    void on_groupBoxLocal_toggled( bool on );
    void on_toolButtonShowLocalFiles_clicked();
    void localFileItemDoubleClicked(const QModelIndex &index);


    void on_groupBoxRemote_toggled( bool on );
    void on_toolButtonShowRemoteFiles_clicked();
    void tableViewRemoteFileItemDoubleClicked(const QModelIndex &index);
    bool getLocalFilesInfo(const QString &parentDirPath, QByteArray *result, QString *errorMessage);
    bool parseRemoteFilesInfo(const QString &remoteParentDirPath, const QByteArray &data);

    void on_pushButtonUploadToRemote_clicked();
    void on_pushButtonDownloadToLocal_clicked();

private:
    Ui::FileManagement ui;

    ControlCenterPacketsParser *controlCenterPacketsParser;

    QFileSystemModel *localFileSystemModel;
    QCompleter *localFilesCompleter;
    QString m_localCurrentDir;

    FileSystemModel *remoteFileSystemModel;


//    UDTProtocol *m_udtProtocol;
    int m_peerSocket;
    //UDTProtocolForFileTransmission *m_udtProtocolForFileTransmission;
    //UDTSOCKET m_peerFileTransmissionSocket;


    FileManager *m_fileManager;
    QList<int/*File TX Request ID*/> fileTXRequestList;
    QList<QByteArray/*File MD5*/> filesList;

};

}

#endif // FILEMANAGEMENT_H
