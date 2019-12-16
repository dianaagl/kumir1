#ifndef ABSTRACTACTOR_H
#define ABSTRACTACTOR_H

#include <QtCore>
#include <QtGui>
#include <QtScript>
#include <QtScriptTools>

class ScriptEnvironment;

class AbstractActor :
        public QThread,
        public QScriptable
{
Q_OBJECT;
Q_PROPERTY( quint16 x READ x WRITE setX );
Q_PROPERTY( quint16 y READ y WRITE setY );
Q_PROPERTY( quint16 z READ z WRITE setZ );
Q_PROPERTY( bool enabled READ enabled WRITE setEnabled );
Q_PROPERTY( bool visible READ visible WRITE setVisible );
Q_PROPERTY( QString name READ instanceName );
Q_PROPERTY( QString clazz READ className );
Q_PROPERTY( bool evaluating READ isEvaluating );
public:
    explicit AbstractActor(QObject *parent,
                           ScriptEnvironment *env,
                           const QString &moduleRoot,
                           const QString &className,
                           const QString &instanceName);

public slots:
    QScriptValue thisObject();
    void showDebugWindow();
    bool isEvaluating();
    virtual void evaluateCommand(const QString &function,
                                 bool waitForFinished,
                                 const QScriptValueList& arguments);
    void evaluateCommand(const QString &function, bool waitForFinished);
    void evaluateCommand(const QString &function,
                         bool waitForFinished,
                         const QVariant &arg1);
    void evaluateCommand(const QString &function,
                         bool waitForFinished,
                         const QVariant &arg1, const QVariant &arg2);
    void evaluateCommand(const QString &function,
                         bool waitForFinished,
                         const QVariant &arg1, const QVariant &arg2,
                         const QVariant &arg3);
    inline quint16 x() { return i_x; }
    inline quint16 y() { return i_y; }
    inline quint16 z() { return i_z; }
    inline QString className() const { return s_className; }
    inline QString instanceName() const { return s_instanceName; }
    inline bool enabled() const { return b_enabled; }
    virtual bool visible() const = 0;
    inline void setX(quint16 xx) { i_x = xx; setPosition(xx, y(), z()); }
    inline void setY(quint16 yy) { i_y = yy; setPosition(x(), yy, z()); }
    inline void setZ(quint16 zz) { i_z = zz; setPosition(x(), y(), zz); }
    virtual void setPosition(quint16 x, quint16 y, quint16 z) = 0;
    inline void setEnabled(bool v) { b_enabled = v; }
    virtual void setVisible(bool v) = 0;
    inline QScriptValue lastEvaluationResult() const { return m_result; }
    inline void setAttribute(const QString &name, const QVariant &value) { m_attributes[name] = value; }
    inline QVariant attribute(const QString &name) const { return m_attributes.contains(name)? m_attributes[name] : QVariant(); }
    inline void sleep_sec(quint32 seconds) { sleep(seconds); }
    inline void sleep_msec(quint32 milliseconds) { msleep(milliseconds); }


protected:
    virtual void run();

    QScriptValueList m_arguments;
    QScriptValue m_result;

    QMutex *mutex_functionCall;
    QString s_function;

    QMap<QString, QVariant> m_attributes;
    QScriptEngine *m_scriptEngine;
    QScriptEngineDebugger *m_debugger;
    QScriptValue js_thisObject;
    QString s_moduleRoot;
    QString s_instanceName;
    QString s_className;

    quint16 i_x;
    quint16 i_y;
    quint16 i_z;

    bool b_enabled;

    QMutex *mutex_updateFinished;
    bool b_updateFinished;

    QScriptValue m_scriptEngineObject;


signals:
    void executionFinished();

};

#endif
