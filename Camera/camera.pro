TEMPLATE = app
TARGET = camera

QT += widgets multimedia multimediawidgets

HEADERS = \
    camera.h \
    imagesettings.h \
    videosettings.h \
    paintarea.h \
    logdebug.h

SOURCES = \
    main.cpp \
    camera.cpp \
    imagesettings.cpp \
    videosettings.cpp \
    paintarea.cpp

FORMS += \
    camera.ui \
    videosettings.ui \
    imagesettings.ui

#target.path = $$[QT_INSTALL_EXAMPLES]/multimediawidgets/camera
#INSTALLS += target

