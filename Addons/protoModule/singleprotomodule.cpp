#include "singleprotomodule.h"
#include "scriptenvironment.h"
#include "view25d.h"

SingleProtoModule::SingleProtoModule(const QString &rootDir,
                                     const QString &name,
                                     const VirtualModule &module) :
        QObject(),
        s_rootDir(rootDir),
        s_name(name),
        m_module(module)
{
    debug_syncCounter = debug_algCounter = 0;
    i_checkResult = 255;
    w_window = NULL;
}

void SingleProtoModule::start()
{

    m_environment = new ScriptEnvironment(this, s_rootDir);
    if (m_module.viewType.toUpper()=="2.5D") {
        m_25dView = new View25D(NULL, s_rootDir, m_environment);
        connect (m_25dView, SIGNAL(executionFinished(QString,QScriptValue)),
                 this, SLOT(handleExecutionFinished(QString,QScriptValue)));
        w_window = new QMainWindow;
        w_window->setWindowTitle(s_name);
//        m_25dView->setWindowTitle(s_name);
        w_window->setCentralWidget(m_25dView);
        QMenu *m = w_window->menuBar()->addMenu(tr("Environment"));
        m->addAction(tr("Load..."),this,SLOT(handleLoadEnvironmentAction()));
        m_25dView->setProtoScene(m_module.scene);
        for (int i=0; i<m_module.actors.size(); i++) {
            VirtualModuleActor actor = m_module.actors[i];
            m_25dView->addProtoActor(actor.clazz, actor.name);
        }
        m_25dView->reset();
    }
    else {
        qFatal("Views other than 2.5D are not implemented yet!");
    }
}

QString SingleProtoModule::name()
{
    return s_name;
}

QString parseType(const QString &cType) {
    if (cType=="int" ||
        cType=="integer") {
        return QString::fromUtf8("цел ");
    }
    else if (cType=="double" ||
             cType=="real" ||
             cType=="float") {
        return QString::fromUtf8("вещ ");
    }
    else if (cType=="bool" ||
             cType=="boolean") {
        return QString::fromUtf8("лог ");
    }
    else if (cType=="string" ||
             cType.simplified()=="char*" ||
             cType.simplified()=="char *"
             ) {
        return QString::fromUtf8("лит ");
    }
    else if (cType=="char" ||
             cType=="charect") {
        return QString::fromUtf8("сим ");
    }
    return "";
}

QVariant::Type getType(const QString &cType)
{
    if (cType=="int" ||
        cType=="integer") {
        return QVariant::Int;
    }
    else if (cType=="double" ||
             cType=="real" ||
             cType=="float") {
        return QVariant::Double;
    }
    else if (cType=="bool" ||
             cType=="boolean") {
        return QVariant::Bool;
    }
    else if (cType=="string" ||
             cType.simplified()=="char*" ||
             cType.simplified()=="char *"
             ) {
        return QVariant::String;
    }
    else if (cType=="char" ||
             cType=="charect") {
        return QVariant::Char;
    }
    return QVariant::Invalid;
}

QList<Alg> SingleProtoModule::algList()
{
    QList<Alg> result;
    for (int i=0; i<m_module.commands.size(); i++) {
        VirtualModuleCommand command = m_module.commands[i];
        Alg alg;
        alg.kumirHeader = QString::fromUtf8("алг ");
        QString kumType = parseType(command.returns);
        alg.isBoolean = kumType.trimmed()==QString::fromUtf8("лог");
        alg.kumirHeader += parseType(command.returns);
        alg.kumirHeader += command.command;
        QVariantList defaultParameters;
        if (command.arguments.count()>0) {
            alg.kumirHeader += "(";
            for (int j=0; j<command.arguments.count(); j++) {
                VirtualModuleCommandArg arg = command.arguments[j];
                kumType = parseType(arg.type);
                alg.kumirHeader += kumType;
                alg.kumirHeader += arg.name;
                defaultParameters << arg.defalut;
                if (j<command.arguments.count()-1)
                    alg.kumirHeader += ", ";
            }
            alg.kumirHeader += ")";
        }
        alg.pictomirNames << command.command;
        alg.defaultParameters << defaultParameters;
        if (alg.isBoolean) {
            alg.defaultParameters << defaultParameters;
            if (!command.negativeCommand.isEmpty()) {
                alg.pictomirNames << command.negativeCommand;
            }
            else {
                alg.pictomirNames << QString::fromUtf8("не ") +
                        command.command;
            }
        }
        result << alg;
    }
    return result;
}

QList<QVariant> SingleProtoModule::algOptResults()
{
    return QVariantList();
}

QVariant SingleProtoModule::result()
{
    if (i_checkResult==255)
        return v_lastResult;
    else {
        int result = (int)i_checkResult;
        i_checkResult = 255;
        return QVariant(result);
    }
}

void SingleProtoModule::showField()
{
//    m_25dView->setVisible(true);
    w_window->setVisible(true);
}

void SingleProtoModule::hideField()
{
//    m_25dView->setVisible(false);
    w_window->setVisible(false);
}

void SingleProtoModule::showPult()
{
    // nothing to do
}

void SingleProtoModule::hidePult()
{
    // nothing to do
}

void SingleProtoModule::setMode(int mode)
{
    Q_UNUSED(mode);
    // nothing to do
}

void SingleProtoModule::reset()
{
    if (m_module.viewType.toUpper()=="2.5D") {
        m_25dView->reset();
    }
}

void SingleProtoModule::setParameter(const QString &paramName,
                                     const QVariant &paramValue)
{
    if (paramName=="environment") {
        if (m_module.viewType.toUpper()=="2.5D") {
            m_25dView->loadEnvironment(paramValue.toString());
        }
    }
}

QString SingleProtoModule::help()
{
    return m_module.longDescriptionUrl.toString();
}

QWidget* SingleProtoModule::mainWidget()
{
    if (m_module.viewType.toUpper()=="2.5D")
//        return m_25dView;
        return w_window;
    else
        return NULL;
}

void SingleProtoModule::runAlg(QString alg, QList<QVariant>params)
{
    if (alg==QString::fromUtf8("тестирование")) {
        QString script = params.at(0).toString();
        i_checkResult = check(script);
        emit sync(true);
        return;
    }
    else {
        i_checkResult = 255;
    }

    // Формируем JavaScript-запрос на выполнение алгоритма.
    // Запрос состоит из:
    //   1) имени переменной - актора;
    //   2) имени вызываемой функции, принимающей в качестве первого
    //      параметра указатель на актора;
    //   3) опционального списка аргументов.

    QString functionName;
    QString actorName;
    QScriptValueList arguments;

    for (int i=0; i<m_module.commands.length(); i++) {
        VirtualModuleCommand command = m_module.commands[i];
        if (command.command==alg) {
            functionName = command.function;
            actorName = command.actor;
            for (int j=0; j<command.arguments.length(); j++) {
                VirtualModuleCommandArg arg = command.arguments[j];
                QVariant::Type type = getType(arg.type);
                if (type==QVariant::Int)
                    arguments.append(params[j].toInt());
                else if (type==QVariant::Double)
                    arguments.append(QScriptValue(params[j].toDouble()));
                else if (type==QVariant::String)
                    arguments.append(QScriptValue(params[j].toString()));
                else if (type==QVariant::Char)
                    arguments.append(QScriptValue(params[j].toChar()));
                else if (type==QVariant::Bool)
                    arguments.append(QScriptValue(params[j].toBool()));
                else
                    arguments.append(QScriptValue(QScriptValue::UndefinedValue));
            }
            break;
        }
    }
    Q_ASSERT(functionName.length()>0);
    Q_ASSERT(actorName.length()>0);

    // Определяем тип исполнителя. На данный момент это только 2.5D
    if (m_module.viewType.toUpper()=="2.5D") {
        m_25dView->evaluateActorFunction(actorName, functionName, arguments);
    }
    else {
        qFatal("Protos other then 2.5D are not implemented yet!");
    }

    debug_algCounter++;
//    qDebug() << "Alg call counter: " << debug_algCounter;

}

QString SingleProtoModule::errorText() const
{
    return s_lastErrorText;
}

void SingleProtoModule::connectSignalSendText(const QObject *obj,
                                              const char *method)
{
    Q_UNUSED(obj);
    Q_UNUSED(method);
    // TODO implement me!
}

void SingleProtoModule::connectSignalSync(const QObject *obj,
                                          const char *method)
{

    disconnect();
    connect ( this, SIGNAL(sync(bool)),
              obj, method, Qt::DirectConnection);
}

void SingleProtoModule::showOpt()
{
    // nothing to do
}

bool SingleProtoModule::hasPult()
{
    return false;
}



bool SingleProtoModule::hasOpt()
{
    return false;
}

QString SingleProtoModule::optText()
{
    return "";
}


void SingleProtoModule::handleExecutionFinished(const QString &error,
                                                const QScriptValue &ret)
{
    if (error.isEmpty()) {
        v_lastResult = ret.toVariant();
        s_lastErrorText = "";
        emit sync(true);
        debug_syncCounter ++;
//        qDebug() << "Syncs count: " << debug_syncCounter;
    }
    else {
        s_lastErrorText = error;
        emit sync(false);
    }
}


quint8 SingleProtoModule::check(const QString &script)
{
    if (m_module.viewType.toUpper()=="2.5D")
        return m_25dView->check(script);
    else
        qFatal("Not implemented!");
    return 0;
}

void SingleProtoModule::handleLoadEnvironmentAction()
{
    QSettings settings;
    const QString key = s_name+"_envdir";
    QString dir = settings.value(key,QDir::homePath()).toString();
    const QString fileName = QFileDialog::getOpenFileName(w_window,
                                                          tr("Load environment..."),
                                                          dir,
                                                          tr("Environments (*.fil)"));
    if (!fileName.isEmpty()) {
        QFileInfo fi(fileName);
        dir = fi.absoluteDir().absolutePath();
        settings.setValue(key, dir);
        m_25dView->loadEnvironment(fileName);
    }
}
