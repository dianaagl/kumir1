#ifndef NXTSENSOR_H
#define NXTSENSOR_H
#include "nxt.h"
#include "connection.h"
class NxtSensor
{
public:
    NxtSensor(unsigned short number, void *connection);
    bool isTouched(QString &error);
private:
    void *m_sensor;
};

class NxtSonar
{
public:
    NxtSonar(unsigned short number, void *connection);
    int range(QString &error);
private:
    void *m_sonar;
    nxt::Connection *conn;
};


class NxtLight
{
public:
    NxtLight(unsigned short number, void *connection);
    int value(QString &error);
private:
    void *m_light;
    nxt::Connection *conn;
};
#endif // NXTSENSOR_H
