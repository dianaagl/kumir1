#ifndef PROTOMODULE_INTERFACE_H
#define PROTOMODULE_INTERFACE_H

#include <QtCore>
#include <QtGui>

class kumirPluginInterface;


/** @interface ProtoModuleInterface
  * Интерфейс для загрузки протоисполнителей
  */
class ProtoModuleInterface
{
public:
    /** Устанавливает корневой каталог структуры протоисполнителей и загружает их.
      * @param path абсолютный путь к каталогу исполнителей
      */
    virtual void setRootDirectory(const QString &path) = 0;

    /** Возвращает список виртуальных исполнителей после их загрузки (см. setRootDirectory)
      * @returns список имён виртуальных исполнителей, либо пустой список,
      * если протоисполнитель не загружен
      */
    virtual QStringList virtualModulesList() const = 0;

    /** Создаёт экземпляр одного виртуального исполнителя
      * @param virtualModuleName имя виртуального исполнителя
      * @returns Кумир-исполнитель, либо NULL, если указано неверное имя
      */
    virtual QObject* createModule(const QString &virtualModuleName) = 0;

    /** Возвращает краткое виртуального исполнителя
      * @param virtualModuleName имя виртуального исполнителя
      * @returns описание виртуального исполнителя, либо пустая строка в случаях:
      * 1) неправильное имя виртуального исполнителя; 2) описание отсутствует.
      */
    virtual QString moduleDescription(const QString &virtualModuleName) const = 0;

    /** Возвращает URL к файлу (например index.html или modulename.pdf),
      * содержащему подробное описание конкретного виртуального исполнителя
      * @param virtualModuleName имя виртуального исполнителя
      * @returns URL справки виртуального исполнителя, либо invalid URL в случаях:
      * 1) неправильное имя виртуального исполнителя; 2) ссылка отсутствует.
      */
    virtual QUrl moduleHelpEntryPoint(const QString &virtualModuleName) const = 0;

};

Q_DECLARE_INTERFACE (ProtoModuleInterface, "kumir.ProtoModuleInterface/1.7.90" );

#endif // PROTOMODULE_INTERFACE_H
