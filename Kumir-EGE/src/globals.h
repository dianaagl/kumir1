//
// C++ Interface: globals
//
// Description: 
//
//
// Author: Виктор Яковлев <V.Yacovlev@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

enum  PeremType {integer,kumString,charect,real,mass_integer,mass_charect,mass_string,mass_real,mass_bool,boolean,none,bool_or_num,bool_or_lit};

class ProgaText
{
	public:
		QString text;
		int stroka; // номер реальной строки
		int x_offset; // кол-во ксимволов до её начала
	// не используются
		int y_offset;
		int base_type;
		int error;
		int err_start;
		int err_end;
		int err_start_incl;
		int err_end_incl;

};
