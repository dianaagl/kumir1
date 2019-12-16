#-------------------------------------------------
#
# Project created by QtCreator 2010-09-23T17:53:07
#
#-------------------------------------------------

QT       += core xml
QT       -= gui

TARGET = ../kumir
TEMPLATE = lib
CONFIG += plugin

SOURCES += kumircompilerandruntime.cpp \
    textnormalizer.cpp \
    textanalizer.cpp \
    symboltable.cpp \
    functiontable.cpp \
    modulespool.cpp

HEADERS += kumircompilerandruntime.h \
    kumircommoninterface.h \
    kumir_c_interface.h \
    textnormalizer.h \
    kumirerrors.h \
    enums.h \
    ksymbols.h \
    errorcodes.h \
    normalizedtextinstruction.h \
    textanalizer.h \
    kumirinstruction.h \
    symboltable.h \
    functiontable.h \
    modulespool.h
