#ifndef PAINTERPLUGIN_H
#define PAINTERPLUGIN_H

#include <QtGui>
#include "plugin_interface.h"


class PainterPlugin
    : public QObject
    , public kumirPluginInterface
{
    Q_OBJECT;
    Q_INTERFACES(kumirPluginInterface);
public:
    PainterPlugin(QObject *parent = 0);
    inline void start() { }
    QList<Alg> algList();
    QString name();
    QList<QVariant> algOptResults();
    QVariant result();
    void runAlg(QString alg, QList<QVariant> params);
    void showField();
    inline void showPult() { }
    inline void hidePult() { }
    void hideField();
    inline void showOpt() { }
    inline bool hasPult() { return false; }
    inline bool hasOpt() { return false; }
    inline QString optText()  { return ""; }
    void setMode(int mode);
    void reset();
    QString errorText() const;
    quint8 check(const QString &script);
    void setParameter(const QString &paramName, const QVariant &paramValue);
    void connectSignalSendText(const QObject *obj, const char *method);
    void connectSignalSync(const QObject *obj, const char *method);
    QUrl pdfUrl() const;
    QUrl infoXmlUrl() const;
    QWidget * mainWidget();
    inline QWidget * pultWidget() { return 0; }
private slots:
    void handleResetRequest();
    void handleNewImageRequest(int w, int h, const QString &bg, bool useTemplate, const QString &fileName);
    void handleLoadImageRequest(const QString &fileName);
signals:
    void sendText(const QString &text);
    void sync();
private:
    struct PainterPluginPrivate *d;
};

#endif // PAINTERPLUGIN_H
