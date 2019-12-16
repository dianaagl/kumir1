SUBDIRS += src
TEMPLATE = subdirs
QT += opengl
CONFIG += warn_on \
          qt \
          thread

dummy.path = .
unix:dummy.extra = python ../../Scripts/install_addon.py --addon=robotor3D --spec=unix --prefix=$$PREFIX --kumir-dir=$$KUMIR_DIR
macx:dummy.extra = python ../../Scripts/install_addon.py --addon=robotor3D --spec=macx --prefix=$$PREFIX --kumir-dir=$$KUMIR_DIR
win32:dummy.extra = python ../../Scripts/install_addon.py --addon=robotor3D --spec=win32 --prefix=$$PREFIX --kumir-dir=$$KUMIR_DIR
INSTALLS = dummy






