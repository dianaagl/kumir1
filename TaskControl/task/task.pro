# -------------------------------------------------
# Project created by QtCreator 2010-03-09T17:26:04
# -------------------------------------------------
QT += svg \
    xml \
    webkit

TEMPLATE = lib
CONFIG += plugin
TARGET = ../taskControl
macx:TARGET = ../../kumir.app/Contents/Resources/taskControl


SOURCES += main.cpp \
    mainwindow.cpp \
    ../course_model.cpp \
    interface.cpp \
    editdialog.cpp \
    newkursdialog.cpp
HEADERS += mainwindow.h \
    ../course_model.h \
    interface.h \
    editdialog.h \
    newkursdialog.h
FORMS += mainwindow.ui \
    editdialog.ui \
    newkursdialog.ui
OTHER_FILES += 
RESOURCES += marks.qrc
