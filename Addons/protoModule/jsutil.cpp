#include "jsutil.h"
#include "util.h"

#include <cstdio>

Directories::Directories(QObject *parent, const QString &moduleRoot):
        QObject(parent),
        s_moduleRoot(moduleRoot)
{

}

QString Directories::moduleRoot() const
{
    return s_moduleRoot;
}

QString Directories::applicationRoot() const
{
    return QApplication::applicationDirPath();
}

FileReader::FileReader(QObject *parent): QObject(parent) {}


QString FileReader::readText(const QString &fileName, const QString &encoding) const
{
    QStringList parts = fileName.split("/");
    QString path;
    QString moduleRoot;
    for (int i=0; i<parts.count(); i++) {
        path += parts[i];
        QFileInfo fi(path);
        if (i!=parts.count()-1 &&
            (path.endsWith(".proto.jar") || path.endsWith(".proto.zip") || path.endsWith(".proto"))) {
            moduleRoot = path;
            path = fileName.mid(moduleRoot.length()+1);
            break;
        }
        if (i<parts.count()-1) {
            path += "/";
        }
    }

    QString result;

    QByteArray rawData = getFileData(moduleRoot, path);

    QTextStream ts(rawData);
    ts.setCodec(encoding.toAscii().data());
    result = ts.readAll();


    return result;
}

Logger::Logger(QObject *parent) :
        QObject(parent)
{

}

void Logger::writeToStdOut(const QString &string) const
{
    qDebug() << "JS: " << string.toLocal8Bit().data();
}
