#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <QtCore>
#include "enums.h"

struct PeremPrt
{
    int mod_id;
    int perem_id;
};

class perem_value
{
public: perem_value ( int id,
					  Kumir::ValueType type,
					  int size0_start, int size0_end,
					  int size1_start, int size1_end,
					  int size2_start, int size2_end,
					  int razm = 0 );
	void init( int id,
			   Kumir::ValueType type,
			   int size0_start, int size0_end,
			   int size1_start, int size1_end,
			   int size2_start, int size2_end,
			   int razm = 0 );
public:~perem_value();


public:int getValueAsInt();
public:bool getValueAsBool();
public:double getValueAsFloat();
public:QString getValueAsString();
	QChar getValueAsCharect();
public:void  setIntegerValue ( int value );
public:void  setFloatValue ( double value );
public:void  setBoolValue ( bool value );
public:void  setStringValue ( QString value );
	void setCharectValue ( QChar value );
public:int  initMass ( int id );
public:void  setIntMassValue ( int value,int x,int y,int z );
public:void  setStringMassValue ( QString value,int x,int y,int z );
public:void  setBoolMassValue ( bool value,int x,int y,int z );
public:void  setFloatMassValue ( double value,int x,int y,int z );
public:void  setCharMassValue ( QChar value,int x,int y,int z );
public:int   getIntMassValue ( int x,int y,int z );
public:QString  getStringMassValue ( int x,int y,int z );
public:double   getFloatMassValue ( int x,int y,int z );
public:int   getBoolMassValue ( int x,int y,int z );
public:QChar   getCharMassValue ( int x,int y,int z );
	bool isReady ( int x,int y, int z );
	void setReady ( int x, int y, int z, bool value );
	//
	//public:int  setFloatValue(double value);

public: QVector<QVariant> data;
	// bool *massReady;
	//                QVector<bool> massReady;
public:Kumir::ValueType p_type;
private:bool initd;
private:int size_x;
private:int size_y;
private:int size_z;
	int razmer;

};


class SymbolTable
{
	/**
			 * Данные переменной
			 */
	struct s_table
	{
		inline s_table()
		{
			name = "";
			type = Kumir::Undefined;
			alg = "";
			value = NULL;
			size0_start = 0;
			size0_end = 0;
			size1_start = 0;
			size1_end = 0;
			size2_start = 0;
			size2_end = 0;
			str = -1;
			razm = 0;
			is_const = false;
			ready = false;
			used = false;
			is_Res = false;
			is_ArgRes = false;
			is_Arg = false;
			is_FunctionReturnValue = false;
		}
		QString name;
		Kumir::ValueType type;
		QString alg;
		//QString isp;
		perem_value *value;
		int size0_start;
		int size0_end;
		int size1_start;
		int size1_end;
		int size2_start;
		int size2_end;
		int str;
		int razm;
		bool is_const;
		bool ready;
		bool used;
		bool is_Res;
		bool is_ArgRes;
		bool is_Arg; // является арг, рез или аргрез-аргументом функции
		//int pointer;
		PeremPrt pointer;
		bool is_FunctionReturnValue;
		QStringList bounds_xml;

	};

public: QString xmlData(const QString &ns, const QString &kind);
public: SymbolTable();
	~SymbolTable();
public: int add_symbol ( QString name,int str,Kumir::ValueType type,QString alg,bool p_Const );
public: SymbolTable* deepCopy();
public: int valueCopy ( int id,s_table value );
public: int localRestore ( QString alg,SymbolTable* from );
	//	public: int use_symbol ( QString name,int str,QString alg,s_val value );
public: int new_symbol ( int id );

public: int new_argument ( Kumir::ValueType type );

public: double asFloat ( int id );
public: int asInt ( int id );

public: int init ( uint id );
public: int copyMassInt ( uint from,uint to );
public: int copyMassIntArg ( uint from,uint to,SymbolTable* arg_table );

public: int copyMassString ( uint from,uint to );
public: int copyMassStringArg ( uint from,uint to,SymbolTable* arg_table );


public: int copyMassCharect ( uint from,uint to );
public: int copyMassCharectArg ( uint from,uint to,SymbolTable* arg_table );

public: int copyMassBool ( uint from,uint to );
public: int copyMassBoolArg ( uint from,uint to,SymbolTable* arg_table );


public: int copyMassReal ( uint from,uint to );
public: int copyMassRealArg ( uint from,uint to,SymbolTable* arg_table );


public: int drop ( uint id );
public: bool isReady ( uint id )
	{
		return symb_table[id].ready;
	};

	bool isReady ( uint id, int x, int y, int z )
	{
		return symb_table[id].ready && symb_table[id].value->isReady ( x,y,z );
	}

public: bool isUsed ( uint id )
	{
		return symb_table[id].used;
	};
public: bool isRes ( uint id )
	{
		return symb_table[id].is_Res;
	};

public: bool isArgRes ( uint id )
	{
		return symb_table[id].is_ArgRes;
	};

public: PeremPrt pointTo ( uint id )
	{
		return symb_table[id].pointer;
	};

public: void setUsed ( uint id,bool use )
	{
		//  if((id==39))
		//  {
		//  qWarning("ID 39 set.");
		//  };
		symb_table[id].used=use;
	};
public: void setPointer ( uint id,int module,int point )
	{
		Q_ASSERT(point>-1);
		Q_ASSERT(module>-1);
		symb_table[id].pointer.mod_id=module;
		symb_table[id].pointer.perem_id=point;
	};

public: PeremPrt getPointer ( uint id )
	{
		return symb_table[id].pointer;
	};
public: int getRazmById ( uint id )
	{
		return symb_table[id].razm;
	};

public: int free ( int base_symb );
public: bool isConst ( int id );
public: void setConst ( int id,bool isconst );

public: int inTable ( QString name,QString alg ); //возвращает ID переменной
public: int getStrByID ( int id ); //возвращает номер строки описания переменной
public: Kumir::ValueType getTypeByID ( int id );
public: s_table symb_table[512];
public: int count;


public: QString getValueAsString ( int id )
	{
		return symb_table[id].value->getValueAsString();
	};

public: QChar getValueAsCharect ( int id )
	{
		return symb_table[id].value->getValueAsCharect();
	};

public: int getValueAsInt ( int id )
	{
		return symb_table[id].value->getValueAsInt();
	};

public: bool getValueAsBool ( int id )
	{
		return symb_table[id].value->getValueAsBool();
	};


public: double getValueAsFloat ( int id )
	{
		return symb_table[id].value->getValueAsFloat();
	};

public:void resetInitFlag(QString alg_name)
	{

		for(int i=0;i<=count;i++)
		{
			if((symb_table[i].alg==alg_name)&&(!symb_table[i].is_const))symb_table[i].ready=false;
		};
	};


private:
	//isp_table* isp_Table_ptr;
};

#endif // SYMBOLTABLE_H
