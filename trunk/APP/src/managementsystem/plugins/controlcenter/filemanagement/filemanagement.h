#ifndef FILEMANAGEMENT_H
#define FILEMANAGEMENT_H

#include <QObject>
#include <QFileSystemModel>
#include <QCompleter>
#include <QFileIconProvider>
#include <QList>

#include "ui_filemanagement.h"

#include "../../sharedms/global_shared.h"



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

    bool isDir(const QModelIndex &index);
    QString absoluteFilePath(const QModelIndex &index);

    void changePath(const QString &newPath);

private:
    struct FileItemInfo{
        QString name;
        QString size;
        quint8 type;
        QString dateModified;

    };

    QList<FileItemInfo *> fileItems;

    QString currentDirPath;

    QFileIconProvider *m_fileIconProvider;

};



class FileManagement : public QWidget
{
    Q_OBJECT
    
public:
    explicit FileManagement(QWidget *parent = 0);
    

signals:
    void signalShowRemoteFiles(const QString &path);


public slots:
    bool parseRemoteFilesInfo(const QString &remoteParentDirPath, const QByteArray &data);


private slots:

    void on_groupBoxLocal_toggled( bool on );
    void on_toolButtonShowLocalFiles_clicked();
    void localFileItemDoubleClicked(const QModelIndex &index);


    void on_groupBoxRemote_toggled( bool on );
    void on_toolButtonShowRemoteFiles_clicked();
    void tableViewRemoteFileItemDoubleClicked(const QModelIndex &index);

    bool getLocalFilesInfo(const QString &parentDirPath, QByteArray *result, QString *errorMessage);


private:
    Ui::FileManagement ui;


    QFileSystemModel *localFileSystemModel;
    QCompleter *localFilesCompleter;

    FileSystemModel *remoteFileSystemModel;



};

}

#endif // FILEMANAGEMENT_H