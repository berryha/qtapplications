#ifndef PROGRESSINFOWIDGET_H
#define PROGRESSINFOWIDGET_H

#include <QWidget>

namespace Ui {
class ProgressInfoWidget;
}

class ProgressInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProgressInfoWidget(QWidget *parent = 0);
    ~ProgressInfoWidget();

    void requestToSendFile(const QString &filePath, const QString &fileMD5);
    void requestToReceiveFile(const QString &fileName, qint64 size, const QString &fileMD5);


signals:
    void cancelSendingFileRequest(const QString &fileMD5);
    void abortFileTransmission(const QString &fileMD5);

    void acceptFileRequest(const QString &fileMD5, const QString &localSavePath);
    void declineFileRequest(const QString &fileMD5);

public slots:
    void updateProgress(int percent);

private slots:
    void on_toolButtonFile_clicked();
    void on_pushButtonSendOfflineFile_clicked();
    void on_pushButtonCancel_clicked();
    void on_toolButtonAbort_clicked();
    void on_pushButtonAccept_clicked();
    void on_pushButtonSaveAs_clicked();
    void on_pushButtonDecline_clicked();

private:
    QString getFileSizeString(qint64 fileSize) const;


private:
    Ui::ProgressInfoWidget *ui;

    bool m_sendingMode;
    //bool m_transmitting;
    QString m_filePath;
    QString m_fileMD5;


};

#endif // PROGRESSINFOWIDGET_H
