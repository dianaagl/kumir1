/****************************************************************************
**
** Copyright (C) 2004-2010 NIISI RAS. All rights reserved.
**
** This file is part of the KuMir.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "customwindow.h"


#include <iostream>
#include "plugin_interface.h"
#include <QtCore>
#include <QtGui>

#ifndef WIN32
#include "unistd.h"
#endif

#include "starter.h"

int main(int argc, char *argv[])
{
	Q_INIT_RESOURCE(application);
	QApplication app(argc, argv);
	app.setQuitOnLastWindowClosed(true);
	QString fileName = "no fname";
	QString port = "";
	QString xml = "";
	if (argc > 1) {
		//std::cout<<"argc"<<argc<<" argv "<<argv[1]<<"|\n";
		QString param = QString(argv[1]);
		if (param == "-h") {
			std::cout << "Kumir Plugin starter V1.1\n";
			std::cout << "usage:./pluginstarter [KumPluginFile] -o plugin_options \n";
			return 0;
		};
		fileName = param;

		if (argc > 2) {
			port = QString(argv[2]);

			if (argc < 4) {
				std::cout << "No xml file name\n";
				return -1;
			};
			qDebug() << "Arg3" << argv[3] << "arg4" << argv[4];
			xml = QString(argv[3]);

		}
	};
	QString modulesInfoPath = QApplication::applicationDirPath();
	qDebug() << modulesInfoPath + '/' + "../Resources/libvodoley.dylib";
	QString vodoleyLoad = modulesInfoPath + '/' + "../Frameworks/libvodoley.dylib";

#ifdef WIN32
	vodoleyLoad = modulesInfoPath + "/Addons/vodoley.dll";
#endif

	QPluginLoader loader(vodoleyLoad);

	if (kumirPluginInterface  *interface =
			qobject_cast<kumirPluginInterface *>(loader.instance())) {
		std::cout << interface->name().toLocal8Bit().data() << "\n";

		interface->start();
		QWidget *pult = interface->pultWidget();
		QWidget *module = interface->mainWidget();
		bool resX = module->property("resizableX").toBool();
		bool resY = module->property("resizableY").toBool();
		QString name = interface->name();
		CustomWindow *moduleWindow = new CustomWindow(module, name, resX, resY, "");
		moduleWindow->setWindowIcon(QIcon(":/module_window.png"));
		CustomWindow *pultWindow = new CustomWindow(pult, name + QString::fromUtf8(" - Пульт"), false, false, "p_");
		pultWindow->setWindowIcon(QIcon(":/pult_window.png"));
		Starter *s = new Starter(name, moduleWindow, pultWindow, interface);
		app.installEventFilter(s);
		moduleWindow->show();
		pultWindow->show();
//		interface->showPult();
//		interface->showField();
//		interface->showOpt();
		QList<QVariant> args;
		// interface->runAlg("|open port", args << QVariant(7777));
		qDebug() << "xml file:" << xml;

		QStringList argums = app.arguments();
		QString argum = "";
		for (int i = 0; i < argums.count(); i++) {
			argum = argum + " " + argums[i];
		}
		qDebug() << "Args" << argum;

		interface->runAlg("|open xml", args << QVariant(fileName) << QVariant(xml));
		std::cout << "OK\n";
		return app.exec();
	} else {
		std::cout << loader.errorString().toLocal8Bit().data() << "\n";
		return -1;
	};

	qDebug("-h for help");
	return 0;
}

