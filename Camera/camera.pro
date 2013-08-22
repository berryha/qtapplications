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




