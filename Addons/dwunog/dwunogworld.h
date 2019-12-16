#ifndef DWUNOGWORLD_H
#define DWUNOGWORLD_H

#include <QtGui>
#include "dwunog.h"
#include "plugin.h"

#include <QPolygonF>
class Plugin;
class Dwunog;
class Apple;

class DwunogWorld : public QGraphicsView {
    Q_OBJECT

    public:
        DwunogWorld(Plugin *plugin);
        ~DwunogWorld();

    signals:
        int error();
        int sendmsg1(const QString&);
        int sendmsg2(const QString&);

    public slots:
        int  openf(const QString &path);

        void reset();
        void gameWon();
        void msg1(QString text) {emit sendmsg1(text);}
        void msg2(QString text) {emit sendmsg2(text);}
        void doCommand(const QString& command, int arg = 0);
        void fuelchange(int newval);


    private:
        int hang();
        int lfang();
        int rfang();

        int argto5(int arg);

        int dwid;
        int nx, ny, slen;
        int dx, dy, dl, dr, dh;
        QList <QLineF> world;

        QString levelName;


        Apple *apple;
        Dwunog *dwunog;
        Plugin *plugin;
        QGraphicsTextItem *fuelbar;
        int fuelmax;
        QDir *dir;

};


class Apple:public QGraphicsItem
{
    public:
        Apple (int r, QPointF p):r(r){ setPos(p);}

        enum {Type = UserType + 4};
        int type() const { return Type; }

        QPainterPath shape() const
        { QPainterPath shape; shape.addEllipse(QPointF(0, 0), r, r); return shape;}

        void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
        {
            painter->setBrush(Qt::darkGreen);
            painter->drawEllipse(QPointF(0, 0), r, r);
            painter->drawLine(QPointF(0, -r), QPointF(0, -2*r));
            painter->setBrush(Qt::darkYellow);
            painter->drawChord(QRectF(-r, -3*r, 2*r, 2*r), -16*90, 16*90);
            painter->drawChord(QRectF(0, -2*r, 2*r, 2*r), 16*90, 16*90);
        }
        QRectF boundingRect() const {return QRectF(-r, r, 2*r, 2*r);}
    private:
        int r;


};
#endif
