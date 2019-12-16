#ifndef FIELDGRAPH_H
#define FIELDGRAPH_H

#include <QWidget>
#include <mainwindow.h>


class FieldGraph : public QWidget
{
    Q_OBJECT
public:
    explicit FieldGraph(QWidget *parent = 0);
    int size;
    bool ignore;
    bool gamer;
    int power(int a, int i);
    void attack(int x, int y);
    void defend(int x, int y);
    int whoWon();
    int lastX();
    int lastY();

    void keyPressEvent(QKeyEvent *event);
protected:
    bool winner;
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *event);
    int go(int destination, int x, int y);
    int can(int destination, int x, int y);
    bool check(int i);
    void checker();
    TPoint analyze();
    TPoint analyze2();
    char field[100][100];
    TPoint history[1000];
    bool cheat;
    int count;
    int level;
    TPoint coord;
    QPixmap pxNull;
    QPixmap pxCross;
    QPixmap pxBackGround;
    QPixmap pxCurrent;
    QPixmap pxcross;
    QPixmap pxnull;
    int dx[9];
    int dy[9];
signals:
    void fieldChanged();
public slots:
    void up();
    void down();
    void right();
    void left();

    void attack();
    void newgame();
    void ctrlz();
    void cheatEnable();
    void inversion();

    TPoint helpMeSimple();
    TPoint helpMeHard();

    void setLevelHard();
    void setLevelSimple();

    void setFieldSize(int i);

    void saveField();
    void loadField();
};

#endif // FIELDGRAPH_H
