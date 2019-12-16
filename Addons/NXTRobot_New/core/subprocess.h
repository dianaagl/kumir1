#ifndef SUBPROCESS_H
#define SUBPROCESS_H

#include <QProcess>

class SubProcess :
        public QProcess
{
    Q_OBJECT
public:
    SubProcess(QObject *parent, const QString &name, const QString &execuable, const QStringList &environment, bool mergeOutStreams);
    void processData(const QByteArray &data);
private:
    QString m_name;
    QString m_execuable;
    QByteArray m_data;
    QByteArray m_reply;
    QByteArray m_error;
	private slots:
    void handleReadyRead();
    void handleProcessFinished(int code, QProcess::ExitStatus status);
    void handleProcessStarted();
    void handleProcessError(QProcess::ProcessError error);
signals:
    void processFinished(const QString &name, const QByteArray &reply, int errorCode);
};

#endif // SUBPROCESS_H
