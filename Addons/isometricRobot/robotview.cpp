#include "robotview.h"
#include "constants.h"
#include "util.h"
#include "robotdirection.h"
//#include "robotanimator.h"
#include "plugin.h"

#define DEBUG_Z_ORDER false

#define MIN_ROBOT_DELAY 1000
#define MAX_ROBOT_DELAY 200
#define ROBOT_DELAY_STEP 200;

qreal RobotView::m_cellSize = 45.0;
qreal RobotView::m_cellBorderSize = 1.0;
QColor RobotView::m_unpaintedColor = QColor("lightgreen");
QColor RobotView::m_paintedColor = QColor("gray");
qreal RobotView::m_wallWidth = 10;
qreal RobotView::m_wallHeight = 10.0;



RobotView::RobotView(bool with_controls, bool with_bachground,
                     bool allowScrollBars,
                     const QSize &minSize, QWidget *parent) :
        QGraphicsView(parent)
{
    b_withControls = with_controls;
    b_mousePressed = false;
    b_allowScrollBars = allowScrollBars;
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setMinimumSize(minSize);
    m_scene = new QGraphicsScene(this);
    setFrameShape(QFrame::NoFrame);
    setRenderHint(QPainter::Antialiasing, true);
    setScene(m_scene);
    m_robotItem = NULL;
    const QString imagesRoot = Plugin::moduleImagesRoot();

    if (with_bachground) {
        QPixmap bgPixmap(imagesRoot+"/stars.png");
        setBackgroundBrush(QBrush(bgPixmap));
    }
    else {
        setBackgroundBrush(QColor("black"));
    }
//    m_robotAnimator = new RobotAnimator(this);
//    QPixmap grass(imagesRoot+"/grass.png");
//    QPixmap pgrass(imagesRoot+"/grass_painted.png");
    for (int i=0; i<8; i++) {
        QPixmap px(imagesRoot+"/grass_"+QString::number(i)+".png");
        QBrush br(px);
        lbr_grass << br;
    }
//    m_grassBrush = new QBrush(grass);
//    m_paintedGrassBrush = new QBrush(pgrass);
#ifndef QT_NO_CURSOR
    if (allowScrollBars) {
        horizontalScrollBar()->setCursor(Qt::ArrowCursor);
        verticalScrollBar()->setCursor(Qt::ArrowCursor);
    }
#endif
    if (with_controls) {
        setMouseTracking(true);
    }


}


void RobotView::setPlugin(Plugin *p)
{
    m_plugin = p;
}



void RobotView::createField()
{
    m_scene->clear();
    QPointF sceneTopLeft, sceneBottomRight;
    QRectF rect;
    for (int i=0; i<m_field.size(); i++) {
        rect = createEmptyCell(-1,i,false,false,true);
        if (rect.topLeft().x()<sceneTopLeft.x())
            sceneTopLeft.setX(rect.topLeft().x());
        if (rect.topLeft().y()<sceneTopLeft.y())
            sceneTopLeft.setY(rect.topLeft().y());
        rect = createEmptyCell(m_field[0].size(),i,false,false,true);
        if (rect.bottomRight().x()>sceneBottomRight.x())
            sceneBottomRight.setX(rect.bottomRight().x());
        if (rect.bottomRight().y()>sceneBottomRight.y())
            sceneBottomRight.setY(rect.bottomRight().y());
        for (int j=0; j<m_field[i].size(); j++) {
            createEmptyCell(j,i,m_field[i][j].painted,m_field[i][j].pointed,false);
            if (m_field[i][j].wallUp) {
                createHorizontalWall(j,i,m_field[i][j].baseZOrder-0.1);
            }
            if (m_field[i][j].wallDown) {
                createHorizontalWall(j,i+1,m_field[i][j].baseZOrder+0.001);
            }
            if (m_field[i][j].wallLeft) {
                createVerticalWall(j,i,m_field[i][j].baseZOrder-0.1);
            }
            if (m_field[i][j].wallRight) {
                createVerticalWall(j+1,i,m_field[i][j].baseZOrder+0.001);
            }
        }
    }
    for (int i=0; i<m_field[0].size(); i++) {
        createEmptyCell(i,-1,false,false,true);
        createEmptyCell(i,m_field.size(),false,false,true);
    }
    createEmptyCell(-1,-1,false,false,true);
    createEmptyCell(m_field[0].size(),-1,false,false,true);
    createEmptyCell(m_field[0].size(),m_field.size(),false,false,true);
    createEmptyCell(-1,m_field.size(),false,false,true);

    qreal sceneX = sceneTopLeft.x();
    qreal sceneY = sceneTopLeft.y();
    qreal sceneWidth = fabs(sceneBottomRight.x()-sceneTopLeft.x());
    qreal sceneHeight = fabs(sceneBottomRight.y()-sceneTopLeft.y());


//    m_scene->addRect(sceneX+2, sceneY+2,
//                     sceneWidth-4, sceneHeight-4,
//                     QPen(QColor("white"),4), Qt::NoBrush);

    setSceneRect(sceneX, sceneY, sceneWidth, sceneHeight);
}

void RobotView::createHorizontalWall(int x, int y, qreal zOrder)
{
    QVector<QPointF> points;
    QPolygonF polygon;
    QGraphicsPolygonItem *item = NULL;

    qreal x1_isometric;
    qreal y1_isometric;
    qreal z1_isometric;

    qreal x2_isometric;
    qreal y2_isometric;
    qreal z2_isometric;

    qreal x3_isometric;
    qreal y3_isometric;
    qreal z3_isometric;

    qreal x4_isometric;
    qreal y4_isometric;
    qreal z4_isometric;

    /* В изометрическом изображении стена имеет только 3 видимые грани:
                 две боковые и верхнюю */

    /* передняя грань (обход координат по часовой стрелке от левого нижнего угла)*/
    x1_isometric = x*m_cellSize;// - m_wallWidth/2;
    y1_isometric = y*m_cellSize + m_wallWidth/2;
    z1_isometric = 0.0;

    x2_isometric = x*m_cellSize;// - m_wallWidth/2;
    y2_isometric = y*m_cellSize + m_wallWidth/2;
    z2_isometric = m_wallHeight;

    x3_isometric = (x+1)*m_cellSize;// + m_wallWidth/2;
    y3_isometric = y*m_cellSize + m_wallWidth/2;
    z3_isometric = m_wallHeight;

    x4_isometric = (x+1)*m_cellSize;// + m_wallWidth/2;
    y4_isometric = y*m_cellSize + m_wallWidth/2;
    z4_isometric = 0.0;

    points.clear();
    points << mapToIsometricCoordinates(x1_isometric,y1_isometric,z1_isometric);
    points << mapToIsometricCoordinates(x2_isometric,y2_isometric,z2_isometric);
    points << mapToIsometricCoordinates(x3_isometric,y3_isometric,z3_isometric);
    points << mapToIsometricCoordinates(x4_isometric,y4_isometric,z4_isometric);
    polygon = QPolygonF(points);
    item = new QGraphicsPolygonItem(polygon);
    item->setPen(wallPen());
    item->setBrush(wallBrush());
    item->setZValue(zOrder);
    m_scene->addItem(item);

    /* боковая грань (обход координат по часовой стрелке от левого нижнего угла)*/

    x1_isometric = (x+1)*m_cellSize;// + m_wallWidth/2;
    y1_isometric = y*m_cellSize + m_wallWidth/2;
    z1_isometric = 0.0;

    x2_isometric = (x+1)*m_cellSize;// + m_wallWidth/2;
    y2_isometric = y*m_cellSize + m_wallWidth/2;
    z2_isometric = m_wallHeight;

    x3_isometric = (x+1)*m_cellSize;// + m_wallWidth/2;
    y3_isometric = y*m_cellSize - m_wallWidth/2;
    z3_isometric = m_wallHeight;

    x4_isometric = (x+1)*m_cellSize;// + m_wallWidth/2;
    y4_isometric = y*m_cellSize - m_wallWidth/2;
    z4_isometric = 0.0;

    points.clear();
    points << mapToIsometricCoordinates(x1_isometric,y1_isometric,z1_isometric);
    points << mapToIsometricCoordinates(x2_isometric,y2_isometric,z2_isometric);
    points << mapToIsometricCoordinates(x3_isometric,y3_isometric,z3_isometric);
    points << mapToIsometricCoordinates(x4_isometric,y4_isometric,z4_isometric);
    polygon = QPolygonF(points);
    item = new QGraphicsPolygonItem(polygon);
    item->setPen(wallPen());
    item->setBrush(wallBrush());
    item->setZValue(zOrder);
    m_scene->addItem(item);

    /* верхняя грань (обход координат по часовой стрелке от левого нижнего угла)*/

    x1_isometric = x*m_cellSize;// - m_wallWidth/2;
    y1_isometric = y*m_cellSize + m_wallWidth/2;
    z1_isometric = m_wallHeight;

    x2_isometric = x*m_cellSize;// - m_wallWidth/2;
    y2_isometric = y*m_cellSize - m_wallWidth/2;
    z2_isometric = m_wallHeight;

    x3_isometric = (x+1)*m_cellSize;// + m_wallWidth/2;
    y3_isometric = y*m_cellSize - m_wallWidth/2;
    z3_isometric = m_wallHeight;

    x4_isometric = (x+1)*m_cellSize;// + m_wallWidth/2;
    y4_isometric = y*m_cellSize + m_wallWidth/2;
    z4_isometric = m_wallHeight;

    points.clear();
    points << mapToIsometricCoordinates(x1_isometric,y1_isometric,z1_isometric);
    points << mapToIsometricCoordinates(x2_isometric,y2_isometric,z2_isometric);
    points << mapToIsometricCoordinates(x3_isometric,y3_isometric,z3_isometric);
    points << mapToIsometricCoordinates(x4_isometric,y4_isometric,z4_isometric);
    polygon = QPolygonF(points);
    item = new QGraphicsPolygonItem(polygon);
    item->setPen(wallPen());
    item->setBrush(wallBrush());
    item->setZValue(zOrder);
    m_scene->addItem(item);

}

void RobotView::createVerticalWall(int x, int y, qreal zOrder)
{
    QVector<QPointF> points;
    QPolygonF polygon;
    QGraphicsPolygonItem *item = NULL;

    qreal x1_isometric;
    qreal y1_isometric;
    qreal z1_isometric;

    qreal x2_isometric;
    qreal y2_isometric;
    qreal z2_isometric;

    qreal x3_isometric;
    qreal y3_isometric;
    qreal z3_isometric;

    qreal x4_isometric;
    qreal y4_isometric;
    qreal z4_isometric;

    /* В изометрическом изображении стена имеет только 3 видимые грани:
                 две боковые и верхнюю */

    /* передняя грань (обход координат по часовой стрелке от левого нижнего угла)*/
    x1_isometric = x*m_cellSize - m_wallWidth/2;
    y1_isometric = (y+1)*m_cellSize;// - m_wallWidth/2;
    z1_isometric = 0.0;

    x2_isometric = x*m_cellSize - m_wallWidth/2;
    y2_isometric = (y+1)*m_cellSize;// - m_wallWidth/2;
    z2_isometric = m_wallHeight;

    x3_isometric = x*m_cellSize + m_wallWidth/2;
    y3_isometric = (y+1)*m_cellSize;// - m_wallWidth/2;
    z3_isometric = m_wallHeight;

    x4_isometric = x*m_cellSize + m_wallWidth/2;
    y4_isometric = (y+1)*m_cellSize;// - m_wallWidth/2;
    z4_isometric = 0.0;

    points.clear();
    points << mapToIsometricCoordinates(x1_isometric,y1_isometric,z1_isometric);
    points << mapToIsometricCoordinates(x2_isometric,y2_isometric,z2_isometric);
    points << mapToIsometricCoordinates(x3_isometric,y3_isometric,z3_isometric);
    points << mapToIsometricCoordinates(x4_isometric,y4_isometric,z4_isometric);
    polygon = QPolygonF(points);
    item = new QGraphicsPolygonItem(polygon);
    item->setPen(wallPen());
    item->setBrush(wallBrush());
    item->setZValue(zOrder);
    m_scene->addItem(item);

    /* боковая грань (обход координат по часовой стрелке от левого нижнего угла)*/

    x1_isometric = x*m_cellSize + m_wallWidth/2;
    y1_isometric = (y+1)*m_cellSize;// - m_wallWidth/2;
    z1_isometric = 0.0;

    x2_isometric = x*m_cellSize + m_wallWidth/2;
    y2_isometric = (y+1)*m_cellSize;// - m_wallWidth/2;
    z2_isometric = m_wallHeight;

    x3_isometric = x*m_cellSize + m_wallWidth/2;
    y3_isometric = (y)*m_cellSize;// - m_wallWidth/2;
    z3_isometric = m_wallHeight;

    x4_isometric = x*m_cellSize + m_wallWidth/2;
    y4_isometric = (y)*m_cellSize;// - m_wallWidth/2;
    z4_isometric = 0.0;

    points.clear();
    points << mapToIsometricCoordinates(x1_isometric,y1_isometric,z1_isometric);
    points << mapToIsometricCoordinates(x2_isometric,y2_isometric,z2_isometric);
    points << mapToIsometricCoordinates(x3_isometric,y3_isometric,z3_isometric);
    points << mapToIsometricCoordinates(x4_isometric,y4_isometric,z4_isometric);
    polygon = QPolygonF(points);
    item = new QGraphicsPolygonItem(polygon);
    item->setPen(wallPen());
    item->setBrush(wallBrush());
    item->setZValue(zOrder);
    m_scene->addItem(item);

    /* верхняя грань (обход координат по часовой стрелке от левого нижнего угла)*/

    x1_isometric = x*m_cellSize - m_wallWidth/2;
    y1_isometric = (y+1)*m_cellSize;// - m_wallWidth/2;
    z1_isometric = m_wallHeight;

    x2_isometric = x*m_cellSize - m_wallWidth/2;
    y2_isometric = (y)*m_cellSize;// - m_wallWidth/2;
    z2_isometric = m_wallHeight;

    x3_isometric = x*m_cellSize + m_wallWidth/2;
    y3_isometric = (y)*m_cellSize;// - m_wallWidth/2;
    z3_isometric = m_wallHeight;

    x4_isometric = x*m_cellSize + m_wallWidth/2;
    y4_isometric = (y+1)*m_cellSize;// - m_wallWidth/2;
    z4_isometric = m_wallHeight;

    points.clear();
    points << mapToIsometricCoordinates(x1_isometric,y1_isometric,z1_isometric);
    points << mapToIsometricCoordinates(x2_isometric,y2_isometric,z2_isometric);
    points << mapToIsometricCoordinates(x3_isometric,y3_isometric,z3_isometric);
    points << mapToIsometricCoordinates(x4_isometric,y4_isometric,z4_isometric);
    polygon = QPolygonF(points);
    item = new QGraphicsPolygonItem(polygon);
    item->setPen(wallPen());
    item->setBrush(wallBrush());
    item->setZValue(zOrder);
    m_scene->addItem(item);
}

QPen RobotView::wallPen()
{
    return QPen(QColor("black"),m_cellBorderSize);
}

QBrush RobotView::wallBrush()
{
    return QBrush(QColor("sandybrown"), Qt::Dense4Pattern);
}

QRectF RobotView::createEmptyCell(int x, int y,
                                  bool painted, bool pointed,
                                  bool isBorder)
{

    qreal x1_isometric = m_cellSize*x;
    qreal y1_isometric = m_cellSize*y;
    qreal z1_isometric = 0.0;

    qreal x2_isometric = m_cellSize*x;
    qreal y2_isometric = m_cellSize*y;
    qreal z2_isometric = 0.0;

    qreal x3_isometric = m_cellSize*x;
    qreal y3_isometric = m_cellSize*y;
    qreal z3_isometric = 0.0;

    qreal x4_isometric = m_cellSize*x;
    qreal y4_isometric = m_cellSize*y;
    qreal z4_isometric = 0.0;

    if (isBorder) {
        if (y==m_field.size()) {
            y3_isometric += m_cellSize/2;
            y4_isometric += m_cellSize/2;
        }
        else if (y==-1) {
            y1_isometric += m_cellSize/2;
            y2_isometric += m_cellSize/2;
            y3_isometric += m_cellSize;
            y4_isometric += m_cellSize;
        }
        else {
            y3_isometric += m_cellSize;
            y4_isometric += m_cellSize;
        }
        if (x==m_field[0].size()) {
            x2_isometric += m_cellSize/2;
            x3_isometric += m_cellSize/2;
        }
        else if (x==-1) {
            x1_isometric += m_cellSize/2;
            x4_isometric += m_cellSize/2;
            x2_isometric += m_cellSize;
            x3_isometric += m_cellSize;
        }
        else {
            x2_isometric += m_cellSize;
            x3_isometric += m_cellSize;
        }

    }
    else {
        x2_isometric += m_cellSize;
        x3_isometric += m_cellSize;
        y3_isometric += m_cellSize;
        y4_isometric += m_cellSize;
    }

    QVector<QPointF> points;

    points << mapToIsometricCoordinates(x1_isometric, y1_isometric, z1_isometric);
    points << mapToIsometricCoordinates(x2_isometric, y2_isometric, z2_isometric);
    points << mapToIsometricCoordinates(x3_isometric, y3_isometric, z3_isometric);
    points << mapToIsometricCoordinates(x4_isometric, y4_isometric, z4_isometric);

    QPolygonF polygon(points);

    QGraphicsPolygonItem *result = new QGraphicsPolygonItem(polygon);

    m_scene->addItem(result);
    result->setZValue(-1000);
    if (y>=0&&y<m_field.size()&&x>=0&&x<m_field[0].size()) {
        m_field[y][x].cellItem = result;
        updateCell(x,y,painted);
    }
    else {
        result->setPen(QPen(QColor("black"),m_cellBorderSize));
        result->setBrush(lbr_grass[0]);
    }

    if (DEBUG_Z_ORDER) {
        QPointF textPoint = mapToIsometricCoordinates(m_cellSize*x-m_cellSize/2+20,m_cellSize*y+m_cellSize/2+2,0);
        QGraphicsTextItem *text = new QGraphicsTextItem(QString::number(m_field[y][x].baseZOrder,'f',1));
        text->setFont(QFont("sans-serif",6));
        text->setZValue(-900);
        text->setPos(textPoint);
        m_scene->addItem(text);
    }

    if (pointed) {

        QVector<QPointF> points(4);

        qreal OFFSET = 15;

        points[0] = mapToIsometricCoordinates(m_cellSize*x+OFFSET, m_cellSize*y+OFFSET, 0);
        points[1] = mapToIsometricCoordinates(m_cellSize*(x+1)-OFFSET, m_cellSize*y+OFFSET, 0);
        points[2] = mapToIsometricCoordinates(m_cellSize*(x+1)-OFFSET, m_cellSize*(y+1)-OFFSET, 0);
        points[3] = mapToIsometricCoordinates(m_cellSize*x+OFFSET, m_cellSize*(y+1)-OFFSET, 0);



        QAbstractGraphicsShapeItem *item = new QGraphicsPolygonItem(QPolygonF(points));

        item->setBrush(QColor(255,255,255,100));
        item->setPen(QPen(QColor("black"), 2));
        item->setZValue(m_field[y][x].baseZOrder);
        scene()->addItem(item);


    }
    else {
//        m_field[y][x].pointItem = NULL;
    }

    return result->boundingRect();

}

void RobotView::updateCell(int x, int y, bool painted)
{
    m_field[y][x].painted = painted;
    m_field[y][x].paintState = painted? lbr_grass.size()-1 : 0;
    QGraphicsPolygonItem *item = m_field[y][x].cellItem;
    item->setPen(QPen(QColor("black"),m_cellBorderSize));
    item->setBrush(painted? (lbr_grass.last()) : (lbr_grass.first()));
    item->update();
}

void RobotView::createRobot(int x, int y, RobotItem::Direction direction)
{
    m_robotItem = new RobotItem(this);
    m_robotItem->setAnimated(false);
    m_robotItem->setDirection(direction);
    Point2Di p;
    p.x = x;
    p.y = y;
    m_robotItem->setScenePosition(p);
//    m_robotX = x;
//    m_robotY = y;
//    m_originalRobotX = x;
//    m_originalRobotY = y;
//    m_originalRobotDirection = direction;
//    m_robotItem->setPos(calculateRobotPosition(x,y));
//    m_robotItem->setZValue(m_field[y][x].baseZOrder);
//    m_robotItem->setDirection(direction);
//    m_robotAnimator->setRobotItem(m_robotItem, m_robotDirection);
}



void RobotView::moveRobot(int x, int y)
{
    Point2Di p;
    p.x = x;
    p.y = y;
    m_robotItem->setScenePosition(p);
//    m_robotX = x;
//    m_robotY = y;
//    QPointF targetPoint = calculateRobotPosition(x,y);
//    m_robotItem->setZValue(m_field[y][x].baseZOrder);
////    m_robotAnimator->setSourceZOrder(m_robotItem->zValue());
////    m_robotAnimator->setTargetZOrder(m_field[y][x].baseZOrder);
////    m_robotAnimator->moveTo(targetPoint);
//    Point3Dr coord;
//    coord.x = targetPoint.x();
//    coord.y = targetPoint.y();
//    coord.z = m_field[y][x].baseZOrder;
//    m_robotItem->moveTo(coord);
}

void RobotView::finishEvaluation()
{
    for (int y=0; y<m_field.size(); y++) {
        for (int x=0; x<m_field[y].size(); x++) {
            updateCell(x, y, m_field[y][x].painted);
        }
    }
    m_robotItem->setAnimated(true);
    m_robotItem->setScenePosition(m_robotItem->scenePosition());
    m_robotItem->setDirection(m_robotItem->direction());
//    m_robotItem->setPos(calculateRobotPosition(m_robotX, m_robotY));
//    m_robotItem->setZValue(m_field[m_robotY][m_robotX].baseZOrder);
//    m_robotAnimator->setRobotItem(m_robotItem, m_robotDirection);
//    if (m_broken)
//        m_robotAnimator->showRobotBroken(m_robotDirection);
}





// TODO : Implement all these methods

bool RobotView::goForward()
{
    qint16 nextX = m_robotItem->scenePosition().x;
    qint16 nextY = m_robotItem->scenePosition().y;
    if (m_robotItem->direction()==RobotItem::North)
        nextY--;
    else if (m_robotItem->direction()==RobotItem::South)
        nextY++;
    else if (m_robotItem->direction()==RobotItem::West)
        nextX--;
    else
        nextX++;
    bool wall = false;
    if (nextX<0 || nextX>=m_field[0].size() || nextY<0 || nextY>=m_field.size())
        wall = true;
    if (m_robotItem->direction()==RobotItem::North)
        wall = wall || m_field[m_robotItem->scenePosition().y][m_robotItem->scenePosition().x].wallUp;
    if (m_robotItem->direction()==RobotItem::South)
        wall = wall || m_field[m_robotItem->scenePosition().y][m_robotItem->scenePosition().x].wallDown;
    if (m_robotItem->direction()==RobotItem::West)
        wall = wall || m_field[m_robotItem->scenePosition().y][m_robotItem->scenePosition().x].wallLeft;
    if (m_robotItem->direction()==RobotItem::East)
        wall = wall || m_field[m_robotItem->scenePosition().y][m_robotItem->scenePosition().x].wallRight;
    if (wall) {
        m_robotItem->setBroken(true);
        return false;
    }
    else {
        Point2Di p;
        p.x = nextX;
        p.y = nextY;
        m_robotItem->moveTo(p);
        return true;
    }
}

void RobotView::turnLeft()
{
    m_robotItem->turnLeft();
}

void RobotView::turnRight()
{
    m_robotItem->turnRight();
}

void RobotView::doPaint()
{
    m_robotItem->doPaint();
}

bool RobotView::isWall()
{
    bool v = false;
    Point2Di rp = m_robotItem->scenePosition();
    RobotCell cell = m_field[rp.x][rp.y];
    if (m_robotItem->direction()==RobotItem::North)
        v = cell.wallUp;
    if (m_robotItem->direction()==RobotItem::South)
        v = cell.wallDown;
    if (m_robotItem->direction()==RobotItem::East)
        v = cell.wallRight;
    if (m_robotItem->direction()==RobotItem::West)
        v = cell.wallLeft;
    return v;
}

bool RobotView::isPainted()
{
    Point2Di rp = m_robotItem->scenePosition();
    RobotCell cell = m_field[rp.x][rp.y];
    bool v = cell.painted;
    return v;
}

void RobotView::reset()
{
    for (int i=0;i<m_originalField.size(); i++)
    {
        for (int j=0;j<m_originalField[i].size(); j++) {
            updateCell(j,i,m_originalField[i][j].painted);
        }
    }
    m_robotItem->setAnimated(false);
    m_robotItem->setScenePosition(m_originalRobotPosition);
    m_robotItem->setDirection(m_originalRobotDirection);
    m_robotItem->setBroken(false);
}

#define LEFT_WALL 	0x1
#define RIGHT_WALL 	0x2
#define DOWN_WALL 	0x4
#define UP_WALL   	0x8


RobotErrors::EnvironmentError RobotView::loadEnvironment(const QString &fileName)
{
    QFile f(fileName.trimmed());
    if (!QFile::exists(fileName.trimmed()))
        return RobotErrors::FileNotExists;
    if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
        m_field.clear();
        int w, h;
        int roboX, roboY;
        bool sizeRead = false;
        bool roboPosRead = false;
        RobotItem::Direction dir = RobotItem::South;
        QStringList lines = QString::fromAscii(f.readAll().data()).split('\n');
        f.close();
        foreach (QString line, lines) {
            if (line.trimmed().startsWith(";") || line.trimmed().isEmpty())
                continue;
            if (!sizeRead) {
                QStringList sVals = line.split(QRegExp("\\s+"));
                if (sVals.count()<2) {
                    qDebug() << "Error at: " << __LINE__;
                    return RobotErrors::FileDamaged;
                }
                bool ok;
                w = sVals.at(0).toInt(&ok);
                if (!ok) {
                    qDebug() << "Error at: " << __LINE__;
                    return RobotErrors::FileDamaged;
                }
                h = sVals.at(1).toInt(&ok);
                if (!ok) {
                    qDebug() << "Error at: " << __LINE__;
                    return RobotErrors::FileDamaged;
                }
                sizeRead = true;
                m_field = QVector< QVector<RobotCell> > (h, QVector<RobotCell>(w) );
                for (int i=0;i<m_field.size();i++)
                    for (int j=0;j<m_field[i].size();j++) {
                    m_field[i][j].painted = false;
                    m_field[i][j].wallLeft = j==0;
                    m_field[i][j].wallRight = j==m_field[i].size()-1;
                    m_field[i][j].wallUp = i==0;
                    m_field[i][j].wallDown = i==m_field.size()-1;
                    m_field[i][j].cellItem = NULL;
                }
                for (int i=0; i<m_field.size(); i++ ) {
                    for (int j=0; j<m_field[0].size(); j++ ) {
                        m_field[i][j].baseZOrder = 10.0*i+10.0*j;
                    }
                }
            }
            else if (!roboPosRead) {
                QStringList sVals = line.split(QRegExp("\\s+"));
                if (sVals.count()<2) {
                    qDebug() << "Error at: " << __LINE__;
                    return RobotErrors::FileDamaged;
                }
                bool ok;
                roboX = sVals[0].toInt(&ok);
                if (!ok) {
                    qDebug() << "Error at: " << __LINE__;
                    return RobotErrors::FileDamaged;
                }
                roboY = sVals[1].toUInt(&ok);
                if (!ok) {
                    qDebug() << "Error at: " << __LINE__;
                    return RobotErrors::FileDamaged;
                }
                if (sVals.count()>2) {
                    QString v = sVals[2];
                    if (v.toLower()=="north")
                        dir = RobotItem::North;
                    else if (v.toLower()=="east")
                        dir = RobotItem::East;
                    else if (v.toLower()=="west")
                        dir = RobotItem::West;
                    else
                        dir = RobotItem::South;
                }                
                roboPosRead = true;
            }
            else {
                QStringList sVals = line.split(QRegExp("\\s+"));
                if (sVals.count()<4) {
                    qDebug() << "Error at: " << __LINE__;
                    return RobotErrors::FileDamaged;
                }
                bool ok;
                int xx = sVals[0].toInt(&ok);
                if (!ok || xx<0 || xx>=w) {
                    qDebug() << "Error at: " << __LINE__;
                    return RobotErrors::FileDamaged;
                }
                int yy = sVals[1].toInt(&ok);
                if (!ok || yy<0 || yy>=h) {
                    qDebug() << "Error at: " << __LINE__;
                    return RobotErrors::FileDamaged;
                }
                int wall = sVals[2].toInt(&ok);
                if (!ok) {
                    qDebug() << "Error at: " << __LINE__;
                    return RobotErrors::FileDamaged;
                }
                bool wallLeft = (wall&LEFT_WALL) == LEFT_WALL;
                bool wallDown = (wall&DOWN_WALL) == DOWN_WALL;
                bool wallRight = (wall&RIGHT_WALL) == RIGHT_WALL;
                bool wallUp = (wall&UP_WALL) == UP_WALL;
                bool pointed = false;
                if (sVals.count()>8) {
                    QString v = sVals[8];
                    if (v=="1") {
                        pointed = true;
                    }
                }
                m_field[yy][xx].wallLeft |= wallLeft;
                m_field[yy][xx].wallDown |= wallDown;
                m_field[yy][xx].wallRight |= wallRight;
                m_field[yy][xx].wallUp |= wallUp;
                m_field[yy][xx].painted = sVals[3]!="0";
                m_field[yy][xx].pointed = pointed;
                if (yy>0) {
                    m_field[yy-1][xx].wallDown |= wallUp;
                }
                if (yy<h-1) {
                    m_field[yy+1][xx].wallUp |= wallDown;
                }
                if (xx>0) {
                    m_field[yy][xx-1].wallRight |= wallLeft;
                }
                if (xx<w-1) {
                    m_field[yy][xx+1].wallLeft |= wallRight;
                }
            }
        }
        createField();
        createRobot(roboX, roboY, dir);
        m_originalField = m_field;
        m_originalRobotDirection = m_robotItem->direction();
        m_originalRobotPosition = m_robotItem->scenePosition();
        setProperMouseCursor();
//        m_robotItem->ensureVisible();
        qDebug() << "Load environ" << fileName << " success!";
        return RobotErrors::NoFileError;
    }
    else {
        qDebug() << "Error opening file: " << fileName;
        qDebug() << " --- " << f.errorString();
        return RobotErrors::FileAccessDenied;
    }
}


void RobotView::mousePressEvent(QMouseEvent *event)
{
    QGraphicsView::mousePressEvent(event);
//    if (b_withControls) {
//        QRect btnDecrSpeed(16+32, 16, 9, 9);
//        QRect btnIncrSpeed(16+32+11, 16, 9, 9);
//        if (btnDecrSpeed.contains(event->pos())) {
//            b_decrSpeedPressed = true;
//            b_incrSpeedPressed = false;
//        }
//        else if (btnIncrSpeed.contains(event->pos())) {
//            b_decrSpeedPressed = false;
//            b_incrSpeedPressed = true;
//        }
//        else {
//            b_decrSpeedPressed = b_incrSpeedPressed = false;
//            b_mousePressed = true;
//            m_lastMousePoint = event->pos();
//            if (horizontalScrollBar()->isVisible() || verticalScrollBar()->isVisible())
//                QApplication::setOverrideCursor(Qt::ClosedHandCursor);
//        }
//    }
//    else {
        b_mousePressed = true;
        m_lastMousePoint = event->pos();
//    }

}

void RobotView::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);
    if (b_mousePressed) {
        QPoint c = event->pos();
        QPoint l = m_lastMousePoint;
        QPoint diff = c - l;
        int h = horizontalScrollBar()->value();
        int v = verticalScrollBar()->value();
        h -= diff.x();
        v -= diff.y();
        horizontalScrollBar()->setValue(h);
        verticalScrollBar()->setValue(v);
        m_lastMousePoint = c;
    }
//    if (b_withControls) {
//        QRect btnDecrSpeed(16+32, 16, 9, 9);
//        QRect btnIncrSpeed(16+32+11, 16, 9, 9);
//        if (btnDecrSpeed.contains(event->pos())) {

//            unsetCursor();
//        }
//        else if (btnIncrSpeed.contains(event->pos())) {

//            unsetCursor();
//        }
//        else {
//            setProperMouseCursor();
//        }
//    }
}

void RobotView::mouseReleaseEvent(QMouseEvent *event)
{
    QGraphicsView::mouseReleaseEvent(event);
#ifndef QT_NO_CURSOR
    if (horizontalScrollBar()->isVisible() || verticalScrollBar()->isVisible())
        QApplication::restoreOverrideCursor();
#endif
//    if (b_withControls) {
//        QRect btnDecrSpeed(16+32, 16, 9, 9);
//        QRect btnIncrSpeed(16+32+11, 16, 9, 9);
//        if (btnDecrSpeed.contains(event->pos())) {
//            if (m_plugin && b_decrSpeedPressed) {
//                update();
//            }
//        }
//        else if (btnIncrSpeed.contains(event->pos())) {
//            if (m_plugin && b_incrSpeedPressed) {
//                update();
//            }
//        }
//    }
    b_mousePressed = false;

}

void RobotView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    setProperMouseCursor();
}

void RobotView::setProperMouseCursor()
{
#ifndef QT_NO_CURSOR
    if (horizontalScrollBar()->isVisible() || verticalScrollBar()->isVisible()) {
        setCursor(Qt::OpenHandCursor);
    }
    else {
        unsetCursor();
    }
#endif
}

quint16 RobotView::unpaintedPoints() const
{
    quint16 result = 0;
    for (int y=0; y<m_field.size(); y++) {
        for (int x=0; x<m_field[y].size(); x++) {
            if (m_field[y][x].pointed && !m_field[y][x].painted)
                result ++;
        }
    }
    return result;
}

void RobotView::paintEvent(QPaintEvent *event)
{
    QGraphicsView::paintEvent(event);
//    if (!m_plugin || !b_withControls)
//        return;


//    const int INDICATOR_WIDTH = 30;

//    QPainter p(viewport());
//    p.save();
//    p.setRenderHint(QPainter::Antialiasing, true);
//    p.setPen(QPen(QColor("gray"),1));
//    p.setBrush(QColor("black"));
//    p.drawRect(QRect(16,16,INDICATOR_WIDTH+2,9));

//    qreal currentSpeed = robotSpeed();
//    int w = qMax(0,(int)(currentSpeed * INDICATOR_WIDTH));
//    p.setPen(Qt::NoPen);

//    QLinearGradient grad(QPointF(0,0), QPointF(INDICATOR_WIDTH,0));
//    grad.setColorAt(0.0, QColor("red"));
//    grad.setColorAt(1.0, QColor("yellow"));

//    p.setBrush(grad);
//    p.drawRect(17,17,INDICATOR_WIDTH,7);
//    p.setBrush(QColor("black"));
//    p.drawRect(17+w, 17, INDICATOR_WIDTH-w, 7);

//    QColor enabled("chocolate");
//    QColor disabled("gray");

//    p.setPen(QPen(QColor("gray"),1));
//    bool canDecreaseSpeed = robotSpeed() > MAX_SPEED;
//    p.setBrush(QColor(canDecreaseSpeed? enabled : disabled));
//    p.drawRect(20+INDICATOR_WIDTH, 16, 9, 9);

//    bool canIncreaseSpeed = robotSpeed() < MIN_SPEED;
//    p.setBrush(QColor(canIncreaseSpeed? enabled : disabled));
//    p.drawRect(20+INDICATOR_WIDTH+11, 16, 9, 9);

//    p.setPen(Qt::NoPen);
//    p.setBrush(QColor("white"));
//    p.drawRect(22+INDICATOR_WIDTH, 20, 5, 1);
//    p.drawRect(22+INDICATOR_WIDTH+11, 20, 5, 1);
//    p.drawRect(22+INDICATOR_WIDTH+13, 18, 1, 5);

//    p.restore();
//    p.end();
}
