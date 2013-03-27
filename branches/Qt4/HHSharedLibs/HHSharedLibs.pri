

### Config file for using the libs

HHSHAREDLIBS_ROOT_PATH = $$PWD

isEmpty(HHSharedLibs){

    contains(QT, core){
        HHSharedLibs += HHSharedCore
    }
    contains(QT, gui){
        HHSharedLibs += HHSharedGUI
    }
    contains(QT, network){
        HHSharedLibs += HHSharedNetwork
    }


    #HHSharedLibs += HHSharedCore HHSharedGUI HHSharedNetwork HHSharedService
    #win32{
    #    HHSharedLibs += HHSharedWindowsManagement
    #}

}

contains(HHSharedLibs, HHSharedCore){
    include(src/core/core.pri)
    INCLUDEPATH += src/core
    DEPENDPATH += src/core
    #HEADERS += $${HHSHAREDLIBS_ROOT_PATH}/include/HHSharedCore
    #HEADERS += ./include/HHSharedCore

}

contains(HHSharedLibs, HHSharedGUI){
    include(src/gui/gui.pri)
    INCLUDEPATH += src/gui
    DEPENDPATH += src/gui
    #HEADERS += ./include/HHSharedGUI
}

contains(HHSharedLibs, HHSharedNetwork){
    include(src/network/network.pri)
    INCLUDEPATH += src/network
    DEPENDPATH += src/network
}

contains(HHSharedLibs, HHSharedService){
    include(src/service/service.pri)
    INCLUDEPATH += src/service
    DEPENDPATH += src/service
}

contains(HHSharedLibs, HHSharedUDT){
    include(src/udt/udt.pri)
    INCLUDEPATH += src/udt
    DEPENDPATH += src/udt

    win32:win32-g++:DEFINES += __MINGW__
}

win32{
    contains(HHSharedLibs, HHSharedWindowsManagement){
        include(src/windowsmanagement/windowsmanagement.pri)
        INCLUDEPATH += src/windowsmanagement
        DEPENDPATH += src/windowsmanagement
    }
}

#INCLUDEPATH += $${HHSHAREDLIBS_ROOT_PATH}/include

