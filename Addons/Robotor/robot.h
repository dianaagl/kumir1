/* ------------------------------------------------ */
/* Robot: [0..nx, 0..ny] - field size;              */
/* x, xstart E[0..nx-1]; y, ystart E[0..ny-1] ;     */
/* int f(i, j, p, q) translates screen square(i, j) */
/* to (p, q) in local coords, returns orientation,  */
/* where p, q E[0..nx-1, 0..ny-1]                   */
/* ------------------------------------------------ */


#ifndef ROBOT_H
#define ROBOT_H

#include <QtGui>
class QColor;

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

        enum TYPE {torus, klein};

    public slots:
        void moveUp();
        void moveDown();
        void moveLeft();
        void moveRight();

        bool wallUp()       const;
        bool wallDown()     const;
        bool wallLeft()     const;
        bool wallRight()    const;
        bool isFilled()     const;
        bool isDotted()     const;

        int rad()           const;
        int temp()          const;
        QString text()      const;

        void fill();
        void dot();
        void setText(const QString &val);
        void setTemp(int val);
        void setRad(int val);
        void setWallUp();
        void setWallDown();
        void setWallLeft();
        void setWallRight();

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
        void decline(QString);
        void sendmsg(QString);

    private:

        TYPE type;

        int f(int i, int j, int *p, int *q);


        int x, y;
        int rx, ry;
        int nx, ny;
        int xstart, ystart, xlen, ylen;
        int len;
        int side;

        Point *field;
        Point *rfield;

        QPoint lastPos;

};

#endif // ROBOT_H
