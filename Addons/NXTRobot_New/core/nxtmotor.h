#ifndef NXTMOTOR_H
#define NXTMOTOR_H

#include <QtCore>

class NxtMotor
{
public:
    NxtMotor(unsigned short number, void *connection);
    QString reset();
    QString on(int speed, bool ignoreSensor);
    int rotationCount(QString &error);
    bool isRunning() const;
    bool isIgnoringSensor() const;
    QString stop();
private:
    void *m_motor;
    bool m_running;
    QMutex *m_runningMutex;
    bool m_ignoreSensor;
    QMutex *m_ignoreSensorMutex;
    int oldMotorRotation;
};

#endif // NXTMOTOR_H
