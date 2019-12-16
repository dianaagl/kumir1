QT += svg script network
TEMPLATE = lib
TARGET = ../robot25d
CONFIG += plugin
INCLUDEPATH += ../

HEADERS += cellgraphicsitem.h \
    graphicsimageitem.h \
    robotcell.h roboterrors.h robotview.h \
    util.h robotitem.h sch_environment.h sch_command.h sch_game.h sch_algorhitm.h sch_task.h sch_program.h \
    kumirplugin.h \
    robot25dwindow.h
SOURCES += cellgraphicsitem.cpp \
    graphicsimageitem.cpp \
    robotview.cpp \
    util.cpp robotitem.cpp sch_environment.cpp sch_command.cpp sch_game.cpp sch_task.cpp sch_program.cpp \
    kumirplugin.cpp \
    robot25dwindow.cpp \
    sch_algorhitm.cpp

FORMS += \
    robot25dwindow.ui

RESOURCES +=
