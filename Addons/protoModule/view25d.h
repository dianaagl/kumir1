#ifndef VIEW25D_H
#define VIEW25D_H

#include <QtCore>
#include <QtGui>
#include <QtScript>
#include <QtScriptTools>

class Scene25D;
class Actor25D;
class View25D;

class ScriptEnvironment;

class View25D : public QGraphicsView
{
Q_OBJECT;
public:
    explicit View25D(QWidget *parent,const QString &rootDir, ScriptEnvironment *env);
    void setProtoScene(const QString &sceneClass);
    void addProtoActor(const QString &actorClass, const QString &actorName);
    quint8 check(const QString &script = QString());
    void loadEnvironment(const QString &fileName);

signals:

public slots:
    void evaluateActorFunction(const QString &actorName,
                               const QString &functionName,
                               const QScriptValueList &arguments);
    void reset();

protected slots:
    void handleActorExecutionFinished();
    void reloadScripts();

protected:
    virtual void timerEvent(QTimerEvent *);
    virtual void contextMenuEvent(QContextMenuEvent *event);
    qreal r_sceneRotationAngle, r_sceneSlopeAngle, r_zMax;
    Scene25D *m_scene;
    ScriptEnvironment *m_scriptEnvironment;

    QString s_rootDir;
    Actor25D* a_currentEvaluatingActor;

    QMap<QString,Actor25D*> m_actors;



signals:
    void executionFinished(const QString &error, const QScriptValue &result);

};

#endif // VIEW25D_H
