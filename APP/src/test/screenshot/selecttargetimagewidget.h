#ifndef SELECTTARGETIMAGEWIDGET_H
#define SELECTTARGETIMAGEWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QResizeEvent>


#include "ui_selecttargetimagewidget.h"

namespace HEHUI {


class SelectTargetImageWidget : public QWidget
{
    Q_OBJECT


public:
    //TargetPixmapWidget(QPixmap *originalPixmap, const QRect &selectedRect, QWidget *parent = 0, Qt::WindowFlags fl = Qt::Popup | Qt::FramelessWindowHint | Qt::SplashScreen);
    SelectTargetImageWidget(const QImage &m_originalImage, const QRect &selectedRect, QWidget *parent = 0, Qt::WindowFlags fl = Qt::Window | Qt::CustomizeWindowHint);

    ~SelectTargetImageWidget();


protected:
    void paintEvent(QPaintEvent *event);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

    void resizeEvent(QResizeEvent *event);


//    void enterEvent(QEvent *event);


signals:
    void imageSelected(const QImage &image);


private slots:
    void saveScreenshot();


private:
    Ui::SelectTargetImageWidgetClass ui;


    QImage m_originalImage;
    QImage m_resultImage;

    QPoint dragPosition;




};

} //namespace HEHUI

#endif // SELECTTARGETIMAGEWIDGET_H
