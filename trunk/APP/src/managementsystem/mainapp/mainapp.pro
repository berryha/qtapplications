# ##
TARGET = ms
TEMPLATE = app
QT += core \
    gui \
    widgets \
    sql

# include config file
include( ../config.pri )
include(../../HHSharedLibs.pri)

# ####
UI_DIR = ./ui

# OBJECTS_DIR = ./tmp
# MOC_DIR = ./tmp
# RCC_DIR = ./tmp
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
HEADERS += shared/app_constants.h \
    shared/settings.h \
    about/aboutdialog.h \
    mainwindow/mainwindow.h
SOURCES += shared/settings.cpp \
    main.cpp \
    about/aboutdialog.cpp \
    mainwindow/mainwindow.cpp
FORMS += about/aboutdialog.ui \
    mainwindow/mainwindow.ui
RESOURCES += resources.qrc

# win32:RC_FILE = iconresource.rc
#win32:APP_ICON = resources/images/app.ico
win32:RC_ICONS = resources/images/app.ico
mac:ICON = ./resources/images/app.icns

# define some usefull values
# QMAKE_TARGET_COMPANY	= "He Hui Team"
QMAKE_TARGET_PRODUCT = "Main Application For Management System"
QMAKE_TARGET_DESCRIPTION = "Crossplatform Application Based On Qt"
DEFINES *= "APP_NAME=\"\\\"$${QMAKE_TARGET_PRODUCT}\\\"\""
