TEMPLATE = subdirs
SUBDIRS = 
dummy.path = .
unix:dummy.extra = python ../../Scripts/install_convertor.py --addon=convertor --spec=unix --prefix=$$PREFIX --kumir-dir=$$KUMIR_DIR
macx:dummy.extra = python ../../Scripts/install_convertor.py --addon=convertor --spec=macx --prefix=$$PREFIX --kumir-dir=$$KUMIR_DIR
win32:dummy.extra = python ../../Scripts/install_convertor.py --addon=convertor --spec=win32 --prefix=$$PREFIX --kumir-dir=$$KUMIR_DIR
INSTALLS = dummy          