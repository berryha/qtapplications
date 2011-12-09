#include "udtprotocolforfiletx.h"

//#include <QDateTime>
#include <QFileInfo>
#include <QtConcurrentRun>


namespace HEHUI {


//int UDTProtocolForFileTransmission::lastFileID = 0;
UDTProtocolForFileTransmission::UDTProtocolForFileTransmission(bool stream, const SocketOptions *options, QObject *parent) :
    UDTProtocolBase(stream, options, parent)
{

    m_blockSize = 1500;


//    qsrand(QDateTime::currentDateTime().toTime_t());
//    if(0 == lastFileID){
//        lastFileID = qrand();
//    }


}

void UDTProtocolForFileTransmission::streamDataReceived(UDTSOCKET socket, QByteArray *data){
    qDebug()<<"--UDTProtocol::streamDataReceived(...) "<<"socket:"<<socket;

    QtConcurrent::run(this, &UDTProtocolForFileTransmission::parseIncomingData, socket, data);

}

void UDTProtocolForFileTransmission::messageDataReceived(UDTSOCKET socket, QByteArray *data){
    qDebug()<<"--UDTProtocol::messageDataReceived(...) "<<"socket:"<<socket;

    QtConcurrent::run(this, &UDTProtocolForFileTransmission::parseIncomingData, socket, data);

}

bool UDTProtocolForFileTransmission::requestToUploadFile(int socketID, const QString &filePath){

    QFileInfo fi(filePath);
    if(!fi.exists()){
        emit error(quint8(FILR_NOT_EXIST_ERROR), tr("File '%1' does not exist!").arg(filePath));
        return false;
    }

    QFile *file = new QFile(filePath);
    if(!file->open(QIODevice::ReadOnly)){
        emit error(quint8(FILR_READ_ERROR), file->errorString());
        return false;
    }

    //FileInfo *info = getToBeUploadedFileInfo(socketID, filePath);
    FileInfo *info = filesToBeUploaded.value(socketID);
    if(!info){
        info = new FileInfo();
        info->socketID = socketID;
        info->localLocation = filePath;
        info->size = file->size();
        info->status = STOP_STATUS;
        //info->checksum = qChecksum()
        //info->stream = new QDataStream(file);
        info->file = file;
        filesToBeUploaded.insert(socketID, info);

    }else{
        //TODO
        if(info->localLocation == filePath && info->status == TX_STATUS){
            emit error(quint8(FILE_IS_BEEN_TX_ERROR), tr("File is been uploaded!"));
            return false;
        }else{
            info->localLocation = filePath;
            info->size = file->size();
            info->status = STOP_STATUS;
            //info->checksum = qChecksum()
            //info->stream = new QDataStream(file);
            QFile *f = info->file;
            if(f && f != file){
                f->close();
                delete f;
            }
            info->file = file;

        }
    }



    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);
    out << quint8(REQUEST_UPLOAD_TYPE) << fi.fileName() << file->size();

    return sendData(socketID, &ba);


}

bool UDTProtocolForFileTransmission::responseUploadFile(int socketID, int fileID, bool accept, quint8 errorCode, const QString &message){

    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);
    out << quint8(RESPONSE_UPLOAD_TYPE) << fileID << accept << errorCode << message;

    return sendData(socketID, &ba);

}

bool UDTProtocolForFileTransmission::requestToDownloadFile(int socketID, const QString &filePath){

    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);
    out << quint8(REQUEST_DOWNLOAD_TYPE) << filePath;

    return sendData(socketID, &ba);

}

bool UDTProtocolForFileTransmission::responseDownloadFile(int socketID, const QString &filePath, bool accept, quint8 errorCode, const QString &message){

    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);
    out << quint8(RESPONSE_DOWNLOAD_TYPE) << filePath << accept << errorCode << message;

    return sendData(socketID, &ba);

}


//bool FileManager::sendFile(int fileID){

//    FileInfo *info = filesToBeSent.value(fileID);
//    if(!info){
//        emit error(quint8(UnknownError), tr("File Info does not exist!"));
//        return false;
//    }

//    QDataStream *in = info->stream;
//    info->status = TX_Status;
//    QByteArray fragmentData;

//    QByteArray ba;
//    QDataStream out(&ba, QIODevice::WriteOnly);
//    out.setVersion(QDataStream::Qt_4_6);

//    bool ok = false;
//    while ( !in->atEnd() && (info->status == TX_Status) ) {
//        fragmentData.resize(m_blockSize);
//        qint64 size = dev->read(fragmentData.data(), m_blockSize);
//        if(dev->atEnd()){
//            fragmentData.resize(size);
//        }

//        ba.clear();
//        out.dev->seek(0);
//        out << fileID << size << qChecksum(fragmentData.data(), size) << fragmentData;

//        ok =  sendData(info->socketID, &ba);
//        if(!ok){
//            info->status = Stop_Status;
//            emit error(quint8(NetworkError), getLastErrorMessage());
//            return false;
//        }

//    }

//}


bool UDTProtocolForFileTransmission::requestDataBlock(int socketID, int fileID, quint64 offset, quint64 length){

    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);
    out << quint8(REQUEST_DATA_BLOCK_TYPE) << fileID << offset << length;

    return sendData(socketID, &ba);

}

bool UDTProtocolForFileTransmission::sendDataBlock(int fileID, quint64 offset, quint64 length){

    FileInfo *info = filesToBeUploaded.value(fileID);
    if(!info){
        emit error(quint8(UNKNOWN_ERROR), tr("File Info does not exist!"));
        return false;
    }

    if( (offset + length) > info->size){
        emit error(quint8(), tr("Length out of file size!"));
        return false;
    }

    QFile *file = info->file;
    file->seek(offset);
    info->status = TX_STATUS;
    QByteArray fragmentData;

    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);

    bool ok = false;
    int socketID = info->socketID;
    int blockSize = m_blockSize - sizeof(quint8) - sizeof(qint64) - sizeof(quint16);
    quint64 sentSize = 0;
    while ( sentSize < length && (info->status == TX_STATUS) ) {
        fragmentData.resize(blockSize);
        qint64 size = file->read(fragmentData.data(), blockSize);
        if(size == -1){
            info->status = STOP_STATUS;
            emit error(quint8(FILR_READ_ERROR), file->errorString());
            return false;
        }
        if(file->atEnd()){
            fragmentData.resize(size);
        }

        ba.clear();
        out.device()->seek(0);
        out << quint8(BLOCK_DATA_TYPE) << fileID << size << qChecksum(fragmentData.data(), size) << fragmentData;

        ok =  sendData(socketID, &ba);
        if(!ok){
            info->status = STOP_STATUS;
            emit error(quint8(NETWORK_ERROR), getLastErrorMessage());
            return false;
        }else{
            sentSize += size;
        }

    }

    info->status = DONE_status;
    return true;

}

//UDTProtocolForFileTransmission::FileInfo * UDTProtocolForFileTransmission::getToBeUploadedFileInfo(int socketID, quint16 fileID){

//    foreach (FileInfo *info, filesToBeUploaded.values(fileID)) {
//        if( info->socketID == socketID){
//            return info;
//        }
//    }

//    return 0;

//}

//UDTProtocolForFileTransmission::FileInfo * UDTProtocolForFileTransmission::getToBeUploadedFileInfo(int socketID, const QString &filePath){

//    foreach (FileInfo *info, filesToBeUploaded.values()) {
//        if( info->socketID == socketID && info->localLocation == filePath){
//            return info;
//        }
//    }

//    return 0;

//}

//UDTProtocolForFileTransmission::FileInfo * UDTProtocolForFileTransmission::getToBeDownloadedFileInfo(int socketID, quint16 fileID){

//    foreach (FileInfo *info, filesToBeDownloaded.values(fileID)) {
//        if(info->socketID == socketID){
//            return info;
//        }

//    }

//    return 0;

//}

bool UDTProtocolForFileTransmission::writeDataToFile(int socketID, int fileID, quint64 offset, quint64 length, QByteArray *data){

}

void UDTProtocolForFileTransmission::parseIncomingData(int socketID, QByteArray &data){

    QDataStream in(&data, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_6);

    quint8 dataType = quint8(UNKNOWN_DATA_TYPE);

    switch(dataType){
    case quint8(REQUEST_UPLOAD_TYPE):
    {
        int fileID = 0;
        QString fileName = "";
        qint64 size = 0;
        in >> fileID >> fileName >> size;

        FileInfo *info = filesToBeDownloaded.value(fileID);
        if(!info){
            info = new FileInfo();
            info->fileID = fileID;
            info->size = size;
        }

        emit peerRequestToUploadFile(fileID, fileName, size);

    }
        break;
    case quint8(RESPONSE_UPLOAD_TYPE):
    {
        int fileID = 0;
        bool accept = false;
        quint8 errorCode = quint8(UNKNOWN_ERROR);
        QString message = "";
        in >> fileID >> accept >> errorCode >> message;

        FileInfo *info = filesToBeUploaded.value(fileID);
        if(!info){return;}
        if(accept){
            QtConcurrent::run(this, &UDTProtocolForFileTransmission::sendDataBlock, fileID, 0, info->size);
        }else{
            info->stream->device()->close();
            delete info->stream;
            delete info;
            info = 0;
            filesToBeUploaded.remove(fileID);
        }

        emit peerResponseUploadFile(socketID, info->localLocation, accept, errorCode, message);

    }
        break;
    case quint8(REQUEST_DOWNLOAD_TYPE):
    {
        QString filePath = "";
        in >> filePath;


    }
        break;
    case quint8(RESPONSE_DOWNLOAD_TYPE):
    {

    }
        break;
    case quint8(REQUEST_DATA_BLOCK_TYPE):
    {

    }
        break;
    case quint8(BLOCK_DATA_TYPE):
    {

    }
        break;
    case quint8(CHANGE_TX_STATUS_TYPE):
    {

    }
        break;
    case quint8(ERROR_INFO_Type):
    {

    }
        break;
    default:
        break;

    }



}















}
