#ifndef SCRIPTENVIRONMENT_H
#define SCRIPTENVIRONMENT_H

#include <QtCore>
#include <QtScript>

#include "jsutil.h"

class AbstractScene;
class AbstractActor;

class ScriptEnvironment :
        public QObject,
        public QScriptable
{
    Q_OBJECT;
    Q_PROPERTY ( QString ModuleRoot READ moduleRoot );
    Q_PROPERTY ( QString ApplicationRoot READ applicationRoot );
    Q_PROPERTY ( QString Log READ dummyString WRITE writeToStdOut );
    Q_PROPERTY ( QObject* Scene READ scene );

public:
    explicit ScriptEnvironment(QObject *parent, const QString &moduleRoot);
public slots:
    inline QString moduleRoot() const { return m_directories->moduleRoot(); }
    inline QString applicationRoot() const { return m_directories->applicationRoot(); }
    inline QString readText(const QString &fileName, const QString &encoding) const {
        return m_fileReader->readText(fileName, encoding);
    }
    inline void writeToStdOut(const QString &string) const {
        m_logger->writeToStdOut(string);
    }
    void setScene(AbstractScene *scene);
    inline QString dummyString() const { return QString(); }
    QObject* scene() const;
    QObject* actor(const QString &name) const;
    QStringList actorNames() const;
//    inline int actorsCount() const { return l_actors.count(); }
    void addActor(AbstractActor *actor);
private:
    Directories *m_directories;
    Logger *m_logger;
    FileReader *m_fileReader;
    AbstractScene *m_scene;
    QList<AbstractActor*> l_actors;

};

#endif // SCRIPTENVIRONMENT_H
