#include <QtGui/QApplication>
#include "robot.h"
#include "pult.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(icons);

    QApplication a(argc, argv);
    Robot *w = new Robot();
    Pult  *p = new Pult (w);

    QObject::connect(w, SIGNAL(decline(QString)), p, SLOT(setLabelText(QString)));
    QObject::connect(w, SIGNAL(sendmsg(QString)), p, SLOT(setLabelText(QString)));
    QObject::connect(w, SIGNAL(destroyed()), p, SLOT(deleteLater()));
    QObject::connect(p, SIGNAL(destroyed()), w, SLOT(deleteLater()));

    w->show();
    p->show();
    return a.exec();
}
