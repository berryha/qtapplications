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

#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QBitArray>
#include <QHash>
#include <QMutex>
#include <QThread>
#include <QWaitCondition>



#include "sharedmslib.h"


QT_BEGIN_NAMESPACE
class QByteArray;
class QFile;
class QTimerEvent;
QT_END_NAMESPACE

class SHAREDMSLIB_API FileManager : public QThread
{
    Q_OBJECT

public:
    enum Error{ERROR_NO_ERROR = 0, ERROR_UNKNOWN, ERROR_FILE_EXIST, ERROR_FILE_NOT_EXIST, ERROR_FILE_IN_USE, FILE_READ_ERROR, FILE_WRITE_ERROR};
    struct FileMetaInfo
    {
        FileMetaInfo(){
            //fileID = qrand();
            md5sum.resize(0);
            size = 0;
            //name = "";
            //newFile = false;
            //pieceLength = FILE_PIECE_LENGTH;
            sha1Sums.clear();
            //verifiedPieces.fill(false);
            file = 0;
            infoFileName = "";
            //pendingVerificationPieces.clear();
        }

        //int fileID;
        QByteArray md5sum;
        quint64 size;
        //QString name;
        //bool newFile;
        //int pieceLength;
        QHash<int/*Piece Index*/, QByteArray/*SHA1 Hash*/> sha1Sums;
        QBitArray verifiedPieces;

        QFile *file;
        QString infoFileName;
        //QString destinationPath;
        //QList<int/*Piece index*/> pendingVerificationPieces;
    };

    FileManager(QObject *parent = 0);
    virtual ~FileManager();

    const FileMetaInfo * tryToSendFile(const QString &localSavePath, QString *errorString/*, int pieceLength = FILE_PIECE_LENGTH*/);
    const FileMetaInfo * tryToReceiveFile(QByteArray fileMD5Sum, const QString &localSavePath, quint64 size, QString *errorString/*, int pieceLength = FILE_PIECE_LENGTH*/);

    int readPiece(const QByteArray &fileMD5, int pieceIndex);
    void writePiece(const QByteArray &fileMD5, int pieceIndex, const QByteArray &data, const QByteArray &dataSHA1SUM);
//    void verifyPiece(QByteArray fileMD5, int pieceIndex);

//    QBitArray completedPieces(const QByteArray &fileMD5) const;
//    void setCompletedPieces(const QByteArray &fileMD5, const QBitArray &pieces);

    QList<int/*Piece Index*/> completedPieces(const QByteArray &fileMD5);
    QList<int/*Piece Index*/> uncompletedPieces(const QByteArray &fileMD5);
    int getOneUncompletedPiece(const QByteArray &fileMD5);

    //    QByteArray calcFileMD5(const QString &filePath);

    void closeFile(const QString &filePath);
    void closeFile(const QByteArray &fileMD5);

    QString getFileLocalSavePath(const QByteArray &fileMD5);

//    int regenerateFileID(int oldFileID);
//    QFile *generateNewFile(quint64 size, const QString &localSavePath);

//    QString errorString() const;

public slots:
//    void startDataVerification();

signals:
    void dataRead(int requestID, const QByteArray &fileMD5, int pieceIndex, const QByteArray &data, const QByteArray &dataSHA1SUM);
    void error(int requestID, const QByteArray &fileMD5, quint8 errorCode, const QString &errorString);
//    void verificationProgress(QByteArray fileMD5, int percent);
//    void verificationDone(QByteArray fileMD5);
    void pieceVerified(const QByteArray &fileMD5, int pieceIndex, bool verified, int verificationProgress);

protected:
    void run();

private slots:
    void wakeUp();

private:
    typedef QByteArray MD5 ; //File MD5 Hash

    QHash<MD5/*File MD5*/ , FileMetaInfo *> fileMetaInfoHash;


    QByteArray readBlock(int requestID, FileMetaInfo *info, int pieceIndex);
    bool writeBlock(FileMetaInfo *info, int pieceIndex, const QByteArray &data, const QByteArray &dataSHA1SUM);
    bool verifySinglePiece(FileMetaInfo *info, int pieceIndex);
//    void verifyFileContents(FileMetaInfo *info);
//    void VerifyAllPendingFiles();

    struct WriteRequest {
        //int fileID;
        QByteArray fileMD5;
        int pieceIndex;
        QByteArray data;
        QByteArray dataSHA1SUM;
    };
    struct ReadRequest {
        //int fileID;
        QByteArray fileMD5;
        int pieceIndex;
        //int length;
        int id;
    };


//    QString errString;



    int readId;
//    bool startVerification;
    bool m_quit;
    bool m_wokeUp;

    QList<WriteRequest> writeRequests;
    QList<ReadRequest> readRequests;
//    QList<MD5/*File MD5*/> pendingVerificationRequests;
//    QList<MD5/*File MD5*/> newPendingVerificationRequests;

    mutable QMutex mutex;
    mutable QWaitCondition cond;

};

#endif
