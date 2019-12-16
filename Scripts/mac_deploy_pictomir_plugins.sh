#!/bin/sh

install_name_tool -change QtCore.framework/Versions/4/QtCore  @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore pictomir.app/Contents/PlugIns/libisometricRobot.dylib

install_name_tool -change QtGui.framework/Versions/4/QtGui  @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui pictomir.app/Contents/PlugIns/libisometricRobot.dylib

install_name_tool -change QtSvg.framework/Versions/4/QtSvg  @executable_path/../Frameworks/QtSvg.framework/Versions/4/QtSvg pictomir.app/Contents/PlugIns/libisometricRobot.dylib

install_name_tool -change QtCore.framework/Versions/4/QtCore  @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore pictomir.app/Contents/PlugIns/libkrt.dylib


