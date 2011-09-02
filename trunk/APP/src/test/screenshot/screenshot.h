#ifndef SCREENSHOT_H
#define SCREENSHOT_H

#include <QtGui/QWidget>
#include <QEvent>


#include "ui_screenshot.h"

#include "selecttargetimagewidget.h"



namespace HEHUI {


class Screenshot : public QWidget
{
    Q_OBJECT

public:
    Screenshot(QWidget *parent = 0, Qt::WindowFlags fl = Qt::Popup | Qt::FramelessWindowHint | Qt::SplashScreen);
    ~Screenshot();


protected:
    bool event(QEvent *e);


signals:
    void imageSelected(const QImage &image);


public slots:


private:
    void drawBackground();

    void showSelectedRect();
    void updateSelectedRect();

private:
    Ui::ScreenshotClass ui;

    SelectTargetImageWidget * m_targetPixmapWidget;

    QPixmap originalPixmap;

    QPixmap resultPixmap;

//    QImage sourceImage;
//    QImage destinationImage;
    QImage backgroundImage;

    QPoint topLeft;
    QPoint bottomRight;





};

} //namespace HEHUI

#endif // SCREENSHOT_H
