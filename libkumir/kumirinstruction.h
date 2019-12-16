#ifndef KUMIRINSTRUCTION_H
#define KUMIRINSTRUCTION_H

#include <QtCore>

#include "enums.h"

struct KumirInstruction {

	KumirInstruction();

	/**Нормализованная строка
		 */
	QString line;

	/**Тип строки
		 */
	Kumir::InstructionType str_type;

	/**Счетчик проходов по строке
		 */
	int counter;

	/**Строка прыжка
		 */
	int else_pos;

	/** Ссылка на реальную строку в тексте
		 */
	int source_filename;
	int source_position;
	int source_length;
	int real_line_begin;
	int error;
	int err_start;
	int err_length;
	int forceJump;



	/**Список выражений в строке
		 */
	QList<QStringList> VirajList;
	/**Типы выражений
		 */
	QList<Kumir::ValueType> VirajTypes;
	bool is_break;
	QList<KumirInstruction> generatedLinesAfter;
	QStringList extraSubExpressions;

};


#endif // KUMIRINSTRUCTION_H
