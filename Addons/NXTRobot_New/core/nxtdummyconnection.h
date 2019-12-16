#ifndef NXTDUMMYCONNECTION_H
#define NXTDUMMYCONNECTION_H

#include <QtCore>
#include "nxtconnection.h"


class NXTDummyConnection :
        public NXTConnection
{

    friend class NXTCore;
    Q_OBJECT
public:
    virtual void close();
public slots:
    virtual void penMotorStart(int speed, unsigned int time_ms);
    virtual void goForward(const unsigned int distance);
    virtual void goBackward(const unsigned int distance);
    virtual void turnLeft(const unsigned int degrees);
    virtual void turnRight(const unsigned int degrees);
    virtual void doBeep();
    virtual bool isSensorPressedNow(QString &error);
    virtual void isWall();
    virtual QString reset();
protected:
    NXTDummyConnection();
};

#endif // NXTDUMMYCONNECTION_H
