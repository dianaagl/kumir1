/****************************************************************************
**
** Copyright (C) 2004-2008 NIISI RAS. All rights reserved.
**
** This file is part of the KuMir.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#include "interface.h"

void TurtleStarter::start()
{
    //turtleHeader=new WHeader();
    mw = new NXTSetup();
    t_pult=new NXTPult();
    errortext="";
    skipSync=false;
    mw->resize(300,150);
    //mw->scene->addLine(-600,0,100,0);
    //mw->scene->addLine(0,-200,0,100);
    mw->logger=t_pult->pltLogger();
    server=new KNPServer();

    //turtleHeader->setChildWidget(turtle);

    server->SigCross->setNXT(mw);
    //  mw->show();
    //	scene->addLine(0,0,200,0);

    t_pult->turtleObj=mw;
    t_pult->Connect(server);
    t_pult->libMode=true;
    mw->Tpult=t_pult;
    connect(t_pult,SIGNAL(sendText(QString)),this,SLOT(sendText2Kumir(QString)));
    connect(mw,SIGNAL(NXTReady(QString,QVariant)),
            this,SLOT(NXTReady(QString,QVariant)),
            Qt::DirectConnection
            );
    for(int i=0;i<4;i++)
             timers.append(QTime());

    // t_pult->show();
};
void TurtleStarter::NXTReady(QString err, QVariant value)
{
    qDebug()<<"NXT Sync";
    errortext=err;
    replay=value;
    emit sync();
};
QList<Alg> TurtleStarter::algList()
{

    QList<Alg> toRet;
    QFile f;
    QByteArray d;
    Alg tmp;
    tmp.kumirHeader=trUtf8("алг вперед(цел расстояние_30)");
    //tmp.stepsCount=5;
    f.setFileName(":/pictomir_icons/actions/forward.svg");
    f.open(QIODevice::ReadOnly);
    d = f.readAll();
    f.close();
    //tmp.iconSvgData = d;

    toRet<<tmp;
    tmp.kumirHeader=trUtf8("алг назад(цел расстояние_30)");
    f.setFileName(":/pictomir_icons/actions/backward.svg");
    f.open(QIODevice::ReadOnly);
    d = f.readAll();
    f.close();


    toRet<<tmp;
    tmp.kumirHeader=trUtf8("алг скорость А(цел скорость)");
    f.setFileName(":/pictomir_icons/actions/backward.svg");
    f.open(QIODevice::ReadOnly);
    d = f.readAll();
    f.close();

    toRet<<tmp;
    tmp.kumirHeader=trUtf8("алг скорость A(цел скорость)");
    f.setFileName(":/pictomir_icons/actions/backward.svg");
    f.open(QIODevice::ReadOnly);
    d = f.readAll();
    f.close();

    toRet<<tmp;
    tmp.kumirHeader=trUtf8("алг скорость В(цел скорость)");
    f.setFileName(":/pictomir_icons/actions/backward.svg");
    f.open(QIODevice::ReadOnly);
    d = f.readAll();
    f.close();

    toRet<<tmp;
    tmp.kumirHeader=trUtf8("алг скорость B(цел скорость)");
    f.setFileName(":/pictomir_icons/actions/backward.svg");
    f.open(QIODevice::ReadOnly);
    d = f.readAll();
    f.close();


    toRet<<tmp;
    tmp.kumirHeader=trUtf8("алг скорость С(цел скорость)");
    f.setFileName(":/pictomir_icons/actions/backward.svg");
    f.open(QIODevice::ReadOnly);
    d = f.readAll();
    f.close();

    toRet<<tmp;
    tmp.kumirHeader=trUtf8("алг скорость C(цел скорость)");
    f.setFileName(":/pictomir_icons/actions/backward.svg");
    f.open(QIODevice::ReadOnly);
    d = f.readAll();
    f.close();

    toRet<<tmp;
    tmp.kumirHeader=trUtf8("алг запустить моторы(лог A,лог B, лог C)");
    f.setFileName(":/pictomir_icons/actions/backward.svg");
    f.open(QIODevice::ReadOnly);
    d = f.readAll();
    f.close();

    toRet<<tmp;
    tmp.kumirHeader=trUtf8("алг остановить моторы(лог A,лог B, лог C)");
    f.setFileName(":/pictomir_icons/actions/backward.svg");
    f.open(QIODevice::ReadOnly);
    d = f.readAll();
    f.close();



    //tmp.iconSvgData = d;
    //tmp.stepsCount=5;
    toRet<<tmp;
    tmp.kumirHeader=trUtf8("алг вправо(цел угол_90)");
    f.setFileName(":/pictomir_icons/actions/turn_right.svg");
    f.open(QIODevice::ReadOnly);
    d = f.readAll();
    f.close();
    //tmp.iconSvgData = d;
    //tmp.stepsCount=3;
    toRet<<tmp;

    tmp.kumirHeader=trUtf8("алг влево(цел угол_90)");
    f.setFileName(":/pictomir_icons/actions/turn_left.svg");
    f.open(QIODevice::ReadOnly);
    d = f.readAll();
    f.close();
    //tmp.iconSvgData = d;
    //tmp.stepsCount=3;
    toRet<<tmp;


    tmp.kumirHeader=trUtf8("алг ждать(вещ секунд)");
    f.setFileName(":/pictomir_icons/actions/turn_left.svg");
    f.open(QIODevice::ReadOnly);
    d = f.readAll();
    f.close();
    //tmp.iconSvgData = d;
    //tmp.stepsCount=3;
    toRet<<tmp;

    //tmp.kumirHeader=trUtf8("алг ждать(цел время)");
    //toRet<<tmp;



    tmp.kumirHeader=trUtf8("алг лог стена");
    f.setFileName(":/pictomir_icons/conditions/wall.svg");
    f.open(QIODevice::ReadOnly);
    d = f.readAll();
    f.close();
    //tmp.iconSvgData = d;
    //tmp.stepsCount=1;
    toRet<<tmp;

    tmp.kumirHeader=trUtf8("алг цел расстояние");
    toRet<<tmp;


    tmp.kumirHeader=trUtf8("алг лог тык");
    toRet<<tmp;

    tmp.kumirHeader=trUtf8("алг забыть тык");
    toRet<<tmp;

    tmp.kumirHeader=trUtf8("алг цел свет");

    toRet<<tmp;

    tmp.kumirHeader=trUtf8("алг цел уголА");
    toRet<<tmp;

    tmp.kumirHeader=trUtf8("алг цел уголВ");
    toRet<<tmp;

    tmp.kumirHeader=trUtf8("алг цел уголС");
    toRet<<tmp;

    tmp.kumirHeader=trUtf8("алг сбросС");
    toRet<<tmp;
    tmp.kumirHeader=trUtf8("алг сбросА");
    toRet<<tmp;
    tmp.kumirHeader=trUtf8("алг сбросВ");
    toRet<<tmp;

    tmp.kumirHeader=trUtf8("алг вещ таймер(цел номер)");
    toRet<<tmp;

    tmp.kumirHeader=trUtf8("алг сбросить таймер(цел номер)");
    toRet<<tmp;

    tmp.kumirHeader=trUtf8("алг моторС(цел скорость)");
    toRet<<tmp;
    tmp.kumirHeader=trUtf8("алг моторА(цел скорость)");
    toRet<<tmp;
    tmp.kumirHeader=trUtf8("алг моторВ(цел скорость)");
    toRet<<tmp;




    tmp.kumirHeader=trUtf8("алг motorC(арг цел Время,арг цел Скорость)");
    //tmp.iconSvgData = d;
    //tmp.stepsCount=1;
    toRet<<tmp;
    tmp.kumirHeader=trUtf8("алг звук");
    f.setFileName(":/pictomir_icons/actions/sound.svg");
    f.open(QIODevice::ReadOnly);
    d = f.readAll();
    f.close();
    //tmp.iconSvgData = d;
    //tmp.stepsCount=1;
    toRet<<tmp;
    return toRet;
};
QList<QVariant> TurtleStarter::algOptResults()
{
    QList<QVariant> tmp;
    tmp<<QVariant("NO OPT RESULTS");
    return tmp;
};

void TurtleStarter::runAlg(QString alg,QList<QVariant> params)
{
    skipSync=false;
    errortext="";
    if(!mw->NXTConnected()) //NET NXT
    {
        errortext=QString(trUtf8("Нет соединения с NXT"));
        sync();
    };
    if(alg==QString::fromUtf8("вперед"))
    {
        //qDebug()<<"Vpered";
        if(params.count()<1)
        {
            errortext="No argument.";sync();return ;
        };
        mw->step=params[0].toInt();
        if(mw->moveT()){return ;}
        else{ errortext=QString::fromUtf8("Край пустыни");sync();
            return ;};
    };
    if(alg==QString::fromUtf8("назад"))
    {

        if(params.count()<1)
        {
            errortext="No argument.\n";sync();return ;
        };
        mw->step=-params[0].toInt();
        if(mw->moveT()){qDebug()<<"Nazad! form iface"<<" Step"<<mw->step;return ;}
        else{ errortext=QString::fromUtf8("Край пустыни");sync();
            return ;};

    };



    if(alg==QString::fromUtf8("скорость А") ||alg==QString::fromUtf8("скорость A"))
    {

        if(params.count()<1)
        {
            errortext="No argument.\n";sync();return ;
        };
        mw->MotorAS=params[0].toInt();
       sync();
            return ;

    }

    if(alg==QString::fromUtf8("скорость B") ||alg==QString::fromUtf8("скорость В"))
{

    if(params.count()<1)
    {
        errortext="No argument.\n";sync();return ;
    };
    mw->MotorBS=params[0].toInt();
   sync();
        return ;

};
    if(alg==QString::fromUtf8("скорость C") ||alg==QString::fromUtf8("скорость С"))
{

    if(params.count()<1)
    {
        errortext="No argument.\n";sync();return ;
    };
    mw->MotorCS=params[0].toInt();
   sync();
        return ;

};

    if(alg==QString::fromUtf8("моторА"))
    {

        if(params.count()<1)
        {
            errortext="No argument.\n";sync();return ;
        };
        int speed=params[0].toInt();
        if(speed>100 || speed<-100)
        {
            errortext=trUtf8("Недопустимое значение.\n");sync();return ;
        }
        if(speed!=0)
        {mw->MotorAS=speed;
            mw->runEngine(true,false,false);
           sync();
        }else
        {
            mw->stopEngine(true,false,false);

        }
       sync();
            return ;

    }


    if(alg==QString::fromUtf8("моторВ"))
    {

        if(params.count()<1)
        {
            errortext="No argument.\n";sync();return ;
        };
        int speed=params[0].toInt();
        if(speed>100 || speed<-100)
        {
            errortext=trUtf8("Недопустимое значение.\n");sync();return ;
        }
        if(speed!=0)
        {mw->MotorBS=speed;
            mw->runEngine(false,true,false);
           sync();
        }else
        {
            mw->stopEngine(false,true,false);

        }
       sync();
            return ;

    }
    if(alg==QString::fromUtf8("моторС"))
    {

        if(params.count()<1)
        {
            errortext="No argument.\n";sync();return ;
        };
        int speed=params[0].toInt();
        if(speed>100 || speed<-100)
        {
            errortext=trUtf8("Недопустимое значение.\n");sync();return ;
        }
        if(speed!=0)
        {mw->MotorCS=speed;
            mw->runEngine(false,false,true);
           sync();
        }else
        {
            mw->stopEngine(false,false,true);

        }
       sync();
            return ;

    }


    if(alg==QString::fromUtf8("запустить моторы") )
{

    if(params.count()<3)
    {
        errortext="No argument.\n";sync();return ;
    };
    mw->runEngine(params[0].toBool(),params[1].toBool(),params[2].toBool());
   sync();
        return ;

};
    if(alg==QString::fromUtf8("остановить моторы") )
{

    if(params.count()<3)
    {
        errortext="No argument.\n";sync();return ;
    };
    mw->stopEngine(params[0].toBool(),params[1].toBool(),params[2].toBool());
    qDebug()<<"STOP ENGINE";
   sync();
        return ;

};


    if(alg==QString::fromUtf8("вправо"))
    {
        if(params.count()<1)
        {
            errortext="No argument.";sync();return;
        };
        mw->DoRotate(params[0].toInt());
        return ;
    };
    if(alg==QString::fromUtf8("влево"))
    {

        if(params.count()<1)
        {
            errortext="No argument.";sync();return ;
        };
        mw->DoRotate(-params[0].toInt());
        return ;
    };
    if(alg==QString::fromUtf8("ждать"))
    {

        if(params.count()<1)
        {
            errortext="No argument.";sync();return ;
        };
        mw->DoSleep(params[0].toFloat()*1000);
        return ;
    };


    if(alg==QString::fromUtf8("таймер"))
    {

        if(params.count()<1)
        {
            errortext="No argument.";sync();return ;
        };
        int timerId=params[0].toInt();
        if(timerId<1 || timerId>4)
        {
            errortext=trUtf8("Неверный номер таймера.");sync();return ;
        };

        replay=(double)timers[timerId-1].elapsed ()/1000;
        qDebug()<<"Timer replay"<<replay;
        sync();
        return ;
    };
    if(alg==QString::fromUtf8("сбросить таймер"))
    {

        if(params.count()<1)
        {
            errortext="No argument.";sync();return ;
        };
        int timerId=params[0].toInt();
        if(timerId<1 || timerId>4)
        {
            errortext=trUtf8("Неверный номер таймера.");sync();return ;
        };

        timers[timerId-1].restart();
        sync();
        return ;
    };


    if(alg==QString::fromUtf8("motorC"))
    {
        if(params.count()<2)
        {
            errortext="No argument.";sync();return;
        };
        mw->DoMotorC(params[0].toInt(),params[1].toInt());
    };

    if(alg==QString::fromUtf8("стена"))
    {
        mw->CheckForWall();

        return;
    };
    if(alg==QString::fromUtf8("звук"))
    {
        mw->beep();
        return;
    };

    if(alg==QString::fromUtf8("тык"))
    {
        mw->touch();
        return;
    };


    if(alg==QString::fromUtf8("|open port"))
    {
        int port=params[0].toInt();
        t_pult->libMode=false;
        if(port<1024) return;
        openServerPort(port);
    };

    if(alg==QString::fromUtf8("расстояние"))
    {
        qDebug()<<"get range!";
       mw->range();
    };
    if(alg==QString::fromUtf8("свет"))
    {
       mw->light();
    };
    if(alg==QString::fromUtf8("уголА"))
    {
       mw->rotation(1);
    };
    if(alg==QString::fromUtf8("уголВ"))
    {
       mw->rotation(2);
    };
    if(alg==QString::fromUtf8("уголС"))
    {
       mw->rotation(3);
    };
    if(alg==QString::fromUtf8("сбросС"))
    {
        mw->resetRot(3);
    };
    if(alg==QString::fromUtf8("сбросА"))
    {
       mw->resetRot(1);
    };
    if(alg==QString::fromUtf8("сбросВ"))
    {
       mw->resetRot(2);

    };

    return;


};
void TurtleStarter::openServerPort(int port)
{
    t_pult->libMode=false;
    if(!server->OpenPort("localhost",port))
    {
        QMessageBox::critical(NULL, QString::fromUtf8("Ошибка открытия порта"),
                              QString::fromUtf8("Невозможно открыть порт %1")
                              .arg(port));
    }else
    {
        t_pult->showMessage(QString::fromUtf8("Открыт порт %1").arg(port));
    };

};

QVariant TurtleStarter::result(){return replay;};

void TurtleStarter::setMode(int che_mode)
{
    mode=che_mode;

    if(mode==1)t_pult->noLink();
    else t_pult->LinkOK();

};
void TurtleStarter::connectSignalSync( const QObject *obj, const char *method)
{connect(this,SIGNAL(sync()),obj,method);};

void TurtleStarter::connectSignalSendText( const QObject *obj, const char *method ) 
{connect(this,SIGNAL(sendText(QString)),obj,method);};

void TurtleStarter::sendText2Kumir(QString text)
{
    emit sendText(text);
};


Q_EXPORT_PLUGIN2(turtleStarter, TurtleStarter)
