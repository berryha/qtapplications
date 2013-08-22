#ifndef CVCAMERAVIEW_H
#define CVCAMERAVIEW_H

#include <QWidget>
#include <QPaintEvent>
#include <QResizeEvent>


#include "ui_cvcameraview.h"


namespace HEHUI {


class CVCameraView : public QWidget
{
    Q_OBJECT
    
public:
//    enum ScaleMode{IgnoreAspectRatio, KeepAspectRatio, KeepAspectRatioByExpanding};

    explicit CVCameraView(QWidget *parent = 0);
    

public slots:
    void updateImage(const QImage &image);
    bool saveOrignalImage(const QString &fileName);
    QImage getOrignalImage() const;


protected:
    void paintEvent(QPaintEvent *event);

    void resizeEvent ( QResizeEvent * event );

private:
    void calculateImageTLPoint();

private:
    Ui::CVCameraView ui;

    int windowWidth;
    int windowHeight;
    int centerX;
    int centerY;

    QImage orignalImage;
    QImage displayedImage;

    QPoint imageTopLeftPoint;

    //是否自动缩放图像帧大小
    bool autoScaleVideoFrameToFitWindowSize;
    Qt::AspectRatioMode aspectRatioMode;

    QSize lastDisplayedImageSize;




};

} //namespace HEHUI

#endif // CVCAMERAVIEW_H
