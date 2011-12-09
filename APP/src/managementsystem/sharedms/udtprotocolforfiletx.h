#ifndef UDTPROTOCOLFORFILETX_H
#define UDTPROTOCOLFORFILETX_H

#include <QObject>
#include <QFile>

#include "HHSharedUDT/hudtprotocolbase.h"




namespace HEHUI {


class UDTProtocolForFileTransmission : public UDTProtocolBase
{
    Q_OBJECT
public:
    enum Error{UNKNOWN_ERROR = 0, FILR_READ_ERROR, FILR_WRITE_ERROR, FILR_NOT_EXIST_ERROR, FILE_IS_BEEN_TX_ERROR, NETWORK_ERROR};
    explicit UDTProtocolForFileTransmission(bool stream = false, const SocketOptions *options = 0, QObject *parent = 0);


signals:

    void error(quint8 errorNO, const QString &message);

    void peerRequestToUploadFile(int fileID, const QString &fileName, qint64 fileSize);
    void peerResponseUploadFile(int socketID, const QString &filePath, bool accept, int errorNO, const QString &message);

    void peerRequestToDownloadFile(int fileID, const QString &filePath);
    void peerResponseDownloadFile(int fileID, bool accept, int errorNO, const QString &message);


public slots:

    bool requestToUploadFile(int socketID, const QString &filePath);
    bool responseUploadFile(int socketID, int fileID, bool accept, quint8 errorCode, const QString &message);

    bool requestToDownloadFile(int socketID, const QString &filePath);
    bool responseDownloadFile(int socketID, const QString &filePath, bool accept, quint8 errorCode, const QString &message);

    bool requestDataBlock(int socketID, int fileID, quint64 offset, quint64 length);

private:
    //static int lastFileID;
    enum FileStatus{STOP_STATUS = 0, READY_FOR_TX_STATUS, TX_STATUS, PAUSE_STATUS, DONE_status};

    struct FileInfo{
        FileInfo(){
            socketID = UDTProtocolBase::INVALID_UDT_SOCK;
            ///fileID = (++lastFileID);
            localLocation = "";
            size = 0;
            //checksum = 0;
            file = 0;
            status = STOP_STATUS;
        }

        int socketID;
        //quint16 fileID; //File ID
        QString localLocation; //File Path
        quint64 size;
        //quint16 checksum;
        QFile *file;
        FileStatus status;

    };

private slots:
    void streamDataReceived(UDTSOCKET socket, QByteArray *data) ;
    void messageDataReceived(UDTSOCKET socket, QByteArray *data) ;


    bool sendDataBlock(int fileID, quint64 offset, quint64 length);

//    FileInfo * getToBeUploadedFileInfo(int socketID, quint16 fileID);
//    FileInfo * getToBeUploadedFileInfo(int socketID, const QString &filePath);

//    FileInfo * getToBeDownloadedFileInfo(int socketID, quint16 fileID);

    bool writeDataToFile(int socketID, int fileID, quint64 offset, quint64 length, QByteArray *data);

    void parseIncomingData(int socketID, QByteArray &data);

private:

    enum DataType{UNKNOWN_DATA_TYPE = 0, REQUESR_FILD_ID_TYPE, RESPONSE_FILD_ID_TYPE, REQUEST_UPLOAD_TYPE, RESPONSE_UPLOAD_TYPE, REQUEST_DOWNLOAD_TYPE,  RESPONSE_DOWNLOAD_TYPE, REQUEST_DATA_BLOCK_TYPE, BLOCK_DATA_TYPE, CHANGE_TX_STATUS_TYPE, ERROR_INFO_Type};

//    QReadWriteLock filesToBeUploadedLock;
    QHash<int/*Socket ID*/, FileInfo*> filesToBeUploaded;
//    QReadWriteLock filesToBeDownloadedLock;
    QHash<int/*Socket ID*/, FileInfo*> filesToBeDownloaded;

    int m_blockSize;




};

}

#endif // UDTPROTOCOLFORFILETX_H
