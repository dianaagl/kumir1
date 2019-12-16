#ifndef DWUNOG_H
#define DWUNOG_H

#include <QtGui>
#include <QObject>
class DwunogWorld;
class QTimer;

struct Command {
    QString cmd;
    int arg;
};

class Dwunog: public QObject, public QGraphicsItem{

    Q_OBJECT

    public:
        Dwunog ():id(-1),donecmd(0),cmdk1(0){}
        Dwunog (int id):id(id),donecmd(0),cmdk1(0){}
        ~Dwunog();

        void setParameters(int lfang, int rfang, int nang, int slen,
                    QPointF cpos, QList <QLineF> wrld, int fuelm);

        enum { Type = UserType + 3 };
        int type() const { return Type; }

      //  int headHeight()    const { return (int) mapToScene(hPos()).y(); }
//        int lFootHeight()   const { return (int) mapToScene(lPos()).y(); }
  //      int rFootHeight()   const { return (int) mapToScene(lPos()).y(); }
    //    int centerHeight()  const { return (int) mapToScene(cPos()).y(); }
        int hAngle()    const { return hang % 360; }
        int lAngle()    const { return lang % 360; }
        int rAngle()    const { return rang % 360; }

        QPainterPath shape()    const;
        QRectF boundingRect()   const;
        bool collidesWithItem(const QGraphicsItem * other,
                              Qt::ItemSelectionMode = Qt::IntersectsItemShape) const;
    signals:
        void sync();
        void decline(QString);
        void collision();
        void fuelchange(int);

    public slots:
        void reset();
        void setID      (int newid);
        void doCommand  (QString cmd, int arg = 0);

    private slots:
        void hturn();
        void lfturn();
        void rfturn();
        void lfwait();
        void rfwait();
        void saveToFile(const QString &name);

    protected:
        void paint(QPainter *painter,
                   const QStyleOptionGraphicsItem *option,
                   QWidget *widget);

    private:

        enum Actions {none = 0, lfoot = 1, rfoot = 2, head = 3,
                      waitl = 4, waitr = 5, k1 = 6};

        bool lfCollides();
        bool rfCollides();
        bool hCollides();

        QPointF hPos()          const;
        QPointF foot(int ang)   const;
        bool isStand(int ind)   const;

        void finish     (QString message = "");
        void setAction  (Actions action, int arg);
        void addToList  (QList <Command> *list);


        //parameters
        int id;
        int lang, rang, hang;
        int flen, nlen, hrad;
        bool lstand, rstand, cstand;
        QPointF lpos, rpos, cpos;

        int fuelmax;
        int fuel;

        //current action
        bool stop;
        int angle, count, sign, speed;
        Actions action;

        //k1
        int indexk1;
        bool savek1, savehead;

        //starting params for reset
        int dl, dr, dh;
        QPointF dpos;

        QTimer timer;
        QList <QLineF> world;
        QList <Command> *donecmd;
        QList <Command> *cmdk1;
};

#endif
