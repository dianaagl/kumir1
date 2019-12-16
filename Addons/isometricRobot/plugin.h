#ifndef PLUGIN_H
#define PLUGIN_H

#include <QtCore>
#include "pictomirplugininterface.h"
#include "roboterrors.h"
#include <QtGui>

class RobotView;

class Plugin :
        public QObject,
        public PictomirPluginInterface
{
    Q_OBJECT;
    Q_INTERFACES(PictomirPluginInterface);
public:
    explicit Plugin();

    QString name(const QLocale::Language language = QLocale::C) const;
    QList<Command> conditions() const;
    QList<Command> actions() const;


    void init();
    LoadFileError loadDataFile(const QString &filename);
    void reset();
    void evaluateCommandAnimated(int id);
    void evaluateCommandHidden(int id);
    QVariant lastResult() const;
    QString lastError(const QLocale::Language language = QLocale::C) const;
    void finishEvaluation();
    TaskScore checkEvaluationResult(const QLocale::Language language = QLocale::C) const;


    QWidget* mainWidget(TargetDevice device);
    QWidget* controlWidget();
    QList<QMenu*> menuBar();
    void connectSyncSignal(QObject *object, const char *method);
    static QString moduleImagesRoot();


public slots:
    void loadEnvironmentClicked();
    void loadDefaultEnvironment();

private:
    void createMenuBar();
    void createMainWidget(TargetDevice device);
    void evaluateCommand(int id, bool animate);

    RobotView *m_robotView;
    bool m_lastResult;
    QMap<QString,QByteArray> m_icons;
    int m_stepsToDone;

    RobotErrors::RuntimeError e_lastError;



    QAction *a_actionLoadEnvironment;
    QAction *a_actionDefaultEnvironment;
    QList<QMenu*> m_menuBar;

signals:
    void signalSync();
};

#endif // PLUGIN_H
