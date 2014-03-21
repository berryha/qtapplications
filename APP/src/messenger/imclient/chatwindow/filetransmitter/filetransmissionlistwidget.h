#ifndef FILETRANSMISSIONLISTWIDGET_H
#define FILETRANSMISSIONLISTWIDGET_H

#include <QWidget>

#include "progressinfowidget.h"


namespace Ui {
class FileTransmissionListWidget;
}

namespace HEHUI {


class FileTransmissionListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FileTransmissionListWidget(QWidget *parent = 0);
    ~FileTransmissionListWidget();


signals:
    void cancelSendingFileRequest(const QString &fileMD5);
    void abortFileTransmission(const QString &fileMD5);

    void acceptFileRequest(const QString &fileMD5, const QString &localSavePath);
    void declineFileRequest(const QString &fileMD5);

    void signlaCloseWidget();

public slots:
    void slotFileRequestReceivedFromContact(const QString &fileName, qint64 size, const QString &fileMD5);
    void slotSendFileRequestToContact(const QString &fileName, const QString &fileMD5);

    //    void slotFileRequestCanceledByContact(const QString &fileMD5);
//    void slotFileTransmissionAbortedByContact(const QString &fileMD5);
    void slotCloseProgressInfoWidget(const QString &fileMD5);


    void slotCancelSendingFileRequest(const QString &fileMD5);
    void slotAbortFileTransmission(const QString &fileMD5);
    void slotAcceptFileRequest(const QString &fileMD5, const QString &localSavePath);
    void slotDeclineFileRequest(const QString &fileMD5);

    void updateFileTransmissionProgress(const QString &fileMD5, int percent);


private:
    int findListWidgetItemIndex(ProgressInfoWidget *wgt);
    ProgressInfoWidget * createProgressInfoWidgetItem(const QString &fileMD5);

private:
    Ui::FileTransmissionListWidget *ui;

    //QHash<QString /*File MD5*/, QString /*File Name*/> fileInfoHash;
    QHash<QString /*File MD5*/, ProgressInfoWidget*> progressInfoWidgetHash;


};

} //namespace HEHUI

#endif // FILETRANSMISSIONLISTWIDGET_H
