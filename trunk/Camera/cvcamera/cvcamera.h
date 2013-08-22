#ifndef CVCAMERA_H
#define CVCAMERA_H

#include <QObject>
#include <QtWidgets>

#include <cv.h>
#include <highgui.h>
//#include "cxcore.h"
//#include "cvaux.h"


#include "cvcameraview.h"


namespace HEHUI {


class CVCamera : public QObject
{
    Q_OBJECT
public:
    enum FileType{AVI, MPEG, UNKNOWN};


    explicit CVCamera(QObject *parent = 0);
    ~CVCamera();

    void setViewWidget(CVCameraView *viewWidget);

    QImage currentImage() const;



protected:
    void timerEvent(QTimerEvent*);

    
signals:
    void signalImageCaptured(QImage &image);

 public slots:
     //开始摄像机捕获
     bool startCameraCapture(int camNumber = 0);
     //开始文件捕获
     bool startFileCapture(const QString &fileName, FileType ft = UNKNOWN);

     //关闭捕获
     bool stopCapture();
     //暂停捕获
     bool pauseCapture();
     //继续捕获
     bool resumeCapture();

     //重置查询图像帧率
     bool resetFPS(int newFPS = 25);

     //设置图像分辨率
     void setresolution(int width, int height);
     void setresolution(const QSize &resolution);


private slots:
    //启动摄像机
     bool openCam(int camNumber = 0);
     //打开视频文件
     bool openFile(const QString &fileName, FileType ft = UNKNOWN);


private:
    //图像格式转换
    QImage IplImageToQImage(const IplImage * iplImage, double mini = 0, double maxi = 0);

    //获取图像帧
    bool queryFrameFromCapture();


private:

    //要显示的图像
    QImage image;
    //从Capture中捕获的图像
    IplImage *pImage;
    //摄像机对象
    CvCapture *pCapture;

    //是否正在捕获
    bool capturing;
    //Capture是否已经启动
    bool captureStarted;

    //帧率
    int fps;

    //计时器ID
    int timerID;

    //摄像头分辨率
    QSize m_resolution;

    CVCameraView *m_viewWidget;



    
};

} //namespace HEHUI

#endif // CVCAMERA_H
