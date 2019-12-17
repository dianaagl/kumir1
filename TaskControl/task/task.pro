# -------------------------------------------------
# Project created by QtCreator 2010-03-09T17:26:04
# -------------------------------------------------
QT += svg xml webkit

TEMPLATE = lib
CONFIG += plugin

TARGET = ../taskControl
macx:TARGET = ../../kumir.app/Contents/Resources/taskControl

SOURCES += \
	../course_model.cpp \
	main.cpp \
	mainwindow.cpp \
	interface.cpp \
	editdialog.cpp \
	newkursdialog.cpp

HEADERS += \
	../course_model.h \
	../course_changes.h \
	../csInterface.h \
	../taskControlInterface.h \
	mainwindow.h \
	interface.h \
	editdialog.h \
	newkursdialog.h

FORMS += \
	mainwindow.ui \
	editdialog.ui \
	newkursdialog.ui

RESOURCES += marks.qrc
