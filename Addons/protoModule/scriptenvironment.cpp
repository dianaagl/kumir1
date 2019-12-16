#include "scriptenvironment.h"
#include "jsutil.h"
#include "abstractactor.h"
#include "abstractscene.h"

ScriptEnvironment::ScriptEnvironment(QObject *parent, const QString &moduleRoot) :
        QObject(parent),
        QScriptable()
{
    m_directories = new Directories(this, moduleRoot);
    m_logger = new Logger(this);
    m_fileReader = new FileReader(this);
    m_scene = NULL;
}


QObject* ScriptEnvironment::actor(const QString &name) const
{
    QObject *result = NULL;
    for (int i=0; i<l_actors.length(); i++) {
        if (l_actors[i]->instanceName()==name) {
            result = l_actors[i];
            break;
        }
    }
    return result;
}

void ScriptEnvironment::addActor(AbstractActor *actor)
{
    l_actors << actor;
}

QObject* ScriptEnvironment::scene() const
{
    return m_scene;
}

void ScriptEnvironment::setScene(AbstractScene *scene)
{
    m_scene = scene;
}

QStringList ScriptEnvironment::actorNames() const
{
    QStringList result;
    for (int i=0; i<l_actors.size(); i++) {
        result.append(l_actors[i]->instanceName());
    }
    return result;
}
