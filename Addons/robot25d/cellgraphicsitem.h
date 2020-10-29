#ifndef CELLGRAPHICSITEM_H
#define CELLGRAPHICSITEM_H

#include <QGraphicsPolygonItem>
#include <QtCore>
#include <QtGui>

namespace Robot25D {

class RobotView;

class CellGraphicsItem :
        public QGraphicsPolygonItem
{
public:
    CellGraphicsItem(const QPolygonF &poly,
                     const QPolygonF &polySouth,
                     const QPolygonF &polyEast,
                     const QPolygonF &polyNorth,
                     const QPolygonF &polyWest,
                     bool editable,
                     int cellX,
                     int cellY,
                     RobotView *view,
                     QGraphicsItem *parent = 0);
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
private:
    int whichWall(const QPointF &p) const;

    QPolygonF p_south;
    QPolygonF p_east;
    QPolygonF p_north;
    QPolygonF p_west;
    bool b_editable;
    int i_cellX;
    int i_cellY;
    RobotView *m_view;
    bool b_hoveredCell;

    bool b_pressed;

};

}

#endif // CELLGRAPHICSITEM_H
