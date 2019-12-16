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
#include "kum_tables.h"
#include "kum_instrument.h"
#include "int_proga.h"
#include "symboltable.h"


f_table::f_table(int moduleId, int functionId)
{
	name = "";
	doc = "";
	str = -1;
	proga_str = -1;
	ends = -1;
	type = none;
	disabled = false;
	badalg = false;
	teacher = false;
	locals = new Kum::SymbolTable(moduleId, functionId, name);
	Kum::SymbolTable::registerTable(locals);
	initializationIP = -1;
	initializationDone = false;
}

QString function_table::nameById(int id) const
{
	Q_ASSERT ( id>=0 );
	Q_ASSERT( id < l_table.size() );
	return l_table[id].name;
}

bool function_table::isRes(int id, int arg_id) const
{
	Q_ASSERT ( id>=0 );
	Q_ASSERT( id < l_table.size() );
	Q_ASSERT ( arg_id >= 0 );
	Q_ASSERT ( arg_id < l_table[id].l_arguments.size() );
	return l_table[id].l_arguments[arg_id].res;
}

void function_table::setArgType(int id, int arg_id, PeremType argType)
{
	Q_ASSERT( id >=0 );
	Q_ASSERT( id < l_table.size() );
	Q_ASSERT ( arg_id >= 0 );
	Q_ASSERT ( arg_id < l_table[id].l_arguments.size() );
	l_table[id].l_arguments[arg_id].type = argType;

}

int function_table::append(const QString &name, PeremType returnType, int declPos)
{
	f_table f(i_moduleId, l_table.size());
	f.name = name;
	f.proga_str = f.str = declPos;
	f.type = returnType;
	f.ends = 32000;
	l_table << f;
	return l_table.size()-1;
}

int function_table::appendDummy()
{
	f_table f(i_moduleId, l_table.size());
	f.name = "";
	f.type = none;
	f.ends = -1;
	f.str = -1;
	f.proga_str = -1;
	f.doc = -1;
	l_table << f;
	hiddenAlgorhitmId = l_table.size()-1;
	return l_table.size()-1;
}

Kum::SymbolTable* function_table::locals(int id)
{
	if (id<0)
		return 0;
	return l_table[id].locals;
}
