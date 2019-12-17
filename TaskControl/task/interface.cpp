#include "interface.h"
#include <QDebug>
#include <QtPlugin>

void ControlInterface::start(QString csName)
{
	CSname = csName;
	w.setCS(CSName());
	w.setup();
}

void ControlInterface::setCSmode(int mode)
{
	if (mode == 5) {
		w.setTeacher(true);
	}
	if (mode == -1) {
		qDebug() << "LockCheck";
		w.lockCheck();
	}
}

void ControlInterface::setCSinterface(CSInterface *interface)
{
	csInterface = interface;
	w.setInterface(interface);
}

void ControlInterface::checkFinished(int mark)
{
	Mark = mark;
	qDebug() << "Get Mark" << mark;
	w.setMark(mark);
}

Q_EXPORT_PLUGIN2(controlInterface, ControlInterface);
