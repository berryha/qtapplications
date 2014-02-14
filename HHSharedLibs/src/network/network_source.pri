
HEADERS += \
#    rudp/rudpsocket.h \
#    rudp/rudppacket.h \
#    rudp/rudp_global.h \
#    rudp/rudpchannel.h \
#    rudp/rudppacketstreamoperator.h \
#    packethandler/packetparserbase.h \
#    networkmanagerbase.h \
    packethandler/packetstreamoperator.h \
    udp/udpsocket.h \
    udp/multicast/ipmulticast.h \
    udp/multicast/ipmulticastsocketbase.h \
    packethandler/packet.h \
    packethandler/packethandlerbase.h \
    networkutilities.h \
    global_network.h \
    networklib.h \
    tcp/tcpbase.h

FORMS += 
SOURCES += \
#    rudp/rudpsocket.cpp \
#    rudp/rudppacket.cpp \
#    rudp/rudpchannel.cpp \
#    rudp/rudppacketstreamoperator.cpp \
#    packethandler/packetparserbase.cpp \
#    networkmanagerbase.cpp \
    udp/udpsocket.cpp \
    udp/multicast/ipmulticastsocketbase.cpp \
    packethandler/packet.cpp \
    packethandler/packethandlerbase.cpp \
    networkutilities.cpp \
    packethandler/packetstreamoperator.cpp \
    tcp/tcpbase.cpp
    
#win32 {
#    HEADERS += udp/multicast/multicastwin.h
#    SOURCES += udp/multicast/multicastwin.cpp
    
#    LIBS += -lws2_32

#    win32-g++{
#        LIBS += -Lresources/lib \
#        -lwsock32
#    }
#}
#unix {
#    HEADERS += udp/multicast/multicastlinux.h
#    SOURCES += udp/multicast/multicastlinux.cpp
#}

RESOURCES +=