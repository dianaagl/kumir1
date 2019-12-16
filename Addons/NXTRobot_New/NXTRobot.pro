TEMPLATE = subdirs
SUBDIRS = core roboLib
dummy.path = .
unix:dummy.extra = python ../../Scripts/install_addon.py --addon=roboLib --spec=unix --prefix=$$PREFIX --kumir-dir=$$KUMIR_DIR
macx:dummy.extra = python ../../../Scripts/install_addon.py --addon=roboLib --spec=macx --prefix=$$PREFIX --kumir-dir=$$KUMIR_DIR
win32:dummy.extra = python ../../../Scripts/install_addon.py --addon=roboLib --spec=win32 --prefix=$$PREFIX --kumir-dir=$$KUMIR_DIR
INSTALLS = dummy