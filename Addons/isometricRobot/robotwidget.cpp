#include "robotwidget.h"

RobotWidget::RobotWidget(QWidget *parent):
		KumRobot(parent)
{
	m_direction = NORTH;

}

bool RobotWidget::goForward() {
	bool result;
	switch (m_direction) {
		case SOUTH:
		result = !DownWall(0,0);
		break;
		case NORTH:
		result = !UpWall(0,0);
		break;
		case EAST:
		result = !RightWall(0,0);
		break;
		case WEST:
		result = !LeftWall(0,0);
		break;
	}
	if (result) {
		switch (m_direction) {
			case SOUTH:
			field->stepDown();
			break;
			case NORTH:
			field->stepUp();
			break;
			case EAST:
			field->stepRight();
			break;
			case WEST:
			field->stepLeft();
			break;
		}
	}
	update();
	return result;
}

bool RobotWidget::goBackward() {
	bool result;
	switch (m_direction) {
		case SOUTH:
		result = !UpWall(0,0);
		break;
		case NORTH:
		result = !DownWall(0,0);
		break;
		case EAST:
		result = !LeftWall(0,0);
		break;
		case WEST:
		result = !RightWall(0,0);
		break;
	}
	if (result) {
		switch (m_direction) {
			case SOUTH:
			field->stepUp();
			break;
			case NORTH:
			field->stepDown();
			break;
			case EAST:
			field->stepLeft();
			break;
			case WEST:
			field->stepRight();
			break;
		}
	}
	update();
	return result;
}

bool RobotWidget::turnLeft()
{
	switch (m_direction) {
		case SOUTH:
		m_direction = EAST;
		break;
		case NORTH:
		m_direction = WEST;
		break;
		case EAST:
		m_direction = NORTH;
		break;
		case WEST:
		m_direction = SOUTH;
		break;
	}
	field->robot->setDirection(m_direction);
	update();
	return true;
}

bool RobotWidget::turnRight()
{
	switch (m_direction) {
		case SOUTH:
		m_direction = WEST;
		break;
		case NORTH:
		m_direction = EAST;
		break;
		case EAST:
		m_direction = SOUTH;
		break;
		case WEST:
		m_direction = NORTH;
		break;
	}
	field->robot->setDirection(m_direction);
	update();
	return true;
}


bool RobotWidget::doPaint()
{
	if (!field->currentCell()->isColored()) {
		field->reverseColorCurrent();
		update();
	}
	return true;
}

bool RobotWidget::isPainted(bool &v)
{
	v = field->currentCell()->isColored();
	return true;
}

bool RobotWidget::isWall(bool &v)
{
	switch (m_direction) {
		case SOUTH:
		v = DownWall(0,0);
		break;
		case NORTH:
		v = UpWall(0,0);
		break;
		case EAST:
		v = RightWall(0,0);
		break;
		case WEST:
		v = LeftWall(0,0);
		break;
		default:
		v = false;
		break;
	}
	return true;
}

void RobotWidget::reset()
{
	field->robot->setDirection(NORTH);
	m_direction = NORTH;
	PrepareRunMode();
}


bool RobotWidget::loadEnvironment(const QString &fileName)
{
	int result = LoadFromFile(fileName.trimmed());
	return result==0;
}
