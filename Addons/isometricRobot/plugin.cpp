#include "plugin.h"
#include "robotview.h"

Plugin::Plugin()
{
    a_actionDefaultEnvironment = NULL;
    a_actionLoadEnvironment = NULL;
    m_robotView = NULL;   
    m_stepsToDone = -1;
}

void Plugin::init()
{

}

QString Plugin::moduleImagesRoot()
{
    return ":/resources/";
}

QList<PictomirPluginInterface::Command> Plugin::conditions() const
{
    const QString imagesRoot = moduleImagesRoot();
    QList<Command> result;
    Command cmd;
    QFile f;

    cmd.id = 0;
    cmd.name = "cell not painted";
    cmd.name_i18n[QLocale::Russian] = QString::fromUtf8("клетка не закрашена");
    cmd.tooltip = "Is cell clean?";
    cmd.tooltip_i18n[QLocale::Russian] = QString::fromUtf8("Клетка чистая?");
    cmd.shortcut = QString::fromUtf8("ч?");
    f.setFileName(imagesRoot+"/icons/conditions/clean.svg");
    f.open(QIODevice::ReadOnly);
    cmd.svgImageData = f.readAll();
    f.close();
    result << cmd;
    cmd = Command();

    cmd.id = 1;
    cmd.name = "cell painted";
    cmd.name_i18n[QLocale::Russian] = QString::fromUtf8("клетка закрашена");
    cmd.tooltip = "Is cell painted?";
    cmd.tooltip_i18n[QLocale::Russian] = QString::fromUtf8("Клетка закрашена?");
    cmd.shortcut = QString::fromUtf8("з?");
    f.setFileName(imagesRoot+"/icons/conditions/painted.svg");
    f.open(QIODevice::ReadOnly);
    cmd.svgImageData = f.readAll();
    f.close();
    result << cmd;
    cmd = Command();

    cmd.id = 2;
    cmd.name = "not wall ahead";
    cmd.name_i18n[QLocale::Russian] = QString::fromUtf8("впереди не стена");
    cmd.tooltip = "Is free ahead?";
    cmd.tooltip_i18n[QLocale::Russian] = QString::fromUtf8("Впереди свободно?");
    cmd.shortcut = QString::fromUtf8("св?");
    f.setFileName(imagesRoot+"/icons/conditions/free.svg");
    f.open(QIODevice::ReadOnly);
    cmd.svgImageData = f.readAll();
    f.close();
    result << cmd;
    cmd = Command();

    cmd.id = 3;
    cmd.name = "wall ahead";
    cmd.name_i18n[QLocale::Russian] = QString::fromUtf8("впереди стена");
    cmd.tooltip = "Is wall ahead?";
    cmd.tooltip_i18n[QLocale::Russian] = QString::fromUtf8("Впереди стена?");
    cmd.shortcut = QString::fromUtf8("ст?");
    f.setFileName(imagesRoot+"/icons/conditions/wall.svg");
    f.open(QIODevice::ReadOnly);
    cmd.svgImageData = f.readAll();
    f.close();
    result << cmd;

    return result;
}

QList<PictomirPluginInterface::Command> Plugin::actions() const
{
    const QString imagesRoot = moduleImagesRoot();
    QList<Command> result;
    Command cmd;
    QFile f;

    cmd.id = 5;
    cmd.name = "turn left";
    cmd.name_i18n[QLocale::Russian] = QString::fromUtf8("повернуть налево");
    cmd.tooltip = "Turn left";
    cmd.tooltip_i18n[QLocale::Russian] = QString::fromUtf8("Повернуть налево");
    cmd.shortcut = QString::fromUtf8("л");
    f.setFileName(imagesRoot+"/icons/actions/turn_left.svg");
    f.open(QIODevice::ReadOnly);
    cmd.svgImageData = f.readAll();
    f.close();
    result << cmd;
    cmd = Command();

    cmd.id = 4;
    cmd.name = "go forward";
    cmd.name_i18n[QLocale::Russian] = QString::fromUtf8("вперед");
    cmd.tooltip = "Go forward";
    cmd.tooltip_i18n[QLocale::Russian] = QString::fromUtf8("Двигаться вперед");
    cmd.shortcut = QString::fromUtf8("в");
    f.setFileName(imagesRoot+"/icons/actions/forward.svg");
    f.open(QIODevice::ReadOnly);
    cmd.svgImageData = f.readAll();
    f.close();
    result << cmd;
    cmd = Command();

    cmd.id = 6;
    cmd.name = "turn right";
    cmd.name_i18n[QLocale::Russian] = QString::fromUtf8("повернуть направо");
    cmd.tooltip = "Turn right";
    cmd.tooltip_i18n[QLocale::Russian] = QString::fromUtf8("Повернуть направо");
    cmd.shortcut = QString::fromUtf8("п");
    f.setFileName(imagesRoot+"/icons/actions/turn_right.svg");
    f.open(QIODevice::ReadOnly);
    cmd.svgImageData = f.readAll();
    f.close();
    result << cmd;
    cmd = Command();

    cmd.id = 7;
    cmd.name = "paint";
    cmd.name_i18n[QLocale::Russian] = QString::fromUtf8("закрасить");
    cmd.tooltip = "Paint cell";
    cmd.tooltip_i18n[QLocale::Russian] = QString::fromUtf8("Закрасить клетку");
    cmd.shortcut = QString::fromUtf8("з");
    f.setFileName(imagesRoot+"/icons/actions/paint.svg");
    f.open(QIODevice::ReadOnly);
    cmd.svgImageData = f.readAll();
    f.close();
    result << cmd;

    return result;
}


void Plugin::createMenuBar()
{
    QString translationsRoot;
#ifndef Q_OS_MAC
    translationsRoot = QCoreApplication::applicationDirPath()+"/Addons/isometricRobot/resources";
#endif
#ifdef Q_OS_WINCE
	translationsRoot = QCoreApplication::applicationDirPath()+"/data/";
#endif
    QString locale = QLocale::system().name();
    if (QFile::exists(translationsRoot+"/"+locale+".qm")) {
        QTranslator *translator = new QTranslator();
        translator->load(translationsRoot+"/"+locale+".qm");
        QCoreApplication::instance()->installTranslator(translator);
    }
    QMenu *menu = new QMenu(tr("Planet"));
    a_actionLoadEnvironment = menu->addAction(tr("Load environment..."),this,SLOT(loadEnvironmentClicked()));
    a_actionDefaultEnvironment = menu->addAction(tr("Load default environment"),this,SLOT(loadDefaultEnvironment()));
    m_menuBar << menu;
}

void Plugin::createMainWidget(TargetDevice device)
{
    QString environmentsRoot;
#ifndef Q_OS_MAC
    environmentsRoot = QCoreApplication::applicationDirPath()+"/Addons/isometricRobot/resources";
#endif
#ifdef Q_OS_WINCE
	environmentsRoot = QCoreApplication::applicationDirPath()+"/data/";
#endif
    bool opts = device==Destkop||device==Netbook||device==TabletPC;
    m_robotView = new RobotView(opts, opts, opts, opts? QSize(600,400) : QSize(0,0));
    m_robotView->setPlugin(this);
    if (!opts) {
//        m_robotView->scale(0.5, 0.5);
    }
    m_robotView->loadEnvironment(environmentsRoot+"/_default.fil");
}

QList<QMenu*> Plugin::menuBar()
{
    if (m_menuBar.isEmpty())
        createMenuBar();
    return m_menuBar;
}

QWidget *Plugin::mainWidget(TargetDevice device)
{
    if (m_robotView==NULL)
        createMainWidget(device);
    return m_robotView;
}

QWidget *Plugin::controlWidget()
{
    return NULL;
}

void Plugin::loadEnvironmentClicked()
{
    QSettings s;
    QString dir = s.value("Addons/IsometricRobot/LastDir",QDir::homePath()).toString();
    QString fn = QFileDialog::getOpenFileName(NULL, tr("Load environment"), dir, tr("Environments (*.fil)"));
    if (!fn.isEmpty()) {
        m_robotView->loadEnvironment(fn);
        dir = QFileInfo(fn).absoluteDir().absolutePath();
        s.setValue("Addons/IsometricRobot/LastDir",dir);
    }
}

void Plugin::loadDefaultEnvironment()
{
    QString imagesRoot;
    QString environmentsRoot;
    QString translationsRoot;
#ifndef Q_OS_MAC
    translationsRoot = environmentsRoot = imagesRoot = QCoreApplication::applicationDirPath()+"/Addons/isometricRobot/resources";
#endif
#ifdef Q_OS_WINCE
	translationsRoot = environmentsRoot = imagesRoot = QCoreApplication::applicationDirPath()+"/data/";
#endif
    m_robotView->loadEnvironment(environmentsRoot+"/_default.fil");
}



QString Plugin::name(const QLocale::Language language) const
{
    if (language==QLocale::Russian)
        return QString::fromUtf8("Робот_Вертун");
    else
        return "Rotating_Robot";
}


QVariant Plugin::lastResult() const
{
    return QVariant(m_lastResult);
}

QString Plugin::lastError(QLocale::Language language) const
{
    if (e_lastError==RobotErrors::WallCollisionError) {
        if (language==QLocale::Russian)
            return QString::fromUtf8("Робот ударился об стену и разбился");
        else
            return "Wall collision caused to robot damage";
    }
    return "";
}

void Plugin::reset()
{
    m_robotView->reset();
}

void Plugin::evaluateCommand(int id, bool animated)
{
    /*
     0 - cell clean
     1 - cell painted
     2 - free ahead
     3 - wall ahead
     4 - go forward
     5 - turn left
     6 - turn right
     7 - paint
     */
    Q_ASSERT ( id>=0 && id<=7 );
    e_lastError = RobotErrors::NoRuntimeError;
    m_robotView->setAnimated(animated);
    if (id==0) {
        bool v = m_robotView->isPainted();
        v = !v;
        m_lastResult = v;
    }
    else if (id==1) {
        bool v = m_robotView->isPainted();
        m_lastResult = v;
    }
    else if (id==2) {
        bool v = m_robotView->isWall();
        v = !v;
        m_lastResult = v;
    }
    else if (id==3) {
        bool v = m_robotView->isWall();
        m_lastResult = v;
    }
    else if (id==4) {
        if (!m_robotView->goForward()) {
            e_lastError = RobotErrors::WallCollisionError;
        }
    }
    else if (id==5) {
        m_robotView->turnLeft();
    }
    else if (id==6) {
        m_robotView->turnRight();
    }
    else if (id==7) {
        m_robotView->doPaint();
    }
}

void Plugin::evaluateCommandAnimated(int id)
{
    evaluateCommand(id, true);
}

void Plugin::evaluateCommandHidden(int id)
{
    evaluateCommand(id, false);
}


void Plugin::finishEvaluation()
{
    m_robotView->finishEvaluation();
}

PictomirPluginInterface::TaskScore Plugin::checkEvaluationResult(QLocale::Language language) const
{
    TaskScore score;
    if (m_robotView->unpaintedPoints()>0) {
        score.value = 1;
        if (language==QLocale::Russian) {
            quint16 unpaintedCells = m_robotView->unpaintedPoints();
            if (unpaintedCells) {
                int ends = unpaintedCells % 10;
                QString msg;
                if (ends == 1 && (unpaintedCells < 10 || unpaintedCells > 20) ) {
                    msg = QString::fromUtf8("Осталась незакрашена %1 клетка").arg(unpaintedCells);
                }
                else if ( (ends>=2 && ends<=4) &&
                          !(ends>=10 && ends<20) )
                {
                    msg = QString::fromUtf8("Остались незакрашены %1 клетки").arg(unpaintedCells);;
                }
                else {
                    msg = QString::fromUtf8("Осталось незакрашено %1 клеток").arg(unpaintedCells);
                }
                score.comment = msg;
            }
        }
        else { // english by default
            quint16 unpaintedCells = m_robotView->unpaintedPoints();
            if (unpaintedCells==1)
                score.comment = QString::fromAscii("%1 cell stills unpainted").arg(unpaintedCells);
            else
                score.comment = QString::fromAscii("%1 cells still unpainted").arg(unpaintedCells);
        }
    }
    else {
        score.value = 10;
    }
    return score;
}

void Plugin::connectSyncSignal(QObject *object, const char *method)
{
//    connect ( this, SIGNAL(signalSync()), object, method, Qt::DirectConnection);
    connect (m_robotView, SIGNAL(sync()), object, method);
}



PictomirPluginInterface::LoadFileError Plugin::loadDataFile(const QString &filename)
{
    RobotErrors::EnvironmentError err = m_robotView->loadEnvironment(filename);
    if (err==RobotErrors::FileNotExists)
        return PictomirPluginInterface::NotExists;
    else if (err==RobotErrors::FileAccessDenied)
        return PictomirPluginInterface::PermissionDenied;
    else if (err==RobotErrors::FileDamaged)
        return PictomirPluginInterface::FileDamaged;
    else
        return PictomirPluginInterface::NoError;

}

#ifndef Q_OS_WINCE
Q_EXPORT_PLUGIN2(isometricRobot,Plugin);
#endif

