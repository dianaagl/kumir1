#ifndef PICTOMIR_PLUGIN_INTERFACE
#define PICTOMIR_PLUGIN_INTERFACE

#include <QtCore>
#include <QtGui>


class PictomirPluginInterface
{
public:
    struct Command {
        int id;
        QString name;
        QString tooltip;
        QMap<QLocale::Language, QString> name_i18n;
        QMap<QLocale::Language, QString> tooltip_i18n;
        QString shortcut;
        QByteArray svgImageData;
    };

    struct TaskScore {
        quint8 value;
        QString comment;
    };

    enum LoadFileError {
        NoError,
        NotExists,
        PermissionDenied,
        FileDamaged
    };

    enum TargetDevice {
        Destkop,
        Netbook,
        TabletPC,
        MID,
        Smartphone
    };

    /** Имя исполнителя
      @param language язык, на котором назван исполнитель
    */
    virtual QString name(const QLocale::Language language = QLocale::C) const = 0;

    /** Список команд-условий */
    virtual QList<Command> conditions() const = 0;

    /** Список команд-действий */
    virtual QList<Command> actions() const = 0;



    /** Инициализация исполнителя */
    virtual void init() = 0;

    /** Загрузка файла данных исполнителя */
    virtual LoadFileError loadDataFile(const QString &filename) = 0;

    /** Возвращение к исходному состоянию */
    virtual void reset() = 0;

    /** Выполнение команды в режиме анимации
      НЕ БЛОКИРУЕТ выполнение потока и ПОСЫЛАЕТ сигнал о завершении
      @param id уникальный идентификатор команды (см. @struct Command)
    */
    virtual void evaluateCommandAnimated(int id) = 0;

    /** Выполнение команды в быстром режиме
      БЛОКИРУЕТ выполнение потока и сигнал о завершении НЕ ПОСЫЛАЕТ
      @param id уникальный идентификатор команды (см. @struct Command)
    */
    virtual void evaluateCommandHidden(int id) = 0;

    /** Текст сообщения об ошибке выполения последней команды
      @param language язык, на котором выдать сообщение
    */
    virtual QString lastError(const QLocale::Language language = QLocale::C) const = 0;

    /** Результат выполнения последней команды */
    virtual QVariant lastResult() const = 0;

    /** Завершение выполнения программы */
    virtual void finishEvaluation() = 0;

    /** Проверка результата */
    virtual TaskScore checkEvaluationResult(const QLocale::Language language = QLocale::C) const = 0;



    /** Главный виджет исполнителя */
    virtual QWidget* mainWidget(TargetDevice) = 0;

    /** Пульт исполнителя */
    virtual QWidget* controlWidget() = 0;

    /** Меню исполнителя */
    virtual QList<QMenu*> menuBar() = 0;

    /** Соединяет сигнал "Выполнение комады завершено" исполнителя
      @param object имя объекта
      @param method имя слота
    */
    virtual void connectSyncSignal(QObject *object, const char *method) = 0;


};

Q_DECLARE_INTERFACE (PictomirPluginInterface, "pictomir.pluginInterface/0.9.0" );
#endif
