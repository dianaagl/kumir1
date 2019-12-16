#include <QtCore>
#include <QtGui>

#include "robotview.h"
#include "plugin.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Plugin *plugin = new Plugin();
    QWidget *w = plugin->mainWidget();
    w->show();
    return app.exec();
}

