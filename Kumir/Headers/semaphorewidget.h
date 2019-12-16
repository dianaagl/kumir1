//
// C++ Interface: semaphorewidget
//
// Description: 
//
//
// Author: Виктор Яковлев <V.Yacovlev@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SEMAPHOREWIDGET_H
#define SEMAPHOREWIDGET_H

#include <QWidget>

/**
	@author Виктор Яковлев <V.Yacovlev@gmail.com>
*/
class SemaphoreWidget : public QWidget
{
Q_OBJECT
public:
    SemaphoreWidget(QWidget *parent = 0);
    ~SemaphoreWidget();
		bool fastRepaint;
	public slots:
		void setModeRed();
		void setModeGreen();
		void setModeYellow();
	protected:
		void paintEvent(QPaintEvent *event);
	private:
		uint mode; // 0 = red, 1 = yellow, 2 = green
};

#endif
