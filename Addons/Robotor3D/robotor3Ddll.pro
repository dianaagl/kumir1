#-------------------------------------------------
#
# Project created by QtCreator 2009-10-09T11:58:40
#
#-------------------------------------------------

TEMPLATE = lib
TARGET = robotor3D
QT += opengl

FORMS += pult.ui

RESOURCES += icons.qrc

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
        plugin_interface.h
