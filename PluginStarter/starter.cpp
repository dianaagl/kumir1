#include "starter.h"
#include "customwindow.h"

#include <QFileOpenEvent>


Starter::Starter(
	const QString &name,
	CustomWindow *mainWindow,
	CustomWindow *pultWindow,
	kumirPluginInterface *interface
) : QObject(0)
{
	Interface = interface;
	m_mainWindow = mainWindow;
	m_pultWindow = pultWindow;
	sName = name;
	connect(mainWindow, SIGNAL(closed()), this, SLOT(handleOneWindowClosed()));
	connect(pultWindow, SIGNAL(closed()), this, SLOT(handleOneWindowClosed()));
	QSettings settings("NIISI RAS", "KumPluginStarter");
	mainWindow->restoreGeometry(settings.value(name + "/MainWindowGeometry", "").toByteArray());
	pultWindow->restoreGeometry(settings.value(name + "/PultWindowGeometry", "").toByteArray());
}


void Starter::handleOneWindowClosed()
{
	QSettings settings("NIISI RAS", "KumPluginStarter");
	settings.setValue(sName + "/MainWindowGeometry", m_mainWindow->saveGeometry());
	settings.setValue(sName + "/PultWindowGeometry", m_pultWindow->saveGeometry());
	qApp->exit();
}


void Starter::loadFile(QString fname)
{
	fName = fname;
	QList<QVariant> args;
	Interface->runAlg("|open xml", args << QVariant(fName) << QVariant(fname));
}


bool Starter::eventFilter(QObject *watched, QEvent *event)
{
	Q_UNUSED(watched);

	switch (event->type()) {

	case QEvent::FileOpen:
		loadFile(static_cast<QFileOpenEvent *>(event)->file());
		return true;

	default:
		return false;
	}
}

