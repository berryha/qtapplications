
#ifndef PAINTAREA_H
#define PAINTAREA_H

#include <QColor>
#include <QImage>
#include <QPainterPath>
#include <QWidget>

class BrushInterface;

class PaintArea : public QWidget
{
    Q_OBJECT

public:
    PaintArea(QWidget *parent = 0);

    bool openImage(const QString &fileName);
    bool saveImage(const QString &fileName, const char *fileFormat);
    void setImage(const QImage &image);
//    void insertShape(const QPainterPath &path);
//    void setBrushColor(const QColor &color);
//    void setBrushWidth(int width);
//    void setBrush(BrushInterface *brushInterface, const QString &brush);

//    QImage image() const { return theImage; }
//    QColor brushColor() const { return color; }
//    int brushWidth() const { return thickness; }

//    QSize sizeHint() const;

    void setFileDirectorys(const QString &targetImagePath);


protected:
    void paintEvent(QPaintEvent *event);

//    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void mouseDoubleClickEvent ( QMouseEvent * event );

    void resizeEvent ( QResizeEvent * event );


public slots:

    void moveOutlineRectToLeft();
    void moveOutlineRectToRight();
    void moveOutlineRectUp();
    void moveOutlineRectDown();


    void saveTargetImage();

private slots:
    void calculateImageTLPoint();

    void moveOutlineRect(QPoint topLeft);

signals:
    void targetImageSaved(const QString &fileName);


private:
    void setupPainter(QPainter &painter);

    void drawOutline(QPainter &painter);

    QImage theImage;
    QImage orignalImage;
    QColor color;
    int thickness;

    QString brush;
    QPoint lastPos;

    QPainterPath pendingPath;



    QSize preferedImage;
    QRect outlineRect;
    QPoint dragPosition;

    QPoint imageTopLeftPoint;

//    QString snapshotImagesPath;
    QString targetImagePath;




};

#endif
