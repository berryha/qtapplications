#ifndef LOGDEBUG_H
#define LOGDEBUG_H


//#if defined(LOG_DEBUG)
#include <QDebug>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QTime>
#include <QtCore/QMutex>
#if defined(Q_OS_WIN32)
#include <qt_windows.h>
#else
#include <unistd.h>
#include <stdlib.h>
#endif

static QFile* file = 0;

static void closeDebugLog()
{
    if (!file)
        return;
    QString ps(QTime::currentTime().toString("HH:mm:ss.zzz ") + QLatin1String("--- DEBUG LOG CLOSED ---\n\n"));
    //QString ps(QDateTime::currentDateTime().toString("yyyy.MM.dd HH:mm:ss.zzz ") + QLatin1String("--- DEBUG LOG CLOSED ---\n\n"));
    file->write(ps.toAscii());
    file->flush();
    file->close();
    delete file;
    file = 0;
}

void logDebug(QtMsgType type, const char* msg)
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    QString s(QTime::currentTime().toString("HH:mm:ss.zzz "));
    //QString s(QDateTime::currentDateTime().toString("yyyy.MM.dd HH:mm:ss.zzz "));
    s += QString("[%1] ").arg(
#if defined(Q_OS_WIN32)
                               GetCurrentProcessId());
#else
                               getpid());
#endif

    if (!file) {
        static QString logFilename = QFileInfo(QCoreApplication::applicationFilePath()).baseName() + QDateTime::currentDateTime().toString("-yyyy-MM") + ".log";
#if defined(Q_OS_WIN32)
        file = new QFile("./" + logFilename);
#else
        file = new QFile("/tmp/" + logFilename);
#endif
        if (!file->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
            delete file;
            file = 0;
            return;
        }
        //QString ps(QLatin1String("\n") + s + QLatin1String("--- DEBUG LOG OPENED ---\n"));
        QString ps(QDateTime::currentDateTime().toString("yyyy.MM.dd ") + QLatin1String("\n") + s + QLatin1String("--- DEBUG LOG OPENED ---\n"));
        file->write(ps.toAscii());
    }

    switch (type) {
    case QtWarningMsg:
        s += QLatin1String("WARNING: ");
        break;
    case QtCriticalMsg:
        s += QLatin1String("CRITICAL: ");
        break;
    case QtFatalMsg:
        s+= QLatin1String("FATAL: ");
        break;
    case QtDebugMsg:
        s += QLatin1String("DEBUG: ");
        break;
    default:
        // Nothing
        break;
    }

    s += msg;
    s += QLatin1String("\n");

    file->write(s.toAscii());
    file->flush();

    if (type == QtFatalMsg) {
        closeDebugLog();
        exit(1);
    }
}

//#endif


























#endif // LOGDEBUG_H
