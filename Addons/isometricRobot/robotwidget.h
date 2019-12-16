#ifndef ROBOTWIDGET_H
#define ROBOTWIDGET_H

#include <QtCore>
#include <QtGui>
#include "robotinterface.h"
#include "robot/robot.h"

#include "robot/robotdirection.h"

class RobotWidget :
		public KumRobot,
		public RobotInterface
{
	Q_OBJECT
	Q_INTERFACES(RobotInterface)
	public:
    RobotWidget(QWidget *parent = NULL);
		virtual bool goForward();
		virtual bool goBackward();
		virtual bool turnLeft();
		virtual bool turnRight();
		virtual bool doPaint();
		virtual bool isWall(bool &v);
		virtual bool isPainted(bool &v);
		virtual void reset();
		virtual bool loadEnvironment(const QString &fileName);


	protected:
		RobotDirection m_direction;

};

#endif // ROBOTWIDGET_H
