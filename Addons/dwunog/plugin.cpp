#include "plugin.h"

Plugin::Plugin()
{
    world = new DwunogWorld(this);
    pult = new Pult(world);
    QObject::connect(world, SIGNAL(sendmsg1(QString)),
                     pult, SLOT(setLabelText(QString)));
}

Plugin::~Plugin()
{
    delete world;
    delete pult;
}

void Plugin::start()
{
    world->openf("addons/dwunogmaps/ladder.def");
}

QList<Alg> Plugin::algList()
{
    QList<Alg> result;
    Alg a;
    a.kumirHeader   = QString::fromUtf8("алг голову влево(цел угол)");
    result << a;
    a.kumirHeader   = QString::fromUtf8("алг голову вправо(цел угол)");
    result << a;
    a.kumirHeader   = QString::fromUtf8("алг голову прямо");
    result << a;
    a.kumirHeader   = QString::fromUtf8("алг ждать(цел угол)");
    result << a;
    a.kumirHeader   = QString::fromUtf8("алг красную ногу вверх(цел угол)");
    result << a;
    a.kumirHeader   = QString::fromUtf8("алг красную ногу вниз(цел угол)");
    result << a;
    a.kumirHeader   = QString::fromUtf8("алг зеленую ногу вверх(цел угол)");
    result << a;
    a.kumirHeader   = QString::fromUtf8("алг зеленую ногу вверх(цел угол)");
    result << a;
    return result;
}

QString Plugin::name()
{
    return trUtf8("Dwunog");
}

void Plugin::emitSync()
{
    emit sync();
}

QList<QVariant> Plugin::algOptResults()
{
    QList<QVariant> tmp;
    tmp << QVariant("NO OPT RESULTS");
    return tmp;
}

QVariant Plugin::result()
{
    return QVariant(" ");;
}

void Plugin::runAlg(QString alg, QList<QVariant> params)
{
    int arg;
    QString cmd;

    arg = params[0].toInt();

    if (alg == QString::fromUtf8("ждать"))
        world->doCommand("wait", arg);
    else if (alg == QString::fromUtf8("голову влево"))
        world->doCommand("hturn", arg);
    else if (alg == QString::fromUtf8("голову вправо"))
        world->doCommand("hturn", -arg);
    else if (alg == QString::fromUtf8("голову прямо"))
        world->doCommand("headup");
    else if (alg == QString::fromUtf8("красную ногу вверх"))
        world->doCommand("lfturn", -arg);
    else if (alg == QString::fromUtf8("красную ногу вниз"))
        world->doCommand("lfturn", arg);
    else if (alg == QString::fromUtf8("зеленую ногу вверх"))
        world->doCommand("rfturn", arg);
    else if (alg == QString::fromUtf8("зеленую ногу вниз"))
        world->doCommand("rfturn", -arg);
//    emit sync();
}

void Plugin::showField()
{
    world->show();
}

void Plugin::showPult()
{
    pult->show();
}

void Plugin::hidePult()
{
    pult->hide();
}

void Plugin::hideField()
{
    world->hide();
}

void Plugin::showOpt()
{

}

bool Plugin::hasPult()
{
    return 1;
}

bool Plugin::hasOpt()
{
    return 0;
}

QString Plugin::optText()
{
    return "";
}

void Plugin::setMode(int mode)
{

}

void Plugin::reset()
{
    world->doCommand("reset");
}

QString Plugin::errorText() const
{
    return "";
}

void Plugin::setParameter(const QString &paramName, const QVariant &paramValue)
{

}

void Plugin::connectSignalSendText( const QObject *obj, const char *method )
{
    connect(this, SIGNAL(sendText(QString)), obj, method);
}

void Plugin::connectSignalSync( const QObject *obj, const char *method)
{
    connect(this, SIGNAL(sync()), obj, method);
}

QString Plugin::help()
{
    return QString("Dwunog");
}

QWidget *Plugin::mainWidget()
{
    return world;
}

Q_EXPORT_PLUGIN2(dwunog, Plugin)
