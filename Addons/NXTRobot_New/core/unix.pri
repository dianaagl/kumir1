TARGET = core

INCLUDEPATH += unix

HEADERS += unix/adc_8591.h \
           unix/bluetooth.h \
           unix/brick.h \
           unix/color.h \
           unix/compass.h \
           unix/connection.h \
           unix/core.h \
           unix/DistNx.h \
           unix/error.h \
           unix/filesystem.h \
           unix/gyro.h \
           unix/i2c.h \
           unix/io_8574.h \
           unix/light.h \
           unix/motor.h \
           unix/nxt.h \
           unix/rcx_light.h \
           unix/rotation.h \
           unix/sensor.h \
           unix/sonar.h \
           unix/sound.h \
           unix/temperature.h \
           unix/tilt.h \
           unix/touch.h \
           unix/usbnxt.h
SOURCES += unix/adc_8591.cpp \
           unix/bluetooth.cpp \
           unix/brick.cpp \
           unix/color.cpp \
           unix/compass.cpp \
           unix/DistNx.cpp \
           unix/error.cpp \
           unix/filesystem.cpp \
           unix/i2c.cpp \
           unix/io_8574.cpp \
           unix/motor.cpp \
           unix/sensor.cpp \
           unix/sonar.cpp \
           unix/tilt.cpp \
           unix/usbnxt.cpp
