#ifndef JSUTIL_H
#define JSUTIL_H

#include <QtCore>
#include <QtGui>
#include <QtScript>

class Directories :
        public QObject,
        public QScriptable
{
    Q_OBJECT;
    Q_PROPERTY(QString moduleRoot READ moduleRoot);
    Q_PROPERTY(QString applicationRoot READ applicationRoot);
public:
    explicit Directories(QObject *parent, const QString &moduleRoot);
public slots:
    QString moduleRoot() const;
    QString applicationRoot() const;
private:
    QString s_moduleRoot;
};

class FileReader :
        public QObject,
        public QScriptable
{
    Q_OBJECT;
public:
    explicit FileReader(QObject *parent);
public slots:
    QString readText(const QString &fileName, const QString &encoding) const;
};

class Logger :
        public QObject,
        public QScriptable
{
    Q_OBJECT;
public:
    explicit Logger(QObject *parent);
public slots:
    void writeToStdOut(const QString &string) const;
};

#endif // JSUTIL_H
