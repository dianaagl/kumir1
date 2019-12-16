#include "abstractactor.h"
#include "scriptenvironment.h"

AbstractActor::AbstractActor(QObject *parent,
                             ScriptEnvironment *env,
                             const QString &moduleRoot,
                             const QString &className,
                             const QString &instanceName) :
    QThread(parent),
    s_moduleRoot(moduleRoot),
    s_instanceName(instanceName),
    s_className(className),
    b_enabled(false)
{
    b_updateFinished = false;
    s_function = "";
    mutex_updateFinished = new QMutex;
    mutex_functionCall = new QMutex;
    m_scriptEngine = new QScriptEngine(this);
    m_debugger = new QScriptEngineDebugger(this);
    m_debugger->attachTo(m_scriptEngine);
    m_debugger->standardWindow()->setWindowTitle(instanceName);
    m_debugger->setAutoShowStandardWindow(true);
    QScriptValue obj = m_scriptEngine->newQObject(env);
    m_scriptEngine->globalObject().setProperty("G", obj);
}


void AbstractActor::run()
{
    QString function;
    forever{
        mutex_functionCall->lock();
        function = s_function;
        mutex_functionCall->unlock();
        if (function.isEmpty()) {
            msleep(20);
            continue;
        }
        Q_ASSERT (!m_scriptEngine->isEvaluating());
        QScriptValue func = m_scriptEngine->globalObject().property(function);

        QScriptValueList args;

        QScriptValue actor = thisObject();

        for (int i=0; i<m_arguments.count(); i++) {
            args << m_arguments[i];
        }

        QScriptValue ret = func.call(actor, args);
        while (m_scriptEngine->isEvaluating()) {
            msleep(20);
        }
        if (ret.isError()) {
            qDebug() << "Execution error: ";
            qDebug() << ret.toString();
        }
        m_result = ret;
        mutex_functionCall->lock();
        s_function = "";
        mutex_functionCall->unlock();
        emit executionFinished();
    }
}

bool AbstractActor::isEvaluating()
{
    bool result;
    mutex_functionCall->lock();
    result = !s_function.isEmpty();
    mutex_functionCall->unlock();
    return result;
}

QScriptValue AbstractActor::thisObject()
{
//    if (js_thisObject.isNull()) {
        js_thisObject = m_scriptEngine->newQObject(this);
        QScriptValueIterator it(m_scriptEngine->globalObject());
        while (it.hasNext()) {
            it.next();
            if (it.value().isFunction()) {
                js_thisObject.setProperty(it.name(), it.value());
            }
        }
//    }
    return js_thisObject;
}

void AbstractActor::evaluateCommand(const QString &function,
                                    bool waitForFinished,
                                    const QScriptValueList &arguments)
{
    mutex_functionCall->lock();
    s_function = function;
    m_arguments = arguments;
    mutex_functionCall->unlock();
    if (waitForFinished)
        wait();
}

void AbstractActor::evaluateCommand(const QString &function, bool waitForFinished)
{
    evaluateCommand(function, waitForFinished, QScriptValueList());
}

QScriptValue scriptValueFromVariant(const QVariant &v)
{
    if (v.type()==QVariant::Int)
        return QScriptValue(v.toInt());
    else if (v.type()==QVariant::Double)
        return QScriptValue(v.toDouble());
    else if (v.type()==QVariant::Char)
        return QScriptValue(v.toChar());
    else if (v.type()==QVariant::String)
        return QScriptValue(v.toString());
    else if (v.type()==QVariant::Bool)
        return QScriptValue(v.toBool());
    else
        return QScriptValue(QScriptValue::UndefinedValue);
}

void AbstractActor::evaluateCommand(const QString &function, bool waitForFinished, const QVariant &arg1)
{
    QScriptValueList arguments;
    arguments << scriptValueFromVariant(arg1);
    evaluateCommand(function, waitForFinished, arguments);
}

void AbstractActor::evaluateCommand(const QString &function, bool waitForFinished,
                                    const QVariant &arg1, const QVariant &arg2)
{
    QScriptValueList arguments;
    arguments << scriptValueFromVariant(arg1);
    arguments << scriptValueFromVariant(arg2);
    evaluateCommand(function, waitForFinished, arguments);
}

void AbstractActor::evaluateCommand(const QString &function, bool waitForFinished,
                                    const QVariant &arg1, const QVariant &arg2, const QVariant &arg3)
{
    QScriptValueList arguments;
    arguments << scriptValueFromVariant(arg1);
    arguments << scriptValueFromVariant(arg2);
    arguments << scriptValueFromVariant(arg3);
    evaluateCommand(function, waitForFinished, arguments);
}

void AbstractActor::showDebugWindow()
{
    m_debugger->standardWindow()->show();
}

