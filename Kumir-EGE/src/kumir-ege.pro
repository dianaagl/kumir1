TEMPLATE = app

QT -= gui
QT += script
unix:QMAKE_CXXFLAGS += -Werror -std=c++0x -Woverloaded-virtual -Wundef #-Wold-style-cast
TARGET = ../bin/ckumir

!equals(PWD, $${OUT_PWD}) {
    TARGET = ckumir
    win32: TARGET = ../ckumir
}

HEADERS += strtypes.h \
    application.h \
    int_proga.h \
    const.h \
    enums.h \
    tools.h \
    error.h \
    error_internal.h \
    text_analiz.h \
    compiler.h \
    messagesprovider.h \
    run.h \
    kum_instrument.h \
    kum_tables.h \
    kassert.h \
    integeroverflowchecker.h \
    doubleoverflowchecker.h \
    pushdownautomata.h \
    pwrulesnormalizer.h \
    kummodules.h \
    kumsinglemodule.h \
    symboltable.h
SOURCES += \
    main.cpp \
    application.cpp \
    int_proga.cpp \
    tools.cpp \
    text_analiz.cpp \
    compiler.cpp \
    messagesprovider.cpp \
    run.cpp \
    kum_instrument.cpp \
    kum_tables.cpp \
    integeroverflowchecker.cpp \
    doubleoverflowchecker.cpp \
    pushdownautomata.cpp \
    pwrulesnormalizer.cpp \
    kummodules.cpp \
    kumsinglemodule.cpp \
    symboltable.cpp
