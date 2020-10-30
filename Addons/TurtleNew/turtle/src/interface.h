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
 Q_PLUGIN_METADATA(IID "kumir.pluginInterface/turtle" FILE "mymetadata.json")
public:	 
         void start(); //Запуск исполнителя
         QList<Alg> algList(); //Список алгоритмов исполнителя
	QString name(){return trUtf8("Черепаха");};
	 QList<QVariant> algOptResults(); //резы
	 QVariant     result();//возвращаемое значение
	 void	      runAlg(QString alg,QList<QVariant> params); //Запуск алгоритма
	 inline quint8 check(const QString &script = QString()) { Q_UNUSED(script); return 10; }
	 QString     errorText() const {return errortext;};
         void        showField(){mw->showTurtle();
                            t_pult->showNormal();t_pult->raise();}; //Показать поле
 	 void        showPult(){t_pult->showNormal();t_pult->raise();};//Показать пульт
	 void        hidePult(){t_pult->hide();};
         void        hideField(){mw->hideTurtle();};

 	 void        showOpt(){};
         bool        hasPult(){return true;}; //Есть ли пульт
 	 bool        hasOpt(){return false;}; //Есть ли дополнительное окно
	 QString     optText(){return "";};
         void 	      setMode(int mode); //Установить режим
      
	void reset(){mw->reset();errortext="";};
	void setParameter(const QString &paramName, const QVariant &paramValue){ Q_UNUSED(paramName); Q_UNUSED(paramValue);};
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
         void connectSignal( const QObject *obj, const char *method ){Q_UNUSED(obj); Q_UNUSED(method);};
        QWidget* mainWidget(){return (QWidget*)mw;};   
        inline QWidget *pultWidget() { return t_pult; }
        virtual QUrl     pdfUrl() const; //Помощь
        virtual QUrl     infoXmlUrl() const; // Ссылка на информацию об исполнителе
	
  signals:
      void sync();
      void sendText(QString text);
 public slots:
     void sendText2Kumir(QString text);
 private:
	void openServerPort(int port);
	int mode;
	turtle * mw;
	TurtlePult *t_pult;
	QString errortext;
	 KNPServer* server;

};
