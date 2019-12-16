#include "view25d.h"

#include "scene25d.h"
#include "actor25d.h"
#include "scriptenvironment.h"



View25D::View25D(QWidget *parent, const QString &rootDir, ScriptEnvironment *env) :
    QGraphicsView(parent),
    s_rootDir(rootDir)
{
    setScene(new QGraphicsScene);
    m_scene = NULL;
    m_scriptEnvironment = env;
    startTimer(40);
}


void View25D::setProtoScene(const QString &sceneName)
{
    const QString sceneRoot = s_rootDir+
                              "/scene/"+
                              sceneName;

    m_scene = new Scene25D(this,
                           s_rootDir,
                           sceneName,
                           scene(),
                           m_scriptEnvironment);
    m_scriptEnvironment->setScene(m_scene);
    m_scene->setSceneRootDir(sceneRoot);
}

void View25D::addProtoActor(const QString &actorClass, const QString &actorName)
{
    Actor25D *actor = new Actor25D(this, scene(), m_scriptEnvironment,
                                   s_rootDir, actorClass, actorName);
    connect(actor, SIGNAL(executionFinished()),
            this, SLOT(handleActorExecutionFinished()));
    actor->setScene(m_scene);
    actor->loadActorFromDir("actors/"+actorClass);
    m_actors[actorName] = actor;
    m_scriptEnvironment->addActor(actor);
    actor->start();
}

void View25D::reloadScripts()
{
    const QString sceneRoot = s_rootDir+
                              "/scene/"+
                              m_scene->className();
    m_scene->setSceneRootDir(sceneRoot);
    foreach (const QString &key, m_actors.keys()) {
        Actor25D *actor = m_actors[key];
        actor->loadActorFromDir(s_rootDir+"/actors/"+actor->className());
    }
    reset();
}

//QVariant View25D::evaluateScript(const QString &program, const QString &fileName)
//{
//    Q_UNUSED(program);
//    Q_UNUSED(fileName);
////    if (!m_scriptEngine) {
////        qDebug() << "Script engine not initialized!";
////        return QVariant();
////    }
////    QScriptValue value = m_scriptEngine->evaluate(program,fileName);
////    if (value.isError()) {
////        qDebug() << "Error evaluating script: ";
////        qDebug() << value.toString();
////        qDebug() << m_scriptEngine->currentContext()->backtrace();
////        return QVariant();
////    }
////    else {
////        return value.toVariant();
////    }
//    return QVariant();
//}

void View25D::evaluateActorFunction(const QString &actorName,
                                    const QString &functionName,
                                    const QScriptValueList &arguments)
{
    Actor25D *actor = m_actors.value(actorName, NULL);
    a_currentEvaluatingActor = actor;
    Q_CHECK_PTR(actor);
    actor->evaluateCommand(functionName, false, arguments);
}

void View25D::handleActorExecutionFinished()
{
    if (sender()==a_currentEvaluatingActor) {
        QScriptValue result = a_currentEvaluatingActor->lastEvaluationResult();
        emit executionFinished("", result);
    }
}


void View25D::reset()
{
    qDebug() << "Reset...";
    m_scene->reset();
    a_currentEvaluatingActor = NULL;
    qDebug() << "Reset complete!";
}

void View25D::timerEvent(QTimerEvent *event)
{
    event->accept();
    viewport()->update();
}

void View25D::contextMenuEvent(QContextMenuEvent *event)
{
    event->accept();
    QMenu *contextMenu = new QMenu("Debug",this);
    contextMenu->setTearOffEnabled(true);
    contextMenu->addAction("Reload proto module", this, SLOT(reloadScripts()));
    contextMenu->addSeparator();
    contextMenu->addAction("Debug window for scene", m_scene, SLOT(showDebugWindow()));
    contextMenu->addSeparator();
    foreach (const QString &key,  m_actors.keys()) {
        Actor25D *actor = m_actors[key];
        contextMenu->addAction("Debug window for actor '"+actor->instanceName()+"'",
                               actor, SLOT(showDebugWindow()));
    }
    contextMenu->exec(event->globalPos());
}

quint8 View25D::check(const QString &script)
{
    return m_scene->check(script, true);
}

void View25D::loadEnvironment(const QString &fileName)
{
    m_scene->loadEnvironment(fileName);
}
