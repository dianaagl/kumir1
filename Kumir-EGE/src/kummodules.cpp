#include "kummodules.h"
#include "kumsinglemodule.h"
#include "kum_instrument.h"

KumModules::KumModules(QObject *parent) :
		QObject(parent)
{
	plugins_ready=false;
}

int KumModules::lastModuleId() const
{
	return Modules.size()-1;
}

int KumModules::count() const
{
	return Modules.size();
}

void KumModules::enableModule(int id)
{
	K_ASSERT(id>=0);
	if ( id>lastModuleId() )
	{
//		qWarning ( "KumModules:enable: id>lastModuleId()" );
		return;
	}
	Modules[id]->setEnabled ( true );
}

void KumModules::disableModule(int id)
{
	K_ASSERT(id>=0);
	if ( id>lastModuleId() )
	{
//		qWarning ( "KumModules:disbale: id>lastModuleId()" );
		return;
	}
	Modules[id]->setEnabled ( false );
}

KumSingleModule*  KumModules::module ( int id )
{
    KumSingleModule *result = NULL;
    for (int i=0; i<Modules.size(); i++) {
        KumSingleModule* module = Modules[i];
        if (i==id) {
            result = module;
        }
    }
    return result;
}

KumSingleModule* KumModules::lastModule()
{
	return Modules.last();
}

void KumModules::FuncByName(const QString &name, int *moduleId, int *id)
{
	for ( int i=0;i<Modules.count();i++ )
	{
		KumSingleModule *module = Modules[i];
		if ( module->isEnabled() )
		{
			*id=module->Functions()->idByName ( name );
			if ( *id>-1 )
			{
				*moduleId=i;
				return;
			};
		};
	};
	*moduleId=-1;
	*id=-1;
}



int KumModules::appendMainModule ()
{
	Modules.append (new KumSingleModule (Modules.size(),  0,"main",kumIsp ) );
	Modules.last()->setEnabled ( true );
	return Modules.count()-1;
};

int KumModules::appendInternalModule ( const QString &name,KumInstrument * instrument )
{
	Modules.append (new KumSingleModule (Modules.size(),  0,name,internal ) );

	Modules.last()->setInstrument ( instrument );
	Modules.last()->setEnabled ( true );
	return Modules.count()-1;
};


int KumModules::idByName ( QString name )
{
	for ( int i=0;i<Modules.count();i++ )
	{
		QString curName = Modules[i]->name;
		if ( curName==name ) return i;
	};
	return -1;
}


bool KumModules::testNameConflict(int module_id)
{
    for(int i=0;i<Modules[module_id]->Functions()->size();i++)
	{
        QString name=Modules[module_id]->Functions()->nameById(i);
        int mod_id,perem_id;
        ExtFuncByName(name,module_id,&mod_id,&perem_id);
        if((mod_id>0)&&(mod_id!=module_id))return false;
	};
    return true;
}



void KumModules::setIntegerValue(PeremPrt ptr,int value)
{
    K_ASSERT(Modules[ptr.mod_id]->isEnabled());
    Modules[ptr.mod_id]->Values()->symb_table[ptr.perem_id].value->setIntegerValue(value);
};
void KumModules::setFloatValue(PeremPrt ptr,double value)
{
    K_ASSERT(Modules[ptr.mod_id]->isEnabled());
    Modules[ptr.mod_id]->Values()->symb_table[ptr.perem_id].value->setFloatValue(value);
};
void KumModules::setBoolValue(PeremPrt ptr,bool value)
{
    K_ASSERT(Modules[ptr.mod_id]->isEnabled());
    Modules[ptr.mod_id]->Values()->symb_table[ptr.perem_id].value->setBoolValue(value);
};
void KumModules::setCharValue(PeremPrt ptr,QChar value)
{
    K_ASSERT(Modules[ptr.mod_id]->isEnabled());
    Modules[ptr.mod_id]->Values()->symb_table[ptr.perem_id].value->setCharectValue(value);
};
void KumModules::setStringValue(PeremPrt ptr,QString value)
{
    K_ASSERT(Modules[ptr.mod_id]->isEnabled());
    Modules[ptr.mod_id]->Values()->symb_table[ptr.perem_id].value->setStringValue(value);
};

QString KumModules::getStringValue(PeremPrt ptr)
{
    K_ASSERT(Modules[ptr.mod_id]->isEnabled());
    return Modules[ptr.mod_id]->Values()->symb_table[ptr.perem_id].value->getValueAsString();
}


bool KumModules::isGraphics(uint id)
{
	return Modules[id]->Graphics();
}

KumInstrument* KumModules::Instrument(uint id)
{
	return Modules[id]->instrument();
}

void KumModules::ExtFuncByName(QString name, int my_module_id, int *ext_moduleId, int *id)
{
	for ( int i=0;i<Modules.count();i++ )
	{
		if(i==my_module_id)continue;
		if (( Modules[i]->isEnabled() ))
		{
			*id=Modules[i]->Functions()->idByName ( name );
			if ( *id>-1 )
			{
				*ext_moduleId=i;
				return;
			};
		};
	};
	*ext_moduleId=-1;
	*id=-1;
}

int KumModules::FuncPerm(int func_id, int moduleId)
{
	if ( !Modules[moduleId]->isEnabled() )
	{
//		qWarning ( "Module %i disabled!",moduleId );
		return -1;
	};
	QString name=Modules[moduleId]->Functions()->nameById(func_id);
	return Modules[moduleId]->Values()->inTable ( name,"global" );
}

QString KumModules::FuncName(int func_id, int moduleId)
{
	if ( !Modules[moduleId]->isEnabled() )
	{
//		qWarning ( "Module %i disabled!",moduleId );
		return "";
	};
	return Modules[moduleId]->Functions()->nameById(func_id);
}

PeremType KumModules::FuncType(int func_id, int moduleId)
{
	if ( !Modules[moduleId]->isEnabled() )
	{
//		qWarning ( "Module %i disabled!",moduleId );
		return none;
	};
	return Modules[moduleId]->Functions()->typeById(func_id);
}



void KumModules::removeModule(int module_id)
{
	if ( module_id>=Modules.count() )
	{
//		qWarning ( "KumModules::remove No such module!" );
		return;
	};
	Modules.removeAt ( module_id );
}

void KumModules::Debug()
{
	for(int i=0;i<Modules.count();i++)
	{
//		if(Modules[i]->isEnabled())
//			qWarning()<<"Module "<< i <<" "<< Modules[i]->name<<" enabled";
//		else
//			qWarning()<<"Module "<< i <<" "<< Modules[i]->name<<" disabled";
	}
}

void KumModules::setUsed(PeremPrt ptr, bool flag)
{
	K_ASSERT(Modules[ptr.mod_id]->isEnabled());
	Modules[ptr.mod_id]->Values()->setUsed(ptr.perem_id,flag);
}

bool KumModules::isUsed(PeremPrt ptr)
{
	K_ASSERT(Modules[ptr.mod_id]->isEnabled());
	return Modules[ptr.mod_id]->Values()->isUsed(ptr.perem_id);
}

void KumModules::tempDisable()
{
	tempDisabled.clear();
	for(int i=2;i<=lastModuleId();i++)
	{
		if(Modules[i]->isEnabled())tempDisabled.append(i);
		disableModule(i);
	}
}

void KumModules::tempEnable()
{
	for(int i=0;i<tempDisabled.count();i++)enableModule(tempDisabled[i]);
}

