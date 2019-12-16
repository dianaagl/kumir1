#ifndef PROTOMODULE_H
#define PROTOMODULE_H

#include <QtCore>
#include <QtScript>
#include "protomodule_interface.h"
#include "virtualobjects.h"

class ProtoModule :
        public QObject,
        public ProtoModuleInterface
{
    Q_OBJECT;
    Q_INTERFACES(ProtoModuleInterface);
public:
    ProtoModule();
    void setRootDirectory(const QString &path);
    QStringList virtualModulesList() const;
    QObject* createModule(const QString &virtualModuleName);
    QString moduleDescription(const QString &virtualModuleName) const;
    QUrl moduleHelpEntryPoint(const QString &virtualModuleName) const;   
protected:
    void loadProtoModule(const QString &path);
private:
    QString s_rootDirectory;
    QScriptEngine *m_scriptEngine;
    QMap<QString, VirtualModule> m_modules;

};

#endif // PROTOMODULE_H
