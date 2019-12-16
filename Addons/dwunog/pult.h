#ifndef PULT_H
#define PULT_H

#include <QtGui>
#include "dwunogworld.h"

class QLabel;
class QWidget;
class QSpinBox;
class QTcpSocket;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;

class Pult : public QWidget
{
    Q_OBJECT

public:
    Pult(DwunogWorld *world);
    ~Pult();

signals:
    void doStop();

public slots:

    void open();
    void headRight();
    void headLeft();
    void headUp();

    void lFootUp();
    void lFootDown();
    void rFootUp();
    void rFootDown();
    void wait();
    void getInfo();
    void reset();
    void sstop();
    void dok1();
    void dotake();

    void setLabelText(const QString &text);

private:

    QToolButton hl, hr, hu, lfu, lfd, rfu, rfd, wt, rst, stp, nfo, opn;
    QToolButton take, k1;

    QVBoxLayout vl, vl1;
    QHBoxLayout hl1, hl2, hl3, hl4;
    QLabel      label, l1, l2, l3;

    int id;
    bool k1set;

    void createButtons();
    void createLayouts();
    void createButton(QToolButton *bt, QSize size, QString text, QString addr,
                      QKeySequence key = 0);
    DwunogWorld *world;

};

#endif
