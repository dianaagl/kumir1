SOURCES += main.cpp \
    pult.cpp \
    network.cpp \
    vodoley.cpp \
    dialog.cpp \
    kumfiledialog.cpp \
    interface.cpp
HEADERS += pult.h \
    network.h \
    vodoley.h \
    dialog.h \
    kumfiledialog.h \
    ../../plugin_interface.h \
    interface.h \
    ../../isp_window_header.h
CONFIG += warn_on \
    thread \
    qt \
    plugin \
    dll
RESOURCES = application.qrc
QT += svg \
    network
FORMS += pult.ui \
    dialog.ui
TARGET = ../../vodoley
win32:TARGET = ../../../vodoley
#macx:TARGET = ../../../kumir.app/Contents/Resources/Addons/vodoley
macx:TARGET = vodoley
TEMPLATE = lib

DISTFILES += \
    mymetadata.json \
    mymetadata.json
