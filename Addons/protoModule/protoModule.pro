win32-g++:QMAKE_CXXFLAGS += -Werror
unix:QMAKE_CXXFLAGS += -Werror
#CONFIG += testing
QT += svg \
    script \
    xml scripttools
INCLUDEPATH += ../

# Embed libquazip
INCLUDEPATH += 3rd-party/quazip-0.3
HEADERS += 3rd-party/quazip-0.3/quazip/crypt.h \
3rd-party/quazip-0.3/quazip/ioapi.h \
3rd-party/quazip-0.3/quazip/JlCompress.h \
3rd-party/quazip-0.3/quazip/quaadler32.h \
3rd-party/quazip-0.3/quazip/quachecksum32.h \
3rd-party/quazip-0.3/quazip/quacrc32.h \
3rd-party/quazip-0.3/quazip/quazipfile.h \
3rd-party/quazip-0.3/quazip/quazipfileinfo.h \
3rd-party/quazip-0.3/quazip/quazip.h \
3rd-party/quazip-0.3/quazip/quazipnewinfo.h \
3rd-party/quazip-0.3/quazip/unzip.h \
3rd-party/quazip-0.3/quazip/zip.h

SOURCES += 3rd-party/quazip-0.3/quazip/ioapi.c \
3rd-party/quazip-0.3/quazip/unzip.c \
3rd-party/quazip-0.3/quazip/zip.c \
3rd-party/quazip-0.3/quazip/JlCompress.cpp \
3rd-party/quazip-0.3/quazip/quaadler32.cpp \
3rd-party/quazip-0.3/quazip/quacrc32.cpp \
3rd-party/quazip-0.3/quazip/quazip.cpp \
3rd-party/quazip-0.3/quazip/quazipfile.cpp \
3rd-party/quazip-0.3/quazip/quazipnewinfo.cpp

HEADERS += view25d.h \
    abstractactor.h \
    abstractscene.h \
    util.h \
    scene25d.h \
    actor25d.h \
    jsutil.h \
    graphicsimageitem.h \
    protomodule.h \
    protomodule_interface.h \
    singleprotomodule.h \
    virtualobjects.h \
    scriptenvironment.h
SOURCES += view25d.cpp \
    abstractactor.cpp \
    abstractscene.cpp \
    util.cpp \
    scene25d.cpp \
    actor25d.cpp \
    jsutil.cpp \
    graphicsimageitem.cpp \
    protomodule.cpp \
    singleprotomodule.cpp \
    scriptenvironment.cpp
include(../../Scripts/common.pri)
contains(CONFIG, testing) { 
    TEMPLATE = app
    HEADERS += test.h
    SOURCES += test.cpp
    TARGET = test_isometricProtoModule
    DESTDIR = ../../
}
!contains(CONFIG, testing) {
    TEMPLATE = lib
    TARGET = protoModule
    CONFIG += plugin
}
