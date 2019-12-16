#include "nxtconnectionevent.h"

NXTConnectionEvent::NXTConnectionEvent(NXTConnection* connection, const QString &error) :
        QEvent((QEvent::Type)2000)
{
    m_connection = connection;
    m_error = error;
}
