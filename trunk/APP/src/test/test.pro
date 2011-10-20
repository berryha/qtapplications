# ##
TARGET = test
TEMPLATE = app
QT += core \
    gui \
    sql \
    network

# include config file
include( ../config_global.pri )
include(../HHSharedLibs.pri)
VER_MAJ = 2010
VER_MIN = 06
VER_PAT = 26
VER_CMP = 1618
VERSION = $${VER_MAJ}.$${VER_MIN}.$${VER_PAT}.$${VER_CMP}

# #
INCLUDEPATH += $$UI_DIR \
    $$PWD

# ####
UI_DIR = ./ui

# ###
DEPENDPATH += . \
    shared \
    about \
    mainwindow
INCLUDEPATH += . \
    shared \
    mainwindow \
    about

# Input
HEADERS += listview/explistview.h \
    rudp/clientpacketsparser.h \
    screenshot/selecttargetimagewidget.h \
    screenshot/screenshot.h \
    rudp/rudp.h \
    shared/global_app.h \
    about/aboutdialog.h \
    mainwindow/mainwindow.h \
    shared/settings.h
SOURCES += listview/explistview.cpp \
    rudp/clientpacketsparser.cpp \
    screenshot/selecttargetimagewidget.cpp \
    screenshot/screenshot.cpp \
    rudp/rudp.cpp \
    shared/settings.cpp \
    main.cpp \
    about/aboutdialog.cpp \
    mainwindow/mainwindow.cpp
FORMS += listview/explistview.ui \
    screenshot/selecttargetimagewidget.ui \
    screenshot/screenshot.ui \
    rudp/rudp.ui \
    about/aboutdialog.ui \
    mainwindow/mainwindow.ui
RESOURCES += resources.qrc
win32:RC_FILE = iconresource.rc
mac:ICON = ./resources/images/app.icns
