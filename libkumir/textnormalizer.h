/****************************************************************************
**
** Copyright (C) 2004-2010 NIISI RAS. All rights reserved.
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
#ifndef TEXT_NORMALIZER_H
#define TEXT_NORMALIZER_H

#include <QtCore>

#include "enums.h"
#include "normalizedtextinstruction.h"

class TextNormalizer: public QObject
{
	Q_OBJECT
public:
	TextNormalizer();
	void initNormalizator(const QString &fileName);
	void init();
	inline void setAllowHiddenNames(bool v) { m_allowHiddenNames = v; }
	int test_skobk ( QString stroka );
	Kumir::InstructionType getStringType ( const QString &str );
	int ks_count ( QString stroka,int begin,int end );
	int getPosByKS ( int ks_id,QString stroka );
	int getKSCount ( int pos,QString stroka );
	QString *ispKwd;
	QString *useKwd;

	bool isBooleanConstant(const QString &text) const;
	void setExtraCharset(const QString &charsetSpec);
	bool isValidChar(const QChar &ch) const;

	int test_name ( QString stroka,int &pos, int &len );
	QString SplitPrisvBlock ( const QStringList &p_List,
							  QStringList &p_Result,
							  int p_Num, bool p_FuncParam );

	int splitFStringList ( const QStringList FSpisok,
						   QList<NormalizedTextInstruction> &result,
						   int str, int ID, int offset,
						   const QString &file,
						   const QList<QList<int> > &PP,
						   const QList<QList<int> > &LL );

	static QStringList splitElse ( const QString &normal_else_string );
	static QStringList splitTo ( const QString &normal_to_string );
	bool allowIndent( const int str_type, const int err_code );
	QPair<int,int> getIndentRank( const NormalizedTextInstruction &ptl);
	QPair<int,int> getIndentRank( const QList<NormalizedTextInstruction> &ptl);

	int splitUserLine ( const QString &text, QPair<int,int> &errorPosition, QList<NormalizedTextInstruction> &result );
	int splitUserFile ( QFile &file, QList< QList<NormalizedTextInstruction> > &result );

	QVector<bool> recalculateErrorPositions ( const QString &text, const QList<NormalizedTextInstruction> &ptl );
	bool inLit ( const QString &s, int pos );

	/**
		 * Нормализация строки
		 * @param src IN: строка исходного Кумир-текста
		 * @param P OUT: вектор начальных позиций нормализованных символов в исходной строке
		 * @param L OUT: вектор длин нормализованных символов в исходной строке
		 * @param error OUT: код ошибки при нормализации
		 * @param err_start OUT: позиция начала ошибки в исходной строке
		 * @param err_len OUT: длина ошибочной области в исходной строке
		 * @return нормализованная строка
		 */
	QString normalize(const QString & src, QList<int> &P, QList<int> &L, int &error, int &err_start, int &err_len);
	void trim(QString &s, QList<int> &P, QList<int> &L ) ;
	QRegExp *rxKeyWords;
	QRegExp *rxTypes;
	QRegExp *rxConst;
	QStringList getKeyWords();
	QStringList getTypeNames();
	QStringList getConstNames();

private:
	QHash<QString,QString> *kwdMap;
	QRegExp *rxCompound;
	void initNormalizator();
	QRegExp rxLitBreaks;
	QString Breaks;
	QRegExp brks;
	QString zamena ( QString string );
	QRegExp rx_NOT;
	QRegExp rx_OR;
	QRegExp rx_AND;
	QRegExp rx_INT_TAB;
	QRegExp rx_STRING_TAB;
	QRegExp rx_REAL_TAB;
	QRegExp rx_CHAR_TAB;
	QRegExp rx_BOOL_TAB;
	QRegExp rx_INT;
	QRegExp rx_STRING;
	QRegExp rx_REAL;
	QRegExp rx_CHAR;
	QRegExp rx_BOOL;
	QRegExp rx_IF;
	QRegExp rx_THEN;
	QRegExp rx_ELSE;
	QRegExp rx_DONE;
	QRegExp rx_SWITCH;
	QRegExp rx_CASE;
	QRegExp rx_ALG;
	QRegExp rx_ARG;
	QRegExp rx_BEGIN;
	QRegExp rx_END;
	QRegExp rx_LOOP;
	QRegExp rx_ENDLOOP;
	QRegExp rx_FOR;
	QRegExp rx_TIMES;
	QRegExp rx_WHILE;
	QRegExp rx_NEWLINE;
	QRegExp rx_MODULE;
	QRegExp rx_ENDMODULE;
	QRegExp rx_PRE;
	QRegExp rx_POST;
	QRegExp rx_EXIT;

	QRegExp rx_FROM;
	QRegExp rx_TO;
	QRegExp rx_STEP;
	QRegExp rx_RES;
	QRegExp rx_ARGRES;
	QRegExp rx_OUTPUT;
	QRegExp rx_TRUE;
	QRegExp rx_FALSE;
	QRegExp rx_ASSERT;

	QRegExp *KSIMS;//10-08-Mordol
	QString st_INT;
	QString st_STRING;
	QString st_BOOL;
	QString st_CHAR;
	QString st_REAL;

	QStringList compounds;
	QStringList compounds2;
	QStringList keyWords;
	QStringList typeNames;
	QStringList constNames;

	QList<ushort> m_extraChars;
	QList< QPair<ushort,ushort> > m_extraCharRanges;
	bool m_allowHiddenNames;

};

#endif // TEXT_NORMALIZER_H
