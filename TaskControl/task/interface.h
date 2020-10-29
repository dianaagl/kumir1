#ifndef INTERFACE_H
#define INTERFACE_H

#include "../taskControlInterface.h"
#include <mainwindow.h>

class CSInterface;
class ControlInterface: public QObject, public taskControlInterface
{
	Q_OBJECT    
    Q_PLUGIN_METADATA(IID "kumir.taskControlInterface/1.7.1")
    Q_INTERFACES(taskControlInterface)


public:
	void start(QString csName);
	void setCSmode(int mode);
	void setCSinterface(CSInterface *interface);

	void setWindowGeometry(QRect rect)
	{
		w.setGeometry(rect);
	}

	void show()
	{
		w.showNormal();
	}

	void checkFinished(int mark);

	KumZadanie *Task()
	{
		return w.getTask();
	}

	QString Isp(int no)
	{
		return w.getTask()->Isp(no);
	}

	QString ispName() { return ""; }
	QString CSName() { return CSname; }


	CSInterface  *Interface()
	{
		return csInterface;
	}

private:
	MainWindowTask w;
	QString CSname;
	CSInterface *csInterface;
	int Mark;
};

#endif // INTERFACE_H
