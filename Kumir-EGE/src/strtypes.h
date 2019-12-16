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
/*
Типы строк Кумир
V 0.1.5a
*/

#ifndef __STR_TYPES_H
#define __STR_TYPES_H

namespace Kum {

    enum InstructionType {
        Empty,
        VarDecl,
        AlgDecl,
        AlgBegin,
        AlgEnd,
        LoopBegin,
        LoopEnd,
        LoopEndCond,
        If,
        Then,
        Switch,
        Case,
        Else,
        Fin,
        Exit,
        Assign,
        Input,
        Output,
        FInput,
        FOutput,
        Assert,
        Pre,
        Post,
        ModBegin,
        ModEnd,
        UseMod,
        Include,
        Doc,
        Comment,
        AlgCall,
        Undefined = -1
    };
}

#include <QtCore>
Q_ENUMS(Kum::InstructionType);


#define  MAX_PEREM_IMPL_ID       6  //максимальное количество базовых типов переменных


//#define   EMPTY       0  //пустая строка


//#define   P_VAR_IMPL       1  //описание переменной
//#define   P_LOG_IMPL      2  //описание переменной
//#define   P_VESH_IMPL    3  //описание переменной
//#define   P_LIT_IMPL        4  //описание переменной
//#define   P_SIM_IMPL      5  //описание переменной


//#define   A_IMPL    12 //описание алгоритма
//#define   A_OTKR   13 //открывающая скобка алгоритма
//#define   A_ZAKR   14 //закрывабщая скобка алгоритма
//#define   O_OTKR   15 //откр. цикла. скобка нц
//#define   O_ZAKR   16 //закр. цикла.  скобка кц
//#define   ELSE     17 //else
//#define   C_OTKR   18 //откр. цикла. скобка

//#define   EXIT     19 //Команда выход.


//#define   IF_BEGINS   20 //начало описания IF
//#define   TO_OTKR   21 //начало описания IF
//#define   IF_ZAKR   22 //конец описания IF
//#define   VIBOR_BEGINS   23 //начало описания VIBOR
//#define   PRI_BEGINS   24 //начало при

//#define   PRISV   25 // :=
//#define   PRISV_ERR   26 // ошибочное := (ааа=1111) забытое :



//#define   VIVOD   27 //
//#define   UTV     29
//#define   DANO    30
//#define   NADO    31
//#define   VVOD    32
//#define   KC_PRI  33

//#define   ISP     34
//#define   ISP_END 35
//#define   USE_ISP 36

//#define   FVVOD  37
//#define   FVIVOD 38

//#define STR_INCLUDE 80

//#define		DOC				98 // API-документация
//#define   COMMENT   99 // коментарий.


//#define   ALG_USE   100 // другое (вызов алгоритма)

#endif
