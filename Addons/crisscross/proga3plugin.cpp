#include "proga3plugin.h"
#include "mainwindow.h"

struct Proga3PluginPrivate {
    QVariant lastResult;
    QList<QVariant> optResults;
    QString lasterror;

};

proga3Plugin::proga3Plugin(QObject *parent) :
    QObject(parent)
{
    d = new Proga3PluginPrivate;
    m_mainWidget = NULL;
}

QList<Alg> proga3Plugin::algList()
{
    QList<Alg> l;
    Alg a;

    a.kumirHeader = QString::fromUtf8("алг начать игру(цел размер поля, цел интеллект, лог кумир начинает)");
    l << a;
    a.kumirHeader = QString::fromUtf8("алг передать свой ход(цел X, Y)");
    l << a;
//    a.kumirHeader = QString::fromUtf8("алг сделать ход");
//    l << a;
    a.kumirHeader = QString::fromUtf8("алг цел последний ход исполнителя X");
    l << a;
    a.kumirHeader = QString::fromUtf8("алг цел последний ход исполнителя Y");
    l << a;
    a.kumirHeader = QString::fromUtf8("алг лог игра закончена");
    l << a;
    a.kumirHeader = QString::fromUtf8("алг цел результат игры");
    l << a;
    a.kumirHeader = QString::fromUtf8("алг подсказка (цел уровень)");
    l << a;
    a.kumirHeader = QString::fromUtf8("алг цел подсказка X");
    l << a;
    a.kumirHeader = QString::fromUtf8("алг цел подсказка Y");
    l << a;

    return l;
}

QString proga3Plugin::name()
{
    return QString::fromUtf8("Крестики_Нолики");
}

QList<QVariant> proga3Plugin::algOptResults()
{
    return d->optResults;
}

QVariant proga3Plugin::result()
{
    QVariant v = d->lastResult;
    return v;
}

void proga3Plugin::runAlg(QString alg, QList<QVariant> params)
{
    d->lasterror = "";
    d->lastResult = QVariant();
    mainWidget();
    if (alg == QString::fromUtf8("начать игру")) {
        m_mainWidget->setSize(params[0].toInt());
        if (params[1].toInt() == 0) m_mainWidget->setHard(); else
                                    m_mainWidget->setSimple();
        m_mainWidget->setGamer(params[2].toBool());
        m_mainWidget->setIgnore(false);
    } else if (alg == QString::fromUtf8("передать свой ход")) {
        m_mainWidget->defend(params[1].toInt(), params[0].toInt());
    } else if (alg == QString::fromUtf8("цел последний ход исполнителя X")) {
        d->lastResult = m_mainWidget->lastX();
    } else if (alg == QString::fromUtf8("цел последний ход исполнителя Y")) {
        d->lastResult = m_mainWidget->lastY();
    } else if (alg == QString::fromUtf8("лог игра закончена")) {
        d->lastResult = m_mainWidget->checkIgnore();
    } else if (alg == QString::fromUtf8("цел результат игры")) {
        d->lastResult = m_mainWidget->whoWon();
    } else if (alg == QString::fromUtf8("подсказка")) {
        TPoint p = m_mainWidget->helpHim();
        d->optResults.clear();
        d->optResults << p.x << p.y;
    }
    else if (alg == QString::fromUtf8("подсказка X")) {
        d->lastResult = d->optResults[0];
    }
    else if (alg == QString::fromUtf8("подсказка Y")) {
        d->lastResult = d->optResults[1];
    }
    emit sync();
}

void proga3Plugin::showField()
{
    mainWidget()->show();
}

void proga3Plugin::hideField()
{
    mainWidget()->hide();
}

void proga3Plugin::setMode(int mode)
{

}

void proga3Plugin::reset()
{
    d->lasterror = "";
    d->lastResult = QVariant();
}

QString proga3Plugin::errorText() const
{
    return d->lasterror;
}

void proga3Plugin::connectSignalSendText(const QObject *obj, const char *method)
{
    connect(this, SIGNAL(sendText(QString)), obj, method);
}

void proga3Plugin::connectSignalSync(const QObject *obj, const char *method)
{
    connect(this, SIGNAL(sync()), obj, method);
}

QString proga3Plugin::help()
{
    return "No HELP!!!";
}

void proga3Plugin::handleResetRequest()
{
    reset();
}

void proga3Plugin::handleLoadImageRequest(const QString &fileName)
{

}

void proga3Plugin::handleNewImageRequest(int w, int h, const QString &bg)
{

}

QWidget *proga3Plugin::mainWidget()
{
    if (!m_mainWidget) {
        m_mainWidget = new CrissCrossWindow();
    }
    return m_mainWidget;
}

Q_EXPORT_PLUGIN2(crisscross, proga3Plugin)
