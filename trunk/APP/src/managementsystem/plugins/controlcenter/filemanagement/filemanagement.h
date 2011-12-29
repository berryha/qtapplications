#ifndef FILEMANAGEMENT_H
#define FILEMANAGEMENT_H


#include <QFileSystemModel>
#include <QCompleter>

#include "ui_filemanagement.h"


namespace HEHUI {


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





private:
    Ui::FileManagement ui;


    QFileSystemModel *localFileSystemModel;

    QCompleter *localFilesCompleter;



};

}

#endif // FILEMANAGEMENT_H
