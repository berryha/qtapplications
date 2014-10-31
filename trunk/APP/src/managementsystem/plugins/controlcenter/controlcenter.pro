# ##
TEMPLATE = lib
CONFIG += plugin shared
TARGET = controlcenter

# VERSION = 2009.09.08.1055
QT += core \
    gui \
    widgets \
    sql \
    network

# include config file
include( ../../config.pri )
HHSharedLibs += HHSharedCore \
    HHSharedGUI \
    HHSharedNetwork \
    HHSharedUDT
win32:HHSharedLibs += HHSharedWindowsManagement
include(../../../HHSharedLibs.pri)
HHSharedLibs += HHSharedMS
include(../../sharedms/sharedms.pri)

# ##
UI_DIR = ./ui

INCLUDEPATH += $$UI_DIR \
               $$PWD \
               filemanagement

# Input
HEADERS += \
    networkmanager/controlcenterpacketsparser.h \
    taskmanagement/taskinfoviewwidget.h \
    taskmanagement/taskmanagement.h \
    announcement/announcement.h \
    clientinfomodel/clientinfomodel.h \
    systemmanagement/systemmanagementwidget.h \
    controlcenter.h \
    controlcenterplugin.h \
    constants.h \
    networkmanager/resourcesmanagerinstance.h \
    filemanagement/filemanagement.h
FORMS += taskmanagement/taskinfoviewwidget.ui \
    taskmanagement/taskmanagement.ui \
    announcement/announcement.ui \
    systemmanagement/systemmanagementwidget.ui \
    controlcenter.ui \
    filemanagement/filemanagement.ui
SOURCES += \
    networkmanager/controlcenterpacketsparser.cpp \
    taskmanagement/taskinfoviewwidget.cpp \
    taskmanagement/taskmanagement.cpp \
    announcement/announcement.cpp \
    clientinfomodel/clientinfomodel.cpp \
    systemmanagement/systemmanagementwidget.cpp \
    controlcenter.cpp \
    controlcenterplugin.cpp \
    networkmanager/resourcesmanagerinstance.cpp \
    filemanagement/filemanagement.cpp
RESOURCES += controlcenter.qrc

# define some usefull values
QMAKE_TARGET_PRODUCT = "Control Center"
QMAKE_TARGET_DESCRIPTION = "Crossplatform Control Center For Sitoy Based On Qt"
DEFINES *= "APP_NAME=\"\\\"$${QMAKE_TARGET_PRODUCT}\\\"\""
