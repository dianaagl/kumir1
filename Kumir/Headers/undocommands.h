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
#ifndef UNDOCOMMANDS_H
#define UNDOCOMMANDS_H

class TextEdit;

/** Команда ввода символа */
class KeyCharectCommand
	: public QUndoCommand
{
	public:
		/**
		 * Конструктор
		 * @param editor ссылка на редактор
		 * @param text текст
		 */
		KeyCharectCommand(TextEdit *editor, const QString text);
		/**
		 * Код команды
		 * @return всегда возвращает 0
		 */
		int id();
		/**
		 * Отмена действия
		 */
		void undo();
		/**
		 * Повторение действия
		 */
		void redo();
		/**
		 * Сжатие команд - объединение с другой командой
		 * @param other ссылка на другую команду
		 * @return true, если объединение успешно или false в ином случае
		 */
		bool mergeWith ( const QUndoCommand * other );
	private:
		/** Позиция курсора */ 
		uint position;
		/** Количество удаленных символов, если было выделение */
		uint removeSize;
		/** Флаг "было выделение" */
		bool hasSelection;
		/** Выделенный текст до действия команды */
		QString selectedText;
		/** Начало выделения */
		uint selectionStart;
		/** Конец выделения */
		uint selectionEnd;
		/** Текст команды */
		QString text;
		/** Ссылка на редактор */
		TextEdit *editor;
		bool overwrite;
		QString prevCharect;
		
};

/** Команда ввода Enter */
class KeyEnterCommand
	: public QUndoCommand
{
	public:
		/**
		 * Конструктор
		 * @param editor ссылка на редактор
		 */
		KeyEnterCommand(TextEdit *editor);
		/**
		 * Код команды
		 * @return всегда возвращает 1
		 */
		int id();
		/**
		 * Отмена действия
		 */
		void undo();
		/**
		 * Повторение действия
		 */
		void redo();
	private:
		/** Позиция курсора */ 
		uint position;
		/** Флаг "было выделение" */
		bool hasSelection;
		/** Выделенный текст до действия команды */
		QString selectedText;
		/** Начало выделения */
		uint selectionStart;
		/** Конец выделения */
		uint selectionEnd;
		/** Ссылка на редактор */
		TextEdit *editor;
		QString toInsert;
};

/** Команда нажатия Delete */
class KeyDeleteCommand
	: public QUndoCommand
{
	public:
		/**
		 * Конструктор 
		 * @param editor ссылка на редактор
		 */
		KeyDeleteCommand(TextEdit *editor);
		/**
		 * Код команды
		 * @return всегда возвращает 2
		 */
		int id();
		/**
		 * Отмена действия
		 */
		void undo();
		/**
		 * Повторение действия
		 */
		void redo();
	private:
		/** Позиция курсора */ 
		uint position;
		/** Флаг "было выделение" */
		bool hasSelection;
		/** Выделенный текст до действия команды */
		QString selectedText;
		/** Начало выделения */
		uint selectionStart;
		/** Конец выделения */
		uint selectionEnd;
		/** Ссылка на редактор */
		TextEdit *editor;
		/** Текст команды */
		QString text;
};

/** Команда "Вырезать текст" */
class CutCommand
	: public QUndoCommand
{
	public:
		/**
		 * Конструктор
		 * @param editor ссылка на редактор
		 */
		CutCommand(TextEdit *editor);
		/**
		 * Код команды
		 * @return всегда возвращает 3
		 */
		int id();
		/**
		 * Отмена действия
		 */
		void undo();
		/**
		 * Повторение действия
		 */
		void redo();
	private:
		/** Позиция курсора */ 
		uint position;
		/** Выделенный текст до действия команды */
		QString selectedText;
		/** Начало выделения */
		uint selectionStart;
		/** Конец выделения */
		uint selectionEnd;
		/** Ссылка на редактор */
		TextEdit *editor;
};

/** Команда нажатия BackSpace */
class KeyBackspaceCommand
	: public QUndoCommand
{
	public:
		/**
		 * Конструктор
		 * @param editor ссылка на редактор
		 */
		KeyBackspaceCommand(TextEdit *editor);
		/**
		 * Код команды
		 * @return всегда возвращает 4
		 */
		int id();
		/**
		 * Отмена действия
		 */
		void undo();
		/**
		 * Повторение действия
		 */
		void redo();
	private:
		/** Позиция курсора */ 
		uint position;
		/** Флаг "было выделение" */
		bool hasSelection;
		/** Выделенный текст до действия команды */
		QString selectedText;
		/** Начало выделения */
		uint selectionStart;
		/** Конец выделения */
		uint selectionEnd;
		/** Ссылка на редактор */
		TextEdit *editor;
		/** Текст команды */
		QString text;
};

/** Команда вставки текста */
class InsertCommand
	: public QUndoCommand
{
	public:
		/**
		 * Конструктор
		 * @param editor ссылка на редактор
		 * @param txt вставленный текст
		 */
		InsertCommand(TextEdit *editor, QString txt);
		/**
		 * Код команды
		 * @return всегда возвращает 5
		 */
		int id();
		/**
		 * Отмена действия
		 */
		void undo();
		/**
		 * Повторение действия
		 */
		void redo();
	private:
		/** Ссылка на редактор */
		TextEdit *editor;
		/** Текст команды */
		QString txt;
		/** Позиция курсора */ 
		uint position;
		bool hasSelection;
		uint selectionStart, selectionEnd;
		QString selText;
};

/** Команда комментирования выделения или строки */
class CommentCommand
	: public QUndoCommand
{
	public:
		/**
		 * Конструктор
		 * @param editor ссылка на редактор
		 * @param from номер строки, с которой начинается комментирование
		 * @param to номер строки, на которой заканчивается комментирование
		 */
		CommentCommand(TextEdit *editor, uint from, uint to);
		/**
		 * Код команды
		 * @return всегда возвращает 6
		 */
		int id();
		/**
		 * Отмена действия
		 */
		void undo();
		/**
		 * Повторение действия
		 */
		void redo();
	private:
		/** Позиция курсора */ 
		uint position;
		/** Ссылка на редактор */
		TextEdit *editor;
		/** Номер строки, с которой начинается комментирование */
		uint from;
		/** Номер строки, на которой заканчивается комментирование */
		uint to;
};

/** Команда разкомментирования выделения или строки */
class UncommentCommand
	: public QUndoCommand
{
	public:
		/**
		 * Конструктор
	   * @param editor ссылка на редактор
	   * @param from номер строки, с которой начинается разкомментирование
	   * @param to номер строки, на которой заканчивается разкомментирование
		 */
		UncommentCommand(TextEdit *editor, uint from, uint to);
		/**
		 * Код команды
		 * @return всегда возвращает 7
		 */
		int id();
		/**
		 * Отмена действия
		 */
		void undo();
		/**
		 * Повторение действия
		 */
		void redo();
	private:
		/** Позиция курсора */ 
		uint position;
		/** Количество пробелов, которые были удалены в начале строки */
		uint spacesCount;
		/** Ссылка на редактор */
		TextEdit *editor;
		/** Номер строки, с которой начинается комментирование */
		uint from;
		/** Номер строки, на которой заканчивается комментирование */
		uint to;
};

class SetTextCommand
	: public QUndoCommand
{
	public:
		SetTextCommand ( TextEdit *editor, const QString & txt );
		int id();
		void undo();
		void redo();
	private:
		QString newText;
		QString oldText;
		int newPos;
		int oldPos;
		TextEdit *editor;
		QTextCursor cursor;
};

class InsertLineCommand
	: public QUndoCommand 
{
	public:
		InsertLineCommand ( TextEdit *editor, uint no, const QString & txt );
		int id();
		void undo();
		void redo();
	private:
		QString txt;
		int lineNo;
		TextEdit *editor;
		QTextCursor cursor;
};

class ChangeLineTextCommand
	: public QUndoCommand 
{
	public:
		ChangeLineTextCommand ( TextEdit *editor, uint no, const QString & txt );
		int id();
		void undo();
		void redo();
	private:
		QString txt;
		QString oldText;
		int lineNo;
		TextEdit *editor;
		QTextCursor cursor;
};

class FastInsertTextCommand
	: public QUndoCommand
{
	public:
		FastInsertTextCommand( TextEdit *editor, const QString & txt , int removeCount);
		int id();
		void undo();
		void redo();
	private:
		QString txt;
		int removeCount;
		QString removeTxt;
		QTextCursor cursor;
		TextEdit *editor;
};

#endif
