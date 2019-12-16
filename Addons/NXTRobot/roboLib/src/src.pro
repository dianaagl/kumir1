LIBS += -L../../core \
    -lcore
unix:LIBS += -lusb \
    -lbluetooth
win32:LIBS += -lwsock32
SOURCES += turtle.cpp \
    main.cpp \
    pult.cpp \
    network.cpp \
    interface.cpp \

HEADERS += turtle.h \
    pult.h \
    network.h \
    ../../../plugin_interface.h \
    interface.h \

CONFIG += warn_on \
    thread \
    qt \
    plugin
RESOURCES = application.qrc
QT += svg \
    network
FORMS += pult.ui \
    control.ui
TEMPLATE = lib
unix:TARGET = ../../../roboLib
win32:TARGET = ../../../../roboLib
