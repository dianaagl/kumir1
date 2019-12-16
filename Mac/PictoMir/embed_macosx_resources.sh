#!/bin/sh

rsync -aC *.qm PictoMir.app/Contents/Resources/
rsync -aC ht/* PictoMir.app/Contents/Resources/ht/
rsync -aC tasks/* PictoMir.app/Contents/Resources/tasks/
rsync -aC environments/* PictoMir.app/Contents/Resources/environments/
rsync -aC images/* PictoMir.app/Contents/Resources/images/
rsync -aC ../../app_icons/mac/pictomir.icns PictoMir.app/Contents/Resources/
rsync -aC mac/Info.plist PictoMir.app/Contents/
rsync -aC ../isometricRobot/libisometricRobot.dylib PictoMir.app/Contents/PlugIns/
rsync -aC loadTaskDialog PictoMir.app/Contents/MacOS/

install_name_tool -id @executable_path/../Frameworks/QtCore.framework/Versions/4.0/QtCore\
	PictoMir.app/Contents/Frameworks/QtCore.framework/Versions/4/QtCore
install_name_tool -id @executable_path/../Frameworks/QtGui.framework/Versions/4.0/QtGui\
	PictoMir.app/Contents/Frameworks/QtGui.framework/Versions/4/QtGui
install_name_tool -id @executable_path/../Frameworks/QtSvg.framework/Versions/4.0/QtSvg\
	PictoMir.app/Contents/Frameworks/QtSvg.framework/Versions/4/QtSvg

install_name_tool -change /Library/Frameworks/QtCore.framework/Versions/4/QtCore\
	@executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore\
	PictoMir.app/Contents/MacOS/loadTaskDialog
install_name_tool -change /Library/Frameworks/QtGui.framework/Versions/4/QtGui\
	@executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui\
	PictoMir.app/Contents/MacOS/loadTaskDialog

install_name_tool -change /Library/Frameworks/QtCore.framework/Versions/4/QtCore\
	@executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore\
	PictoMir.app/Contents/PlugIns/libisometricRobot.dylib
install_name_tool -change /Library/Frameworks/QtGui.framework/Versions/4/QtGui\
	@executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui\
	PictoMir.app/Contents/PlugIns/libisometricRobot.dylib
install_name_tool -change /Library/Frameworks/QtSvg.framework/Versions/4/QtSvg\
	@executable_path/../Frameworks/QtSvg.framework/Versions/4/QtSvg\
	PictoMir.app/Contents/PlugIns/libisometricRobot.dylib
