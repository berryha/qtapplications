#include "cvcameraview.h"

#include <QPainter>
#include <QDebug>


namespace HEHUI {


CVCameraView::CVCameraView(QWidget *parent) :
    QWidget(parent)
{
    ui.setupUi(this);


    windowWidth = geometry().width();
    windowHeight = geometry().height();
    QPoint center = geometry().center();
    centerX = center.x();
    centerX = center.y();

    imageTopLeftPoint = QPoint();

    autoScaleVideoFrameToFitWindowSize = true;
    aspectRatioMode = Qt::KeepAspectRatio;

    lastDisplayedImageSize = QSize(0, 0);

}

void CVCameraView::updateImage(const QImage &image){
    //qDebug()<<"--CVCameraView::updateImage()";

    orignalImage = image;



    QSize orignalImageSize = orignalImage.size();
    if(orignalImageSize.width() > windowWidth || orignalImageSize.height() > windowHeight){
        displayedImage = orignalImage.scaled(windowWidth, windowHeight, aspectRatioMode, Qt::SmoothTransformation);
        calculateImageTLPoint();
    }else{
        displayedImage = orignalImage;
    }

//    if(autoScaleVideoFrameToFitWindowSize){
//        displayedImage = orignalImage.scaled(windowWidth, windowHeight, aspectRatioMode, Qt::SmoothTransformation);
//        calculateImageTLPoint();
//    }else{
//        displayedImage = orignalImage;
//    }



//    update();
    repaint();

}

bool CVCameraView::saveOrignalImage(const QString &fileName){
    return orignalImage.save(fileName);
}

QImage CVCameraView::getOrignalImage() const{
    return orignalImage;
}

void CVCameraView::paintEvent(QPaintEvent * /* event */)
{

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform, true);

    painter.drawImage(imageTopLeftPoint, displayedImage);

}

void CVCameraView::resizeEvent ( QResizeEvent * event ){

    windowWidth = event->size().width();
    windowHeight = event->size().height();

    QPoint center = geometry().center();
    centerX = center.x();
    centerY = center.y();

    lastDisplayedImageSize = QSize(0, 0);

    //***重置坐标
    imageTopLeftPoint = QPoint();
    //计算坐标
    calculateImageTLPoint();




//    repaint();
    update();

}

void CVCameraView::calculateImageTLPoint(){
//    qDebug()<<"--CVCameraView::calculateImageTLPoint()";


//    if(imageTopLeftPoint.isNull()){

//        QPoint center = geometry().center();

//        int x_TL = center.x() - ( displayedImage.size().width() /2 );
//        int y_TL = center.y() - ( displayedImage.size().height() /2);
//        imageTopLeftPoint = QPoint(x_TL, y_TL);

//    }



    QSize displayedImageSize = displayedImage.size();
    if(lastDisplayedImageSize != displayedImageSize){
        int x_TL = centerX - ( displayedImageSize.width() /2 );
        int y_TL = centerY - ( displayedImageSize.height() /2);
        imageTopLeftPoint = QPoint(x_TL, y_TL);
        lastDisplayedImageSize = displayedImageSize;
    }


}








} //namespace HEHUI
