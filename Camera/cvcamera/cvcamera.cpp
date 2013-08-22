#include "cvcamera.h"



namespace HEHUI {


CVCamera::CVCamera(QObject *parent) :
    QObject(parent)
{


    image = QImage(0, 0, QImage::Format_RGB32);
    pImage = 0;
    pCapture = 0;
    capturing = false;
    captureStarted = false;

    fps = 25;
    timerID = -1;

    m_resolution = QSize(640, 480);

    m_viewWidget = 0;


}

CVCamera::~CVCamera(){

    if(captureStarted){
        stopCapture();
    }

    cvReleaseImage(&pImage);
    cvReleaseCapture(&pCapture);


}

void CVCamera::setViewWidget(CVCameraView *viewWidget){
    m_viewWidget = viewWidget;
}

QImage CVCamera::currentImage() const{
    return image;
}

void CVCamera::timerEvent(QTimerEvent*)
{
   // qDebug()<<"CVCamera::timerEvent(QTimerEvent*)";
    //从计算机获取图像帧
    queryFrameFromCapture();


}

bool CVCamera::startCameraCapture(int camNumber)
{
    qDebug()<<"CVCamera::startCameraCapture(int camNumber)  camNumber:"<<camNumber;

    if(openCam(camNumber)){
        //启动计时器
        timerID = startTimer( 1000/fps );
        capturing = true;
        return true;
    }

    qCritical()<<"ERROR! Failed to start camera capture!";
    return false;


}

bool CVCamera::startFileCapture(const QString &fileName, FileType ft){
    qDebug()<<"CVCamera::startFileCapture(....)";

    if(openFile(fileName, ft)){
        //启动计时器
        timerID = startTimer( 1000/fps );
        capturing = true;
        return true;
    }

    return false;
}

bool CVCamera::stopCapture()
{

    qDebug()<<"CVCamera::stopCapture()";

    killTimer(timerID);

//    cvReleaseImage(&pImage);
//    cvReleaseCapture(&pCapture);


    pImage = 0;
    pCapture = 0;

    capturing = false;
    captureStarted = false;

    return true;
}

bool CVCamera::pauseCapture()
{
    qDebug()<<"CVCamera::pauseCapture()";

    if(timerID != -1){
        killTimer(timerID);
        capturing = false;
    }

    return true;

}

bool CVCamera::resumeCapture()
{
    qDebug()<<"CVCamera::resumeCapture()";

    timerID = startTimer( 1000/fps );
    capturing = true;

    return true;

}

bool CVCamera::resetFPS(int newFPS){

    qDebug()<<"CVCamera::resetFPS(int newFPS)";

    if(newFPS < 1 || newFPS > 50 ){
        newFPS = 25;
        qWarning()<<"FPS should be between 1 and 50! The default value is 25!";
    }

    this->fps = newFPS;

    if(capturing){
        killTimer(timerID);
        timerID = startTimer( 1000/fps );
    }

    if(pCapture){
        cvSetCaptureProperty(pCapture, CV_CAP_PROP_FPS, fps);
    }

    return true;

}

void CVCamera::setresolution(int width, int height){

    m_resolution = QSize(width, height);

    setresolution(m_resolution);

}

void CVCamera::setresolution(const QSize &resolution){

    m_resolution = resolution;

    if(pauseCapture()){
        cvSetCaptureProperty(pCapture, CV_CAP_PROP_FRAME_WIDTH, m_resolution.width());
        cvSetCaptureProperty(pCapture, CV_CAP_PROP_FRAME_HEIGHT, m_resolution.height());
    }

}

bool CVCamera::openCam(int camNumber)
{
    qDebug()<<"CVCamera::openCam(int camNumber)";

    if((pCapture = cvCaptureFromCAM(camNumber)) == NULL){
        captureStarted = false;
        qDebug() << "Open camera failed! ";
        return false;
    }

    resetFPS();

    setresolution(m_resolution);

    captureStarted = true;
    return true;


}

bool CVCamera::openFile(const QString &fileName, FileType ft){
    qDebug()<<"CVCamera::openFile(....)";

    QFileInfo fi(fileName);
    if(!fi.exists()){
        //QMessageBox::critical(this, "", tr("File '%1' does not exist!").arg(fileName));
        return false;
    }
    if(!fi.isReadable()){
        //QMessageBox::critical(this, tr("Fatal Error"), tr("File '%1' is not readable!").arg(fileName));
        return false;
    }

    FileType type = ft;

    if(ft == UNKNOWN){
        QString fileSuffix = fi.suffix().toLower();
        if(fileSuffix == "avi"){
            type = AVI;
        }else if(fileSuffix == "mpg" || fileSuffix == "mpeg" || fileSuffix == "mp4"){
            type = MPEG;
        }

    }

        char fileNameChar[512];
        strcpy(fileNameChar, fileName.toStdString().c_str());
        //QMessageBox::information(this, "fileNameChar", fileNameChar);

    switch (type){
    case AVI:
                pCapture = cvCaptureFromAVI(fileNameChar);
        break;
    case MPEG:
                pCapture = cvCaptureFromFile(fileNameChar);
        break;
    default:
                pCapture = cvCaptureFromFile(fileNameChar);
        break;
    }

    if(!pCapture){
        captureStarted = false;
        //QMessageBox::critical(this, tr("Fatal Error"), tr("Can not open the file!"));
        return false;
    }else{
        captureStarted = true;
        return true;
    }


}




QImage CVCamera::IplImageToQImage(const IplImage * iplImage,double mini, double maxi)
{
    qDebug()<<"CVCamera::IplImageToQImage(....)";


    uchar *qImageBuffer = NULL;

    int width = iplImage->width;

    /**//* Note here that OpenCV image is stored so that each lined is
32-bits aligned thus
* explaining the necessity to "skip" the few last bytes of each
line of OpenCV image buffer.
*/
    int widthStep = iplImage->widthStep;
    int height = iplImage->height;

    switch (iplImage->depth)
    {
    case IPL_DEPTH_8U:
        if(iplImage->nChannels == 1)
        {
            /**//* OpenCV image is stored with one byte grey pixel. We convert it
            to an 8 bit depth QImage.
            */

            qImageBuffer = (uchar *) malloc(width*height*sizeof(uchar));
            uchar *QImagePtr = qImageBuffer;
            const uchar *iplImagePtr = (const uchar *) iplImage->imageData;

            for(int y = 0; y < height; y++)
            {
                // Copy line by line
                memcpy(QImagePtr, iplImagePtr, width);
                QImagePtr += width;
                iplImagePtr += widthStep;
            }

        }
        else if(iplImage->nChannels == 3)
        {
            /**//* OpenCV image is stored with 3 byte color pixels (3 channels).
                    We convert it to a 32 bit depth QImage.
                    */
            qImageBuffer = (uchar *) malloc(width*height*4*sizeof(uchar));
            uchar *QImagePtr = qImageBuffer;
            const uchar *iplImagePtr = (const uchar *) iplImage->imageData;
            for(int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    // We cannot help but copy manually.
                    QImagePtr[0] = iplImagePtr[0];
                    QImagePtr[1] = iplImagePtr[1];
                    QImagePtr[2] = iplImagePtr[2];
                    QImagePtr[3] = 0;

                    QImagePtr += 4;
                    iplImagePtr += 3;
                }
                iplImagePtr += widthStep-3*width;
            }

        }
        else
        {
            qDebug("IplImageToQImage: image format is not supported : depth=8U and %d channels ", iplImage->nChannels);
        }
        break;
            case IPL_DEPTH_16U:
        if(iplImage->nChannels == 1)
        {
            /**//* OpenCV image is stored with 2 bytes grey pixel. We convert it
            to an 8 bit depth QImage.
            */
            qImageBuffer = (uchar *) malloc(width*height*sizeof(uchar));
            uchar *QImagePtr = qImageBuffer;
            //const uint16_t *iplImagePtr = (const uint16_t *);
            const unsigned int *iplImagePtr = (const unsigned int *)iplImage->imageData;
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    // We take only the highest part of the 16 bit value. It is
                    //similar to dividing by 256.
                    *QImagePtr++ = ((*iplImagePtr++) >> 8);
                }
                iplImagePtr += widthStep/sizeof(unsigned int)-width;
            }
        }
        else
        {
            qDebug("IplImageToQImage: image format is not supported : depth=16U and %d channels ", iplImage->nChannels);

        }
        break;
            case IPL_DEPTH_32F:
        if(iplImage->nChannels == 1)
        {
            /**//* OpenCV image is stored with float (4 bytes) grey pixel. We
            convert it to an 8 bit depth QImage.
            */
            qImageBuffer = (uchar *) malloc(width*height*sizeof(uchar));
            uchar *QImagePtr = qImageBuffer;
            const float *iplImagePtr = (const float *) iplImage->imageData;
            for(int y = 0; y < height; y++)
            {
                for(int x = 0; x < width; x++)
                {
                    uchar p;
                    float pf = 255 * ((*iplImagePtr++) - mini) / (maxi - mini);
                    if(pf < 0) p = 0;
                    else if(pf > 255) p = 255;
                    else p = (uchar) pf;

                    *QImagePtr++ = p;
                }
                iplImagePtr += widthStep/sizeof(float)-width;
            }
        }
        else
        {
            qDebug("IplImageToQImage: image format is not supported : depth=32F and %d channels ", iplImage->nChannels);
        }
        break;
               case IPL_DEPTH_64F:
        if(iplImage->nChannels == 1)
        {
            /**//* OpenCV image is stored with double (8 bytes) grey pixel. We
                convert it to an 8 bit depth QImage.
                */
            qImageBuffer = (uchar *) malloc(width*height*sizeof(uchar));
            uchar *QImagePtr = qImageBuffer;
            const double *iplImagePtr = (const double *) iplImage->imageData;
            for(int y = 0; y < height; y++)
            {
                for(int x = 0; x < width; x++)
                {
                    uchar p;
                    double pf = 255 * ((*iplImagePtr++) - mini) / (maxi - mini);

                    if(pf < 0) p = 0;
                    else if(pf > 255) p = 255;
                    else p = (uchar) pf;

                    *QImagePtr++ = p;
                }
                iplImagePtr += widthStep/sizeof(double)-width;
            }
        }
        else
        {
            qDebug("IplImageToQImage: image format is not supported : depth=64F and %d channels ", iplImage->nChannels);
        }
        break;
            default:
        qDebug("IplImageToQImage: image format is not supported : depth=%d and %d channels ", iplImage->depth, iplImage->nChannels);
    }

    QImage qImage;
    QVector<QRgb> vcolorTable;
    if(iplImage->nChannels == 1)
    {
        // We should check who is going to destroy this allocation.
        QRgb *colorTable = new QRgb[256];
        for(int i = 0; i < 256; i++)
        {
            colorTable[i] = qRgb(i, i, i);
            vcolorTable[i] = colorTable[i];
        }
        qImage = QImage(qImageBuffer, width, height, QImage::Format_Indexed8).copy();
        qImage.setColorTable(vcolorTable);
    }
    else
    {
        qImage = QImage(qImageBuffer, width, height, QImage::Format_RGB32).copy();
    }
    free(qImageBuffer);
    return qImage;
}


bool CVCamera::queryFrameFromCapture(){
    //qDebug()<<"CVCamera::queryFrameFromCapture()";

    Q_ASSERT_X(pCapture != NULL, "Query Frame From Camera", "Camera Is Not Open!");

    if(!pCapture){return false;}

/*
    if((pImage = cvQueryFrame(pCapture)) != NULL){
        *image = IplImageToQImage(pImage);
        //TODO:效率？
        emit signalImageCaptured(*image);

        int imageWidth = image->size().width();
        int imageHeight = image->size().height();

        if((imageWidth > windowWidth) || (imageHeight > windowHeight) ){
            if(autoExpandWindowToFitVideoFrameSize){
                //缩放窗口
                resize(imageWidth, imageHeight);
            }else{
                //缩放图像
                *image = image->scaled(windowWidth, windowHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            }

        }

        qDebug()<<"CamDisplayWidget::QueryFrameFromCam---Image Captured:True";
        return true;
    }else{
        qDebug()<<"CamDisplayWidget::QueryFrameFromCam---Image Captured:False";
        return false;
    }

*/


    if((pImage = cvQueryFrame(pCapture)) != NULL)
    {
        cvCvtColor(pImage, pImage, CV_BGR2RGB);
         image = QImage((uchar *)pImage->imageData, pImage->width, pImage->height, pImage->widthStep, QImage::Format_RGB888);
        //image = IplImageToQImage(pImage, 640, 480);

         if(m_viewWidget){
             m_viewWidget->updateImage(image);
         }
         emit signalImageCaptured(image);
         return true;
    }

    return false;

}









} //namespace HEHUI
