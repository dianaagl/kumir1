#include <QtCore>
#include "nxtsensor.h"
//#include "nxtsensor.h"
#include "nxt.h"
#include "connection.h"


#ifdef Q_OS_UNIX
typedef nxt::Nxt_exception NxtException;
#endif
#ifdef Q_OS_WIN32
typedef Nxt_exception NxtException;
#endif

#ifdef Q_OS_UNIX
NxtSensor::NxtSensor(unsigned short number, void *connection)
{
    nxt::Connection *conn = static_cast<nxt::Connection*>(connection);
    Q_CHECK_PTR(conn);
    if (number==0) {
        m_sensor = new nxt::Touch(nxt::IN_1, conn);
    }
    else if (number==1) {
        m_sensor = new nxt::Touch(nxt::IN_2, conn);
    }
    else if (number==2) {
        m_sensor = new nxt::Touch(nxt::IN_3, conn);
    }
    else if (number==3) {
        m_sensor = new nxt::Touch(nxt::IN_4, conn);
    }
}

bool NxtSensor::isTouched(QString &error)
{
    nxt::Touch *sensor = static_cast<nxt::Touch*>(m_sensor);
    Q_CHECK_PTR(sensor);
    bool value;
    error = "";
    QString v;
    try {
        v = QString::fromAscii(sensor->print().c_str());
        value = v.toLower() == "pressed";
    }
    catch (NxtException &e) {
        value = false;
        error = "Error "+QString::number(e.error_code())+": "+QString::fromAscii(e.what());
    }
    return value;
}

#endif

#ifdef Q_OS_WIN32
NxtSensor::NxtSensor(unsigned short number, void *connection)
{
    Connection *conn = static_cast<Connection*>(connection);
    Q_CHECK_PTR(conn);
    if (number==0) {
        m_sensor = new Touch(IN_1, conn);
    }
    else if (number==1) {
        m_sensor = new Touch(IN_2, conn);
    }
    else if (number==2) {
        m_sensor = new Touch(IN_3, conn);
    }
    else if (number==3) {
        m_sensor = new Touch(IN_4, conn);
    }
}

bool NxtSensor::isTouched(QString &error)
{
    Touch *sensor = static_cast<Touch*>(m_sensor);
    Q_CHECK_PTR(sensor);
    bool value;
    error = "";
    QString v;
    try {
        v = QString::fromAscii(sensor->print().c_str());
        value = v.toLower() == "pressed";
    }
    catch (NxtException &e) {
        value = false;
        error = "Error "+QString::number(e.error_code())+": "+QString::fromAscii(e.what());
    }
    return value;
}




#endif


NxtSonar::NxtSonar(unsigned short number, void *connection)
{
    conn = static_cast<nxt::Connection*>(connection);
    Q_CHECK_PTR(conn);
    if (number==0) {
        m_sonar = new nxt::Sonar(nxt::IN_1, conn);
    }
    else if (number==1) {
        m_sonar = new nxt::Sonar(nxt::IN_2, conn);
    }
    else if (number==2) {
        m_sonar = new nxt::Sonar(nxt::IN_3, conn);
    }
    else if (number==3) {
        m_sonar = new nxt::Sonar(nxt::IN_4, conn);
    }
}
int NxtSonar::range(QString &error)
{
    m_sonar = new nxt::Sonar(nxt::IN_2, conn);
    nxt::Sonar *sensor = static_cast<nxt::Sonar*>(m_sonar);
    Q_CHECK_PTR(sensor);
    int value;
    error = "";
    QString v;
    qDebug()<<"Get sonar value";
    try {
       // v = QString::fromAscii(sensor->print().c_str());
        sensor->init(true);
        sensor->sonar_reset();
        value=sensor->read();
        qDebug()<<"Value:"<<value;
        //value = v.toLower() == "pressed";


    }
    catch (NxtException &e) {
        value = 0;
        error = "Error "+QString::number(e.error_code())+": "+QString::fromAscii(e.what());
        qDebug()<<"Error"<<error;
    }
    return value;
}


NxtLight::NxtLight(unsigned short number, void *connection)
{
    conn = static_cast<nxt::Connection*>(connection);
    Q_CHECK_PTR(conn);
    if (number==0) {
        m_light = new nxt::Light(nxt::IN_1, conn);
    }
    else if (number==1) {
        m_light = new nxt::Light(nxt::IN_2, conn);
    }
    else if (number==2) {
        m_light = new nxt::Light(nxt::IN_3, conn);
    }
    else if (number==3) {
        m_light = new nxt::Light(nxt::IN_4, conn);
    }
}
int NxtLight::value(QString &error)
{
    m_light = new nxt::Light(nxt::IN_3, conn);//light
    nxt::Light *sensor = static_cast<nxt::Light*>(m_light);
   // m_light = new nxt::Compass(nxt::IN_3, conn);//light
  //  nxt::Compass *sensor = static_cast<nxt::Compass*>(m_light);

    Q_CHECK_PTR(sensor);
    int value;
    error = "";
    QString v;
    qDebug()<<"Get light value";
    try {
       // v = QString::fromAscii(sensor->print().c_str());
        sensor->init(true);

        value=sensor->read();
        qDebug()<<"Light Value:"<<value;
        //value = v.toLower() == "pressed";


    }
    catch (NxtException &e) {
        value = 0;
        error = "Error "+QString::number(e.error_code())+": "+QString::fromAscii(e.what());
        qDebug()<<"Error"<<error;
    }
    return value;
}
