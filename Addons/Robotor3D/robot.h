#ifndef ROBOT_H
#define ROBOT_H

#include <QtGui>
class QColor;

//properties
struct Point {
    bool wallLeft;
    bool wallRight;
    bool wallUp;
    bool wallDown;
    bool filled;
    bool dotted;

    QString text;

    double temp;
    double rad;
};

class Robot : public QWidget
{
    Q_OBJECT

    public:
        Robot(QWidget *parent = 0);
        ~Robot();

        enum TYPE {plane, torus, cyllinder, klein, mebius};
        TYPE type;


        // robot inner pos
	// used to draw 3D
        int x, y;
        int rx, ry;
        int nx, ny;
        int xstart, ystart, xlen, ylen;

        int len; //square length in pixels

        Point *field;
        Point *rfield; //field = rfield after reset


    public slots:

        // moving (in inner coords)!
        void moveUp();
        void moveDown();
        void moveLeft();
        void moveRight();

        // checks
	bool wallUp();
	bool wallDown();
	bool wallLeft();
	bool wallRight();
	bool isFilled();
	bool isClean();
	bool isDotted();

	bool noWallUp();
	bool noWallDown();
	bool noWallLeft();
	bool noWallRight();


	int rad();
	int temp();
	QString text();
	QString declineText() const;

        //actions
        void fill();
        void dot();
        void setText(const QString &val);
        void setTemp(int val);
        void setRad(int val);
        void setWallUp();
        void setWallDown();
        void setWallLeft();
        void setWallRight();

        // sys
        void setType    (TYPE newtype);
        void setSize    (int xn, int yn);
        void setViewPart(int xs, int ys, int xl, int yl);

        int saveFile(const QString &name);
        int openFile(const QString &name);
        void reset();

    protected:
        void paintEvent(QPaintEvent *event);
        void resizeEvent(QResizeEvent *event);
        void mousePressEvent(QMouseEvent *event);
        void mouseMoveEvent(QMouseEvent *event);

    signals:

        //robot can't do an action
        void decline(QString);
        void sendmsg(QString);
        void result(QString, QString);
        void result(QString, QString, QString);

        void update3D();

    private:

	QString tleft,
	   tright,
	   tup,
	   tdown,
	   tfill,
	   tunfill,
	   tfilled,
	   tnfilled,
	   twup,
	   twdown,
	   twright,
	   twleft,
	   tnwup,
	   tnwdown,
	   tnwright,
	   tnwleft,
	   ttemp,
	   ttext,
	   trad,
	   tsuc,
	   tdec,
	   tyes,
	   tno;


	//coordinate translation, (i, j) - screen
	//(p, q) - local, returns orientation
        int f(int i, int j, int *p, int *q);

        QPoint lastPos;

};

#endif // ROBOT_H
