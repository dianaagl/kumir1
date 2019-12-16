#ifndef NXTCONNECTIONEVENT_H
#define NXTCONNECTIONEVENT_H

#include <QtCore>

class NXTConnection;

class NXTConnectionEvent:
        public QEvent
{
public:
    explicit NXTConnectionEvent(NXTConnection* connection, const QString &error);
    inline NXTConnection* connection() const { return m_connection; }
    inline QString error() const { return m_error; }
private:
    NXTConnection* m_connection;
    QString m_error;
};

#endif // NXTCONNECTIONEVENT_H
