SOURCES += main.cpp customwindow.cpp \
    starter.cpp
HEADERS += customwindow.h customwindow_p.h \
    starter.h
TEMPLATE = app
CONFIG += warn_on \
	  thread \
          qt \ x11
QT += svg \
network
RESOURCES += application.qrc
TARGET = pluginstarter
win32:TARGET = pluginstarter
HEADERS += ../Addons/plugin_interface.h
INCLUDEPATH += ../Addons
include(../Scripts/common.pri)
dummy.path = ./
unix: dummy.extra = python ../Scripts/install_pluginstarter.py --spec=unix --prefix=$$PREFIX --kumir-dir=$$KUMIR_DIR
macx: dummy.extra = python ../Scripts/install_pluginstarter.py --spec=macx --prefix=$$PREFIX --kumir-dir=$$KUMIR_DIR
win32: dummy.extra = python ../Scripts/install_pluginstarter.py --spec=win32 --prefix=$$PREFIX --kumir-dir=$$KUMIR_DIR
INSTALLS = dummy
