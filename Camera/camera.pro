TEMPLATE = app
TARGET = camera

QT += widgets multimedia multimediawidgets

HEADERS = \
    camera.h \
    imagesettings.h \
    videosettings.h \
    paintarea.h \
    logdebug.h \
    cvcamera/cvcameraview.h \
    cvcamera/cvcamera.h

SOURCES = \
    main.cpp \
    camera.cpp \
    imagesettings.cpp \
    videosettings.cpp \
    paintarea.cpp \
    cvcamera/cvcameraview.cpp \
    cvcamera/cvcamera.cpp

FORMS += \
    camera.ui \
    videosettings.ui \
    imagesettings.ui \
    cvcamera/cvcameraview.ui

RESOURCES += \
    resources.qrc

INCLUDEPATH += cvcamera

win32:RC_ICONS = resources/images/app.ico
#mac:ICON = ./resources/images/app.icns


win32 {
    INCLUDEPATH += E:\DEV\openCV2.4.6\build\include \
                    E:\DEV\openCV2.4.6\build\include\opencv

    LIBS += -LE:\DEV\openCV2.4.6\build\x86\vc10\lib \
            -lopencv_core246 \
            -lopencv_imgproc246 \
            -lopencv_highgui246 \
            -lopencv_video246
}
unix {
    INCLUDEPATH += /usr/local/include/opencv
    LIBS += -L/usr/local/lib \
        -lcv \
        -lhighgui \
        -lcxcore
}


# define some usefull values
QMAKE_TARGET_COMPANY	= "He Hui"
QMAKE_TARGET_PRODUCT	= "Camera Capturer"
QMAKE_TARGET_DESCRIPTION    = "Crossplatform Tools Based On Qt"
QMAKE_TARGET_COPYRIGHT	= "Copyright (C) 2007 - 2013 He Hui"


