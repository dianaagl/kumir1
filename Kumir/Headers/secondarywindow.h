#ifndef SECONDARYWINDOW_H
#define SECONDARYWINDOW_H
#include <QtGlobal>

#ifdef Q_OS_WIN32
#include "secondarywindow_win32.h"
#endif

#ifdef Q_WS_X11
#include "secondarywindow_x11.h"
#endif

#ifdef Q_OS_MAC
#include "secondarywindow_mac.h"
#endif

#endif // SECONDARYWINDOW_H
