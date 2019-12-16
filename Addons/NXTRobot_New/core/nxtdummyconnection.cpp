#include "nxtdummyconnection.h"

NXTDummyConnection::NXTDummyConnection() :
    NXTConnection(NULL, "Dummy")
{
}

void NXTDummyConnection::close()
{
}

void NXTDummyConnection::penMotorStart(int speed, unsigned int time_ms)
{
    emit commandFinished("Not connected to NXT",false);
}

void NXTDummyConnection::goForward(unsigned int)
{
    emit commandFinished("Not connected to NXT",false);
}

void NXTDummyConnection::goBackward(unsigned int)
{
    emit commandFinished("Not connected to NXT",false);
}

void NXTDummyConnection::turnLeft(unsigned int)
{
    emit commandFinished("Not connected to NXT",false);
}

void NXTDummyConnection::turnRight(unsigned int)
{
    emit commandFinished("Not connected to NXT",false);
}

void NXTDummyConnection::doBeep()
{
    emit commandFinished("Not connected to NXT",false);
}

bool NXTDummyConnection::isSensorPressedNow(QString &error)
{
    error = "Not connected to NXT";
    emit commandFinished("Not connected to NXT",false);
    return false;
}


void NXTDummyConnection::isWall()
{
    QString error = "Not connected to NXT";
    emit commandFinished("Not connected to NXT",false);
}

QString NXTDummyConnection::reset()
{
    return "Not connected to NXT";
}
