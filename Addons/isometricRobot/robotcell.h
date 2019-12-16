#ifndef ROBOTCELL_H
#define ROBOTCELL_H

#include <QtGui>

struct RobotCell {
    inline RobotCell() {
        painted = pointed = false;
        wallUp = wallDown = wallLeft = wallRight = false;
        cellItem = NULL;
        pointItem = NULL;
        paintState = 0;
    }
    bool painted;
    bool wallUp;
    bool wallDown;
    bool wallLeft;
    bool wallRight;
    bool pointed;
    quint8 paintState;
    qreal baseZOrder;
    QGraphicsPolygonItem *cellItem;
    QGraphicsItem *pointItem;
};

#endif // ROBOTCELL_H
