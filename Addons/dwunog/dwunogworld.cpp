#include <QApplication>
#include <QDesktopWidget>
#include <math.h>
#include <stdio.h>
#include <fstream>
using namespace std;

#include "dwunog.h"
#include "dwunogworld.h"

DwunogWorld::DwunogWorld(Plugin *plugin):
        plugin(plugin)
{
    this -> setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    this -> setWindowTitle(QString::fromUtf8("Мир Двунога"));

    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->setSceneRect(QRectF(0, 0, 1, 1));
    setScene(scene);
}

void DwunogWorld::gameWon()
{
    if (dwunog->collidesWithItem((QGraphicsItem *)apple)) {
        QString path = "";
        QStringList l = dir->entryList(QDir::Files|QDir::Readable);
        int ind = l.indexOf(levelName);
        if ((ind != -1) && (ind != l.size() - 1))
            path = dir->absoluteFilePath(l.at(ind + 1));


        QMessageBox msg(this);
        msg.setText(QString::fromUtf8("Задание выполнено!"));
        msg.setInformativeText(QString::fromUtf8("Перейти на следующий уровень?"));
        msg.setDetailedText(QString::fromUtf8("Следующий уровень: ") + path);
        msg.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msg.setDefaultButton(QMessageBox::Ok);
        int res = msg.exec();
        if (res == QMessageBox::Ok) {
                openf(dir->absoluteFilePath(l.at(ind + 1)));
                return;
        }
        else
            dwunog->reset();

        //        msg1("Task completed!");
    }
}

int DwunogWorld::argto5(int arg)
{
    if (arg == 0) return 0;
    int a = arg % 5;
    if (abs (a) >= 3)   return arg + 5 - abs(a);
    else return arg - a;
}

void DwunogWorld::doCommand(const QString& cmd, int arg)
{
    if (cmd == "reset")
        reset();
    else dwunog->doCommand(cmd, arg);
}

void DwunogWorld::reset()
{
    if (!dwunog) return;
    dwunog->reset();
}

void DwunogWorld::fuelchange(int newval)
{
    if (newval > 0)
        fuelbar->setPlainText(QString::fromUtf8("Топливо: ") + QString::number(newval)
                     + "/" + QString::number(fuelmax));
    else
        fuelbar->setPlainText(QString::fromUtf8("Топливо закончилось!"));
}


int DwunogWorld::openf(const QString &path)
{
    scene()->clear();
    world.clear();

    QPointF p, p1, p2;
    QGraphicsScene *scene = new QGraphicsScene(this);
    int i, n, m, x, y;
    slen = QApplication::desktop()->width() / 100;
    FILE *f;
    f = fopen(qPrintable(path), "r");


    if (f == 0) {
        emit sendmsg1("Can't open file " + path + "!");
        return 1;
    }

    setScene(scene);


    int k = 0;
    char c;
    char s[256];
    do {
        fscanf(f, "%c", &c);
        if (c != '\n') s[k] = c;
        k++;
    }
    while (c != '\n');

    if (fscanf(f, "%d\n", &i) != 1) {
        emit sendmsg1("Can't open file " + path + "!");
        fclose(f);
        return 1;
    }
    fuelmax = i;

    if (fscanf(f, "%d %d %d\n", &nx, &ny, &n) != 3) {
        emit sendmsg1("Can't open file " + path + "!");
        fclose(f);
        return 1;
    }

    scene->setSceneRect(QRectF(0, -slen, nx*slen, ny*slen));
    scene->setBackgroundBrush(Qt::black);

    if (fscanf(f, "%d %d\n", &x, &y) != 2) {
        emit sendmsg1("Can't open file " + path + "!");
        fclose(f);
        return 1;
    }

    QPolygonF poly;
    p = QPointF(x*slen, y*slen);
    p1 = p;
    poly << p;
    for (i = 1; i < n; i++) {
        if (fscanf(f, "%d %d\n", &x, &y) != 2) {
            emit sendmsg1("Can't open file " + path + "!");
            fclose(f);
            return 1;
        }
        p2 = QPointF(x*slen, y*slen);
        scene->addLine(QLineF(p1, p2));
        world.append(QLineF(p1, p2));
        p1 = p2;
        poly << p2;
    }
    world.append(QLineF(p2, p));
    scene->addLine(QLineF(p2, p));
    scene->addPolygon(poly, QPen(Qt::black), QBrush(Qt::white));

    QGraphicsTextItem *text = new QGraphicsTextItem(QString::fromUtf8("Задание: ")
                                                    + QString::fromUtf8(s));
    text->setDefaultTextColor(Qt::white);
    text->setPos(slen, -slen);
    scene->addItem(text);
    fuelbar = new QGraphicsTextItem(QString::fromUtf8("Топливо: ") + QString::number(fuelmax)
                     + "/" + QString::number(fuelmax));
    fuelbar->setDefaultTextColor(Qt::white);
    fuelbar->setPos((nx-11)*slen, -slen);
    scene->addItem(fuelbar);


    if (fscanf(f, "%d %d", &x, &y) != 2) {
        emit sendmsg1("Can't open file " + path + "!");
        fclose(f);
        return 1;
    }
    apple = new Apple(10, QPointF(x*slen, y*slen));

    if (fscanf(f, "%d\n", &m) != 1) {
        emit msg1("Can't open file " + path + "!");
        fclose(f);
        return 1;
    }


    dwunog = new Dwunog(0);
    if (fscanf(f, "%d %d %d %d %d\n", &dx, &dy, &dl, &dr, &dh) != 5) {
        emit msg1("Can't open file " + path + "!");
        fclose(f);
        return 1;
    }
    dwunog->setParameters(dl, dr, dh, slen, QPointF(dx*slen, dy*slen), world, fuelmax);

    connect(dwunog, SIGNAL(sync()),             plugin, SLOT(emitSync()));
    connect(dwunog, SIGNAL(collision()),        this,   SLOT(gameWon()));
    connect(dwunog, SIGNAL(decline(QString)),   this,   SLOT(msg1(QString)));
    connect(dwunog, SIGNAL(fuelchange(int)),   this,   SLOT(fuelchange(int)));

    fclose(f);

    scene->addItem(dwunog);
    scene->addItem(apple);

    dir = new QDir (QDir::currentPath() + "/addons/dwunogmaps/");
    QStringList l = path.split('/');
    levelName = l.last();

    this->setFixedSize(nx*slen+10, ny*slen+10);

//    gameWon();
    return 0;
}

DwunogWorld::~DwunogWorld()
{
    qDebug() <<"World destructor";
    if (apple != 0) delete apple;
    if (dwunog != 0) delete dwunog;
    if (fuelbar != 0) delete fuelbar;
}
