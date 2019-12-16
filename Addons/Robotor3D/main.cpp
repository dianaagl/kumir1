#include <QtGui/QApplication>

#include "robot.h"
#include "pult.h"
#include "window.h"


int main(int argc, char *argv[])
{

    QApplication app(argc, argv);
    Robot *w = new Robot();
    Pult  *p = new Pult (w);
    Window  *globj = new Window(w);

    QObject::connect(w, SIGNAL(decline(QString)), p, SLOT(setLabelText(QString)));
    QObject::connect(w, SIGNAL(sendmsg(QString)), p, SLOT(setLabelText(QString)));
    QObject::connect(w, SIGNAL(destroyed()), p, SLOT(deleteLater()));
    QObject::connect(p, SIGNAL(destroyed()), w, SLOT(deleteLater()));

    QObject::connect(w, SIGNAL(update3D()), globj, SLOT(updateWidget()));

    w->show();
    p->show();
    globj->show();

    return app.exec();
}
