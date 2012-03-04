# ##
TEMPLATE = app
TARGET = messenger
QT += core \
    gui \
    sql \
    network \
    xml \
    webkit

# include config file
include( ../config.pri )
HHSharedLibs += HHSharedCore \
    HHSharedGUI \
    HHSharedNetwork \
    HHSharedUDT
include(../../HHSharedLibs.pri)
HHSharedLibs += HHSharedIM
include (../sharedim/sharedim.pri)

# #
UI_DIR = ./ui

# ##
DEPENDPATH += . \
    about \
    debug \
    mainwindow
INCLUDEPATH += . \
    mainwindow \
    about \
    chatwindow

# Input
HEADERS += contactinfowidget/contactinfowidget.h \
    informationtips/userinfotipwindow.h \
    chatwindow/contactchatwindow.h \
    chatwindow/groupchatwindow.h \
    chatwindow/emoticonselecter/emoticonselectoruibase.h \
    chatwindow/emoticonselecter/customface.h \
    chatwindow/emoticonselecter/evaresource.h \
    search/userinfomodel.h \
    search/search.h \
    imageresource.h \
    imuser.h \
    settings.h \
    app_constants_client.h \
    contactsmanager/contact.h \
    networkmanager/servermanagerwindow.h \
    networkmanager/serverinfo.h \
    networkmanager/serverinfomodel.h \
    networkmanager/clientnetworkmanager.h \
    login/loginwidget.h \
    contactsmanager/contactgroup.h \
    chatwindow/chatmessagewindow.h \
    chatwindow/chatwindowmanager.h \
    contactwidget/contactwidget.h \
    contactsmanager/contactsmanager.h \
    login/login.h \
    about/aboutdialog.h \
    mainwindow/mainwindow.h \
    login/applyforregistrationwidget.h \
    search/addcontactdialog.h \
    chatwindow/emoticonselecter/emoticonselector.h \
    contactsmanager/interestgroup.h \
    packetmanager/imclientpacketsparser.h
SOURCES += contactinfowidget/contactinfowidget.cpp \
    informationtips/userinfotipwindow.cpp \
    chatwindow/contactchatwindow.cpp \
    chatwindow/groupchatwindow.cpp \
    chatwindow/emoticonselecter/emoticonselectoruibase.cpp \
    chatwindow/emoticonselecter/customface.cpp \
    chatwindow/emoticonselecter/evaresource.cpp \
    search/userinfomodel.cpp \
    search/search.cpp \
    imageresource.cpp \
    imuser.cpp \
    settings.cpp \
    contactsmanager/contact.cpp \
    networkmanager/servermanagerwindow.cpp \
    networkmanager/serverinfo.cpp \
    networkmanager/serverinfomodel.cpp \
    networkmanager/clientnetworkmanager.cpp \
    login/loginwidget.cpp \
    contactsmanager/contactgroup.cpp \
    chatwindow/chatmessagewindow.cpp \
    chatwindow/chatwindowmanager.cpp \
    contactwidget/contactwidget.cpp \
    contactsmanager/contactsmanager.cpp \
    login/login.cpp \
    main.cpp \
    about/aboutdialog.cpp \
    mainwindow/mainwindow.cpp \
    login/applyforregistrationwidget.cpp \
    search/addcontactdialog.cpp \
    chatwindow/emoticonselecter/emoticonselector.cpp \
    contactsmanager/interestgroup.cpp \
    packetmanager/imclientpacketsparser.cpp
FORMS += contactinfowidget/contactinfowidget.ui \
    informationtips/userinfotipwindow.ui \
    chatwindow/contactchatwindow.ui \
    chatwindow/groupchatwindow.ui \
    search/search.ui \
    networkmanager/servermanagerwindow.ui \
    login/loginwidget.ui \
    chatwindow/chatmessagewindow.ui \
    chatwindow/chatwindowmanager.ui \
    contactwidget/contactwidget.ui \
    about/aboutdialog.ui \
    mainwindow/mainwindow.ui \
    login/applyforregistrationwidget.ui \
    search/addcontactdialog.ui
RESOURCES += resources/images/emoticons/emoticons.qrc \
    resources/images/face/face.qrc \
    resources/images/messageeditor/messageeditor.qrc \
    resources.qrc

# win32:RC_FILE = iconresource.rc
win32:APP_ICON = resources/images/app.ico
mac:ICON = ./resources/images/app.icns

# DESTDIR = ../bin
unix:target.path += ../bin
win32:target.path += %windir%/system32
INSTALLS += target

# define some usefull values
# QMAKE_TARGET_COMPANY	= "He Hui Team"
QMAKE_TARGET_PRODUCT = "Messenger"
QMAKE_TARGET_DESCRIPTION = "Crossplatform Client Application For IM Based On Qt"

# QMAKE_TARGET_COPYRIGHT	= "Copyright (C) 2007 - 2010 He Hui"
DEFINES *= "APP_NAME=\"\\\"$${QMAKE_TARGET_PRODUCT}\\\"\""
