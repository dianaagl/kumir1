#ifndef PLUGIN_H
#define PLUGIN_H

#include "pult.h"
#include "robot.h"
#include "window.h"

#include "../../plugin_interface.h"
#include <QObject>
#include <QList>
#include <QVariant>
class Pult;
class Robot;
class Window;

class Plugin:
        public QObject,
        public kumirPluginInterface
{

    Q_OBJECT
    Q_INTERFACES(kumirPluginInterface)

    public:

        Plugin();
        ~Plugin();

        void start(); //Запуск исполнителя

        void showField();
        void hideField();

        bool hasPult();
        void showPult();
        void hidePult();

        bool hasOpt(); //options
        void showOpt();
        void hideOpt();

        void reset();
        void setMode(int mode);  //Установить режим

        QString name();
        QString help();
        QString optText();  //Текст для дополнительного окна
        QString errorText() const;
        QWidget *mainWidget();

        QVariant        result();       //возвращаемое значение
        QList<Alg>      algList();      //Список алгоритмов исполнителя
        QList<QVariant> algOptResults();//резы

        void runAlg(QString alg,QList<QVariant> params); //Запуск алгоритма
        void setParameter(const QString &paramName, const QVariant &paramValue);
        void connectSignalSync(const QObject *obj, const char *method);
        void connectSignalSendText(const QObject *obj, const char *method);

        QUrl pdfUrl() const;
        QUrl infoXmlUrl() const;

	quint8 check(const QString &script = QString());
	QWidget* pultWidget();

    public slots:
        void emitSync();
	void connectPult();
	void disconnectPult();
	void processRobotMessage(QString s1, QString s2, QString s3);

    signals:
        void sendText(QString);
        void sync();

    private:
	RoboPult *pult;
        Robot *robot;
	Window *globj;
        QVariant res;
	QString errorstring;
};

#endif // PLUGIN_H
