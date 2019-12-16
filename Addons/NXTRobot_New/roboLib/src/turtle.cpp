//****************************************************************************
//**
//** Copyright (C) 2008-2010 NIISI RAS. All rights reserved.
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


#include <QtCore>
#include <QtGui>
#include "turtle.h"
#include "pult.h"
#include <QTextEdit>
#include <QTextStream>
#include <QCloseEvent>
#include <QFileDialog>
        static const double Pi = 3.14159265358979323846264338327950288419717;
NXTSetup::NXTSetup(QWidget *parent) : QWidget(parent)

{
    setupUi(this);
    autoClose=false;

    //scene->addLine(0,0,1000,300);
    //QPalette PallGrey;
    //PallGrey.setBrush(QPalette::Window,QBrush(QColor(140,140,100)));
    //mw->setTitle(QString::fromUtf8("Черепаха"));
    ;
    //scene->setPalette (PallGrey);

    //desertBorders.append(QLineF(260-delta,260-delta,260-delta,-220+delta));//Правый край
    // desertBorders.append(QLineF(-270,280-delta,270,280-delta));//нижний край
    // desertBorders.append(QLineF(-225+delta,260-delta,-225+delta,-225+delta));//левый край
    // desertBorders.append(QLineF(-250,-220+delta,260,-220+delta));//верхний край

    ang=0;
    step=30;
    tail=true;
    curX=0;
    curY=0;
    curTurtleId=1;
    zoom=0.2;

    delta=100*zoom;


    m_NXTDriver = new NXTCore(this);
    connect(m_NXTDriver, SIGNAL(scanFinished(QStringList)),
            this, SLOT(scanFinished(QStringList)));

    CreateBorders();
    // 	for(int i=0;i<desertBorders.count();i++)
    // 	   {
    // 		QGraphicsLineItem* bortLine=new QGraphicsLineItem(desertBorders[i]);
    //
    // 		bortLine->setPen(redPen);
    // 		scene->addItem(bortLine);
    // 	   };
    CreateTurtle();
    drawTail();

    QSettings settings("NIISI RAS", "NXT ROBOT");
    int A = settings.value("Motor A",100).toInt();
    int B = settings.value("Motor B",100).toInt();
    calibrateMotorA->setValue(A);
    spinBoxA->setValue(A);
    calibrateMotorB->setValue(B);
    spinBoxB->setValue(B);
    addrLine->addItem(settings.value("BT","").toString());

    connect (calibrateMotorA, SIGNAL(valueChanged(int)),
             this, SLOT(storeValuesInSettings()));
    connect (calibrateMotorB, SIGNAL(valueChanged(int)),
             this, SLOT(storeValuesInSettings()));

    connect (spinBoxA, SIGNAL(valueChanged(int)),
             this, SLOT(storeValuesInSettings()));
    connect (spinBoxB, SIGNAL(valueChanged(int)),
             this, SLOT(storeValuesInSettings()));

    connect (connectButton, SIGNAL(clicked()),
             this, SLOT(storeValuesInSettings()));

    motorAtuningBox->setVisible(false);
    motorBtuningBox->setVisible(false);
}



void NXTSetup::CreateBorders()
{


}
//-----------------------------------
void NXTSetup::CreateTurtle(void)
{
    connect(connectButton,SIGNAL(pressed()),this,SLOT(connectNXT()));
    connected=false;
    connect(disconnectButton,SIGNAL(pressed()),this,SLOT(disconnectNXT()));
    connect(scanButton,SIGNAL(pressed()),this,SLOT(scan()));

    QString appName;
    appName = QCoreApplication::applicationName();
    if (appName.toLower().trimmed()==QString::fromUtf8("пиктомир")) {
        layout()->setContentsMargins(16,16,16,100);
    }
    else {
        imageBox->setVisible(false);
    }

}
void NXTSetup::disconnectNXT()
{
    disconnectButton->setEnabled(false);
    if(connected)connection->close();
    motorAtuningBox->setEnabled(false);
    motorBtuningBox->setEnabled(false);
    connectButton->setEnabled(true);
    connected=false;
    statusl->setText("NXT Disonnected.");
};
void NXTSetup::connectNXT()
{
    statusl->setText("Connecting...");
    //    connection=NXTCore::makeConnection(addrLine->currentText(),error);
    connectButton->setEnabled(false);
    m_NXTDriver->requestConnection(this, addrLine->currentText());
}

bool NXTSetup::event(QEvent *event)
{
    if (event->type()==2000) {
        event->accept();
        NXTConnectionEvent *ce = dynamic_cast<NXTConnectionEvent*>(event);
        Q_CHECK_PTR(ce);
        QString error = ce->error();
        connection = ce->connection();
        qDebug()<<"NXT CONNECTION:"<<error;
        if(error=="") {
            statusl->setText("NXT Connected");
            connected=true;
        }
        else {
            statusl->setText(error);
            connectButton->setEnabled(true);
        }
        connect(connection,SIGNAL(commandFinished(QString,QVariant)),
                this,SLOT(NXTFin(QString,QVariant)),
                Qt::DirectConnection);
        if(connected)
        {
            connection->setRightMotorDistanceScale(3*calibrateMotorA->value()/100);
            connection->setLeftMotorDistanceScale(3*calibrateMotorB->value()/100);
            motorAtuningBox->setEnabled(false);

            motorBtuningBox->setEnabled(false);
            disconnectButton->setEnabled(true);
        }
        return true;
    }
    else {
        return QWidget::event(event);
    }
}


void NXTSetup::scan()
{
    scanButton->setEnabled(false);
    statusl->setText(tr("Scanning..."));
    m_NXTDriver->requestDevicesList();
}

void NXTSetup::scanFinished(const QStringList &devices)
{
    addrLine->clear();
    addrLine->addItems(devices);
    if (devices.isEmpty()) {
        statusl->setText(tr("No devices found"));
    }
    else if (devices.count()==1) {
        statusl->setText(tr("Search finished. Found 1 device"));
    }
    else {
        statusl->setText(tr("Search finished. Choose device from list"));
    }
    scanButton->setEnabled(true);
}

void NXTSetup::NXTFin(QString err,QVariant val)
{
    qDebug()<<"NXTFIN";
    emit NXTReady(err,val);
};
//-----------------------------------
void NXTSetup::rotate(void)
{

    //TODO NXT ROTATE
}
void NXTSetup::rotateImages()
{


};
void NXTSetup::beep()
{
    if(connected)
        connection->doBeep();
};

void NXTSetup::touch()
{
    if(connected)
        connection->touch();
};


bool NXTSetup::moveT(void)
{
    if(!connected)return false;
    qDebug()<<"Step"<<step;
    if(step>0)
        connection->goForward(step);
    else {connection->goBackward(-step);qDebug()<<"go back";};
    bool toret=true;
    if(!toret)
        emit Otkaz(QString::fromUtf8("Край пустыни"));else emit Ok();
    return toret;
}

bool NXTSetup::runEngine(bool a,bool b,bool c)
{
    if(!connected)return false;
    bool toret=true;

    connection->MotorStart(MotorAS,MotorBS,MotorCS,a,b,c);
    if(!toret)
        emit Otkaz(QString::fromUtf8("Ошибка"));else emit Ok();
    return toret;


}

bool NXTSetup::stopEngine(bool a,bool b,bool c)
{
    if(!connected)return false;
    bool toret=true;
    qDebug()<<"NXTSET stop ENGN";
    connection->MotorStop(a,b,c);
    if(!toret)
        emit Otkaz(QString::fromUtf8("Ошибка"));else emit Ok();
    return toret;


}


void NXTSetup::CheckForWall()
{
    if (connected) {
        connection->isWall();
    }
}

void NXTSetup::range(){
     if (connected) {
         qDebug()<<"Setup range";
         connection->range();
     }
 }
void NXTSetup::light(){
     if (connected) {
         qDebug()<<"light setup";
         connection->light();
     }
 }
void NXTSetup::rotation(int motor_id){
     if (connected) {
         qDebug()<<"rotation";
         connection->rotation(motor_id);
     }
 }

void NXTSetup::resetRot(int motor_id){
     if (connected) {
         qDebug()<<"rotation Reset";
         connection->resetRot(motor_id);
     }
};
void NXTSetup::drawTail()
{

};





bool NXTSetup::checkPos(qreal* x,qreal *y)
{




    return true;
};

NXTSetup::~NXTSetup()
{

}
void NXTSetup:: DoRotate(int deg)
{
    qDebug()<<"DO ROTATE";
    if(connected)
        if(deg>0)connection->turnRight(deg);else connection->turnLeft(-deg);
};

void NXTSetup::DoSleep(int time)
{
    qDebug()<<"DO Sleep";
    if(connected)
        connection->sleep_m_sec(time);
};

  void NXTSetup::DoMotorC(int time,int speed)
 {if(connected)
     connection->motorC(time,speed);
 };
void NXTSetup:: DoMove(int range)
{
    step=range;
    moveT();
};

//
//
//
//bool NXTSetup:: isWall()
//{
//    QString err;
//    if(!connected)return false;
//    if(connection->isSensorPressedNow(err))return true;
//    else return false;
//};

void NXTSetup:: closeEvent ( QCloseEvent * event )
{
    qDebug()<<"libM"<<Tpult->libMode<<" autoClose"<<autoClose;
    if ((Tpult->libMode)||(autoClose))
    {
        close();
        event->accept();
        return;
    };
    int ret = QMessageBox::warning(this, QString::fromUtf8("Черепаха"),
                                   QString::fromUtf8("Закрыть исполнитель черепаха?"),
                                   QMessageBox::Yes | QMessageBox::Default,
                                   QMessageBox::No,
                                   QMessageBox::Cancel | QMessageBox::Escape);
    if (ret == QMessageBox::Yes) {
        Tpult->AutoClose();
        Tpult->close();
        event->accept();
    } else {
        event->ignore();
    }

};
void NXTSetup::hideTurtle(QPoint pos)
{

};
void NXTSetup::storeValuesInSettings()
{
    QSettings settings("NIISI RAS", "NXT ROBOT");
    settings.setValue("Motor A",calibrateMotorA->value());
    settings.setValue("Motor B",calibrateMotorB->value());
    settings.setValue("BT",addrLine->currentText());
}
