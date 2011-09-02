TEMPLATE = subdirs
CONFIG += ordered \
    debug_and_release
SUBDIRS += sharedms \
    mainapp \
    clientservice \
    serverservice \
    plugins
