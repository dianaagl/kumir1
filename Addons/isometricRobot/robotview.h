#ifndef ROBOTVIEW_H
#define ROBOTVIEW_H

#include <QtCore>
#include <QtGui>

#include "robotcell.h"
#include "roboterrors.h"
#include "robotitem.h"

class Plugin;

class RobotView :
        public QGraphicsView
{
    friend class RobotItem;
    Q_OBJECT;
    Q_PROPERTY(quint16 unpaintedPoints READ unpaintedPoints);
public:
    RobotView(bool withConrols, bool withBackground,
              bool allowScrollBars,
              const QSize &minSize, QWidget *parent=NULL);
    quint16 unpaintedPoints() const;
    void setPlugin(Plugin *p);
    void finishEvaluation();
    inline qreal baseZOrder(int x, int y) { return m_field[y][x].baseZOrder; }
    inline int robotSpeed() const { return m_robotItem->speed(); }
public slots:
    inline void setRobotSpeed(int msec) { m_robotItem->setSpeed(msec); }
protected:
    QRectF createEmptyCell(int x, int y, bool painted, bool pointed, bool isBorder);
    void updateCell(int x, int y, bool painted);
    void createHorizontalWall(int x, int y, qreal zOrder);
    void createVerticalWall(int x, int y, qreal zOrder);
    void createField();
    void createRobot(int x, int y, RobotItem::Direction direction);

    static QPen wallPen();
    static QBrush wallBrush();
    static qreal m_zMax;
    static qreal m_sceneRotationAngle;
    static qreal m_sceneSlopeAngle;
    static qreal m_cellSize;
    static QColor m_unpaintedColor;
    static QColor m_paintedColor;
    static qreal m_cellBorderSize;
    static qreal m_wallWidth;
    static qreal m_wallHeight;
    static QColor m_wallColor;


    QVector< QVector<RobotCell> > m_field;
    QVector< QVector<RobotCell> > m_originalField;

    RobotItem *m_robotItem;


    bool b_withControls;
    bool b_allowScrollBars;

    Point2Di m_originalRobotPosition;
    RobotItem::Direction m_originalRobotDirection;


//    RobotAnimator *m_robotAnimator;
//    QBrush *m_grassBrush;
//    QBrush *m_paintedGrassBrush;
    QList<QBrush> lbr_grass;

    QGraphicsScene *m_scene;
    Plugin *m_plugin;

    bool b_incrSpeedPressed;
    bool b_decrSpeedPressed;
    bool b_mousePressed;


    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
    virtual void paintEvent(QPaintEvent *event);

    void setProperMouseCursor();


    QPoint m_lastMousePoint;

protected slots:
    void moveRobot(int x, int y);

    /* Robot actor methods */
public slots:
    inline void setAnimated(bool v) { m_robotItem->setAnimated(v); }
    bool goForward();
    void turnLeft();
    void turnRight();
    void doPaint();
    bool isWall();
    bool isPainted();
    void reset();
    RobotErrors::EnvironmentError loadEnvironment(const QString &fileName);

signals:
    void sync();



};

#endif // ROBOTVIEW_H
