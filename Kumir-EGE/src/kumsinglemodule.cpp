#include "kumsinglemodule.h"
#include "kum_tables.h"
#include "kum_instrument.h"


using namespace Kum;

KumSingleModule::KumSingleModule (
		int moduleId,
		QObject *parent,
		const QString &Name,
		IspType Type )
{
	Q_UNUSED(parent);
	i_moduleId = moduleId;
	enabled=false;
	compiled=false;
	graphics=false;
	m_globals = 0;
	Instrument=NULL;
	values.append ( new symbol_table() );
	functions = new function_table(moduleId);
	functions->free ( 0 );
	proga_value.clear();
	name=Name;
	File="";
	type=Type;
    IntErr=0;
    m_initialIP = 0;
}

KumSingleModule::~KumSingleModule()
{
    delete functions;
}


void KumSingleModule::runFunc(QString name,QList<KumValueStackElem>* arguments,int* err)
{
    KumInstrument* instrum=instrument();
    instrum->runFunc(name,arguments,err);
}

QList<QVariant> KumSingleModule::convertKVSE2QVariant(QList<KumValueStackElem>* KVSE)
{
    QList<QVariant> temp;
    for(int i=0;i<KVSE->count();i++)
	{
        temp<<KVSE->value(i).asVariant();
	};
    return temp;
};
void KumSingleModule::connectSig()
{
    connect(instrument(),SIGNAL(sync()),this,SLOT(syncSlot()));

};
void KumSingleModule::disconectSig()
{if(!isFriend())
	{
        disconnect(instrument(),SIGNAL(sync()),this,
                   SLOT(syncSlot()));
	}else
	{

	};
};
void KumSingleModule::syncSlot()
{
    emit sync();
}


void KumSingleModule::SwitchIspMode(int mode)
{
    instrument()->SwitchMode(mode);
}


void KumSingleModule::setInstrument(KumInstrument *instrument)
{
	Instrument=instrument;
}

KumInstrument* KumSingleModule::instrument()
{
	if ( type!=internal ) return  NULL;
	return Instrument;
}

void KumSingleModule::setType(IspType mType)
{
	if ( mType==kumIsp ) //Текстовый
	{
		Instrument =NULL;
	}
	type=mType;
}

symbol_table* KumSingleModule::Values()
{
	if(values.count()==0){
//		qWarning ( "NO Values in module!!!." );
		return NULL;
	}
	return values.first();
}

QList<ProgaText> * KumSingleModule::Proga_Text()
{
//	if ( type!=kumIsp )
//		qWarning ( "Proga_Text:type!=kumIsp." );
	return &proga_text;
}

QList<proga> * KumSingleModule::Proga_Value()
{
//	if ( type!=kumIsp ) qWarning ( "Proga_Text:type!=kumIsp." );
	return &proga_value;
}


void KumSingleModule::syncTables()
{
	// TODO remove when Compiler will be ready to work with new tables directly
	if (!m_globals) {
		m_globals = new SymbolTable(i_moduleId, -1, "");
		m_globals->setMappedIdMode(true);
	}
	symbol_table* st = Values();
	for (int i=0; i<=st->count; i++) {
		symbol_table::s_table e = st->symb_table[i];
		SymbolTable *newTable = 0;
		if (e.alg=="global") {
			newTable = m_globals;
		}
		else {
			int funcId = Functions()->idByName(e.alg);
			newTable = Functions()->locals(funcId);
			newTable->setMappedIdMode(true);
		}
		if (e.is_const) {
			int id = newTable->addConstant(e.type, e.value->data[0]);
			newTable->setMapId(id, i);
		}
		else if (e.is_Arg) {
			int id = newTable->addArgument(e.type,
										   e.name,
										   e.constantValue,
								  e.is_Res,
								  e.is_ArgRes,
								  e.razm,
								  e.size0_start,
								  e.size0_end,
								  e.size1_start,
								  e.size1_end,
								  e.size2_start,
								  e.size2_end);
			newTable->setMapId(id, i);
		}
		else if (e.is_FunctionReturnValue) {
			int funcId = Functions()->idByName(e.name);
			SymbolTable * locals = Functions()->locals(funcId);
			if (locals) {
				int id = locals->addReturnValue(e.type);
				locals->setMappedIdMode(true);
				locals->setMapId(id, i);
			}
		}
		else {
			int id = newTable->addVariable(e.type,
										   e.name,
										   e.constantValue,
										   e.razm,
										   e.size0_start,
										   e.size0_end,
										   e.size1_start,
										   e.size1_end,
										   e.size2_start,
										   e.size2_end);
			newTable->setMapId(id, i);
			newTable->setInitialValue(id, e.value->data);
		}
	}
}
