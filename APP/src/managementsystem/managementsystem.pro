TEMPLATE = subdirs
CONFIG += ordered \
    debug_and_release
SUBDIRS += sharedms \
    mainapp \  
    #help \
    clientservice \
    serverservice \
    plugins
