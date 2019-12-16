//****************************************************************************
//**
//** Copyright (C) 2004-2008 IMPB RAS. All rights reserved.
//**
//** This file is part of the KuMir.
//**
//** This file may be used under the terms of the GNU General Public
//** License version 2.0 as published by the Free Software Foundation
//** and appearing in the file LICENSE.GPL included in the packaging of
//** this file.
//**
//** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
//** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//**
//****************************************************************************/
#ifndef TURTLE_H
#define TURTLE_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QGraphicsScene>
#include <QtSvg>
 #include <QGraphicsSvgItem>
//#include "../../../isp_window_header.h"
# include "../../core/nxtcore.h"
# include "../../core/nxtconnection.h"
#include "../../core/nxtconnectionevent.h"

//class QAction;
//class QMenu;
//class QTextEdit;
#define FIELD_SX 500
#define FIELD_SY 500
#define BORDER_SZ 20

#include "ui_control.h"

class pultLogger;
class NXTPult;

class NXTSetup:
        public QWidget,
        private Ui::ControlForm
{
      Q_OBJECT

public:
      NXTSetup(QWidget *parent = 0);
      ~NXTSetup();

 double   step;
 double   MotorAS,MotorBS,MotorCS;

 qreal grad;
 pultLogger* logger;
 NXTPult* Tpult;
 NXTConnection * connection;
 void AutoClose(){autoClose=true;close();};
 void showTurtle(){showNormal();raise();};
 void hideTurtle(){hide();};
//bool isWall();
bool NXTConnected(){return connected;};
protected:
virtual bool event(QEvent *event);
void closeEvent ( QCloseEvent * event );
public slots:
 void rotate();
 bool moveT();
 bool runEngine(bool a,bool b,bool c);
 bool stopEngine(bool a,bool b,bool c);

 void NXTFin(QString err,QVariant val);


 void DoRotate(int deg);
 void DoSleep(int msec);
 void DoMotorC(int time,int speed);
 void DoMove(int range);
 void CheckForWall();
  void range();
    void light();
 void rotation(int motor_id);
 void resetRot(int motor_id);
 void hideTurtle(QPoint pos);
 void connectNXT();
 void disconnectNXT();
 void scan();
 void scanFinished(const QStringList &devices);
 void beep();
 void touch();
 void storeValuesInSettings();
 signals:
 void Otkaz(QString msg);
 void Ok();
 void NXTReady(QString err,QVariant val);
protected:

private:
 void drawTail();

 void rotateImages();
 void CreateBorders();
 bool checkPos()
	{
	qreal x,y;
	return checkPos(&x,&y);
	};
  bool checkPos(qreal * x,qreal *y);


  void CreateTurtle(void);      
 qreal ang;
 QList<QLineF> desertBorders; 
 bool tail;
 qreal curX,curY;
 qreal zoom,delta;
 qreal AncX,AncY;


 QPointF tailPoint;
 int curTurtleId;
 bool autoClose,obod,connected;
 NXTCore *m_NXTDriver;

// QLineEdit * addrLine;
// QLabel* btal,*statusl,*calLabA,*calLabB,*calLabC;
// QPushButton * connectButton,*scanButton,*disconnectButton;
//QSlider* calibrateMotorA; QSpinBox *AsBox;
//QSlider* calibrateMotorB; QSpinBox *BsBox;
//QSlider* calibrateMotorC; QSpinBox *CsBox;

};

#endif
