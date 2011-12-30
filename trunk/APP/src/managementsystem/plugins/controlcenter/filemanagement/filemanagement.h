#ifndef FILEMANAGEMENT_H
#define FILEMANAGEMENT_H


#include <QFileSystemModel>
#include <QCompleter>
#include <QFileIconProvider>

#include "ui_filemanagement.h"


namespace HEHUI {

enum FileType{FILE = 0, DRIVE, FOLDER };

class FileSystemModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    FileSystemModel(QFileIconProvider *fileIconProvider, QObject *parent = 0);
    virtual ~FileSystemModel();

    void setFileItems(QList<FileItemInfo> fileItems);
    void addFileItem(const QString &name, const QString &size, const QString &type, const QString &dateModified);
    void deleteFileItem(const QString &name);


    int rowCount ( const QModelIndex & parent = QModelIndex() ) const ;
    int	columnCount ( const QModelIndex & parent = QModelIndex() ) const;
    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const ;
    QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;


private:
    struct FileItemInfo{
        QString name;
        QString size;
        QString type;
        QString dateModified;

    };

    QList<FileItemInfo> fileItems;

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



private slots:

    void on_groupBoxLocal_toggled( bool on );
    void on_toolButtonShowLocalFiles_clicked();
    void localFileItemDoubleClicked(const QModelIndex &index);


    void on_groupBoxRemote_toggled( bool on );
    void on_toolButtonShowRemoteFiles_clicked();
    void tableViewRemoteFileItemDoubleClicked(const QModelIndex &index);

    bool getLocalFilesInfo(const QString &parentDirPath, QByteArray *result, QString *errorMessage);
    bool parseRemoteFilesInfo(const QString &remoteParentDirPath, const QByteArray &data);


private:
    Ui::FileManagement ui;


    QFileSystemModel *localFileSystemModel;

    QCompleter *localFilesCompleter;



};

}

#endif // FILEMANAGEMENT_H
