SOURCES += robot.cpp\
        pult.cpp\
        plugin.cpp\
        window.cpp\
        glwidget.cpp

HEADERS  += robot.h\
        pult.h\
        window.h\
        glwidget.h\
        plugin.h\
         ../../plugin_interface.h \
    ../../plugin_interface.h

CONFIG += warn_on \
    thread \
    qt \
    plugin \
    dll

RESOURCES += ../resources/icons.qrc

QT += opengl

FORMS += pult.ui

TARGET = ../../robotor3D
win32:TARGET = ../../../robotor3D
macx:TARGET = ../../../kumir.app/Contents/Resources/Addons/robotor3D

TEMPLATE = lib
