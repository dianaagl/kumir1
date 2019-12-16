#include "starter.h"
#include "customwindow.h"

Starter::Starter(const QString &name, CustomWindow *mainWindow, CustomWindow *pultWindow,kumirPluginInterface  *interface) :
    QObject(0)
{
    Interface=interface;
    m_mainWindow = mainWindow;
    m_pultWindow = pultWindow;
    s_name = name;
    connect (mainWindow, SIGNAL(closed()), this, SLOT(handleOneWindowClosed()));
    connect (pultWindow, SIGNAL(closed()), this, SLOT(handleOneWindowClosed()));
    QSettings settings("NIISI RAS", "KumPluginStarter");
    mainWindow->restoreGeometry(settings.value(name+"/MainWindowGeometry","").toByteArray());
    pultWindow->restoreGeometry(settings.value(name+"/PultWindowGeometry","").toByteArray());
}

void Starter::handleOneWindowClosed()
{
    QSettings settings("NIISI RAS", "KumPluginStarter");
    settings.setValue(s_name+"/MainWindowGeometry",m_mainWindow->saveGeometry());
    settings.setValue(s_name+"/PultWindowGeometry",m_pultWindow->saveGeometry());
    qApp->exit();
}


bool Starter::eventFilter ( QObject * watched, QEvent * event )
{
    switch (event->type()) {
            case QEvent::FileOpen:{
                loadFile(static_cast<QFileOpenEvent *>(
                         event)->file());
               };
                return true;
            default:
                return false;
};
};
