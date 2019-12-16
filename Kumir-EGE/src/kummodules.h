#ifndef KUMMODULES_H
#define KUMMODULES_H

#include <QtCore>

class KumSingleModule;
class ProgramTab;
class KumInstrument;

#include "kum_tables.h"

class KumModules : public QObject

{
	Q_OBJECT
public:

	explicit KumModules ( QObject *parent = 0 );
	int lastModuleId() const;
	int count() const;
	void enableModule ( int id );
	void disableModule ( int id );

	KumSingleModule* module ( int id );

	KumSingleModule* lastModule ();
	int appendMainModule ();
	int appendInternalModule ( const QString &name, KumInstrument* instrument );
	int idByName ( QString name );

	bool  isGraphics ( uint id );

	KumInstrument* Instrument ( uint id );

	void FuncByName ( const QString &name, int * moduleId,int *id );


	void ExtFuncByName ( QString name,int my_module_id,int * ext_moduleId,int *id );

	int FuncPerm ( int func_id,int moduleId );

	QString FuncName ( int func_id,int moduleId );
	PeremType FuncType ( int func_id,int moduleId );

	void removeModule ( int module_id );

	void Debug ();
	void setUsed(PeremPrt ptr,bool flag);
	bool isUsed(PeremPrt ptr);
	void tempDisable();
	void tempEnable();


	void setIntegerValue(PeremPrt ptr,int value);
	void setFloatValue(PeremPrt ptr,double value);
	void setBoolValue(PeremPrt ptr,bool value);
	void setCharValue(PeremPrt ptr,QChar value);
	void setStringValue(PeremPrt ptr,QString value);
	bool testNameConflict(int module_id);
	QString getStringValue(PeremPrt ptr);


signals:
	void sync();
private:
	QList<KumSingleModule*> Modules;
	symbol_table funcGlobPerem;
	QList<int> tempDisabled;
	bool plugins_ready;

};

#endif // KUMMODULES_H
