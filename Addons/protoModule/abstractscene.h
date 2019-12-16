#ifndef ABSTRACT_SCENE_H
#define ABSTRACT_SCENE_H

#include <QtCore>
#include <QtGui>
#include <QtScript>
#include <QtScriptTools>


struct CellViewInterface {};
class ScriptEnvironment;

struct CellItem
{
    // Состояние ячейки -- целое число
    // семантика трактуется реализацией
    // конкретного исполнителя
    QMap<QString,QVariant> cellAttribute;

    int topBorderState;
    int rightBorderState;
    int bottomBorderState;
    int leftBorderState;
    int frontBorderState;
    int backBorderState;

    // Аттрибуты отображения

    CellViewInterface *view;
    inline CellItem() { view = NULL; topBorderState=rightBorderState=bottomBorderState=leftBorderState=frontBorderState=backBorderState=0; }
};

struct Size
{
    quint16 width;
    quint16 height;
    quint16 length;
    inline Size() { width=length=height=0; }
    inline Size(quint16 w, quint16 l, quint16 h) { width=w;length=l;height=h; }
};

class AbstractScene :
        public QThread,
        public QScriptable
{
Q_OBJECT;
Q_PROPERTY(Size size READ size WRITE setSize);
Q_PROPERTY(quint16 width READ width);
Q_PROPERTY(quint16 height READ height);
Q_PROPERTY(quint16 length READ length);
Q_PROPERTY(QString clazz READ className);

public:
    explicit AbstractScene(QObject *parent,
                           const QString &moduleRoot,
                           const QString &sceneClass,
                           ScriptEnvironment *env);

signals:

public slots:
    virtual void reset();
    virtual quint8 check(const QString &script, bool wait);
    virtual void loadEnvironment(const QString &fileName);
    virtual void showDebugWindow();
    Size size() const;
    inline QString className() const { return s_className; }
    inline quint16 width() const { return size().width; }
    inline quint16 height() const { return size().height; }
    inline quint16 length() const { return size().length; }
    inline void setSize(quint16 w, quint16 l, quint16 h) { setSize(Size(w,l,h)); };
    void setSize(const Size &size);

    void setCellAttribute(quint16 x, quint16 y, quint16 z, const QString &name, const QVariant &value);
    QVariant cellAttribute(quint16 x, quint16 y, quint16 z, const QString &name) const;

    void setCellBorderState(quint16 x, quint16 y, quint16 z, const QString &borderName, int state);
    int cellBorderState(quint16 x, quint16 y, quint16 z, const QString &borderName) const;


protected:
    virtual void run();
    virtual void updateCellBorder(quint16 x, quint16 y, quint16 z, const QString &borderName) = 0;
    virtual void clearScene() = 0;
    virtual void initScene() = 0;

    QScriptEngine *m_scriptEngine;
    QScriptEngineDebugger *m_debugger;
    QString s_className;

    QMutex *mutex_script;
    QString s_script;
    QString s_func;
    QScriptValueList jsl_arguments;
    QScriptValue js_runResult;
    QString s_moduleRoot;

    QVector< QVector< QVector<CellItem> > > m_cells;

};

#endif
