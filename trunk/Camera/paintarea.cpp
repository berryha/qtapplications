

#include "paintarea.h"

#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QMessageBox>
#include <QInputDialog>
#include <QApplication>



PaintArea::PaintArea(QWidget *parent) :
    QWidget(parent),
    theImage(640, 480, QImage::Format_RGB32),
    color(Qt::blue),
    thickness(3),
    lastPos(-1, -1)
{

    setAttribute(Qt::WA_StaticContents);
//    setAttribute(Qt::WA_NoBackground);

//    setBackgroundRole(QPalette::Base);
//    setAutoFillBackground(true);


    theImage.fill(qRgb(255, 255, 255));
    orignalImage = theImage;



    preferedImage = QSize(360, 480);
    outlineRect = QRect(QPoint(0, 0), preferedImage);

    dragPosition = QPoint(0, 0);
    imageTopLeftPoint = QPoint(0, 0);

}

bool PaintArea::openImage(const QString &fileName)
{
    QImage image;
    if (!image.load(fileName))
        return false;

    setImage(image);

    return true;
}

bool PaintArea::saveImage(const QString &fileName, const char *fileFormat)
{
    return theImage.save(fileName, fileFormat);
}

void PaintArea::setImage(const QImage &image)
{
    theImage = image.convertToFormat(QImage::Format_RGB32);
    orignalImage = theImage;



//    imageTopLeftPoint = QPoint();
//    calculateImageTLPoint();
//    outlineRect = QRect(imageTopLeftPoint, preferedImage);


    QPainter painter(&theImage);
    drawOutline(painter);


    update();
    updateGeometry();

}

//void PaintArea::insertShape(const QPainterPath &path)
//{
//    pendingPath = path;
//#ifndef QT_NO_CURSOR
//    setCursor(Qt::CrossCursor);
//#endif
//}

//void PaintArea::setBrushColor(const QColor &color)
//{
//    this->color = color;
//}

//void PaintArea::setBrushWidth(int width)
//{
//    thickness = width;
//}

//void PaintArea::setBrush(BrushInterface *brushInterface, const QString &brush)
//{
//    this->brushInterface = brushInterface;
//    this->brush = brush;
//}


QSize PaintArea::sizeHint() const
{
    //return theImage.size();
    return QSize(653, 490);
}

void PaintArea::setFileDirectorys(const QString &targetImagePath){

    this->targetImagePath = targetImagePath;

//    QDir dir;
//    if(dir.mkpath(snapshotImagesPath)){
//        QMessageBox::critical(this, tr("Error"), tr("Can not create directory:<p>%1</p>").arg(snapshotImagesPath));
//    }
//    if(dir.mkpath(targetImagePath)){
//        QMessageBox::critical(this, tr("Error"), tr("Can not create directory:<p>%1</p>").arg(targetImagePath));
//    }


}


void PaintArea::paintEvent(QPaintEvent * /* event */)
{

    QPainter painter(this);
    painter.drawImage(imageTopLeftPoint, theImage);

}

//void PaintArea::keyPressEvent(QKeyEvent *event){
//    qDebug()<<"----------0";

//    if (event->isAutoRepeat())
//        return;

//    if(QApplication::keyboardModifiers() == Qt::ControlModifier){
//        qDebug()<<"----------1";
//        switch (event->key()) {

//        case Qt::Key_Left:
//        {
//            qDebug()<<"----------2";

//            QPoint cutTopLeft = outlineRect.topLeft();

//            moveOutlineRect(cutTopLeft - QPoint(10, cutTopLeft.y()));

//        }
//            break;
//        case Qt::Key_Right:
//        {

//        }
//            break;

//        default:
//            QWidget::keyPressEvent(event);
//        }

//    }



//}

void PaintArea::mousePressEvent(QMouseEvent *event)
{

    if (event->button() == Qt::LeftButton) {

        QPoint curMousePos = mapFromGlobal(event->globalPos());
        dragPosition = curMousePos - outlineRect.topLeft();
        if(outlineRect.contains(curMousePos)){
            lastPos = event->pos();
        }

        update();
    }
}

void PaintArea::mouseMoveEvent(QMouseEvent *event)
{

    //qDebug()<<"--PaintArea::mouseMoveEvent(...)";


    if ((event->buttons() & Qt::LeftButton) && lastPos != QPoint(-1, -1) ) {

        QPoint curMousePos = mapFromGlobal(event->globalPos());

        QPoint newTopLeft =  curMousePos - dragPosition;
//        if(newTopLeft.x() < imageTopLeftPoint.x()){
//            newTopLeft.setX(imageTopLeftPoint.x());
//        }
//        if(newTopLeft.y() < imageTopLeftPoint.y()){
//            newTopLeft.setY(imageTopLeftPoint.y());
//        }

//        int maxTopLeftX = imageTopLeftPoint.x() + orignalImage.width() - outlineRect.width();
//        if(newTopLeft.x() > maxTopLeftX){
//            newTopLeft.setX(maxTopLeftX);
//        }
//        int maxTopLeftY = imageTopLeftPoint.y() + orignalImage.height() - outlineRect.height() ;
//        if(newTopLeft.y() > maxTopLeftY ){
//            newTopLeft.setY(maxTopLeftY);
//        }


//        outlineRect.moveTopLeft(newTopLeft);

//        theImage = orignalImage;
//        QPainter painter(&theImage);
//        drawOutline(painter);

//        update();

        moveOutlineRect(newTopLeft);

        lastPos = event->pos();

    }
}

void PaintArea::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && lastPos != QPoint(-1, -1)) {
        lastPos = QPoint(-1, -1);
    }
}

void PaintArea::mouseDoubleClickEvent ( QMouseEvent * event ){

//    QRect targetRect = outlineRect;
//    targetRect.moveTopLeft(outlineRect.topLeft() - imageTopLeftPoint);

//    QImage targetImage = orignalImage.copy(targetRect);


//    bool ok = false;
//    do {
//        QString text = QInputDialog::getText(this, tr("Save Image"),
//                                             tr("File name:"), QLineEdit::Normal,
//                                             "", &ok);
//        if (ok && !text.isEmpty()){
//            QString fileName = targetImagePath + "/" + text + ".jpg";
//            if(QFile::exists(fileName)){
//                QMessageBox msgBox;
//                msgBox.setText(tr("File '%1' already exists!").arg(fileName));
//                msgBox.setInformativeText("Do you want to overwrite it?");
//                msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
//                msgBox.setDefaultButton(QMessageBox::No);
//                int ret = msgBox.exec();
//                if(ret == QMessageBox::No){
//                    continue;
//                }
//            }

//            ok = targetImage.save(fileName);
//            if(ok){
//                emit targetImageSaved(fileName);
//            }else{
//                QMessageBox::critical(this, tr("Error"), tr("Can not save image to:<p>%1</p>").arg(fileName));
//            }
//            return;
//        }


//    } while (ok);


    saveTargetImage();


    event->accept();

}

void PaintArea::resizeEvent ( QResizeEvent * event ){
    //qDebug()<<"PaintArea::resizeEvent ( QResizeEvent * event )";


    QPoint offset = outlineRect.topLeft() - imageTopLeftPoint;

    //***重置坐标
    imageTopLeftPoint = QPoint();
    //计算坐标
    calculateImageTLPoint();


    outlineRect.moveTopLeft(imageTopLeftPoint + offset);


//    repaint();
    update();
//    updateGeometry();

    //QWidget::resizeEvent(event);


}


void PaintArea::calculateImageTLPoint(){

    //qDebug()<<"PaintArea::calculateImageTLPoint()";


    if(imageTopLeftPoint.isNull()){
        //qDebug()<<"Null imageTopLeftPoint!";

        QPoint center = geometry().center();
        //qDebug()<<"center.x():"<<center.x();
        //qDebug()<<"orignalImage.size().x():"<<orignalImage.size().width() /2;


        int x_TL = center.x() - ( orignalImage.size().width() /2 );
        int y_TL = center.y() - ( orignalImage.size().height() /2);
        imageTopLeftPoint = QPoint(x_TL, y_TL);

    }


}

void PaintArea::saveTargetImage(){

    QRect targetRect = outlineRect;
    targetRect.moveTopLeft(outlineRect.topLeft() - imageTopLeftPoint);

    QImage targetImage = orignalImage.copy(targetRect);


    bool ok = false;
    do {
        QString text = QInputDialog::getText(this, tr("Save Image"),
                                             tr("File name:"), QLineEdit::Normal,
                                             "", &ok);
        if (ok && !text.isEmpty()){
            QString fileName = targetImagePath + "/" + text + ".jpg";
            if(QFile::exists(fileName)){
                QMessageBox msgBox;
                msgBox.setText(tr("File '%1' already exists!").arg(fileName));
                msgBox.setInformativeText("Do you want to overwrite it?");
                msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                msgBox.setDefaultButton(QMessageBox::No);
                int ret = msgBox.exec();
                if(ret == QMessageBox::No){
                    continue;
                }
            }

            ok = targetImage.save(fileName);
            if(ok){
                emit targetImageSaved(fileName);
            }else{
                QMessageBox::critical(this, tr("Error"), tr("Can not save image to:<p>%1</p>").arg(fileName));
            }
            return;
        }


    } while (ok);


}



void PaintArea::setupPainter(QPainter &painter)
{
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(color, thickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
}

void PaintArea::drawOutline(QPainter &painter)
{
//    painter.setPen(Qt::darkGreen);
//    painter.setPen(Qt::DashLine);
//    painter.setBrush(Qt::NoBrush);
    //painter.drawRect(10, 10, 100, 100);

    //painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform, true);
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(Qt::red, 2, Qt::DashLine));

    QRect oldRect = outlineRect;
    oldRect.moveTopLeft(outlineRect.topLeft() - imageTopLeftPoint);

    painter.drawRect(oldRect);

}


void PaintArea::moveOutlineRectToLeft(){

    qDebug()<<"--PaintArea::moveOutlineRectToLeft()";

    QPoint curRectTopLeft = outlineRect.topLeft();

    moveOutlineRect(curRectTopLeft - QPoint(5, curRectTopLeft.y()));

}

void PaintArea::moveOutlineRectToRight(){

    //qDebug()<<"--PaintArea::moveOutlineRectToRight()";

    QPoint curRectTopLeft = outlineRect.topLeft();
    moveOutlineRect(curRectTopLeft + QPoint(5, curRectTopLeft.y()));

}

void PaintArea::moveOutlineRectUp(){

    //qDebug()<<"--PaintArea::moveOutlineRectUp()";

    QPoint curRectTopLeft = outlineRect.topLeft();
    moveOutlineRect(curRectTopLeft - QPoint(curRectTopLeft.x(), 5));

}
void PaintArea::moveOutlineRectDown(){

    //qDebug()<<"--PaintArea::moveOutlineRectDown()";

    QPoint curRectTopLeft = outlineRect.topLeft();
    moveOutlineRect(curRectTopLeft + QPoint(curRectTopLeft.x(), 5));

}


void PaintArea::moveOutlineRect(QPoint topLeft){


    QPoint newTopLeft = topLeft;

    if(newTopLeft.x() < imageTopLeftPoint.x()){
        newTopLeft.setX(imageTopLeftPoint.x());
    }
    if(newTopLeft.y() < imageTopLeftPoint.y()){
        newTopLeft.setY(imageTopLeftPoint.y());
    }

    int maxTopLeftX = imageTopLeftPoint.x() + orignalImage.width() - outlineRect.width();
    if(newTopLeft.x() > maxTopLeftX){
        newTopLeft.setX(maxTopLeftX);
    }
    int maxTopLeftY = imageTopLeftPoint.y() + orignalImage.height() - outlineRect.height() ;
    if(newTopLeft.y() > maxTopLeftY ){
        newTopLeft.setY(maxTopLeftY);
    }


    outlineRect.moveTopLeft(newTopLeft);

    theImage = orignalImage;
    QPainter painter(&theImage);
    drawOutline(painter);

    update();

}


