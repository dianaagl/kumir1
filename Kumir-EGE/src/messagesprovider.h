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
#ifndef MESSAGESPROVIDER_H
#define MESSAGESPROVIDER_H

#include <QtCore>

/**
	Поставщик текстов сообщений об ошибках
**/

class MessagesProvider : public QObject
{
Q_OBJECT
Q_PROPERTY(bool debugMode READ isDebugMode WRITE setDebugMode)
public:
    /**
     * Конструктор
     * @param parent ссылка на объект-владелец
     */
    MessagesProvider(QObject *parent = 0);
    /**
     * Деструктор
     */
    ~MessagesProvider();
		/**
		 * Возвращает сообщение об ошибке по его номеру
		 * @param code код ошибки
		 * @return текст сообщения
		 */
		QString errorMessage(uint code);
		/**
		 * Возвращает значение флага debugMode
		 * @return значение флага
		 */
		bool isDebugMode();
		/**
		 * Устанавливает значение флага debugMode
		 * @param mode значение флага
		 */
		void setDebugMode(bool mode);
	public slots:
		/**
		 * Инициализация
		 * @param languageFileName имя файла с текстами ошибок
		 */
		void init(const QString & languageFileName);
	protected:
		/** Флаг отладочного режима. Если true, то в текстах 
		 * сообщений об ошибках всегда присутствует их код */
		bool debugMode;
		/** Отбражение из кодов ошибок в тексты */
		QMap<uint,QString> map;

};

#endif
