#ifndef SINGLEPROTOMODULE_H
#define SINGLEPROTOMODULE_H

#include <QtCore>
#include <QtGui>
#include <QtScript>
#include <QtScriptTools>
#include "plugin_interface.h"
#include "virtualobjects.h"

class View25D;
class ScriptEnvironment;

class SingleProtoModule :
        public QObject,
        public kumirPluginInterface
{
    Q_OBJECT;
    Q_INTERFACES(kumirPluginInterface);
public:
    explicit SingleProtoModule(const QString &rootDir,
                               const QString &name,
                               const VirtualModule &module);

    void start(); //Запуск исполнителя
    QList<Alg> algList(); //Список алгоритмов исполнителя
    QString name();
    QList<QVariant> algOptResults(); //резы
    QVariant     result();//возвращаемое значение
    void	      runAlg(QString alg,QList<QVariant> params); //Запуск алгоритма
    void        showField(); //Показать поле
    void        showPult();//Показать пульт
    void        hidePult();
    void        hideField();
    void        showOpt();
    bool        hasPult(); //Есть ли пульт
    bool        hasOpt(); //Есть ли дополнительное окно
    QString     optText();//Текст для дополнительного окна
    void 	      setMode(int mode); //Установить режим
    inline QWidget*    pultWidget() {return NULL;}
    void reset();
    quint8  check(const QString &script = QString());
    QString errorText() const;
    void setParameter(const QString &paramName, const QVariant &paramValue);
    /**
      Присоединяет слот @param method объекта @param obj к сигналу "Отправить текст"
      Пример использования:
        plugin->connectSignalSendText(kumir, SLOT(slotName(QString))
      */
    void connectSignalSendText( const QObject *obj, const char *method );
    /**
      Присоединяет слот @param method объекта @param obj к сигналу "Выполнение команды завершено"
      Пример использования:
        plugin->connectSignalSendSync(kumir, SLOT(slotName(bool))
      */
    void connectSignalSync( const QObject *obj, const char *method);
    QString     help(); //Помощь
    QWidget* mainWidget();


private:
    QString s_rootDir;
    QString s_name;
    VirtualModule m_module;
    View25D *m_25dView;
    QVariant v_lastResult;
    QString s_lastErrorText;
    int debug_algCounter;
    int debug_syncCounter;
    quint8 i_checkResult;

    ScriptEnvironment *m_environment;
    QMainWindow *w_window;




private slots:
    void handleExecutionFinished(const QString &error, const QScriptValue &ret);
    void handleLoadEnvironmentAction();

signals:
    void sync(bool status);

};

#endif // SINGLEPROTOMODULE_H
