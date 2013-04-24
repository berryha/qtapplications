TARGET = clientservice
TEMPLATE = app
CONFIG += qt \
    console
QT += core \
    network \
    sql
QT -= gui

# include config file
include( ../config.pri )
HHSharedLibs += HHSharedCore \
    HHSharedNetwork \
    HHSharedService \
    HHSharedUDT
win32:HHSharedLibs += HHSharedWindowsManagement
include(../../HHSharedLibs.pri)
HHSharedLibs += HHSharedMS
include(../sharedms/sharedms.pri)

# ###
DEPENDPATH += .
INCLUDEPATH += .
RESOURCES += resources.qrc
SOURCES += \
    systeminfo.cpp \
    process.cpp \
    settings.cpp \
    packetmanager/clientpacketsparser.cpp \
    main.cpp \
    clientservice/clientservice.cpp \
    clientresourcesmanager.cpp
HEADERS += \
    systeminfo.h \
    process.h \
    settings.h \
    app_constants.h \
    packetmanager/clientpacketsparser.h \
    clientservice/clientservice.h \
    clientresourcesmanager.h



# win32:RC_FILE = iconresource.rc
#win32:APP_ICON = resources/images/app.ico
win32:RC_ICONS = resources/images/app.ico
mac:ICON = ./resources/images/app.icns

# define some usefull values
# QMAKE_TARGET_COMPANY	= "He Hui Team"
QMAKE_TARGET_PRODUCT = "Client Service Application For Management System"
QMAKE_TARGET_DESCRIPTION = "Crossplatform Service Application Based On Qt"
DEFINES *= "APP_NAME=\"\\\"$${QMAKE_TARGET_PRODUCT}\\\"\""

