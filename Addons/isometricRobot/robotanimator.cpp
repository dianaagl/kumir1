#include "robotanimator.h"
#include <QtCore>
#include <QtGui>
#include "plugin.h"


RobotAnimator::RobotAnimator(QObject *parent)
        : QObject(parent)
{
    m_robotItem = NULL;
    QString imagesRoot = Plugin::moduleImagesRoot();
    m_framesCount = 16;
    m_framesPerTurn = 4;
    for ( int i=1; i<=m_framesCount; i++ ) {
        Q_ASSERT ( QFile::exists(imagesRoot+"/robot_frames/"+QString::number(i)+".png") );
        QPixmap *frame = new QPixmap(imagesRoot+"/robot_frames/"+QString::number(i)+".png");
        m_movie.append(frame);
    }
    Q_ASSERT ( QFile::exists(imagesRoot+"/robot_frames/broken_north.png") );
    Q_ASSERT ( QFile::exists(imagesRoot+"/robot_frames/broken_south.png") );
    Q_ASSERT ( QFile::exists(imagesRoot+"/robot_frames/broken_east.png") );
    Q_ASSERT ( QFile::exists(imagesRoot+"/robot_frames/broken_west.png") );
    m_brokenPixmaps[NORTH] = new QPixmap(imagesRoot+"/robot_frames/broken_north.png");
    m_brokenPixmaps[SOUTH] = new QPixmap(imagesRoot+"/robot_frames/broken_south.png");
    m_brokenPixmaps[EAST] = new QPixmap(imagesRoot+"/robot_frames/broken_east.png");
    m_brokenPixmaps[WEST] = new QPixmap(imagesRoot+"/robot_frames/broken_west.png");
    Q_ASSERT ( !m_brokenPixmaps[NORTH]->isNull() );
    Q_ASSERT ( !m_brokenPixmaps[SOUTH]->isNull() );
    Q_ASSERT ( !m_brokenPixmaps[EAST]->isNull() );
    Q_ASSERT ( !m_brokenPixmaps[WEST]->isNull() );
    m_currentFrame = 0;
    int w = m_movie[0]->width();
    int h = m_movie[0]->height();
    m_width = (double) w;
    m_height = (double) h;
    m_action = None;
    m_needChangePixmap = false;
    m_sourceZ = m_targetZ = 0;
}

int RobotAnimator::framesPerTurn() const
{
    return m_framesPerTurn;
}


void RobotAnimator::setRobotItem(QGraphicsPixmapItem *item, RobotDirection direction)
{
    m_robotItem = item;
    int blockNo = 0;
    if (direction==EAST)
        blockNo = 1;
    else if (direction==NORTH)
        blockNo = 2;
    else if (direction==WEST)
        blockNo = 3;
    int frameNo = blockNo * m_framesPerTurn;
    m_currentFrame = frameNo;
    m_needChangePixmap = true;
    connect(this, SIGNAL(updateRequest()), this, SLOT(updateRobot()));
    emit updateRequest();
//    updateRobot();
}

void RobotAnimator::setSourceZOrder(double z)
{
    m_sourceZ = z;
}

void RobotAnimator::setTargetZOrder(double z)
{
    m_targetZ = z;
}

double RobotAnimator::robotWidth() const
{
    return m_width;
}

double RobotAnimator::robotHeight() const
{
    return m_height;
}

void RobotAnimator::previousFrame()
{
    if (m_currentFrame==0) {
        m_currentFrame = m_framesCount-1;
    }
    else {
        m_currentFrame--;
    }
}

void RobotAnimator::nextFrame()
{
    if (m_currentFrame==m_framesCount-1) {
        m_currentFrame = 0;
    }
    else {
        m_currentFrame++;
    }
}

void RobotAnimator::turnLeft()
{
    m_action = TurnLeft;
}

void RobotAnimator::turnRight()
{
    m_action = TurnRight;
}

void RobotAnimator::moveTo(const QPointF &d)
{
    m_sourcePoint = m_robotItem->pos();
    m_targetPoint = d;
    m_action = Move;
}

void RobotAnimator::sleep()
{
    m_action = Sleep;
}

void RobotAnimator::clock()
{
    if (m_action==Sleep) {
        m_needChangePixmap = true;
    }
    if (m_action==TurnRight) {
        previousFrame();
        m_needChangePixmap = true;
    }
    if (m_action==TurnLeft) {
        nextFrame();
        m_needChangePixmap = true;
    }
    if (m_action==Move) {
        QPointF distance = m_targetPoint - m_sourcePoint;
        double dz = (m_targetZ - m_sourceZ)/m_framesPerTurn;
        double dx = distance.x()/m_framesPerTurn;
        double dy = distance.y()/m_framesPerTurn;
        m_needChangePixmap = false;
        m_robotItem->moveBy(dx,dy);
        m_robotItem->setZValue(m_robotItem->zValue()+dz);
    }
    emit updateRequest();
}
void RobotAnimator::updateRobot() {
    if (m_needChangePixmap)
        m_robotItem->setPixmap(*(m_movie[m_currentFrame]));
    QRectF r = QRectF(m_robotItem->pos()-QPointF(m_width,m_height),QSizeF(m_width*3,m_height*3));
    m_robotItem->scene()->update(r);
}

void RobotAnimator::showRobotBroken(RobotDirection direction) {
    Q_ASSERT ( m_brokenPixmaps.contains(direction) );
    QPixmap *px = m_brokenPixmaps[direction];
    m_robotItem->setPixmap(*px);
    QRectF r = QRectF(m_robotItem->pos()-QPointF(m_width,m_height),QSizeF(m_width*3,m_height*3));
    m_robotItem->scene()->update(r);
    m_needChangePixmap = true;
}
