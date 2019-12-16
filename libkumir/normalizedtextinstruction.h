#ifndef NORMALIZEDTEXTINSTRUCTION_H
#define NORMALIZEDTEXTINSTRUCTION_H

#include <QtCore>
#include "enums.h"

struct NormalizedTextInstruction {
	QString filename;
	QString text;
	int sourcePosition;
	int sourceLength;
	Kumir::InstructionType instructionType;
	int errorStart; // Error position in normalized string!
	int errorLength; // Error position in normalized string!
	int errorCode;
	QPair<int,int> indentRank;
	QVector<int> P;
	QVector<int> L;
};

#endif // NORMALIZEDTEXTINSTRUCTION_H
