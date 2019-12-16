#include <QtCore>
#include "kumircommoninterface.h"

int main(int argc, char *argv[])
{
    QCoreApplication *app = new QCoreApplication(argc, argv);
    QPluginLoader *loader = new QPluginLoader(QCoreApplication::applicationDirPath()+"/libkumir.so");
    if ( !loader->load() ) {
        qFatal(loader->errorString().toLocal8Bit());
    }
    QObject *obj = loader->instance();
    KumirCommonInterface *kumir = dynamic_cast<KumirCommonInterface*>(obj);
    Q_CHECK_PTR(kumir);
    return app->exec();
}
