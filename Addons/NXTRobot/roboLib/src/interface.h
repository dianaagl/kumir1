/****************************************************************************
**
** Copyright (C) 2004-2008 NIISI RAS. All rights reserved.
**
** This file is part of the KuMir.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
 #include <QtCore>
 #include "turtle.h"
 #include "pult.h"
#include "../../../plugin_interface.h"


class TurtleStarter:public QObject, public kumirPluginInterface
{
 Q_OBJECT 
 Q_INTERFACES(kumirPluginInterface)
public:	 
         void start(); //Запуск исполнителя
         QList<Alg> algList(); //Список алгоритмов исполнителя
        QString name(){return trUtf8("ЛегоРобот");};
	 QList<QVariant> algOptResults(); //резы
	 QVariant     result();//возвращаемое значение
	 void	      runAlg(QString alg,QList<QVariant> params); //Запуск алгоритма
	 QString     errorText() const {return errortext;};
         void        showField(){mw->showTurtle();
                            t_pult->showNormal();t_pult->raise();}; //Показать поле
 	 void        showPult(){t_pult->showNormal();t_pult->raise();};//Показать пульт
	 void        hidePult(){t_pult->hide();};
         void        hideField(){mw->hideTurtle();};

 	 void        showOpt(){};
         bool        hasPult(){return false;}; //Есть ли пульт
 	 bool        hasOpt(){return false;}; //Есть ли дополнительное окно
	 QString     optText(){return "";};
         void 	      setMode(int mode); //Установить режим
      
        void reset(){
            errortext="";
            for(int i=0;i<4;i++)timers[i].restart();
             };

        quint8 check(const QString &script = QString()){return 0;};

	void setParameter(const QString &paramName, const QVariant &paramValue){return;};
        /**
          Присоединяет слот @param method объекта @param obj к сигналу "Отправить текст"
          Пример использования:
            plugin->connectSignalSendText(kumir, SLOT(slotName(QString))
          */
         void connectSignalSendText( const QObject *obj, const char *method );
        /**
          Присоединяет слот @param method объекта @param obj к сигналу "Выполнение команды завершено"
          Пример использования:
            plugin->connectSignalSendSync(kumir, SLOT(slotName(bool))
          */
       void connectSignalSync( const QObject *obj, const char *method);
         void connectSignal( const QObject *obj, const char *method ){return;};
         QWidget* mainWidget(){return (QWidget*)mw;};
         QWidget* pultWidget(){return (QWidget*)t_pult;};//Пульт исполнителяw;};
	QString     help(){return QString("help")+QDir::separator()+"turtle.pdf";}; //Помощь
    QUrl     infoXmlUrl() const{return QUrl();};
    QUrl     pdfUrl()const {return QUrl();};
  signals:
      void sync();
      void sendText(QString text);
 public slots:
     void sendText2Kumir(QString text);
     void NXTReady(QString err,QVariant value);
 private:
	void openServerPort(int port);
	int mode;
        NXTSetup * mw;
        NXTPult *t_pult;
	QString errortext;
        QVariant replay;
	 KNPServer* server;
     QList<QTime> timers;
     bool skipSync;//Skip one sync signal

};
