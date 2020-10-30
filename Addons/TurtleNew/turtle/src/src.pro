SOURCES += turtle.cpp \
    main.cpp \
    pult.cpp \
    network.cpp \
    interface.cpp
HEADERS += turtle.h \
    pult.h \
    network.h \
    ../../../plugin_interface.h \
    interface.h
CONFIG += warn_on \
    thread \
    qt \
    plugin
RESOURCES = application.qrc
QT += svg \
    network
FORMS += pult.ui
TEMPLATE = lib
TARGET = ../../../turtle
win32:TARGET = ../../../../turtle
macx:TARGET = ../../../../kumir.app/Contents/Resources/Addons/turtle

DISTFILES += \
    mymetadata.json
