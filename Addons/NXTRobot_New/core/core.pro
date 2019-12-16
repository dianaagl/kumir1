# -------------------------------------------------
# Project created by QtCreator 2010-01-19T18:04:05
# -------------------------------------------------
QT -= gui
CONFIG += static
TEMPLATE = lib
SOURCES += nxtcore.cpp \
    nxtconnection.cpp \
    nxtmotor.cpp \
    nxtsensor.cpp \
    subprocess.cpp \
    nxtdummyconnection.cpp \
    nxtbrick.cpp \
    nxtthread.cpp \
    nxtconnectionevent.cpp
HEADERS += nxtcore.h \
    nxtconnection.h \
    nxtmotor.h \
    nxtsensor.h \
    subprocess.h \
    nxtdummyconnection.h \
    nxtbrick.h \
    nxtthread.h \
    nxtconnectionevent.h
unix:include(unix.pri)
win32:include(win32.pri)
