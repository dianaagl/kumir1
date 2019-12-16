#ifndef VIRTUALOBJECTS_H
#define VIRTUALOBJECTS_H

#include <QtCore>

struct VirtualModuleCommandArg
{
    QString type;
    QString name;
    QVariant defalut;
};

struct VirtualModuleCommand
{
    QString command;
    QString negativeCommand;
    QString returns;
    QList<VirtualModuleCommandArg> arguments;
    QString function;
    QString actor;
};

struct VirtualModuleActor
{
    QString clazz;
    QString name;
};

struct VirtualModule
{
    QString moduleRoot;
    QString shortDescription;
    QUrl longDescriptionUrl;
    QList<VirtualModuleCommand> commands;
    QString viewType;
    QList<VirtualModuleActor> actors;
    QString scene;
    QString latinName;
};

#endif // VIRTUALOBJECTS_H
