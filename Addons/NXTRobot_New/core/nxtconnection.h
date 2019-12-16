#ifndef NXTCONNECTION_H
#define NXTCONNECTION_H

#include <QtCore>

class NxtMotor;
class NxtSensor;
class NxtBrick;
class NxtSonar;
class NxtLight;
class NXTThread;

class NXTConnection : public QObject
{
    friend class NXTCore;

    Q_OBJECT

public:

    /** Returns information on connection */
    virtual QString information() const;

    /** Closes connection */
    virtual void close();

    /** Closes connection and deletes itself */
    virtual ~NXTConnection();

    /** Sets scale factor for left motor while moving
      * @param scale floating-point scale factor
      */
    virtual void setLeftMotorDistanceScale(const qreal scale);

    /** Sets scale factor for right motor while moving
      * @param scale floating-point scale factor
      */
    virtual void setRightMotorDistanceScale(const qreal scale);

    /** Sets scale factor for left motor while turning
      * @param scale floating-point scale factor
      */
    virtual void setLeftMotorDegreeScale(const qreal scale);

    /** Sets scale factor for right motor while turning
      * @param scale floating-point scale factor
      */
    virtual void setRightMotorDegreeScale(const qreal scale);

public slots:

    /** Start motor C
      * @param speed motor speed
      * @param time_ms time to work in milliseconds
      */
    virtual void penMotorStart(int speed, unsigned int time_ms);
    /** Start motors A,B,C
      * @param speed motor speed
      * @param time_ms time to work in milliseconds
      */
    virtual void MotorStart(int speeda,int speedb,int speedc,bool a,bool b,bool c);
    virtual void MotorStop(bool a,bool b,bool c);




    /** Go forward
      * @param distance distance in centimeters
      */
    virtual void goForward(const unsigned int distance);

    /** Go backward
      * @param distance distance in centimeters
      */
    virtual void goBackward(const unsigned int distance);

    /** Turn left
      * @param degrees number of degrees to turn
      */
    virtual void turnLeft(const unsigned int degrees);

    /** Turn right
      * @param degrees number of degrees to turn
      */
    virtual void turnRight(const unsigned int degrees);
    virtual void motorC(int time,int speed);

    /** Play sound
      * @returns error string if error occured, or empty string if ok
      */
    virtual void doBeep();
    virtual void touch();

    /** Check for touch sensor pressed at the moment using touch sensor
      * @param error reference to variable, where error string will be stored
      * @returns true, if there is a wall at the moment
      */
    virtual bool isSensorPressedNow(QString &error);

    /** Schedules to check for wall */
    virtual void isWall();
    virtual int range();
       virtual int light();
    virtual void rotation(int motor_id);
    virtual void resetRot(int motor_id);
    virtual void sleep_m_sec(int time);
    /** Reset state: stop motors and clean sensor flag
      * @returns error string if error occured, or empty string if ok
      */
    virtual QString reset();

signals:
    void commandFinished(const QString &error, QVariant value);

protected:
    explicit NXTConnection(void *conn, const QString &connectionInfo);

    void *m_connection;
    QString m_connectionInfo;

    NxtMotor *m_leftMotor;
    NxtMotor *m_rightMotor;
    NxtMotor *m_penMotor;
    NxtSensor *m_touchSensor;
    NxtBrick *m_brick;
    NxtSonar *m_sonar;
    NxtLight *m_light;
    NXTThread *m_thread;

    // for debugging
    int m_emitsCount;

 protected slots:
    void handleReplyFromThread(const QString &error, QVariant value);


};

#endif // NXTCONNECTION_H
