#ifndef MODULESPOOL_H
#define MODULESPOOL_H

#include <QtCore>

class FunctionTable;
class SymbolTable;

class ModulesPool : public QObject
{
    Q_OBJECT
public:
    explicit ModulesPool(QObject *parent = 0);
    QPair<int,int> funcByName(const QString &name) const;
    int moduleId(const QString &name) const;
    bool moduleEnabled(int id) const;
    inline bool moduleEnabled(const QString &name) { return moduleEnabled(moduleId(name)); }
    FunctionTable* functionTable(int moduleId);
    SymbolTable* symbolTable(int moduleId);
    QString moduleName(int moduleId) const;


signals:

public slots:

};

#endif // MODULESPOOL_H
