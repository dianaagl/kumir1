#include "nxtthread.h"
#include "nxtmotor.h"
#include "nxtsensor.h"
#include "nxtbrick.h"

#define BASE_MOTOR 33
#define QUANTUM_TIME 10 // in milliseconds
#define SENSOR_THRESHOLD_TIME 100 // in milliseconds
#define SOUND_TIME 100 // in milliseconds

NXTThread::NXTThread(QObject *parent,
                     NxtMotor *A, NxtMotor *B, NxtMotor *C,
                     NxtSensor *sensor,NxtSonar *sonar,NxtLight* light,
                     NxtBrick *brick
                     ):
    QThread(parent)
{
    nxt_motorA = A;
    nxt_motorB = B;
    nxt_motorC = C;
    nxt_touchSensor = sensor;
    nxt_brick = brick;
    nxt_sonar=sonar;
    nxt_light=light;
    was_touch=false;
    mutex_motorA = new QMutex;
    mutex_motorB = new QMutex;
    mutex_motorC = new QMutex;
    mutex_sensor = new QMutex;
    mutex_brick = new QMutex;
    mutex_stop = new QMutex;
    mutex_error = new QMutex;
    mutex_executing = new QMutex;

    b_stop = false;

    i_speedA = i_speedB = i_speedC = 85;
    r_scaleDistanceA = r_scaleDistanceB = 1.0;
    r_scaleTurningA = r_scaleTurningB = 1.0;

    b_rec_resetA=false;
    b_rec_resetB=false;
    b_rec_resetC=false;
    reset();

    i_sensorRequests = 0;
    i_sensorSignals = 0;

}

void NXTThread::reset()
{
mutex_executing->lock();
    b_ignoreSensor = false;
    i_timeLeftA = i_timeLeftB = i_timeLeftC = 0;
    b_sensorState = false;
    b_requestSensor = false;
    i_sensorTimer = 0;
    b_executing = false;
    s_last = "";
    i_brickTimer = 0;
    wait_timer=0;
    was_touch=false;
    b_requestRotA=false;
    b_requestRotB=false;
    b_requestRotC=false;
    qDebug()<<"reset!";
    mutex_executing->unlock();
}

void NXTThread::requestPenMotorStart(int speed, unsigned int time_ms)
{
    mutex_executing->lock();
    if (b_executing) {
        qDebug() << "Request pen motor while executing last command: " << s_last;
        mutex_executing->unlock();
        return;
    }
    else {
        b_executing = true;
        s_last = "MOTOR_C START";
        qDebug()<<"PenMOtorSTART!";
        mutex_executing->unlock();
    }

    mutex_motorC->lock();
    i_speedC = speed;
    i_timeLeftC = time_ms;
    mutex_motorC->unlock();
}


void NXTThread::requestMotorStart(int speeda,int speedb,int speedc,bool a,bool b,bool c)
{
    mutex_executing->lock();
    if (b_executing) {
        qDebug() << "Request pen motor while executing last command: " << s_last;
        mutex_executing->unlock();
        return;
    }
    else {
       // b_executing = true;
        s_last = "MOTORS START";
        mutex_executing->unlock();
    }
    if(a)
    {


    mutex_motorA->lock();
    i_speedA = speeda;
    i_timeLeftA = 64000;
    mutex_motorA->unlock();
    }

    if(b)
    {
    mutex_motorB->lock();
    i_speedB = speedb;
    i_timeLeftB = 64000;
    mutex_motorB->unlock();
    }


    if(c)
    {
    mutex_motorC->lock();
    i_speedC = speedc;
    i_timeLeftC = 64000;
    mutex_motorC->unlock();
    }
    b_executing=false;
    mutex_executing->unlock();
}


void NXTThread::requestMotorStop(bool a,bool b,bool c)
{
    mutex_executing->lock();
    if (b_executing) {
        qDebug() << "Request pen motor while executing last command: " << s_last;
        mutex_executing->unlock();
        return;
    }
    else {
        b_executing = true;
        s_last = "MOTOR_ STOP";
        mutex_executing->unlock();
    }
    qDebug()<<"NXTCore::stopMotors"<<a<<b<<c;
    if(a)
    {
    mutex_motorA->lock();
    //nxt_motorA->stop();
    QString err;
    //qDebug()<<"MotorA new STOP:"<<nxt_motorA->rotationCount(err)<<" "<<err;
    i_timeLeftA = 0;
   //  qDebug()<<"MotorA new:"<<nxt_motorA->rotationCount(err)<<" "<<err;
    mutex_motorA->unlock();
    //qDebug()<<"MotorA new2:"<<nxt_motorA->rotationCount(err)<<" "<<err;
    }

    if(b)
    {
    mutex_motorB->lock();
    //nxt_motorB->stop();
    i_timeLeftB = 0;
    qDebug()<<"MotorB STOP";
    mutex_motorB->unlock();
    }


    if(c)
    {
    mutex_motorC->lock();
    //nxt_motorC->stop();
    i_timeLeftC = 0;
    mutex_motorC->unlock();
    }
     b_executing=false;
    mutex_executing->unlock();
}


void NXTThread::requestMotorsGoForward(unsigned int distance)
{
    mutex_executing->lock();
    if (b_executing) {
        qDebug() << "Request go forward while executing last command: " << s_last;
        mutex_executing->unlock();
        return;
    }
    else {
        b_executing = true;
        s_last = "FORWARD";
        mutex_executing->unlock();
    }
    mutex_motorA->lock();
    mutex_motorB->lock();

    if (i_timeLeftA>0 || i_timeLeftB>0) {
        qDebug() << "Request go forward while motors are running";
    }
    else {
        b_ignoreSensor = false;
        qreal fLeftTimeToWork = r_scaleDistanceA * distance;
        qreal fRightTimeToWork = r_scaleDistanceB * (distance);
        Q_ASSERT ( fLeftTimeToWork >= 0.0 );
        Q_ASSERT ( fRightTimeToWork >= 0.0 );
        i_timeLeftA = fLeftTimeToWork;
        i_timeLeftB = fRightTimeToWork;
        if(distance<10)
        {
            i_speedA = 30;
            i_speedB = 30;
            i_timeLeftA *= 2.5;
            i_timeLeftB *= 2.5;
        }else
        if (distance<22) {
            i_speedA = 40;
            i_speedB = 40;
            i_timeLeftA *= 2.0;
            i_timeLeftB *= 2.0;
            qDebug()<<"TEST SET"<<i_timeLeftA;
        }
        else if (distance<45){
            i_speedA = 54;
            i_speedB = 54;
            i_timeLeftA *= 1.35;
            i_timeLeftB *= 1.35;
        }else
        {
            i_speedA = 85;
            i_speedB = 83;
            i_timeLeftA *= 1;
            i_timeLeftB *= 1;
        };
    }

    mutex_motorA->unlock();
    mutex_motorB->unlock();
}

void NXTThread::requestMotorsGoBackward(unsigned int distance)
{
    mutex_executing->lock();
    if (b_executing) {
        qDebug() << "Request go backward while executing last command: " << s_last;
        mutex_executing->unlock();
        return;
    }
    else {
        b_executing = true;
        s_last = "BACKWARD";
        mutex_executing->unlock();
    }
    mutex_motorA->lock();
    mutex_motorB->lock();

    if (i_timeLeftA>0 || i_timeLeftB>0) {
        qDebug() << "Request go backward while motors are running";
    }
    else {
        b_ignoreSensor = true;
        qreal fLeftTimeToWork = r_scaleDistanceA * distance;
        qreal fRightTimeToWork = r_scaleDistanceB * (distance);
        Q_ASSERT ( fLeftTimeToWork >= 0.0 );
        Q_ASSERT ( fRightTimeToWork >= 0.0 );
        i_timeLeftA = fLeftTimeToWork;
        i_timeLeftB = fRightTimeToWork;
             if(distance<10)
        {
            i_speedA = -30;
            i_speedB = -30;
            i_timeLeftA *= 2.5;
            i_timeLeftB *= 2.5;
        }else
        if (distance<22) {
            i_speedA = -40;
            i_speedB = -40;
            i_timeLeftA *= 2.0;
            i_timeLeftB *= 2.0;
            qDebug()<<"TEST SET"<<i_timeLeftA;
        }
        else if (distance<45){
            i_speedA =- 54;
            i_speedB =-54;
            i_timeLeftA *= 1.35;
            i_timeLeftB *= 1.35;
        }else
        {
            i_speedA = -85;
            i_speedB = -83;
            i_timeLeftA *= 1;
            i_timeLeftB *= 1;
        };
    }

    mutex_motorA->unlock();
    mutex_motorB->unlock();
}


void NXTThread::requestMotorsTurnLeft(unsigned int degrees)
{
    mutex_executing->lock();
    if (b_executing) {
        qDebug() << "Request turn left while executing last command: " << s_last;
        mutex_executing->unlock();
        return;
    }
    else {
        b_executing = true;
        s_last = "LEFT";
        mutex_executing->unlock();
    }
    mutex_motorA->lock();
    mutex_motorB->lock();

    if (i_timeLeftA>0 || i_timeLeftB>0) {
        qDebug() << "Request turn left while motors are running";
    }
    else {
        b_ignoreSensor = true;
        qreal leftMultiplay = r_scaleTurningA/BASE_MOTOR;
        qreal rightMultiplay = r_scaleTurningB/BASE_MOTOR;

        i_speedA = -50;
        i_speedB = 50;

        if (degrees<=45) {
        i_speedA = -22;
        i_speedB = 22;
            leftMultiplay *= 2.75;
            rightMultiplay *= 2.75;
        }
        else if (degrees<=90) {
            leftMultiplay *= 1.8;
            rightMultiplay *= 1.8;
        }
        else if (degrees<=180) {
            i_speedA = -75;
            i_speedB = 75;
            leftMultiplay *= 1.05;
            rightMultiplay *= 1.05;
        }
        else {
            i_speedA = -75;
            i_speedB = 75;
            leftMultiplay *= 0.9;
            rightMultiplay *= 0.9;
        }

        qreal fLeftTimeToWork = r_scaleTurningA * degrees * leftMultiplay;
        qreal fRightTimeToWork = r_scaleTurningB * degrees * rightMultiplay;

        Q_ASSERT ( fLeftTimeToWork >= 0.0 );
        Q_ASSERT ( fRightTimeToWork >= 0.0 );

        i_timeLeftA = fLeftTimeToWork;
        i_timeLeftB = fRightTimeToWork;
    }

    mutex_motorA->unlock();
    mutex_motorB->unlock();
}

void NXTThread::requestMotorsTurnRight(unsigned int degrees)
{
    mutex_executing->lock();
    if (b_executing) {
        qDebug() << "Request turn right while executing last command: " << s_last;
        mutex_executing->unlock();
        return;
    }
    else {
        b_executing = true;
        s_last = "RIGHT";
        mutex_executing->unlock();
    }
    mutex_motorA->lock();
    mutex_motorB->lock();

    if (i_timeLeftA>0 || i_timeLeftB>0) {
        qDebug() << "Request turn right while motors are running";
    }
    else {
        b_ignoreSensor = true;
        qreal leftMultiplay = r_scaleTurningA/BASE_MOTOR;
        qreal rightMultiplay = r_scaleTurningB/BASE_MOTOR;

        i_speedA = 50;
        i_speedB = -50;

        if (degrees<=45) {
       i_speedA = 22;
        i_speedB = -22;
            leftMultiplay *= 2.75;
            rightMultiplay *= 2.75;
        }
        else if (degrees<=90) {
            leftMultiplay *= 1.8;
            rightMultiplay *= 1.8;
        }
        else if (degrees<=180) {
            i_speedA = 75;
            i_speedB = -75;
            leftMultiplay *= 1.05;
            rightMultiplay *= 1.05;
        }
        else {
            i_speedA = 75;
            i_speedB = -75;
            leftMultiplay *= 0.9;
            rightMultiplay *= 0.9;
        }

        qreal fLeftTimeToWork = r_scaleTurningA * degrees * leftMultiplay;
        qreal fRightTimeToWork = r_scaleTurningB * degrees * rightMultiplay;

        Q_ASSERT ( fLeftTimeToWork >= 0.0 );
        Q_ASSERT ( fRightTimeToWork >= 0.0 );

        i_timeLeftA = fLeftTimeToWork;
        i_timeLeftB = fRightTimeToWork;
    }

    mutex_motorA->unlock();
    mutex_motorB->unlock();
}

void NXTThread::requestSound()
{
    mutex_executing->lock();
    if (b_executing) {
        qDebug() << "Request sound while executing last command: " << s_last;
        mutex_executing->unlock();
        return;
    }
    else {
        b_executing = true;
        s_last = "SOUND";
        mutex_executing->unlock();
    }
    mutex_brick->lock();
    if (i_brickTimer>0) {
        qDebug() << "Request sound while sound playing";
    }
    else {
        i_brickTimer = SOUND_TIME;
    }
    mutex_brick->unlock();
}

void NXTThread::requestSensorState()
{
    mutex_executing->lock();
    if (b_executing) {
        qDebug() << "Request sensor state while executing last command: " << s_last;
        mutex_executing->unlock();
        return;
    }
    else {
        b_executing = true;
        s_last = "SENSOR";
        mutex_executing->unlock();
    }
    mutex_sensor->lock();
    QString err;
    nxt_sonar->range(err);
    qDebug()<<err;
    b_requestSensor = true;
    i_sensorRequests++;
    qDebug() << "Sensor requests count: " << i_sensorRequests;
    mutex_sensor->unlock();
}

 void NXTThread::requestRot(int motor_id)
 {
     mutex_executing->lock();
     if (b_executing) {
         qDebug() << "Request rotation state while executing last command: " << s_last;
         mutex_executing->unlock();
         return;
     }
     else {
         b_executing = true;
         s_last = "ROTATION";
         mutex_executing->unlock();
     }

     if(motor_id==1)
     {
          mutex_motorA->lock();
          b_requestRotA=true;
          mutex_motorA->unlock();
     }

     if(motor_id==2)
     {
          mutex_motorB->lock();
          b_requestRotB=true;
          mutex_motorB->unlock();
     }
     if(motor_id==3)
     {
          mutex_motorC->lock();
          b_requestRotC=true;
          mutex_motorC->unlock();
     }

 };

 void NXTThread::sleep_msc(int msecs)
 {
     mutex_executing->lock();
     if (b_executing) {
         qDebug() << "Request rotation state while executing last command: " << s_last;
         mutex_executing->unlock();
         return;
     }
     else {
         b_executing = true;
         s_last = "WAIT";
         time.restart();
         wait_timer=msecs;
         mutex_executing->unlock();
     }




 }

 void NXTThread::resetRot(int motor_id)
 {
     mutex_executing->lock();
     if (b_executing) {
         qDebug() << "Request rotation state while executing last command: " << s_last;
         mutex_executing->unlock();
         return;
     }
     else {
         b_executing = true;
         s_last = "ROTATION_RESET";
         mutex_executing->unlock();
     }

     if(motor_id==1)
     {
          mutex_motorA->lock();
          //nxt_motorA->reset();
          b_rec_resetA=true;
          mutex_motorA->unlock();
     }

     if(motor_id==2)
     {
          mutex_motorC->lock();
          //b_requestRotC=true;
          b_rec_resetB=true;
          mutex_motorC->unlock();
     }
     if(motor_id==3)
     {
          mutex_motorC->lock();
          b_rec_resetC=true;
          mutex_motorC->unlock();
     }


 };

 void NXTThread::checkTouch()
 {
     mutex_executing->lock();
     if (b_executing) {
         qDebug() << "Request touch sensor state while executing last command: " << s_last;
         mutex_executing->unlock();
         return;
     }
     else {
         b_executing = true;
         s_last = "Touch SENSOR";
         mutex_executing->unlock();
     }
     mutex_sensor->lock();
     qDebug()<<"CHECK TOUCH!!!"<<was_touch;
     mutex_executing->lock();
     b_executing = false;
     s_last = "";

     mutex_executing->unlock();
     emit commandFinished("", was_touch);
     mutex_sensor->unlock();
 };

void NXTThread::resetTouch()
{

    mutex_sensor->lock();
    was_touch=false;
    mutex_sensor->unlock();
};

void NXTThread::requestSonar()
{
    mutex_executing->lock();
    if (b_executing) {
        qDebug() << "Request sensor state while executing last command: " << s_last;
        mutex_executing->unlock();
        return;
    }
    else {
        b_executing = true;
        s_last = "SENSOR";
        mutex_executing->unlock();
    }
    mutex_sensor->lock();
    QString err;
    msleep(20);
    //int value=nxt_sonar->range(err);
    //qDebug()<<err<<"value"<<value;
    b_requestSonar = true;
    i_sensorRequests++;
    qDebug() << "Sensor requests count: " << i_sensorRequests;
    mutex_sensor->unlock();
}

void NXTThread::requestLight()
{
    mutex_executing->lock();
    if (b_executing) {
        qDebug() << "Request sensor state while executing last command: " << s_last;
        mutex_executing->unlock();
        return;
    }
    else {
        b_executing = true;
        s_last = "LIGHT_SENSOR";
        mutex_executing->unlock();
    }
    mutex_sensor->lock();
    QString err;
    //nxt_sonar->range(err);
    //int value=nxt_light->value(err);
    //qDebug()<<"LIGHT:"<<value;
    //qDebug()<<err;
    b_requestLight = true;
    b_requestRotA=false;
    b_requestRotB=false;
    i_sensorRequests++;
    //qDebug() << "Sensor requests count: " << i_sensorRequests;
    //emit commandFinished(err
      //                  , value);
    mutex_sensor->unlock();
}


NXTThread::~NXTThread()
{
    stopThread();
}

void NXTThread::stopThread()
{
    mutex_stop->lock();
    b_stop = true;
    mutex_stop->unlock();
}

void NXTThread::run()
{
    bool processThisQuantum;
    bool motorARunning;
    bool motorBRunning;
    bool motorAStopped;
    bool motorBStopped;
    bool motorCStopped;
    bool sensorPressed;
    bool sensorRequest;
    bool  sonarRequest;
    bool  lightRequest;
    int rotation;
    int Lcounter=0;
    int zerro_check=0;
    wait_timer=0;
    QString sensorError;
    QString signalStringValue;
    bool signalBoolValue;
    forever {
        motorAStopped = false;
        motorBStopped = false;
        motorCStopped = false;
        sensorPressed = false;
        sensorError = "";
        mutex_error->lock();
        s_error = "";
        mutex_error->unlock();

        // --- Check for thread termination

        mutex_stop->lock();
        processThisQuantum = !b_stop;
        mutex_stop->unlock();
        if (!processThisQuantum) {
            qDebug() << "Exiting NXT thread";
            break;
        }

        // --- Check motor A
         QString err;
        mutex_motorA->lock();
        motorARunning = nxt_motorA->isRunning();

        if(b_requestRotA)
        {   msleep(10);
            rotation=nxt_motorA->rotationCount(err);
            if(rotation/10000==0)zerro_check++;

            if(rotation/10000>0 || zerro_check>5)
            {
                mutex_executing->lock();
                b_executing = false;
                s_last = "";
                b_requestRotA=false;
                zerro_check=0;
                mutex_executing->unlock();

                emit commandFinished(err, rotation/10000);
            };
            };

        if(b_rec_resetA)
        {
            msleep(10);
            nxt_motorA->reset();
            mutex_executing->lock();
            b_executing = false;
            s_last = "";
            b_rec_resetA=false;
            mutex_executing->unlock();
            emit commandFinished("", "");
        }
        if (i_timeLeftA>0) {
           // if (!motorARunning) {
                // Start motor A if not running and should work
                mutex_error->lock();
                s_error += nxt_motorA->on(i_speedA, b_ignoreSensor);
                mutex_error->unlock();
            //}
        }
        else if (i_timeLeftA<=0) {
            if (motorARunning) {
                // Stop motor A if running and should not work
                mutex_error->lock();
                s_error += nxt_motorA->stop();
                mutex_error->unlock();
                motorAStopped = true;
            }
        }

        mutex_motorA->unlock();

        // --- Check motor B

        mutex_motorB->lock();
        motorBRunning = nxt_motorB->isRunning();

        if(b_requestRotB)
        {msleep(10);
            rotation=nxt_motorB->rotationCount(err);

            if(rotation/10000==0)zerro_check++;

            if(rotation/10000>0 || zerro_check>5)
            {
                mutex_executing->lock();
                b_executing = false;
                s_last = "";
                b_requestRotB=false;
                zerro_check=0;
                mutex_executing->unlock();

                emit commandFinished(err, rotation/10000);
            };



        qDebug()<<"Rotation last:"<<rotation/10000;};
        if(b_rec_resetB)
        {
            msleep(20);
            nxt_motorB->reset();
            mutex_executing->lock();
            b_executing = false;
            s_last = "";
            b_rec_resetB=false;
            mutex_executing->unlock();
            emit commandFinished("", "");
        }

        if (i_timeLeftB>0) {
            //if (!motorBRunning) {
                // Start motor B if not running and should work
                mutex_error->lock();
                s_error += nxt_motorB->on(i_speedB, b_ignoreSensor);
                mutex_error->unlock();
            //}
        }
        else if (i_timeLeftB<=0) {
            if (motorBRunning) {
                // Stop motor B if running and should not work
                mutex_error->lock();
                s_error += nxt_motorB->stop();
                mutex_error->unlock();
                motorBStopped = true;
            }
        }

        mutex_motorB->unlock();

        // --- Check motor C

        mutex_motorC->lock();

        if(b_requestRotC)
        {rotation=nxt_motorC->rotationCount(err);

          mutex_executing->lock();
            b_executing = false;
            s_last = "";
            b_requestRotC=false;
            mutex_executing->unlock();
            emit commandFinished(err, rotation/10000);



        qDebug()<<"Rotation last:"<<rotation/10000;};
        if(b_rec_resetC)
        {
            msleep(10);
            nxt_motorC->reset();
            mutex_executing->lock();
            b_executing = false;
            s_last = "";
            b_rec_resetC=false;
            mutex_executing->unlock();
            emit commandFinished("", "");
        }


        if (i_timeLeftC>0 && !nxt_motorC->isRunning()) {
            // Start motor C if not running and should work
            mutex_error->lock();
            s_error += nxt_motorC->on(i_speedC, b_ignoreSensor);
            mutex_error->unlock();
        }
        else if (i_timeLeftC<=0 && nxt_motorC->isRunning()) {
            // Stop motor C if running and should not work
            mutex_error->lock();
            s_error += nxt_motorC->stop();
            mutex_error->unlock();
            motorCStopped = true;
        }

        mutex_motorC->unlock();

        // --- Check sensor

        mutex_sensor->lock();
        mutex_error->lock();
        sensorPressed = nxt_touchSensor->isTouched(sensorError);
        s_error += sensorError;
        mutex_error->unlock();

        if (sensorPressed) {
            i_sensorTimer = SENSOR_THRESHOLD_TIME + QUANTUM_TIME;
            b_sensorState = true;
            // Stop running motors if need
            if (!nxt_motorA->isIgnoringSensor() && nxt_motorA->isRunning()) {
                mutex_motorA->lock();
                mutex_error->lock();
                s_error += nxt_motorA->stop();
                motorAStopped = true;
                i_timeLeftA = 0;
                mutex_error->unlock();
                mutex_motorA->unlock();
            }
            if (!nxt_motorB->isIgnoringSensor() && nxt_motorB->isRunning()) {
                mutex_motorB->lock();
                mutex_error->lock();
                s_error += nxt_motorB->stop();
                i_timeLeftB = 0;
                motorBStopped = true;

                mutex_error->unlock();
                mutex_motorB->unlock();
            }
            if (!nxt_motorC->isIgnoringSensor() && nxt_motorC->isRunning()) {
                mutex_motorC->lock();
                mutex_error->lock();
                s_error += nxt_motorC->stop();
                mutex_error->unlock();
                mutex_motorC->unlock();
            }
        }

        mutex_sensor->unlock();

        // --- Check brick

        mutex_brick->lock();

        if (i_brickTimer==SOUND_TIME) {
            mutex_error->lock();
            s_error += nxt_brick->playSound();
            mutex_error->unlock();
        }


        mutex_brick->unlock();

        // --- Decrease time intervals and emit signals if need

        if (i_timeLeftA>0) {
            i_timeLeftA -= QUANTUM_TIME;
        }

        if (i_timeLeftB>0) {
            i_timeLeftB -= QUANTUM_TIME;
        }

        if (i_timeLeftC>0) {
            i_timeLeftC -= QUANTUM_TIME;
        }

        if (motorAStopped || motorBStopped) {
            // This quantum leads to one (or both) motor to stop.
            // Thus we should check for motors, and
            // if both are stopped then emit signal
            if (!nxt_motorA->isRunning() && !nxt_motorB->isRunning()) {
                mutex_error->lock();
                signalStringValue = s_error;
                s_error = "";
                mutex_error->unlock();
                qDebug() << "Motors are stopped, sending sync signal...";
                mutex_executing->lock();
                b_executing = false;
                s_last = "";
                mutex_executing->unlock();
                emit commandFinished(signalStringValue, false);
            }
        }

        if (motorCStopped) {
            mutex_error->lock();
            signalStringValue = s_error;
            s_error = "";
            mutex_error->unlock();
            qDebug() << "Pencil motor stopped, sending sync signal...";
            mutex_executing->lock();
            b_executing = false;
            s_last = "";
            mutex_executing->unlock();
            emit commandFinished(signalStringValue, false);
        }

        if (i_brickTimer>0) {
            i_brickTimer -= QUANTUM_TIME;
            if (i_brickTimer<=0) {
                // Brick has done with sound
                mutex_error->lock();
                signalStringValue = s_error;
                s_error = "";
                mutex_error->unlock();
                qDebug() << "Brick has done with sound, sending sync signal...";
                mutex_executing->lock();
                b_executing = false;
                s_last = "";
                mutex_executing->unlock();
                emit commandFinished(signalStringValue, false);
            }
        }

        if (i_sensorTimer>0) {
            i_sensorTimer -= QUANTUM_TIME;
        }
        else {
            if (!sensorPressed) {
                // After threshold we should set sensor
                // value to false if not pressed again
                mutex_sensor->lock();
                b_sensorState = false;
                mutex_sensor->unlock();
            }
        }

        // --- Check for sensor request

        mutex_sensor->lock();
        sensorRequest = b_requestSensor;
        sonarRequest=b_requestSonar;
        lightRequest=b_requestLight;
        mutex_sensor->unlock();

        Lcounter-=QUANTUM_TIME;
        if(Lcounter<0)
        {
        Lcounter=4*QUANTUM_TIME;
        mutex_sensor->lock();
        signalBoolValue = b_sensorState;
        if(signalBoolValue)was_touch=signalBoolValue;
       // qDebug()<<"TOUCH!"<<was_touch;
        mutex_sensor->unlock();

        }
        if (sensorRequest) {
            mutex_sensor->lock();
            signalBoolValue = b_sensorState;
            b_requestSensor = false;
            mutex_sensor->unlock();
            mutex_error->lock();
            signalStringValue = s_error;
            s_error = "";
            mutex_error->unlock();
            i_sensorSignals++;
            qDebug() << "Sensor signals count: " << i_sensorSignals;
            qDebug() << "Sensor request finished, sending sync signal with value: " <<signalBoolValue<<" ...";
            mutex_executing->lock();
            b_executing = false;
            s_last = "";
            mutex_executing->unlock();
            emit commandFinished(signalStringValue, signalBoolValue);
        }

        if (b_requestSonar) {
            mutex_sensor->lock();
           int sensorRange= nxt_sonar->range(signalStringValue);
           if(sensorRange<2)zerro_check++;
           qDebug()<<"Sonar"<<sensorRange;
           // signalBoolValue = b_sensorState;
            b_requestSonar = false;
            mutex_sensor->unlock();
            mutex_error->lock();
            signalStringValue = s_error;
            s_error = "";
            mutex_error->unlock();
            i_sensorSignals++;
           // qDebug() << "Sonar signals count: " << i_sensorSignals;
            qDebug() << "Sonar request finished, sending sync signal with value: " <<sensorRange<<" ...";
            mutex_executing->lock();
            b_executing = false;
            s_last = "";
            mutex_executing->unlock();
            if(sensorRange>2 || zerro_check>3)
            {
                mutex_executing->lock();
                b_executing = false;
                s_last = "";
                zerro_check=0;
                mutex_executing->unlock();
                emit commandFinished(signalStringValue, sensorRange);

            };
        }

        if (b_requestLight) {

            mutex_sensor->lock();
            msleep(10);
           int sensorValue= nxt_light->value(signalStringValue);
           qDebug()<<"Light:"<<sensorValue;
           if(sensorValue<3 || sensorValue>100)zerro_check++;
           // signalBoolValue = b_sensorState;
           if(sensorValue==0 && zerro_check>3)msleep(30);
            mutex_sensor->unlock();
            mutex_error->lock();
            signalStringValue = s_error;
            s_error = "";
            mutex_error->unlock();
            i_sensorSignals++;
   //         qDebug() << "Light signals count: " << i_sensorSignals;
     //       qDebug() << "Light new request finished, sending sync signal with value: " <<sensorValue<<" ...";
            if((sensorValue>2 && sensorValue<100)||zerro_check>7)
            {

            mutex_executing->lock();
            b_executing = false;
            s_last = "";
            zerro_check=0;
            b_requestLight = false;
            mutex_executing->unlock();
            if(sensorValue<1)qDebug()<<"Zerro LIGHT EMIT!!!";
            emit commandFinished(signalStringValue, sensorValue);
            };
        }

        // --- Wait for next quantum
        if(wait_timer>0)
        {
        msleep(QUANTUM_TIME);
        //wait_timer-=time.elapsed();
        //time.restart();
       // qDebug()<<"wait timer"<<wait_timer;
        if(wait_timer<=time.elapsed())
        {
            mutex_executing->lock();
            b_executing = false;
            s_last = "";
            wait_timer=0;
            mutex_executing->unlock();
            qDebug()<<"Emit!";
            emit commandFinished("", "");
        }
        }else msleep(QUANTUM_TIME);

    }
}


