/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
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
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef MOVIEPLAYER_H
#define MOVIEPLAYER_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLabel;
class QVBoxLayout;

class QScrollArea;
class QScrollBar;
class QToolButton;
QT_END_NAMESPACE

#ifndef QT_NO_PRINTER
#include <QPrinter>
#include <QPrintDialog>
#endif

#include "imageviewercontroler.h"
#include "animationcontroler.h"

#include "HHSharedCore/hsingleton.h"


class ImageViewer : public QWidget, public Singleton<ImageViewer>
{
    Q_OBJECT

    friend class Singleton<ImageViewer> ;

public:
    ImageViewer(QWidget *parent = 0, Qt::WindowFlags fl = Qt::FramelessWindowHint);
    ~ImageViewer();


    void setImages(const QStringList &images, unsigned int initIndex = 0);


protected:
    //#ifndef QT_NO_WHEELEVENT
    //    void wheelEvent(QWheelEvent *);
    //#endif
    //    void keyPressEvent(QKeyEvent *event);
    //    void resizeEvent(QResizeEvent * event);
    //    void mouseMoveEvent(QMouseEvent * event);

    bool eventFilter(QObject *obj, QEvent *event);


private slots:
    void open();
    void openFile(const QString &fileName);
    void openFile(int imageIndex);


    void updatePixmap(const QPixmap &pixmap);

    void updateAnimationFrame(const QPixmap &pixmap);

    void saveAs();
    void print();

    void rotate(int angle);
    void flip(bool horizontally, bool vertically);

    //    void flipHor();
    //    void flipVer();
    void zoomIn();
    void zoomOut();
    void zoomFitBest();
    void zoomOrignal();

    void reset();

    void showContextMenu(const QPoint &pos);

    void showScaleFactor();
    void showImageInfo();
    void moveCloseButton();

private:
    void createActions();
    void updateActions();

    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);

    void showTip(const QString &tip);


private:
    QString currentImageDirectory;
    QStringList images;
    int curImageIndex;

    QLabel *imageLabel;
    QPixmap curPixmap;
    QPixmap orignalPixmap;


    QScrollArea *scrollArea;
    double scaleFactor;
    bool m_fitToWindow;
    int rotateAngle;


    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *normalSizeAct;
    QAction *fitToWindowAct;

    QAction *resetAct;

    QAction *saveAsAct;
    QAction *printAct;

    QAction *openAct;
    QAction *exitAct;


    QVBoxLayout *mainLayout;


    ImageViewerControler *imageControler;
    AnimationControler *animationControler;
    QToolButton *toolButtonClose;

    QSize m_minimumScrollAreaSize;
    bool runningValidMovie;

    QLabel *tipLabel;
    QTimer *tipTimer;
    bool tipScaleFactor;

    bool  flipHorizontally;
    bool flipVertically;


    QPoint dragPosition;




#ifndef QT_NO_PRINTER
    QPrinter printer;
#endif

};

#endif
