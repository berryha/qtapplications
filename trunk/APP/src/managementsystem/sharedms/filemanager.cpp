/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include "filemanager.h"

#include <QByteArray>
#include <QDir>
#include <QFile>
#include <QTimer>
#include <QTimerEvent>
#include <QCryptographicHash>
#include <QSettings>

#define SUFFIX_TEMP_FILE "tf"
#define SUFFIX_INFO_FILE "fi"







FileManager::FileManager(QObject *parent)
    : QThread(parent)
{
    quit = false;
    readId = 0;
//    startVerification = false;
    wokeUp = false;

    //qsrand(QDateTime::currentDateTime().toTime_t());

//    qRegisterMetaType<FileManager::Error>("FileManager::Error");
}

FileManager::~FileManager()
{
    quit = true;
    cond.wakeOne();
    wait();



    foreach (FileMetaInfo *metaInfo, fileMetaInfoHash) {
            metaInfo->file->close();
            delete metaInfo->file;
            //fileMetaInfoHash.remove(metaInfo->md5sum);
            delete metaInfo;
    }
    fileMetaInfoHash.clear();


}



int FileManager::readPiece(const QByteArray &fileMD5, int pieceIndex)
{
    ReadRequest request;
    //request.fileID = pieceIndex;
    request.fileMD5 = fileMD5;
    request.pieceIndex = pieceIndex;
    //request.length = length;

    QMutexLocker locker(&mutex);
    request.id = readId++;
    readRequests << request;

    if (!wokeUp) {
        wokeUp = true;
        QMetaObject::invokeMethod(this, "wakeUp", Qt::QueuedConnection);
    }

    return request.id;
}

void FileManager::writePiece(const QByteArray &fileMD5, int pieceIndex, const QByteArray &data, const QByteArray &dataSHA1SUM)
{
    WriteRequest request;
    //request.fileID = fileID;
    request.fileMD5 = fileMD5;
    request.pieceIndex = pieceIndex;
    request.data = data;
    request.dataSHA1SUM = dataSHA1SUM;

    QMutexLocker locker(&mutex);
    writeRequests << request;

    if (!wokeUp) {
        wokeUp = true;
        QMetaObject::invokeMethod(this, "wakeUp", Qt::QueuedConnection);
    }
}

//void FileManager::verifyPiece(QByteArray fileMD5, int pieceIndex)
//{
//    QMutexLocker locker(&mutex);

//    FileMetaInfo *info = fileMetaInfoHash.value(fileMD5);
//    if(!info){
//        return;
//    }
//    info->pendingVerificationPieces << pieceIndex;

//    if(!pendingVerificationRequests.contains(fileMD5)){
//        pendingVerificationRequests << fileMD5;
//    }

//    startVerification = true;

//    if (!wokeUp) {
//        wokeUp = true;
//        QMetaObject::invokeMethod(this, "wakeUp", Qt::QueuedConnection);
//    }
//}



//QBitArray FileManager::completedPieces(const QByteArray &fileMD5) const
//{
//    QMutexLocker locker(&mutex);

//    FileMetaInfo * info = fileMetaInfoHash.value(fileMD5);
//    if(!info){
//        return QBitArray();
//    }

//    return info->verifiedPieces;
//}

//void FileManager::setCompletedPieces(const QByteArray &fileMD5, const QBitArray &pieces)
//{
//    QMutexLocker locker(&mutex);

//    FileMetaInfo * info = fileMetaInfoHash.value(fileMD5);
//    if(!info){
//        return ;
//    }
//    info->verifiedPieces = pieces;

//}

QList<int/*Piece Index*/> FileManager::uncompletedPieces(const QByteArray &fileMD5){

    QMutexLocker locker(&mutex);

    QList<int> ret;
    FileMetaInfo * info = fileMetaInfoHash.value(fileMD5);
    if(!info){
        return ret;
    }

    for(int i=0; i< info->verifiedPieces.size(); i++){
        if(!info->verifiedPieces.testBit(i)){
            ret << i;
        }
    }

    return ret;

}

int FileManager::getOneUncompletedPiece(const QByteArray &fileMD5){

    QMutexLocker locker(&mutex);

    FileMetaInfo * info = fileMetaInfoHash.value(fileMD5);
    if(!info){
        return -1;
    }

    for(int i=0; i< info->verifiedPieces.size(); i++){
        if(!info->verifiedPieces.testBit(i)){
            return i;
        }
    }

}



//QString FileManager::errorString() const
//{
//    return errString;
//}

void FileManager::run()
{


    do {
        {
            // Go to sleep if there's nothing to do.
            QMutexLocker locker(&mutex);
            if (!quit && readRequests.isEmpty() && writeRequests.isEmpty() )
                cond.wait(&mutex);
        }

        // Read pending read requests
        mutex.lock();
        QList<ReadRequest> newReadRequests = readRequests;
        readRequests.clear();
        mutex.unlock();
        while (!newReadRequests.isEmpty()) {
            ReadRequest request = newReadRequests.takeFirst();
            FileMetaInfo *info = fileMetaInfoHash.value(request.fileMD5) ;
            if(!info){continue;}
            QByteArray block = readBlock(request.id, info, request.pieceIndex);
            if(block.isEmpty()){continue;}
            QByteArray dataSHA1SUM = QCryptographicHash::hash(block, QCryptographicHash::Sha1);
            emit dataRead(request.id, request.fileMD5, request.pieceIndex, block, dataSHA1SUM);
        }

        // Write pending write requests
        mutex.lock();
        QList<WriteRequest> newWriteRequests = writeRequests;
        writeRequests.clear();
        while (!quit && !newWriteRequests.isEmpty()) {
            WriteRequest request = newWriteRequests.takeFirst();
            FileMetaInfo *info = fileMetaInfoHash.value(request.fileMD5) ;
            if(!info){continue;}
            writeBlock(info, request.pieceIndex, request.data);
        }

        // Process pending verification requests
//        if (startVerification) {
//            newPendingVerificationRequests = pendingVerificationRequests;
//            pendingVerificationRequests.clear();
////            verifyFileContents();
//            VerifyAllPendingFiles();
//            startVerification = false;
//        }
        mutex.unlock();
//        newPendingVerificationRequests.clear();

    } while (!quit);

    // Write pending write requests
    mutex.lock();
    QList<WriteRequest> newWriteRequests = writeRequests;
    writeRequests.clear();
    mutex.unlock();
    while (!newWriteRequests.isEmpty()) {
        WriteRequest request = newWriteRequests.takeFirst();
        FileMetaInfo *info = fileMetaInfoHash.value(request.fileMD5) ;
        if(!info){continue;}
        writeBlock(info, request.pieceIndex, request.data);
    }
}

//void FileManager::startDataVerification()
//{
//    QMutexLocker locker(&mutex);
//    startVerification = true;
//    cond.wakeOne();
//}

const FileManager::FileMetaInfo * FileManager::tryToSendFile( const QString &localSavePath, QString *errorString/*, int pieceLength*/){

    Q_ASSERT(errorString);

    {
        QMutexLocker locker(&mutex);
        foreach (FileMetaInfo *metaInfo, fileMetaInfoHash) {
            if(metaInfo->file->fileName() == localSavePath){
                //QString errString = tr("File '%1' is in use!").arg(localSavePath);
                //emit error(FILE_READ_ERROR, errString);
                return metaInfo;
            }
        }
    }

    if(!QFile::exists(localSavePath)){
        *errorString = tr("File '%1' does not exist! %2").arg(localSavePath);
        //emit error(ERROR_FILE_NOT_EXIST, errString);
        return 0;
    }

    QFile *file = new QFile(localSavePath);
    if (!file->open(QFile::ReadWrite)) {
        *errorString = tr("Failed to open file %1! %2").arg(localSavePath).arg(file->errorString());
        //emit error(FILE_READ_ERROR, errString);
        delete file;
        return 0;
    }

    QByteArray fileMD5Sum = QCryptographicHash::hash(file->readAll(), QCryptographicHash::Md5);
    file->seek(0);
    qint64 fileSize = file->size();
    QHash<int/*Piece Index*/, QByteArray/*SHA1 Hash*/> sha1Sums;
    int pieceIndex = 0;
    QByteArray block;
    while (!file->atEnd()) {
        block.clear();
        qint64 sizeToRead = qMin<qint64>(FILE_PIECE_LENGTH, fileSize - file->pos());
        block.resize(sizeToRead);

        qint64 read = file->read(block.data(), sizeToRead);
        if(read != sizeToRead && !file->atEnd()){
            *errorString = tr("Failed to read data from file '%1'! %2").arg(localSavePath).arg(file->errorString());
            //emit error(FILE_READ_ERROR, errString);
            file->close();
            delete file;
            return 0;
        }
        sha1Sums.insert(pieceIndex, QCryptographicHash::hash(block, QCryptographicHash::Sha1));
        pieceIndex++;
    }

    FileMetaInfo *info = new FileMetaInfo();
    info->md5sum = fileMD5Sum;
    info->size = fileSize;
    //info->pieceLength = pieceLength;
    info->sha1Sums = sha1Sums;
    info->verifiedPieces.resize(sha1Sums.size());
    info->verifiedPieces.fill(true);
    //info->newFile = true;
    info->file = file;
    file->close();

    {
        QMutexLocker locker(&mutex);
        fileMetaInfoHash.insert(fileMD5Sum, info);
    }

    return info;

}

const FileManager::FileMetaInfo * FileManager::tryToReceiveFile(QByteArray fileMD5Sum, const QString &localSavePath, quint64 size, QString *errorString/*, int pieceLength*/){

    Q_ASSERT(errorString);

    QString tempFilePath = localSavePath + "." + SUFFIX_TEMP_FILE;
    QString infoFilePath = localSavePath + "." + SUFFIX_INFO_FILE;
    if(QFile::exists(localSavePath)){
        *errorString = tr("File '%1' already exists!").arg(localSavePath);
        //emit error(ERROR_FILE_EXIST, errString);
        return 0;
    }

    {
        QMutexLocker locker(&mutex);
        foreach (FileMetaInfo *metaInfo, fileMetaInfoHash) {
            if(metaInfo->file->fileName() == tempFilePath){
                if(metaInfo->md5sum == fileMD5Sum ){
                    return metaInfo;
                }else{
                    *errorString = tr("File '%1' is in use!").arg(localSavePath);
                    //emit error(ERROR_FILE_EXIST, errString);
                    return 0;
                }
            }
        }
    }

    QFile *file = new QFile(tempFilePath);
    if (!file->open(QFile::ReadWrite)) {
        *errorString = tr("Failed to open/create file %1: %2").arg(file->fileName()).arg(file->errorString());
        //emit error(FILE_READ_ERROR, errString);
        delete file;
        return 0;
    }


    QSettings settings(infoFilePath);
    QByteArray oldMD5 = settings.value("MD5", QByteArray()).toByteArray();
    quint64 oldSize = settings.value("size", 0).toULongLong();
    if(oldMD5 != fileMD5Sum || oldSize != size || file->size() == 0){
        file->resize(0);
        if (!file->resize(size)) {
            *errorString = tr("Failed to resize file %1: %2").arg(file->fileName()).arg(file->errorString());
            //emit error(FILE_WRITE_ERROR, errString);
            delete file;
            return 0;
        }
    }
    file->close();


    QBitArray ba = settings.value("Pieces", QBitArray()).toBitArray();
    if(ba.isEmpty()){
        int pieces = size / FILE_PIECE_LENGTH;
        if(size % FILE_PIECE_LENGTH){
            pieces++;
        }
        ba.resize(pieces);
        ba.fill(false);
    }

    settings.setValue("MD5", fileMD5Sum);
    settings.setValue("size", size);
    settings.setValue("Pieces", ba);
    settings.sync();

    FileMetaInfo *info = new FileMetaInfo();

    info->md5sum = fileMD5Sum;
    info->size = size;
    //info->pieceLength = pieceLength;
    info->verifiedPieces = ba;
    //info->newFile = true;
    info->file = file;

    {
        QMutexLocker locker(&mutex);
//        while(fileMetaInfoHash.contains(info->fileID)){
//            info->fileID = qrand();
//        }
        fileMetaInfoHash.insert(fileMD5Sum, info);
    }

    return info;

}

//bool FileManager::setFileMetaInfo(QByteArray fileMD5Sum, int pieceLength, const QList<QByteArray> &sha1Sums){

//    FileMetaInfo *info = fileMetaInfoHash.value(fileMD5Sum) ;
//    if(!info){return false;}

//    info->pieceLength = pieceLength;
//    info->sha1Sums = sha1Sums;

//}

//QByteArray FileManager::calcFileMD5(const QString &filePath){

//    QFile file(filePath);
//    if (!file.open(QFile::ReadOnly)) {
//        errString = tr("Failed to open file %1: %2").arg(filePath).arg(file.errorString());
//        emit error();
//        return QByteArray();
//    }

//    return QCryptographicHash::hash(file.readAll(), QCryptographicHash::Md5);

//}

void FileManager::closeFile(const QString &filePath){

    QMutexLocker locker(&mutex);
    foreach (FileMetaInfo *metaInfo, fileMetaInfoHash) {
        if(metaInfo->file->fileName() == filePath){
            metaInfo->file->close();
            delete metaInfo->file;
            fileMetaInfoHash.remove(metaInfo->md5sum);
            delete metaInfo;
            return;
        }
    }

    //errString = tr("File '%1' does not exist!").arg(filePath);


}

void FileManager::closeFile(const QByteArray &fileMD5){

    QMutexLocker locker(&mutex);
    FileMetaInfo *metaInfo = fileMetaInfoHash.take(fileMD5);
    if(metaInfo){
        metaInfo->file->close();
        delete metaInfo->file;
    }

    delete metaInfo;

}

//int FileManager::regenerateFileID(int oldFileID){

//    QMutexLocker locker(&mutex);
//    FileMetaInfo *info = fileMetaInfoHash.take(oldFileID);
//    if(!info){
//        return 0;
//    }


//    int id = info->fileID;

//    while(fileMetaInfoHash.contains(id) || id == 0){
//        id = qrand();
//    }
//    fileMetaInfoHash.insert(id, info);

//    if(pendingVerificationRequests.contains(info->fileID)){
//        pendingVerificationRequests.removeAll(id);
//        pendingVerificationRequests.append(id);
//    }
//    if(newPendingVerificationRequests.contains(info->fileID)){
//        newPendingVerificationRequests.removeAll(id);
//        newPendingVerificationRequests.append(id);
//    }

//    info->fileID = id;

//}

//QFile * FileManager::generateNewFile(quint64 size, const QString &localSavePath)
//{

//    // Set up the thread local data
//    QMutexLocker locker(&mutex);

//    //        QString prefix;
//    //        if (!destinationPath.isEmpty()) {
//    //            prefix = destinationPath;
//    //            if (!prefix.endsWith("/"))
//    //                prefix += "/";
//    //            QDir dir;
//    //            if (!dir.mkpath(prefix)) {
//    //                errString = tr("Failed to create directory %1").arg(prefix);
//    //                emit error();
//    //                return false;
//    //            }
//    //        }
//    QFile *file = new QFile(localSavePath);
//    if (!file->open(QFile::ReadWrite)) {
//        errString = tr("Failed to open/create file %1: %2").arg(file->fileName()).arg(file->errorString());
//        emit error();
//        return 0;
//    }
//    if (file->size() != size) {
//        newFile = true;
//        if (!file->resize(size)) {
//            errString = tr("Failed to resize file %1: %2").arg(file->fileName()).arg(file->errorString());
//            emit error();
//            return 0;
//        }
//    }
//    file->close();

//    return file;
//}

QByteArray FileManager::readBlock(int requestID, FileMetaInfo *info, int pieceIndex)
{

    QByteArray block;

    if(!info){
        QString errString = tr("Failed to read file info!");
        emit error(requestID, info->md5sum, quint8(ERROR_UNKNOWN), errString);
        return block;
    }


    qint64 startReadIndex = (quint64(pieceIndex) * FILE_PIECE_LENGTH);

    QFile *file = info->file;
    qint64 currentFileSize = info->size;
    if (!file->isOpen()) {
        if (!file->open(QFile::ReadWrite)) {
            QString errString = tr("Failed to read data from file '%1'! %2").arg(file->fileName()).arg(file->errorString());
            emit error(requestID, info->md5sum, quint8(FILE_READ_ERROR), errString);
            return block;
        }
    }

    file->seek(startReadIndex);
    qint64 sizeToRead = qMin<qint64>(FILE_PIECE_LENGTH, currentFileSize - file->pos());
    block = file->read(sizeToRead);
    file->close();

    if (block.size() != sizeToRead) {
        QString errString = tr("Failed to read data from file '%1'! %2").arg(file->fileName()).arg(file->errorString());
        emit error(requestID, info->md5sum, quint8(FILE_READ_ERROR), errString);
        return QByteArray();
    }

    return block;
}

bool FileManager::writeBlock(FileMetaInfo *info, int pieceIndex, const QByteArray &data)
{

    if(!info){
        QString errString = tr("Failed to read file info!");
        emit error(0, info->md5sum, quint8(ERROR_UNKNOWN), errString);
        return false;
    }


    qint64 startWriteIndex = (qint64(pieceIndex) * FILE_PIECE_LENGTH);
    int bytesToWrite = data.size();

    QFile *file = info->file;
    qint64 currentFileSize = info->size;

    if (!file->isOpen()) {
        if (!file->open(QFile::ReadWrite)) {
            QString errString = tr("Failed to open file '%1'! %2").arg(file->fileName()).arg(file->errorString());
            emit error(0, info->md5sum, quint8(FILE_READ_ERROR), errString);
            return false;
        }
    }

    file->seek(startWriteIndex);
    qint64 bytesWritten = file->write(data.constData(), qMin<qint64>(bytesToWrite, currentFileSize - file->pos()));
    file->close();

    if (bytesWritten <= 0) {
        QString errString = tr("Failed to write data to file '%1'! %2").arg(file->fileName()).arg(file->errorString());
        emit error(0, info->md5sum, quint8(FILE_WRITE_ERROR), errString);
        return false;
    }

    return verifySinglePiece(info, pieceIndex);


}

//void FileManager::verifyFileContents(FileMetaInfo *info)
//{

//    if(!info){
//        errString = tr("Failed to read file info!");
//        emit error();
//        return;
//    }

//    // Verify all pieces the first time
////    if (info->pendingVerificationPieces.isEmpty()) {
//        if (info->verifiedPieces.count(true) == 0) {
//            info->verifiedPieces.resize(info->sha1Sums.size());

//            int oldPercent = 0;
//            int numPieces = info->sha1Sums.size();

//            for (int index = 0; index < numPieces; ++index) {
//                verifySinglePiece(info, index);

//                int percent = ((index + 1) * 100) / numPieces;
//                if (oldPercent != percent) {
//                    emit verificationProgress(info->md5sum, percent);
//                    oldPercent = percent;
//                }
//            }

//        }
//        emit verificationDone(info->md5sum);
//        return;
////    }else{
////        foreach (int pieceIndex, info->pendingVerificationPieces) {
////            verifySinglePiece(info, pieceIndex);
////        }
////        info->pendingVerificationPieces.clear();
////    }

//}

//void FileManager::VerifyAllPendingFiles(){

//    // Verify all pending pieces
//    foreach (QByteArray fileMD5, newPendingVerificationRequests){
//        FileMetaInfo *info = fileMetaInfoHash.value(fileMD5);
//        if(!info){
//            errString = tr("Failed to read file info!");
//            emit error();
//            continue;
//        }
//        verifyFileContents(info);

//    }
//    newPendingVerificationRequests.clear();


//}

bool FileManager::verifySinglePiece(FileMetaInfo *info, int pieceIndex)
{

    if(!info){
        return false;
    }

    //qint64 startReadIndex = (quint64(pieceIndex) * FILE_PIECE_LENGTH);

    QByteArray block = readBlock(0, info, pieceIndex);
    QByteArray sha1Sum = QCryptographicHash::hash(block, QCryptographicHash::Sha1);

    bool verified = false;
    if (sha1Sum != info->sha1Sums.value(pieceIndex)){
        verified = false;
    }else{
        info->verifiedPieces.setBit(pieceIndex);
        verified = true;
    }

    int verificationProgress = (info->verifiedPieces.count(true) * 100) / info->verifiedPieces.size();
    Q_ASSERT(verificationProgress >= 0 && verificationProgress <= 100);

    emit pieceVerified(info->md5sum, pieceIndex, verified, verificationProgress);

    return verified;
}

void FileManager::wakeUp()
{
    QMutexLocker locker(&mutex);
    wokeUp = false;
    cond.wakeOne();
}
