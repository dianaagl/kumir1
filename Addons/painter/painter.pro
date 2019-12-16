#-------------------------------------------------
#
# Project created by QtCreator 2010-12-06T12:40:29
#
#-------------------------------------------------

QT       += core gui

TARGET = painter
TEMPLATE = lib
CONFIG += plugin

INCLUDEPATH += ../
DESTDIR = ../

SOURCES += painterplugin.cpp \
    painterwindow.cpp \
    painterview.cpp \
    painterruler.cpp \
    painternewimagedialog.cpp \
    paintertools.cpp

HEADERS += painterplugin.h \
    painterwindow.h \
    painterview.h \
    painterruler.h \
    painternewimagedialog.h \
    paintertools.h

FORMS += \
    painterwindow.ui \
    painternewimagedialog.ui
macx:TARGET = ../../kumir.app/Contents/Resources/Addons/painter
