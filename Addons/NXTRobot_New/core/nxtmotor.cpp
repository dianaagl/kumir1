#include <QtCore>

#include "nxtmotor.h"
#include "nxt.h"
#include "connection.h"

#ifdef Q_OS_UNIX
typedef nxt::Nxt_exception NxtException;
#endif
#ifdef Q_OS_WIN32
typedef Nxt_exception NxtException;
#endif

#ifdef Q_OS_UNIX

NxtMotor::NxtMotor(unsigned short number, void *connection)
{
    nxt::Connection *conn = static_cast<nxt::Connection*>(connection);
    if (number==0) {
        nxt::Motor *motor = new nxt::Motor(nxt::OUT_A, conn);
        m_motor = motor;
    }
    else if (number==1) {
        nxt::Motor *motor = new nxt::Motor(nxt::OUT_B, conn);
        m_motor = motor;
    }
    else if (number==2) {
        nxt::Motor *motor = new nxt::Motor(nxt::OUT_C, conn);
        m_motor = motor;
    }
    m_ignoreSensor = false;
    m_running = false;
    m_runningMutex = new QMutex;
    m_ignoreSensorMutex = new QMutex;
}

QString NxtMotor::reset()
{
    nxt::Motor *motor = static_cast<nxt::Motor*>(m_motor);
    Q_CHECK_PTR(motor);
    QString error;
    try {
        motor->reset_rotation();
        oldMotorRotation=motor->get_rotation();
        qDebug()<<"Old mot rot:"<<oldMotorRotation;
    }
    catch (NxtException &e) {
        error = "Error "+QString::number(e.error_code())+": "+QString::fromAscii(e.what());
    }
    return error;
}

int NxtMotor::rotationCount(QString &error)
{
    nxt::Motor *motor = static_cast<nxt::Motor*>(m_motor);
    Q_CHECK_PTR(motor);
    int value;
    error = "";
    try {
        value = motor->get_rotation();
        //qDebug()<<"getRotoation"<<value;
    }
    catch (NxtException &e) {
        error = "Error "+QString::number(e.error_code())+": "+QString::fromAscii(e.what());
        qDebug() << "Error while getting rotation";
    }
    return (value-oldMotorRotation)/6.7;
}

QString NxtMotor::on(int speed, bool ignoreSensor)
{
    QMutexLocker l(m_runningMutex);
    m_ignoreSensorMutex->lock();
    m_ignoreSensor = ignoreSensor;
    m_ignoreSensorMutex->unlock();
    nxt::Motor *motor = static_cast<nxt::Motor*>(m_motor);
    Q_CHECK_PTR(motor);
    QString error;
    try {
        motor->on(speed);
        m_running = true;
    }
    catch (NxtException &e) {
        error = "Error "+QString::number(e.error_code())+": "+QString::fromAscii(e.what());
    }
    return error;
}

QString NxtMotor::stop()
{
    QMutexLocker l(m_runningMutex);
    nxt::Motor *motor = static_cast<nxt::Motor*>(m_motor);
    Q_CHECK_PTR(motor);
    QString error;
    try {
        motor->stop();
        m_running = false;
    }
    catch (NxtException &e) {
        error = "Error "+QString::number(e.error_code())+": "+QString::fromAscii(e.what());
    }
    return error;
}

bool NxtMotor::isRunning() const
{
    QMutexLocker l(m_runningMutex);
    return m_running;
}

bool NxtMotor::isIgnoringSensor() const
{
    QMutexLocker l(m_ignoreSensorMutex);
    return m_ignoreSensor;
}

#endif


#ifdef Q_OS_WIN32

NxtMotor::NxtMotor(unsigned short number, void *connection)
{
    Connection *conn = static_cast<Connection*>(connection);
    if (number==0) {
        Motor *motor = new Motor(OUT_A, conn);
        m_motor = motor;
    }
    else if (number==1) {
        Motor *motor = new Motor(OUT_B, conn);
        m_motor = motor;
    }
    else if (number==2) {
        Motor *motor = new Motor(OUT_C, conn);
        m_motor = motor;
    }
    m_ignoreSensor = false;
    m_running = false;
    m_runningMutex = new QMutex;
    m_ignoreSensorMutex = new QMutex;
}

QString NxtMotor::reset()
{
    Motor *motor = static_cast<Motor*>(m_motor);
    Q_CHECK_PTR(motor);
    QString error;
    try {
        motor->reset_rotation();
    }
    catch (NxtException &e) {
        error = "Error "+QString::number(e.error_code())+": "+QString::fromAscii(e.what());
    }
    return error;
}

int NxtMotor::rotationCount(QString &error)
{
    Motor *motor = static_cast<Motor*>(m_motor);
    Q_CHECK_PTR(motor);
    int value;
    error = "";
    try {
        value = motor->get_rotation();
    }
    catch (NxtException &e) {
        error = "Error "+QString::number(e.error_code())+": "+QString::fromAscii(e.what());
        qDebug() << "Error while getting rotation";
    }
    return value;
}

QString NxtMotor::on(int speed, bool ignoreSensor)
{
    QMutexLocker l(m_runningMutex);
    m_ignoreSensorMutex->lock();
    m_ignoreSensor = ignoreSensor;
    m_ignoreSensorMutex->unlock();
    Motor *motor = static_cast<Motor*>(m_motor);
    Q_CHECK_PTR(motor);
    QString error;
    try {
        motor->on(speed);
        m_running = true;
    }
    catch (NxtException &e) {
        error = "Error "+QString::number(e.error_code())+": "+QString::fromAscii(e.what());
    }
    return error;
}

QString NxtMotor::stop()
{
    QMutexLocker l(m_runningMutex);
    Motor *motor = static_cast<Motor*>(m_motor);
    Q_CHECK_PTR(motor);
    QString error;
    try {
        motor->stop();
        m_running = false;
    }
    catch (NxtException &e) {
        error = "Error "+QString::number(e.error_code())+": "+QString::fromAscii(e.what());
    }
    return error;
}

bool NxtMotor::isRunning() const
{
    QMutexLocker l(m_runningMutex);
    return m_running;
}

bool NxtMotor::isIgnoringSensor() const
{
    QMutexLocker l(m_ignoreSensorMutex);
    return m_ignoreSensor;
}

#endif
