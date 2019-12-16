#include "nxtcore.h"
#include "nxtconnection.h"
#include "nxtdummyconnection.h"
#include "nxt.h"
#include "connection.h"
#include "subprocess.h"

#include "nxtconnectionevent.h"

#ifdef Q_OS_UNIX
#include "usbnxt.h"
#include "bluetooth.h"
#endif
#include "nxt.h"
#include "connection.h"


class ConnectionRunner :
        public QThread
{
public:
    ConnectionRunner(QObject *parent);
    void setAddress(const QString &address);
    void setEventReceiver(QObject *eventReceiver);
    NXTConnection* result() const;
    QString error() const;
protected:
    void run();
private:
    QString m_address;
    QString m_error;
    NXTConnection* m_result;
    QObject *m_receiver;
};

ConnectionRunner::ConnectionRunner(QObject *parent) :
        QThread(parent),
        m_address(""),
        m_error(""),
        m_result(NULL),
        m_receiver(NULL)
{
}

void ConnectionRunner::setEventReceiver(QObject *eventReceiver)
{
    m_receiver = eventReceiver;
}

void ConnectionRunner::setAddress(const QString &address)
{
    m_address = address;
}

void ConnectionRunner::run()
{
    m_error="";
    NXTConnection *connection = NXTCore::makeConnection(m_address, m_error);
    m_result = connection;
    NXTConnectionEvent *event = new NXTConnectionEvent(m_result, m_error);
    QCoreApplication::postEvent(m_receiver, event);
}

NXTConnection* ConnectionRunner::result() const
{
    return m_result;
}

QString ConnectionRunner::error() const
{
    return m_error;
}


NXTCore::NXTCore(QObject *parent) :
        QObject(parent)
{
    m_hciToolSubprocess = new SubProcess(this,
                                         "Bluetooth scanner",
                                         "hcitool scan",
                                         QStringList(),
                                         false);
    connect(m_hciToolSubprocess, SIGNAL(processFinished(QString,QByteArray,int)),
            this, SLOT(handleHciToolFinished(QString,QByteArray,int)));


    m_connectionRunner = new ConnectionRunner(this);
    connect(m_connectionRunner, SIGNAL(finished()),
            this, SLOT(handleConnectionRunnerFinished()));

}


NXTConnection *NXTCore::makeConnection(const QString &address, QString &error)
{
    if (address.isEmpty()) {
        error = "Address is empty!";
        return new NXTDummyConnection();
    }
    QString normalizedAddress = address;
#ifdef Q_OS_WIN32
    normalizedAddress = normalizedAddress.remove("COM");
    normalizedAddress = normalizedAddress.remove("com");
    normalizedAddress = normalizedAddress.trimmed();
#endif
#ifdef Q_OS_UNIX
    normalizedAddress = normalizedAddress.trimmed().toLower();
#endif

#ifdef Q_OS_UNIX
    nxt::Connection *conn = new nxt::Bluetooth();
#endif

#ifdef Q_OS_WIN32
    Connection *conn = new Bluetooth();
#endif


#ifdef Q_OS_WIN32
    bool canConvert;
    int COMPortNumber = normalizedAddress.toInt(&canConvert);
    if (!canConvert) {
        error = "Not valid COM-port number";
        return new NXTDummyConnection();
    }
    try {
        conn->connect(COMPortNumber);
    }
    catch (Nxt_exception& e) {
        error = "Error "+QString::number(e.error_code())+" while connecting: "+QString::fromAscii(e.what());
        conn->disconnect();
    }
    if (!error.isEmpty()) {
        return new NXTDummyConnection();
    }
#endif

#ifdef Q_OS_UNIX
    char *BTAddress = address.toAscii().data();
    try {
        conn->connect(BTAddress);
    }
    catch (nxt::Nxt_exception& e) {
        qDebug()<<"Catch ERR";
        error = "Error "+QString::number(e.error_code())+" while connecting: "+QString::fromAscii(e.what());
        conn->disconnect();
    }
    if (!error.isEmpty()) {
        return new NXTDummyConnection();
    }
#endif

    return new NXTConnection(conn, normalizedAddress);

}

void NXTCore::handleHciToolFinished(const QString &processName,
                                    const QByteArray &data,
                                    int errorCode
                                    )
{
    QStringList result;
    QStringList lines;
    QString line;
    QStringList columns;
    QString address;
    QString name;
    if (errorCode==0) {
        QString output = QString::fromAscii(data);
        lines = output.split("\n");
        for (int i=1; i<lines.count(); i++) {
            line = lines.at(i).simplified();
            columns = line.split(QRegExp("\\s+"));
            if (columns.count()==2) {
                address = columns[0];
                name = columns[1];
                if (name.toLower().startsWith("nxt")) {
                    result << address;
                }
            }
        }
    }
    emit scanFinished(result);
}

#ifdef Q_OS_UNIX
void NXTCore::requestDevicesList()
{
    m_hciToolSubprocess->processData(QByteArray());
}

#endif


void NXTCore::requestConnection(QObject *eventReceiver, const QString &address)
{
    if (m_connectionRunner->isRunning())
        return;
    m_connectionRunner->setAddress(address);
    m_connectionRunner->setEventReceiver(eventReceiver);
    m_connectionRunner->start();
}

void NXTCore::handleConnectionRunnerFinished()
{
    qDebug() << "Connection runner finished";
    emit connectionDone();
}

NXTConnection *NXTCore::lastConnection()
{
    return m_connectionRunner->result();
}

QString NXTCore::lastError() const
{
    return m_connectionRunner->error();
}

#ifdef Q_OS_WIN32
void NXTCore::requestDevicesList()
{
    QStringList result;
    result << "COM3" << "COM4" << "COM5" << "COM6" << "COM7" << "COM8" << "COM9";
    emit scanFinished(result);
}
#endif

