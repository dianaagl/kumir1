#ifndef FUNCTIONTABLE_H
#define FUNCTIONTABLE_H

#include <QtCore>
#include "enums.h"

struct Argument
{
    Kumir::ValueType type;
    int dimension;
    bool res;
    bool argres;
    QString name;
    int linkId;
};

struct f_table
{
	f_table()
	{
		name = "";
		doc = "";
		str = -1;
		proga_str = -1;
		ends = -1;
		type = Kumir::Undefined;
		disabled = false;
		badalg = false;
		teacher = false;
	}
	QString name;
	//QString isp;
	QString doc;
	int str;
	int proga_str;
	int ends;
	//	Kumir::ValueType perems[MAX_ARG];
	//	int razm[MAX_ARG]; // Размерность аргументор-массивов
	//	bool is_res[MAX_ARG];
	//	bool is_argRes[MAX_ARG];
	//	int nperem;
	//	int arg_perem_id[MAX_ARG];
	//	QString arg_name[MAX_ARG];
	QList<Argument> l_arguments;
	Kumir::ValueType type;
	bool disabled;
	bool badalg;
	QString xmlData;
	QString xmlHeader;
	bool runStepInto;
	int outboundAlgorhitmIP;
	bool teacher;
};

class FunctionTable
{
public:
	FunctionTable();
	QString xmlData(class symbol_table *st);
	QString tempXmlData(int id) const;
	void setTempXmlData(int id, const QString &v);
	int size() const;
	bool isBroken(int id) const;
	void setBroken(int id, bool v);
	QString docString(int id) const;
	void setDocString(int id, const QString &v);
	bool isRunStepInto(int id) const;

	void addFunction ( const QString &name, int str );
	void addInternalFunction ( const QString &name, Kumir::ValueType type, uint nperem ); //добавление встроенной функции
	int append(const QString &name, Kumir::ValueType returnType, int declPos);
	int appendDummy();
	int appendArg(int funcId, Kumir::ValueType argType,
				  int argDim, int isRes, int isArgRes,
				  int argPeremId);
	int argPeremId(int funcId, int argId) const;
	QString nameByPos( int pos ) const; //Возвращает имя алгоритма по номеру строки - global если такого алгоритма нет
	int idByPos( int pos ) const;
	int posById(int id) const;
	int bytePosById(int id) const;
	int idByName ( const QString &name ) const; //возвращает номер вункции в таблице или -1 если функции нет
	QString nameById ( int id ) const;

	bool isRes( int id,int arg_id ) const;
	void setRes(int id, int arg_id, bool v);
	bool isArgRes( int id,int arg_id ) const;

	void free ( int base ); //отчистка
	int declPosById ( int id ) const; //возвращает номер строки описания функции
	void setEndPos ( int id, int pos ); //устанавливает конец алгоритма
	Kumir::ValueType typeById ( int id ) const; //возвращает тип функции
	int argCountById ( int id ) const;

	Kumir::ValueType argTypeById( int id, int argNo ) const;
	int argDimById(int id, int argNo) const;
	void setArgDim(int id, int argNo, int dim);
	void setArgType ( int funcNo,int argId,Kumir::ValueType argType );
	void setRunStepInto(int funcId, bool v);
	bool isTeacher(int id) const;
    void setTeacher(int id,bool flag);

private:
	QList<f_table> l_table;
};

#endif // FUNCTIONTABLE_H
