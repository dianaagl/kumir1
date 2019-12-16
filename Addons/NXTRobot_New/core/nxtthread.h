#ifndef NXTTHREAD_H
#define NXTTHREAD_H

#include <QtCore>

class NxtMotor;
class NxtSensor;
class NxtBrick;
class NxtSonar;
class NxtLight;
class NXTThread : public QThread
{
    Q_OBJECT
public:
    explicit NXTThread(QObject *parent,
                       NxtMotor *A, NxtMotor *B, NxtMotor *C,
                       NxtSensor *sensor,NxtSonar* sonar,NxtLight* light,
                       NxtBrick *brick
                       );
    void requestMotorsGoForward(unsigned int distance);
    void requestMotorsGoBackward(unsigned int distance);
    void requestMotorsTurnLeft(unsigned int degrees);
    void requestMotorsTurnRight(unsigned int degrees);
    void requestPenMotorStart(int speed, unsigned int time_ms);
    void requestMotorStart(int speeda,int speedb,int speedc,bool a,bool b,bool c);
    void requestMotorStop(bool a,bool b,bool c);

    void requestSound();
    void requestRot(int motor_id);//motor_id: 0- none ;1- A;2 -B ;3 -C;
    void resetRot(int motor_id);
    void sleep_msc(int msecs);
    void requestSensorState();
    void requestSonar();
    void requestLight();
    bool getTouch(){return was_touch;};
    void resetTouch();
    void checkTouch();
    void reset();
    void stopThread();

    inline void setDistanceScaleMotorA(const qreal scale) {
        r_scaleDistanceA = scale;
    }

    inline void setDistanceScaleMotorB(const qreal scale) {
        r_scaleDistanceB = scale;
    }

    inline void setTurningScaleMotorA(const qreal scale) {
        r_scaleTurningA = scale;
    }

    inline void setTurningScaleMotorB(const qreal scale) {
        r_scaleTurningB = scale;
    }


    ~NXTThread();

signals:
    void commandFinished(const QString &error, QVariant result);

protected:
    void run();

private:
    // Real devices pointers
    NxtMotor *nxt_motorA;
    NxtMotor *nxt_motorB;
    NxtMotor *nxt_motorC;
    NxtSonar *nxt_sonar;
      NxtLight *nxt_light;

    NxtSensor *nxt_touchSensor;
    NxtBrick *nxt_brick;

    // Motor A flags and states
    QMutex *mutex_motorA;
    int i_timeLeftA;

    // Motor B flags and states
    QMutex *mutex_motorB;
    int i_timeLeftB;

    // Motor C flags and states
    QMutex *mutex_motorC;
    int i_timeLeftC;

    // Sensor flags and states
    QMutex *mutex_sensor;
    bool b_sensorState;
    bool b_ignoreSensor;
    bool b_requestSensor;
    bool b_requestSonar;
    bool b_requestRotA,b_rec_resetA;
    bool b_requestRotB,b_rec_resetB;
    bool b_requestRotC,b_rec_resetC;
    bool was_touch;
        bool b_requestLight;
    int i_sensorTimer,wait_timer;

    // Sound flags and states
    QMutex *mutex_brick;
    int i_brickTimer;

    // Calibrations
    int i_speedA;
    int i_speedB;
    int i_speedC;

    qreal r_scaleDistanceA;
    qreal r_scaleDistanceB;

    qreal r_scaleTurningA;
    qreal r_scaleTurningB;


    // Self management
    bool b_stop;
    QMutex *mutex_stop;
    QMutex *mutex_error;
    QString s_error;

    QMutex *mutex_executing;
    bool b_executing;
    QString s_last;
    QTime time;

    // Debug
    int i_sensorRequests;
    int i_sensorSignals;

};

#endif // NXTTHREAD_H
