#include "nxtconnection.h"
#include "nxt.h"
#include "connection.h"
#include "nxtmotor.h"
#include "nxtsensor.h"

#include "nxtbrick.h"
#include "nxtthread.h"


#define BASE_MOTOR 3
#ifdef Q_OS_UNIX
typedef nxt::Nxt_exception NxtException;
#endif
#ifdef Q_OS_WIN32
typedef Nxt_exception NxtException;
#endif


NXTConnection::NXTConnection(void *conn, const QString &connectionInfo):
        QObject(NULL),
        m_connection(conn),
        m_connectionInfo(connectionInfo)
{
    m_emitsCount = 0;
    m_thread = NULL;
    if (conn!=NULL) {
        m_leftMotor = new NxtMotor(0, conn);
        m_rightMotor = new NxtMotor(1, conn);
        m_touchSensor = new NxtSensor(0, conn); 
        m_sonar=new NxtSonar(1,conn);
        m_brick = new NxtBrick(conn);
        m_penMotor = new NxtMotor(2, conn);
        m_light=new NxtLight(3,conn);
        m_thread = new NXTThread(this,
                                 m_leftMotor, m_rightMotor, m_penMotor,
                                 m_touchSensor,m_sonar,m_light,
                                 m_brick
                                 );
        m_thread->setDistanceScaleMotorA(BASE_MOTOR);
        m_thread->setDistanceScaleMotorB(BASE_MOTOR);
        m_thread->setTurningScaleMotorA(3.7);
        m_thread->setTurningScaleMotorB(3.7);
        connect(m_thread, SIGNAL(commandFinished(QString,QVariant)),
                this, SLOT(handleReplyFromThread(QString,QVariant)),
                Qt::DirectConnection);
        m_thread->start();
    }    
}

void NXTConnection::close()
{
#ifdef Q_OS_UNIX
    nxt::Connection* conn = static_cast<nxt::Connection*>(m_connection);
#endif
#ifdef Q_OS_WIN32
    Connection* conn = static_cast<Connection*>(m_connection);
#endif
    Q_CHECK_PTR(conn);
    m_thread->stopThread();
    conn->disconnect();
    delete conn;
}

NXTConnection::~NXTConnection()
{
    close();
}

void NXTConnection::penMotorStart(int speed, unsigned int time_ms)
{
    if (m_thread!=NULL) {
        m_thread->requestPenMotorStart(speed, time_ms);
    }
    else {
        qDebug() << "NXT thread is NULL!";
    }
}


void NXTConnection::MotorStart(int speeda,int speedb,int speedc,bool a,bool b,bool c)
{
    if (m_thread!=NULL) {
        m_thread->requestMotorStart(speeda,speedb,speedc,a,b,c);
    }
    else {
        qDebug() << "NXT thread is NULL!";
    }
}
void NXTConnection::MotorStop(bool a,bool b,bool c)
{
    qDebug()<<"NXTCON STOP MTR";
    if (m_thread!=NULL) {
        m_thread->requestMotorStop(a,b,c);
    }
    else {
        qDebug() << "NXT thread is NULL!";
    }
}

void NXTConnection::goForward(const unsigned int distance)
{
    if (m_thread!=NULL) {
        m_thread->requestMotorsGoForward(distance);
    }
    else {
        qDebug() << "NXT thread is NULL!";
    }
}

void NXTConnection::goBackward(const unsigned int distance)
{
    if (m_thread!=NULL) {
        m_thread->requestMotorsGoBackward(distance);
    }
    else {
        qDebug() << "NXT thread is NULL!";
    }
}

void NXTConnection::turnLeft(const unsigned int degrees)
{
    if (m_thread!=NULL) {
        m_thread->requestMotorsTurnLeft(degrees);
    }
    else {
        qDebug() << "NXT thread is NULL!";
    }
}

void NXTConnection::turnRight(const unsigned int degrees)
{
    if (m_thread!=NULL) {
        m_thread->requestMotorsTurnRight(degrees);
    }
    else {
        qDebug() << "NXT thread is NULL!";
    }
}

void NXTConnection::motorC(int time,int speed)
{
    if (m_thread!=NULL) {
        m_thread->requestPenMotorStart(speed,time);
    }
    else {
        qDebug() << "NXT thread is NULL!";
    }
}

void NXTConnection::doBeep()
{
    if (m_thread!=NULL) {
        m_thread->requestSound();
    }
    else {
        qDebug() << "NXT thread is NULL!";
    }
}


void NXTConnection::touch()
{
    if (m_thread!=NULL) {
        m_thread->checkTouch();
        qDebug()<<"Check touch!";
    }
    else {
        qDebug() << "NXT thread is NULL!";
    }
}


bool NXTConnection::isSensorPressedNow(QString &error)
{
    if (m_thread!=NULL) {
        return m_touchSensor->isTouched(error);
    }
    else {
        qDebug() << "NXT thread is NULL!";
    }
}

void NXTConnection::setLeftMotorDegreeScale(const qreal scale)
{
    if (m_thread!=NULL) {
        m_thread->setTurningScaleMotorA(scale);
    }
    else {
        qDebug() << "NXT thread is NULL!";
    }
}

void NXTConnection::setLeftMotorDistanceScale(const qreal scale)
{
    if (m_thread!=NULL) {
        m_thread->setDistanceScaleMotorA(scale);
    }
    else {
        qDebug() << "NXT thread is NULL!";
    }
}

void NXTConnection::setRightMotorDegreeScale(const qreal scale)
{
    if (m_thread!=NULL) {
        m_thread->setTurningScaleMotorB(scale);
    }
    else {
        qDebug() << "NXT thread is NULL!";
    }
}

void NXTConnection::setRightMotorDistanceScale(const qreal scale)
{
    if (m_thread!=NULL) {
        m_thread->setDistanceScaleMotorB(scale);
    }
    else {
        qDebug() << "NXT thread is NULL!";
    }
}

void NXTConnection::isWall()
{
    if (m_thread!=NULL) {
        m_thread->requestSensorState();
    }
    else {
        qDebug() << "NXT thread is NULL!";
    }
}

int NXTConnection::range()
{
    if (m_thread!=NULL) {
        qDebug()<<"SONAR!!!";
        m_thread->requestSonar();
    }
    else {
        qDebug() << "NXT thread is NULL!";
    }
}

int NXTConnection::light()
{
    if (m_thread!=NULL) {
        qDebug()<<"Connection light";
        m_thread->requestLight();
    }
    else {
        qDebug() << "NXT thread is NULL!";
    }
}

void NXTConnection::rotation(int motor_id)
{
    if (m_thread!=NULL) {
        qDebug()<<"Get rot";
        m_thread->requestRot(motor_id);
    }
    else {
        qDebug() << "NXT thread is NULL!";
    }
}

void NXTConnection::resetRot(int motor_id)
{
    if (m_thread!=NULL) {
        qDebug()<<"res rot";
        m_thread->resetRot(motor_id);
    }
    else {
        qDebug() << "NXT thread is NULL!";
    }
}

void NXTConnection::sleep_m_sec(int time)
{
    if (m_thread!=NULL) {
        qDebug()<<"res rot";
        m_thread->sleep_msc(time);
    }
    else {
        qDebug() << "NXT thread is NULL!";
    }
};
QString NXTConnection::reset()
{
    if (m_thread!=NULL) {
        m_thread->reset();
    }
    else {
        qDebug() << "NXT thread is NULL!";
    }
    return "";
}

QString NXTConnection::information() const
{
    return m_connectionInfo;
}

void NXTConnection::handleReplyFromThread(const QString &error, QVariant value) {
    qDebug() << "Reply from NXT thread: command finished";
    m_emitsCount++;
    qDebug() << "Total emits count: " << m_emitsCount;
    emit commandFinished(error, value);
}
