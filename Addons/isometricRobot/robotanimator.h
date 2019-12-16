#ifndef ROBOTANIMATOR_H
#define ROBOTANIMATOR_H

#include <QtCore>
#include <QtGui>

#include "robotdirection.h"

enum AnimatorAction { None, Move, TurnLeft, TurnRight, Sleep };


class RobotAnimator
        : public QObject
{
    Q_OBJECT
        public:
    RobotAnimator(QObject *parent);
    void setRobotItem(QGraphicsPixmapItem *item, RobotDirection direction);
    void setAction(AnimatorAction action);
    void setTargetPoint(const QPointF &point);
    void setTargetZOrder(double z);
    void setSourceZOrder(double z);
    void showRobotBroken(RobotDirection direction);
    double robotHeight() const;
    double robotWidth() const;
    void turnLeft();
    void turnRight();
    void moveTo(const QPointF &d);
    void sleep();
    int framesPerTurn() const;
    void clock();
        public slots:
    void updateRobot();
        private:
    void nextFrame();
    void previousFrame();
    QList<QPixmap*> m_movie;
    QMap<RobotDirection, QPixmap*> m_brokenPixmaps;
    QGraphicsPixmapItem *m_robotItem;
    bool m_needChangePixmap;
    int m_currentFrame;
    int m_framesPerTurn;
    double m_width;
    double m_height;
    AnimatorAction m_action;
    QPointF m_targetPoint;
    QPointF m_sourcePoint;
    int m_framesCount;
    double m_sourceZ;
    double m_targetZ;
        signals:
    void updateRequest();
};


#endif // ROBOTANIMATOR_H
