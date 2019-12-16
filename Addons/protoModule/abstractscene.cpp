#include "abstractscene.h"
#include "util.h"
#include "scriptenvironment.h"

AbstractScene::AbstractScene(QObject *parent,
                             const QString &moduleRoot,
                             const QString &sceneClass,
                             ScriptEnvironment *env) :
    QThread(parent)
{
    s_moduleRoot = moduleRoot;
    s_className = sceneClass;
    m_scriptEngine = new QScriptEngine(this);
    QScriptValue obj = m_scriptEngine->newQObject(env);
    m_scriptEngine->globalObject().setProperty("G", obj);
    s_script = "";
    mutex_script = new QMutex;
    m_debugger = new QScriptEngineDebugger(this);
    m_debugger->attachTo(m_scriptEngine);
    m_debugger->standardWindow()->setWindowTitle("scene");
    m_debugger->setAutoShowStandardWindow(true);
}


void AbstractScene::setSize(const Size &size)
{
    clearScene();
    m_cells.clear();
    for (int z=0; z<size.height; z++) {
        QVector< QVector<CellItem> > layer;
        for (int y=0; y<size.length; y++) {
            QVector<CellItem> row(size.width, CellItem());
            layer.append(row);
        }
        m_cells.append(layer);
    }
    initScene();
}

void AbstractScene::setCellAttribute(quint16 x, quint16 y, quint16 z, const QString &name, const QVariant &value)
{
    if (z>=m_cells.size() || y>=m_cells[0].size() || x>=m_cells[0][0].size()) {
        qDebug() << "setCellAttribute: index " << x << ", " << y << ", " << z << " out of scene, ignoring action!";
        return;
    }
    Q_ASSERT ( m_cells.size() > 0 );

    m_cells[z][y][x].cellAttribute[name] = value;
}

QVariant AbstractScene::cellAttribute(quint16 x, quint16 y, quint16 z, const QString &attributeName) const
{
    if (z>=m_cells.size() || y>=m_cells[0].size() || x>=m_cells[0][0].size()) {
        qDebug() << "cellAttribute: index " << x << ", " << y << ", " << z << " out of scene, ignoring action!";
        return QVariant();
    }
    Q_ASSERT ( m_cells.size() > 0 );

    if (m_cells[z][y][x].cellAttribute.contains(attributeName))
        return m_cells[z][y][x].cellAttribute[attributeName];
    else {
        qDebug() << "cell has no attribute: " << attributeName;
        return QVariant();
    }
}

void AbstractScene::setCellBorderState(quint16 x, quint16 y, quint16 z, const QString &borderName, int state)
{
    if (z>=m_cells.size() || y>=m_cells[0].size() || x>=m_cells[0][0].size()) {
        qDebug() << "setCellBorderState: index " << x << ", " << y << ", " << z << " out of scene, ignoring action!";
        return;
    }
    Q_ASSERT ( m_cells.size() > 0 );

    const QString border = borderName.trimmed().toLower();
    if (border=="front") {
        m_cells[z][y][x].frontBorderState = state;
        updateCellBorder(x,y,z,"front");
    }
    else if (border=="right") {
        m_cells[z][y][x].rightBorderState = state;
        updateCellBorder(x,y,z,"right");
    }
    else if (border=="back") {
        m_cells[z][y][x].backBorderState = state;
        updateCellBorder(x,y,z,"back");
    }
    else if (border=="left") {
        m_cells[z][y][x].leftBorderState = state;
        updateCellBorder(x,y,z,"left");
    }
    else if (border=="top") {
        m_cells[z][y][x].topBorderState = state;
        updateCellBorder(x,y,z,"top");
    }
    else if (border=="bottom") {
        m_cells[z][y][x].bottomBorderState = state;
        updateCellBorder(x,y,z,"bottom");
    }
    QApplication::processEvents();
}

int AbstractScene::cellBorderState(quint16 x, quint16 y, quint16 z, const QString &borderName) const
{
    if (z>=m_cells.size() || y>=m_cells[0].size() || x>=m_cells[0][0].size()) {
        qDebug() << "cellBorderState: index " << x << ", " << y << ", " << z << " out of scene, ignoring action!";
        return 0;
    }
    Q_ASSERT ( m_cells.size() > 0 );

    const QString border = borderName.trimmed().toLower();
    if (border=="top")
        return m_cells[z][y][x].topBorderState;
    else if (border=="right")
        return m_cells[z][y][x].rightBorderState;
    else if (border=="bottom")
        return m_cells[z][y][x].bottomBorderState;
    else if (border=="left")
        return m_cells[z][y][x].leftBorderState;
    else if (border=="front")
        return m_cells[z][y][x].frontBorderState;
    else if (border=="back")
        return m_cells[z][y][x].backBorderState;
    else
        return 0;
}

Size AbstractScene::size() const
{
    int w=0;
    int l=0;
    int h=0;

    h = m_cells.size();

    if (m_cells.size()) {
        // есть хотя бы один слой клеток
        l = m_cells[0].size();

        if (m_cells[0].size()) {
            // в слое есть хотя бы одна строка клеток
            w = m_cells[0][0].size();
        }
    }

    return Size(w,l,h);
}

void AbstractScene::run()
{
    mutex_script->lock();
    QScriptValue scene = m_scriptEngine->newQObject(this);
    QScriptValue func = m_scriptEngine->globalObject().property(s_func);
    QString script = s_script;
    bool simple = s_func.isEmpty();
    mutex_script->unlock();
    if (simple) {
        js_runResult = m_scriptEngine->evaluate(script);
    }
    else {
        js_runResult = func.call(scene, jsl_arguments);
    }
    if (js_runResult.isError()) {
        qDebug() << "AbstractScene:run: " << js_runResult.toString();
    }
    while (m_scriptEngine->isEvaluating()) {
        msleep(20);
    }
}

quint8 AbstractScene::check(const QString &script, bool mustWait)
{
    mutex_script->lock();
    jsl_arguments.clear();
    if (script.isEmpty()) {
        s_script = "";
        s_func = "check";
    }
    else {
        s_func = "";
        s_script = script;
    }
    mutex_script->unlock();
    start();
    if (mustWait)
        wait();
    if (js_runResult.isNumber()) {
        return (quint8)js_runResult.toInteger();
    }
    else {
        return 0;
    }
}



void AbstractScene::reset()
{
    mutex_script->lock();
    jsl_arguments.clear();
    s_func = "reloadEnvironment";
    s_script = "";
    mutex_script->unlock();
    start();
    wait();
}

void AbstractScene::loadEnvironment(const QString &fileName)
{
    mutex_script->lock();
    jsl_arguments.clear();
    jsl_arguments.append(QScriptValue(fileName));
    s_func = "loadEnvironment";
    s_script = "";
    mutex_script->unlock();
    start();
    wait();
}


void AbstractScene::showDebugWindow()
{
    m_debugger->standardWindow()->show();
}
