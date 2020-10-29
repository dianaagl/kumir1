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
	Q_UNUSED(mode);
	qDebug() << "DUMMY call ControlInterface::setCSmode";
}

void ControlInterface::setCSinterface(CSInterface *interface)
{
	csInterface = interface;
	w.setInterface(interface);
}

QString ControlInterface::ispName()
{
	return "";
}

void ControlInterface::checkFinished(int mark)
{
	Mark = mark;
	qDebug() << "Get Mark" << mark;
	qDebug() << " ControlInterface::checkFinished calls duumy w.setMark(mark)";
}

//Q_EXPORT_PLUGIN2(controlInterface, ControlInterface);
