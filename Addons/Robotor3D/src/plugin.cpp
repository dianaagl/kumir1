#include "plugin.h"
#include "fstream"
using namespace std;


Plugin::Plugin()
{

    robot = new Robot();
    globj = new Window(robot);
    pult = new RoboPult();

    connectPult();
    QObject::connect(robot, SIGNAL(update3D()), globj, SLOT(updateWidget()));

    QObject::connect(robot, SIGNAL(result(QString, QString, QString)),
		     this,  SLOT(processRobotMessage(QString, QString, QString)));

//    globj->show();

    errorstring = "";
}


void Plugin::connectPult()
{
    QObject::connect(pult, SIGNAL(goLeft()), robot, SLOT(moveLeft()));
    QObject::connect(pult, SIGNAL(goRight()), robot, SLOT(moveRight()));
    QObject::connect(pult, SIGNAL(goUp()), robot, SLOT(moveUp()));
    QObject::connect(pult, SIGNAL(goDown()), robot, SLOT(moveDown()));

    QObject::connect(pult, SIGNAL(hasLeftWall()), robot, SLOT(wallLeft()));
    QObject::connect(pult, SIGNAL(hasRightWall()), robot, SLOT(wallRight()));
    QObject::connect(pult, SIGNAL(hasUpWall()), robot, SLOT(wallUp()));
    QObject::connect(pult, SIGNAL(hasDownWall()), robot, SLOT(wallDown()));

    QObject::connect(pult, SIGNAL(noLeftWall()), robot, SLOT(noWallLeft()));
    QObject::connect(pult, SIGNAL(noRightWall()), robot, SLOT(noWallRight()));
    QObject::connect(pult, SIGNAL(noUpWall()), robot, SLOT(noWallUp()));
    QObject::connect(pult, SIGNAL(noDownWall()), robot, SLOT(noWallDown()));



    QObject::connect(pult, SIGNAL(Color()), robot, SLOT(fill()));
    QObject::connect(pult, SIGNAL(Colored()), robot, SLOT(isFilled()));
    QObject::connect(pult, SIGNAL(Clean()), robot, SLOT(isClean()));

    QObject::connect(pult, SIGNAL(Temp()), robot, SLOT(temp()));
    QObject::connect(pult, SIGNAL(Rad()), robot, SLOT(rad()));

    QObject::connect(robot, SIGNAL(result(QString,QString,QString)),
		     pult->pltLogger(), SLOT(appendText(QString,QString,QString)));

    QObject::connect(pult, SIGNAL(update3D()), globj, SLOT(updateWidget()));

}

void Plugin::disconnectPult()
{
    QObject::disconnect(pult, SIGNAL(goLeft()), robot, SLOT(moveLeft()));
    QObject::disconnect(pult, SIGNAL(goRight()), robot, SLOT(moveRight()));
    QObject::disconnect(pult, SIGNAL(goUp()), robot, SLOT(moveUp()));
    QObject::disconnect(pult, SIGNAL(goDown()), robot, SLOT(moveDown()));

    QObject::disconnect(pult, SIGNAL(hasLeftWall()), robot, SLOT(wallLeft()));
    QObject::disconnect(pult, SIGNAL(hasRightWall()), robot, SLOT(wallRight()));
    QObject::disconnect(pult, SIGNAL(hasUpWall()), robot, SLOT(wallUp()));
    QObject::disconnect(pult, SIGNAL(hasDownWall()), robot, SLOT(wallDown()));

    QObject::disconnect(pult, SIGNAL(noLeftWall()), robot, SLOT(noWallLeft()));
    QObject::disconnect(pult, SIGNAL(noRightWall()), robot, SLOT(noWallRight()));
    QObject::disconnect(pult, SIGNAL(noUpWall()), robot, SLOT(noWallUp()));
    QObject::disconnect(pult, SIGNAL(noDownWall()), robot, SLOT(noWallDown()));



    QObject::disconnect(pult, SIGNAL(Color()), robot, SLOT(fill()));
    QObject::disconnect(pult, SIGNAL(Colored()), robot, SLOT(isFilled()));
    QObject::disconnect(pult, SIGNAL(Clean()), robot, SLOT(isClean()));

    QObject::disconnect(pult, SIGNAL(Temp()), robot, SLOT(temp()));
    QObject::disconnect(pult, SIGNAL(Rad()), robot, SLOT(rad()));

    QObject::disconnect(robot, SIGNAL(result(QString,QString,QString)),
		     pult->pltLogger(), SLOT(appendText(QString,QString,QString)));

    QObject::disconnect(pult, SIGNAL(update3D()), globj, SLOT(updateWidget()));

}


void Plugin::processRobotMessage(QString s1, QString s2, QString s3)
{
    if (s3 == robot->declineText()) {
	if (s2 == QString::fromUtf8("вправо"))
	    errorstring = QString::fromUtf8("справа стена!");
	if (s2 == QString::fromUtf8("влево"))
	    errorstring = QString::fromUtf8("слева стена!");
	if (s2 == QString::fromUtf8("вверх"))
	    errorstring = QString::fromUtf8("сверху стена!");
	if (s2 == QString::fromUtf8("вниз"))
	    errorstring = QString::fromUtf8("снизу стена!");
    }

}

Plugin::~Plugin()
{
    delete robot;
    delete pult;
    delete globj;
}

void Plugin::start()
{
//    int res = robot->openFile("../Addons/robotmaps/default.fil");

/*
    robot->setSize(10, 10);
    robot->setType(Robot::torus);
*/
}

QList<Alg> Plugin::algList()
{
    QList<Alg> result;
    Alg a;
    a.kumirHeader   = QString::fromUtf8("алг вверх");
    result << a;
    a.kumirHeader   = QString::fromUtf8("алг влево");
    result << a;
    a.kumirHeader   = QString::fromUtf8("алг вниз");
    result << a;
    a.kumirHeader   = QString::fromUtf8("алг вправо");
    result << a;
    a.kumirHeader   = QString::fromUtf8("алг лог сверху стена");
    result << a;
    a.kumirHeader   = QString::fromUtf8("алг лог слева стена");
    result << a;
    a.kumirHeader   = QString::fromUtf8("алг лог снизу стена");
    result << a;
    a.kumirHeader   = QString::fromUtf8("алг лог справа стена");
    result << a;

    a.kumirHeader   = QString::fromUtf8("алг лог сверху свободно");
    result << a;
    a.kumirHeader   = QString::fromUtf8("алг лог слева свободно");
    result << a;
    a.kumirHeader   = QString::fromUtf8("алг лог снизу свободно");
    result << a;
    a.kumirHeader   = QString::fromUtf8("алг лог справа свободно");
    result << a;


/*
    a.kumirHeader   = QString::fromUtf8("алг лог метка");
    result << a;
*/
    a.kumirHeader   = QString::fromUtf8("алг лог клетка закрашена");
    result << a;
    a.kumirHeader   = QString::fromUtf8("алг лог клетка чистая");
    result << a;

    a.kumirHeader   = QString::fromUtf8("алг закрасить");
    result << a;
    a.kumirHeader   = QString::fromUtf8("алг цел температура");
    result << a;
    a.kumirHeader   = QString::fromUtf8("алг цел радиация");
    result << a;
    a.kumirHeader   = QString::fromUtf8("алг лит текст");
    result << a;

    return result;
}

QString Plugin::name()
{
    return QString::fromUtf8("Роботор3Д");

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
    errorstring = "";

    if (alg == QString::fromUtf8("вверх"))
        robot->moveUp();
    else if (alg == QString::fromUtf8("влево"))
        robot->moveLeft();
    else if (alg == QString::fromUtf8("вниз"))
        robot->moveDown();
    else if (alg == QString::fromUtf8("вправо"))
        robot->moveRight();
    else if (alg == QString::fromUtf8("закрасить"))
        robot->fill();

    else if (alg == QString::fromUtf8("сверху стена"))
        res.setValue(robot->wallUp());
    else if (alg == QString::fromUtf8("слева стена"))
        res.setValue(robot->wallLeft());
    else if (alg == QString::fromUtf8("снизу стена"))
        res.setValue(robot->wallDown());
    else if (alg == QString::fromUtf8("справа стена"))
        res.setValue(robot->wallRight());

    else if (alg == QString::fromUtf8("сверху свободно"))
	res.setValue(!robot->wallUp());
    else if (alg == QString::fromUtf8("слева свободно"))
	res.setValue(!robot->wallLeft());
    else if (alg == QString::fromUtf8("снизу свободно"))
	res.setValue(!robot->wallDown());
    else if (alg == QString::fromUtf8("справа свободно"))
	res.setValue(!robot->wallRight());

/*
    else if (alg == QString::fromUtf8("метка"))
        res.setValue(robot->isDotted());
*/
    else if (alg == QString::fromUtf8("клетка закрашена"))
        res.setValue(robot->isFilled());
    else if (alg == QString::fromUtf8("клетка чистая"))
	res.setValue(!robot->isFilled());

    else if (alg == QString::fromUtf8("температура"))
        res.setValue(robot->temp());
    else if (alg == QString::fromUtf8("радиация"))
        res.setValue(robot->rad());
    else if (alg == QString::fromUtf8("текст"))
        res.setValue(robot->text());

    globj->updateWidget();
    emit sync();
}

void Plugin::showField()
{
    globj->show();
}

void Plugin::showPult()
{
    pult->show();
    robot->show();
}

void Plugin::hidePult()
{
    pult->hide();
    robot->hide();
}

void Plugin::hideField()
{
//    robot->hide();
    globj->hide();
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
    if (mode == 0) {
	disconnectPult();
	connectPult();
    }

    if (mode == 1) {
	disconnectPult();
    }

}

void Plugin::reset()
{
    robot->reset();
}

QString Plugin::errorText() const
{
    return errorstring;
}

void Plugin::setParameter(const QString &paramName, const QVariant &paramValue)
{


}

quint8 Plugin::check(const QString &script)
{
    return 0;
}

QWidget* Plugin::pultWidget()
{
    return pult;
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
    return QString::fromUtf8("Роботор");
}

QWidget *Plugin::mainWidget()
{
    return globj;

//    return robot;
}

QUrl     Plugin::pdfUrl() const
{
    return QUrl();
}

QUrl     Plugin::infoXmlUrl() const
{
    return QUrl();
}


Q_EXPORT_PLUGIN2(robotor3D, Plugin)
