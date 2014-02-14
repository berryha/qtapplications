
HEADERS += logdebug.h \
    cryptography/tea/teacrypt.h \
    cryptography/cryptography.h \
    plugin/abstractplugininterface.h \
    plugin/coreinterface.h \
    plugin/pluginmanager.h \
    plugin/corepluginbase.h \
    userbase.h \
    utilities.h \
    settingscore.h \
    global_core.h \
    database/databaseutility.h \
    singleton.h \
    user.h \
    error.h \
    core_lib.h
FORMS += 
SOURCES += cryptography/tea/teacrypt.cpp \
    cryptography/cryptography.cpp \
    plugin/pluginmanager.cpp \
    plugin/corepluginbase.cpp \
    userbase.cpp \
    utilities.cpp \
    settingscore.cpp \
    database/databaseutility.cpp \
    singleton.cpp \
    user.cpp \
    error.cpp
RESOURCES += 
win32:LIBS += -Lresources/lib \
    -lwsock32 \
    -lws2_32

INCLUDEPATH += $$PWD

