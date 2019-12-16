#ifndef STARTER_H
#define STARTER_H

#include <QtCore>
#include <QtGui>
#include "plugin_interface.h"
class CustomWindow;

class Starter : public QObject
{
    Q_OBJECT
public:
    explicit Starter(
            const QString &name,
            CustomWindow *mainWindow,
            CustomWindow *pultWindow,kumirPluginInterface  *interface);
bool	eventFilter ( QObject * watched, QEvent * event );
void loadFile(QString fname)
{
    fileName=fname;
    QList<QVariant> args;
    Interface->runAlg("|open xml", args<<QVariant(fileName) << QVariant(fname));
};
QString FileName()
{
    return fileName;
}

protected slots:
    void handleOneWindowClosed();

private:

    CustomWindow *m_mainWindow;
    CustomWindow *m_pultWindow;
    QString s_name,fileName;
kumirPluginInterface  *Interface;
};

#endif // STARTER_H
