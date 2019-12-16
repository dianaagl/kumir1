# -------------------------------------------------
# Project created by QtCreator 2009-10-22T15:43:04
# -------------------------------------------------
QT += svg
CONFIG += plugin
TARGET = isometricRobot

TEMPLATE = lib


INCLUDEPATH += ../
DEFINES += ISOMETRICROBOT_LIBRARY
SOURCES += robotview.cpp \
    plugin.cpp \
    graphicsimageitem.cpp \
    util.cpp \
    robotitem.cpp
HEADERS += robotview.h \
    constants.h \
    plugin.h \
    robotcell.h \
    roboterrors.h \
    graphicsimageitem.h \
    util.h \
    robotitem.h
TRANSLATIONS += resources/ru_RU.ts
include(../../Scripts/common.pri)
dummy.path = .
unix:dummy.extra = python ../../Scripts/install_addon.py --addon=isometricRobot --spec=unix --prefix=$$PREFIX --kumir-dir=$$KUMIR_DIR --resources=resources
macx:dummy.extra = python ../../Scripts/install_addon.py --addon=isometricRobot --spec=macx --prefix=$$PREFIX --kumir-dir=$$KUMIR_DIR --resources=resources
win32:dummy.extra = python ../../Scripts/install_addon.py --addon=isometricRobot --spec=win32 --prefix=$$PREFIX --kumir-dir=$$KUMIR_DIR --resources=resources
INSTALLS = dummy          

RESOURCES += resources.qrc
