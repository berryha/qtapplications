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
    startVerification = false;
    wokeUp = false;

    //qsrand(QDateTime::currentDateTime().toTime_t());

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

int FileManager::read(QByteArray fileMD5, int offset, int length)
{
    ReadRequest request;
    //request.fileID = pieceIndex;
    request.md5sum = fileMD5;
    request.offset = offset;
    request.length = length;

    QMutexLocker locker(&mutex);
    request.id = readId++;
    readRequests << request;

    if (!wokeUp) {
        wokeUp = true;
        QMetaObject::invokeMethod(this, "wakeUp", Qt::QueuedConnection);
    }

    return request.id;
}

void FileManager::write(QByteArray fileMD5, int offset, const QByteArray &data)
{
    WriteRequest request;
    //request.fileID = fileID;
    request.md5sum = fileMD5;
    request.offset = offset;
    request.data = data;

    QMutexLocker locker(&mutex);
    writeRequests << request;

    if (!wokeUp) {
        wokeUp = true;
        QMetaObject::invokeMethod(this, "wakeUp", Qt::QueuedConnection);
    }
}

void FileManager::verifyPiece(QByteArray fileMD5, int pieceIndex)
{
    QMutexLocker locker(&mutex);

    FileMetaInfo *info = fileMetaInfoHash.value(fileMD5);
    if(!info){
        return;
    }
    info->pendingVerificationPieces << pieceIndex;

    if(!pendingVerificationRequests.contains(fileMD5)){
        pendingVerificationRequests << fileMD5;
    }

    startVerification = true;

    if (!wokeUp) {
        wokeUp = true;
        QMetaObject::invokeMethod(this, "wakeUp", Qt::QueuedConnection);
    }
}



QBitArray FileManager::completedPieces(QByteArray fileMD5) const
{
    QMutexLocker locker(&mutex);

    FileMetaInfo * info = fileMetaInfoHash.value(fileMD5);
    if(!info){
        return QBitArray();
    }

    return info->verifiedPieces;
}

void FileManager::setCompletedPieces(QByteArray fileMD5, const QBitArray &pieces)
{
    QMutexLocker locker(&mutex);

    FileMetaInfo * info = fileMetaInfoHash.value(fileMD5);
    if(!info){
        return ;
    }
    info->verifiedPieces = pieces;

}

QString FileManager::errorString() const
{
    return errString;
}

void FileManager::run()
{


    do {
        {
            // Go to sleep if there's nothing to do.
            QMutexLocker locker(&mutex);
            if (!quit && readRequests.isEmpty() && writeRequests.isEmpty() && !startVerification)
                cond.wait(&mutex);
        }

        // Read pending read requests
        mutex.lock();
        QList<ReadRequest> newReadRequests = readRequests;
        readRequests.clear();
        mutex.unlock();
        while (!newReadRequests.isEmpty()) {
            ReadRequest request = newReadRequests.takeFirst();
            FileMetaInfo *info = fileMetaInfoHash.value(request.md5sum) ;
            if(!info){continue;}
            QByteArray block = readBlock(info, 0, request.offset, request.length);
            emit dataRead(request.id, request.md5sum, request.offset, block);
        }

        // Write pending write requests
        mutex.lock();
        QList<WriteRequest> newWriteRequests = writeRequests;
        writeRequests.clear();
        while (!quit && !newWriteRequests.isEmpty()) {
            WriteRequest request = newWriteRequests.takeFirst();
            FileMetaInfo *info = fileMetaInfoHash.value(request.md5sum) ;
            if(!info){continue;}
            writeBlock(info, 0, request.offset, request.data);
        }

        // Process pending verification requests
        if (startVerification) {
            newPendingVerificationRequests = pendingVerificationRequests;
            pendingVerificationRequests.clear();
//            verifyFileContents();
            VerifyAllPendingFiles();
            startVerification = false;
        }
        mutex.unlock();
        newPendingVerificationRequests.clear();

    } while (!quit);

    // Write pending write requests
    mutex.lock();
    QList<WriteRequest> newWriteRequests = writeRequests;
    writeRequests.clear();
    mutex.unlock();
    while (!newWriteRequests.isEmpty()) {
        WriteRequest request = newWriteRequests.takeFirst();
        FileMetaInfo *info = fileMetaInfoHash.value(request.md5sum) ;
        if(!info){continue;}
        writeBlock(info, 0, request.offset, request.data);
    }
}

void FileManager::startDataVerification()
{
    QMutexLocker locker(&mutex);
    startVerification = true;
    cond.wakeOne();
}

const FileManager::FileMetaInfo * FileManager::tryReceiveFile(QByteArray fileMD5Sum, const QString &localSavePath, quint64 size){

    FileMetaInfo *info = new FileMetaInfo();
    QString tempFilePath = localSavePath + "." + SUFFIX_TEMP_FILE;
    QString infoFilePath = localSavePath + "." + SUFFIX_INFO_FILE;
    if(QFile::exists(localSavePath)){
        errString = tr("File '%1' already exists!").arg(localSavePath);
        delete info;
        return 0;
    }

    {
        QMutexLocker locker(&mutex);
        foreach (FileMetaInfo *metaInfo, fileMetaInfoHash) {
            if(metaInfo->md5sum == fileMD5Sum || metaInfo->file->fileName() == tempFilePath){
                errString = tr("File '%1' is in use!").arg(localSavePath);
                delete info;
                return 0;
            }
        }
    }

    QFile *file = new QFile(tempFilePath);
    if (!file->open(QFile::ReadWrite)) {
        errString = tr("Failed to open/create file %1: %2").arg(file->fileName()).arg(file->errorString());
        emit error();
        delete file;
        delete info;
        return 0;
    }


    QSettings settings(infoFilePath);
    quint64 oldSize = settings.value("size", 0).toULongLong();
    if(oldSize != size || file->size() == 0){
        file->resize(0);
        if (!file->resize(size)) {
            errString = tr("Failed to resize file %1: %2").arg(file->fileName()).arg(file->errorString());
            emit error();
            delete file;
            delete info;
            return 0;
        }
    }
    file->close();

    settings.setValue("size", size);
    settings.sync();

    info->md5sum = fileMD5Sum;
    info->size = size;
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

QByteArray FileManager::calcFileMD5(const QString &filePath){

    QFile file(filePath);
    if (!file.open(QFile::ReadOnly)) {
        errString = tr("Failed to open file %1: %2").arg(filePath).arg(file.errorString());
        emit error();
        return QByteArray();
    }

    return QCryptographicHash::hash(file.readAll(), QCryptographicHash::Md5);

}

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

    errString = tr("File '%1' does not exist!").arg(filePath);


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

QByteArray FileManager::readBlock(FileMetaInfo *info, int pieceIndex, int offset, int length)
{

    QByteArray block;

    if(!info){
        errString = tr("Failed to read file info!");
        emit error();
        return block;
    }


    qint64 startReadIndex = (quint64(pieceIndex) * info->pieceLength) + offset;

    QFile *file = info->file;
    qint64 currentFileSize = info->size;
    if (!file->isOpen()) {
        if (!file->open(QFile::ReadWrite)) {
            errString = tr("Failed to read from file %1: %2").arg(file->fileName()).arg(file->errorString());
            emit error();
            return block;
        }
    }

    file->seek(startReadIndex);
    block = file->read(qMin<qint64>(length, currentFileSize - file->pos()));
    file->close();

    if (block.size() < 0) {
        errString = tr("Failed to read from file %1 (read %3 bytes): %2").arg(file->fileName()).arg(file->errorString()).arg(length);
        emit error();
        return QByteArray();
    }

    return block;
}

bool FileManager::writeBlock(FileMetaInfo *info, int pieceIndex, int offset, const QByteArray &data)
{

    if(!info){
        errString = tr("Failed to read file info!");
        emit error();
        return false;
    }

    qint64 startWriteIndex = (qint64(pieceIndex) * info->pieceLength) + offset;
    int bytesToWrite = data.size();

    QFile *file = info->file;
    qint64 currentFileSize = info->size;

    if (!file->isOpen()) {
        if (!file->open(QFile::ReadWrite)) {
            errString = tr("Failed to write to file %1: %2").arg(file->fileName()).arg(file->errorString());
            emit error();
            return false;
        }
    }

    file->seek(startWriteIndex);
    qint64 bytesWritten = file->write(data.constData(), qMin<qint64>(bytesToWrite, currentFileSize - file->pos()));
    file->close();

    if (bytesWritten <= 0) {
        errString = tr("Failed to write to file %1: %2").arg(file->fileName()).arg(file->errorString());
        emit error();
        return false;
    }

    return true;
}

void FileManager::verifyFileContents(FileMetaInfo *info)
{

    if(!info){
        errString = tr("Failed to read file info!");
        emit error();
        return;
    }

    // Verify all pieces the first time
    if (info->pendingVerificationPieces.isEmpty()) {
        if (info->verifiedPieces.count(true) == 0) {
            info->verifiedPieces.resize(info->sha1Sums.size());

            int oldPercent = 0;
            int numPieces = info->sha1Sums.size();

            for (int index = 0; index < numPieces; ++index) {
                verifySinglePiece(info, index);

                int percent = ((index + 1) * 100) / numPieces;
                if (oldPercent != percent) {
                    emit verificationProgress(info->md5sum, percent);
                    oldPercent = percent;
                }
            }

        }
        emit verificationDone(info->md5sum);
        return;
    }else{
        foreach (int pieceIndex, info->pendingVerificationPieces) {
            verifySinglePiece(info, pieceIndex);
        }
        info->pendingVerificationPieces.clear();
    }

}

void FileManager::VerifyAllPendingFiles(){

    // Verify all pending pieces
    foreach (QByteArray fileMD5, newPendingVerificationRequests){
        FileMetaInfo *info = fileMetaInfoHash.value(fileMD5);
        if(!info){
            errString = tr("Failed to read file info!");
            emit error();
            continue;
        }
        verifyFileContents(info);

    }
    newPendingVerificationRequests.clear();


}

bool FileManager::verifySinglePiece(FileMetaInfo *info, int pieceIndex)
{

    if(!info){
        return false;
    }

    QByteArray block = readBlock(info, pieceIndex, 0, info->pieceLength);
    QByteArray sha1Sum = QCryptographicHash::hash(block, QCryptographicHash::Sha1);

    bool verified = false;
    if (sha1Sum != info->sha1Sums.at(pieceIndex)){
        verified = false;
    }else{
        info->verifiedPieces.setBit(pieceIndex);
        verified = true;
    }

    emit pieceVerified(info->md5sum, pieceIndex, verified);

    return verified;
}

void FileManager::wakeUp()
{
    QMutexLocker locker(&mutex);
    wokeUp = false;
    cond.wakeOne();
}
