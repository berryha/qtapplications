#include "filemanager.h"

#include <QFile>
#include <QFileInfo>
#include <QtConcurrentRun>
#include <QDateTime>

namespace HEHUI {



UDTProtocolForFileTX::UDTProtocolForFileTX(bool stream, const SocketOptions *options, QObject *parent) :
    UDTProtocolBase(stream, options, parent)
{


}

void UDTProtocolForFileTX::streamDataReceived(UDTSOCKET socket, QByteArray *data){
    qDebug()<<"--UDTProtocol::streamDataReceived(...) "<<"socket:"<<socket;

    emit dataReceived(socket, data, true);

}

void UDTProtocolForFileTX::messageDataReceived(UDTSOCKET socket, QByteArray *data){
    qDebug()<<"--UDTProtocol::messageDataReceived(...) "<<"socket:"<<socket;

    emit dataReceived(socket, data, false);

}


////////////////////////////////////////////////////


FileManager::FileManager(QObject *parent) :
    QObject(parent)
{

    m_blockSize = 1500;

    m_ready = false;

    m_udtProtocol = new UDTProtocolForFileTX(true, 0, this);
    UDTSOCKET socket = m_udtProtocol->listen();
    if(socket == UDTProtocolBase::INVALID_UDT_SOCK){
        delete m_udtProtocol;
        m_udtProtocol = 0;
        qCritical()<<m_udtProtocol->getLastErrorMessage();
    }else{
        m_ready = true;
    }

    qsrand(QDateTime::currentDateTime().toTime_t());

}

FileManager::~FileManager(){

    m_udtProtocol->closeUDTProtocol();
    delete m_udtProtocol;
}

bool FileManager::isReady(){
    return m_ready;
}

bool FileManager::requestToTXFile(int socketID, const QString &filePath){

    QFileInfo fi(filePath);
    if(!fi.exists()){
        emit error(quint8(FileNotExistError), tr("File '%1' does not exist!").arg(filePath));
        return false;
    }

    QFile *file = new QFile(filePath);
    if(!file->open(QIODevice::ReadOnly)){
        emit error(quint8(ReadError), file->errorString());
        return false;
    }

    FileInfo *info = getToBeSentFileInfo(socketID, filePath);
    if(!info){
        FileInfo *info = new FileInfo();
        info->socketID = socketID;
        info->localLocation = filePath;
        info->size = file->size();
        info->status = WaitingForStart_Status;
        //info->checksum = qChecksum()
        info->stream = new QDataStream(file);
        filesToBeSent.insert(info->fileID, info);

    }else{
        //TODO
    }



    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);
    out << quint8(REQUEST_TX_Type) << info->fileID << fi.fileName() << file->size();

    return m_udtProtocol->sendData(socketID, &ba);


}

bool FileManager::responseTXFile(int socketID, int fileID, bool accept, quint8 errorCode, const QString &message){

    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);
    out << quint8(RESPONSE_TX_Type) << fileID << accept << errorCode << message;

    return m_udtProtocol->sendData(socketID, &ba);

}

bool FileManager::requestToRXFile(int socketID, const QString &filePath){

    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);
    out << quint8(REQUEST_RX_Type) << filePath;

    return m_udtProtocol->sendData(socketID, &ba);

}

bool FileManager::responseRXFile(int socketID, const QString &filePath, bool accept, quint8 errorCode, const QString &message){

    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);
    out << quint8(RESPONSE_RX_Type) << filePath << accept << errorCode << message;

    return m_udtProtocol->sendData(socketID, &ba);

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

//        ok =  m_udtProtocol->sendData(info->socketID, &ba);
//        if(!ok){
//            info->status = Stop_Status;
//            emit error(quint8(NetworkError), m_udtProtocol->getLastErrorMessage());
//            return false;
//        }

//    }

//}


bool FileManager::requestDataBlock(int socketID, int fileID, quint64 offset, quint64 length){

    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);
    out << quint8(REQUEST_DATA_BLOCK_TYPE) << fileID << offset << length;

    return m_udtProtocol->sendData(socketID, &ba);

}

bool FileManager::sendDataBlock(int fileID, quint64 offset, quint64 length){

    FileInfo *info = filesToBeSent.value(fileID);
    if(!info){
        emit error(quint8(UnknownError), tr("File Info does not exist!"));
        return false;
    }

    if( (offset + length) > info->size){
        emit error(quint8(), tr("Length out of file size!"));
        return false;
    }

    QDataStream *in = info->stream;
    QIODevice *inDev = in->device();
    inDev->seek(offset);
    info->startOffset = offset;
    info->status = TX_Status;
    QByteArray fragmentData;

    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);

    bool ok = false;
    int socketID = info->socketID;
    int blockSize = m_blockSize - sizeof(quint8) - sizeof(int) - sizeof(qint64) - sizeof(quint16);
    quint64 sentSize = 0;
    while ( sentSize < length && (info->status == TX_Status) ) {
        fragmentData.resize(blockSize);
        qint64 size = inDev->read(fragmentData.data(), blockSize);
        if(size == -1){
            info->status = Stop_Status;
            emit error(quint8(ReadError), inDev->errorString());
            return false;
        }
        if(inDev->atEnd()){
            fragmentData.resize(size);
        }

        ba.clear();
        out.device()->seek(0);
        out << quint8(BLOCK_DATA_Type) << fileID << size << qChecksum(fragmentData.data(), size) << fragmentData;

        ok =  m_udtProtocol->sendData(socketID, &ba);
        if(!ok){
            info->status = Stop_Status;
            emit error(quint8(NetworkError), m_udtProtocol->getLastErrorMessage());
            return false;
        }else{
            sentSize += size;
        }

    }

    info->status = Done_status;
    return true;

}

FileManager::FileInfo * FileManager::getToBeSentFileInfo(int socketID, const QString &filePath){

    foreach (FileInfo *info, filesToBeSent.values()) {
        if( info->socketID == socketID && info->localLocation == filePath){
            return info;
        }
    }

    return 0;

}

FileManager::FileInfo * FileManager::getToBeReceivedFileInfo(int socketID, int fileID){

    foreach (FileInfo *info, filesToBeReceived.values(fileID)) {
        if(info->socketID == socketID){
            return info;
        }

    }

    return 0;

}

void FileManager::parseIncomingData(int socket, QByteArray *data, bool stream){

    QDataStream in(data, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_6);

    quint8 dataType = quint8(UNKNOWN_TYPE);

    switch(dataType){
    case quint8(REQUEST_TX_Type):
    {
        int fileID = 0;
        QString fileName = "";
        qint64 size = 0;
        in >> fileID >> fileName >> size;

        FileInfo *info = filesToBeReceived.value(fileID);
        if(!info){
            info = new FileInfo();
            info->fileID = fileID;
            info->size = size;
        }

        emit peerRequestToTxFile(fileID, fileName, size);

    }
        break;
    case quint8(RESPONSE_TX_Type):
    {
        int fileID = 0;
        bool accept = false;
        quint8 errorCode = quint8(NoError);
        QString message = "";
        in >> fileID >> accept >> errorCode >> message;

        FileInfo *info = filesToBeSent.value(fileID);
        if(!info){return;}
        if(accept){
            QtConcurrent::run(this, &FileManager::sendDataBlock, fileID, 0, info->size);
        }else{
            info->stream->device()->close();
            delete info->stream;
            delete info;
            info = 0;
            filesToBeSent.remove(fileID);
        }

        emit peerResponseTxFile(socket, info->localLocation, accept, errorCode, message);

    }
        break;
    case quint8(REQUEST_RX_Type):
    {
        QString filePath = "";
        in >> filePath;


    }
        break;
    case quint8(RESPONSE_RX_Type):
    {

    }
        break;
    case quint8(REQUEST_DATA_BLOCK_TYPE):
    {

    }
        break;
    case quint8(BLOCK_DATA_Type):
    {

    }
        break;
    case quint8(PAUSE_TX_Type):
    {

    }
        break;
    case quint8(PAUSE_RX_Type):
    {

    }
        break;
    case quint8(CONTINUE_TX_Type):
    {

    }
        break;
    case quint8(CONTINUE_RX_Type):
    {

    }
        break;
    case quint8(STOP_TX_Type):
    {

    }
        break;
    case quint8(STOP_RX_Type):
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















} //namespace HEHUI
