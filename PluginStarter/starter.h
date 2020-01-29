#ifndef STARTER_H
#define STARTER_H

#include "plugin_interface.h"
#include <QObject>
#include <QString>

class CustomWindow;
class QEvent;

class Starter : public QObject
{
	Q_OBJECT

public:
	explicit Starter(
		const QString &name,
		CustomWindow *mainWindow,
		CustomWindow *pultWindow,
		kumirPluginInterface  *interface
	);

	bool eventFilter(QObject *watched, QEvent *event);

	void loadFile(QString fname);
	QString getFileName() const { return fName; }

protected slots:
	void handleOneWindowClosed();

private:
	CustomWindow *m_mainWindow;
	CustomWindow *m_pultWindow;
	QString sName, fName;
	kumirPluginInterface *Interface;
};

#endif // STARTER_H
