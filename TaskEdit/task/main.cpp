#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setWindowIcon(QIcon(":/taskEdit.ico"));
	MainWindowTask w;
	w.setWindowIcon(QIcon(":/taskEdit.ico"));
	w.setup();
	w.show();
	return a.exec();
}
