#include "plugin.h"

Plugin::Plugin()
{
    robot = new Robot();
    pult = new Pult(robot);
    QObject::connect(robot, SIGNAL(sendmsg(QString)),
                     pult, SLOT(setLabelText(QString)));
    QObject::connect(robot, SIGNAL(decline(QString)),
                     pult, SLOT(setLabelText(QString)));
}

Plugin::~Plugin()
{
    delete robot;
    delete pult;
}

void Plugin::start()
{
    robot->openFile("Addons/robotmaps/default.fil");
}

QList<Alg> Plugin::algList()
{
    QList<Alg> result;
    Alg a;
    a.iconSvgData   = 0;
    a.stepsCount    = 0;
    a.kumirHeader   = QString::fromUtf8("алг крас");
    result << a;
    a.kumirHeader   = QString::fromUtf8("алг жел");
    result << a;
    a.kumirHeader   = QString::fromUtf8("алг син");
    result << a;
    a.kumirHeader   = QString::fromUtf8("алг гол");
    result << a;
    a.kumirHeader   = QString::fromUtf8("алг лог стенакрас");
    result << a;
    a.kumirHeader   = QString::fromUtf8("алг лог стенажел");
    result << a;
    a.kumirHeader   = QString::fromUtf8("алг лог стенасин");
    result << a;
    a.kumirHeader   = QString::fromUtf8("алг лог стенагол");
    result << a;
    a.kumirHeader   = QString::fromUtf8("алг лог метка");
    result << a;
    a.kumirHeader   = QString::fromUtf8("алг лог закрашено");
    result << a;
    a.kumirHeader   = QString::fromUtf8("алг закрасить");
    result << a;
    a.kumirHeader   = QString::fromUtf8("алг цел темп");
    result << a;
    a.kumirHeader   = QString::fromUtf8("алг цел рад");
    result << a;
    a.kumirHeader   = QString::fromUtf8("алг лит текст");
    result << a;

    return result;
}

QString Plugin::name()
{
    return trUtf8("Роботор");
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
    return res;
}

void Plugin::runAlg(QString alg, QList<QVariant> params)
{
    if (alg == QString::fromUtf8("крас"))
        robot->moveUp();
    else if (alg == QString::fromUtf8("жел"))
        robot->moveRight();
    else if (alg == QString::fromUtf8("син"))
        robot->moveDown();
    else if (alg == QString::fromUtf8("гол"))
        robot->moveLeft();
    else if (alg == QString::fromUtf8("закрасить"))
        robot->fill();

    else if (alg == QString::fromUtf8("стенакрас"))
        res.setValue(robot->wallUp());
    else if (alg == QString::fromUtf8("стенажел"))
        res.setValue(robot->wallRight());
    else if (alg == QString::fromUtf8("стенасин"))
        res.setValue(robot->wallDown());
    else if (alg == QString::fromUtf8("стенагол"))
        res.setValue(robot->wallLeft());
    else if (alg == QString::fromUtf8("метка"))
        res.setValue(robot->isDotted());
    else if (alg == QString::fromUtf8("закрашено"))
        res.setValue(robot->isFilled());

    else if (alg == QString::fromUtf8("темп"))
        res.setValue(robot->temp());
    else if (alg == QString::fromUtf8("рад"))
        res.setValue(robot->rad());
    else if (alg == QString::fromUtf8("текст"))
        res.setValue(robot->text());

    emit sync();
}

void Plugin::showField()
{
    robot->show();
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
    robot->hide();
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
    robot->reset();
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
    return QString("Robot");
}

QWidget *Plugin::mainWidget()
{
    return robot;
}

Q_EXPORT_PLUGIN2(robotor, Plugin)
