#include "protomodule.h"
#include "plugin_interface.h"
#include "singleprotomodule.h"
#include "util.h"

#include <QtXml>

ProtoModule::ProtoModule() :
        QObject()
{
    m_scriptEngine = NULL;
}


void ProtoModule::setRootDirectory(const QString &path)
{
    s_rootDirectory = path;
    if (m_scriptEngine)
        m_scriptEngine->deleteLater();

    m_scriptEngine = new QScriptEngine;
    m_modules.clear();

    QDir root(path);
    foreach (const QString &entry, root.entryList(QStringList() << "*.proto" << "*.proto.jar")) {
        if (entry.endsWith(".proto") && QFileInfo(root.absoluteFilePath(entry)).isDir()) {
            loadProtoModule(root.absoluteFilePath(entry));
        }
        if (entry.endsWith(".proto.jar")) {
            loadProtoModule(root.absoluteFilePath(entry));
        }
    }
}

void ProtoModule::loadProtoModule(const QString &path)
{    
    QDir modulesDir(path+"/modules");
    const QStringList modulesFileList = getEntryList(path, "modules");
    foreach (const QString &entry, modulesFileList) {
        if (entry.endsWith(".xml")) {
            QDomDocument moduleDocument;
            QByteArray data = getFileData(path, "modules/"+entry);
            QString xmlError;
            int xmlErrorLine;
            int xmlErrorColumn;
            if (!moduleDocument.setContent(data,&xmlError,
                                           &xmlErrorLine, &xmlErrorColumn)) {
                qDebug() << "Error parsing " << entry << " at " <<
                        xmlErrorLine << ":" << xmlErrorColumn;
                qDebug() << " --- " << xmlError;
                continue;
            }
            QDomNode module = moduleDocument.documentElement();
            if (!module.toElement().attribute("name","").isEmpty()) {
                QString name = module.toElement().attribute("name","");
                QString url = module.toElement().attribute("url","");
                QString scene = module.toElement().attribute("scene","");
                QString view = module.toElement().attribute("view","");
                if (scene.isEmpty() || view.isEmpty())
                    continue ; // Module is not valid!
                VirtualModule mod;
                mod.moduleRoot = path;
                mod.longDescriptionUrl = QUrl(url);
                mod.scene = scene;
                mod.viewType = view;
                mod.latinName = entry.left(entry.length()-4);
                QString xmlText;
                for (int c1=0; c1<module.childNodes().count(); c1++) {
                    QDomNode child1 = module.childNodes().at(c1);
                    if (child1.isText()) {
                        xmlText += child1.toText().data();
                    }
                    else if (child1.isElement() &&
                             child1.nodeName()=="actor") {
                        QString actorClass =
                                child1.toElement().attribute("class","");
                        QString actorName =
                                child1.toElement().attribute("name","");
                        if (actorClass.isEmpty() || actorName.isEmpty())
                            continue; // Actor is not valid!
                        VirtualModuleActor act;
                        act.clazz = actorClass;
                        act.name = actorName;
                        mod.actors << act;
                    }
                    else if (child1.isElement() &&
                             child1.nodeName()=="command") {
                        QString commandName =
                                child1.toElement().attribute("name","");
                        QString commandNegativeName =
                                child1.toElement().attribute("negative_name","");
                        QString returnType =
                                child1.toElement().attribute("return","void");
                        QString actor =
                                child1.toElement().attribute("actor","");
                        QString func =
                                child1.toElement().attribute("func","");
                        if (commandName.isEmpty()) {
                            continue ; // Command is not valid
                        }
                        VirtualModuleCommand cmd;
                        cmd.command = commandName;
                        cmd.negativeCommand = commandNegativeName;
                        cmd.actor = actor;
                        cmd.function = func;
                        cmd.returns = returnType;
                        for (int c2=0; c2<child1.childNodes().count(); c2++) {
                            QDomNode child2 =
                                    child1.childNodes().at(c2);
                            if (child2.isElement() &&
                                child2.nodeName()=="argument") {
                                QString type =
                                        child2.toElement().attribute("type","");
                                QString argumentName =
                                        child2.toElement().attribute("name","");
                                QString def =
                                        child2.toElement().attribute("default","");
                                if (type.isEmpty()) {
                                    continue; // Argument not valid
                                }
                                VirtualModuleCommandArg argument;
                                argument.type = type;
                                argument.name = argumentName;
                                argument.defalut = def;
                                cmd.arguments << argument;
                            }
                        }
                        mod.commands << cmd;
                    }
                }
                m_modules.insert(name, mod);
            }
        }
    }
}

QStringList ProtoModule::virtualModulesList() const
{
    return m_modules.keys();
}

QString ProtoModule::moduleDescription(const QString &virtualModuleName) const
{
    if (!m_modules.contains(virtualModuleName))
        return "";
    else
        return m_modules[virtualModuleName].shortDescription;
}

QUrl ProtoModule::moduleHelpEntryPoint(const QString &virtualModuleName) const
{
    if (!m_modules.contains(virtualModuleName))
        return QUrl();
    else
        return m_modules[virtualModuleName].longDescriptionUrl;
}

QObject* ProtoModule::createModule(
        const QString &virtualModuleName
        )
{
    if (!m_modules.contains(virtualModuleName))
        return NULL;
    SingleProtoModule *m = new SingleProtoModule(m_modules[virtualModuleName].moduleRoot,
                                                 virtualModuleName,
                                                 m_modules[virtualModuleName]);
    return m;
}



Q_EXPORT_PLUGIN2(protoModule, ProtoModule);
