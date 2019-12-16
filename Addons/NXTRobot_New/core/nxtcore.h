#ifndef NXTCORE_H
#define NXTCORE_H

#include <QtCore>

class NXTConnection;
class SubProcess;
class ConnectionRunner;

class NXTCore:
        public QObject
{
    Q_OBJECT
public:
    NXTCore(QObject *parent = NULL);
    static NXTConnection* makeConnection(const QString &address, QString &error);
    void requestConnection(QObject *eventReceiver, const QString &address);
    void requestDevicesList();
    NXTConnection* lastConnection();
    QString lastError() const;

signals:
    void scanFinished(const QStringList &adresses);
    void connectionDone();

private slots:
    void handleHciToolFinished(const QString &processName,
                               const QByteArray &data,
                               int errorCode
                               );
    void handleConnectionRunnerFinished();

private:
    SubProcess* m_hciToolSubprocess;
    ConnectionRunner *m_connectionRunner;

};

#endif // NXTCORE_H
