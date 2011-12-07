#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>

#include "HHSharedUDT/hudtprotocolbase.h"


namespace HEHUI {


class UDTProtocolForFileTX : public UDTProtocolBase
{
    Q_OBJECT
public:
    explicit UDTProtocolForFileTX(bool stream = false, const SocketOptions *options = 0, QObject *parent = 0);


signals:
    void dataReceived(UDTSOCKET socket, QByteArray *data, bool stream);

private slots:
    void streamDataReceived(UDTSOCKET socket, QByteArray *data) ;
    void messageDataReceived(UDTSOCKET socket, QByteArray *data) ;

};


class FileManager : public QObject
{
    Q_OBJECT
public:
    explicit FileManager(QObject *parent = 0);
    ~FileManager();

    enum ErrorType{NoError = 0, ReadError, FileNotExistError, WriteError, NoSpaceError, AbortError, NetworkError, UnknownError};


    bool isReady();


signals:
//    void SendFileProgressupdated(int fileID, int progress);
//    void ReceiveFileProgressupdated(int fileID, int progress);

    void error(quint8 errorNO, const QString &message);

    void peerRequestToTxFile(int fileID, const QString &fileName, qint64 fileSize);
    void peerResponseTxFile(int socketID, const QString &filePath, bool accept, int errorNO, const QString &message);

    void peerRequestToRxFile(int fileID, const QString &filePath);
    void peerResponseRxFile(int fileID, bool accept, int errorNO, const QString &message);


public slots:
    bool requestToTXFile(int socketID, const QString &filePath);
//    bool sendFile(int fileID);
    bool responseTXFile(int socketID, int fileID, bool accept, quint8 errorCode, const QString &message);

    bool requestToRXFile(int socketID, const QString &filePath);
    bool responseRXFile(int socketID, const QString &filePath, bool accept, quint8 errorCode, const QString &message);

    bool requestDataBlock(int socketID, int fileID, quint64 offset, quint64 length);

private:
    enum FileStatus{WaitingForStart_Status, TX_Status, PAUSE_Status, Stop_Status, Done_status};

    struct FileInfo{
        FileInfo(){
            socketID = UDTProtocolBase::INVALID_UDT_SOCK;
            fileID = qrand();
            localLocation = "";
            size = 0;
            //checksum = 0;
            stream = 0;
            status = Stop_Status;
            startOffset = 0;
        }

        int socketID;
        int fileID; //File ID
        QString localLocation; //File Path
        quint64 size;
        //quint16 checksum;
        QDataStream *stream;
        FileStatus status;
        quint64 startOffset;

    };

private slots:

    bool sendDataBlock(int fileID, quint64 offset, quint64 length);

    FileInfo * getToBeSentFileInfo(int socketID, const QString &filePath);

    FileInfo * getToBeReceivedFileInfo(int socketID, int fileID);

    void parseIncomingData(int socket, QByteArray *data, bool stream);

private:

    enum DataType{UNKNOWN_TYPE, REQUEST_TX_Type, RESPONSE_TX_Type, REQUEST_RX_Type,  RESPONSE_RX_Type, REQUEST_DATA_BLOCK_TYPE, BLOCK_DATA_Type, PAUSE_TX_Type, PAUSE_RX_Type, CONTINUE_TX_Type, CONTINUE_RX_Type, STOP_TX_Type, STOP_RX_Type, ERROR_INFO_Type};


    UDTProtocolForFileTX *m_udtProtocol;




    QHash<int/*File ID*/, FileInfo*> filesToBeSent;
    QMultiHash<int/*File ID*/, FileInfo*> filesToBeReceived;

    int m_blockSize;

    bool m_ready;



};

} //namespace HEHUI

#endif // FILEMANAGER_H
