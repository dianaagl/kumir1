OBJECTS_DIR = Build/Objects
MOC_DIR = Build/Moc
UI_DIR = Build/Ui
RCC_DIR = Build/Rcc
linux-g++: include(unix.pri)
freebsd-g++: include(unix.pri)
linux-g++-64: include(unix.pri)
freebsd-g++-64: include(unix.pri)
macx: include(mac.pri)
win32: include(win32.pri)
