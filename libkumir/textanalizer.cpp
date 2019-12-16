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

#include <QtCore>
#include <cmath>

#include "textanalizer.h"
#include "ksymbols.h"
#include "integeroverflowchecker.h"
#include "doubleoverflowchecker.h"
#include "errorcodes.h"
#include "symboltable.h"
#include "functiontable.h"
#include "textnormalizer.h"
#include "modulespool.h"

//int find_viraj_Func(QStringList virajList,QList<Kumir::ValueType> *virajTypes,FunctionTable* table,KumModules * modules,QString func,int *err)
//{ //TODO ne FUNCT
//    QString func_name=func.left(func.indexOf('&'));
//    if(func.indexOf('&')<0)
//        func_name=func;
//    int func_id=-1;
//    int module_id=-1;
//    modules->FuncByName( func_name ,&module_id,&func_id);
//    if(func_id<0)
//    {
//        *err=UNKN_ALG;
//        return -1;
//    };

//    QString args_id=func.mid(func.indexOf('&')+1);
//    QString args=virajList[args_id.toInt()];
//    QStringList arg_List = args.split(",",QString::SkipEmptyParts);
//    int otl=arg_List.count();
//    // int dbg=table->func_table[func_id].nperem;
//    if (func_id==-1)return 2;
//    if(otl!=table->argCountById(func_id))
//    {
//        *err=PARAM_COUNT_ERR;
//        return -1;
//    };

//    for(int i=0;i<otl;i++)
//    {
//        int id;
//        QString tmp=arg_List[i].mid(1);
//        id=tmp.toInt();
//        Kumir::ValueType perType;
//        QList<Kumir::ValueType> types=*virajTypes;
//        if(otl==1)
//            perType=types[args_id.toInt()];
//        else
//            perType=types[id];
//        //        Kumir::ValueType log=table->func_table[func_id].perems[i];
//        if(table->argTypeById(func_id, i)!=perType)
//        {
//            *err=PARAM_TYPE_ERR;
//            return -1;
//        };

//    };

//    return func_id;
//};

/**
 * Проверка константы
 * @param name текст предполагаемой константы
 * @return Тип константы (Kumir::Undefined если не удалось определить)
 */
Kumir::ValueType TextAnalizer::testConst (  QString name, int *err )
{
    name = name.trimmed();
    QString debug = name;
    bool ok;
    int base=10;
    *err = 0;
    int iResult = 0;
    if (isBooleanConstant(name))
        return Kumir::Boolean;
    bool isLit = name.startsWith("\"") && name.endsWith("\"");
    if ( !isLit )
    {
        if (!name.startsWith("$")) {
            name.replace(QString::fromUtf8("Е"),"E");
            name.replace(QString::fromUtf8("е"),"E");
        }
        name.replace("e","E");


        if ( name.count("E") && !name.startsWith("$"))
        {
            if ( (name.count("E+")==0) && (name.count("E-")==0) )
            {
                name.replace("E","E+");
            }
        }

        if ( name.startsWith ( "$" ) )
        {
            QRegExp rxHex = QRegExp("\\$[0-9A-F]+");
            if ( !rxHex.exactMatch(name.simplified()) ) {
                *err = GNVR_BAD_HEX_CONST;
                return Kumir::Undefined;
            }
            base=16;
            name=name.mid ( 1,name.length()-1 );
            if ( name.count(".") || name.count("e+") || name.count("E+") )
            {
                *err = TN_BAD_NAME_1;
                return Kumir::Undefined;
            }
            if ( !IntegerOverflowChecker::checkFromString("$"+name) ) {
                *err = GNVR_BIG_INT_CONST;
                return Kumir::Undefined;
            }
            iResult = name.toInt ( &ok,base );

            // 			if ( ok &&
            // 							( ( iResult < -1073741824 ) || ( iResult > 1073741824 ) )
            // 				 )
            // 			{
            // 				*err = GNVR_BIG_INT_CONST;
            // 			}
            //
            if ( ok )
                return Kumir::Integer;
            else
            {
                *err = GNVR_BIG_INT_CONST;
                return Kumir::Undefined;
            }
        };
        if ( name.startsWith ( "0x" ) )
        {
            base=16;
            if ( name.count(".") || name.count("e+") || name.count("E+") )
            {
                *err = TN_BAD_NAME_1;
                return Kumir::Undefined;
            }
            if ( !IntegerOverflowChecker::checkFromString(name.simplified()) ) {
                *err = GNVR_BIG_INT_CONST;
                return Kumir::Undefined;
            }
        }
        QString tmp = name.simplified();
        ok = true;
        for (int i=0; i<tmp.length(); i++ ) {
            if ( !tmp[i].isDigit() ) {
                if (tmp[i]=='-' && i==0 )
                    continue;
                else {
                    ok = false;
                    break;
                }
            }
        }

        if ( ok ) {
            if ( !IntegerOverflowChecker::checkFromString(tmp) ) {
                *err = GNVR_BIG_INT_CONST;
                return Kumir::Undefined;
            }
            else {
                return Kumir::Integer;
            }
        }
        // 		iResult = name.toInt ( &ok,base );
        //
        // 		if ( ok &&
        // 					( ( iResult < -1073741824 ) || ( iResult > 1073741824 ) )
        // 			)
        // 		{
        // 			*err = GNVR_BIG_INT_CONST;
        // 		}


        bool possible_double = true;
        bool dot_found = false;
        bool e_found = false;
        for ( int i=0; i<name.length(); i++ ) {
            if ( name[i].isDigit() )
                continue;
            else if ( name[i]=='-' || name[i]=='+' ) {
                if (i>0) {
                    if (e_found) {
                        possible_double = false;
                        break;
                    }
                    if ( name[i-1]=='E' ) {
                        e_found = true;
                        continue;
                    }
                }
                else {
                    possible_double = false;
                    break;
                }
            }
            else if ( name[i]=='E' ) {
                if (e_found) {
                    possible_double = false;
                    break;
                }
                else {
                    continue;
                }
            }
            else if ( name[i]=='.' ) {
                if ( dot_found ) {
                    possible_double = false;
                    break;
                }
                else
                    dot_found = true;
            }
            else {
                possible_double = false;
                break;
            }
        }
        if ( ok || possible_double )
        {
            if ( !DoubleOverflowChecker::checkFromString(tmp) )
            {
                *err = GNVR_BIG_FLOAT_CONST;
                return Kumir::Undefined;
            }
            else {
                return Kumir::Real;
            }
            // 			if (
            // 					( name.toLower() != "inf" )
            // 					&&
            // 					( name.toLower() != "nan" )
            // 			)
            // 			{
            // 				if ( ok &&
            // 								( ( fResult < -1e+30 ) || ( fResult > 1e+30 ) )
            // 					)
            // 				{
            // 					*err = GNVR_BIG_FLOAT_CONST;
            // 				}
            // 				return Kumir::Real;
            // 			}
        }
    }

    // NEW V.Y. #4.3 02.08.2006
    if ( ( name.startsWith ( "\"" ) && ( name.endsWith ( "\"" ) ) ) )
    {
        if ( name.length() ==3 )
            return Kumir::Charect;
        else
            return Kumir::String;
    }
    // end NEW #4.3
    if ( ( name.startsWith ( "\"" ) && ( !name.endsWith ( "\"" ) ) ) )
        return Kumir::Undefined;

    if ( name==KS_TRUE )
        return Kumir::Boolean;
    if ( name==KS_FALSE )
        return Kumir::Boolean;

    return Kumir::Undefined;
};

Kumir::ValueType TextAnalizer::resType(
        int *err,
        Kumir::ValueType type1,
        Kumir::ValueType type2,
        QChar oper
        )
{
    *err=0;
    if ( oper == KS_ASSIGN )
    {
        *err = RESTP_ASSIGN_INSTEAD_EQ;
        return Kumir::Undefined;
    }
    if(type1==Kumir::Undefined)
    {
        if(type2==Kumir::Integer)
        {
            if ( oper == QChar(UNARY_MINUS) )
                return Kumir::Integer;
            if ( oper == QChar(UNARY_PLUS) )
                return Kumir::Integer;
            *err=RESTP_UNARY_INT;
            return Kumir::Undefined;
        }

        if(type2==Kumir::Real)
        {
            if ( oper == QChar(UNARY_MINUS) )
                return Kumir::Real;
            if ( oper == QChar(UNARY_PLUS) )
                return Kumir::Real;
            *err=RESTP_UNARY_REAL;
            return Kumir::Undefined;
        }

        if(type2==Kumir::Boolean)
        {
            if(oper==QChar(KS_NOT))
                return Kumir::Boolean;
            *err=RESTP_UNARY_BOOL;
            return Kumir::Undefined;
        }



        if ( type2==Kumir::BoolOrDecimal )
        {
            if(oper==QChar(KS_NOT))
                return Kumir::Boolean;
            *err=RESTP_UNARY_BOOL;
            return Kumir::Undefined;
        }

        if ( type2==Kumir::BoolOrLiteral )
        {
            if(oper==QChar(KS_NOT))
                return Kumir::Boolean;
            *err=RESTP_UNARY_BOOL;
            return Kumir::Undefined;
        }

        *err=RESTP_UNARY;
        return Kumir::Undefined;
    }

    if(type1==Kumir::Integer)
    {
        if(type2==Kumir::Integer)
        {
            if(oper=='+')
                return Kumir::Integer;
            if(oper=='-')
                return Kumir::Integer;
            if(oper=='*')
                return Kumir::Integer;
            if(oper=='/')
                return Kumir::Real;
            if(oper==KS_POWER)
                return Kumir::Integer;

            if(oper=='>')

                return Kumir::BoolOrDecimal;
            if(oper=='<')

                return Kumir::BoolOrDecimal;
            if(oper=='=')
                return Kumir::BoolOrDecimal;


            if(oper==KS_NEQ)
                return Kumir::BoolOrDecimal;
            if(oper==KS_GEQ)

                return Kumir::BoolOrDecimal;
            if(oper==KS_LEQ)

                return Kumir::BoolOrDecimal;

            if(oper==KS_OR)
            {
                *err = RESTP_INT_INT_OR;
                return Kumir::Undefined;
            };
            if(oper==KS_AND)
            {
                *err = RESTP_INT_INT_AND;
                return Kumir::Undefined;
            };
            if(oper==KS_XOR)
            {
                *err = RESTP_INT_INT_XOR;
                return Kumir::Undefined;
            }
        }

        if(type2==Kumir::Real)
        {
            if ( oper=='+' )
                return Kumir::Real;
            if ( oper=='-' )
                return Kumir::Real;
            if ( oper=='*' )
                return Kumir::Real;
            if ( oper=='/' )
                return Kumir::Real;
            if ( oper==KS_POWER )
                return Kumir::Real;

            if(oper==KS_NEQ)

                return Kumir::BoolOrDecimal;
            if(oper==KS_GEQ)

                return Kumir::BoolOrDecimal;
            if(oper==KS_LEQ)

                return Kumir::BoolOrDecimal;

            if(oper=='>')

                return Kumir::BoolOrDecimal;
            if(oper=='<')

                return Kumir::BoolOrDecimal;
            if(oper=='=')
                return Kumir::BoolOrDecimal;

            if(oper==KS_OR)
            {
                *err = RESTP_INT_REAL_OR;
                return Kumir::Undefined;
            };
            if(oper==KS_AND)
            {
                *err = RESTP_INT_REAL_AND;
                return Kumir::Undefined;
            };
            if(oper==KS_XOR)
            {
                *err = RESTP_INT_REAL_XOR;
                return Kumir::Undefined;
            }

        }

        if ( type2 == Kumir::Boolean )
        {
            if ( oper == '+' )
                *err = RESTP_INT_BOOL_PLUS;
            if ( oper == '-' )
                *err = RESTP_INT_BOOL_MINUS;
            if ( oper == '*' )
                *err = RESTP_INT_BOOL_MUL;
            if ( oper == '/' )
                *err = RESTP_INT_BOOL_DIV;
            if ( oper == '<' )
                *err = RESTP_INT_BOOL_LESS;
            if ( oper == '>' )
                *err = RESTP_INT_BOOL_GREAT;
            if ( oper == '=' )
                *err = RESTP_INT_BOOL_EQ;
            if ( oper == QChar(KS_POWER) )
                *err = RESTP_INT_BOOL_POW;
            if ( oper == KS_NEQ )
                *err = RESTP_INT_BOOL_NEQ;
            if ( oper == KS_LEQ )
                *err = RESTP_INT_BOOL_LEQ;
            if ( oper == KS_GEQ )
                *err = RESTP_INT_BOOL_GEQ;
            if ( oper == QChar(KS_OR) )
                *err = RESTP_INT_BOOL_OR;
            if ( oper == QChar(KS_XOR) )
                *err = RESTP_INT_BOOL_XOR;
            if ( oper == QChar(KS_AND) )
                *err = RESTP_INT_BOOL_AND;
            return Kumir::Undefined;
        }

        if ( type2 == Kumir::Charect )
        {
            if ( oper == '+' )
                *err = RESTP_INT_CHAR_PLUS;
            if ( oper == '-' )
                *err = RESTP_INT_CHAR_MINUS;
            if ( oper == '*' )
                *err = RESTP_INT_CHAR_MUL;
            if ( oper == '/' )
                *err = RESTP_INT_CHAR_DIV;
            if ( oper == '<' )
                *err = RESTP_INT_CHAR_LESS;
            if ( oper == '>' )
                *err = RESTP_INT_CHAR_GREAT;
            if ( oper == '=' )
                *err = RESTP_INT_CHAR_EQ;
            if ( oper == QChar(KS_POWER) )
                *err = RESTP_INT_CHAR_POW;
            if ( oper == KS_NEQ )
                *err = RESTP_INT_CHAR_NEQ;
            if ( oper == KS_LEQ )
                *err = RESTP_INT_CHAR_LEQ;
            if ( oper == KS_GEQ )
                *err = RESTP_INT_CHAR_GEQ;
            if ( oper == QChar(KS_OR) )
                *err = RESTP_INT_CHAR_OR;
            if ( oper == QChar(KS_XOR) )
                *err = RESTP_INT_CHAR_XOR;
            if ( oper == QChar(KS_AND) )
                *err = RESTP_INT_CHAR_AND;
            return Kumir::Undefined;
        }

        if ( type2 == Kumir::String )
        {
            if ( oper == '+' )
                *err = RESTP_INT_STR_PLUS;
            if ( oper == '-' )
                *err = RESTP_INT_STR_MINUS;
            if ( oper == '*' )
                *err = RESTP_INT_STR_MUL;
            if ( oper == '/' )
                *err = RESTP_INT_STR_DIV;
            if ( oper == '<' )
                *err = RESTP_INT_STR_LESS;
            if ( oper == '>' )
                *err = RESTP_INT_STR_GREAT;
            if ( oper == '=' )
                *err = RESTP_INT_STR_EQ;
            if ( oper == QChar(KS_POWER) )
                *err = RESTP_INT_STR_POW;
            if ( oper == KS_NEQ )
                *err = RESTP_INT_STR_NEQ;
            if ( oper == KS_LEQ )
                *err = RESTP_INT_STR_LEQ;
            if ( oper == KS_GEQ )
                *err = RESTP_INT_STR_GEQ;
            if ( oper == QChar(KS_OR) )
                *err = RESTP_INT_STR_OR;
            if ( oper == QChar(KS_XOR) )
                *err = RESTP_INT_STR_XOR;
            if ( oper == QChar(KS_AND) )
                *err = RESTP_INT_STR_AND;
            return Kumir::Undefined;
        }

        if ( type2 == Kumir::BoolOrLiteral )
        {

            if(oper=='>')
                *err = RESTP_INT_BL_MORE;
            if(oper=='<')
                *err = RESTP_INT_BL_LESS;
            if(oper=='=')
                *err = RESTP_INT_BL_EQ;

            if(oper==KS_NEQ)
                *err = RESTP_INT_BL_NEQ;
            if(oper==KS_GEQ)
                *err = RESTP_INT_BL_GEQ;
            if(oper==KS_LEQ)
                *err = RESTP_INT_BL_LEQ;

            if ( oper == QChar(KS_OR) )
                *err = RESTP_INT_BL_OR;
            if ( oper == QChar(KS_XOR) )
                *err = RESTP_INT_BL_XOR;
            if ( oper == QChar(KS_AND) )
                *err = RESTP_INT_BL_AND;

            return Kumir::Undefined;
        }

        if ( type2 == Kumir::BoolOrDecimal )
        {
            if(oper=='>')
                return Kumir::BoolOrDecimal;
            if(oper=='<')
                return Kumir::BoolOrDecimal;
            if(oper=='=')
                return Kumir::BoolOrDecimal;

            if(oper==KS_NEQ)
                return Kumir::BoolOrDecimal;
            if(oper==KS_GEQ)
                return Kumir::BoolOrDecimal;
            if(oper==KS_LEQ)
                return Kumir::BoolOrDecimal;

            if ( oper == QChar(KS_OR) )
                *err = RESTP_INT_BN_OR;
            if ( oper == QChar(KS_XOR) )
                *err = RESTP_INT_BN_XOR;
            if ( oper == QChar(KS_AND) )
                *err = RESTP_INT_BN_AND;

            return Kumir::Undefined;
        }

        *err = RESTP_INT;
        return Kumir::Undefined;

    } // end if (type1==Kumir::Integer)


    if(type1==Kumir::Boolean)
    {
        if(type2==Kumir::Boolean)
        {
            if(oper==KS_OR)
            {
                return Kumir::Boolean;
            };
            if(oper==KS_AND)
            {
                return Kumir::Boolean;
            };
            if(oper==KS_XOR)
            {
                return Kumir::Boolean;
            }
            if ( oper == KS_NEQ )
                return Kumir::Boolean;
            if(oper=='=')
                return Kumir::Boolean;
            if ( oper == '+' )
                *err = RESTP_BOOL_BOOL_PLUS;
            if ( oper == '-' )
                *err = RESTP_BOOL_BOOL_MINUS;
            if ( oper == '*' )
                *err = RESTP_BOOL_BOOL_MUL;
            if ( oper == '/' )
                *err = RESTP_BOOL_BOOL_DIV;
            if ( oper == '<' )
                *err = RESTP_BOOL_BOOL_LESS;
            if ( oper == '>' )
                *err = RESTP_BOOL_BOOL_GREAT;
            if ( oper == '=' )
                *err = RESTP_BOOL_BOOL_EQ;
            if ( oper == QChar(KS_POWER) )
                *err = RESTP_BOOL_BOOL_POW;
            if ( oper == KS_LEQ )
                *err = RESTP_BOOL_BOOL_LEQ;
            if ( oper == KS_GEQ )
                *err = RESTP_BOOL_BOOL_GEQ;

            return Kumir::Undefined;

        }
        if ( type2==Kumir::Integer )
        {
            if ( oper == '+' )
                *err = RESTP_BOOL_INT_PLUS;
            if ( oper == '-' )
                *err = RESTP_BOOL_INT_MINUS;
            if ( oper == '*' )
                *err = RESTP_BOOL_INT_MUL;
            if ( oper == '/' )
                *err = RESTP_BOOL_INT_DIV;
            if ( oper == '<' )
                *err = RESTP_BOOL_INT_LESS;
            if ( oper == '>' )
                *err = RESTP_BOOL_INT_GREAT;
            if ( oper == '=' )
                *err = RESTP_BOOL_INT_EQ;
            if ( oper == QChar(KS_POWER) )
                *err = RESTP_BOOL_INT_POW;
            if ( oper == KS_NEQ )
                *err = RESTP_BOOL_INT_NEQ;
            if ( oper == KS_LEQ )
                *err = RESTP_BOOL_INT_LEQ;
            if ( oper == KS_GEQ )
                *err = RESTP_BOOL_INT_GEQ;
            if ( oper == QChar(KS_OR) )
                *err = RESTP_BOOL_INT_OR;
            if ( oper == QChar(KS_XOR) )
                *err = RESTP_BOOL_INT_XOR;
            if ( oper == QChar(KS_AND) )
                *err = RESTP_BOOL_INT_AND;
            return Kumir::Undefined;
        }
        if ( type2==Kumir::Real )
        {
            if ( oper == '+' )
                *err = RESTP_BOOL_REAL_PLUS;
            if ( oper == '-' )
                *err = RESTP_BOOL_REAL_MINUS;
            if ( oper == '*' )
                *err = RESTP_BOOL_REAL_MUL;
            if ( oper == '/' )
                *err = RESTP_BOOL_REAL_DIV;
            if ( oper == '<' )
                *err = RESTP_BOOL_REAL_LESS;
            if ( oper == '>' )
                *err = RESTP_BOOL_REAL_GREAT;
            if ( oper == '=' )
                *err = RESTP_BOOL_REAL_EQ;
            if ( oper == QChar(KS_POWER) )
                *err = RESTP_BOOL_REAL_POW;
            if ( oper == KS_NEQ )
                *err = RESTP_BOOL_REAL_NEQ;
            if ( oper == KS_LEQ )
                *err = RESTP_BOOL_REAL_LEQ;
            if ( oper == KS_GEQ )
                *err = RESTP_BOOL_REAL_GEQ;
            if ( oper == QChar(KS_OR) )
                *err = RESTP_BOOL_REAL_OR;
            if ( oper == QChar(KS_XOR) )
                *err = RESTP_BOOL_REAL_XOR;
            if ( oper == QChar(KS_AND) )
                *err = RESTP_BOOL_REAL_AND;
            return Kumir::Undefined;
        }
        if ( type2==Kumir::Charect )
        {
            if ( oper == '+' )
                *err = RESTP_BOOL_CHAR_PLUS;
            if ( oper == '-' )
                *err = RESTP_BOOL_CHAR_MINUS;
            if ( oper == '*' )
                *err = RESTP_BOOL_CHAR_MUL;
            if ( oper == '/' )
                *err = RESTP_BOOL_CHAR_DIV;
            if ( oper == '<' )
                *err = RESTP_BOOL_CHAR_LESS;
            if ( oper == '>' )
                *err = RESTP_BOOL_CHAR_GREAT;
            if ( oper == '=' )
                *err = RESTP_BOOL_CHAR_EQ;
            if ( oper == QChar(KS_POWER) )
                *err = RESTP_BOOL_CHAR_POW;
            if ( oper == KS_NEQ )
                *err = RESTP_BOOL_CHAR_NEQ;
            if ( oper == KS_LEQ )
                *err = RESTP_BOOL_CHAR_LEQ;
            if ( oper == KS_GEQ )
                *err = RESTP_BOOL_CHAR_GEQ;
            if ( oper == QChar(KS_OR) )
                *err = RESTP_BOOL_CHAR_OR;
            if ( oper == QChar(KS_XOR) )
                *err = RESTP_BOOL_CHAR_XOR;
            if ( oper == QChar(KS_AND) )
                *err = RESTP_BOOL_CHAR_AND;
            return Kumir::Undefined;
        }
        if ( type2==Kumir::String )
        {
            if ( oper == '+' )
                *err = RESTP_BOOL_STR_PLUS;
            if ( oper == '-' )
                *err = RESTP_BOOL_STR_MINUS;
            if ( oper == '*' )
                *err = RESTP_BOOL_STR_MUL;
            if ( oper == '/' )
                *err = RESTP_BOOL_STR_DIV;
            if ( oper == '<' )
                *err = RESTP_BOOL_STR_LESS;
            if ( oper == '>' )
                *err = RESTP_BOOL_STR_GREAT;
            if ( oper == '=' )
                *err = RESTP_BOOL_STR_EQ;
            if ( oper == QChar(KS_POWER) )
                *err = RESTP_BOOL_STR_POW;
            if ( oper == KS_NEQ )
                *err = RESTP_BOOL_STR_NEQ;
            if ( oper == KS_LEQ )
                *err = RESTP_BOOL_STR_LEQ;
            if ( oper == KS_GEQ )
                *err = RESTP_BOOL_STR_GEQ;
            if ( oper == QChar(KS_OR) )
                *err = RESTP_BOOL_STR_OR;
            if ( oper == QChar(KS_XOR) )
                *err = RESTP_BOOL_STR_XOR;
            if ( oper == QChar(KS_AND) )
                *err = RESTP_BOOL_STR_AND;
            return Kumir::Undefined;
        }
        if ( type2 == Kumir::BoolOrLiteral )
        {
            if ( oper == QChar(KS_OR) )
                return Kumir::Boolean;
            if ( oper == QChar(KS_XOR) )
                return Kumir::Boolean;
            if ( oper == QChar(KS_AND) )
                return Kumir::Boolean;
            if ( oper == QChar('=') )
                return Kumir::Boolean;
            if ( oper == KS_NEQ )
                return Kumir::Boolean;
            *err = RESTP_BOOL;
            return Kumir::Undefined;
        }

        if ( type2 == Kumir::BoolOrDecimal )
        {
            if ( oper == QChar(KS_OR) )
                return Kumir::Boolean;
            if ( oper == QChar(KS_XOR) )
                return Kumir::Boolean;
            if ( oper == QChar(KS_AND) )
                return Kumir::Boolean;
            if ( oper == QChar('=') )
                return Kumir::Boolean;
            if ( oper == KS_NEQ )
                return Kumir::Boolean;
            *err = RESTP_BOOL;
            return Kumir::Undefined;
        }
        *err = RESTP_BOOL;
        return Kumir::Undefined;
    } // end if (type1==Kumir::Boolean)

    if (type1==Kumir::Real)
    {
        if(type2==Kumir::Integer)
        {
            if(oper=='+')
                return Kumir::Real;
            if(oper=='-')
                return Kumir::Real;
            if(oper=='*')
                return Kumir::Real;
            if(oper=='/')
                return Kumir::Real;
            if ( oper==KS_POWER )
                return Kumir::Real;

            if(oper=='>')
                return Kumir::BoolOrDecimal;
            if(oper=='<')
                return Kumir::BoolOrDecimal;
            if(oper=='=')
                return Kumir::BoolOrDecimal;

            if(oper==KS_NEQ)
                return Kumir::Boolean;
            if(oper==KS_GEQ)
                return Kumir::Boolean;
            if(oper==KS_LEQ)
                return Kumir::Boolean;

            if ( oper == QChar(KS_OR) )
                *err = RESTP_REAL_INT_OR;
            if ( oper == QChar(KS_XOR) )
                *err = RESTP_REAL_INT_XOR;
            if ( oper == QChar(KS_AND) )
                *err = RESTP_REAL_INT_AND;
            return Kumir::Undefined;

        }

        if (type2==Kumir::Real)
        {
            if(oper=='+')
                return Kumir::Real;
            if(oper=='-')
                return Kumir::Real;
            if(oper=='*')
                return Kumir::Real;
            if(oper=='/')
                return Kumir::Real;
            if ( oper==KS_POWER )
                return Kumir::Real;

            if(oper=='>')
                return Kumir::BoolOrDecimal;
            if(oper=='<')
                return Kumir::BoolOrDecimal;
            if(oper=='=')
                return Kumir::BoolOrDecimal;

            if(oper==KS_NEQ)
                return Kumir::BoolOrDecimal;
            if(oper==KS_GEQ)
                return Kumir::BoolOrDecimal;
            if(oper==KS_LEQ)
                return Kumir::BoolOrDecimal;

            if ( oper == QChar(KS_OR) )
                *err = RESTP_REAL_REAL_OR;
            if ( oper == QChar(KS_XOR) )
                *err = RESTP_REAL_REAL_XOR;
            if ( oper == QChar(KS_AND) )
                *err = RESTP_REAL_REAL_AND;
            return Kumir::Undefined;

        }

        if ( type2==Kumir::String )
        {
            if ( oper == '+' )
                *err = RESTP_REAL_STR_PLUS;
            if ( oper == '-' )
                *err = RESTP_REAL_STR_MINUS;
            if ( oper == '*' )
                *err = RESTP_REAL_STR_MUL;
            if ( oper == '/' )
                *err = RESTP_REAL_STR_DIV;
            if ( oper == '<' )
                *err = RESTP_REAL_STR_LESS;
            if ( oper == '>' )
                *err = RESTP_REAL_STR_GREAT;
            if ( oper == '=' )
                *err = RESTP_REAL_STR_EQ;
            if ( oper == QChar(KS_POWER) )
                *err = RESTP_REAL_STR_POW;
            if ( oper == KS_NEQ )
                *err = RESTP_REAL_STR_NEQ;
            if ( oper == KS_LEQ )
                *err = RESTP_REAL_STR_LEQ;
            if ( oper == KS_GEQ )
                *err = RESTP_REAL_STR_GEQ;
            if ( oper == QChar(KS_OR) )
                *err = RESTP_REAL_STR_OR;
            if ( oper == QChar(KS_XOR) )
                *err = RESTP_REAL_STR_XOR;
            if ( oper == QChar(KS_AND) )
                *err = RESTP_REAL_STR_AND;
            return Kumir::Undefined;
        }

        if ( type2==Kumir::Charect )
        {
            if ( oper == '+' )
                *err = RESTP_REAL_CHAR_PLUS;
            if ( oper == '-' )
                *err = RESTP_REAL_CHAR_MINUS;
            if ( oper == '*' )
                *err = RESTP_REAL_CHAR_MUL;
            if ( oper == '/' )
                *err = RESTP_REAL_CHAR_DIV;
            if ( oper == '<' )
                *err = RESTP_REAL_CHAR_LESS;
            if ( oper == '>' )
                *err = RESTP_REAL_CHAR_GREAT;
            if ( oper == '=' )
                *err = RESTP_REAL_CHAR_EQ;
            if ( oper == QChar(KS_POWER) )
                *err = RESTP_REAL_CHAR_POW;
            if ( oper == KS_NEQ )
                *err = RESTP_REAL_CHAR_NEQ;
            if ( oper == KS_LEQ )
                *err = RESTP_REAL_CHAR_LEQ;
            if ( oper == KS_GEQ )
                *err = RESTP_REAL_CHAR_GEQ;
            if ( oper == QChar(KS_OR) )
                *err = RESTP_REAL_CHAR_OR;
            if ( oper == QChar(KS_XOR) )
                *err = RESTP_REAL_CHAR_XOR;
            if ( oper == QChar(KS_AND) )
                *err = RESTP_REAL_CHAR_AND;
            return Kumir::Undefined;
        }

        if ( type2==Kumir::Boolean )
        {
            if ( oper == '+' )
                *err = RESTP_REAL_BOOL_PLUS;
            if ( oper == '-' )
                *err = RESTP_REAL_BOOL_MINUS;
            if ( oper == '*' )
                *err = RESTP_REAL_BOOL_MUL;
            if ( oper == '/' )
                *err = RESTP_REAL_BOOL_DIV;
            if ( oper == '<' )
                *err = RESTP_REAL_BOOL_LESS;
            if ( oper == '>' )
                *err = RESTP_REAL_BOOL_GREAT;
            if ( oper == '=' )
                *err = RESTP_REAL_BOOL_EQ;
            if ( oper == QChar(KS_POWER) )
                *err = RESTP_REAL_BOOL_POW;
            if ( oper == KS_NEQ )
                *err = RESTP_REAL_BOOL_NEQ;
            if ( oper == KS_LEQ )
                *err = RESTP_REAL_BOOL_LEQ;
            if ( oper == KS_GEQ )
                *err = RESTP_REAL_BOOL_GEQ;
            if ( oper == QChar(KS_OR) )
                *err = RESTP_REAL_BOOL_OR;
            if ( oper == QChar(KS_XOR) )
                *err = RESTP_REAL_BOOL_XOR;
            if ( oper == QChar(KS_AND) )
                *err = RESTP_REAL_BOOL_AND;
            return Kumir::Undefined;
        }

        if ( type2 == Kumir::BoolOrLiteral )
        {

            if(oper=='>')
                *err = RESTP_REAL_BL_MORE;
            if(oper=='<')
                *err = RESTP_REAL_BL_LESS;
            if(oper=='=')
                *err = RESTP_REAL_BL_EQ;

            if(oper==KS_NEQ)
                *err = RESTP_REAL_BL_NEQ;
            if(oper==KS_GEQ)
                *err = RESTP_REAL_BL_GEQ;
            if(oper==KS_LEQ)
                *err = RESTP_REAL_BL_LEQ;

            if ( oper == QChar(KS_OR) )
                *err = RESTP_REAL_BL_OR;
            if ( oper == QChar(KS_XOR) )
                *err = RESTP_REAL_BL_XOR;
            if ( oper == QChar(KS_AND) )
                *err = RESTP_REAL_BL_AND;

            return Kumir::Undefined;
        }

        if ( type2 == Kumir::BoolOrDecimal )
        {
            if(oper=='>')
                return Kumir::BoolOrDecimal;
            if(oper=='<')
                return Kumir::BoolOrDecimal;
            if(oper=='=')
                return Kumir::BoolOrDecimal;

            if(oper==KS_NEQ)
                return Kumir::BoolOrDecimal;
            if(oper==KS_GEQ)
                return Kumir::BoolOrDecimal;
            if(oper==KS_LEQ)
                return Kumir::BoolOrDecimal;

            if ( oper == QChar(KS_OR) )
                *err = RESTP_REAL_BN_OR;
            if ( oper == QChar(KS_XOR) )
                *err = RESTP_REAL_BN_XOR;
            if ( oper == QChar(KS_AND) )
                *err = RESTP_REAL_BN_AND;

            return Kumir::Undefined;
        }

        *err = RESTP_REAL;
        return Kumir::Undefined;

    } // end if (type1==Kumir::Real)

    if(type1==Kumir::String)
    {
        if ( type2==Kumir::String )
        {
            if ( oper=='+' )
                return Kumir::String;
            if(oper=='>')
                return Kumir::BoolOrLiteral;
            if(oper=='<')
                return Kumir::BoolOrLiteral;
            if(oper=='=')
                return Kumir::BoolOrLiteral;

            if(oper==KS_NEQ)
                return Kumir::BoolOrLiteral;
            if(oper==KS_GEQ)
                return Kumir::BoolOrLiteral;
            if(oper==KS_LEQ)
                return Kumir::BoolOrLiteral;

            if ( oper == '-' )
                *err = RESTP_STR_STR_MINUS;
            if ( oper == '*' )
                *err = RESTP_STR_STR_MUL;
            if ( oper == '/' )
                *err = RESTP_STR_STR_DIV;
            if ( oper == QChar(KS_OR) )
                *err = RESTP_STR_STR_OR;
            if ( oper == QChar(KS_POWER) )
                *err = RESTP_STR_STR_POW;
            if ( oper == QChar(KS_XOR) )
                *err = RESTP_STR_STR_XOR;
            if ( oper == QChar(KS_AND) )
                *err = RESTP_STR_STR_AND;
            return Kumir::Undefined;
        }
        if ( type2==Kumir::Charect )
        {
            if ( oper=='+' )
                return Kumir::String;
            if(oper=='>')
                return Kumir::Boolean;
            if(oper=='<')
                return Kumir::Boolean;
            if(oper=='=')
                return Kumir::Boolean;

            if(oper==KS_NEQ)
                return Kumir::Boolean;
            if(oper==KS_GEQ)
                return Kumir::Boolean;
            if(oper==KS_LEQ)
                return Kumir::Boolean;

            if ( oper == '-' )
                *err = RESTP_STR_CHAR_MINUS;
            if ( oper == '*' )
                *err = RESTP_STR_CHAR_MUL;
            if ( oper == QChar(KS_POWER) )
                *err = RESTP_STR_CHAR_POW;
            if ( oper == '/' )
                *err = RESTP_STR_CHAR_DIV;
            if ( oper == QChar(KS_OR) )
                *err = RESTP_STR_CHAR_OR;
            if ( oper == QChar(KS_XOR) )
                *err = RESTP_STR_CHAR_XOR;
            if ( oper == QChar(KS_AND) )
                *err = RESTP_STR_CHAR_AND;
            return Kumir::Undefined;
        }
        if ( type2==Kumir::Boolean )
        {
            if ( oper == '+' )
                *err = RESTP_STR_BOOL_PLUS;
            if ( oper == '-' )
                *err = RESTP_STR_BOOL_MINUS;
            if ( oper == '*' )
                *err = RESTP_STR_BOOL_MUL;
            if ( oper == '/' )
                *err = RESTP_STR_BOOL_DIV;
            if ( oper == '<' )
                *err = RESTP_STR_BOOL_LESS;
            if ( oper == '>' )
                *err = RESTP_STR_BOOL_GREAT;
            if ( oper == '=' )
                *err = RESTP_STR_BOOL_EQ;
            if ( oper == QChar(KS_POWER) )
                *err = RESTP_STR_BOOL_POW;
            if ( oper == KS_NEQ )
                *err = RESTP_STR_BOOL_NEQ;
            if ( oper == KS_LEQ )
                *err = RESTP_STR_BOOL_LEQ;
            if ( oper == KS_GEQ )
                *err = RESTP_STR_BOOL_GEQ;
            if ( oper == QChar(KS_OR) )
                *err = RESTP_STR_BOOL_OR;
            if ( oper == QChar(KS_XOR) )
                *err = RESTP_STR_BOOL_XOR;
            if ( oper == QChar(KS_AND) )
                *err = RESTP_STR_BOOL_AND;
            return Kumir::Undefined;
        }
        if ( type2==Kumir::Real )
        {
            if ( oper == '+' )
                *err = RESTP_STR_REAL_PLUS;
            if ( oper == '-' )
                *err = RESTP_STR_REAL_MINUS;
            if ( oper == '*' )
                *err = RESTP_STR_REAL_MUL;
            if ( oper == '/' )
                *err = RESTP_STR_REAL_DIV;
            if ( oper == '<' )
                *err = RESTP_STR_REAL_LESS;
            if ( oper == '>' )
                *err = RESTP_STR_REAL_GREAT;
            if ( oper == '=' )
                *err = RESTP_STR_REAL_EQ;
            if ( oper == QChar(KS_POWER) )
                *err = RESTP_STR_REAL_POW;
            if ( oper == KS_NEQ )
                *err = RESTP_STR_REAL_NEQ;
            if ( oper == KS_LEQ )
                *err = RESTP_STR_REAL_LEQ;
            if ( oper == KS_GEQ )
                *err = RESTP_STR_REAL_GEQ;
            if ( oper == QChar(KS_OR) )
                *err = RESTP_STR_REAL_OR;
            if ( oper == QChar(KS_XOR) )
                *err = RESTP_STR_REAL_XOR;
            if ( oper == QChar(KS_AND) )
                *err = RESTP_STR_REAL_AND;
            return Kumir::Undefined;
        }
        if ( type2==Kumir::Integer )
        {
            if ( oper == '+' )
                *err = RESTP_STR_INT_PLUS;
            if ( oper == '-' )
                *err = RESTP_STR_INT_MINUS;
            if ( oper == '*' )
                *err = RESTP_STR_INT_MUL;
            if ( oper == '/' )
                *err = RESTP_STR_INT_DIV;
            if ( oper == '<' )
                *err = RESTP_STR_INT_LESS;
            if ( oper == '>' )
                *err = RESTP_STR_INT_GREAT;
            if ( oper == '=' )
                *err = RESTP_STR_INT_EQ;
            if ( oper == QChar(KS_POWER) )
                *err = RESTP_STR_INT_POW;
            if ( oper == KS_NEQ )
                *err = RESTP_STR_INT_NEQ;
            if ( oper == KS_LEQ )
                *err = RESTP_STR_INT_LEQ;
            if ( oper == KS_GEQ )
                *err = RESTP_STR_INT_GEQ;
            if ( oper == QChar(KS_OR) )
                *err = RESTP_STR_INT_OR;
            if ( oper == QChar(KS_XOR) )
                *err = RESTP_STR_INT_XOR;
            if ( oper == QChar(KS_AND) )
                *err = RESTP_STR_INT_AND;
            return Kumir::Undefined;
        }

        if ( type2 == Kumir::BoolOrDecimal )
        {

            if(oper=='>')
                *err = RESTP_STR_BL_MORE;
            if(oper=='<')
                *err = RESTP_STR_BL_LESS;
            if(oper=='=')
                *err = RESTP_STR_BL_EQ;

            if(oper==KS_NEQ)
                *err = RESTP_STR_BL_NEQ;
            if(oper==KS_GEQ)
                *err = RESTP_STR_BL_GEQ;
            if(oper==KS_LEQ)
                *err = RESTP_STR_BL_LEQ;

            if ( oper == QChar(KS_OR) )
                *err = RESTP_STR_BL_OR;
            if ( oper == QChar(KS_XOR) )
                *err = RESTP_STR_BL_XOR;
            if ( oper == QChar(KS_AND) )
                *err = RESTP_STR_BL_AND;

            return Kumir::Undefined;
        }

        if ( type2 == Kumir::BoolOrLiteral )
        {
            if(oper=='>')
                return Kumir::BoolOrLiteral;
            if(oper=='<')
                return Kumir::BoolOrLiteral;
            if(oper=='=')
                return Kumir::BoolOrLiteral;

            if(oper==KS_NEQ)
                return Kumir::BoolOrLiteral;
            if(oper==KS_GEQ)
                return Kumir::BoolOrLiteral;
            if(oper==KS_LEQ)
                return Kumir::BoolOrLiteral;

            if ( oper == QChar(KS_OR) )
                *err = RESTP_STR_BN_OR;
            if ( oper == QChar(KS_XOR) )
                *err = RESTP_STR_BN_XOR;
            if ( oper == QChar(KS_AND) )
                *err = RESTP_STR_BN_AND;

            return Kumir::Undefined;
        }

        *err = RESTP_STRING;
        return Kumir::Undefined;
    } //string


    if(type1==Kumir::Charect)
    {
        if ( type2==Kumir::String )
        {
            if ( oper=='+' )
                return Kumir::String;
            if(oper=='>')
                return Kumir::BoolOrLiteral;
            if(oper=='<')
                return Kumir::BoolOrLiteral;
            if(oper=='=')
                return Kumir::BoolOrLiteral;

            if(oper==KS_NEQ)
                return Kumir::BoolOrLiteral;
            if(oper==KS_GEQ)
                return Kumir::BoolOrLiteral;
            if(oper==KS_LEQ)
                return Kumir::BoolOrLiteral;
            if ( oper == '-' )
                *err = RESTP_CHAR_STR_MINUS;
            if ( oper == '*' )
                *err = RESTP_CHAR_STR_MUL;
            if ( oper == '/' )
                *err = RESTP_CHAR_STR_DIV;
            if ( oper == QChar(KS_OR) )
                *err = RESTP_CHAR_STR_OR;
            if ( oper == QChar(KS_POWER) )
                *err = RESTP_CHAR_STR_POW;
            if ( oper == QChar(KS_XOR) )
                *err = RESTP_CHAR_STR_XOR;
            if ( oper == QChar(KS_AND) )
                *err = RESTP_CHAR_STR_AND;
            return Kumir::Undefined;
        }
        if ( type2==Kumir::Charect )
        {
            if ( oper=='+' )
                return Kumir::String;
            if(oper=='>')
                return Kumir::BoolOrLiteral;
            if(oper=='<')
                return Kumir::BoolOrLiteral;
            if(oper=='=')
                return Kumir::BoolOrLiteral;

            if(oper==KS_NEQ)
                return Kumir::BoolOrLiteral;
            if(oper==KS_GEQ)
                return Kumir::BoolOrLiteral;
            if(oper==KS_LEQ)
                return Kumir::BoolOrLiteral;
            if ( oper == '-' )
                *err = RESTP_CHAR_CHAR_MINUS;
            if ( oper == '*' )
                *err = RESTP_CHAR_CHAR_MUL;
            if ( oper == '/' )
                *err = RESTP_CHAR_CHAR_DIV;
            if ( oper == QChar(KS_OR) )
                *err = RESTP_CHAR_CHAR_OR;
            if ( oper == QChar(KS_POWER) )
                *err = RESTP_CHAR_CHAR_POW;
            if ( oper == QChar(KS_XOR) )
                *err = RESTP_CHAR_CHAR_XOR;
            if ( oper == QChar(KS_AND) )
                *err = RESTP_CHAR_CHAR_AND;
            return Kumir::Undefined;
        }
        if ( type2==Kumir::Boolean )
        {
            if ( oper == '+' )
                *err = RESTP_CHAR_BOOL_PLUS;
            if ( oper == '-' )
                *err = RESTP_CHAR_BOOL_MINUS;
            if ( oper == '*' )
                *err = RESTP_CHAR_BOOL_MUL;
            if ( oper == '/' )
                *err = RESTP_CHAR_BOOL_DIV;
            if ( oper == '<' )
                *err = RESTP_CHAR_BOOL_LESS;
            if ( oper == '>' )
                *err = RESTP_CHAR_BOOL_GREAT;
            if ( oper == '=' )
                *err = RESTP_CHAR_BOOL_EQ;
            if ( oper == QChar(KS_POWER) )
                *err = RESTP_CHAR_BOOL_POW;
            if ( oper == KS_NEQ )
                *err = RESTP_CHAR_BOOL_NEQ;
            if ( oper == KS_LEQ )
                *err = RESTP_CHAR_BOOL_LEQ;
            if ( oper == KS_GEQ )
                *err = RESTP_CHAR_BOOL_GEQ;
            if ( oper == QChar(KS_OR) )
                *err = RESTP_CHAR_BOOL_OR;
            if ( oper == QChar(KS_XOR) )
                *err = RESTP_CHAR_BOOL_XOR;
            if ( oper == QChar(KS_AND) )
                *err = RESTP_CHAR_BOOL_AND;
            return Kumir::Undefined;
        }
        if ( type2==Kumir::Real )
        {
            if ( oper == '+' )
                *err = RESTP_CHAR_REAL_PLUS;
            if ( oper == '-' )
                *err = RESTP_CHAR_REAL_MINUS;
            if ( oper == '*' )
                *err = RESTP_CHAR_REAL_MUL;
            if ( oper == '/' )
                *err = RESTP_CHAR_REAL_DIV;
            if ( oper == '<' )
                *err = RESTP_CHAR_REAL_LESS;
            if ( oper == '>' )
                *err = RESTP_CHAR_REAL_GREAT;
            if ( oper == '=' )
                *err = RESTP_CHAR_REAL_EQ;
            if ( oper == QChar(KS_POWER) )
                *err = RESTP_CHAR_REAL_POW;
            if ( oper == KS_NEQ )
                *err = RESTP_CHAR_REAL_NEQ;
            if ( oper == KS_LEQ )
                *err = RESTP_CHAR_REAL_LEQ;
            if ( oper == KS_GEQ )
                *err = RESTP_CHAR_REAL_GEQ;
            if ( oper == QChar(KS_OR) )
                *err = RESTP_CHAR_REAL_OR;
            if ( oper == QChar(KS_XOR) )
                *err = RESTP_CHAR_REAL_XOR;
            if ( oper == QChar(KS_AND) )
                *err = RESTP_CHAR_REAL_AND;
            return Kumir::Undefined;
        }
        if ( type2==Kumir::Integer )
        {
            if ( oper == '+' )
                *err = RESTP_CHAR_INT_PLUS;
            if ( oper == '-' )
                *err = RESTP_CHAR_INT_MINUS;
            if ( oper == '*' )
                *err = RESTP_CHAR_INT_MUL;
            if ( oper == '/' )
                *err = RESTP_CHAR_INT_DIV;
            if ( oper == '<' )
                *err = RESTP_CHAR_INT_LESS;
            if ( oper == '>' )
                *err = RESTP_CHAR_INT_GREAT;
            if ( oper == '=' )
                *err = RESTP_CHAR_INT_EQ;
            if ( oper == QChar(KS_POWER) )
                *err = RESTP_CHAR_INT_POW;
            if ( oper == KS_NEQ )
                *err = RESTP_CHAR_INT_NEQ;
            if ( oper == KS_LEQ )
                *err = RESTP_CHAR_INT_LEQ;
            if ( oper == KS_GEQ )
                *err = RESTP_CHAR_INT_GEQ;
            if ( oper == QChar(KS_OR) )
                *err = RESTP_CHAR_INT_OR;
            if ( oper == QChar(KS_XOR) )
                *err = RESTP_CHAR_INT_XOR;
            if ( oper == QChar(KS_AND) )
                *err = RESTP_CHAR_INT_AND;
            return Kumir::Undefined;
        }
        if ( type2 == Kumir::BoolOrDecimal )
        {

            if(oper=='>')
                *err = RESTP_CHAR_BL_MORE;
            if(oper=='<')
                *err = RESTP_CHAR_BL_LESS;
            if(oper=='=')
                *err = RESTP_CHAR_BL_EQ;

            if(oper==KS_NEQ)
                *err = RESTP_CHAR_BL_NEQ;
            if(oper==KS_GEQ)
                *err = RESTP_CHAR_BL_GEQ;
            if(oper==KS_LEQ)
                *err = RESTP_CHAR_BL_LEQ;

            if ( oper == QChar(KS_OR) )
                *err = RESTP_CHAR_BL_OR;
            if ( oper == QChar(KS_XOR) )
                *err = RESTP_CHAR_BL_XOR;
            if ( oper == QChar(KS_AND) )
                *err = RESTP_CHAR_BL_AND;

            return Kumir::Undefined;
        }

        if ( type2 == Kumir::BoolOrLiteral )
        {
            if(oper=='>')
                return Kumir::BoolOrLiteral;
            if(oper=='<')
                return Kumir::BoolOrLiteral;
            if(oper=='=')
                return Kumir::BoolOrLiteral;

            if(oper==KS_NEQ)
                return Kumir::BoolOrLiteral;
            if(oper==KS_GEQ)
                return Kumir::BoolOrLiteral;
            if(oper==KS_LEQ)
                return Kumir::BoolOrLiteral;

            if ( oper == QChar(KS_OR) )
                *err = RESTP_CHAR_BN_OR;
            if ( oper == QChar(KS_XOR) )
                *err = RESTP_CHAR_BN_XOR;
            if ( oper == QChar(KS_AND) )
                *err = RESTP_CHAR_BN_AND;

            return Kumir::Undefined;
        }
        *err = RESTP_CHAR;
        return Kumir::Undefined;
    } //string

    if ( type1 == Kumir::BoolOrDecimal )
    {
        if ( type2 == Kumir::Integer )
        {
            if(oper=='>')
                return Kumir::BoolOrDecimal;
            if(oper=='<')
                return Kumir::BoolOrDecimal;
            if(oper=='=')
                return Kumir::BoolOrDecimal;


            if(oper==KS_NEQ)
                return Kumir::BoolOrDecimal;
            if(oper==KS_GEQ)
                return Kumir::BoolOrDecimal;
            if(oper==KS_LEQ)
                return Kumir::BoolOrDecimal;

            if ( oper == QChar(KS_OR) )
                *err = RESTP_BN_INT_OR;
            if ( oper == QChar(KS_XOR) )
                *err = RESTP_BN_INT_XOR;
            if ( oper == QChar(KS_AND) )
                *err = RESTP_BN_INT_AND;

            return Kumir::Undefined;
        }
        else if ( type2 == Kumir::Real )
        {
            if(oper=='>')
                return Kumir::BoolOrDecimal;
            if(oper=='<')
                return Kumir::BoolOrDecimal;
            if(oper=='=')
                return Kumir::BoolOrDecimal;

            if(oper==KS_NEQ)
                return Kumir::BoolOrDecimal;
            if(oper==KS_GEQ)
                return Kumir::BoolOrDecimal;
            if(oper==KS_LEQ)
                return Kumir::BoolOrDecimal;

            if ( oper == QChar(KS_OR) )
                *err = RESTP_BN_REAL_OR;
            if ( oper == QChar(KS_XOR) )
                *err = RESTP_BN_REAL_XOR;
            if ( oper == QChar(KS_AND) )
                *err = RESTP_BN_REAL_AND;
            return Kumir::Undefined;
        }
        else if ( type2 == Kumir::String )
        {

            if ( oper == '+' )
                *err = RESTP_BN_STR_PLUS;
            if ( oper == '-' )
                *err = RESTP_BN_STR_MINUS;
            if ( oper == '*' )
                *err = RESTP_BN_STR_MUL;
            if ( oper == '/' )
                *err = RESTP_BN_STR_DIV;
            if ( oper == '<' )
                *err = RESTP_BN_STR_LESS;
            if ( oper == '>' )
                *err = RESTP_BN_STR_GREAT;
            if ( oper == '=' )
                *err = RESTP_BN_STR_EQ;
            if ( oper == QChar(KS_POWER) )
                *err = RESTP_BN_STR_POW;
            if ( oper == KS_NEQ )
                *err = RESTP_BN_STR_NEQ;
            if ( oper == KS_LEQ )
                *err = RESTP_BN_STR_LEQ;
            if ( oper == KS_GEQ )
                *err = RESTP_BN_STR_GEQ;
            if ( oper == QChar(KS_OR) )
                *err = RESTP_BN_STR_OR;
            if ( oper == QChar(KS_XOR) )
                *err = RESTP_BN_STR_XOR;
            if ( oper == QChar(KS_AND) )
                *err = RESTP_BN_STR_AND;
            return Kumir::Undefined;
        }
        else if ( type2 == Kumir::Charect )
        {

            if ( oper == '+' )
                *err = RESTP_BN_CHAR_PLUS;
            if ( oper == '-' )
                *err = RESTP_BN_CHAR_MINUS;
            if ( oper == '*' )
                *err = RESTP_BN_CHAR_MUL;
            if ( oper == '/' )
                *err = RESTP_BN_CHAR_DIV;
            if ( oper == '<' )
                *err = RESTP_BN_CHAR_LESS;
            if ( oper == '>' )
                *err = RESTP_BN_CHAR_GREAT;
            if ( oper == '=' )
                *err = RESTP_BN_CHAR_EQ;
            if ( oper == QChar(KS_POWER) )
                *err = RESTP_BN_CHAR_POW;
            if ( oper == KS_NEQ )
                *err = RESTP_BN_CHAR_NEQ;
            if ( oper == KS_LEQ )
                *err = RESTP_BN_CHAR_LEQ;
            if ( oper == KS_GEQ )
                *err = RESTP_BN_CHAR_GEQ;
            if ( oper == QChar(KS_OR) )
                *err = RESTP_BN_CHAR_OR;
            if ( oper == QChar(KS_XOR) )
                *err = RESTP_BN_CHAR_XOR;
            if ( oper == QChar(KS_AND) )
                *err = RESTP_BN_CHAR_AND;
            return Kumir::Undefined;
        }
        else if ( type2 == Kumir::Boolean )
        {
            if ( oper == QChar(KS_OR) )
                return Kumir::Boolean;
            if ( oper == QChar(KS_XOR) )
                return Kumir::Boolean;
            if ( oper == QChar(KS_AND) )
                return Kumir::Boolean;
            if ( oper == QChar('=') )
                return Kumir::Boolean;
            if ( oper == KS_NEQ )
                return Kumir::Boolean;

            if ( oper == '<' )
                *err = RESTP_BN_BOOL_LESS;
            if ( oper == '>' )
                *err = RESTP_BN_BOOL_GREAT;
            // 			if ( oper == '=' )
            // 				*err = RESTP_BN_BOOL_EQ;


            // 			if ( oper == KS_NEQ )
            // 				*err = RESTP_BN_BOOL_NEQ;
            if ( oper == KS_LEQ )
                *err = RESTP_BN_BOOL_LEQ;
            if ( oper == KS_GEQ )
                *err = RESTP_BN_BOOL_GEQ;
            return Kumir::Undefined;
        }
        else if ( type2 == Kumir::BoolOrDecimal )
        {
            if ( oper == QChar(KS_OR) )
                return Kumir::Boolean;
            if ( oper == QChar(KS_XOR) )
                return Kumir::Boolean;
            if ( oper == QChar(KS_AND) )
                return Kumir::Boolean;
            if ( oper == '<' )
                return Kumir::BoolOrDecimal;
            if ( oper == '>' )
                return Kumir::BoolOrDecimal;
            if ( oper == '=' )
                return Kumir::BoolOrDecimal;


            if ( oper == KS_NEQ )
                return Kumir::BoolOrDecimal;
            if ( oper == KS_LEQ )
                return Kumir::BoolOrDecimal;
            if ( oper == KS_GEQ )
                return Kumir::BoolOrDecimal;

            return Kumir::Undefined;
        }
        else if ( type2 == Kumir::BoolOrLiteral )
        {
            if ( oper == QChar(KS_OR) )
                return Kumir::Boolean;
            if ( oper == QChar(KS_XOR) )
                return Kumir::Boolean;
            if ( oper == QChar(KS_AND) )
                return Kumir::Boolean;
            if ( oper == '<' )
                *err = RESTP_BN_BL_LESS;
            if ( oper == '>' )
                *err = RESTP_BN_BL_MORE;
            if ( oper == '=' )
                *err = RESTP_BN_BL_EQ;

            if ( oper == KS_NEQ )
                *err = RESTP_BN_BL_NEQ;
            if ( oper == KS_LEQ )
                *err = RESTP_BN_BL_LEQ;
            if ( oper == KS_GEQ )
                *err = RESTP_BN_BL_GEQ;
            return Kumir::Undefined;
        }
    } // Kumir::BoolOrDecimal

    if ( type1 == Kumir::BoolOrLiteral )
    {
        if ( type2 == Kumir::Charect )
        {
            if(oper=='>')
                return Kumir::BoolOrLiteral;
            if(oper=='<')
                return Kumir::BoolOrLiteral;
            if(oper=='=')
                return Kumir::BoolOrLiteral;


            if(oper==KS_NEQ)
                return Kumir::BoolOrLiteral;
            if(oper==KS_GEQ)
                return Kumir::BoolOrLiteral;
            if(oper==KS_LEQ)
                return Kumir::BoolOrLiteral;

            if ( oper == QChar(KS_OR) )
                *err = RESTP_BL_CHAR_OR;
            if ( oper == QChar(KS_XOR) )
                *err = RESTP_BL_CHAR_XOR;
            if ( oper == QChar(KS_AND) )
                *err = RESTP_BL_CHAR_AND;

            return Kumir::Undefined;
        }
        else if ( type2 == Kumir::String )
        {
            if(oper=='>')
                return Kumir::BoolOrLiteral;
            if(oper=='<')
                return Kumir::BoolOrLiteral;
            if(oper=='=')
                return Kumir::BoolOrLiteral;

            if(oper==KS_NEQ)
                return Kumir::BoolOrLiteral;
            if(oper==KS_GEQ)
                return Kumir::BoolOrLiteral;
            if(oper==KS_LEQ)
                return Kumir::BoolOrLiteral;

            if ( oper == QChar(KS_OR) )
                *err = RESTP_BL_STR_OR;
            if ( oper == QChar(KS_XOR) )
                *err = RESTP_BL_STR_XOR;
            if ( oper == QChar(KS_AND) )
                *err = RESTP_BL_STR_AND;
            return Kumir::Undefined;
        }
        else if ( type2 == Kumir::Real )
        {

            if ( oper == '+' )
                *err = RESTP_BL_REAL_PLUS;
            if ( oper == '-' )
                *err = RESTP_BL_REAL_MINUS;
            if ( oper == '*' )
                *err = RESTP_BL_REAL_MUL;
            if ( oper == '/' )
                *err = RESTP_BL_REAL_DIV;
            if ( oper == '<' )
                *err = RESTP_BL_REAL_LESS;
            if ( oper == '>' )
                *err = RESTP_BL_REAL_GREAT;
            if ( oper == '=' )
                *err = RESTP_BL_REAL_EQ;
            if ( oper == QChar(KS_POWER) )
                *err = RESTP_BL_REAL_POW;
            if ( oper == KS_NEQ )
                *err = RESTP_BL_REAL_NEQ;
            if ( oper == KS_LEQ )
                *err = RESTP_BL_REAL_LEQ;
            if ( oper == KS_GEQ )
                *err = RESTP_BL_REAL_GEQ;
            if ( oper == QChar(KS_OR) )
                *err = RESTP_BL_REAL_OR;
            if ( oper == QChar(KS_XOR) )
                *err = RESTP_BL_REAL_XOR;
            if ( oper == QChar(KS_AND) )
                *err = RESTP_BL_REAL_AND;
            return Kumir::Undefined;
        }
        else if ( type2 == Kumir::Integer )
        {

            if ( oper == '+' )
                *err = RESTP_BL_INT_PLUS;
            if ( oper == '-' )
                *err = RESTP_BL_INT_MINUS;
            if ( oper == '*' )
                *err = RESTP_BL_INT_MUL;
            if ( oper == '/' )
                *err = RESTP_BL_INT_DIV;
            if ( oper == '<' )
                *err = RESTP_BL_INT_LESS;
            if ( oper == '>' )
                *err = RESTP_BL_INT_GREAT;
            if ( oper == '=' )
                *err = RESTP_BL_INT_EQ;
            if ( oper == QChar(KS_POWER) )
                *err = RESTP_BL_INT_POW;
            if ( oper == KS_NEQ )
                *err = RESTP_BL_INT_NEQ;
            if ( oper == KS_LEQ )
                *err = RESTP_BL_INT_LEQ;
            if ( oper == KS_GEQ )
                *err = RESTP_BL_INT_GEQ;
            if ( oper == QChar(KS_OR) )
                *err = RESTP_BL_INT_OR;
            if ( oper == QChar(KS_XOR) )
                *err = RESTP_BL_INT_XOR;
            if ( oper == QChar(KS_AND) )
                *err = RESTP_BL_INT_AND;
            return Kumir::Undefined;
        }
        else if ( type2 == Kumir::Boolean )
        {
            if ( oper == QChar(KS_OR) )
                return Kumir::Boolean;
            if ( oper == QChar(KS_XOR) )
                return Kumir::Boolean;
            if ( oper == QChar(KS_AND) )
                return Kumir::Boolean;
            if ( oper == QChar('=') )
                return Kumir::Boolean;
            if ( oper == KS_NEQ )
                return Kumir::Boolean;

            if ( oper == '<' )
                *err = RESTP_BL_BOOL_LESS;
            if ( oper == '>' )
                *err = RESTP_BL_BOOL_GREAT;
            // 			if ( oper == '=' )
            // 				*err = RESTP_BL_BOOL_EQ;

            // 			if ( oper == KS_NEQ )
            // 				*err = RESTP_BL_BOOL_NEQ;
            if ( oper == KS_LEQ )
                *err = RESTP_BL_BOOL_LEQ;
            if ( oper == KS_GEQ )
                *err = RESTP_BL_BOOL_GEQ;
            return Kumir::Undefined;
        }
        else if ( type2 == Kumir::BoolOrDecimal )
        {
            if ( oper == QChar(KS_OR) )
                return Kumir::Boolean;
            if ( oper == QChar(KS_XOR) )
                return Kumir::Boolean;
            if ( oper == QChar(KS_AND) )
                return Kumir::Boolean;


            if ( oper == '<' )
                *err = RESTP_BL_BN_LESS;
            if ( oper == '>' )
                *err = RESTP_BL_BN_MORE;
            if ( oper == '=' )
                *err = RESTP_BL_BN_EQ;

            if ( oper == KS_NEQ )
                *err = RESTP_BL_BN_NEQ;
            if ( oper == KS_LEQ )
                *err = RESTP_BL_BN_LEQ;
            if ( oper == KS_GEQ )
                *err = RESTP_BL_BN_GEQ;

            return Kumir::Undefined;
        }
        else if ( type2 == Kumir::BoolOrLiteral )
        {
            if ( oper == QChar(KS_OR) )
                return Kumir::Boolean;
            if ( oper == QChar(KS_XOR) )
                return Kumir::Boolean;
            if ( oper == QChar(KS_AND) )
                return Kumir::Boolean;

            if ( oper == '<' )
                return Kumir::BoolOrLiteral;
            if ( oper == '>' )
                return Kumir::BoolOrLiteral;
            if ( oper == '=' )
                return Kumir::BoolOrLiteral;

            if ( oper == KS_NEQ )
                return Kumir::BoolOrLiteral;
            if ( oper == KS_LEQ )
                return Kumir::BoolOrLiteral;
            if ( oper == KS_GEQ )
                return Kumir::BoolOrLiteral;

            return Kumir::Undefined;
        }
    } // Kumir::BoolOrLiteral

    *err=RESTP_FINAL;
    return Kumir::Undefined;
}


QString TextAnalizer::dropSpace(QString viraj, QList<int> &P, QList<int> &L)//,QList<int> *skobka,QList<int> *sk_type)
{
    QList<int> rP, rL;
    QString to_ret;
    bool inLit = false;
    bool space = false;
    bool first = true;
    for(int i=0;i<viraj.length();i++)
    {
        QChar next;
        if ( i<(viraj.length()-1) )
            next = viraj[i+1];
        else
            next = ' ';
        if(viraj[i]==' ' || viraj[i]=='\t')
        {
            if (
                    (((next.unicode()<1000)&&(next.unicode()>63))||((next.unicode()>1006)&&(next.unicode()<3500))||(next.isNumber()))
                    &&!first
                    )
            {
                to_ret=to_ret+viraj[i];
                rP << P[i];
                rL << L[i];
            }
            // 					if (inLit||!space) {
            // 						to_ret=to_ret+viraj[i];
            // 						rP << P[i];
            // 						rL << L[i];
            // 					}
            // 					space = true;
        }
        else {
            to_ret=to_ret+viraj[i];
            rP << P[i];
            rL << L[i];
            space = false;
            first = false;
        }
        if (viraj[i]=='"')
            inLit = !inLit;
    }
    P = rP;
    L = rL;
    return to_ret;
}

int ipow(int base, int power)
{
    int result = 1;
    for ( int i=0; i<power; i++ ) {
        result *= base;
    }
    return result;
}

int hexValue(QString strValue)
{
    strValue = strValue.toLower();
    int power = 0;
    int result = 0;
    while ( !strValue.isEmpty() ) {
        QChar c = strValue[strValue.length()-1];
        int base = 0;
        if ( c == '0' ) base = 0;
        else if ( c=='1') base = 1;
        else if ( c=='2') base = 2;
        else if ( c=='3') base = 3;
        else if ( c=='4') base = 4;
        else if ( c=='5') base = 5;
        else if ( c=='6') base = 6;
        else if ( c=='7') base = 7;
        else if ( c=='8') base = 8;
        else if ( c=='9') base = 9;
        else if ( c=='a') base = 10;
        else if ( c=='b') base = 11;
        else if ( c=='c') base = 12;
        else if ( c=='d') base = 13;
        else if ( c=='e') base = 14;
        else if ( c=='f') base = 15;
        result += base*ipow(16,power);
        power++;
        strValue.truncate(strValue.length()-1);
    }
    return result;
}

QVariant TextAnalizer::createConstValue(const QString &str, const Kumir::ValueType pt)
{
    Q_ASSERT ( !str.isEmpty() );
    QVariant result;
    if ( pt==Kumir::Integer ) {
        int base=10;
        bool ok;
        QString const_value = str;
        if(const_value.startsWith("$"))
        {
            base=16;
            const_value=const_value.mid(1,const_value.length()-1);
        }
        if(const_value.startsWith("0x"))
            base=16;
        int int_value = const_value.toInt(&ok,base);
        if ( base == 16 )
            int_value = hexValue(const_value);
        result = QVariant(int_value);
    }
    else if ( pt==Kumir::Real ) {
        QString v = str;
        v.replace(QString::fromUtf8("Е"),"E");
        v.replace(QString::fromUtf8("е"),"e");
        v.replace("e","E");
        bool ok;
        double d = v.toDouble(&ok);
        result = QVariant(d);
    }
    else if ( pt==Kumir::String ) {
        QString ready_const;
        for(int i=1;i<str.length()-1;i++)
        {
            ready_const=ready_const+QChar(str[i].unicode()-STR_HIDE_OFFSET);
        }
        result = QVariant(ready_const);
    }
    else if ( pt==Kumir::Charect ) {
        QString ready_const;
        for(int i=1;i<str.length()-1;i++)
        {
            QChar z =  QChar(str[i].unicode()-STR_HIDE_OFFSET);
            ready_const=ready_const+QChar(str[i].unicode()-STR_HIDE_OFFSET);
        }
        Q_ASSERT( ready_const.length()==1 );
        result = QVariant(ready_const[0]);
    }
    else if ( pt==Kumir::Boolean ) {
        QString yes = QString(KS_TRUE);
        QString no = QString(KS_FALSE);
        Q_ASSERT ( str==yes || str==no );
        if ( str==no )
            result = QVariant(false);
        if ( str==yes )
            result = QVariant(true);
    }
    else {
        qFatal("Trying to create Constant of non-regular type");
    }
    return result;
}


int set_const_value(int perem_id,const QString &val, SymbolTable &symbols)
{
    QString const_value = val;

    if(const_value.isEmpty())
    {
        symbols.symb_table[perem_id].value->setIntegerValue(0);
        symbols.setUsed(perem_id,TRUE);
        return 0;
    }
    if(symbols.getTypeByID(perem_id)==Kumir::Integer)
    {
        int int_value = TextAnalizer::createConstValue(val,Kumir::Integer).toInt();
        symbols.symb_table[perem_id].value->setIntegerValue(int_value);
        symbols.setUsed(perem_id,TRUE);
    }


    if(symbols.getTypeByID(perem_id)==Kumir::Real)
    {
        double d = TextAnalizer::createConstValue(val,Kumir::Real).toDouble();
        symbols.symb_table[perem_id].value->setFloatValue(d);
        symbols.setUsed(perem_id,TRUE);
    }

    if(symbols.getTypeByID(perem_id)==Kumir::String)
    {
        QString s = TextAnalizer::createConstValue(val,Kumir::String).toString();
        symbols.symb_table[perem_id].value->setStringValue(s);
        symbols.setUsed(perem_id,TRUE);
    }

    // NEW 28.08.2006
    if (symbols.getTypeByID(perem_id)==Kumir::Boolean)
    {
        bool value = TextAnalizer::createConstValue(val,Kumir::Boolean).toBool();
        symbols.symb_table[perem_id].value->setBoolValue(value);
        symbols.setUsed(perem_id,TRUE);
    }
    if(symbols.getTypeByID(perem_id)==Kumir::Charect)
    {
        QChar c = TextAnalizer::createConstValue(val,Kumir::Charect).toChar();
        symbols.symb_table[perem_id].value->setCharectValue(c);
        symbols.setUsed(perem_id,TRUE);
    }
    //
    /// END NEW

    return 0;
}


/**
 * Определяет внутренний тип выражения. Является основной для исползования.
 * @param viraj выражение
 * @param table таблица алгоритмов
 * @param symbols таблица переменных
 * @param algorithm имя алгоритма в котором происходит разбор
 * @param err Вывод ошибки
 * @param parsedViraj Spisok podvyrazhenii v D-zapisi (see below)
 * @param err_block  Вывод местополоджения ошибки
 * @return Тип выражения
 */
/*
Kumir::ValueType TextAnalizer::parceExpression(
                QString			viraj,			// In: Исходное выражение (текст)
                FunctionTable	*table,			// In: Таблица имен функций
                SymbolTable	*symbols,		// In: Таблица имен переменных
                QString			algorithm,		// In: Имя алгоритма (global, если вне?)
                int				*err,			// Out:Код ошибки (0 - ОК)
                QStringList		*parsedViraj,	// Out:список глпаных подвыражений (см. ниже)
                                                                        //     каждое подвыражение представлено
                                                                        //	   в Д-форме (см. ниже)
                int				*err_block,		// Out:номер ошиб. блока, устарело (см. ниже)
//MR-New
        StructType		*virStruct
//RM
)

        Производит аналтз корректности записи выражения в соответствии с Описанием
         (ч.1, раздел 6; ч.2, раздел 2, пп.2.3-2.5);
         строит внутреннее предсталение выражения - список главных подвыражений;
         возвращает тип выражения

        Терминология.
        1. В соответствии с определениями пп.2.3-2.5 (ч.2) каждое выражение можно представить
в виде корневого дерева (дерева разбора), каждая вершина дерева помечена выражением
и правилом вывода, в соответствии с которым это дерево порождено.
        Главное подвыражение вырадения Е - это подвыражение, которое встречается в
дереве разбора Е.
        Замечание. Если Е1 - главное подвыражение Е, то поддерево дерева разбора Е с
корнем в Е1 - это дерево разбора Е1.
        2. На множестве главных подвыражений определен



Razobraanoe vyrazhenie - eto vyrazhenie v D-zapisi, t.e. v vide
[Oper0] Obj0 {Oper Obj}

zdes' Oper0 - eto znak unarnoi operatsii;
Obj0, Obj - eto object;
Oper - znak binarnoi operatsii
(vse probely udaleny).
Znak unarnoi operatsii - eto UNARY_PLUS, UNARY_MINUS, NOT
(??? Kogda '+' i '-' zameniautsia na  UNARY_PLUS, UNARY_MINUS ??)
Znak binarnoi operatsii - eto
arifm.oper ILI boolen oper ILI concaten (+) ILI sravnenie
(see Delimiters)
Obj - eto:
^<nomer> - Var ili  Const (v tom chisle - sdtroka)
??? NewGet ne ispolzuetsya pri obrabotke imen massivov, a ne ih elementiv ??
???    e.g. pri vyzove functsiy ????
@<nomer>{&<nom_vyr>} - vyzov Func, zatem - ssylka na vyrazh-argum
%<nomer>{{&<nom_vyr>}} - element massiva, t.e.
imya massiva, zatem - nepustoi spisok znachenii indexov
|<nomer>&<nom_vyr> - element stroki (charact)
|<nomer>&<nom_vyr>&<nom_vyr> - vyrezka iz stroki
<nomer> - ssylka v tablitsu perem ili algoritmov;
<nom_vyr> - ssylka na predshestv element parsedViraj

        Vozvrashchaet:
tip v sootvetstvii s imenem (NO MASS !!!!) - krome "%<nomer>"
esli %<nomer> - mass_.
*/

/**
* Возвращает начальную позицию и длину ошибки.
* @param start IN: позиция начала ошибки
* @param end IN: позиция конца ошибки
* @param P IN: вектор P (см. описание)
* @param L IN: вектор L (см. описание)
* @return позиция начала ошибки (с учетом преобразований по таблице) и ее длина
*/
QPoint translateErrorPosition(int start, int end, const QList<int> &P, const QList<int> &L)
{
    if (start==-1) {
        // Ошибка внутри сгенерированного кода
        return QPoint(-1,-1);
    }
    int min = 0;
    int max = 0;
    bool minSet = false;
    bool maxSet = false;
    int p = 0;
    int l = 0;
    for ( int i=start; i<end; i++ ) {
        p = P[i];
        l = L[i];
        if ( minSet )
            min = qMin(min,P[i]);
        else {
            min = P[i];
            minSet = true;
        }
        if ( maxSet )
            max = qMax(max,P[i]+L[i]);
        else {
            max = P[i]+L[i];
            maxSet = true;
        }
    }
    return QPoint(min,max-min);
}


Kumir::ValueType TextAnalizer::parceExpression (
        QString expression,
        FunctionTable &functions,
        SymbolTable &symbols,
        QString algorhitm,
        QStringList &parsedExpression,
        StructType  &expressionType,
        Kumir::CompileError &err,
        QStringList &extraSubExpressions,
        QString &xmlRoot
        )
{
    err.code = 0;

    enum  BlockType // Block = fragment between delimiters
    {
        None,           // empty or error
        Sim_perem,      // SimpleVar or Const or 0-ary function
        //  (vozmozhno, - imya massiva bez [] ili imya stroki
        Mass,           // Array or String, posle kotoryh idut []
        Sim_funct       // Function with ()
    };

    BlockType cur_block_type=None;
    QString out_put_string="";
    QString work_block;
    //  expression = expression.trimmed();



    //              A. Prolog
    int cur_pos=0; // Position in expression
    int cur_block_id=0; // Block number

    QChar oper=' ',last_oper=' '; // Current (oper) and previous (last_oper) delimiters

    // Pozitsii i tipy skobok (according skobka? See G.1 ??).
    //  QList<int> skobk_pos,skobk_type;
    //  QList<bool> op_skobka_is_funct,close_skobka_is_funct;   //  ????

    //      Remove Spaces and
    //    Set Skobka positions and types:

    QList<int> P, L, OP, OL;

    for ( int i=0; i<expression.length(); i++ ) {
        P << i;
        L << 1;
    }

    expression=dropSpace ( expression, P, L );//, &skobk_pos, &skobk_type );

    //  qDebug() << "P: " << P;
    //  qDebug() << "L: " << L;

    //              Main loop  (B-F)
    //      One prohod - one block.
    //      Exclusion: Exponential double

    // NEW V.Y. 21.08.2006
    cur_block_id = -2;

    int virLength = expression.length();
    while ( cur_pos<virLength )
    {
        //          B. Prolog of Main loop
        cur_block_id++;
        cur_block_type=None;

        //      B1. Search next delimiter
        int new_cur_pos=operators.indexIn ( expression,cur_pos );
        if ( new_cur_pos == -1 || new_cur_pos-cur_pos>=1 ) {
            bool allowEmptyOper = false;
            if ( new_cur_pos != -1 )
                allowEmptyOper = expression[new_cur_pos]==')' || expression[new_cur_pos]==']';
            if (!allowEmptyOper&&(oper==')' || oper==']')) {
                err.position = cur_pos;
                err.length = new_cur_pos==-1? expression.length()-cur_pos : new_cur_pos-cur_pos;
                err.code = GNVR_MISSED_OPERATOR;
                return Kumir::Undefined;
            }
        }
        if ( new_cur_pos==-1 )
        {
            new_cur_pos=expression.length();
            oper = ' ';
        }
        else
        {
            oper=expression[new_cur_pos];
        }

        //      B2. Get current block
        work_block=expression.mid ( cur_pos,new_cur_pos-cur_pos );


        //          C. Work exponential double
        //      C1. Check current block for Mantissa:
        //   1) starts with digit or point;
        //   2) ends with E (latin or cyrillic)
        //   3) next delimiter '+' or '-'
        bool expf;
        int test;
        if ( new_cur_pos <= cur_pos && work_block.trimmed().length()>0 )
        {
            expf = false;
            test = 0;
        }
        else
        {
            QString wbt = work_block.trimmed();
            test = wbt[0].digitValue();
            if ( wbt[0]=='.' )
                test = 10;
            expf = false;
            if (!wbt.isEmpty())
                expf= ( wbt[wbt.length()-1]=='E' ) || ( wbt[wbt.length()-1]=='e' ) ||
                      ( wbt[wbt.length()-1]==QChar ( 1045 ) ) || ( wbt[wbt.length()-1]==QChar ( 0x0435 ) ) ||
                      ( wbt.count('E')==1 ) || ( wbt.count('e')==1 ) || ( wbt.count(QChar ( 1045 ))==1 ) || ( wbt.count(QChar ( 0x0435 ))==1 ) ;
        }

        bool check_expf = ( expf ) && ( test>=0 ) && ( ( oper=='-' ) || ( oper=='+' ) );
        if ( check_expf )
        {
            //          !!! Candidate to exp double !!!
            //      C2. Check it carefully
            //      this (=left) and next (=right) are candidates to exp double
            cur_pos=new_cur_pos+1;
            new_cur_pos=operators.indexIn ( expression,cur_pos );
            if ( new_cur_pos==-1 )
                new_cur_pos=expression.length();

            //      left: kill last 'E'
            QString work_block1;
            work_block1=work_block.left ( work_block.length()-1 );

            //      left must be double
            bool const_t;
            double left=work_block1.toFloat ( &const_t );
            Q_UNUSED(left);

            if ( !const_t )
            {

                err.code = GNVR_BAD_EXP_LEFT;
                err.position = cur_pos;
                err.length = new_cur_pos - cur_pos;
                return Kumir::Undefined;
            }
            //      set right
            work_block=expression.mid ( cur_pos,new_cur_pos-cur_pos );
            //      right must be Kumir::Integer
            double right=work_block.toInt ( &const_t );
            Q_UNUSED(right);
            if ( !const_t )
            {

                err.code = GNVR_BAD_EXP_RIGHT;
                err.position = cur_pos;
                err.length = new_cur_pos - cur_pos;
                return Kumir::Undefined;
            }

            //      At last: reright block into decimal

            work_block = work_block1+"E"+oper+work_block;

            int virLength = expression.length();
            if ( new_cur_pos<virLength )
            {
                oper=expression[new_cur_pos];
            }
            else
            {
                oper = ' ';
            }
            // increment cur_block_id - for error mess
            cur_block_id++;       //
        } // if(check_expf)


        //          D. !!! Set Block Type - check next delimiter
        //  LEMMA.
        //    1. oper = '(' && NonEmpty => Function name
        //    2. oper = '[' ==>  NonEmpty && Array (or string) name
        //    3. Otherwise: NonEmpty => SimpleVar or Const or 0-ary Function
        //          or golyi massive (v vyzove funkcii) !!!         //MR-New


        if ( oper == '(' ) {
            if ( ( work_block.simplified().length() >0 )  && ( work_block.simplified() != QString ( QChar ( KS_NOT ) ) ) )
            {
                cur_block_type=Sim_funct;
            }
            else
                cur_block_type=None;
        }
        else if ( oper == '[' ) {
            if ( work_block.simplified().length() ==0 )
            {
                //    Empty ==> Error
                err.code = GNVR_NO_ARRAY_NAME; // Empty before '['
                QPoint pnt = translateErrorPosition(new_cur_pos,new_cur_pos+1,P,L);
                err.position = pnt.x();
                err.length = 1;
                return Kumir::Undefined;
            }
            cur_block_type=Mass; // !!!!  Empty => Error ??
        }

        else {
            if ( work_block.trimmed().length() >0 )
            {
                cur_block_type=Sim_perem;
            }
            else
            {
                // NEW V.Y. 22.08.2006
                if  ( last_oper == ',' )
                {
                    if ( oper == '-' )
                    {
                        oper = QChar(UNARY_MINUS);
                    }
                    else if ( oper == '+' )
                    {
                        oper = QChar(UNARY_PLUS);
                    }
                    else
                    {
                        err.position = cur_pos-1;
                        err.length = 1;
                        err.code = GNVR_EXTRA_COMMA;
                        return Kumir::Undefined;
                    }
                }
                cur_block_type=None; // Empty block has type None
            }
        }




        //          E1. Work NOT
        bool not_flag=false;
        int not_pos = 0;

        //      work_block = work_block.trimmed();
        //      qDebug("zzz: %s, %s",expression.utf8().data(),work_block.utf8().data());
        //      no NOT v konce!!!!!
        if ( ( work_block.trimmed().length() > 1 ) && ( work_block.trimmed()[work_block.trimmed().length()-1]== QChar ( KS_NOT ) )  )
        {
            err.code = GNVR_NOT_END;
            //          *err_block=cur_block_id;
            int t = expression.indexOf(QChar(KS_NOT),cur_pos);
            Q_ASSERT ( t>=0 );
            if ( t>=0 )
                not_pos = t;
            QPoint pnt = translateErrorPosition(not_pos,not_pos+1,P,L);
            err.position = pnt.x();
            err.length = pnt.y();
            return Kumir::Undefined;
        }

        //      no more than one NOT
        if ( work_block.count ( QChar ( KS_NOT ) ) >1 )
        {
            err.position = cur_pos;
            err.length = new_cur_pos - cur_pos;
            int t = expression.indexOf(KS_NOT,cur_pos);
            Q_ASSERT ( t>=0 );
            t = expression.indexOf(KS_NOT,t+1);
            if ( t>=0 )
                not_pos = t;
            QPoint pnt = translateErrorPosition(not_pos,not_pos+1,P,L);
            err.position = pnt.x();
            err.length = pnt.y();
            err.code = GNVR_MANY_NOT;
            return Kumir::Undefined;
        }

        //      one NOT is NOT
        if ( work_block.count ( QChar ( KS_NOT ) ) ==1 )
        {
            int t = expression.indexOf(QChar(KS_NOT),cur_pos);
            Q_ASSERT ( t>=0 );
            if ( t>=0 )
                not_pos = t;
            work_block.replace ( QChar ( KS_NOT ),"" );
            not_flag=true;
        }

        //          E2. Replace unary '-' and '+' with UNARY_MINUS and UNARY_PLUS
        if ( ( cur_block_type==None ) &&
             ( ( last_oper==' ' ) ||
               ( last_oper=='(' ) ||
               ( last_oper=='[' ) ||
               ( last_oper=='<' ) ||
               ( last_oper=='>' ) ||
               ( last_oper=='=' ) ||
               ( last_oper==KS_GEQ ) ||
               ( last_oper==KS_LEQ ) ||
               ( last_oper==KS_NEQ )
               )
            )
            {
            QChar local_oper;

            if ( oper == '-' )
            {
                local_oper = UNARY_MINUS;
            }
            else if ( oper == '+' )
            {
                local_oper = UNARY_PLUS;
            }
            else
            {
                local_oper = oper;
            }


            oper = local_oper;
        } // end if (....)

        //          E3. Remove spaces
        //      work_block=work_block.simplified();

        //              F. !!! Append out_put_string - the inner representation-1
        //    On this stage we replace every name (including consts)
        //  with the group <char><Kumir::Integer>  where
        //     <char> denotes the Block Type;
        //     <Kumir::Integer> is the id  in name functions
        //  Notation for Block type:
        //          ^ - Sim_perem
        //          @ - Sim funct
        //          % - Mass
        //          & - podvyrazhenie (poiavitsya pozzhe)
        //
        //   NB:  esli <char> = '@', to id - ssylka v tabl functiy;
        //      esli <char> = '%', to id - ssylka v tabl imen;
        //        esli <char> = '^', to my id mozhet byt'
        //          ssylkoi v tabl functiy )0-arnaia function)
        //          ili v tabl imen (const ili simple var);
        //
        //  ================================================================

        //          F0. Check for wrong ksimbols within the name
        /*MR@@-99   Dopisat' pozzhe ??
                bool  test = CheckWrongKsi(work_block);
                if (!test){
                *err_block=cur_block_id;
                *err= WRONG_SYMB_IN_NAME;
                return Kumir::Undefined;
                };
                */

        switch ( cur_block_type )
        {
            //          F1. Functions
        case Sim_funct:
            {
                int tn_err = 0, tn_start = 0, tn_len = 0;
                tn_err = m_textNormalizer->test_name(work_block, tn_start, tn_len);
                if ( tn_err > 0 ) {
                    QPoint pnt = translateErrorPosition(cur_pos+tn_start,cur_pos+tn_start+tn_len,P,L);
                    err.position = pnt.x();
                    err.length = pnt.y();
                    err.code = tn_err;
                    return Kumir::Undefined;
                }
                int func_id=-1;
                int module_id=-1;

                QPair<int,int> funcRef = m_modulesPool->funcByName(work_block.simplified());
                module_id = funcRef.first;
                func_id = funcRef.second;

                if ( func_id==-1 )
                {
                    err.position = cur_pos;
                    err.length = new_cur_pos - cur_pos;
                    err.code = GNVR_NO_IMPL;
                    return Kumir::Undefined;
                };
                if ( m_modulesPool->functionTable(module_id)->isBroken(func_id) ) {
                    err.position = cur_pos;
                    err.length = new_cur_pos - cur_pos;
                    err.code = GNVR_USE_BAD_ALG;
                    return Kumir::Undefined;
                }

                if ( m_modulesPool->functionTable(module_id)->typeById(func_id)==Kumir::Undefined ) {
                    err.position = cur_pos;
                    err.length = new_cur_pos - cur_pos;
                    err.code = GNVR_ALG_INSTEAD_OF_FUNCTION;
                    return Kumir::Undefined;
                }

                if ( not_flag )
                {
                    if ( m_modulesPool->functionTable(module_id)->typeById(func_id)!=Kumir::Boolean )
                    {
                        QPoint pnt = translateErrorPosition(not_pos,not_pos+1,P,L);
                        err.position = pnt.x();
                        err.length = pnt.y();
                        err.code = GNVR_NOT_LOG;
                        return Kumir::Undefined;
                    };
                    out_put_string=out_put_string+KS_NOT;
                    OP << not_pos;
                    OL << 1;
                };
                QString out_append = "@"+QString::number ( func_id )+"|"+QString::number ( module_id) +oper;
                for ( int x=0; x<out_append.length()-1; x++ ) {
                    OP << cur_pos;
                    OL << new_cur_pos - cur_pos;
                }
                OP << new_cur_pos;
                OL << 1;
                out_put_string=out_put_string+out_append;
            };
            break;

            //          F2. SimpeVar or Const or 0-ary Function
        case Sim_perem:
            {
                int func_id=-1;
                int module_id=-1;

                QPair<int,int> funcRef = m_modulesPool->funcByName(work_block.simplified());
                module_id = funcRef.first;
                func_id = funcRef.second;

                if ( func_id==-1 )
                {
                    //      F2.2. Not funct => check for Var local?
                    int perem_id=symbols.inTable ( work_block.simplified(),algorhitm);

                    //   global?
                    if ( perem_id<0 )
                        perem_id = symbols.inTable ( work_block.simplified(),"global");

                    if ( perem_id<0 && work_block.trimmed()==QObject::trUtf8("знач") ) {
                        perem_id = symbols.inTable ( algorhitm, "global" );

                    }

                    Kumir::ValueType b_type;
                    if ( perem_id > -1 )
                    {
                        b_type= symbols.getTypeByID ( perem_id );
                    }
                    else
                    {
                        b_type=Kumir::Undefined;
                        int le;
                        if ( work_block.count("\"") > 2 ) {
                            err.code = GNVR_MANY_QOUTES_IN_BLOCK;
                            return Kumir::Undefined;
                        }
                        b_type=testConst ( work_block,&le );
                        if ( le > 0 )
                        {
                            err.code = le;
                            if ( expf ) {
                                cur_pos = expression.lastIndexOf(expression,cur_pos);
                                cur_pos = cur_pos==-1? 0 : cur_pos;
                            }
                            QPoint pnt = translateErrorPosition(cur_pos,new_cur_pos,P,L);
                            err.position = pnt.x();
                            err.length = pnt.y();
                            return Kumir::Undefined;
                        }

                        if ( ( perem_id==-1 ) && ( b_type==Kumir::Undefined ) && ( !work_block.isEmpty() ) )
                        {
                            if ( work_block.trimmed()==QObject::trUtf8("знач") ) {
                                QPoint pnt = translateErrorPosition(cur_pos,new_cur_pos,P,L);
                                err.position = pnt.x();
                                err.length = pnt.y();
                                err.code = GNVR_PROCEDURE_RETVAL_ACCESS;
                                return Kumir::Undefined;
                            }
                            int tn_start = 0, tn_len = 0;
                            int tn = m_textNormalizer->test_name ( work_block, tn_start, tn_len );
                            if ( tn > 0 )
                            {
                                QPoint pnt = translateErrorPosition(cur_pos+tn_start, cur_pos+tn_start+tn_len,P,L);
                                err.position = pnt.x();
                                err.length = pnt.y();
                                err.code = tn;

                                return Kumir::Undefined;
                            }
                            QPoint pnt = translateErrorPosition(cur_pos,new_cur_pos,P,L);
                            err.position = pnt.x();
                            err.length = pnt.y();
                            if ( work_block.trimmed()==QObject::trUtf8("знач") )
                                err.code = GNVR_PROCEDURE_RETVAL_ACCESS;
                            else
                                err.code = GNVR_NO_IMPL_2;
                            return Kumir::Undefined;
                        }
                        else
                        {
                            //      F2.4. CONST !!!!!
                            //   add line to the symbol functions (including the value)
                            symbols.add_symbol
                                    ( "!KUM_CONST",-1,b_type,algorhitm ,true );
                            symbols.new_symbol ( symbols.count );
                            perem_id=symbols.count;
                            QString const_value = work_block.simplified();
                            set_const_value ( perem_id,const_value,symbols );
                            symbols.setUsed ( perem_id, true );
                        }
                    }
                    //      F2.5. Hurra! Correct Var or Const
                    //  (perem_id - ref to symbol functions)

                    //  a) work NOT (check if Kumir::Boolean later)
                    //  b) append to out_put_string
                    if ( not_flag )
                    {
                        if ( b_type !=Kumir::Boolean )
                        {
                            QPoint pnt = translateErrorPosition(not_pos,not_pos+1,P,L);
                            err.position = pnt.x();
                            err.length = pnt.y();
                            err.code = GNVR_NOT_LOG_2;
                            return Kumir::Undefined;
                        }
                        else
                        {
                            out_put_string = out_put_string + KS_NOT;
                            OP << not_pos;
                            OL << 1;
                        }
                    }
                    if ( perem_id>-1 ) // Zachrm etot if?
                    {
                        QString out_append = "^"+QString::number ( perem_id ) +oper;
                        for ( int x=0; x<out_append.length()-1; x++ ) {
                            OP << cur_pos;
                            OL << new_cur_pos - cur_pos;
                        }
                        OP << new_cur_pos;
                        OL << 1;
                        out_put_string=out_put_string+out_append;
                    }

                } // end if(func_id==-1)
                else
                {
                    //      F2.6. 0-ary function
                    if ( m_modulesPool->functionTable(module_id)->argCountById ( func_id ) >0 )
                    {
                        //                         result.position =  cur_block_id;
                        //                         Err_end = cur_block_id + 1;
                        //                         Err_start_incl = -1;
                        //                         Err_end_incl = -1;
                        //                      *err_pos = QPair<int,int> ( cur_block_id,
                        //                                                 cur_block_id + 1,
                        //                                                 -1,-1 );
                        QPoint pnt = translateErrorPosition(cur_pos, new_cur_pos, P, L);
                        err.position = pnt.x();
                        err.length = pnt.y();

                        err.code = GNVR_NO_ARG;
                        //                      *err_block = cur_block_id;
                        return Kumir::Undefined;
                    };
                    if ( m_modulesPool->functionTable(module_id)->typeById(func_id)==Kumir::Undefined ) {
                        //                      *err_pos = QPair<int,int> ( cur_block_id,
                        //                                                                           cur_block_id + 1,
                        //                                      -1,-1 );
                        QPoint pnt = translateErrorPosition(cur_pos, new_cur_pos, P, L);
                        err.position = pnt.x();
                        err.length = pnt.y();
                        err.code = GNVR_ALG_INSTEAD_OF_FUNCTION;
                        //                      *err_block = cur_block_id;
                        return Kumir::Undefined;
                    }
                    if ( not_flag )
                    {
                        if ( m_modulesPool->functionTable(module_id)->typeById(func_id)!=Kumir::Boolean )
                        {
                            QPoint pnt = translateErrorPosition(not_pos,not_pos+1,P,L);
                            err.position = pnt.x();
                            err.length = pnt.y();
                            err.code = GNVR_NOT_LOG_3;
                            //                          *err_block = cur_block_id;
                            return Kumir::Undefined;
                        };

                        out_put_string=out_put_string+QChar ( KS_NOT );
                        //                        OP << -1;
                        //                        OL << 1;
                        OP << not_pos;
                        OL << 1;
                    };
                    QString out_append = "@"+QString::number ( func_id )+"|"+QString::number(module_id) +oper;
                    for ( int x=0; x<out_append.length()-1; x++ ) {
                        OP << cur_pos;
                        OL << new_cur_pos - cur_pos;
                    }
                    OP << new_cur_pos;
                    OL << 1;
                    out_put_string=out_put_string+out_append;

                } // end else for if(func_id==-1)
            }
            break;
            //                  F.3. Array or String
        case Mass:
            {
                //  is it a name?
                int perem_id=symbols.inTable ( work_block.simplified(),algorhitm );
                if ( perem_id<0 )
                    perem_id=symbols.inTable ( work_block.simplified(),"global");

                if ( perem_id<0 && work_block.simplified()==QObject::trUtf8("знач") ) {
                    perem_id = symbols.inTable ( algorhitm, "global" );

                }

                // maybe function?

                int func_id=-1;
                int module_id=-1;

                QPair<int,int> funcRef = m_modulesPool->funcByName(work_block.simplified());
                module_id = funcRef.first;
                func_id = funcRef.second;

                if (func_id!=-1) {
                    QPoint pnt = translateErrorPosition(new_cur_pos, new_cur_pos+1, P, L);
                    err.position = pnt.x();
                    err.length = pnt.y();
                    err.code = GNVR_BRACKET_MISMATCH;
                    return Kumir::Undefined;
                }


                if ( ( perem_id==-1 ) )
                {
                    QPoint pnt = translateErrorPosition(cur_pos, new_cur_pos, P, L);
                    err.position = pnt.x();
                    err.length = pnt.y();
                    err.code = GNVR_NO_IMPL_3;
                    return Kumir::Undefined;
                };

                // @@@-B2. For strings
                // Predlagaetsia sdelat' raznye priznaki dlia stroki i massiva
                // % - dlia massiva;
                // | - dlia stroki
                // Eto potom ispolzovat' pri kontrole indeksov

                //   Etot kusok ne otlazhivalsia

                if ( symbols.getTypeByID ( perem_id ) ==Kumir::String )
                {
                    if ( not_flag ) {
                        out_put_string=out_put_string+QChar ( KS_NOT );
                        OP << not_pos;
                        OL << 1;
                    }
                    QString out_append = QString ( QChar ( TYPE_SKOBKA ) ) +
                                         QString::number ( perem_id ) +
                                         oper;
                    out_put_string = out_put_string + out_append;
                    for ( int x=0; x<out_append.length()-1; x++ ) {
                        OP << cur_pos;
                        OL << new_cur_pos - cur_pos;
                    }
                    OP << new_cur_pos;
                    OL << 1;
                }
                else
                {
                    if ( ( symbols.getTypeByID ( perem_id ) ==Kumir::IntegerArray ) ||
                         ( symbols.getTypeByID ( perem_id ) ==Kumir::CharectArray ) ||
                         ( symbols.getTypeByID ( perem_id ) ==Kumir::StringArray ) ||
                         ( symbols.getTypeByID ( perem_id ) ==Kumir::BooleanArray ) ||
                         ( symbols.getTypeByID ( perem_id ) ==Kumir::RealArray ) )
                    {
                        if ( not_flag ) {
                            out_put_string=out_put_string+QChar ( KS_NOT );
                            //                            OP << -1;
                            //                            OL << 1;
                            OP << not_pos;
                            OL << 1;
                        }
                        QString out_append = "%"+QString::number ( perem_id ) +oper;
                        for ( int x=0; x<out_append.length()-1; x++ ) {
                            OP << cur_pos;
                            OL << new_cur_pos - cur_pos;
                        }
                        OP << new_cur_pos;
                        OL << 1;
                        out_put_string=out_put_string+out_append;
                    }
                    else
                    {
                        QPoint pnt = translateErrorPosition(cur_pos, new_cur_pos, P, L);
                        err.position = pnt.x();
                        err.length = pnt.y();
                        err.code = GNVR_CANT_INDEX;
                        return Kumir::Undefined;
                    }
                }
            } // end case Mass
            break;

            //              F.5. Empty block
        case None:
            {
                //      NOT pered skobkoi
                if ( not_flag ) {
                    out_put_string=out_put_string+QChar ( KS_NOT );
                    OP << not_pos;
                    OL << 1;
                }
                OP << new_cur_pos;
                OL << 1;
                out_put_string=out_put_string+oper;
            }
            break;

        } // end switch (cur_block_type)

        cur_pos=new_cur_pos+1;
        last_oper = oper;
    } // end while (cur_pos<expression.length())

    // ==== NEW 2007-07-10 ====
    // Проверка на мусор в конце выражения.
    // Гипотеза: последним оператором после разбора может быть
    //      а) " " -- ничего;
    //      б) ")" -- закр. скобка функции;
    //      в) "]" -- закр. скобка массива.
    // Всё остальное -- лишнее, и является ошибкой "Знак в конце выражения"
    if (
            ( last_oper == ' ' )
            ||
            ( last_oper == ')' )
            ||
            ( last_oper == ']' )
            )
    {
        // OK.
    }
    else
    {
        // Ошибка.
        QString compound_sign = QString(QChar(KS_NOT))+
                                QString(QChar(KS_OR))+
                                QString(QChar(KS_XOR))+
                                QString(QChar(KS_AND));
        if ( compound_sign.indexOf(last_oper) != -1 )
            err.code = GSV_ZNAK_V_KONCE_2;
        else
            err.code = GSV_ZNAK_V_KONCE;
        // Выделяем ошибку
        //      int voidParam;
        //      int a;
        //      a = FindNomerNorm ( out_put_string,out_put_string.length()-1,1,&voidParam );
        //      *err_pos = QPair<int,int> ( a,a,1,1 );
        QPoint pnt = translateErrorPosition(cur_pos-1,cur_pos,P,L);
        err.position = pnt.x();
        err.length = pnt.y();
        return Kumir::Undefined;
    }


    //========================================================================
    //                          G.PART 2
    //      Vydelenie podvyrazjenii (ili ih spiskov) v skobkah.
    //  Vhod: DN-representation (vse imena zameneny na ssylki <char><nomer>
    //  Vyhod: D-predstavlenie (spisok podvyrazhenii)
    //========================================================================
    // Vydelenie podvyrazjenii (ili ih spiskov) v skobkah.
    // Formirovanie (a) QStringList *parsedExpression (podvyrazheniya) i
    //              (b) QList<Kumir::ValueType> int_types; - ih tipy
    // Zamena podvyrazhenii na &<Kumir::Integer>,
    //      gde <Kumir::Integer> - nomer v parsedExpression
    //
    //--------------------------------------------------------------------
    //              G.1. Data initialization
    //              -------------------------
    // dobit' probely - mogli poiavitsya posle NOT

    //  out_put_string=out_put_string.trimmed();


    //  qDebug() << "OP: " << OP;
    //  qDebug() << "OL: " << OL;

    m_textNormalizer->trim(out_put_string,OP,OL);

    //  qDebug() << "OP: " << OP;
    //  qDebug() << "OL: " << OL;


    Q_ASSERT ( out_put_string.length() == OP.count() ) ;
    Q_ASSERT ( OL.count() == OP.count() ) ;
    // vnutr tipy i struct tipy podvyrazhenii iz ParseVyraj //MR-New
    // stroiatsya parallelno s ParseVyraj
    QList<Kumir::ValueType>  int_types;
    QList<StructType> virStruct;    //MR-New

    int_types.clear();
    virStruct.clear();                  //MR-New

    // rabochaia stroka - see dropSpace
    QString string_cpy = dropSpace(out_put_string,OP,OL);

    //  qDebug() << "OP: " << OP;
    //  qDebug() << "OL: " << OL;

    Q_ASSERT ( string_cpy.length() == OP.count() ) ;
    Q_ASSERT ( OL.count() == OP.count() ) ;

    // vremennye perem to replace podvyrazheniya
    QString work_str;
    // ili spiski podvyrazhenii pri vyzove functii, v mnogom. massivah i vyrezkah
    QStringList arg_expression_List;

    // pozitsii skobok i ih tipy
    //      KRUG-KVAD:  0,2 - 1,3;
    //      OPEN-CLOSE: 0,1 - 2,3
    //   QList<int> skobk_pos,skobk_type; - see A.Prolog
    //  skobk_pos.clear();
    //  skobk_type.clear();


    //      Priznaki '[' and ']' ?? Co-numbered with skobk_pos ??
    //  QList<bool> open_sk_is_mass,close_sk_is_mass;
    // nu tak...
    err.code = 0;

    //-------------------------------------------------------------------------------
    //              G.2. Cykl po podvyrazheniyam - iznutri naruzhu
    //              ----------------------------------------------
    //  Na ocherednom shage:
    //    1) berem ocherednuuiu paru sootvetsvennyh sosednih skobok;
    //    2) vydeliaem mezhdu nimi:
    //          2.1) SimpleVyraj ili
    //          2.2) gruppu SimpleVyraj, razdelennyh:
    //              a) zapiatymi (esli skobki - eto vyzov mnigo-param func
    //                            ili ssylka na element mnogomernogo massiva).
    //                  ili
    //              b) dvoetochiem (esli vyrezka)
    //    3) formiruem text vida {&<Kumir::Integer>};
    //       zdes'  <Kumir::Integer> - eto id v ParseVyraj
    //       Etot text - eto work_str (esli 1 vyr) ili arg_expression_List (esli > 1)
    //
    //    4) zamenyaem nashi skobki i to, chto bylo mezhdu nimi
    //       na work_str ili arg_expression_List (para skobok ushla !!!) i
    //       correctiroem skobk_pos, skobk_type
    //   ---------
    //      SimpleVyraj - eto Vyrazh, vnutri kotorogo net skobok
    //          string_cpy - eto stroka, s kotoroi my rabotaem
    //=================================================================================

    // NEW == Разбиение выражения на подвыражения без скобок

    // список подвыражений
    QStringList exprList = parsedExpression;

    Kumir::CompileError local_error;
    local_error.code = 0;


    QList< QList<int> > FP, FL;

    for ( int i=0; i<exprList.count(); i++ ) {
        FP << QList<int>();
        FL << QList<int>();
    }

    //  qDebug("=== expr: %s",expression.toUtf8().data());

    // разбиение на подвыражения
    F ( string_cpy, exprList, local_error, FP, FL );



    if ( local_error.code > 0 )
    {
        // Вычисляем позицию ошибки по таблице перевода out_put_string -> expression
        QPoint stage1 = translateErrorPosition(local_error.position,local_error.length,OP,OL);
        // Вычисляем позицию ошибки по таблице перевода expression -> in.expression [после dropSpace(expression)]
        QPoint stage2 = translateErrorPosition(stage1.x(), stage1.x()+stage1.y(), P, L);
        err.position = stage2.x();
        err.length = stage2.y();
        err.code = local_error.code;
        return Kumir::Undefined;
    }

    // список типов подвыражений
    QList<Kumir::ValueType> exprTypeList;
    // список видов подвыражений
    QList<StructType> exprStructList;
    QStringList parsedNodes;

    for ( int i=0; i<exprList.count(); i++ )
    {
        StructType st;
        Kumir::ValueType pt;
        bool include_bounds_in_error; // включение граничных символов (\b) в область ошибки
        // Определяем тип каждого выражения
        QString expr;

        pt = G ( exprList[i], functions, symbols,
                 exprTypeList, exprStructList, exprList,
                 st, local_error, include_bounds_in_error,
                 parsedNodes, expr);

        if ( local_error.code > 0 )
        {
            // Вычисляем позицию ошибки по таблице перевода exprList[i] -> out_put_string
            QPoint stage0 = translateErrorPosition(local_error.position,local_error.length, FP[i], FL[i]);
            if ( !include_bounds_in_error ) {
                QChar start = out_put_string[stage0.x()];
                QChar end = out_put_string[stage0.x()+stage0.y()-1];
                if ( start=='(' || start=='[' || start==',' ) {
                    stage0.setX(stage0.x()+1);
                    stage0.setY(stage0.y()-1);
                }
                if ( end==')' || end==']' || end==',' ) {
                    stage0.setY(stage0.y()-1);
                }
            }
            // Вычисляем позицию ошибки по таблице перевода out_put_string -> expression
            QPoint stage1 = translateErrorPosition(stage0.x(),stage0.x()+stage0.y(),OP,OL);
            // Вычисляем позицию ошибки по таблице перевода expression -> in.expression [после dropSpace(expression)]
            QPoint stage2 = translateErrorPosition(stage1.x(), stage1.x()+stage1.y(), P, L);
            err.position = stage2.x();
            err.length = stage2.y();
            err.code = local_error.code;
            return Kumir::Undefined;
        }

        parsedNodes << expr;

        exprTypeList.append ( pt );
        exprStructList.append ( st );
    }

    // Если в подвыражениях ошибок не обнаружено,
    // то удаляем из них пробелы

    for ( int i=0; i<exprList.count(); i++ )
    {
        exprList[i].remove ( ' ' );
    }

    // Выделяем подвыражения между логическими операторами в отдельные блоки
    int exprCount = exprList.count();

    for (int i=0; i<exprCount; i++) {
        exprList[i] = splitLogicalExpressions(exprList[i], extraSubExpressions);
    }

    parsedExpression = exprList;
    expressionType = exprStructList.last();
    Kumir::ValueType result = exprTypeList.last();
    if ( ( result == Kumir::BoolOrDecimal ) || ( result == Kumir::BoolOrLiteral ) )
        result = Kumir::Boolean;
    if (!parsedNodes.isEmpty()) {
        xmlRoot += parsedNodes.last();
    }

    return result;
}


struct Bracket {
    // Пустой конструктор (требуется синтаксисом языка C++)
    Bracket() { t = ' '; p1 = -1; p2 = -1; ot = ' '; }
    // Конструктор открывающей скобки
    Bracket(QChar a, int b) { t = a; p1 = b; p2 = b; ot = a; }
    // Конструктор внутренней скобки ( "," или ":" )
    Bracket(QChar a, int b, QChar c, int d) { t = a; p1 = b; ot = c; p2 = d; }
    QChar t; // тип скобки
    QChar ot; // тип открывающей скобки (если t == ',' или ':' )
    int p1; // позиция скобки
    int p2; // позиция соотв. открыв. скобки
};

void F_processSubExpr(const QString &s, const int start, QList< QList<int> > &P, QList< QList<int> > &L)
{
    QList<int> p, l;
    int cur, prev;
    prev = -1;
    int shift = 0;
    forever {
        cur = s.indexOf("&",prev+1);
        if ( cur == -1 ) {
            for ( int i=prev+1; i<s.length(); i++ ) {
                p << i+start+shift;
                l << 1;
            }
            break;
        }
        else {
            for ( int i=prev+1; i<cur; i++ ) {
                p << i+start+shift;
                l << 1;
            }
            QString link;
            prev = s.length()-1;
            for ( int i=cur+1; i<s.length(); i++ ) {
                if ( s[i].isDigit() )
                    link += s[i];
                else {
                    prev = i-1;
                    break;
                }
            }
            int lnk = link.toInt();
            int left = P[lnk][0];
            int right = 0;
            for ( int i=0; i<P[lnk].count(); i++ ) {
                left = qMin(left, P[lnk][i]);
                right = qMax(right, P[lnk][i]+L[lnk][i]);
            }
            int len = right - left;
            bool isMass = false;
            if ( cur >= 0 ) {
                for ( int i=cur-1; i>=0; i-- ) {
                    if ( s[i]=='%' || s[i]==QChar(TYPE_SKOBKA) )
                        isMass = true;
                    if ( !s[i].isDigit() && !(s[i]=='&')  )
                        break;
                }
            }
            bool isFirstArg = true;
            if ( cur >= 0 ) {
                for ( int i=cur-1; i>=0; i-- ) {
                    if ( s[i]=='&' )
                        isFirstArg = false;
                    if ( !s[i].isDigit() )
                        break;
                }
            }
            if ( isMass ) {
                shift += isFirstArg? 2 : 1;
            }
            for ( int i=0; i<link.length()+1; i++ ) {
                p << left;
                l << len;
            }
            shift += len;

            shift -= 1+link.length();
        }
    }
    P << p;
    L << l;
}

/**
 * Разбивает выражение со скобками на простые подвыражения
 * @param s Исходное выражение
 * @param *EL Ссылка на список подвыражений
 * @param *BL Ссылка на список границ подвыражений
 * @param *err Ссылка на код ошибки
 * @param *err_start Ссылка на позицию начала ошибочной области
 * @param *err_end Ссылка на позицию конца ошибочной области
 */
void TextAnalizer::F(const QString &s,
                     QStringList &EL,
                     Kumir::CompileError &err,
                     QList< QList<int> > &P,
                     QList< QList<int> > &L
                     )
{
    QRegExp rxDelim = QRegExp("[,:\\[(]");
    QString mainExpr;
    QList<int> mainP, mainL;
    // Стек найденных открывающих скобок
    QStack<Bracket> stack;
    err.code = 0;
    err.length = 1;
    for ( int i=0; i<s.length(); i++ )
    {
        // Открывающая круглая скобка
        if ( s[i] == '(' )
        {
            stack.push(Bracket('(',i));
        }
        else if ( s[i] == '[' )
        {
            stack.push(Bracket('[',i));
        }
        else if ( s[i] == ')' )
        {
            if ( stack.isEmpty() )
            {
                err.code = FIS_1; // нет "(" для ")"
                err.position = i;
                return;
            }
            if ( stack.last().t == '[' )
            {
                err.code = FIS_2;  // ")" вместо "]"
                err.position = i;
                return;
            }
            Bracket last = stack.pop();
            int delPos = rxDelim.lastIndexIn(mainExpr);
            QString subExpr = mainExpr.mid(delPos+1);
            if ( subExpr.trimmed().isEmpty() )
            {
                err.code = FIS_3; // пусто между скобками
                err.position = last.p1;
                err.length = i - last.p1;
                return;
            }
            if ( last.t == ',' )
            {
                if ( last.ot == '[' )
                {
                    err.code = FIS_10;  // ")" вместо "]"
                    err.position = i;
                    return;
                }
            }
            if ( last.t == ':' )
            {
                err.code = FIS_11; // здесь не нужно ":"
                err.position = i;
                return;
            }
            while ( mainExpr.length()>0 )
            {
                bool stop = mainExpr.endsWith(last.t);
                mainP.pop_back();
                mainL.pop_back();
                mainExpr.chop(1);
                if ( stop )
                    break;
            }
            QString repl = "&"+QString::number(EL.count());
            mainExpr += repl;

            subExpr = " "+subExpr+" ";

            for ( int j=0; j<repl.length(); j++ ) {
                mainP << last.p1;
                mainL << i-last.p1;
            }

            F_processSubExpr(subExpr,last.p1,P,L);
            EL << subExpr;

            continue;
        }
        else if ( s[i] == ']' )
        {
            if ( stack.isEmpty() )
            {
                err.code = FIS_4; // нет "[" для "]"
                err.position = i;
                return;
            }
            if ( stack.last().t == '(' )
            {
                err.code = FIS_5;  // "]" вместо ")"
                err.position = i;
                return;
            }
            Bracket last = stack.pop();

            if ( last.t == ',' )
            {
                if ( last.ot == '(' )
                {
                    err.code = FIS_21;  // "]" вместо ")"
                    err.position = i;
                    return;
                }
            }
            int delPos = rxDelim.lastIndexIn(mainExpr);
            QString subExpr = mainExpr.mid(delPos+1);
            if ( subExpr.trimmed().isEmpty() )
            {
                if ( last.t==':' || last.t==',' )
                    err.code = FIS_14;
                else
                    err.code = FIS_6; // пусто между скобками
                err.position = last.p1;
                err.length = i - last.p1;
                return;
            }
            while ( mainExpr.length()>0 )
            {
                bool stop = mainExpr.endsWith(last.t);
                mainP.pop_back();
                mainL.pop_back();
                mainExpr.chop(1);
                if ( stop )
                    break;
            }
            QString repl = "&"+QString::number(EL.count());
            mainExpr += repl;


            for ( int j=0; j<repl.length(); j++ ) {
                mainP << last.p1+1;
                mainL << i-last.p1-1;
            }

            F_processSubExpr(subExpr, last.p1+1, P, L);
            EL << subExpr;
            continue;
        }
        else if ( s[i] == ',' )
        {
            if ( stack.isEmpty() )
            {
                err.code = FIS_7; // лишняя запятая
                err.position = i;
                return;
            }
            Bracket open = stack.last();
            if ( ( open.ot != '(' ) && ( open.ot != '[' ) )
            {
                err.code = FIS_8; // лишняя запятая
                return;
            }

            if ( open.ot == '[' )
            {
                // Проверка типа -- должен быть массив, иначе -- ошибка
                int block_start = open.p2;
                while ( ( s[block_start] != '^' ) && ( s[block_start] != '%' ) && ( s[block_start] != QChar(TYPE_SKOBKA) )
                    && ( block_start > 0 ) )
                    block_start--;

                if ( s[block_start] != '%' )
                {
                    err.code = FIS_20; // "," вместо ":"
                    err.position = i;
                    return;
                }
            }

            Bracket last = stack.pop();
            int delPos = rxDelim.lastIndexIn(mainExpr);
            QString subExpr = mainExpr.mid(delPos+1);
            if ( subExpr.trimmed().isEmpty() )
            {
                if ( s[i]==',' ) {
                    err.code = FIS_8;
                }
                else {
                    err.code = FIS_9; // пусто между скобками
                    err.position = last.p1;
                    err.length = i - last.p1;
                }
                return;
            }
            while ( mainExpr.length()>0 )
            {
                bool stop = mainExpr.endsWith(last.t);
                mainP.pop_back();
                mainL.pop_back();
                mainExpr.chop(1);
                if ( stop )
                    break;
            }
            QString repl = "&"+QString::number(EL.count());
            mainExpr += repl;
            for ( int j=0; j<repl.length(); j++ ) {
                mainP << last.p1+1;
                mainL << i-last.p1-1;
            }

            F_processSubExpr(subExpr,last.p1+1,P,L);
            EL << subExpr;

            stack.push(Bracket(',',i,open.ot,open.p2));
        }
        else if ( s[i] == ':' )
        {
            if ( stack.isEmpty() )
            {
                err.code = FIS_12; // лишнее ":"
                err.position = i;
                return;
            }
            Bracket open = stack.last();
            if ( ( open.ot != '(' ) && ( open.ot != '[' ) )
            {
                err.code = FIS_13; // лишнее ":"
                err.position = i;
                return;
            }

            // Проверка типа -- должен быть string, иначе -- ошибка
            int block_start = open.p2;
            while ( ( s[block_start] != '^' ) && ( s[block_start] != '%' ) && ( s[block_start] != QChar(TYPE_SKOBKA) )
                && ( block_start > 0 ) )
                block_start--;

            if ( s[block_start] != QChar(TYPE_SKOBKA) )
            {
                err.code = FIS_19; // Вырезка не из строки
                err.position = i;
                return;
            }


            Bracket last = stack.pop();
            int delPos = rxDelim.lastIndexIn(mainExpr);
            QString subExpr = mainExpr.mid(delPos+1);
            if ( subExpr.trimmed().isEmpty() )
            {
                err.code = FIS_14; // нет границы
                err.position = last.p1;
                err.length = i - last.p1;
                return;
            }
            while ( mainExpr.length()>0 )
            {
                bool stop = mainExpr.endsWith(last.t);
                mainP.pop_back();
                mainL.pop_back();
                mainExpr.chop(1);
                if ( stop )
                    break;
            }
            QString repl = "&"+QString::number(EL.count());
            mainExpr += repl;



            for ( int j=0; j<repl.length(); j++ ) {
                mainP << last.p1+1;
                mainL << i-last.p1;
            }

            F_processSubExpr(subExpr,last.p1+1,P,L);
            EL << subExpr;

            stack.push(Bracket(':',i,open.ot,open.p2));
        }
        mainExpr += s[i];
        mainP << i;
        mainL << 1;
    }
    // 	s.remove(' ');
    if ( !stack.isEmpty() )
    {
        if ( stack.last().t == '[' )
        {
            err.code = FIS_15;
            err.position = stack.last().p1;
            return;
        }
        if ( stack.last().t == '(' )
        {
            err.code = FIS_16;
            err.position = stack.last().p1;
            return;
        }
        if ( ( stack.last().t == ',' ) || ( stack.last().t == ':' ) )
        {
            if ( stack.last().ot == '[' )
            {
                err.code = FIS_17;
                err.position = stack.last().p2;
                return;
            }
            if ( stack.last().ot == '(' )
            {
                err.code = FIS_18;
                err.position = stack.last().p2;
                return;
            }
        }

    }
    EL << mainExpr;
    F_processSubExpr(mainExpr,0,P,L);
}


Kumir::CompileError TextAnalizer::parceAlgorhitmCall(KumirInstruction &pv,
                                                     FunctionTable &table,
                                                     SymbolTable &symbols,
                                                     int str,
                                                     QString &xml)
{
    Kumir::CompileError result;
    result.code = 0;
    QString func = pv.line;
    pv.VirajTypes.clear();
    pv.VirajList.clear();

    //     func=func.trimmed();

    if ( func.startsWith(KS_FOR) )
    {
        result.position = 0;
        result.length = 1;
        result.code = FUNC_STARTS_WITH_FOR;
        return result;
    }

    if ( func.startsWith(KS_WHILE) )
    {
        result.position = 0;
        result.length = 1;
        result.code = FUNC_STARTS_WITH_WHILE;
        return result;
    }

    QString name,args;
    int func_id=-1;
    int skobk_o=func.indexOf("(");
    //int skobk_z;skobk_o=func.indexOf(

    //  if((skobk_z<func.length()-1)&&(skobk_z>-1))return 57;

    // проверка на ошибочное присваивание
    int equalSignPos = func.indexOf('=');
    if ( equalSignPos != -1 && (equalSignPos < skobk_o || skobk_o==-1) ) {
        result.position = equalSignPos;
        result.length = 1;
        result.code = FUNC_EQUAL_SIGN_IN_NAME;
        return result;
    }


    if(skobk_o==-1)
    {
        // Net '(' => net argumentov
        name=func;
        int func_id=-1;
        int module_id=-1;

        QPair<int,int> funcRef = m_modulesPool->funcByName(name.simplified());
        module_id = funcRef.first;
        func_id = funcRef.second;

        if(func_id==-1)
        {
            int tn_start = 0, tn_len = 0;
            int test = m_textNormalizer->test_name(name,tn_start,tn_len);
            if(test!=0)
            {
                result.position = tn_start;
                result.length = tn_start;
                result.code = test;
                return result;
            }
            else
            {
                result.position = 0;
                result.length = name.length();
                result.code = VYZ_NO_IMPL_1;
                return result;
            }
        }

        if ( m_modulesPool->functionTable(module_id)->isBroken(func_id) ) {
            result.position = 0;
            result.length = name.length();
            result.code = VYZ_BAD_ALG;
            return result;

        }
        if ( (m_modulesPool->functionTable(module_id)->typeById(func_id)) != Kumir::Undefined )
        {
            result.position = 0;
            result.length = name.length();
            result.code = VYZ_NO_PROC_1;
            return result;
        }
        //        if (xml!=NULL) {
        //            *xml = "<call module='"+modules->module(module_id)->name+"' algorhitm='"+m_modulesPool->functionTable(module_id)->nameById(func_id)+"'/>\n";
        //        }
        // NEW V.Y. 21.08.2006
        if ( m_modulesPool->functionTable(module_id)->argCountById(func_id) > 0 )
        {
            result.position = 0;
            result.length = name.length();
            result.code = VYZ_NO_ARGS;
            return result;
        }

        pv.line=QString::number(func_id)+"|"+QString::number(module_id);//09-01-Mordol
        return result;
    }
    else
    {
        args=func.mid(skobk_o);
        name=func.left(skobk_o);
        //      name = name.trimmed();
        //  outPutWindow->append("FUNC:"+name+"|");
        // outPutWindow->append("args:"+args+"|");
    };
    if ( name.trimmed().isEmpty() ) {
        result.position = 0;
        result.length = 1;
        result.code = VYZ_NO_ALG_NAME;
        return result;
    }
    func_id=-1;
    int module_id=-1;
    QPair<int,int> funcRef = m_modulesPool->funcByName(name.simplified());
    module_id = funcRef.first;
    func_id = funcRef.second;
    if(func_id==-1)
    {
        int test;
        int tn_start = 0, tn_len = 0;
        test = m_textNormalizer->test_name(name,tn_start,tn_len);
        if(test!=0)
        {
            result.position = tn_start;
            result.length = tn_len;
            result.code = VYZ_BAD_SYM_2;
            return result;
        }
        else
        {
            result.position = 0;
            result.length = skobk_o;
            result.code = VYZ_NO_IMPL_2;
            return result;
        }
    }
    if ( m_modulesPool->functionTable(module_id)->isBroken(func_id) ) {
        result.position =  0;
        result.length =  skobk_o;
        result.code = VYZ_BAD_ALG;
        return result;
    }
    // NEW V.Y. -- check for ret type

    if ( (m_modulesPool->functionTable(module_id)->typeById(func_id)) != Kumir::Undefined )
    {
        result.position =  0;
        result.length =  skobk_o;
        result.code = VYZ_NO_PROC_2;
        return result;
    }

    xml = "<call module='"+m_modulesPool->moduleName(module_id)+"' algorhitm='"+m_modulesPool->functionTable(module_id)->nameById(func_id)+"'>\n";


    QRegExp rxEmptyArgs = QRegExp("\\((\\s*)\\)");

    if ( rxEmptyArgs.exactMatch(args) )
    {
        result.position =  0;
        result.length =  skobk_o;
        result.code = VYZ_EMPTY_BRAKCETS;
        return result;
    }

    // moved here from below
    int s_deep = 0;
    int s_open_pos = 0;
    int s_close_pos = 0;
    QChar cChar;
    for ( int i=0; i<args.length(); i++) {
        cChar = args[i];
        if (cChar=='(') {
            s_deep++;
            s_open_pos = i;
        }
        if (cChar==')') {
            s_deep--;
            s_close_pos = i;
        }
    }
    if (s_deep>0) {
        result.position =  s_open_pos+skobk_o;
        result.length =  1;
        result.code = VYZ_BAD_LAST;
        return result;
    }
    if ((s_close_pos+1)<args.length()) {
        bool spacesOnly = true;
        QChar current;
        bool currentSpace;
        for (int i=s_close_pos+1; i<args.length(); i++) {
            current = args[i];
            currentSpace = (current==' ') || (current=='\t');
            spacesOnly = spacesOnly && currentSpace;
        }
        if (!spacesOnly) {
            result.position =  s_close_pos+skobk_o+1;
            result.length =  args.length()-s_close_pos-1;
            result.code = VYZ_TRASH;
            return result;
        }
    }



    QStringList args_viraj;

    QString viraj;
    int deep=0;

    int open_pos = 0;
    int close_pos = 0;

    int func_length = func.length();

    while ( (open_pos!=-1) && (open_pos<func_length) )
    {
        open_pos = func.indexOf('(',open_pos);
        close_pos = func.indexOf(')',close_pos);
        if ( ( close_pos < open_pos ) && ( close_pos != -1 ) )
        {
            result.position =  close_pos;
            result.length =  1;
            result.code = VYZ_LISH_ZAKR;
            return result;
        }
        if ( ( close_pos != -1 ) && ( open_pos == -1 ) )
        {
            result.position =  close_pos;
            result.length =  1;
            result.code = VYZ_LISH_ZAKR;
            return result;
        }
        if ( ( close_pos == -1 ) && ( open_pos != -1 ) )
        {
            result.position =  open_pos;
            result.length =  1;
            result.code = VYZ_LISH_OTKR;
            return result;
        }
        if ( open_pos != -1 )
            open_pos++;
        if ( close_pos != -1 )
            close_pos++;
    }

    if ( func.count("(")>1 )
    {
        if ( func.count(")") != func.count("(") )
        {
            int err_pos = func.indexOf(')');
            err_pos = func.indexOf('(',err_pos); // next ( after )

            result.position =  err_pos;
            result.length =  1;
            result.code = VYZ_LISH_OTKR;
            return result;
        }
    }

    // END NEW

    args = args.trimmed();

    QList<int> leftBounds;

    for(int i=1;i<args.length();i++) //Разделяем выражения
    {
        // NEW V.Y. -- for no args
        // RegExp matches ( any_count_of_spaces )
        QRegExp rxNoArgs (QString("\\([\\s]*\\)"));
        if ( rxNoArgs.exactMatch(args) )
            break;
        // END
        if((args[i]=='[')||(args[i]=='('))
            deep++;
        if((args[i]==']')||(args[i]==')'))
            deep--;
        if(((deep==0)&&(args[i]==','))||(i==args.length()-1))
        {

            args_viraj.append(viraj);
            int shft = i+skobk_o-viraj.length();
            leftBounds << shft;
            //outPutWindow->append("viraj:"+viraj+"|");
            viraj="";
            continue;
        };
        viraj+=args[i];
        if(deep<0)
        {
            int err_pos = func.indexOf('(');
            int close_pos;
            int last_pos;
            bool isClosed = false;
            int func_length = func.length();
            while ( (err_pos<func_length) && (err_pos!=-1) )
            {
                close_pos = func.indexOf(')',err_pos+1);
                if ( close_pos != -1 )
                    isClosed = true;
                last_pos = err_pos;
                err_pos = func.indexOf('(',err_pos+1);
            }
            if ( ( last_pos != -1 ) && ( !isClosed ) )
            {
                result.position =  last_pos;
                result.length =  1;
                result.code = VYZ_LISH_OTKR;
                return result;
            }
        }
    } // end for (i)

    if ( args_viraj.count() != m_modulesPool->functionTable(module_id)->argCountById(func_id) ) {
        result.position =  skobk_o+1;
        int Err_end = func.lastIndexOf(')');
        result.length =  Err_end - result.position;
        result.code = VYZ_PARAM_COUNT_MISMATCH;
        return result;
    }

    if(deep>0)
    {
        int err_pos = func.indexOf('(');
        int close_pos;
        int last_pos;
        bool isClosed = false;
        int fl = func.length();
        while ( (err_pos<fl) && (err_pos!=-1) )
        {
            close_pos = func.indexOf(')',err_pos+1);
            if ( close_pos != -1 )
                isClosed = true;
            last_pos = err_pos;
            err_pos = func.indexOf('(',err_pos+1);
        }
        if ( ( last_pos != -1 ) && ( !isClosed ) )
        {
            result.position =  last_pos;
            result.length =  1;
            result.code = VYZ_LISH_OTKR;
            return result;
        }
    }

    int av_count = args_viraj.count();

    if(av_count!=m_modulesPool->functionTable(module_id)->argCountById(func_id))
    {
        result.position =  func.indexOf('(');
        int end = func.lastIndexOf(')');
        result.length =  end - result.position;
        result.code = VYZ_BAD_ARG_NUM;
        return result;
    }

    QStringList parsedViraj;
    QStringList extraSubExpressions;
    QString callExpr;
    callExpr = "@"+QString::number(func_id);

    foreach (QString arg, args_viraj) {
        if ( arg.trimmed().isEmpty() ) {
            result.position =  func.indexOf('(');
            int end = func.lastIndexOf(')');
            result.length =  end -result.position;
            result.code = VYZ_EMPTY_ARG;
            return result;
        }
    }

    //    if ( name==QObject::trUtf8("установить цвет") ) {
    //        if (modules->module(module_id)->isInternal()) {
    //            if ( args_viraj.count() > 0 ) {
    //                QString arg = args_viraj[0];
    //                int arglen = arg.length();
    //                if ( arg.trimmed().startsWith("\"") && arg.trimmed().endsWith("\"") && arg.count("\"")==2) {
    //                    arg.remove("\"");
    //                    for ( int i=0; i<arg.count(); i++ )
    //                        arg[i] = QChar(arg[i].unicode()-STR_HIDE_OFFSET);
    //                    QRegExp validColorNames = QRegExp(QObject::trUtf8("черный|белый|красный|оранжевый|желтый|зеленый|голубой|синий|фиолетовый"));
    //                    if ( !validColorNames.exactMatch(arg.trimmed()) )
    //                    {
    //                        result.position =  leftBounds[0];
    //                        result.length =  arglen;
    //                        result.code = BAD_COLOR;
    return result;
    //                    }
    //                }
    //            }
    //        }
    //    }

    for(int i=0;i<args_viraj.count();i++)
    {
        result.position =  func.indexOf('(');
        result.length =  1;

        Kumir::CompileError local_error;

        StructType VirStruct;
        Kumir::ValueType intType =
                parceExpression(args_viraj[i],
                                table,
                                symbols,
                                table.nameByPos(str),
                                parsedViraj,
                                VirStruct,
                                local_error,
                                extraSubExpressions,
                                xml);


        Kumir::ValueType extType = m_modulesPool->functionTable(module_id)->argTypeById(func_id, i);
        callExpr += "&"+QString::number(parsedViraj.count()-1);

        bool allowTypeChange = !m_modulesPool->functionTable(module_id)->isRes(func_id, i)
                               && !m_modulesPool->functionTable(module_id)->isArgRes(func_id, i);

        if(local_error.code)
        {
            int sh = leftBounds[i];
            local_error.position += sh;
            return local_error;
        }
        if(extType!=intType)
        {
            if (!allowTypeChange||(!((intType==Kumir::Integer)&&(extType==Kumir::Real)) &&
                                   !((intType==Kumir::Charect)&&(extType==Kumir::String)) /*&&
                                                                        !((intType==Kumir::IntegerArray)&&(extType==Kumir::RealArray))*/
                )
                )
                {
                result.position =  leftBounds[i];
                result.length =  args_viraj[i].length();
                result.code = VYZ_TYPE_MISS_1;
                return result;
            }
        }

        if ( m_modulesPool->functionTable(module_id)->isRes(func_id, i) )
        {
            if (
                    ( VirStruct != Variable ) &&
                    ( VirStruct != Array )
                    )
            {
                result.position =  leftBounds[i];
                result.length =  args_viraj[i].length();
                result.code = VYZ_RES_NOT_SIMPLE;
                return result;
            }
        }
        if ( m_modulesPool->functionTable(module_id)->isArgRes(func_id, i) )
        {
            if (
                    ( VirStruct != Variable ) &&
                    ( VirStruct != Array )
                    )
            {
                result.position =  leftBounds[i];
                result.length =  args_viraj[i].length();
                result.code = VYZ_ARGRES_NOT_SIMPLE;
                return result;
            }
        }
        bool isRes = m_modulesPool->functionTable(module_id)->isRes(func_id, i);
        bool isArgRes = m_modulesPool->functionTable(module_id)->isArgRes(func_id, i);
        if (isRes||isArgRes)
        {
            // Имя алгоритма, из которого происходит вызов
            // (нужно для определения ID переменной)
            QString caller_f_name = table.nameByPos(str);
            args_viraj[i] = args_viraj[i].trimmed();
            int perem_id = symbols.inTable(args_viraj[i],caller_f_name);
            if ( perem_id == -1 )
            {
                perem_id = symbols.inTable(args_viraj[i],"global");
            }
            if ( symbols.symb_table[perem_id].is_Arg && !(symbols.symb_table[perem_id].is_Res||symbols.symb_table[perem_id].is_ArgRes)) {
                result.position =  leftBounds[i];
                result.length =  args_viraj[i].length();
                result.code = isRes? VYZ_ARG_AS_RES : VYZ_ARG_AS_ARGRES;
                return result;
            }
        }
        // Проверка на размерности массива
        if ( ( (intType==Kumir::IntegerArray ) || ( intType==Kumir::RealArray ) || ( intType==Kumir::BooleanArray ) || ( intType==Kumir::CharectArray ) || ( intType==Kumir::StringArray ) ) && ( VirStruct == Array) )
        {
            // Имя алгоритма, из которого происходит вызов
            // (нужно для определения ID переменной)
            QString caller_f_name = table.nameByPos(str);
            args_viraj[i] = args_viraj[i].trimmed();
            int perem_id = symbols.inTable(args_viraj[i],caller_f_name);
            if ( perem_id == -1 )
            {
                perem_id = symbols.inTable(args_viraj[i],"global");
            }
            // Размерность передаваемого массива
            int intRazm = symbols.symb_table[perem_id].razm;
            // Размерность аргумента функции
            int extRazm = m_modulesPool->functionTable(module_id)->argDimById(func_id, i);
            if ( intRazm != extRazm )
            {
                result.position =  leftBounds[i];
                result.length =  args_viraj[i].length();
                result.code = VYZ_MASS_BAD_RAZM;
                return result;

            }
        }
        // Конец проверки размерностей массива


        pv.VirajTypes.append(intType);

        //      qDebug() << *xml;
    }
    xml += "</call>\n";
    parsedViraj.append(callExpr);
    pv.VirajList.append(parsedViraj);
    pv.line=QString::number(func_id)+"|"+QString::number(module_id); // ??? 30.08
    pv.extraSubExpressions = extraSubExpressions;

    return result;
}


QStringList TextAnalizer::splitMassArgs(QString mass_args,QList<int> &starts,int *err)
{
    QStringList temp;
    QString viraj;
    temp.clear();
    int last_zpt_pos=0;
    int deep_sk=0;
    int deep_mass=0;
    for(int i=0;i<mass_args.length();i++)
    {
        if(mass_args[i]=='(')
            deep_sk++;
        if(mass_args[i]==')')
            deep_sk--;

        if(mass_args[i]=='[')
            deep_mass++;
        if(mass_args[i]==']')
            deep_mass--;

        if((deep_mass<0)||(deep_sk<0))
        {
            *err=SYNTAX_ERR;
            return temp;
        };
        if((deep_mass==0)&&(deep_sk==0)&&(mass_args[i]==','))
        {
            viraj=mass_args.mid(last_zpt_pos,i-last_zpt_pos);
            starts << last_zpt_pos;
            last_zpt_pos=i+1;
            temp.append(viraj);

        };

    };


    viraj=mass_args.mid(last_zpt_pos,mass_args.length()-last_zpt_pos);
    starts << last_zpt_pos;
    if(viraj[0]==',')
        viraj=viraj.mid(1);
    temp.append(viraj);



    return temp;

};



/**
* Возвращет тип переменной в массиве. Если не массив возвращеет тип исходной переменной
* @param inType тип массива
* @return тип переменной
 */
Kumir::ValueType TextAnalizer::retUnMassType(Kumir::ValueType inType)
{
    if((inType==Kumir::Integer)||(inType==Kumir::Charect)||(inType==Kumir::Real)||(inType==Kumir::Boolean))
    {
        return inType;
    };
    if((inType==Kumir::String))
        return Kumir::Charect;
    if((inType==Kumir::StringArray))
        return Kumir::String;
    if((inType==Kumir::IntegerArray))
        return Kumir::Integer;
    if((inType==Kumir::RealArray))
        return Kumir::Real;
    if((inType==Kumir::BooleanArray))
        return Kumir::Boolean;
    if((inType==Kumir::CharectArray))
        return Kumir::Charect;

    return Kumir::Undefined;

}



Kumir::CompileError TextAnalizer::parceAssignment(KumirInstruction &pv,
                                                  FunctionTable &table,
                                                  SymbolTable &symbols,
                                                  int str, QString &root)
{
    Kumir::CompileError result;
    result.code = 0;

    QString prisv = pv.line;

    // левая и правая части присваивания
    QString left, right;

    // разделяем выражение на две части
    int assignPos = prisv.indexOf(KS_ASSIGN);
    Q_ASSERT ( assignPos > -1 );
    left = prisv.left(assignPos);
    right = prisv.mid(assignPos+1);

    // левая часть должна быть непустой
    if ( left.simplified().isEmpty() ) {
        result.position =  assignPos;
        result.length =  1;
        result.code = PRISV_NO_LEFT;
        return result;
    }

    // правая часть должна быть непустой
    if ( right.simplified().isEmpty() ) {
        result.position =  assignPos;
        result.length =  1;
        result.code = PRISV_NO_RIGHT;
        return result;
    }

    // список подвыражений, учавствующих в присваивании
    QStringList exprList;

    // типы выражений слева и справа
    Kumir::ValueType leftType, rightType;
    // структура выражения слева
    StructType leftStructType;

    // выполняем разбор выражения слева
    Kumir::CompileError local_error;

    QPair<int,int> local_err_pos;
    QString prisvElem, leftElem, rightElem;


    prisvElem = "<assignment>\n";
    leftElem = "";
    rightElem = "";


    QString currentAlgorhitm = table.nameByPos(str);

    leftType = parceExpression(left,
                               table,
                               symbols,
                               currentAlgorhitm,
                               exprList,
                               leftStructType,
                               local_error,
                               pv.extraSubExpressions,
                               leftElem);
    if ( local_error.code ) {
        return local_error;
    }

    // выражение слева может быть простой переменной, элементом массива, элементом строки или вырезкой
    if ( leftStructType!=Variable && leftStructType!=ArrayElement && leftStructType!=StringElement && leftStructType!=Substring) {
        result.position =  0;
        result.length =  assignPos;
        switch ( leftStructType ) {
        case Constant:
            result.code = PRISV_CONST;
        case Array:
            result.code = PRISV_MASS;
        case Complex:
            result.code = PRISV_COMPLEX;
        default:
            result.code = PRISV_NONE;
        }
        return result;
    }

    if ( leftType==Kumir::IntegerArray || leftType==Kumir::RealArray || leftType==Kumir::CharectArray || leftType==Kumir::StringArray || leftType==Kumir::BooleanArray ) {
        result.position =  assignPos;
        result.length =  1;
        result.code = PRISV_LEFT_ARRAY;
        return result;
    }
    // Знач может быть только у функций, возвращающих значение
    if ( left.simplified()==QObject::trUtf8("знач") ) {
        int fID = table.idByPos(str);
        if ( fID==-1 || table.typeById(fID) == Kumir::Undefined )
        {
            result.position =  0;
            result.length =  assignPos;
            result.code = PRISV_EXTRA_RETURN;
            return result;
        }
        leftElem = "<return_value/>\n";
    }

    // Аргумент алгоритма по умолчанию является const, поэтому ему нельзя присваивать
    QString leftCode = exprList.last();
    QRegExp rxNumber("[0-9]+");
    int p = rxNumber.indexIn(leftCode);
    int variable_id = leftCode.mid(p,rxNumber.matchedLength()).toInt();
    if (symbols.symb_table[variable_id].is_Arg && !(symbols.symb_table[variable_id].is_Res||symbols.symb_table[variable_id].is_ArgRes)) {
        result.position =  0;
        result.length =  assignPos;
        result.code = PRISV_LEFT_IS_ARG;
        return result;
    }

    // результирующая строка
    // Имеет формат: &номер_выражения_слева_в_общем_списке{PRISV}&номер_выражения_справа_в_общем_списке
    // Пример: &2{PRISV}&7
    //   -- вычислить значение 7-го подвыражения в списке и записать его в переменную,
    //   -- которая вычисляется во 2-м подвыражении списка
    QString out;

    out = "&"+QString::number(exprList.count()-1) + QString(KS_ASSIGN);

    // выполняем разбор выражения справа

    leftType = parceExpression(right,
                               table,
                               symbols,
                               currentAlgorhitm,
                               exprList,
                               leftStructType,
                               local_error,
                               pv.extraSubExpressions,
                               rightElem);

    if ( local_error.code ) {
        local_error.position += assignPos + 1;
        return local_error;
    }

    if ( rightType==Kumir::IntegerArray || rightType==Kumir::RealArray || rightType==Kumir::CharectArray || rightType==Kumir::StringArray || rightType==Kumir::BooleanArray ) {
        result.position =  assignPos;
        result.length =  1;
        result.code = PRISV_RIGHT_ARRAY;
        return result;
    }

    // проверяем соответствие типов и выбираем подходящее ругательство

    if(leftType!=rightType)
    {
        result.position =  assignPos;
        result.length =  1;
        if ( leftType == Kumir::Integer )
        {
            if ( rightType == Kumir::Real )
                result.code = PRISV_INT_REAL;
            if ( rightType == Kumir::Charect )
                result.code = PRISV_INT_CHAR;
            if ( rightType == Kumir::String )
                result.code = PRISV_INT_STRING;
            if ( rightType == Kumir::Boolean )
                result.code = PRISV_INT_BOOL;
            if ( rightType == Kumir::RealArray )
                result.code = PRISV_INT_MASSREAL;
            if ( rightType == Kumir::CharectArray )
                result.code = PRISV_INT_MASSCHAR;
            if ( rightType == Kumir::StringArray )
                result.code = PRISV_INT_MASSSTRING;
            if ( rightType == Kumir::BooleanArray )
                result.code = PRISV_INT_MASSBOOL;
        }
        if ( leftType == Kumir::Real )
        {
            if ( rightType == Kumir::Charect )
                result.code = PRISV_REAL_CHAR;
            if ( rightType == Kumir::String )
                result.code = PRISV_REAL_STRING;
            if ( rightType == Kumir::Boolean )
                result.code = PRISV_REAL_BOOL;
            if ( rightType == Kumir::RealArray )
                result.code = PRISV_REAL_MASSREAL;
            if ( rightType == Kumir::CharectArray )
                result.code = PRISV_REAL_MASSCHAR;
            if ( rightType == Kumir::StringArray )
                result.code = PRISV_REAL_MASSSTRING;
            if ( rightType == Kumir::BooleanArray )
                result.code = PRISV_REAL_MASSBOOL;
        }
        if ( leftType == Kumir::Charect )
        {
            if ( rightType == Kumir::Real )
                result.code = PRISV_CHAR_REAL;
            if ( rightType == Kumir::Integer )
                result.code = PRISV_CHAR_INT;
            if ( rightType == Kumir::String )
                result.code = PRISV_CHAR_STRING;
            if ( rightType == Kumir::Boolean )
                result.code = PRISV_CHAR_BOOL;
            if ( rightType == Kumir::RealArray )
                result.code = PRISV_CHAR_MASSREAL;
            if ( rightType == Kumir::CharectArray )
                result.code = PRISV_CHAR_MASSCHAR;
            if ( rightType == Kumir::StringArray )
                result.code = PRISV_CHAR_MASSSTRING;
            if ( rightType == Kumir::BooleanArray )
                result.code = PRISV_CHAR_MASSBOOL;
        }
        if ( leftType == Kumir::Boolean )
        {
            if ( rightType == Kumir::Real )
                result.code = PRISV_BOOL_REAL;
            if ( rightType == Kumir::Integer )
                result.code = PRISV_BOOL_INT;
            if ( rightType == Kumir::String )
                result.code = PRISV_BOOL_STRING;
            if ( rightType == Kumir::Integer )
                result.code = PRISV_BOOL_INT;
            if ( rightType == Kumir::RealArray )
                result.code = PRISV_BOOL_MASSREAL;
            if ( rightType == Kumir::CharectArray )
                result.code = PRISV_BOOL_MASSCHAR;
            if ( rightType == Kumir::StringArray )
                result.code = PRISV_BOOL_MASSSTRING;
            if ( rightType == Kumir::BooleanArray )
                result.code = PRISV_BOOL_MASSBOOL;
        }
        if ( leftType == Kumir::String )
        {
            if ( rightType == Kumir::Real )
                result.code = PRISV_STR_REAL;
            if ( rightType == Kumir::Integer )
                result.code = PRISV_STR_INT;
            if ( rightType == Kumir::Boolean )
                result.code = PRISV_STR_BOOL;
            if ( rightType == Kumir::Integer )
                result.code = PRISV_STR_INT;
            if ( rightType == Kumir::RealArray )
                result.code = PRISV_STR_MASSREAL;
            if ( rightType == Kumir::CharectArray )
                result.code = PRISV_STR_MASSCHAR;
            if ( rightType == Kumir::StringArray )
                result.code = PRISV_STR_MASSSTRING;
            if ( rightType == Kumir::BooleanArray )
                result.code = PRISV_STR_MASSBOOL;
        }
        return result;
    }

    // добавляем ссылку на правую часть в результирующую строку
    out += "&"+QString::number(exprList.count()-1);

    // заносим все в STR_DATA
    pv.VirajList.clear();
    pv.VirajList << exprList;
    pv.line = out;

    prisvElem += leftElem + rightElem + "</assignment>\n";
    root += prisvElem;

    return result;
}



//==================================== CLASS TextAnalizer ========================================
Kumir::ValueType KsymbolToValueType(const QChar &c)
{
    if (c==KS_INT)
        return Kumir::Integer;
    else if (c==KS_INT_TAB)
        return Kumir::IntegerArray;
    else if (c==KS_REAL)
        return Kumir::Real;
    else if (c==KS_REAL_TAB)
        return Kumir::RealArray;
    else if (c==KS_BOOL)
        return Kumir::Boolean;
    else if (c==KS_BOOL_TAB)
        return Kumir::BooleanArray;
    else if (c==KS_STRING)
        return Kumir::String;
    else if (c==KS_STRING_TAB)
        return Kumir::StringArray;
    else if (c==KS_CHAR)
        return Kumir::Charect;
    else if (c==KS_CHAR_TAB)
        return Kumir::CharectArray;
    else
        return Kumir::Undefined;
}

Kumir::CompileError TextAnalizer::parceVariablesDeclaration(KumirInstruction &pv,
                                                            int str,
                                                            const QString &alg,
                                                            SymbolTable &symbols,
                                                            FunctionTable &functions,
                                                            QString &xml)
{
    Kumir::CompileError result;
    result.code = 0;

    if (m_denyVariableDeclaration>0) {
        result.position =  0;
        result.length =  pv.line.length();
        result.code = SYMB_DECL_IN_LOOP; return result;
    }
    QString stroka = pv.line;

    QList<int> ids;
    QPair<int,int> local_err_pos;
    pv.VirajList.clear();
    if ( stroka.trimmed().endsWith(",") )
    {
        result.position =  stroka.lastIndexOf(",");
        result.length =  1;
        result.code = SYMB_EXTRA_COMMA; return result;
    }
    QStringList xmlList;
    QList<KumirInstruction> extraProgram;
    result = parceVariables(stroka,

                            alg,
                            arg,
                            str,
                            symbols,
                            functions,
                            ids,
                            pv.VirajList,
                            pv.extraSubExpressions,
                            xmlList,
                            false,
                            extraProgram);

    int counter = 0;
    pv.line = "";
    for (int i=0; i<extraProgram.count(); i++) {
        //        extraProgram[i].Kumir::Real_line = pv.Kumir::Real_line;
    }
    pv.generatedLinesAfter = extraProgram;
    xml = "<declare>\n";
    if ( result.code == 0 )
    {
        for ( int i=0; i<ids.count(); i++ )
        {
            Kumir::ValueType type = symbols.symb_table[ids[i]].type;
            xml += "<value id='"+QString::number(ids[i])+"' />\n";
            if ( (type == Kumir::Integer) || (type == Kumir::Real) || (type == Kumir::Boolean) || (type == Kumir::String) || (type == Kumir::Charect) )
            {
                pv.line += "^"+QString::number(ids[i]);
            }
            else {
                pv.line += "^"+QString::number(ids[i]);
                int dim = symbols.symb_table[ids[i]].razm;
                for ( int j=0; j<dim*2; j++ )
                {
                    pv.line += "#"+QString::number(counter);
                    counter++;
                }
            }

            if ( i != ( ids.count()-1 ) )
                pv.line += ",";
        }
    }

    xml += "</declare>\n";

    return result;
}



/**
* Парсировка "если ...". Использует глоб. переменные. Набор аргументов стандартный.
* @param instr
* @param functions
* @param symbols
* @param str
* @return Код ошибки
 */
Kumir::CompileError TextAnalizer::parceIf(KumirInstruction &pv, SymbolTable &symbols, FunctionTable &functions,int str,QString &xml)
{
    Kumir::CompileError result;
    result.code = 0;
    QString instr = pv.line;
    int to_pos=instr.indexOf(QChar(KS_TO));
    QString uslovie="";
    if(to_pos>-1)
    {
        uslovie=instr.mid(1,to_pos-2);
        to_pos=instr.length();
    }
    else
        uslovie=instr.mid(1);
    if(uslovie.isEmpty())
    {
        result.position = 1;
        result.length = 1;
        result.code = IF_NO_CONDITION_3;
    }

    if(m_textNormalizer->test_skobk(uslovie)!=0)
    {
        result.position =  1;
        result.length =  instr.length()-1;
        result.code = IF_BAD_CONDITION;
    }

    if (result.code)
        return result;

    QStringList parsedViraj;
    QStringList resViraj;

    xml += "<if>\n<condition>\n";

    StructType st;

    Kumir::ValueType intType = parceExpression(uslovie,
                                               functions,
                                               symbols,
                                               functions.nameByPos(str),
                                               parsedViraj,
                                               st,
                                               result,
                                               pv.extraSubExpressions,
                                               xml);

    xml += "</condition>\n";

    if (result.code) {
        result.position += 1;
        return result;
    }

    if(intType!=Kumir::Boolean)
    {
        result.position =  2;
        result.length =  instr.length()-2;
        result.code = IF_TYPE_MISS;
        return result;
    };
    pv.VirajList.clear();

    pv.VirajList.append(parsedViraj);

    pv.line=instr;

    return result;
};



Kumir::CompileError TextAnalizer::parseLoopBegin(KumirInstruction &pv,
                                                 SymbolTable &symbols,
                                                 FunctionTable &functions,
                                                 int str,
                                                 QString &xml)
{
    Kumir::CompileError result;
    QString instr = pv.line;
    int iz, iz1;
    Kumir::LoopType circle_type = Kumir::FreeLoop;
    QString out_line=instr;
    int ras_pos=instr.indexOf(KS_TIMES);
    pv.VirajList.clear();

    if (instr[2]==KS_FOR)
        circle_type=Kumir::ForLoop;
    else if (instr[2]==KS_WHILE)
        circle_type=Kumir::WhileLoop;

    else if (instr.length()==1)
        circle_type=Kumir::FreeLoop;

    else if (ras_pos>-1)
        circle_type=Kumir::TimesLoop;

    else
    {
        result.position =  0;
        result.length =  instr.length();
        result.code = CIKL_NO_KEYWORD; return result;
    }


    xml += "<loop type='";
    if (circle_type==Kumir::ForLoop)
        xml += "for";
    if (circle_type==Kumir::WhileLoop)
        xml += "while";
    if (circle_type==Kumir::FreeLoop)
        xml += "free";
    if (circle_type==Kumir::TimesLoop)
        xml += "times";
    xml += "'>\n";

    if (circle_type==Kumir::ForLoop)
    {
        int ot_pos=instr.indexOf(QChar(KS_FROM));
        out_line=QChar(KS_LOOP)+' '+QChar(KS_FOR)+' ';
        if(ot_pos<5&&ot_pos>-1)
        {
            result.position =  2;
            result.length =  1;
            result.code = CIKL_NO_LOOP_VAR; return result;
        }
        else if (ot_pos<5) {
            result.position =  2;
            result.length =  1;
            result.code = CICK_NO_FROM; return result;
        }

        // V.Y. -- moved check for 'do' from below
        int do_pos=instr.indexOf(QChar(KS_TO));
        if(do_pos==-1)
        {
            result.position =  0;
            result.length =  instr.length();
            result.code = CIKL_NO_DO; return result;
        }

        if(do_pos<ot_pos+1)
        {
            //MR-8
            // Mark ot OT do DO
            iz = do_pos;
            iz1 = ot_pos;
            if (ot_pos < do_pos)
            {
                iz = ot_pos;
                iz1 = do_pos;
            }
            result.position =  iz;
            result.length =  iz1-iz;
            result.code = CIKL_DO_NE_TAM; return result; // 2607
            //RM
        }


        QString circle_perem=instr.mid(4,ot_pos-5);
        QString alg=functions.nameByPos(str);
        int perem_id=symbols.inTable(circle_perem, functions.nameByPos(str));

        if ( perem_id == -1 )
        {
            perem_id = symbols.inTable(circle_perem,"global");
        }



        // test for Constant -- NEW V.Y.
        bool isLiteralConst = ( circle_perem.startsWith("\"") && circle_perem.endsWith("\"") )
                              || (circle_perem.startsWith("'") && circle_perem.endsWith("'") );

        QRegExp rxNumericConst (QObject::trUtf8("^[0-9]*\\.?[0-9]+[EЕ]?[0-9]*\\.?[0-9]*"));

        if (
                (rxNumericConst.indexIn(circle_perem)!=-1)
                || isLiteralConst
                )
        {
            result.position =  4;
            result.length =  circle_perem.length();
            result.code = CIKL_CONST; return result;
        }
        if (perem_id<0)
        {
            result.position =  4;
            result.length =  circle_perem.length();
            result.code = CIKL_NO_IMPL; return result;
        }

        if (symbols.getTypeByID(perem_id)!=Kumir::Integer)
        {
            result.position =  4;
            result.length =  circle_perem.length();
            result.code = CIKL_PARAM_TYPE_ERR; return result;
        }
        out_line=out_line+'^'+QString::number(perem_id)+' ';

        xml += "<for id='"+QString::number(perem_id)+"' />\n";


        //от проверка
        QString viraj=instr.mid(ot_pos+2,do_pos-ot_pos-2);
        //QStringList virajList=KumTools::instance()->split_prisv(viraj);



        QStringList parsedViraj;

        if ( viraj.trimmed().isEmpty() ) {
            result.position =  ot_pos;
            result.length =  do_pos - ot_pos;
            result.code = CICK_EMPTY_FROM; return result;
        }

        xml += "<from>\n";

        StructType st;
        Kumir::ValueType intType = parceExpression(viraj, // source
                                                   functions, // function table
                                                   symbols, // symbol table
                                                   functions.nameByPos(str), // current algorhitm
                                                   parsedViraj, // expr list
                                                   st,
                                                   result, // error position
                                                   //  moduleId, // module id
                                                   pv.extraSubExpressions,
                                                   xml); // xml out

        xml += "</from>\n";
        if(result.code)
        {
            result.position += ot_pos + 2;
            return result;
        }
        if(intType!=Kumir::Integer)
        {
            result.position =  ot_pos+2;
            result.length =  viraj.length();
            result.code = CIKL_TYPE_MISS_1; return result;
        }
        out_line=out_line+QChar(KS_FROM)+" #0 ";
        pv.VirajList.append(parsedViraj);

        parsedViraj.clear();
        //до проверка
        int shag=instr.indexOf(QChar(KS_STEP));
        if(shag>-1)
            viraj=instr.mid(do_pos+1,instr.indexOf(QChar(KS_STEP))-do_pos-1);
        else
            viraj=instr.mid(do_pos+1);

        if (viraj.simplified().isEmpty()) {
            result.position =  do_pos;
            result.length =  1;
            result.code = CICK_EMPTY_TO; return result;
        }

        iz = shag;

        if ( iz == -1 )
            iz = instr.length();
        //virajList=KumTools::instance()->split_prisv(viraj);

        xml += "<to>\n";
        intType = parceExpression(viraj,
                                  functions,
                                  symbols,
                                  functions.nameByPos(str),
                                  parsedViraj,
                                  st,
                                  result,
                                  pv.extraSubExpressions,
                                  xml);

        xml += "</to>\n";
        if(result.code)
        {
            result.position += do_pos + 1;
            return result;
        }
        if(intType!=Kumir::Integer)
        {
            result.position =  do_pos+1;
            result.length =  viraj.trimmed().length()+1;
            result.code = CIKL_TYPE_MISS_2; return result;
        }

        QStringList resViraj;

        pv.VirajList.append(parsedViraj);
        out_line=out_line+QChar(KS_TO)+" #1 ";

        if(shag>-1) //для циклов с шагом
        {
            viraj=instr.mid(shag+1);


            if (viraj.simplified().isEmpty()) {
                result.position =  shag;
                result.length =  1;
                result.code = CICK_EMPTY_STEP; return result;
            }

            xml += "<step>\n";
            intType=parceExpression(viraj,
                                    functions,
                                    symbols,
                                    functions.nameByPos(str),
                                    parsedViraj,
                                    st,
                                    result,
                                    pv.extraSubExpressions,
                                    xml);

            xml += "</step>\n";

            if(result.code)
            {
                result.position += shag +1;
                return result;
            }
            if(intType!=Kumir::Integer)
            {
                result.position =  shag+1;
                result.length =  viraj.trimmed().length()+1;
                result.code = CIKL_TYPE_MISS_3; return result;
            }

            // KumTools::instance()->SplitPrisvBlock(parsedViraj,&resViraj,-1,false);
            pv.VirajList.append(parsedViraj);
            out_line=out_line+QChar(KS_STEP)+" #2 ";

        };

    };

    if (circle_type==Kumir::TimesLoop)
    {
        if (ras_pos<=2)
        {
            result.position =  ras_pos;
            result.length =  1;
            result.code = CIKL_SKOLKO_RAZ; return result;  // 2611
        }
        int instr_len = instr.length();
        if ( ras_pos != (instr_len-1) )
        {
            result.position =  ras_pos+1;
            result.length =  instr.length()-result.position;
            result.code = CIKL_TRASH_RAZ; return result;
        }
        out_line=QChar(KS_LOOP)+' ';
        QString viraj=instr.mid(2,ras_pos-2);



        QStringList parsedViraj;

        xml += "<times>\n";
        StructType st;
        Kumir::ValueType intType=parceExpression(
                viraj,
                functions,
                symbols,
                functions.nameByPos(str),
                parsedViraj,
                st,
                result,
                pv.extraSubExpressions,
                xml);
        xml += "</times>\n";
        if(result.code)
        {
            result.position += 2;
            return result;
        }
        if(intType!=Kumir::Integer)
        {
            result.position =  2;
            result.length =  viraj.length();
            result.code = CIKL_TYPE_MISS_4; return result;    // 2613
        }
        out_line=out_line+"#0 "+QChar(KS_TIMES);
        pv.VirajList.append(parsedViraj);

    };

    if (circle_type==Kumir::WhileLoop)
    {

        out_line=QChar(KS_LOOP)+' '+QChar(KS_WHILE);
        QString viraj=instr.mid(3);
        if((viraj.isEmpty())||(viraj.isNull())||(viraj=="\n"))
        {
            result.position =  2;
            result.length =  1;
            result.code = CIKL_NO_CONDITION; return result;
        }

        QStringList parsedViraj;


        xml += "<while>\n";
        StructType st;
        Kumir::ValueType intType=parceExpression(viraj,
                                                 functions,
                                                 symbols,
                                                 functions.nameByPos(str),
                                                 parsedViraj,
                                                 st,
                                                 result,
                                                 pv.extraSubExpressions,
                                                 xml);

        xml += "</while>\n";
        if(result.code)
        {
            result.position += 3;
            return result;
        }
        if(intType!=Kumir::Boolean)
        {
            result.position =  4;
            result.length =  instr.length()-4;
            result.code = CIKL_TYPE_MISS_5; return result;
        }
        out_line=out_line+" #0";
        pv.VirajList.append(parsedViraj);

    };

    if (circle_type==Kumir::FreeLoop)
    {
        out_line=QChar(KS_LOOP);
    };

    pv.line=out_line;
    xml += "<body>\n";

    return result;

}



//++++++++++++++++++++++ANALIZ
/**
* Проверка правильности расстановки опер. скобок. и расстановка строк прыжка
* @param nagr Нагрузка текста
* @param Proga_Value Внутреннее представление текста см form1.ui.h
 */

struct StackElem
{
    StackElem(int a, int b, int c) { str_type=a; open_str=b; elem_str=c; }
    StackElem() { str_type = 0; open_str = 0; elem_str = 0; }
    int str_type;
    int open_str;
    int elem_str;
};





/**
 * Парсировка команды "вывод". Использует глоб. переменные. Набор аргументов стандартный.
 * @param prisv
 * @param functions
 * @param symbols
 * @param str
 * @return Код ошибки
 */
Kumir::CompileError TextAnalizer::parceOutput(
        KumirInstruction &pv,
        FunctionTable &functions,
        SymbolTable &symbols,
        int str,QString &xml
        )
{
    Kumir::CompileError result;
    result.position = 0;
    result.length = pv.line.length();
    result.code = 0;
    QString prisv = pv.line;
    if ( prisv.trimmed().length()==1 ) {
        result.position =  0;
        result.length =  1;
        result.code = VYVOD_EMPTY; return result;
    }
    if ( prisv.simplified().length() > 0 && !prisv.simplified()[1].isSpace() ) {

        result.code = VYVOD_NO_SPACE; return result;
    }
    QString viraj=prisv.mid(2);
    QStringList virajs;
    QList<int> start_pos;
    QList<int> end_pos;
    if ( viraj.trimmed().length() == 0 )
    {

        result.code = VYVOD_EMPTY; return result;
    }

    //                      A. Prolog
    int skobka_op=0,skobka_mass=0,last_zpt=1;
    //              A1. Form virajs - spisok termov vyvoda
    for(int i=0;i<prisv.length();i++)
    {
        if(prisv[i]=='(')
            skobka_op++;
        if(prisv[i]==')')
            skobka_op--;

        if(prisv[i]=='[')
            skobka_mass++;
        if(prisv[i]==']')
            skobka_mass--;

        //if(skobka_op==0)skobka_mass=0;

        if((skobka_op<0)||(skobka_mass<0))
        {
            result.position =  2 + viraj.indexOf(')');
            result.length =  1;
            result.code = VYVOD_BAD_SKOBKA_1; return result;
        }
        if((prisv[i]==',')&&(skobka_op==0)&&(skobka_mass==0))
        {
            QString test=prisv.mid(last_zpt+1,i-last_zpt-1);
            virajs.append(prisv.mid(last_zpt+1,i-last_zpt-1));
            start_pos.append(last_zpt+1);
            end_pos.append(i);
            last_zpt=i;
        }
    } // end for ( i )
    if((skobka_op>0)||(skobka_mass>0))
    {
        result.position =  2 + viraj.lastIndexOf('(');
        result.length =  1;
        result.code = VYVOD_BAD_SKOBKA_2; return result;
    }
    //MR1-NEW 24.08
    int lastSymb = prisv.length()-1;
    if (last_zpt == lastSymb){
        result.position =  2 + viraj.lastIndexOf(',');
        result.length =  1;
        result.code = VYVOD_END_ZAP; return result; // 2503-VYVOD_END_ZAP; // Запятая в конце
    } else {
        virajs.append(prisv.mid(last_zpt+1));
        start_pos.append(last_zpt+1);
        end_pos.append(prisv.length());
    }
    //RM
    // Bylo: virajs.append(prisv.mid(last_zpt+1));
    //MR-2 Zapomnit' polozheniyz zapiatyh (esli neslozhno - see last_zpt = i)

    //              A2, Obnulenie VirajTypes, VirajList
    pv.VirajTypes.clear();
    pv.VirajList.clear();

    xml = "<output>\n";

    //                      B. Main Loop po termam
    for(int i=0;i<virajs.count();i++)
    {
        QString vir = virajs[i];
        QStringList virajList;


        QStringList parsedViraj;
        //MR-3  DENIS - PROVER' POZH
        if (virajs[i].trimmed() == QChar(KS_NEWLINE))
        {

            xml += "<new_line/>\n";

            QStringList sList;
            sList.append(QString(QChar(KS_NEWLINE)));
            pv.VirajTypes.append(Kumir::Undefined);
            pv.VirajList.append(sList);

            continue;
        }
        //MR-3 End
        int spaces = 0;
        while (virajs[i].startsWith(" ")) {
            spaces++;
            virajs[i].remove(0,1);
        }

        QString xmlData; StructType st;

        Kumir::ValueType intType =
                parceExpression(
                        virajs[i],
                        functions,symbols,
                        functions.nameByPos(str),
                        virajList,
                        st,
                        result,
                        pv.extraSubExpressions,
                        xmlData);

        xml += xmlData;

        if(result.code)
        {
            result.position += start_pos[i]+spaces;
            return result;
        }
        if (
                (intType==Kumir::BooleanArray)
                ||
                (intType==Kumir::IntegerArray)
                ||
                (intType==Kumir::RealArray)
                ||
                (intType==Kumir::CharectArray)
                ||
                (intType==Kumir::StringArray)
                )
        {
            result.position =  start_pos[i]+spaces;
            result.length =  virajs[i].length();
            result.code = VYVOD_MASS; return result;
        }
        intType=retUnMassType(intType);
        pv.VirajTypes.append(intType);
        pv.VirajList.append(virajList);
    }

    xml += "</output>\n";

    return result;
}





/**
 * Парсировка команды "вывод". Использует глоб. переменные. Набор аргументов стандартный.
 * @param prisv
 * @param functions
 * @param symbols
 * @param str
 * @return Код ошибки
 */
Kumir::CompileError TextAnalizer::parceFileOutput(KumirInstruction &pv, FunctionTable &functions, SymbolTable &symbols, int str, QString &xml)
{
    QString prisv = pv.line;
    Kumir::CompileError result;
    result.position = 0;
    result.length = prisv.length();
    result.code = 0;

    if(!m_modulesPool->moduleEnabled("Files"))
    {
        result.position =  0;
        result.length =  1;
        result.code = RT_ERR_NO_FILE_ISP; return result;
    }

    QString viraj=prisv.mid(2);
    QStringList virajs;
    QList<int> start_pos;
    QList<int> end_pos;
    // NEW 24.08
    if ( viraj.trimmed().length() == 0 )
    {

        result.code = VYVOD_EMPTY; return result;
    }
    // END NEW

    //                      A. Prolog
    int skobka_op=0,skobka_mass=0,last_zpt=1;
    //              A1. Form virajs - spisok termov vyvoda

    xml = "<file_output>\n";

    for(int i=0;i<prisv.length();i++)
    {
        if(prisv[i]=='(')
            skobka_op++;
        if(prisv[i]==')')
            skobka_op--;

        if(prisv[i]=='[')
            skobka_mass++;
        if(prisv[i]==']')
            skobka_mass--;

        //if(skobka_op==0)skobka_mass=0;

        if((skobka_op<0)||(skobka_mass<0))
        {

            result.position =  2 + viraj.indexOf(')');
            result.length =  1;
            result.code = VYVOD_BAD_SKOBKA_1; return result;
        }
        if((prisv[i]==',')&&(skobka_op==0)&&(skobka_mass==0))
        {
            QString test=prisv.mid(last_zpt+1,i-last_zpt-1);
            virajs.append(prisv.mid(last_zpt+1,i-last_zpt-1));
            start_pos.append(last_zpt+1);
            end_pos.append((i-last_zpt-1)+(last_zpt));
            last_zpt=i;
        }
    } // end for ( i )
    if((skobka_op>0)||(skobka_mass>0))
    {
        result.position =  2 + viraj.lastIndexOf('(');
        result.length =  1;
        result.code = VYVOD_BAD_SKOBKA_2; return result;
    }
    //MR1-NEW 24.08
    int lastSymb = prisv.length()-1;
    if (last_zpt == lastSymb){
        result.position =  2 + viraj.lastIndexOf(',');
        result.length =  1;
        result.code = VYVOD_END_ZAP; return result; // 2503-VYVOD_END_ZAP; // Запятая в конце
    } else {
        virajs.append(prisv.mid(last_zpt+1));
        start_pos.append(last_zpt);
        end_pos.append(prisv.length()-1);
    }

    //              A2, Obnulenie VirajTypes, VirajList
    pv.VirajTypes.clear();
    pv.VirajList.clear();

    //                      B. Main Loop po termam
    if(virajs.count()<2)
    {
        result.position =  1;
        result.length =  prisv.length()-1;
        result.code = VYVOD_EMPTY; return result;

    };
    for(int i=0;i<virajs.count();i++)
    {
        QString vir = virajs[i];
        QStringList virajList;


        QStringList parsedViraj;
        //MR-3  DENIS - PROVER' POZH
        if (virajs[i].trimmed() == QChar(KS_NEWLINE))
        {
            QStringList sList;
            sList.append(QString(QChar(KS_NEWLINE)));
            pv.VirajTypes.append(Kumir::Undefined);
            pv.VirajList.append(sList);

            xml = "<new_line/>\n";

            continue;
        }
        //MR-3 End
        QPair<int,int> local_err_pos;
        QString xmlData; StructType st;
        Kumir::ValueType intType =
                parceExpression(
                        virajs[i],
                        functions,symbols,
                        functions.nameByPos(str),
                        virajList,
                        st,
                        result,
                        pv.extraSubExpressions,
                        xmlData);

        xml = xmlData;

        if(result.code)
        {
            result.position += start_pos[i];
            return result;
        }
        if((i==0)&&(intType!=Kumir::Integer)) {
            QString dbg = prisv;
            result.position =  start_pos[i];
            if ( i<virajs.count()-1 ) {


                result.length =  start_pos[i+1]-start_pos[i];
            }
            else {

                result.length =  prisv.length()-start_pos[i];
            }

            result.code = F_KEY_NOT_INT; return result;
        }
        if (
                (intType==Kumir::BooleanArray)
                ||
                (intType==Kumir::IntegerArray)
                ||
                (intType==Kumir::RealArray)
                ||
                (intType==Kumir::CharectArray)
                ||
                (intType==Kumir::StringArray)
                )
        {
            result.position =  start_pos[i]+local_err_pos.first;
            result.length =  virajs[i].length();
            result.code = VYVOD_MASS; return result;
        }
        intType=retUnMassType(intType);
        pv.VirajTypes.append(intType);
        pv.VirajList.append(virajList);
    }

    xml = "<file_output>\n";

    return result;
}



Kumir::CompileError TextAnalizer::parcePre(KumirInstruction &pv,FunctionTable &functions,SymbolTable &symbols,int str,QString &xml)
{
    QString instr = pv.line;
    Kumir::CompileError result;
    result.code = 0;
    result.position = 0;
    result.length = instr.length();

    if(functions.nameByPos(str)=="global")
    {
        result.code = DANO_NOFUNCT;
        return result;
    }
    QString uslovie="";
    uslovie=instr.mid(1);

    if(uslovie.trimmed().isEmpty())
    {
        pv.VirajList.clear();
        return result;
    }

    QStringList resViraj;

    QStringList parsedViraj; StructType st;

    xml = "<input_assert>\n";

    Kumir::ValueType intType=parceExpression(uslovie,
                                             functions,
                                             symbols,
                                             functions.nameByPos(str),
                                             parsedViraj,
                                             st,
                                             result,
                                             pv.extraSubExpressions,
                                             xml);

    xml += "</input_assert>\n";


    if(result.code)
    {
        result.position += 1;
        return result;
    }

    if(intType!=Kumir::Boolean)
    {
        result.position =  1;
        result.length =  instr.length()-1;
        result.code = DANO_TYPE_MISS;
        return result;
    }

    pv.VirajList.clear();
    pv.VirajList.append(parsedViraj);

    pv.line=instr;

    return result;
}




Kumir::CompileError TextAnalizer::parcePost(KumirInstruction &pv, FunctionTable &functions, SymbolTable &symbols, int str, QString &xml)
{
    QString instr = pv.line;
    Kumir::CompileError result;
    result.code = 0;
    result.position = 0;
    result.length = instr.length();
    pv.VirajList.clear();
    if(functions.nameByPos(str)=="global")
    {
        result.code = NADO_NOFUNCT;
        return result;
    }
    QString uslovie="";
    uslovie=instr.mid(1);

    if (uslovie.trimmed().isEmpty())
    {
        pv.VirajList.clear();
        QStringList empty;
        empty << "";
        pv.VirajList.append(empty);
        return result;
    }

    QStringList parsedViraj; StructType st;

    xml = "<output_assert>\n";

    Kumir::ValueType       intType=parceExpression(uslovie,
                                                   functions,
                                                   symbols,
                                                   functions.nameByPos(str),
                                                   parsedViraj,
                                                   st,
                                                   result,
                                                   pv.extraSubExpressions,
                                                   xml);

    xml += "</output_assert>\n";

    if(result.code)
    {
        result.position += 1;
        return result;
    }
    if(intType!=Kumir::Boolean)
    {
        result.position =  1;
        result.length =  instr.length()-1;
        result.code = NADO_TYPE_MISS;
        return result;
    }

    pv.VirajList.append(parsedViraj);
    pv.line=instr;
    return result;
}


/**
* Парсировк строки ввод.  Использует глоб. переменные.
* @param instr
* @param functions
* @param symbols
* @param str
* @param err_start
* @param err_end
* @return
 */

struct Term {
    QString text;
    int start;
    int end;
};

Kumir::CompileError TextAnalizer::parceInput(KumirInstruction &pv, FunctionTable &functions, SymbolTable &symbols,int str, QString &xml)
{
    QString instr = pv.line;
    Kumir::CompileError result;
    result.code = 0;
    result.position = 0;
    result.length = instr.length();
    if ( instr.trimmed().length()==1 ) {
        result.position =  0;
        result.length =  1;
        result.code = VVOD_EMPTY; return result;
    }

    if ( instr.simplified().length() > 0 && !instr.simplified()[1].isSpace() ) {
        result.position =  0;
        result.length =  1;
        result.code = VVOD_NO_SPACE; return result;
    }

    pv.VirajList.clear();

    instr.remove(0,2);
    // NEW V.Y. 24.08
    if (  (instr.isNull())
        ||
                (instr=="\n")
        ||
                (instr==" ")
        ||
                (instr.isEmpty())
        )

        {
        result.position =  0;
        result.length =  1;
        result.code = VVOD_EMPTY; return result;
    }
    // END NEW

    // NEW 25.08
    if (instr.trimmed().endsWith(","))
    {
        result.position =  2+ instr.lastIndexOf(",");
        result.length =  1;
        result.code = VVOD_END_ZAP; return result;
    }

    // END NEW
    int err=0;

    instr = instr.trimmed();

    QList<Term> terms;
    Term first;
    first.text = "";
    first.start = 0;
    first.end = 0;
    terms << first;
    int deep1 = 0; // уровень вложенности ()
    int deep2 = 0; // уровень вложенности []

    xml = "<input>\n";

    for ( int i=0; i<instr.length(); i++ )
    {
        if ( instr[i] == '[' )
            deep2++;
        if ( instr[i] == ']' )
            deep2--;
        if ( instr[i] == '(' )
            deep1++;
        if ( instr[i] == ')' )
            deep1--;

        if ( instr[i] == ','  &&  deep1 == 0 && deep2 == 0  )
        {
            terms.last().end = i;
            Term next;
            next.text = "";
            next.start = i+1;
            terms << next;
        }
        else
        {
            terms.last().text += instr[i];
        }
    }

    terms.last().end = instr.length();
    bool hasSim = false;

    for ( int t=0; t<terms.count(); t++ )
    {
        xml += "<term>\n";
        QString uslovie = terms[t].text.trimmed();
        int op_sk=uslovie.indexOf('[');

        QList<int> mass_arg_starts;
        if(op_sk<1)
        {

            int p_id=symbols.inTable(uslovie,functions.nameByPos(str));
            if ( p_id < 0 )
                p_id = symbols.inTable(uslovie,"global");


            QStringList parsedViraj; StructType st;

            Kumir::ValueType perem_type = parceExpression(uslovie,
                                                          functions,
                                                          symbols,
                                                          functions.nameByPos(str),
                                                          parsedViraj,
                                                          st,
                                                          result,
                                                          pv.extraSubExpressions,
                                                          xml);

            if (
                    (perem_type==Kumir::BooleanArray)
                    ||
                    (perem_type==Kumir::IntegerArray)
                    ||
                    (perem_type==Kumir::RealArray)
                    ||
                    (perem_type==Kumir::CharectArray)
                    ||
                    (perem_type==Kumir::StringArray)
                    )
            {
                result.position =  terms[t].start+2;
                result.length =  terms[t].text.length();
                result.code = VVOD_MASS; return result;
            }
            if ( result.code ) {
                int shift = terms[t].start+2;
                result.position += shift;
                return result;
            }
            if ( st != Variable ) {
                result.position =  terms[t].start+2;
                result.length =  terms[t].text.length();
                if ( st == Constant )
                {
                    result.code = VVOD_NON_STRING_CONST; return result;
                }
                else if ( st != Constant )
                    result.code = VVOD_EXPRESSION; return result;
            }
            if ( !pv.line.isEmpty() )
                pv.line += ",";
            pv.VirajList.append(parsedViraj);
            pv.VirajTypes.append(perem_type);
            pv.line+="^"+QString::number(p_id);

            if ( perem_type == Kumir::Charect )
                hasSim = true;

            if(symbols.isConst(p_id))
            {
                result.position =  terms[t].start+2;
                result.length =  terms[t].text.length();
                result.code = VVOD_CONST; return result;
            }
            if ( symbols.symb_table[p_id].is_Arg && !(symbols.symb_table[p_id].is_Res||symbols.symb_table[p_id].is_ArgRes)) {
                result.position =  terms[t].start+2;
                result.length =  terms[t].text.length();
                result.code = VVOD_ARG; return result;
            }

        }
        if(op_sk>=1)
        {
            int close_skobk=uslovie.indexOf("]");
            int lastSymb = uslovie.length()-1;
            if(close_skobk!=lastSymb)
            {
                if ( close_skobk == -1 )
                    result.position =  instr.indexOf('[')+2;
                else
                    result.position =  instr.indexOf(']')+2;
                result.length =  1;
                result.code = VVOD_BAD_KVAD_ZAKR; return result;
            };
            QString viraj=uslovie.mid(op_sk+1,close_skobk-op_sk-1);//Для отладки

            QString name=uslovie.left(op_sk);
            QStringList args=splitMassArgs(viraj,mass_arg_starts,&err);
            if(err>0)
            {
                result.position =  3;
                result.length =  instr.length()-3;
                result.code = err;
                return result;
            }


            int p_id=symbols.inTable(name,functions.nameByPos(str));
            if ( p_id < 0 )
                p_id = symbols.inTable(name,"global");
            if(p_id<0)
            {
                int tn_start = 0, tn_len = 0;
                if(m_textNormalizer->test_name(name,tn_start,tn_len)>0)
                {
                    result.position =  3;
                    result.length =  1;
                    result.code = VVOD_BAD_NAME_2; return result; // VVOD_BAD_NAME_2;
                };
                result.position =  3;
                result.length =  1;
                result.code = VVOD_NO_IMPL_2; return result; // VVOD_NO_IMPL_2;
            };
            if(symbols.getTypeByID(p_id)==Kumir::Integer
               ||symbols.getTypeByID(p_id)==Kumir::Boolean
               ||symbols.getTypeByID(p_id)==Kumir::Real
               ||symbols.getTypeByID(p_id)==Kumir::Charect
               ||symbols.getTypeByID(p_id)==Kumir::String)
            {
                result.position =  3;
                result.length =  1;
                result.code = VVOD_TYPE_MISS_1; return result;
            }

            int args_count = args.count();
            if(symbols.getRazmById(p_id)!=args_count)
            {
                result.position =  op_sk+2;
                result.length =  instr.indexOf(']')-op_sk;
                result.code = VVOD_CANT_INDEX; return result;
            }

            QStringList parsedViraj; StructType st; QString exprXml;

            Kumir::ValueType  intType = parceExpression(uslovie,
                                                        functions,
                                                        symbols,
                                                        functions.nameByPos(str),
                                                        parsedViraj,
                                                        st,
                                                        result,
                                                        pv.extraSubExpressions,
                                                        exprXml);

            xml += exprXml;

            if(result.code)
            {
                result.position =  instr.indexOf('[')+2;
                result.length =  instr.indexOf(']') - result.position+2;
                return result;
            }
            if ( intType == Kumir::Charect )
                hasSim = true;

            QString line="^"+QString::number(p_id);

            for(int i=0;i<args.count();i++)
            {
                line=line+"&"+QString::number(i);

            }


            if ( !pv.line.isEmpty() )
                pv.line += ",";
            pv.VirajList.append(parsedViraj);
            pv.VirajTypes.append(intType);
            pv.line+=line;

        }
        xml += "</term>\n";
    }

    xml += "</input>\n";

    return result;
}



Kumir::CompileError TextAnalizer::parceFileInput(KumirInstruction &pv, FunctionTable &functions, SymbolTable &symbols, int str, QString &xml)
{
    QString instr = pv.line;
    Kumir::CompileError result;
    result.code = 0;
    result.position = 0;
    result.length = instr.length();

    QList<int> mass_arg_starts;


    if(!m_modulesPool->moduleEnabled("Files"))
    {
        result.position =  0;
        result.length =  1;
        result.code = RT_ERR_NO_FILE_ISP; return result;
    };
    pv.VirajList.clear();

    instr.remove(0,2);
    // NEW V.Y. 24.08
    if (  (instr.isNull())
        ||
                (instr=="\n")
        ||
                (instr==" ")
        ||
                (instr.isEmpty())
        )

        {
        result.position =  0;
        result.length =  1;
        result.code = VVOD_EMPTY; return result;
    }

    if (instr.trimmed().endsWith(","))
    {
        result.position =  instr.lastIndexOf(',');
        result.length = 1;
        result.code = VVOD_END_ZAP; return result;
    }


    xml = "<file_input>\n";

    QList<Term> terms;
    Term first;
    first.text = "";
    first.start = 0;
    first.end = 0;
    terms << first;
    int deep1 = 0; // уровень вложенности ()
    int deep2 = 0; // уровень вложенности []

    for ( int i=0; i<instr.length(); i++ )
    {
        if ( instr[i] == '[' )
            deep2++;
        if ( instr[i] == ']' )
            deep2--;
        if ( instr[i] == '(' )
            deep1++;
        if ( instr[i] == ')' )
            deep1--;

        if ( instr[i] == ','  &&  deep1 == 0 && deep2 == 0  )
        {
            terms.last().end = i;
            Term next;
            next.text = "";
            next.start = i+1;
            terms << next;
        }
        else
        {
            terms.last().text += instr[i];
        }
    }

    terms.last().end = instr.length();
    bool hasSim = false;
    if(terms.count()<2)
    {
        result.position =  0;
        result.length =  1;
        result.code = F_VVOD_NO_PEREM; return result;
    };
    for ( int t=0; t<terms.count(); t++ )
    {
        QString uslovie = terms[t].text.trimmed();
        int op_sk=uslovie.indexOf('[');

        if(op_sk<1)
        {

            int p_id=symbols.inTable(uslovie, functions.nameByPos(str));
            if ( p_id < 0 )
                p_id = symbols.inTable(uslovie,"global");

            if((t==0)&&(p_id<0))
            {

                result.position =  terms[t].start+2;
                result.length =  terms[t].text.length();
                result.code = VVOD_NO_IMPL_2; return result;
            };

            if(symbols.isConst(p_id)&&t>0)
            {
                result.position =  terms[t].start+2;
                result.length =  terms[t].text.length();
                result.code = VVOD_CONST; return result;
            }

            if((t==0)&&(symbols.getTypeByID(p_id)!=Kumir::Integer))
            {
                result.position =  terms[t].start+2;
                result.length =  terms[t].text.length();
                result.code = F_VVOD_KEY_ERROR; return result;
            };


            QStringList parsedViraj;
            StructType st;

            Kumir::ValueType perem_type = parceExpression(uslovie,
                                                          functions,
                                                          symbols,
                                                          functions.nameByPos(str),
                                                          parsedViraj,
                                                          st,
                                                          result,
                                                          pv.extraSubExpressions,
                                                          xml);

            if (
                    (perem_type==Kumir::BooleanArray)
                    ||
                    (perem_type==Kumir::IntegerArray)
                    ||
                    (perem_type==Kumir::RealArray)
                    ||
                    (perem_type==Kumir::CharectArray)
                    ||
                    (perem_type==Kumir::StringArray)
                    )
            {
                result.position =  terms[t].start;
                result.length =  terms[t].text.length();
                result.code = VVOD_MASS; return result;
            }
            if ( result.code ) {
                result.position += terms[t].start;
                return result;
            }
            if ( st != Variable && t>0 ) {
                result.position += terms[t].start;
                result.length =  terms[t].text.length();
                if ( st == Constant )
                {
                    result.code = VVOD_NON_STRING_CONST; return result;
                }
                else if ( st != Constant )
                    result.code = VVOD_EXPRESSION; return result;
            }
            if ( !pv.line.isEmpty() )
                pv.line += ",";
            pv.VirajList.append(parsedViraj);
            pv.VirajTypes.append(perem_type);
            pv.line+="^"+QString::number(p_id);

            if ( perem_type == Kumir::Charect )
                hasSim = true;
        };
        if(op_sk>=1)
        {
            int close_skobk=uslovie.indexOf("]");
            int lastSymb = uslovie.length()-1;
            if(close_skobk!=lastSymb)
            {
                if ( close_skobk == -1 )
                    result.position =  instr.indexOf('[');
                else
                    result.position =  instr.indexOf(']');
                result.length =  instr.length()-result.position;
                result.code = VVOD_BAD_KVAD_ZAKR; return result;
            };
            QString viraj=uslovie.mid(op_sk+1,close_skobk-op_sk-1);
            QString name=uslovie.left(op_sk);
            int err = 0;
            QStringList args=splitMassArgs(viraj,mass_arg_starts,&err);
            if(err>0)
            {
                result.position =  1;
                result.length =  instr.length()-1;
                result.code = err;
                return result;
            }


            int p_id=symbols.inTable(name,functions.nameByPos(str));
            if ( p_id < 0 )
                p_id = symbols.inTable(name,"global");
            if(p_id<0)
            {
                int tn_start = 0, tn_len = 0;
                if(m_textNormalizer->test_name(name,tn_start,tn_len)>0)
                {
                    result.position =  1;
                    result.length =  instr.length()-1;
                    result.code = VVOD_BAD_NAME_2; return result; // VVOD_BAD_NAME_2;
                }
                result.position =  1;
                result.length =  instr.length()-1;
                result.code = VVOD_NO_IMPL_2; return result; // VVOD_NO_IMPL_2;
            }
            if(symbols.getTypeByID(p_id)==Kumir::Integer
               ||symbols.getTypeByID(p_id)==Kumir::Boolean
               ||symbols.getTypeByID(p_id)==Kumir::Real
               ||symbols.getTypeByID(p_id)==Kumir::Charect)
            {
                result.position =  1;
                result.length =  instr.length()-1;
                result.code = VVOD_TYPE_MISS_1; return result;
            }


            int args_count = args.count();
            if(symbols.getRazmById(p_id)!=args_count)
            {
                result.position =  1;
                result.length =  instr.length()-1;
                result.code = VVOD_CANT_INDEX; return result; // VVOD_CANT_INDEX;
            }


            QStringList parsedViraj; StructType st;

            Kumir::ValueType intType=parceExpression(uslovie,
                                                     functions,
                                                     symbols,
                                                     functions.nameByPos(str),
                                                     parsedViraj,
                                                     st,
                                                     result,
                                                     pv.extraSubExpressions,
                                                     xml);
            if(result.code)
            {
                result.position =  1;
                result.length =  instr.length()-1;
                return result;
            }

            if ( hasSim )
            {
                result.position =  1;
                result.length =  instr.length()-1;
                result.code = VVOD_MANY_SYMBOLS; return result;
            }
            if ( intType == Kumir::Charect )
                hasSim = true;

            QString line="^"+QString::number(p_id);

            for(int i=0;i<args.count();i++)
            {
                line=line+"&"+QString::number(i);
            }


            if ( !pv.line.isEmpty() )
                pv.line += ",";
            pv.VirajList.append(parsedViraj);
            pv.VirajTypes.append(intType);
            pv.line+=line;

        }
    }

    xml = "</file_input>\n";

    return result;
}




/**
* Парсровка строки выбор. Использует глоб. переменные.
* @param instr Строка
* @param functions Таблица алгоритмов
* @param symbols Таблица переменных
* @param str Номер строки
* @param err_start Для вывода ошибок
* @param err_end  Для вывода ошибок
* @return Код ошибки
 */
Kumir::CompileError TextAnalizer::parceSwitch(KumirInstruction &pv, FunctionTable &, SymbolTable &, int )
{
    Kumir::CompileError result;
    result.position = 1;
    result.length = pv.line.length()-1;
    result.code = pv.line.length()>1? VYBOR_TRASH : 0;
    return result;
}



/**
* Парсировка оперетора при
* @param instr Строка
* @param functions
* @param symbols
* @param str номер строки
* @param err_start  для вывода ошибок
* @param err_end для вывода ошибок
* @return Код ошибки
 */
Kumir::CompileError TextAnalizer::parceCase(KumirInstruction &pv, FunctionTable &functions, SymbolTable &symbols, int str, QString &xml)
{
    QString instr = pv.line;
    QString uslovie="";
    uslovie=instr.mid(1);
    Kumir::CompileError result;
    result.code = 0;
    result.position = 0;
    result.length = instr.length();

    if(uslovie.trimmed().isEmpty())
    {
        pv.VirajList.clear();
        QStringList empty;
        empty << "";
        pv.VirajList.append(empty);
        result.code = PRI_NO_CONDITION_2;
        return result;
    }

    if(uslovie[uslovie.length()-1]!=':')
    {
        result.position =  0;
        result.length =  1;
        result.code = PRI_NO_DVOET;
        return result;
    }

    uslovie = uslovie.left(uslovie.length()-1);

    pv.VirajList.clear();
    uslovie=uslovie.trimmed();

    QStringList parsedViraj;


    xml += "<case>\n<condition>\n";

    QString xmlCondition;
    StructType st;
    Kumir::ValueType       intType=parceExpression(uslovie,
                                                   functions,
                                                   symbols,
                                                   functions.nameByPos(str),
                                                   parsedViraj,
                                                   st,
                                                   result,
                                                   pv.extraSubExpressions,
                                                   xmlCondition);

    xml += xmlCondition+"</condition>\n<body>\n";

    if(result.code)
    {
        result.position +=  2;
        return result;
    }

    if(intType!=Kumir::Boolean)
    {
        result.position =  2;
        result.length =  instr.length()-2;
        result.code = PRI_TYPE_MISS;
        return result;
    }

    pv.VirajList.append(parsedViraj);
    pv.line="PRI";
    return result;
}



/**
* Парсировка команды "Утв". Использует глоб. переменные. Набор аргументов стандартный.
* @param instr
* @param functions
* @param symbols
* @param str
* @return Код ошибки
 */


Kumir::CompileError TextAnalizer::parceAssert(KumirInstruction &pv, FunctionTable &functions,SymbolTable &symbols, int str, QString &xml)
{
    QString instr = pv.line;
    Kumir::CompileError result;
    result.code = 0;
    result.position = 0;
    result.length = instr.length();

    QString uslovie = instr.mid(1);
    if(uslovie.trimmed().isEmpty())
    {
        uslovie = KS_TRUE;
    }

    uslovie=uslovie.trimmed();

    QStringList parsedViraj;
    StructType st;


    xml += "<assert>\n";

    Kumir::ValueType       intType=parceExpression(uslovie,
                                                   functions,
                                                   symbols,
                                                   functions.nameByPos(str),
                                                   parsedViraj,
                                                   st,
                                                   result,
                                                   pv.extraSubExpressions,
                                                   xml);

    xml += "</assert>\n";

    if(result.code)
    {
        result.position += 2;
        return result;
    }
    if(intType!=Kumir::Boolean)
    {
        result.position =  2;
        result.length =  instr.length()-2;
        result.code = UTV_TYPE_MISS;
        return result;
    }
    pv.VirajList.clear();
    pv.VirajTypes.clear();
    pv.VirajList.append(parsedViraj);
    pv.VirajTypes.append(Kumir::Boolean);
    pv.line=instr;

    return result;
}



/**
* Парсировка команды "кц_при". Использует глоб. переменные. Набор аргументов стандартный.
* @param instr
* @param functions
* @param symbols
* @param str
* @return Код ошибки
 */


Kumir::CompileError TextAnalizer::parceConditionalEndloop(KumirInstruction &pv, FunctionTable &functions, SymbolTable &symbols, int str, QString &xml)
{
    QString instr = pv.line;
    QString uslovie=instr;
    uslovie=instr.mid(2);
    Kumir::CompileError result;
    result.code = 0;
    result.position =  0;
    result.length =  1;
    if(uslovie.trimmed().isEmpty())
    {
        result.code = KCPRI_NO_CONDITION_3;
        return result;
    }

    uslovie=uslovie.trimmed();
    pv.VirajList.clear();
    QStringList parsedViraj;//09-01-Mordol
    StructType st;


    xml += "</body>\n<break_loop>\n";


    Kumir::ValueType       intType=parceExpression(uslovie,
                                                   functions,
                                                   symbols,
                                                   functions.nameByPos(str),
                                                   parsedViraj,
                                                   st,
                                                   result,
                                                   pv.extraSubExpressions,
                                                   xml);

    xml += "</break_loop>\n</loop>\n";

    if(result.code)
    {
        result.position += 2;
        return result;
    }
    if(intType!=Kumir::Boolean)
    {
        result.position =  2;
        result.length =  instr.length()-2;
        result.code = KCPRI_TYPE_MISS;
        return result;
    }

    pv.VirajList.append(parsedViraj);
    pv.line=instr;

    return result;
}


TextAnalizer::TextAnalizer(TextNormalizer *normalizer, QObject *parent)
    : QObject(parent)
    , m_textNormalizer(normalizer)
{
    nestedAlg = 0;
    firstAlg = false;
    // Множество Ω. Разбито на классы по приоритетности операторов.
    Omega.append(QRegExp(QString(KS_OR)+"|"+KS_AND));
    Omega.append(QRegExp(QString(KS_NOT)));
    Omega.append(QRegExp(QString(KS_GEQ)+"|"+KS_LEQ+"|"+KS_NEQ+"|"+KS_ASSIGN+"|<|>|="));
    Omega.append(QRegExp(QString("\\+|-|")+UNARY_PLUS+"|"+UNARY_MINUS));
    Omega.append(QRegExp("\\*|/"));
    Omega.append(QRegExp(QString(KS_POWER)));


    Delim= QString("[+|=|*|\\-|/|<|>|\\(|\\)|\\[|\\]|\\,|\\:|")+UNARY_MINUS+"|"+
           KS_POWER+"|"+KS_NEQ+"|"+KS_OR+"|"+KS_AND+"|"+KS_GEQ+"|"+
           KS_LEQ+"|"+UNARY_PLUS+"|"+KS_ASSIGN+"]";

    DelimNOT= QString("[+|=|*|\\-|/|<|>|\\(|\\)|\\[|\\]|\\,|\\:|")+KS_NOT+UNARY_MINUS+"|"+
              KS_POWER+"|"+KS_NEQ+"|"+KS_OR+"|"+KS_AND+"|"+KS_GEQ+"|"+
              KS_LEQ+"|"+UNARY_PLUS+"]";

    Breaks = QObject::trUtf8("[+№$;=\\?*~`\\-\\\\/<>\\(\\)\\[\\]\\,\\:")+KS_DOC+
             UNARY_MINUS+
             KS_POWER+KS_NEQ
             +KS_OR+KS_AND
             +KS_GEQ+KS_LEQ
             +UNARY_PLUS
             +KS_POWER+KS_GEQ
             +KS_LEQ+KS_NEQ
             +KS_OR+KS_AND
             +KS_ASSIGN+KS_INT
             +KS_INT_TAB+KS_REAL
             +KS_REAL_TAB+KS_STRING
             +KS_STRING_TAB+KS_CHAR
             +KS_CHAR_TAB+KS_BOOL
             +KS_BOOL_TAB+KS_DONE
             +KS_IF+KS_THEN
             +KS_ELSE+KS_BEGIN
             +KS_END
             +KS_STEP
             +KS_LOOP+KS_ENDLOOP
             +KS_FOR+KS_TIMES
             +KS_WHILE
             +KS_ALG+KS_TO
             +KS_FROM+KS_CASE
             +KS_ARG+KS_RES+KS_ARGRES
             +KS_INPUT+KS_OUTPUT
             +KS_ASSERT+KS_PRE
             +KS_POST+KS_COND_ENDLOOP
             +KS_EXP_PLUS+KS_EXP_MINUS
             +KS_NEWLINE+KS_DOC
             +KS_AT+KS_TILDA
             +KS_PERCENT+KS_DOLLAR
             +KS_PAR+KS_DOC
             +KS_SWITCH+"]";

    operators = QRegExp(Delim);
    operatorsNOT = QRegExp(DelimNOT);
    brks = QRegExp(Breaks);
}

#include <QtXml>

QString TextAnalizer::splitLogicalExpressions(const QString &s, QStringList &exprList)
{
    QString head, tail;
    int operPos;
    operPos = s.indexOf(QChar(KS_OR));
    if (operPos==-1)
        operPos = s.indexOf(QChar(KS_AND));
    if (operPos==-1) {
        // Нет операторов "или" или "и"
        return s;
    }
    else {
        head = s.left(operPos);
        tail = s.mid(operPos+1);
        QString result;
        QString subExprId = QString::number(exprList.count());
        if (head.contains(operatorsNOT)) {
            exprList << splitLogicalExpressions(head, exprList);
            result += "#"+subExprId;
        }
        else {
            result += head;
        }
        result += s[operPos];
        subExprId = QString::number(exprList.count());
        if (tail.contains(operatorsNOT)) {
            exprList << splitLogicalExpressions(tail, exprList);
            result += "#"+subExprId;
        }
        else {
            result += tail;
        }
        return result;
    }
}

int TextAnalizer::L(QString s)
{
    // Поиск оператора из множества Ω_i в строке s
    int result = -1;
    for ( int i=0; i<Omega.count(); i++ )
    {
        result = Omega[i].lastIndexIn(s);
        // Если оператор найден в подмножестве Ω_i, то возвращаем его позицию,
        // иначе ищем в следующем подмножестве Ω_{i+1}
        if ( result != -1 )
        {
            break;
        }
    }
    return result;
};

Kumir::ValueType TextAnalizer::G(QString s,
                                 FunctionTable &table,
                                 SymbolTable &symbols,
                                 const QList<Kumir::ValueType> &type_list,
                                 const QList<StructType> &struct_list,
                                 const QStringList &expr_list,
                                 StructType &strNew,
                                 Kumir::CompileError &err,
                                 bool &include_bounds_in_error,
                                 QStringList &parsedNodes,
                                 QString &root)

{
    err.code = 0;
    err.position = 0;
    err.length = s.length();
    strNew = NoStruct;
    QString H, T; // Head, Tail. См. описание в getSimpleViraj.pdf
    int l; // Значение функции L(s). См. описание в getSimpleViraj.pdf
    include_bounds_in_error = true;
    // Есть ли выражение
    if ( s.trimmed().isEmpty() )
    {
        return Kumir::Undefined;
    }

    // Определяем, если ли оператор
    l = L(s);

    if ( l==-1 )
    {
        // Нет оператора. Возвращаем P(s) (getObjType)
        Kumir::CompileError local_error;
        Kumir::ValueType p = getObjType(s,
                                        local_error,
                                        table,
                                        symbols,
                                        type_list,
                                        struct_list,
                                        expr_list,
                                        strNew,
                                        include_bounds_in_error,
                                        parsedNodes,
                                        root);
        if ( local_error.code )
        {
            err = local_error;
            return Kumir::Undefined;
        }
        else
        {
            return p;
        }
    }
    else
    {
        QString leftXml, rightXml;
        // Есть оператор. Возвращаем R(G(H(s,L(s))),L(s),G(T(s,L(s))))
        H = s.left(l);
        T = s.right(s.length()-l-1);

        // Если левая часть выражения пустая, а оператор +, - или "не",
        // то подразумевается унарный оператор. Если другой оператор -- ошибка
        if ( H.isEmpty() )
        {
            if ( s[l] == '+' )
                s[l] = QChar(UNARY_PLUS);
            else if ( s[l] == '-' )
                s[l] = QChar(UNARY_MINUS);
            else if ( s[l] == QChar(UNARY_PLUS) )
                ;
            else if ( s[l] == QChar(UNARY_MINUS) )
                ;
            else if ( s[l] == QChar(KS_NOT) )
                ;
            else
            {
                err.code = GSV_NOT_UNARY;
                err.position = l;
                err.length = 1;
                return Kumir::Undefined;
            }
        }

        // Если обе части выражения пустые -- ошибка
        if ( H.isEmpty() && T.isEmpty() )
        {
            err.position = l;
            err.length = 1;
            err.code = GSV_ONLY_OPER;
            return Kumir::Undefined;
        }

        Kumir::ValueType leftType, rightType;
        StructType leftStruct, rightStruct;

        // Определяем типы H и T
        if ( H.isEmpty() )
        {
            leftType = Kumir::Undefined;
            leftStruct = NoStruct;
        }
        else
        {
            Kumir::CompileError localError;
            leftType = G(H,
                         table,
                         symbols,
                         type_list,
                         struct_list,
                         expr_list,
                         leftStruct,
                         localError,
                         include_bounds_in_error,
                         parsedNodes,
                         leftXml);

            // Если в левой части ошибка -- пересчитываем позицию ошибки
            if ( localError.code )
            {
                err = localError;
                return Kumir::Undefined;
            }
        }

        // Правая часть не может быть пустой,
        // т.к. в КУМИРе не предусмотрено постфиксных операторов
        if ( T.isEmpty() )
        {
            err.code = GSV_EXTRA_OPER; // "Лишний оператор"
            err.position = l;
            err.length = 1;
            return Kumir::Undefined;
        }
        else
        {
            Kumir::CompileError localError;
            rightType = G(T,
                          table,
                          symbols,
                          type_list,
                          struct_list,
                          expr_list,
                          rightStruct,
                          localError,
                          include_bounds_in_error,
                          parsedNodes,
                          rightXml);
            // Если в правой части ошибка -- пересчитываем позицию ошибки
            if ( localError.code )
            {
                err = localError;
                err.position += (l+1);
                return Kumir::Undefined;
            }
        }

        // Определяем тип тройки {p_1,λ,p_2}
        int restype_err = 0;
        Kumir::ValueType r = resType(&restype_err,leftType,rightType,s[l]);
        if ( restype_err )
        {
            err.code = restype_err;
            err.position = l;
            err.length = 1;
            return Kumir::Undefined;
        }

        strNew = Complex;


        bool isCompare = s[l]=='<'||s[l]=='>'||s[l]=='='||s[l]==KS_GEQ||s[l]==KS_LEQ||s[l]==KS_NEQ;
        bool leftIsNumeric = leftType==Kumir::Integer || leftType==Kumir::Real || leftType==Kumir::BoolOrDecimal;
        bool rightIsNumeric = rightType==Kumir::Integer || rightType==Kumir::Real || rightType==Kumir::BoolOrDecimal;
        bool leftIsLiteral = leftType==Kumir::String || leftType==Kumir::Charect || leftType==Kumir::BoolOrLiteral;
        bool rightIsLiteral = rightType==Kumir::String || rightType==Kumir::Charect || rightType==Kumir::BoolOrLiteral;
        bool isNumMulticompare = isCompare && ((leftType==Kumir::BoolOrDecimal && rightIsNumeric) || (leftIsNumeric && rightType==Kumir::BoolOrDecimal));
        bool isLitMulticompare = isCompare && ((leftType==Kumir::BoolOrLiteral && rightIsLiteral) || (leftIsLiteral && rightType==Kumir::BoolOrLiteral));
        QChar currentOperator = s[l];
        if (isNumMulticompare || isLitMulticompare) {
            leftXml = leftXml.remove("\n").trimmed();
            rightXml = rightXml.remove("\n").trimmed();

            QDomDocument l, r;
            Q_ASSERT ( l.setContent(leftXml) );
            Q_ASSERT ( r.setContent(rightXml) );

            QString leftOp, rightOp;
            QString left, right;
            QString lTag = l.documentElement().toElement().tagName();
            QString rTag = r.documentElement().toElement().tagName();
            bool leftIsMulticompare = lTag=="multicompare";
            bool rightIsMulticompare = rTag=="multicompare";
            bool leftIsOperator = lTag=="operator" && (leftType==Kumir::BoolOrDecimal || leftType==Kumir::BoolOrLiteral);
            bool rightIsOperator = rTag=="operator" && (rightType==Kumir::BoolOrDecimal || rightType==Kumir::BoolOrLiteral);
            bool leftIsSimple = !leftIsMulticompare && !leftIsOperator;
            bool rightIsSimple = !rightIsMulticompare && !rightIsOperator;


            if (leftIsOperator) {

                leftOp = l.documentElement().attribute("kind");
            }
            if (rightIsOperator) {

                rightOp = r.documentElement().attribute("kind");
            }

            QDomDocument s;
            QStringList resOperators;
            QStringList resOperands;

            if (leftIsMulticompare) {
                QDomNode l_ops = l.elementsByTagName("operators").at(0);
                QDomNode c = l_ops.toElement().firstChild();
                while (!c.isNull()) {
                    if (c.isElement()&&c.toElement().tagName()=="compare") {
                        QString kind = c.toElement().attribute("kind");
                        kind.replace("<","&lt;");
                        kind.replace(">","&gt;");
                        resOperators << "<compare kind='"+kind+"' />";
                    }
                    c = c.nextSibling();
                }
            }
            if (leftIsOperator) {
                QString kind = leftOp;
                kind.replace("<","&lt;");
                kind.replace(">","&gt;");
                resOperators << "<compare kind='"+kind+"' />";
            }

            resOperators << "<compare kind='"+xmlOperatorName(currentOperator)+"' />";
            if (rightIsMulticompare) {
                QDomNode r_ops = r.elementsByTagName("operators").at(0);
                QDomNode c = r_ops.toElement().firstChild();
                while (!c.isNull()) {
                    if (c.isElement()&&c.toElement().tagName()=="compare") {
                        QString kind = c.toElement().attribute("kind");
                        kind.replace("<","&lt;");
                        kind.replace(">","&gt;");
                        resOperators << "<compare kind='"+kind+"' />";
                    }
                    c = c.nextSibling();
                }
            }
            if (rightIsOperator) {
                QString kind = rightOp;
                kind.replace("<","&lt;");
                kind.replace(">","&gt;");
                resOperators << "<compare kind='"+kind+"' />";
            }

            if (leftIsMulticompare) {
                QDomNode l_ops = l.elementsByTagName("operands").at(0);
                for (int j=0; j<l_ops.childNodes().count(); j++ ) {
                    QDomNode c = l_ops.childNodes().at(j);
                    QString s;
                    QTextStream ts(&s);
                    ts << c;
                    resOperands << s;
                }
            }
            if (leftIsOperator) {
                QDomNode l_ops = l.documentElement();
                for (int j=0; j<l_ops.childNodes().count(); j++ ) {
                    QDomNode c = l_ops.childNodes().at(j);
                    QString s;
                    QTextStream ts(&s);
                    ts << c;
                    resOperands << s;
                }
            }
            if (leftIsSimple) {
                resOperands << leftXml;
            }
            if (rightIsMulticompare) {
                QDomNode r_ops = r.elementsByTagName("operands").at(0);
                for (int j=0; j<r_ops.childNodes().count(); j++ ) {
                    QDomNode c = r_ops.childNodes().at(j);
                    QString s;
                    QTextStream ts(&s);
                    ts << c;
                    resOperands << s;
                }
            }
            if (rightIsOperator) {
                QDomNode r_ops = r.documentElement();
                for (int j=0; j<r_ops.childNodes().count(); j++ ) {
                    QDomNode c = r_ops.childNodes().at(j);
                    QString s;
                    QTextStream ts(&s);
                    ts << c;
                    resOperands << s;
                }
            }
            if (rightIsSimple) {
                resOperands << rightXml;
            }
            Q_ASSERT ( resOperands.count() == resOperators.count()+1 );
            QString str = "<multicompare>\n<operators>\n"+resOperators.join("\n")+"</operators>\n<operands>"+resOperands.join("\n")+"</operands></multicompare>\n";
            //				qDebug() << str;
            root += str;

        }
        else {
            root += "<operator type='"+xmlTypeName(r)+"' kind='"+xmlOperatorName(s[l])+"'>\n";
            root += leftXml + rightXml;
            root += "</operator>\n";
        }

        return r;

    }

}


/*MR-New
        1) Proveriaet semantiku opisatelia ob'ekta i
        2) opredeliaet ego tip

         Format opisatelya:
        <DS_VAR><SYM_adr>									- simple var or const or Array;
                <DS_ARR><SYM_adr> {<DS_VYR><VYR_adr>}**n			- elem of Array (n >0)
        <DS_STR><SYM_adr><DS_VYR><VYR_adr>					- elem of string
        <DS_STR><SYM_adr><DS_VYR><VYR_adr><DS_VYR><VYR_adr> - fragm of string
        <DS_FUN><FUN_adr> {<DS_VYR><VYR_adr>}**f			- function call (f >=0)

        Input-output patameters - see below
*/

void getObjType_findArgErrPos(const QString &s, const int argNo, int &err_start, int &err_end, const int spaces=0)
{
    QRegExp rxArg("&\\d+");
    err_start = -1;
    int curArg = -1;
    forever {
        err_start = rxArg.indexIn(s,err_start+1);
        if ( err_start != -1 ) {
            err_end = err_start + rxArg.matchedLength() -1 ;
            curArg++;
            if ( curArg == argNo )
                break;
        }
        else {
            break;
        }
    };
    for ( int i=err_end+1; i<s.length(); i++ ) {
        if ( s[i]==' ' )
            err_end++;
        else
            break;
    }
    err_start += spaces;
    err_end += spaces;
}

QList<int> getObjType_extractExprIds(const QString &s)
{
    QRegExp rxId("&(\\d+)");
    int start_pos = -1;
    QList<int> result;
    bool OK;
    forever {
        start_pos = rxId.indexIn(s,start_pos+1);
        if ( start_pos != -1 ) {
            result << rxId.cap(1).toInt(&OK);
            Q_ASSERT ( OK );
        }
        else {
            break;
        }
    }
    return result;
}

bool isTableType(Kumir::ValueType t) {
    return t==Kumir::BooleanArray || t==Kumir::CharectArray || t==Kumir::IntegerArray || t==Kumir::RealArray || t==Kumir::StringArray;
}

bool TextAnalizer::isRecursive(QString block, QStringList exprList, int moduleNo, int funcNo, bool firstCall, int &exprNo)
{
    QString dbg = block;
    bool result = false;

    if (!firstCall)
        result = block.indexOf("@"+QString::number(funcNo)+"|"+QString::number(moduleNo))>-1;
    QRegExp rxArgs = QRegExp("&\\d+");
    if ( result )
        return result;
    int p = -1;
    forever {
        p = rxArgs.indexIn(block,p+1);
        if ( p>-1 ) {
            QString part = block.mid(p+1,rxArgs.matchedLength()-1);
            int no = part.toInt();
            result = result || isRecursive(exprList[no],exprList,moduleNo,funcNo,false,exprNo);
            if ( result ) {
                exprNo = no;
                return result;
            }
            p++;
        }
        else {
            break;
        }
    }
    return result;
}

bool getObjType_isSimpleVar(int id, const QStringList &exprList, const SymbolTable &symbols)
{
    QString expr = exprList[id].trimmed();
    bool isVarOrConst = QRegExp("\\^\\d+").exactMatch(expr);
    if ( isVarOrConst ) {
        expr.remove(0,1);
        bool OK;
        int sId = expr.toInt(&OK);
        Q_ASSERT(OK);
        return !symbols.symb_table[sId].is_const;
    }
    else {
        return false;
    }
}


Kumir::ValueType TextAnalizer::getObjType(QString work_block,
                                          Kumir::CompileError &err,
                                          FunctionTable &table,
                                          SymbolTable &symbols,
                                          QList<Kumir::ValueType> type_List,
                                          QList<StructType> struct_List,
                                          QStringList exprList,
                                          StructType &StrNew,
                                          bool &include_bounds_in_error,
                                          QStringList &parsedNodes, QString &root)
{
    int spacesAfter = 0;
    for ( int i=work_block.length()-1; i>=0; i-- )
    {
        if ( work_block[i]==' ' )
            spacesAfter++;
        else
            break;
    }
    int spaces = 0;
    while ( work_block.startsWith ( " " ) )
    {
        spaces++;
        work_block.remove ( 0,1 );
    }
    work_block = work_block.trimmed();

    err.code = 0;
    err.position = spaces;
    err.length= work_block.length();

    Kumir::ValueType viraj_type;

    viraj_type=Kumir::Undefined;
    int p_id;

    //	1. Var or Const or 0-ary function or Array (as a whole)
    if ( work_block[0]=='^' )
    {
        bool OK;
        p_id=work_block.mid ( 1 ).toInt ( &OK );
        Q_ASSERT ( OK && ( p_id >= 0 ) );

            viraj_type=symbols.getTypeByID ( p_id );
            //MR-New
            StrNew = Variable;
            if ( symbols.isConst ( p_id ) )
            {
                StrNew = Constant;
            }
            if ( ( viraj_type == Kumir::IntegerArray ) ||
                 ( viraj_type == Kumir::CharectArray ) ||
                 ( viraj_type == Kumir::StringArray ) ||
                 ( viraj_type == Kumir::RealArray ) ||
                 ( viraj_type == Kumir::BooleanArray )
                )
                {
                StrNew = Array;
            }

                if (StrNew==Constant) {
                    root += "<Constant encoded='base64' type='"+xmlTypeName(viraj_type)+"'>";
                    QString base64;
                    if (viraj_type==Kumir::Integer)
                        base64 = xmlEncodeConstantData(symbols.symb_table[p_id].value->getValueAsInt());
                    if (viraj_type==Kumir::Real)
                        base64 = xmlEncodeConstantData(symbols.symb_table[p_id].value->getValueAsFloat());
                    if (viraj_type==Kumir::Boolean)
                        base64 = xmlEncodeConstantData(symbols.symb_table[p_id].value->getValueAsBool());
                    if (viraj_type==Kumir::Charect)
                        base64 = xmlEncodeConstantData(symbols.symb_table[p_id].value->getValueAsCharect());
                    if (viraj_type==Kumir::String)
                        base64 = xmlEncodeConstantData(symbols.symb_table[p_id].value->getValueAsString());
                    root += base64;
                    root += "</Constant>\n";
                }
                else if (symbols.symb_table[p_id].is_FunctionReturnValue) {
                    root += "<return_value/>\n";
                }
                else {
                    root += "<value type='"+xmlTypeName(viraj_type)+"' id='"+QString::number(p_id)+"'/>\n";
                }
    }

    //     2. Array's element
    //  2.1 Razbiraemsia s imenem
    if ( work_block[0]=='%' )
    {
        StrNew = ArrayElement;  //MR-New
        int diez_pos = work_block.indexOf ( '&' );
        if ( diez_pos == -1 )
        {
            // No arguments
            err.code = GOT_ARRAY_1;
            return Kumir::Undefined;
        }
        bool OK;			// Uneseno vverh
        p_id = work_block.mid ( 1, diez_pos-1 ).toInt ( &OK );
        Q_ASSERT ( ( !OK ) || ( p_id<0 ) );

        //  2.2  Razbiraemsia s argumentami
        //     kolichestvo
        int diez_count = work_block.count ( '&' );
        int mustBe_count = symbols.symb_table[p_id].razm;
        if ( diez_count != mustBe_count )
        {
            err.position = diez_pos+spaces;
            err.length = work_block.length()-spaces-diez_pos;
            err.code = GOT_ARRAY_2;
            return Kumir::Undefined;
        }
        int cur_diez_pos, old_diez_pos;
        old_diez_pos = diez_pos;
        int viraj_id = -1;


        //			tip kazhdogo argumenta - Kumir::Integer
        QList<int> refs;
        for ( int i=0; i<diez_count; i++ )
        {
            QString cap;
            cur_diez_pos = work_block.indexOf ( '&', old_diez_pos+1 );
            if ( cur_diez_pos == -1 )
                cap = work_block.mid ( old_diez_pos+1 );
            else
                cap = work_block.mid ( old_diez_pos+1, cur_diez_pos-old_diez_pos-1 );
            viraj_id = cap.toInt(&OK);
            refs << viraj_id;
            err.position = old_diez_pos+spaces;
            err.length = cur_diez_pos==-1? work_block.length()-old_diez_pos:cur_diez_pos-old_diez_pos;
            old_diez_pos = cur_diez_pos;
            Q_ASSERT ( ( !OK ) || ( viraj_id<0 ) );
            if ( type_List[viraj_id] != Kumir::Integer )
            {
                err.code = GOT_ARRAY_3;
                return Kumir::Undefined;
            }

        }

        int mass_type = symbols.getTypeByID ( p_id );
        if ( mass_type == Kumir::IntegerArray )
            viraj_type = Kumir::Integer;
        if ( mass_type == Kumir::BooleanArray )
            viraj_type = Kumir::Boolean;
        if ( mass_type == Kumir::RealArray )
            viraj_type = Kumir::Real;
        if ( mass_type == Kumir::StringArray )
            viraj_type = Kumir::String;
        if ( mass_type == Kumir::CharectArray )
            viraj_type = Kumir::Charect;



            root += "<Array_element id='"+QString::number(p_id)+"' type='"+xmlTypeName(viraj_type)+"'>\n";
            for (int i=0; i<diez_count; i++) {
                root += parsedNodes.at(refs[i]);
            }
            root += "</Array_element>\n";

    } // end if (work_block[0]=='%')

    // NEW V.Y.

    //		3. STROKA s [] - t.e. obrashchenie k symbolu ili vyrezka
    //  3.1 Razbiraemsia s imenem

    if ( work_block[0]==QChar ( TYPE_SKOBKA ) )
    {
        int diez_pos = work_block.indexOf ( '&' );
        if ( diez_pos == -1 )
        {
            err.code = GOT_STRING_1;
            return Kumir::Undefined;
        }
        bool OK;

        p_id = work_block.mid ( 1, diez_pos-1 ).toInt ( &OK );
        Q_ASSERT ( ( !OK ) || ( p_id<0 ) );


        //  3.2  Razbiraemsia s argumentami
        //     kolichestvo
        int diez_count = work_block.count ( '&' );

        if ( diez_count > 2 )
        {
            err.position = diez_pos+spaces;
            err.length = work_block.length()-err.position;
            err.code = GOT_STRING_2;
            return Kumir::Undefined;
        }
        else
        {

            if ( diez_count == 1 )  // Kumir::Charect s[a]
            {
                viraj_type = Kumir::Charect;
                StrNew = StringElement;
            }
            else  // string s[a:b]
            {
                viraj_type = Kumir::String;
                StrNew = Substring;
            }

        }
        int cur_diez_pos, old_diez_pos = diez_pos;
        int viraj_id = -1;
        QList<int> refs;
        QString dbg = work_block;
        for ( int i=0; i<diez_count; i++ )
        {
            cur_diez_pos = work_block.indexOf ( '&', old_diez_pos+1 );
            if ( cur_diez_pos == -1 )
                viraj_id = work_block.mid ( old_diez_pos+1 ).toInt ( &OK );
            else
                viraj_id = work_block.mid ( old_diez_pos+1, cur_diez_pos-old_diez_pos-1 ).toInt ( &OK );
            refs << viraj_id;
            Q_ASSERT ( ( !OK ) || ( viraj_id<0 ) );
            if ( type_List[viraj_id] != Kumir::Integer )
            {
                err.position = old_diez_pos+spaces;
                err.length = cur_diez_pos==-1? work_block.length()-err.position : cur_diez_pos-old_diez_pos;
                err.code = GOT_STRING_3;
                return Kumir::Undefined;
            }
            old_diez_pos = cur_diez_pos;
        } // end for ( int i=0; i<diez_count; i++ )

            int id;
            if (symbols.symb_table[p_id].is_FunctionReturnValue)
                id = -1;
            else
                id = p_id;

            if (StrNew==StringElement) {
                root += "<StringElementent id='"+QString::number(id)+"'>\n";
                root += parsedNodes.at(refs[0]);
                root += "</StringElementent>\n";
            }
            if (StrNew==Substring) {
                root += "<string_slice id='"+QString::number(id)+"'>\n";
                root += parsedNodes.at(refs[0]);
                root += parsedNodes.at(refs[1]);
                root += "</string_slice>\n";
            }


    } // end if ( work_block[0]==QChar(TYPE_SKOBKA) )

    // end NEW V.Y.

    //		4. Vyrazhenie
    if ( work_block[0]=='&' )
    {
        StrNew = Complex;		//MR-New
        bool OK;
        p_id=work_block.mid ( 1 ).toInt ( &OK );
        if ( ( OK ) && ( p_id>=0 ) )
            viraj_type=type_List[p_id];
        else
        {
            err.code = GOT_BAD_SKOBK;
            return Kumir::Undefined;
        }
        if ( viraj_type == Kumir::BoolOrLiteral || viraj_type==Kumir::BoolOrDecimal )
            viraj_type = Kumir::Boolean;

            root += "<expression type='"+xmlTypeName(viraj_type)+"'>\n";
            root += parsedNodes.at(p_id);
            root += "</expression>\n";

    }

    //		5. Vyzov funccii
    if ( work_block[0]=='@' )
    {
        StrNew = Function;		//MR-New
        QRegExp rxFunctionCall ( "@(\\d+)\\|(\\d+)\\s*" );
        rxFunctionCall.indexIn ( work_block );
        bool OK;
        int funcId = rxFunctionCall.cap ( 1 ).toInt ( &OK );
        Q_ASSERT ( OK );
        int moduleId = rxFunctionCall.cap ( 2 ).toInt ( &OK );
        Q_ASSERT ( OK );
        QList<int> factParameters = getObjType_extractExprIds ( work_block );
        int factParametersCount = factParameters.count();
        int formalParametersCount = table.argCountById ( funcId );

        if ( factParametersCount != formalParametersCount )
        {
            err.code =GOT_FUNC_1;
            err.position = work_block.indexOf("&")+spaces;
            err.length = work_block.length()-err.position;
            include_bounds_in_error = false;
            return Kumir::Undefined;
        }

        int recExprNo = -1;
        bool recursive = isRecursive ( work_block,exprList,moduleId,funcId,true,recExprNo );
        bool tableArgs = false;

        for ( int i=0; i<factParametersCount; i++ )
                {
                    int exprId = factParameters[i];
                    Q_ASSERT ( OK );
                    Q_ASSERT ( exprId >= 0 );
                    Q_ASSERT ( exprId < type_List.count() );
                    Q_ASSERT ( exprId < exprList.count() );
                    Q_ASSERT ( exprId < struct_List.count() );
                    Kumir::ValueType factType = type_List[exprId];
                    Kumir::ValueType formalType = table.argTypeById(funcId, i);
                    err.position = -1;
                    for ( int j = -1; j<i; j++ ) {
                        err.position = work_block.indexOf("&",err.position+1);
                    }
                    err.position += spaces;
                    err.length = QString::number(exprId).length()+1;
                    if ( factType != formalType )
                    {
                        if ( formalType==Kumir::Real && factType==Kumir::Integer ) {
                            bool res = table.isRes(funcId, i);
                            bool argRes = table.isArgRes(funcId, i);
                            if ( res || argRes ) {
                                err.code = GOT_TYPE_MISS;
                                include_bounds_in_error = false;
                                return Kumir::Undefined;
                            }
                        }
                        else if ( formalType==Kumir::String && factType==Kumir::Charect ) {
                            bool res = table.isRes(funcId, i);
                            bool argRes =table.isArgRes(funcId, i);
                            if ( res || argRes ) {
                                err.code = GOT_TYPE_MISS;
                                include_bounds_in_error = false;
                                return Kumir::Undefined;
                            }
                        }
                        else
                        {
                            err.code = GOT_TYPE_MISS;
                            include_bounds_in_error = false;
                            return Kumir::Undefined;
                        }
                    }
                    if ( isTableType ( formalType ) )
                    {
                        tableArgs = true;
                        int symbId = exprList[exprId].trimmed().remove ( "^" ).toInt ( &OK );
                        Q_ASSERT ( OK );
                        int factDim = symbols.getRazmById ( symbId );
                        int formalDim = table.argDimById(funcId, i);
                        if ( factDim != formalDim )
                        {
                            err.code = GOT_BAD_DIM;
                            return Kumir::Undefined;
                        }
                    }
                    bool res =table.isRes(funcId, i);
                    bool argRes = table.isArgRes(funcId, i);
                    if ( res || argRes )
                    {
                        bool isSimple = getObjType_isSimpleVar ( exprId,exprList,symbols );
                        if ( !isSimple )
                        {
                            err.code = GOT_RES_PARAM_NOT_SIMPLE;
                            return Kumir::Undefined;
                        }
                        int symbId = exprList[exprId].trimmed().remove ( "^" ).toInt ( &OK );
                        Q_ASSERT ( OK );
                        bool isArg = symbols.symb_table[symbId].is_Arg;
                        if ( isArg && !symbols.symb_table[symbId].is_Res && !symbols.symb_table[symbId].is_ArgRes)
                        {
                            err.code = res? GOT_ARG_PARAM_AS_RES : GOT_ARG_PARAM_AS_ARGRES;
                            return Kumir::Undefined;
                        }
                    }
                    if ( recursive && tableArgs )
                    {
                        err.code = GOT_RECURSIVE_TABLE;
                        QRegExp rxSubExpr = QRegExp ( "&"+QString::number ( recExprNo ) +"\\s*" );
                        // TODO выделение ошибки
                        //                              err_start = spaces + rxSubExpr.indexIn(work_block);
                        //                              err_end = err_start + rxSubExpr.matchedLength();
                        return Kumir::Undefined;

                    }
                    if ( table.isRes(funcId, i) ||
                         table.isArgRes(funcId, i) )
                    {
                        StructType factStructType = struct_List[exprId];
                        if ( factStructType != Variable &&
                             factStructType != ArrayElement &&
                             factStructType != Array )
                        {
                            err.code = GOT_BAD_RES;
                            return Kumir::Undefined;
                        }
                    }
                }


        viraj_type = m_modulesPool->functionTable(moduleId)->typeById(funcId);

            root += "<call module='"+m_modulesPool->moduleName(moduleId)+
                     "' algorhitm='"+table.nameById(funcId)+
                     "' type='"+xmlTypeName(viraj_type)+"'";
            if (factParameters.count()==0)
               root += " />\n";
            else {
              root += ">\n";
                for (int i=0; i<factParameters.count(); i++ ) {
                    root += parsedNodes.at(factParameters[i]);
                }
                root += "</call>\n";
            }

    }
    return viraj_type;
}


Kumir::CompileError TextAnalizer::parceThen(const QString &instr)
{
    Kumir::CompileError result;
    result.position =  1;
    result.length =  instr.length()-1;
    result.code = instr.trimmed().length()>1? TO_TRASH : 0;
    return result;
}

Kumir::CompileError TextAnalizer::parceElse(const QString &instr)
{
    Kumir::CompileError result;
    result.position =  1;
    result.length =  instr.length()-1;
    result.code = instr.trimmed().length()>1? INACHE_TRASH : 0;
    return result;
}

Kumir::CompileError TextAnalizer::parceDone(const QString &instr)
{
    Kumir::CompileError result;
    result.position =  1;
    result.length =  instr.length()-1;
    result.code = instr.trimmed().length()>1? VSE_TRASH : 0;
    return result;
}

Kumir::CompileError TextAnalizer::parceEndLoop(const QString &instr)
{
    Kumir::CompileError result;
    result.position =  1;
    result.length =  instr.length()-1;
    result.code = instr.trimmed().length()>1? KC_TRASH : 0;
    return result;
}

Kumir::CompileError TextAnalizer::parceEnd(const QString &instr)
{
    Kumir::CompileError result;
    result.position =  1;
    result.length =  instr.length()-1;
    result.code = instr.trimmed().length()>1? KON_TRASH : 0;
    return result;
}

Kumir::CompileError TextAnalizer::parceAlgorhitmBegin(const QString &instr)
{
    Kumir::CompileError result;
    result.position =  1;
    result.length =  instr.length()-1;
    result.code = instr.trimmed().length()>1? NACH_TRASH : 0;
    return result;
}

Kumir::CompileError TextAnalizer::parceExit(const QString &instr)
{
    Kumir::CompileError result;
    result.position =  1;
    result.length =  instr.length()-1;
    result.code = instr.trimmed().length()>1? EXIT_TRASH : 0;
    return result;
}

Kumir::CompileError TextAnalizer::parceEndModule(const QString &instr)
{
    Kumir::CompileError result;
    result.position =  1;
    result.length =  instr.length()-1;
    result.code = instr.trimmed().length()>1? KON_TRASH : 0;
    return result;
}


Kumir::CompileError TextAnalizer::parceImport(const QString &instr_, QString &returnName)
{
    QString instr = instr_.mid(1);
    instr.remove(0,1);
    instr = instr.trimmed();
    Kumir::CompileError result;

    if ( instr.isEmpty() ) {
        result.position = 0;
        result.length = 1;
        result.code = ISP_NO_NAME;
        return result;
    }
    int tn_start = 0, tn_len = 0;
    int err = m_textNormalizer->test_name(instr,tn_start,tn_len);
    if ( err > 0 )
    {
        result.position = 1 + tn_start;
        result.length = tn_len;
        result.code = err;
        return result;
    }
    ispName = instr;
    returnName = ispName;
    result.code = 0;
    return result;
}



/**
 * Выполняет разбор описание группы величин.
 * @param source нормализованная строка описания группы величин
 * @param ispName имя исполнителя, к которому относится описание
 * @param algName имя алгоритма, к которому относится описание
 * @param mod модификатор доступа группы величин
 * @param str номер строки, где встретилось описание
 * @param symbols ссылка на таблицу символов
 * @param functions ссылка на таблицу функций
 * @param err_pos позиция ошибки при разборе
 * @param ids список id успешно разобранных величин
 * @param exprList список выражений для вычисления границ массивов
 * @return код ошибки или 0, если успешно
 */

Kumir::CompileError TextAnalizer::parceVariables( const QString &src,
                                  const QString &algName,
                                  ArgModifer mod,
                                  int str,
                                  SymbolTable &symbols,
                                  FunctionTable &functions,
                                  QList<int> & ids,
                                  QList<QStringList> & exprList,
                                  QStringList & extraSubExpressions,
                                  QStringList &xmlList,
                                  bool isArg,
                                  QList<KumirInstruction> &extraProgram
                                  )
{
    Kumir::CompileError result;
    result.code = 0;
    QString source = src;
    while ( source.endsWith(" ") )
    {
        source.remove(source.length()-1,1);
    }
    source += ";";

    //  Режим работы автомата-парсера.
    //  type -- разбор типа
    //  name -- разбор имени переменной
    //  tn -- разбор типа или имени
    //  tab -- разбор массива
    //  lbN -- разбор левой границы
    //  lbN -- разбор правой границы
    //  value -- разбор константного значения

    enum Parcing { type, name, tn, lb, rb, value };

    //  Состояния автомата
    Parcing par = type; // режим разбора
    Kumir::ValueType cType = Kumir::Undefined; // тип текущей величины
    QString cName = ""; // имя текущей величины
    QString cBound = ""; // текущая граница массива
    QString initValue = ""; // значение инициализации
    int deepV = 0; // глубина вложенности {}
    int deep1 = 0; // глубина вложенности [] внутри правой границы
    int deep2 = 0; // глубина вложенности () внутри правой границы
    int dim = 0;
    bool massDeclared = false;
    int currentID = -1;

    // Переменные цикла:
    // i - номер символа в анализируемой строке
    // p - начало имени переменной
    // q - начало границы массива (символ [)
    // s - начало правой граници массива (символ :)
    // t - позиция ксимвола типа данных
    // z - позиция начала константы
    int z = 0;

    for ( int curPos=0, nameStart=0, ArrayStart=0, ArrayBoundStart=0, typePos=0; curPos<source.length(); curPos++ )
    {

        //      Предполагается, что модификатор группы уже задан, поэтому
        //      если встречается арг, рез или аргрез, то это ошибка

        if (
                ( source[curPos] == QChar(KS_ARG) )
                ||
                ( source[curPos] == QChar(KS_RES) )
                ||
                ( source[curPos] == QChar(KS_ARGRES) )
                )
        {
            result.position = curPos;
            result.length = 1;
            if ( source[curPos] == QChar(KS_ARG) )
                result.code = PV_EXTRA_ARG; return result;
            if ( source[curPos] == QChar(KS_RES) )
                result.code = PV_EXTRA_RES; return result;
            if ( source[curPos] == QChar(KS_ARGRES) )
                result.code = PV_EXTRA_ARGRES; return result;
        }
        else if ( ( par == type ) || ( par == tn ) )
        {
            if ( source[curPos] == QChar(' ')  )
            {
                nameStart++;
                continue;
            }
            //          Разбираемся с типом переменных
            if (
                    ( source[curPos] == QChar(KS_INT) )
                    ||
                    ( source[curPos] == QChar(KS_REAL) )
                    ||
                    ( source[curPos] == QChar(KS_BOOL) )
                    ||
                    ( source[curPos] == QChar(KS_STRING) )
                    ||
                    ( source[curPos] == QChar(KS_CHAR) )
                    ||
                    ( source[curPos] == QChar(KS_INT_TAB) )
                    ||
                    ( source[curPos] == QChar(KS_REAL_TAB) )
                    ||
                    ( source[curPos] == QChar(KS_BOOL_TAB) )
                    ||
                    ( source[curPos] == QChar(KS_STRING_TAB) )
                    ||
                    ( source[curPos] == QChar(KS_CHAR_TAB) )
                    )
            {
                if ( source[curPos] == QChar(KS_INT) )
                    cType = Kumir::Integer;
                else if ( source[curPos] == QChar(KS_REAL) )
                    cType = Kumir::Real;
                else if ( source[curPos] == QChar(KS_BOOL) )
                    cType = Kumir::Boolean;
                else if ( source[curPos] == QChar(KS_STRING) )
                    cType = Kumir::String;
                else if ( source[curPos] == QChar(KS_CHAR) )
                    cType = Kumir::Charect;
                else if ( source[curPos] == QChar(KS_INT_TAB) )
                    cType = Kumir::IntegerArray;
                else if ( source[curPos] == QChar(KS_REAL_TAB) )
                    cType = Kumir::RealArray;
                else if ( source[curPos] == QChar(KS_BOOL_TAB) )
                    cType = Kumir::BooleanArray;
                else if ( source[curPos] == QChar(KS_STRING_TAB) )
                    cType = Kumir::StringArray;
                else
                    cType = Kumir::CharectArray;
                //              Переходим в режим поиска имени
                par = name;
                cName = "";
                nameStart = curPos+1;
                typePos = curPos;
                continue;
            }
            else if ( par == tn )
                par = name;
            else
            {
                if ( typePos != 0 ) {
                    result.position = typePos;
                    result.length = 1;
                }
                else {
                    result.position = 0;
                    result.length = curPos;
                }
                result.code = PV_NO_TYPE; return result; // не указан тип переменной
            }
        } // endif ( par == type || par == tn )
        if ( par == name )
        {
            if ( ( source[curPos] == ' ' ) && cName.isEmpty() )
            {
                nameStart++;
                continue;
            }
            if ( source[curPos] == ']' )
            {
                result.position = curPos;
                result.length = 1;
                result.code = PV_NO_OPEN_QB; return result;
            }
            if ( source[curPos] == '[' )
            {
                if ( massDeclared )
                {
                    result.position = curPos;
                    result.length = 1;
                    result.code = PV_BOUND_REDECL; return result;
                }
                if ( cName.trimmed().isEmpty() )
                {
                    result.position = typePos;
                    result.length = 1;
                    result.code = PV_NO_NAME; return result; // нет имени переменной
                }
                else
                {
                    if ( (cType == Kumir::Integer) || (cType == Kumir::Real) || (cType == Kumir::Boolean) || (cType == Kumir::String) || (cType == Kumir::Charect) )
                    {
                        result.position = curPos;
                        int e = source.indexOf("]",curPos);
                        if ( e == -1 )
                            result.length = source.length()-curPos-1;
                        else
                            result.length = e-curPos+1;
                        result.code = PV_EXTRA_BOUNDS; return result; // границы не нужны
                    }
                    else
                    {
                        cBound = "";
                        par = lb;
                        ArrayStart = curPos;
                        ArrayBoundStart = curPos;
                    }
                }
            } // if ( '[' )
            else if ( source[curPos] == KS_ASSIGN )
            {
                result.position = curPos;
                result.length = 1;
                result.code = PV_ASSIGNMENT; return result;
            }
            else if ( (source[curPos] == ',') || (source[curPos] == ';') || ( source[curPos]=='=' ) )
            {

                if ( cName.trimmed().isEmpty() )
                {
                    result.position = typePos;
                    result.length = 1;
                    result.code = PV_NO_NAME; return result;
                }



                int loc_err, err_start =0, err_len =0;
                loc_err = m_textNormalizer->test_name(cName,err_start,err_len);
                if ( loc_err > 0 )
                {
                    result.position = nameStart+err_start;
                    result.length = err_len;
                    result.code = loc_err;
                    return result; // ошибка в имени
                }
                if ( cName.count("\"") || cName.count("'") )
                {
                    QRegExp rxQuot = QRegExp("[\"']");
                    int st = rxQuot.indexIn(cName);
                    int en = rxQuot.lastIndexIn(cName);
                    result.position = nameStart+st;
                    result.length = st==en? 1 : en-st+1;
                    result.code = PV_LITCONST; return result;
                }
                cName = cName.trimmed();
                result.position = nameStart;
                for ( int j=nameStart; j<curPos; j++ ) {
                    if ( source[j]==' ' )
                        result.position++;
                    else
                        break;
                }
                result.length = curPos-nameStart;
                for ( int j=curPos-1; j>nameStart; j-- ) {
                    if ( source[j]==' ' )
                        result.length--;
                    else
                        break;
                }


                if ( cName == QObject::trUtf8("знач") )
                    result.code = PV_ZNACH_NAME; return result;
                if ( symbols.inTable(cName,algName) != -1 )
                    result.code = PV_REDECLARATION; return result; // переменная уже объявлена
                //              if ( symbols.inTable(cName,"global",ispId) != -1 )

                int func_id=-1;
                int module_id=-1;
                QPair<int,int> aaa = m_modulesPool->funcByName(cName);
                module_id = aaa.first;
                func_id = aaa.second;
                if ( func_id != -1 )
                    result.code = PV_REDECLARATION_ALG; return result;
                if (
                        ! ( (cType == Kumir::Integer) || (cType == Kumir::Real) || (cType == Kumir::Boolean) || (cType == Kumir::String) || (cType == Kumir::Charect) )
                        &&
                        ! massDeclared
                        )
                {
                    result.position = nameStart;
                    result.length = curPos-nameStart;
                    result.code = PV_NO_BOUNDS; return result; // нет границ
                }
                loc_err = symbols.add_symbol(cName, // имя переменной
                                              str, // номер строки
                                              cType, // тип переменной
                                              algName,
                                              isArg && (mod==arg) // константа, если "арг"
                                              );
                if ( loc_err )
                {
                    result.code = 999999; return result; // TODO error code
                }
                int id = symbols.count;
                symbols.symb_table[id].is_Res = mod==res;
                symbols.symb_table[id].is_ArgRes = mod==argres;
                symbols.symb_table[id].used = false;
                symbols.symb_table[id].is_const = false;
                symbols.symb_table[id].is_Arg = isArg;
                symbols.symb_table[id].bounds_xml = xmlList;
                xmlList.clear();

                if (  (cType == Kumir::Integer) || (cType == Kumir::Real) || (cType == Kumir::Boolean) || (cType == Kumir::String) || (cType == Kumir::Charect) )
                {
                    symbols.new_symbol(id);
                    symbols.symb_table[id].razm = 0;
                }
                else
                {
                    symbols.symb_table[id].razm = dim;
                }
                currentID = id;
                ids << id;
                cName = "";
                par = tn;
                dim = 0;
                massDeclared = false;
                nameStart = curPos+1;
                if ( source[curPos]=='=' ) {
                    par = value;
                    z = curPos+1;
                }
            }
            else
            {
                cName += source[curPos];
            }
        } // endif ( par == name || par == tn )
        else if ( par == lb )
        {
            if ( source[curPos] == '[' )
            {
                deep1 ++;
                cBound += '[';
            }
            else if ( source[curPos] == '(' )
            {
                deep2 ++;
                cBound += '(';
            }
            else if ( ( source[curPos] == ']' ) && (deep1 > 0) )
            {
                deep1 --;
                cBound += ']';

            }
            else if ( source[curPos] == ')' )
            {
                deep2 --;
                cBound += ')';
            }
            else if ( source[curPos] == ',' && deep2==0 && deep1 == 0 )
            {
                if ( cBound.trimmed().isEmpty() )
                {
                    result.position = ArrayStart;
                    result.length = curPos-ArrayStart;
                    result.code = PV_NO_LEFT_BOUND; return result;
                }
                else
                {
                    result.position = curPos;
                    result.length = 1;
                    result.code = PV_COMMA_INSTEAD_SEMICOLON; return result;
                }
            }
            //          Разбор левой границы массива
            else if ( source[curPos] == ';' )
            {
                result.position = ArrayStart;
                result.length = 1;
                result.code = PV_NO_CLOSED_QB; return result;
            }
            else if ( source[curPos] == ':' )
            {
                QStringList el;

                if ( cBound.trimmed().isEmpty() )
                {
                    result.position = ArrayStart;
                    result.length = curPos-ArrayStart;
                    result.code = PV_NO_LEFT_BOUND; return result;
                }
                QString leftXml; StructType st;
                Kumir::ValueType leftType = parceExpression(cBound.trimmed(),
                                                            functions,
                                                            symbols,
                                                            algName,
                                                            el,
                                                            st,
                                                            result,
                                                            extraSubExpressions,
                                                            leftXml);
                if ( result.code )
                {
                    result.position += ArrayBoundStart +1;
                    return result; // ошибка разбора левой границы
                }
                if ( leftType != Kumir::Integer )
                {
                    result.position = ArrayBoundStart;
                    result.length = curPos - ArrayBoundStart;
                    result.code = PV_BAD_LEFT; return result; // левая граница -- не целое число
                }
                xmlList << leftXml;
                QString pattern = "\\b"+cName+"\\b";
                if ( !algName.isEmpty() )
                    pattern += "|\\b"+algName+"\\b";
                QRegExp rxRecursion = QRegExp(pattern);
                int rxPos = rxRecursion.indexIn(cBound,0);
                if ( rxPos != -1 )
                {
                    int rxLen = rxRecursion.matchedLength();
                    result.position = ArrayBoundStart+rxPos;
                    result.length = rxLen;
                    result.code = PV_RECURSION_L; return result; // рекурсивное определение массива
                }
                exprList << el;
                cBound = "";
                par = rb;
                ArrayBoundStart = curPos;
            }
            else if ( source[curPos] == ']' && deep1==0 )
            {
                result.position = curPos;
                result.length = 1;
                result.code = PV_NO_LEFT_BOUND; return result;
            }
            else
            {
                cBound += source[curPos];
            }
        } // endif ( par == lb )
        else if ( par == rb )
        {
            if ( source[curPos] == '[' )
            {
                deep1 ++;
                cBound += '[';
            }
            else if ( source[curPos] == '(' )
            {
                deep2 ++;
                cBound += '(';
            }
            else if ( ( source[curPos] == ']' ) && (deep1 > 0) )
            {
                deep1 --;
                cBound += ']';

            }
            else if ( source[curPos] == ')' )
            {
                deep2 --;
                cBound += ')';
            }
            else if ( (source[curPos] == ',') && (deep1 == 0) && (deep2 == 0) )
            {
                if ( dim > 2 )
                {
                    result.position = ArrayStart;
                    int e = source.indexOf("]",curPos);
                    if ( e == -1 )
                        result.length = curPos-ArrayStart;
                    else
                        result.length = e-ArrayStart+1;
                    result.code = PV_BIG_RAZM; return result; // размерность > 3
                }
                dim++;

                QStringList el;

                QString rightXml; StructType st;
                Kumir::ValueType rightType = parceExpression(cBound,
                                                             functions,
                                                             symbols,
                                                             algName,
                                                             el,
                                                             st,
                                                             result,
                                                             extraSubExpressions,
                                                             rightXml);
                if ( result.code )
                {
                    result.position += ArrayBoundStart;
                    return result; // ошибка разбора левой границы
                }
                if ( rightType != Kumir::Integer )
                {
                    result.position = ArrayBoundStart;
                    result.length = curPos-ArrayBoundStart;
                    result.code = PV_BAD_RIGHT; return result; // правая граница -- не целое число
                }
                xmlList << rightXml;
                QString pattern = "\\b"+cName+"\\b";
                if ( !algName.isEmpty() )
                    pattern += "|\\b"+algName+"\\b";
                QRegExp rxRecursion = QRegExp(pattern);
                int rxPos = rxRecursion.indexIn(cBound,0);
                if ( rxPos != -1 )
                {
                    int rxLen = rxRecursion.matchedLength();
                    result.position = ArrayBoundStart+rxPos;
                    result.length = rxLen;
                    result.code = PV_RECURSION_R; return result; // рекурсивное определение массива
                }
                exprList << el;
                cBound = "";
                par = lb;
                ArrayBoundStart = curPos;
            }
            else if ( (source[curPos] == ']') && (deep1 == 0) )
            {
                if ( dim > 2 )
                {
                    result.position = ArrayStart;
                    int e = source.indexOf("]",curPos);
                    if ( e == -1 )
                        result.length = curPos-ArrayStart;
                    else
                        result.length = e-ArrayStart;
                    result.code = PV_BIG_RAZM; return result; // размерность > 3
                }

                dim++;

                QStringList el;

                if ( cBound.trimmed().isEmpty() )
                {
                    result.position = ArrayBoundStart;
                    result.length = curPos-ArrayBoundStart;
                    result.code = PV_NO_RIGHT_BOUND; return result;
                }
                QString rightXml; StructType st;
                Kumir::ValueType rightType = parceExpression(cBound,
                                                             functions,
                                                             symbols,
                                                             algName,
                                                             el,
                                                             st,
                                                             result,
                                                             extraSubExpressions,
                                                             rightXml);
                if ( result.code > 0 )
                {
                    result.position += ArrayBoundStart+1;
                    return result;
                }
                if ( rightType != Kumir::Integer )
                {
                    result.position = ArrayBoundStart+1;
                    result.length = curPos-ArrayBoundStart-1;
                    result.code = PV_BAD_RIGHT; return result; // правая граница -- не целое число
                }
                QString pattern = "\\b"+cName+"\\b";
                if ( !algName.isEmpty() )
                    pattern += "|\\b"+algName+"\\b";
                QRegExp rxRecursion = QRegExp(pattern);
                int rxPos = rxRecursion.indexIn(cBound,0);
                if ( rxPos != -1 )
                {
                    int rxLen = rxRecursion.matchedLength();
                    result.position = ArrayBoundStart+rxPos;
                    result.length = rxLen;
                    result.code = PV_RECURSION_R; return result; // рекурсивное определение массива
                }
                exprList << el;
                par = tn;
                massDeclared = true;
                xmlList << rightXml;
            }
            else if (source[curPos]==';') {
                result.position = ArrayBoundStart;
                result.length = 1;
                result.code = PV_NO_CLOSE_BRACKET; return result;
            }
            else
                cBound += source[curPos];
        } // endif ( par == rb )
        else if ( par==value ) {
            if (isArg) {
                result.position = z-1;
                result.length = 1;
                result.code = PV_CONSTANT_IN_RES; return result;
            }
            if ( source[curPos]=='{' ) {
                deepV ++;
            }
            else if  ( source[curPos]=='}' ) {
                deepV --;
            }
            if ( deepV==0 && ( source[curPos]==',' || source[curPos]==';' )) {

                qDebug() << "Extracted Constant value: " << initValue;
                if (initValue.trimmed().isEmpty()) {
                    result.position = z-1;
                    result.length = 1;
                    result.code = PV_CONST_IS_EMPTY; return result;
                }
                QString cValue;

                Kumir::CompileError err = parceConstant(initValue, cType, dim, cValue);

                if ( err.code ) {
                    result.position = z + err.position;
                    result.length = err.length;
                    result.code = err.code;
                    return result;
                }

                int i_value;
                double d_value;
                QChar c_value;
                bool b_value;
                QString s_value;

                if ( cType==Kumir::Integer || cType==Kumir::Real || cType==Kumir::Boolean || cType==Kumir::Charect || cType==Kumir::String ) {
                    QByteArray ba = QByteArray::fromBase64(cValue.toAscii());
                    QString sval = QString::fromUtf8(ba);
                    if (cType==Kumir::String)
                        s_value = sval;
                    else if (cType==Kumir::Charect) {
                        Q_ASSERT ( sval.length()==1 );
                        c_value = sval[0];
                    }
                    else if (cType==Kumir::Boolean) {
                        b_value = sval=="true";
                    }
                    else if (cType==Kumir::Integer) {
                        bool ok;
                        i_value = sval.toInt(&ok);
                        Q_ASSERT ( ok );
                    }
                    else if (cType==Kumir::Real) {
                        bool ok;
                        d_value = sval.toDouble(&ok);
                        Q_ASSERT ( ok );
                    }
                    symbols.setUsed(currentID, true);
                }

                if (cType==Kumir::Integer) {
                    symbols.symb_table[currentID].value->setIntegerValue(i_value);
                }
                else if (cType==Kumir::Real) {
                    symbols.symb_table[currentID].value->setFloatValue(d_value);
                }
                else if (cType==Kumir::Boolean) {
                    symbols.symb_table[currentID].value->setBoolValue(b_value);
                }
                else if (cType==Kumir::Charect) {
                    symbols.symb_table[currentID].value->setCharectValue(c_value);
                }
                else if (cType==Kumir::String) {
                    symbols.symb_table[currentID].value->setStringValue(s_value);
                }
                else {

                    // Формируем скрытую инструкцию - вызов одной из функций:
                    // алг !memfill_*$(аргрез *таб$ массив, арг лит данные)
                    // где массив - это заполняемый массив,
                    //     данные - сериализованные данные в Base64


                    symbols.symb_table[currentID].razm = dim;


                    // Формируем имя функции на основании типа данных массива
                    // и его размерности

                    QString mft;
                    if ( cType==Kumir::IntegerArray )
                        mft = "i";
                    else if ( cType==Kumir::RealArray )
                        mft = "d";
                    else if ( cType==Kumir::BooleanArray )
                        mft = "b";
                    else if ( cType==Kumir::CharectArray )
                        mft = "c";
                    else if ( cType==Kumir::StringArray )
                        mft = "s";
                    QString functName = QString("!memfill_%1%2").arg(mft).arg(dim);

                    // Заводим константу в таблице символов

                    symbols.add_symbol("!KUM_CONST",-1,Kumir::String,algName,true);
                    symbols.new_symbol(symbols.count);
                    int literal_id = symbols.count;
                    set_const_value(literal_id, "\""+cValue+"\"", symbols);
                    symbols.setUsed(literal_id, true);

                    // Формируем инструкцию


                    KumirInstruction e;
                    e.str_type = Kumir::AlgorhitmCall;
                    int module_id = -1;
                    int func_id = -1;

                    QPair<int,int> aaa = m_modulesPool->funcByName(functName);
                    module_id = aaa.first;
                    func_id = aaa.second;

                    e.line = QString("%1|%2").arg(func_id).arg(module_id);
                    QStringList vl;
                    vl << "^"+QString::number(currentID);
                    vl << "^"+QString::number(literal_id);
                    vl << QString("@%1&0&1").arg(func_id);
                    e.VirajList << vl;

                    e.VirajTypes << cType;
                    e.VirajTypes << Kumir::String;

                    extraProgram << e;

                }
                par = name;
                initValue = "";
            }
            else {
                initValue += source[curPos];
            }

        } // endif ( par==value )
    }
    result.code = 0;
    return result;
}


Kumir::CompileError TextAnalizer::parceConstant(const QString &constant, const Kumir::ValueType pt, int& maxDim, QString &value)
{
    int localErr = 0;
    Kumir::ValueType ct;

    Kumir::CompileError result;
    result.code = 0;
    result.position = 0;
    result.length = constant.length();

    if (constant.trimmed()=="...") {
        result.code = PV_CONSTANT_NOT_TYPED;
        return result;
    }
    if (pt==Kumir::Integer || pt==Kumir::Real || pt==Kumir::Boolean || pt==Kumir::Charect || pt==Kumir::String ) {
        ct = testConst(constant, &localErr);
        if ( ct==Kumir::Undefined ) {
            result.code = PV_NOT_A_CONSTANT;
            return result;
        }
        if (localErr>0) {
            result.code = localErr;
            return result;
        }
        else if ( ( ct==pt ) || (ct==Kumir::Integer || pt==Kumir::Real) )
        {
            QByteArray ba = createConstValue(constant, ct).toString().toUtf8().toBase64();
            value = QString::fromAscii(ba);
        }
        else {
            result.code = PV_REGULAR_TYPE_MISMATCH;
            return result;
        }
    }
    else {
        QVariant d0;
        QList<QVariant> d1;
        QList< QList<QVariant> > d2;
        QList< QList< QList<QVariant> > > d3;
        QList<QVariant> items;
        QString current = "";
        int deep = 0;
        Kumir::ValueType t;
        if (pt==Kumir::IntegerArray)
            t = Kumir::Integer;
        else if (pt==Kumir::RealArray)
            t = Kumir::Real;
        else if (pt==Kumir::BooleanArray)
            t = Kumir::Boolean;
        else if (pt==Kumir::CharectArray)
            t = Kumir::Charect;
        else if (pt==Kumir::StringArray)
            t = Kumir::String;
        else
            qFatal("Trying to untable regular type");
        int maxDeep = 0;
        int lexemStart = 0;
        Kumir::ValueType it;
        for ( int i=0; i<constant.length(); i++ ) {
            if ( constant[i]=='{' ) {
                deep++;
                lexemStart = i+1;
                maxDeep = qMax(deep, maxDeep);
                if ( deep>3 ) {
                    result.position = i;
                    result.length = 1;
                    result.code = PV_CONST_EXTRA_OPEN_BRACKET; return result; // FIXME: extra opening
                }
            }
            else if ( constant[i]=='}' ) {
                if (deep==0) {
                    result.position = i;
                    result.length = 1;
                    result.code = PV_CONST_EXTRA_CLOSE_BRACKET; return result; // FIXME: extra closing }
                }
                deep--;
            }
        }
        for ( int i=0; i<constant.length(); i++ ) {
            if ( constant[i]=='{' ) {
                deep++;
                lexemStart = i+1;

                if ( deep>3 ) {
                    result.position = i;
                    result.length = 1;
                    result.code = PV_CONST_EXTRA_OPEN_BRACKET; return result; // FIXME: extra opening
                }
            }
            else if ( constant[i]=='}' ) {
                if (deep==0) {
                    result.position = i;
                    result.length = 1;
                    result.code = PV_CONST_EXTRA_CLOSE_BRACKET; return result; // FIXME: extra closing }
                }
                if (deep==maxDeep) {
                    if ( current.trimmed().isEmpty() ) {
                        d0 = QVariant();
                        d1 << d0;
                    }
                    else {
                        it = testConst(current, &localErr);
                        if (localErr>0) {
                            result.position = lexemStart;
                            result.length = i-lexemStart;
                            result.code = localErr;
                            return result;
                        }
                        if ( it!=t && !( it==Kumir::Integer && t==Kumir::Real ) ) {
                            result.position = lexemStart;
                            result.length = i-lexemStart;
                            result.code = PV_CONST_ELEM_TYPE_MISMATCH; return result; // FIXME: type mismatch
                        }
                        d0 = createConstValue(current, t);
                        d1 << d0;
                        lexemStart = i+1;
                        current = "";
                    }
                }
                else if ( deep==maxDeep-1 ) {
                    d2 << d1;
                    d1.clear();
                }
                else if ( deep==maxDeep-2 ) {
                    d3 << d2;
                    d2.clear();
                }
                deep--;
            }
            else if ( constant[i]==',') {
                if (deep==maxDeep) {
                    if ( current.trimmed().isEmpty() ) {
                        d0 = QVariant();
                        d1 << d0;
                    }
                    else {
                        it = testConst(current, &localErr);
                        if (localErr>0) {
                            result.position = lexemStart;
                            result.length = i-lexemStart;
                            result.code = localErr;
                            return result;
                        }
                        if ( it!=t && !( it==Kumir::Integer && t==Kumir::Real ) ) {
                            result.position = lexemStart;
                            result.length = i-lexemStart;
                            result.code = PV_CONST_ELEM_TYPE_MISMATCH; return result; // FIXME: type mismatch
                        }
                        d0 = createConstValue(current, t);
                        d1 << d0;
                        lexemStart = i+1;
                        current = "";
                    }
                }
                else if ( deep==maxDeep-1 ) {
                    d2 << d1;
                    d1.clear();
                }
                else if ( deep==maxDeep-2 ) {
                    d3 << d2;
                    d2.clear();
                }
            }
            else {
                current += constant[i];
            }
        }

        int dim;
        if (maxDim==0) {
            dim = maxDeep;
            maxDim = dim;
        }
        else {
            if ( maxDeep != maxDim ) {
                result.position = 0;
                result.length = constant.length();
                result.code = PV_CONST_DIMENSION_MISMATCH; return result; // FIXME: dimension mismatch
            }
            dim = maxDim;
        }
        Q_ASSERT ( dim >= 1 && dim <= 3 );
        Q_ASSERT ( deep==0 );
        value = "";
        if ( dim==1 ) {
            for ( int i=0; i<d1.size(); i++ ) {
                QByteArray ba = d1.at(i).toString().toUtf8().toBase64();
                value += ba;
                if ( i<d1.size()-1)
                    value += ".";
            }
        }
        else if ( dim==2 ) {
            for ( int i=0; i<d2.size(); i++ ) {
                for ( int j=0; j<d2.at(i).size(); j++ ) {
                    QByteArray ba = d2.at(i).at(j).toString().toUtf8().toBase64();
                    value += ba;
                    if (j<d2.at(i).size()-1)
                        value += ".";
                }
                if (i<d2.size()-1)
                    value += ":";
            }
        }
        else if ( dim==3 ) {
            for ( int i=0; i<d3.size(); i++ ) {
                for ( int j=0; j<d3.at(i).size(); j++ ) {
                    for ( int k=0; k<d3.at(i).at(j).size(); k++ ) {
                        QByteArray ba = d3.at(i).at(j).at(k).toString().toUtf8().toBase64();
                        value += ba;
                        if (k<d3.at(i).at(j).size()-1)
                            value += ".";
                    }
                    if (j<d3.at(i).size()-1)
                        value += ":";
                }
                if (i<d3.size()-1)
                    value += "|";
            }
        }
    }

    qDebug() << "VALUE: " << value;
    return result;
}


/**
 * Разбор строки описания алгоритма
 * @param stroka нормализованная разбираемая строка
 * @param str номер нормализованной(?) строки
 * @param *symbols ссылка на таблицу символов
 * @param *functions ссылка на таблицу функций
 * @param KumirInstruction_str
 * @return код ошибки
 */
Kumir::CompileError TextAnalizer::parceAlgorhitmHeader(int moduleId, KumirInstruction &pv, int str, SymbolTable &symbols, FunctionTable &functions)
{
    Kumir::CompileError result;
    result.code = 0;
    QString stroka = pv.line;
    // Проверка возможности добавления в таблицу функций


    // Разбираем строку на имя алгоритма, его тип и список аргументов

    QString name; // имя алгоритма
    Kumir::ValueType returnType = Kumir::Undefined; // возвращаемый тип
    bool hasArgs = true; // флаг "Есть аргументы"
    QRegExp rxNonSpace = QRegExp("\\S+"); // непробелный символ

    // ============== ИМЯ АЛГОРИТМА

    // ищем первый пробел в строке
    int p = stroka.indexOf(' ');
    int str_len = stroka.length();
    int tp = -1;
    if ( ( p < str_len ) && ( p != -1 ) )
    {
        p++;
        returnType = KsymbolToValueType(stroka[p]);
        if ( returnType != Kumir::Undefined )
        {
            tp = p;
            p = stroka.indexOf(rxNonSpace,p+1);
            if ( p == -1 )
                p = stroka.length()-1;
        }
    }

    // ищем открывающую скобку
    int p2 = stroka.indexOf('(');
    if ( p2 == -1 )
    {
        p2 = stroka.length();
        hasArgs = false;
    }

    // выделяем имя алгоритма
    if ( p != -1 ) name = stroka.mid(p,p2-p).trimmed();
    if ( name.startsWith(QString(QChar(KS_DUMMY_ALG_NAME))) )
    {
        QStringList pair = name.split(" ");
        name = "!DUMMY_"+pair[1];
    }
    else {
        // проверяем корректность имени алгоритма
        int tn_start = 0, tn_len = 0;
        int local_err = m_textNormalizer->test_name(name,tn_start,tn_len);
        if ( local_err > 0 )
        {
            result.position =  p + tn_start;
            result.length =  tn_len;
            result.code = local_err;
            return result;
        }
    }
    // qDebug()<<"modules in analizer"<< modules;
    // qDebug()<<"modules in compiler"<< app()->compiler->modules();
    //qDebug()<<"moduleId" <<moduleId;


    const QString ispName = m_modulesPool->moduleName(moduleId);

    bool isFirst = firstAlg && (ispName=="main");

    // флаг firstAlg проставляется в test_all, означает "первый алгоритм"

    if ( isFirst )
    {
        firstAlg = false;
        // если алгоритм первый в тексте, то допускается пустое имя,
        // но при этом не должно быть аргументов и возвращаемого параметра
        if ( name.isEmpty() )
        {
            if ( hasArgs )
            {
                result.position =  p2;
                result.length =  stroka.length()-p2;
                result.code = FUNC_EXTRA_ARGS;
                return result;
            }
            if ( returnType != Kumir::Undefined )
            {
                result.position =  tp;
                result.length =  1;
                result.code = FUNC_MUST_NONE;
                return result;
            }
        }
    }
    else
    {
        // если алгоритм не первый, обязательно должно быть имя, иначе -- ошибка
        if ( name.isEmpty() )
        {
            result.position =  0;
            result.length =  stroka.length();
            result.code = FUNC_NET_IMENI_2;
            return result;
        }
    }

    // Проверяем на повторное описание алгоритма
    int func_id=-1;
    int module_Id=-1;

    QPair<int,int> aaa = m_modulesPool->funcByName(name);
    module_Id = aaa.first;
    func_id = aaa.second;

    if ( func_id != -1 )
    {
        result.position =  p;
        result.length =  p2-p;
        result.code = FUNC_ALG_EXIST;
        return result;
    }

    // Проверяем на наличие переменной с таким же именем

    if ( symbols.inTable(name,"global") != -1 )
    {
        result.position =  p;
        result.length =  p2-p;
        result.code = FUNC_NAME_USED;
        return result;
    }

    // Если функция что-то возвращает, то добавляем в таблицу символов
    // одноименную переменную

    if ( returnType != Kumir::Undefined )
    {
        symbols.add_symbol(name,str,returnType,"global",false);
        symbols.new_symbol(symbols.count);
        symbols.setUsed(symbols.count,false);
        symbols.symb_table[symbols.count].is_FunctionReturnValue = true;
    }

    if ( returnType == Kumir::IntegerArray || returnType == Kumir::RealArray || returnType == Kumir::BooleanArray || returnType == Kumir::CharectArray || returnType == Kumir::StringArray )
    {
        result.position =  tp;
        result.length =  1;
        result.code = FUNC_RETURN_MASS;
        return result;
    }

    // Заносим алгоритм в таблицу функций
    int id = functions.append(name, returnType, str);

    // Формируем строку выполнителя
    pv.VirajList.clear();
    pv.line = QString(QChar(KS_ALG))+" ";
    if ( returnType == Kumir::Integer )
        pv.line += QString(QChar(KS_INT)) + " ";
    if ( returnType == Kumir::Real )
        pv.line += QString(QChar(KS_REAL)) + " ";
    if ( returnType == Kumir::Boolean )
        pv.line += QString(QChar(KS_BOOL)) + " ";
    if ( returnType == Kumir::Charect )
        pv.line += QString(QChar(KS_CHAR)) + " ";
    if ( returnType == Kumir::String )
        pv.line += QString(QChar(KS_STRING)) + " ";
    if ( returnType == Kumir::IntegerArray )
        pv.line += QString(QChar(KS_INT_TAB)) + " ";
    if ( returnType == Kumir::RealArray )
        pv.line += QString(QChar(KS_REAL_TAB)) + " ";
    if ( returnType == Kumir::BooleanArray )
        pv.line += QString(QChar(KS_BOOL_TAB)) + " ";
    if ( returnType == Kumir::CharectArray )
        pv.line += QString(QChar(KS_CHAR_TAB)) + " ";
    if ( returnType == Kumir::StringArray )
        pv.line += QString(QChar(KS_STRING_TAB)) + " ";
    pv.line += name;

    functions.setRunStepInto(id, isFirst && hasArgs);

    // Если нет аргументов, то всё

    if ( !hasArgs ) {
        functions.setBroken(id, false);
        return result;
    }

    // =============== РАЗБОР АРГУМЕНТОВ


    QString argLine = stroka.mid(p2);
    if ( argLine.startsWith("(") && argLine.endsWith(")") )
    {
        argLine.replace(0,1," ");
        argLine.replace(argLine.length()-1,1," ");
        if ( argLine.trimmed().isEmpty() )
        {
            functions.setBroken(id, true);
            result.position =  p2;
            result.length =  stroka.length()-p2;
            result.code = FUNC_NOARG;
            return result;
        }
        QStringList groups;
        QList<ArgModifer> mods;
        QList<int> shifts;
        ArgModifer mod = arg;
        bool hasArraysInArgs = false;
        QString group;
        for ( int i=0; i<argLine.length(); i++ )
        {
            if ( argLine[i] == QChar(KS_ARG) || argLine[i] == QChar(KS_RES) || argLine[i] == QChar(KS_ARGRES) )
            {
                if ( group.trimmed().endsWith(",") )
                {
                    int cp = group.lastIndexOf(",");
                    group.truncate(cp);
                }
                else if (!group.trimmed().isEmpty()) {
                    functions.setBroken(id, true);
                    result.position =  p2+i;
                    result.length =  1;
                    result.code = FUNC_NO_COMMA_BETWEEN_GROUPS;
                    return result;
                }
                if ( argLine[i] == QChar(KS_ARG) )
                    mod = arg;
                if ( argLine[i] == QChar(KS_RES) ) {
                    mod = res;
                }
                if ( argLine[i] == QChar(KS_ARGRES) )
                    mod = argres;
                mods << mod;
                if ( !group.trimmed().isEmpty() )
                {
                    groups << group;
                    group = "";
                }
                shifts << i+1;

            }
            else if ( argLine[i] == QChar(KS_ALG) )
            {
                functions.setBroken(id, true);;
                result.position =  p2+i;
                result.length =  1;
                result.code = FUNC_ALG_INSTEAD_ARG;
                return result;
            }
            else
                group += argLine[i];
        }
        groups << group;
        if ( groups.count() != mods.count() )
        {
            mods.prepend(arg);
        }
        if ( groups.count() != shifts.count() )
        {
            shifts.prepend(0);
        }
        QStringList exprList;
        QList<int> ids;
        QStringList xmlList;
        for ( int i=0; i<groups.count(); i++ )
        {
            QList<KumirInstruction> extraProgram;
            result = parceVariables(groups[i],
                                    name,
                                    mods[i],
                                    str,
                                    symbols,
                                    functions,
                                    ids,
                                    pv.VirajList,
                                    pv.extraSubExpressions,
                                    xmlList,
                                    true,
                                    extraProgram);
            if ( result.code )
            {
                result.position +=  p2 + shifts[i];
                functions.setBroken(id, true);
                return result;
            }
        }
        QString argNormLine;
        int counter = 0;
        for ( int i=0; i<ids.count(); i++ )
        {
            Kumir::ValueType type = symbols.symb_table[ids[i]].type;
            if ( (type == Kumir::Integer) || (type == Kumir::Real) || (type == Kumir::Boolean) || (type == Kumir::String) || (type == Kumir::Charect) )
            {
                argNormLine += "^"+QString::number(ids[i]);
            }
            else
            {
                hasArraysInArgs = true;
                argNormLine += "^"+QString::number(ids[i]);
                int dim = symbols.symb_table[ids[i]].razm;
                for ( int j=0; j<dim*2; j++ )
                {
                    argNormLine += "#"+QString::number(counter);
                    counter++;
                }
            }
            if ( i != ( ids.count()-1 ) )
                argNormLine += ",";

            functions.appendArg(id,
                                 symbols.symb_table[ids[i]].type,
                                 symbols.symb_table[ids[i]].razm,
                                 symbols.symb_table[ids[i]].is_Res,
                                 symbols.symb_table[ids[i]].is_ArgRes,
                                 ids[i]);


        }
        pv.line += "("+argNormLine+")";
        if (isFirst && hasArgs) {
            // Заплатка: запрещаем аргументы-массивы в первом алгоритме с параметрами
            if (hasArraysInArgs) {
                functions.setBroken(id, true);
                result.position =  p2;
                result.length =  stroka.length()-p2;
                result.code = FUNC_ARRAY_PARAM_IN_FIRST_ALG;
                return result;

            }
            // Создание точки входа, которая
            // будет требовать ввод аргументов
            // и вывод результатов
            m_firstAlgorhitmIP = str;
            generateEntryPoint(groups, mods, symbols, functions, name, returnType);
        }
    }
    else
    {
        functions.setBroken(id, true);
        if ( ! argLine.startsWith("(") && argLine.endsWith(")") )
        {
            result.position =  stroka.length()-1;
            result.length =  1;
            result.code = 20201;
            return result;
        }
        if ( argLine.startsWith("(") && ! argLine.endsWith(")") )
        {
            result.position =  p2;
            result.length =  1;
            result.code = 20202;
            return result;
        }
    }
    functions.setBroken(id, false);

    return result;
}

QString screenString(const QString &txt)
{
    QString result;
    for ( int i=0; i<txt.length(); i++ ) {
        result += QChar(txt[i].unicode()+STR_HIDE_OFFSET);
    }
    return result;
}

void TextAnalizer::generateEntryPoint(const QStringList &argGroups,
                                      const QList<ArgModifer> &argMods,
                                      SymbolTable &symbols,
                                      FunctionTable &functions,
                                      const QString &callName,
                                      const Kumir::ValueType &callReturnType)
{
    m_textNormalizer->setAllowHiddenNames(true);
    QRegExp rxSymbolsToRemove("[\\x1000-\\x119D]");
    Q_ASSERT ( rxSymbolsToRemove.isValid() );
    m_hiddenAlgorhitm.clear();
    KumirInstruction cpv;
    QStringList declarations;
    QStringList input;
    QList<int> inpDeclNo;
    QList<int> outDeclNo;
    QVector<bool> declared;
    QStringList output;
    QString varName;
    QStringList vars;

    QStringList arguments;
    QString currentType;
    QChar ct;

    firstAlgorhitmText = QString::fromUtf8("алг\nнач\n");
    Q_ASSERT( argGroups.count() == argMods.count() );
    for (int i=0; i<argGroups.count(); i++ ) {
        varName = argGroups[i];
        ct = varName.trimmed()[0];
        if (ct==QChar(KS_INT))
            currentType = QString::fromUtf8("цел");
        else if (ct==QChar(KS_REAL))
            currentType = QString::fromUtf8("вещ");
        else if (ct==QChar(KS_BOOL))
            currentType = QString::fromUtf8("лог");
        else if (ct==QChar(KS_CHAR))
            currentType = QString::fromUtf8("сим");
        else if (ct==QChar(KS_STRING))
            currentType = QString::fromUtf8("лит");
        else if (ct==QChar(KS_INT_TAB))
            currentType = QString::fromUtf8("целтаб");
        else if (ct==QChar(KS_REAL_TAB))
            currentType = QString::fromUtf8("вещтаб");
        else if (ct==QChar(KS_BOOL_TAB))
            currentType = QString::fromUtf8("логтаб");
        else if (ct==QChar(KS_CHAR_TAB))
            currentType = QString::fromUtf8("симтаб");
        else if (ct==QChar(KS_STRING_TAB))
            currentType = QString::fromUtf8("литтаб");
        if (argMods[i]==arg || argMods[i]==argres) {
            varName.remove(rxSymbolsToRemove);
            vars = varName.split(",");
            foreach ( const QString &var, vars) {
                input << var.simplified();
                firstAlgorhitmText += currentType + " " + var.simplified() + " = ...\n";
                arguments << var.simplified();
                inpDeclNo << i;
            }
        }
        if (argMods[i]==res || argMods[i]==argres) {
            varName = argGroups[i];
            varName.remove(rxSymbolsToRemove);
            vars = varName.split(",");
            foreach ( const QString &var, vars) {
                output << var.simplified();
                if (!input.contains(var.simplified())) {
                    firstAlgorhitmText += currentType + " " + var.simplified() + "\n";
                }
                arguments << var.simplified();
                outDeclNo << i;
            }
        }
        declarations << argGroups[i];
    }

    if (callReturnType!=Kumir::Undefined) {
        if (callReturnType==Kumir::Integer)
            firstAlgorhitmText += QString::fromUtf8("цел ");
        else if (callReturnType==Kumir::Real)
            firstAlgorhitmText += QString::fromUtf8("вещ ");
        else if (callReturnType==Kumir::Boolean)
            firstAlgorhitmText += QString::fromUtf8("лог ");
        else if (callReturnType==Kumir::Charect)
            firstAlgorhitmText += QString::fromUtf8("сим ");
        else if (callReturnType==Kumir::String)
            firstAlgorhitmText += QString::fromUtf8("лит ");
        firstAlgorhitmText += QString::fromUtf8("Результат_")+callName+"\n";
        firstAlgorhitmText += QString::fromUtf8("Результат_")+callName+":="+callName;
    }
    else {
        firstAlgorhitmText += callName;
    }
    firstAlgorhitmText += "("+arguments.join(", ")+")\n";

    declared = QVector<bool>(declarations.count(),false);

    // Заносим алгоритм в таблицу функций

    int id = functions.appendDummy();
    Q_UNUSED(id);

    // Формируем алг-нач
    cpv.line = QString(QChar(KS_ALG))+" ";
    cpv.error = 0;
    cpv.str_type = Kumir::AlgorhitmDeclaration;
    cpv.else_pos = 1;
    m_hiddenAlgorhitm.append(cpv);
    cpv.line = KS_BEGIN;
    cpv.str_type = Kumir::AlgorhitmBegin;
    cpv.else_pos = 2;
    m_hiddenAlgorhitm.append(cpv);


    QString xml;

    int declNo = -1;
    QString inp;
    QString decl;

    // Формируем строки "ввод"
    for( int i=0; i<input.count(); i++ ) {
        inp = input[i];
        declNo = inpDeclNo[i];
        cpv.str_type = Kumir::Output;
        cpv.VirajList.clear();
        cpv.VirajTypes.clear();
        cpv.line = QString(KS_OUTPUT)+" \""+screenString(QObject::tr("Please enter ")+inp+": ")+"\"";
        parceOutput(cpv, functions, symbols, -1, xml);

        m_hiddenAlgorhitm.append(cpv);
        cpv.str_type = Kumir::VariableDeclaration;
        cpv.line = declarations[declNo];
        cpv.VirajList.clear();
        cpv.VirajTypes.clear();
        parceVariablesDeclaration(cpv, -1, "", symbols, functions, xml);

        m_hiddenAlgorhitm.append(cpv);
        declared[declNo] = true;
        cpv.str_type = Kumir::Input;
        cpv.VirajList.clear();
        cpv.VirajTypes.clear();
        cpv.line = QString(KS_INPUT)+" "+inp;
        parceInput(cpv, functions, symbols, -1, xml);

        m_hiddenAlgorhitm.append(cpv);
    }

    bool notDecl;

    // Формируем строки объявления переменных
    for ( int i=0; i<declarations.count(); i++ ) {
        notDecl = ! declared[i];
        if (notDecl) {
            decl = declarations[i];
            cpv.str_type = Kumir::VariableDeclaration;
            cpv.line = decl;
            cpv.VirajList.clear();
            cpv.VirajTypes.clear();
            parceVariablesDeclaration(cpv, -1, "", symbols, functions, xml);

            m_hiddenAlgorhitm.append(cpv);
        }
    }

    // Формируем вызов алгоритма
    if ( callReturnType==Kumir::Undefined ) {
        cpv.str_type = Kumir::AlgorhitmCall;
        cpv.VirajList.clear();
        cpv.VirajTypes.clear();
        cpv.line = callName+"("+arguments.join(",")+")";
        parceAlgorhitmCall(cpv,functions,symbols,-1,xml);

        m_hiddenAlgorhitm.append(cpv);
    }
    else {
        cpv.str_type = Kumir::Output;
        cpv.VirajList.clear();
        cpv.VirajTypes.clear();
        cpv.line = QString(KS_OUTPUT)+" \""+screenString(QObject::tr("Return value = "))+"\","+callName+"("+arguments.join(",")+"),"+QString(KS_NEWLINE);
        parceOutput(cpv, functions, symbols, -1, xml);

        m_hiddenAlgorhitm.append(cpv);
    }

    // Формируем вывод результатов
    foreach ( const QString &out, output ) {
        cpv.str_type = Kumir::Input;
        cpv.VirajList.clear();
        cpv.VirajTypes.clear();
        cpv.line = QString(KS_INPUT)+" \""+screenString(out+" = ")+"\","+out+","+QString(KS_NEWLINE);
        parceOutput(cpv, functions, symbols, -1, xml);

        m_hiddenAlgorhitm.append(cpv);
    }

    // Формируем конец алгоритма
    cpv.str_type = Kumir::AlgorhitmEnd;
    cpv.VirajList.clear();
    cpv.VirajTypes.clear();
    cpv.line = QString(KS_END);
    cpv.else_pos = -1;
    m_hiddenAlgorhitm.append(cpv);
    firstAlgorhitmText += QString::fromUtf8("кон\n");
    m_textNormalizer->setAllowHiddenNames(false);
    qDebug() << "First Algorhitm text: " << firstAlgorhitmText;
}


/**
 * Проверяет, находится ли позиция pos внутри литеральной константы
 * @param s исходная ненормализованная строка
 * @param pos позиция
 * @return true, если внутри литеральной константы или false, если нет
 */

Kumir::CompileError TextAnalizer::parceDocString(const QString &in, FunctionTable &functions, SymbolTable &symbols, int str, QString &xml)
{
    Kumir::CompileError result;
    QString instr = in;
    Q_UNUSED(xml);
    Q_UNUSED(symbols);
    QString algName = functions.nameByPos(str);
    int id = functions.idByName(algName);
    int pos;

    // Заменяем синонимы тегов
    instr.replace(QObject::trUtf8("<прм>"),"<arg>",Qt::CaseInsensitive);
    instr.replace(QObject::trUtf8("<ARG>"),"<arg>",Qt::CaseInsensitive);
    instr.replace(QString::fromUtf8("<ПРМ>"),"<arg>",Qt::CaseInsensitive);
    instr.replace(QString::fromUtf8("</прм>"),"</arg>",Qt::CaseInsensitive);
    instr.replace(QString::fromUtf8("</ПРМ>"),"</arg>",Qt::CaseInsensitive);
    instr.replace(QString::fromUtf8("</ARG>"),"</arg>",Qt::CaseInsensitive);
    instr.replace(QString::fromUtf8("\\прм{"),"\\arg{",Qt::CaseInsensitive);
    instr.replace(QString::fromUtf8("\\ARG{"),"\\arg{",Qt::CaseInsensitive);
    instr.replace(QString::fromUtf8("\\ПРМ{"),"\\arg{",Qt::CaseInsensitive);

    instr.replace(QString::fromUtf8("<прм>"),"<arg>",Qt::CaseInsensitive);
    instr.replace(QString::fromUtf8("<ARG>"),"<arg>",Qt::CaseInsensitive);
    instr.replace(QString::fromUtf8("<ПРМ>"),"<arg>",Qt::CaseInsensitive);
    instr.replace(QString::fromUtf8("</прм>"),"</arg>",Qt::CaseInsensitive);
    instr.replace(QString::fromUtf8("</ПРМ>"),"</arg>",Qt::CaseInsensitive);
    instr.replace(QString::fromUtf8("</ARG>"),"</arg>",Qt::CaseInsensitive);
    instr.replace(QString::fromUtf8("\\прм{"),"\\arg{",Qt::CaseInsensitive);
    instr.replace(QString::fromUtf8("\\ARG{"),"\\arg{",Qt::CaseInsensitive);
    instr.replace(QString::fromUtf8("\\ПРМ{"),"\\arg{",Qt::CaseInsensitive);

    instr.replace(QString::fromUtf8("<ж>"),"<b>",Qt::CaseInsensitive);
    instr.replace(QString::fromUtf8("</ж>"),"</b>",Qt::CaseInsensitive);
    instr.replace(QString::fromUtf8("<к>"),"<i>",Qt::CaseInsensitive);
    instr.replace(QString::fromUtf8("</к>"),"</i>",Qt::CaseInsensitive);
    instr.replace(QString::fromUtf8("\\ж{"),"\\bf{",Qt::CaseInsensitive);
    instr.replace(QString::fromUtf8("\\к{"),"\\it{",Qt::CaseInsensitive);

    // Заменяем ТеХ-теги на HTML-теги
    QRegExp rxAlgTex = QRegExp("\\\\arg\\{(.*)\\}",Qt::CaseInsensitive);
    rxAlgTex.setMinimal(true);
    while ( (pos = rxAlgTex.indexIn(instr)) != -1 )
    {
        int len = rxAlgTex.matchedLength();
        QString param = rxAlgTex.cap(1);
        instr.replace(pos,len,"<arg>"+param+"</arg>");
    }
    QRegExp rxBoldTex = QRegExp("\\\\textbf\\{(.*)\\}|\\\\bf\\{(.*)\\}",Qt::CaseInsensitive);
    rxBoldTex.setMinimal(true);
    while ( (pos = rxBoldTex.indexIn(instr)) != -1 )
    {
        int len = rxBoldTex.matchedLength();
        QString param = rxBoldTex.capturedTexts().last();
        if ( param.isEmpty() )
            param = rxBoldTex.cap(1);
        instr.replace(pos,len,"<b>"+param+"</b>");
    }
    QRegExp rxItalicTex = QRegExp("\\\\textit\\{(.*)\\}|\\\\it\\{(.*)\\}",Qt::CaseInsensitive);
    rxItalicTex.setMinimal(true);
    while ( (pos = rxItalicTex.indexIn(instr)) != -1 )
    {
        int len = rxItalicTex.matchedLength();
        QString param = rxItalicTex.capturedTexts().last();
        if ( param.isEmpty() )
            param = rxItalicTex.cap(1);
        instr.replace(pos,len,"<i>"+param+"</i>");
    }

    // Убиваем недопустимые теги
    QRegExp rxHTMLTag = QRegExp("</?(.*)>",Qt::CaseInsensitive);
    rxHTMLTag.setMinimal(true);
    pos = 0;
    while ( (pos = rxHTMLTag.indexIn(instr,pos)) != -1 )
    {
        int len = rxHTMLTag.matchedLength();
        QStringList caps = rxHTMLTag.capturedTexts();
        QString value;
        if ( caps.count() > 1 )
            value = caps[1];
        if ( value.trimmed().toLower() != "b" )
            if ( value.trimmed().toLower() != "i" )
                if ( value.trimmed().toLower() != "arg" )
                    if ( value.trimmed().toLower() != "br" )
                        if ( value.trimmed().toLower() != "/b" )
                            if ( value.trimmed().toLower() != "/i" )
                                if ( value.trimmed().toLower() != "/arg" )
                                    instr.replace(pos,len,"&laquo;"+value+"&raquo;");
        pos += len;

    }

    // Записываем строку документации
    if ( id != -1 )
    {
        instr.remove(QChar(KS_DOC));
        instr = instr.trimmed();
        instr = instr.simplified();
        QString docString = functions.docString(id);
        if ( instr.isEmpty() )
            docString += "<br>";
        else
        {
            if ( !docString.isEmpty() )
                docString += " ";
            docString += instr;
        }
        functions.setDocString(id, docString);
        result.code = 0;
        return result;
    }
    else {
        result.code = TOS_OUT_OF_ALG;
        result.position = 0;
        result.length = in.length();
    }
}


void TextAnalizer::buildTables(int moduleId, QList<KumirInstruction> &data, SymbolTable &symbols, FunctionTable &functions)
{
    for (int i=0; i<data.size(); i++) {
        if (data[i].str_type==Kumir::AlgorhitmDeclaration) {
            Kumir::CompileError error = parceAlgorhitmHeader(moduleId, data[i], i, symbols, functions);
            if (error.code && !data[i].error) {
                data[i].error = error.code;
                data[i].err_start = error.position;
                data[i].err_length = error.length;
            }
        }
    }
}

enum FI_closes { IF_THEN, IF_ELSE, SWITCH, SWITCH_CASE, SWITCH_ELSE };


void TextAnalizer::analizeModule(int module_id, QList<KumirInstruction> &pvs, SymbolTable &symbols, FunctionTable &functions)
{

    m_denyVariableDeclaration = 0;

    int f_id = -1;


    QStack<FI_closes> fi_closes;
    for ( int i=0; i<pvs.count(); i++ ) {
        KumirInstruction pv = pvs[i];
        pv.extraSubExpressions.clear();
        Kumir::CompileError error;
        error.code = 0;
        error.position = 0;
        error.length = pv.line.length();

        QString xml;

        switch ( pv.str_type ) {

        case Kumir::Import: {
                error.code = ISP_USE_WRONG_POS;
                break;
            }

        case Kumir::AlgorhitmBegin: {
                error = parceAlgorhitmBegin(pv.line);
                xml = "<body>\n";
                break;
            }

        case Kumir::AlgorhitmEnd: {
                error = parceEnd(pv.line);
                xml = "</body>\n";
                break;
            }

        case Kumir::LoopBegin: {
                error = parseLoopBegin(pv, symbols, functions, i, xml);
                m_denyVariableDeclaration ++;
                break;
            }

        case Kumir::LoopEnd: {
                m_denyVariableDeclaration --;
                error = parceEndLoop(pv.line);
                xml = "</body>\n</loop>\n";
                break;
            }

        case Kumir::Else: {
                if (!fi_closes.isEmpty()) {
                    FI_closes l = fi_closes.pop();
                    if (l==IF_THEN) {
                        fi_closes.push(IF_ELSE);
                        xml = "</then>\n<else>\n";
                    }
                    if (l==SWITCH_CASE || l==SWITCH) {
                        fi_closes.push(SWITCH_ELSE);
                        xml = "</body>\n</case>\n<else>\n";
                    }
                }
                break;
            }

        case Kumir::Exit: {
                error = parceExit(pv.line);
                xml = "<exit/>";
                break;
            }

        case Kumir::If: {
                fi_closes.push(IF_THEN);
                error = parceIf( pv, symbols, functions, i, xml);
                break;
            }

        case Kumir::Switch: {
                fi_closes.push(SWITCH);
                m_denyVariableDeclaration ++;
                error = parceSwitch(pv,functions,symbols,i);
                xml = "<switch>\n";
                break;
            }

        case Kumir::Case: {
                if (!fi_closes.isEmpty()) {
                    FI_closes l = fi_closes.pop();
                    if (l==SWITCH_CASE) {
                        xml += "</body></case>\n";
                    }
                    fi_closes.push(SWITCH_CASE);
                }
                error = parceCase(pv,functions,symbols,i,xml);
                m_denyVariableDeclaration --;
                break;
            }

        case Kumir::Then: {
                m_denyVariableDeclaration ++;
                error = parceThen(pv.line);
                xml = "<then>\n";
                break;
            }

        case Kumir::EndSwitchOrIf: {
                error = parceDone(pv.line);
                if (!fi_closes.isEmpty()) {
                    FI_closes l = fi_closes.pop();
                    if (l==IF_THEN) {
                        xml = "</then>\n</if>\n";
                    }
                    if (l==IF_ELSE) {
                        xml = "</else>\n</if>\n";
                    }
                    if (l==SWITCH_CASE) {
                        xml = "</body>\n</case>\n</switch>\n";
                    }
                    if (l==SWITCH_ELSE) {
                        xml = "</else>\n</switch>\n";
                    }
                }
                m_denyVariableDeclaration --;
                break;
            }

        case Kumir::Assignment: {
                error = parceAssignment(pv,functions,symbols,i,xml);
                break;
            }

//        case PRISV_ERR: {
//                error = ANALIZ_MANY_PRISV;
//                break;
//            }

        case Kumir::Output: {
                error = parceOutput(pv,functions,symbols,i,xml);
                break;
            }

        case Kumir::FileOutput: {
                error = parceFileOutput(pv,functions,symbols,i,xml);
                break;
            }

        case Kumir::Assert: {
                error = parceAssert(pv,functions,symbols,i,xml);
                break;
            }

        case Kumir::Pre: {
                error = parcePre(pv,functions,symbols,i,xml);
                break;
            }

        case Kumir::Post: {
                error = parcePost(pv,functions,symbols,i,xml);
                break;
            }

        case Kumir::Input: {
                error = parceInput(pv,functions,symbols,i,xml);
                break;
            }

        case Kumir::FileInput: {
                error = parceFileInput(pv,functions,symbols,i,xml);
                break;
            }

        case Kumir::Documentation: {
                error = parceDocString(pv.line,functions,symbols,i,xml);
                break;
            }

        case Kumir::AlgorhitmCall: {
                error = parceAlgorhitmCall(pv,functions,symbols,i,xml);
                break;
            }

        case Kumir::ModuleEnd: {
                error = parceEndModule(pv.line);
                break;
            }

        case Kumir::VariableDeclaration: {
                error = parceVariablesDeclaration(pv,i,functions.nameByPos(i),symbols,functions,xml);
                break;
            }

        default: {
                break;
            }

        } // end switch

        if ( error.code > 0 && pv.error == 0 ) {
            pv.error = error.code;
            pv.err_start = error.position;
            pv.err_length = error.length;
        }
        if (error.code==0) {
            f_id = functions.idByPos(i);
            if (f_id>=0) {
                QString xmlF = functions.tempXmlData(f_id);
                xmlF += xml;
                functions.setTempXmlData(f_id, xmlF);
            }
            else {
//                module.xmlInitializerBody += xml;
            }
        }
        pvs[i] = pv;

    } // end for ( int i=0; i<module->Proga_Value()->count(); i++ )
    if ( !m_hiddenAlgorhitm.isEmpty() ) {
        int hiddenIP = pvs.size();
        for ( int i=0; i<m_hiddenAlgorhitm.count(); i++ ) {
            m_hiddenAlgorhitm[i].else_pos += hiddenIP;
        }
        KumirInstruction p = pvs[m_firstAlgorhitmIP];
        p.forceJump = hiddenIP;
        pvs[m_firstAlgorhitmIP] = p;
        pvs += m_hiddenAlgorhitm;
        //        module->setInitialIP(hiddenIP);
    }
    else {
        //        module->setInitialIP(app()->compiler->usesListCount());
    }
//    module->setInitialIP(app()->compiler->usesListCount());

}
