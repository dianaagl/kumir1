#ifndef KUM_SINGLE_MODULE_H
#define KUM_SINGLE_MODULE_H

#include <QtCore>

#include "enums.h"
#include "kum_tables.h"
#include "symboltable.h"

class KumInstrument;

namespace Kum {
	class SymbolTable;
}

class KumSingleModule
		: public QObject
{
	Q_OBJECT
public:
	explicit KumSingleModule (
			int moduleId,
			QObject *parent = 0,
			const QString &Name="No Name",
			IspType Type=KumSystem );
	KumInstrument* Instrument; //Сылка на обьект

	void setInstrument ( KumInstrument* instrument );
	KumInstrument * instrument();
	inline bool isEnabled() { return enabled; }
	inline Kum::SymbolTable* globals() { return m_globals; }
	void syncTables();
	~KumSingleModule();
public:
	inline void setEnabled ( bool flag ) { enabled=flag; }

	inline bool Graphics() {
		return graphics;
	}
	/**
		* Установить тип исполнителя
		* @param mType Тип исполнителя
		*/
	void setType ( IspType mType );

	symbol_table* Values();

	inline function_table* Functions() { return functions; }
	QList<ProgaText> * Proga_Text();

	QList<proga> * Proga_Value();


	inline void setProgaValue(const QList<proga> &v) { proga_value = v;	}

	inline void setProgaValueAt(int i, const proga &v ) {
		K_ASSERT ( i < proga_value.count() );
		if ( i < proga_value.count() ) {
			proga_value[i] = v;
		}
	}

	inline int bytecodeSize() const {
		return proga_value.count();
	}

	inline void appendBytecode(const QList<proga> &v) {
		proga_value += v;
	}

	inline void setInitialIP(int ip) {
		m_initialIP = ip;
	}

	inline int initialIP() const {
		return m_initialIP;
	}

	inline void removeDummyLines() {
		for ( int i=0; i<proga_value.count(); i++ ) {
			if ( proga_value[i].real_line.lineNo==-2 ) {
				proga_value.removeAt(i);
				i--;
			}
		}
	}

    inline int progaCount() const { return proga_text.count(); }
    inline int progaValueCount() const { return proga_value.count(); }
	ProgaText * Proga_Text_Line ( int i )
	{
//		if ( type!=kumIsp ) qWarning ( "Proga_Text_Line:type!=kumIsp." );
		if ( ( i<0 ) || ( i>proga_text.count() ) )
		{
//			qWarning ( "Proga_Text_Line:ERROR:no such line!" );

		};
		return &proga_text[i];
	};

	proga  Proga_Value_Line ( int i )
	{
		if ( type!=kumIsp ) qWarning ( "Proga_Value_Line:type!=kumIsp." );
		if ( ( i>proga_value.count() ) )
		{
//			int debug=proga_value.count();
//			qWarning() << "Proga_Value_Line:ERROR:no such line: " << debug;

		}
		if ( i>=0 )
			return proga_value[i];
		else {
			i = -i;
			int baseStr = i / 10;
			int subStr = (i%10)-1;
			return proga_value[baseStr].generatedLinesAfter[subStr];
		}
	};
	/**Устанавливает начало и конец исполнителя
		* в исходном тексте - вкладки или файла
		* @param start первая строка исполнителя
		* @param end Последняя строка исполнителея
		*/
	void setRange ( int start,int end )
	{
//		if ( type!=kumIsp ) qWarning ( "KumSingleModule:: setRange type!=kumIsp" );


//		if ( start < end ) qWarning ( "KumSingleModule::start < end" );

		from=start;
		to=end;
	};

	void Clean()
	{
		values.clear();//TODO Check me
		values.append ( new symbol_table() );
		functions->free ( 0 );
		proga_text.clear();
		proga_value.clear();
	};
	QString file()
	{
		return File;
	};
public:bool isInternal ()
	{
		if ( type==internal ) return true;else return false;
	};

public:bool isKumIsp ( )
	{
		if ( type==kumIsp ) return true;else return false;
	};



public:bool isNetwork ( )
	{
		if ( type==network ) return true;else return false;
	};

public:bool isSystem ( )
	{
		if ( type==KumSystem ) return true;else return false;
	};
public:bool isFriend ( )
	{
		if ( type==friendIsp) return true;else return false;
	};

	QString Name()
	{
		return name;
	};
	void setGraphics ( bool flag )
	{
		graphics=flag;
	};
	void CopyValues()
	{
		values.append ( values.first()->deepCopy() );
	};
	void localRestoreValues ( QString name )
	{
		values.first()->localRestore ( name,values.last() );
		delete values.last();
		values.removeLast();
		K_ASSERT ( values.count() >0 );
	};
	void setProgaText ( QList<ProgaText> Proga_text )
	{
		proga_text=Proga_text;
	};
	void setFile ( QString fileName )
	{
		File=fileName;
	};
	int IntErr;
	void setError(int err)
	{
		IntErr=err;
	};
	QString name;

	QString iconFile;


	void setBinFile(QString name)
	{
		bin_file=name;
	};

	QList<QVariant> convertKVSE2QVariant(QList<KumValueStackElem>* KVSE);
	void connectSig();
	void disconectSig();
public slots:
	void runFunc(QString name,QList<KumValueStackElem>* arguments,int* err);
	void syncSlot();
	void SwitchIspMode(int mode);
signals:
	void sync();
	void IspError(QString text);
private:
	int i_moduleId;
	Kum::SymbolTable* m_globals;
	QString File;
	IspType type;
	bool enabled;
	bool compiled;
	int m_initialIP;
	bool graphics;
    QList<symbol_table*> values;
	function_table* functions;
	QList<proga> proga_value;
	QList<ProgaText> proga_text;
	int from,to;
	QString bin_file;

	QString m_pluginCallName;
};


#endif
