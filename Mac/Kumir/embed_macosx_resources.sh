#!/bin/sh

rsync -aC ../../Kumir/PDScripts/* ../../kumir.app/Contents/Resources/
rsync ../../Kumir/Languages/* ../../kumir.app/Contents/Resources/
rsync -aC ../../Kumir/PDScripts/* ../../Debug/kumir.app/Contents/Resources/
rsync ../../Kumir/Languages/* ../../Debug/kumir.app/Contents/Resources/
rsync -aC *.msg kumir.app/Contents/Resources/
rsync -aC *.rules kumir.app/Contents/Resources/
rsync -aC *.js kumir.app/Contents/Resources/
rsync -aC Macro/*.macro kumir.app/Contents/Resources/
rsync -aC help/* kumir.app/Contents/Resources/help/
rsync -aC ht/* kumir.app/Contents/Resources/ht/
rsync -aC app_icons/mac/kumir.icns kumir.app/Contents/Resources/
rsync -aC mac/Info.plist kumir.app/Contents/
