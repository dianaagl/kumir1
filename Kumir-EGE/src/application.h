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
#ifndef APPLICATION_H
#define APPLICATION_H

#include <QtCore>
#include "enums.h"

namespace Kum {
	class Run;
	class Compiler;
}

class MessagesProvider;


/** Класс приложения КУМИР */
class Application
	: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QStringList languageList READ languageList)
	Q_PROPERTY(QString currentLanguage READ currentLanguage)
	public:
		/**
	   * Конструктор приложения
	   * @param argc количество аргументов командной строки
	   * @param argv[] аргументы командной строки
		 */
		Application();
		static void createInstance();
            public slots:
		void createCompiler();
		void createRuntime();
		void createSettings();
		void createMessagesProvider();
		int compileText(const QString &text,
						const QTextCodec *messagesCodec);
		int evaluateProgram(QTextCodec *inCodec,
							QTextCodec *outCodec,
							QTextCodec *errCodec,
							QFile *inFile);


public:
		static Application* instance();
		/** Возвращает список доступный языков */
		QStringList languageList();
		/** Возвращает текущий язык */
		QString currentLanguage();
		/**
		 * Инициализирует язык приложения в соответствии с настройками.
		 * При этом вызывает установку языка для всех компонентов
		 */

		void setLanguage();
		/** Ссылка на объект компилятора */
		Kum::Compiler *compiler;
		/** Ссылка на объект компилятора Kum::Run */
		Kum::Run *run;
		/** Ссылка на поставщик текстов сообщений об ошибках */
		MessagesProvider *messagesProvider;
		/** Ссылка на поток вывода для пакетного режима */
		QFile *outFile;
		QString output;
		QFile *marginFile;

		QString errorText;
		void debugMsg(QString msg);
	public slots:
		void handleConsoleOutput(const QString &text);
		QStringList handleConsoleInput(const QString &format, QString &error);
		void handleConsoleError(const QString &text);
		/** Вызов компиляции всего что есть.
		* Вызывать перед выполнением или при изменении списка исполнителей
		*/
		void doCompleteCompilation();
		/**
		 * Компиляция текста программы из определенной вкладки.
		 * Если номер вкладки не указан или равен -1, то выполняется
		 * компиляция всех текстов.
		 * @param tabNo номер вкладки (нумерация с 0).
		 */
		void doCompilation(const QString &text);

	protected:
		QTextStream *m_inputStream;
		QTextCodec *m_inCodec;
		QTextCodec *m_outCodec;
		QTextCodec *m_errCodec;
		/** Список доступных языков */
		QStringList m_languageList;
		/** Текущий язык КУМИРа */
		QString m_currentLanguage;
		/** Ссылка на объект переводчика */
		QList<QTranslator*> translators;

		QStringList eventsHistory;
		static Application *m_instance;



};

Application* app();

#endif
