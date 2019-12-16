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
#include "run.h"
#include "int_proga.h"
#include "strtypes.h"
#include "kassert.h"
#include "compiler.h"
#include "kummodules.h"
#include "kumsinglemodule.h"
#include "kum_instrument.h"
#include "stdio.h"
#include "tools.h"
#include "integeroverflowchecker.h"
#include "doubleoverflowchecker.h"
#define EPSILON 0.0001
#ifdef Q_OS_WIN32
#define MAX_CALL_DEEP 800
#include <time.h>
#elif defined(Q_OS_MAC)
#define MAX_CALL_DEEP 134
#else
#define MAX_CALL_DEEP 1800
#endif

using namespace Kum;

Run::Run(QObject *parent)
    : QObject(parent)
{
}

function_table* Run::Cur_Functions()
{
    KumSingleModule* module = Modules->module(m_runStack.top().moduleId);
    function_table* table = module->Functions();
    return table;
}

//symbol_table* Run::Cur_Symbols()
//{
//    KumSingleModule* module = Modules->module(m_runStack.top().moduleId);
//    symbol_table* table = module->Values();
//    return table;
//}

SymbolTable * Run::locals()
{
    Q_ASSERT(!m_runStack.isEmpty());
    return m_runStack.top().locals;
}

SymbolTable * Run::globals()
{
    Q_ASSERT(!m_runStack.isEmpty());

    KumSingleModule* module = Modules->module(m_runStack.top().moduleId);
    return module->globals();

}

SymbolTable * Run::tableForId(const int varId)
{
    if (locals() && locals()->hasMappedId(varId))
        return locals();
    else
        return globals();
}

void Run::init(KumModules* moduleTable,int mainModuleId, int mainModuleStartIp)
{
    i_lastError = 0;
    Modules = moduleTable;
    RunContext mainContext;
    mainContext.IP = mainModuleStartIp;
    mainContext.moduleId = mainModuleId;
    mainContext.finished = false;
    mainContext.initializationBlock = true;
    //    int mainFID = Modules->module(mainModuleId)->Functions()->idByPos(0);
    mainContext.locals = 0;
    m_runStack.push(mainContext);
}


//Выполнение программы
int Run::do_line(int str)
{
    int err=0;
    proga cl = Cur_Line(str);

    if( cl.error > 0)
    {
        return cl.error;

    }

    switch(cl.str_type)
    {
    case Kum::Empty:
        m_runStack.top().IP = str+1;
        break;
    case Kum::UseMod:
        m_runStack.top().IP = str+1;
        break;
    case Kum::ModBegin:
        m_runStack.top().IP = str+1;
        break;
    case Kum::ModEnd:
        m_runStack.top().IP = str+1;
        break;
    case Kum::Include:
        qFatal("Include instruction appeared in runtime");
        break;
    case Kum::Comment:
        qFatal("Comment instruction appeared in runtime");
        break;
    case Kum::Undefined:
        qFatal("Undefined instruction appeared in runtime");
        break;
    case Kum::Doc:
        m_runStack.top().IP = str+1;
        break;
    case Kum::AlgDecl:
        {
            int algId = Modules->module(m_runStack.top().moduleId)->Functions()->idByPos(str);
            int initializationIP = Modules->module(m_runStack.top().moduleId)->Functions()->initializationIP(algId);
            bool firstInitialized = Modules->module(m_runStack.top().moduleId)->Functions()->isInitialized(algId);
            if (initializationIP>-1 && !firstInitialized) {
                Modules->module(m_runStack.top().moduleId)->Functions()->setInitialized(algId, true);
                m_runStack.top().IP = initializationIP;
            }
            else {
                if (m_runStack.top().initializationBlock) {
                    // Run first algorhitm after initialization
                    m_runStack.top().initializationBlock = false;
                    m_runStack.top().finished = true; // Finish initialization context
                    RunContext firstAlgContext;
                    firstAlgContext.moduleId = m_runStack.top().moduleId;
                    firstAlgContext.IP = str;
                    firstAlgContext.finished = false;
                    int algId = Modules->module(firstAlgContext.moduleId)->Functions()->idByPos(str);
                    firstAlgContext.algName = Modules->module(firstAlgContext.moduleId)->Functions()->nameById(algId);
                    firstAlgContext.locals = Modules->module(firstAlgContext.moduleId)->Functions()->locals(algId);
                    m_runStack.push(firstAlgContext);
                }
                err=initPerem(str,m_runStack.top().moduleId);
                m_runStack.top().IP = cl.else_pos;
            }
            break;
        }
    case Kum::AlgBegin:
        m_runStack.top().IP = str+1;
        break;
    case Kum::LoopBegin:
        err=run_circle(str);
        break;
    case Kum::LoopEnd:
        {
            Q_ASSERT(!m_runStack.top().loopStack.isEmpty());
            int nextIP = m_runStack.top().loopStack.top().IP_begin;
            m_runStack.top().IP = nextIP;
            break;
        }
    case Kum::If:
        err=test_if(str);
        break;
    case Kum::Fin:
        m_runStack.top().IP = str+1;
        if (!m_runStack.top().switchStack.isEmpty())
            m_runStack.top().switchStack.pop();
        break;
    case Kum::Then:
        m_runStack.top().IP = str+1;
        break;
    case Kum::Else: {
            int elsepos = str + 1;
            if (m_runStack.top().switchStack.top().flag) {
                elsepos = Cur_Line(str).else_pos;
            }
            m_runStack.top().IP = elsepos;
            break;
        }

    case Kum::Exit:
        {
            if (!m_runStack.top().loopStack.isEmpty()) {
                // Break loop
                int beginIP = m_runStack.top().loopStack.top().IP_begin;
                int endIP = Cur_Line(beginIP).else_pos;
                m_runStack.top().loopStack.pop();
                m_runStack.top().IP = endIP+1;
            }
            else {
                // Exit from algorgitm
                if (!m_runStack.top().locals->hasReturnValue()) {
                    // Pop context only if we do not have return value.
                    // Otherwise context must be pulled by calling function
                    // after taking its return value.
                    m_runStack.pop();
                }
            }
            break;
        }
    case Kum::Assign:
        err=calc_prisv(str);
        break;
    case Kum::Output:
        err=do_vivod(str);
        break;
    case Kum::Input:
        err=do_vvod(str);
        break;
    case Kum::FInput:
        break;
    case Kum::FOutput:
        break;
    case Kum::Switch:
        err=do_vibor(str);
        break;
    case Kum::Case:
        err=do_pri(str);
        break;
    case Kum::Assert:
        err=do_utv(str);
        break;
    case Kum::LoopEndCond:
        err=do_break(str);
        break;
    case Kum::Pre:
        if(Cur_Line(str).VirajList.isEmpty()) {
            err=0;
            int elsepos = Cur_Line(str).else_pos;
            m_runStack.top().IP = elsepos;
        }
        else {
            err=do_dano(str);
            break;
        }
    case Kum::Post:
        {
            int elsepos = Cur_Line(str).else_pos;
            if(Cur_Line(str).VirajList.isEmpty()){
                err=0;
            }
            else{
                err=0;
                err=do_nado(str);
                if ( elsepos!= -1 )
                    m_runStack.top().IP = elsepos;
            }
            if (elsepos==-1) {
                m_runStack.top().finished = true;
                if (!m_runStack.top().locals->hasReturnValue())
                    m_runStack.pop();
            }
            break;
        }

    case Kum::AlgCall:
        err=do_funct(str);
        break;
    case Kum::AlgEnd:
        if(Cur_Line(str).else_pos>0) {
            // Evalueate algorhitm extra program (post-condition etc.)
            m_runStack.top().IP = Cur_Line(str).else_pos;
        }
        else {
            // Finish algorhitm evaluation
            //            if (!m_runStack.top().algName.isEmpty()) {
            //                Modules->module(m_runStack.top().moduleId)->localRestoreValues(m_runStack.top().algName);
            //            }
            //            m_runStack.pop();
            m_runStack.top().finished = true;
            if (!m_runStack.top().locals->hasReturnValue())
                m_runStack.pop();
        }
        break;
    case Kum::VarDecl:
        {
            err=initPerem(str,m_runStack.top().moduleId);
            m_runStack.top().IP = str+1;
            if (!Cur_Line(str).generatedLinesAfter.isEmpty()) {
                QList<proga> extra = Cur_Line(str).generatedLinesAfter;
                int baseStr = str;
                int subStr;
                int strIndex;
                for (int j=extra.count()-1; j>=0; j--) {
                    subStr = j+1;
                    strIndex = - ( baseStr*10 + subStr );
                    m_runStack.top().IP = strIndex;
                }
            }
        }
    }

    return err;
}


int Run::do_vivod(int str)
{


    //KumValueStackElem test(10);

    // QTextCodec *codec = QTextCodec::codecForName("KOI8-R");
    QString out="";
    //  int dbg=Cur_Line(str).VirajList.count();
    for(int i=0;i<Cur_Line(str).VirajList.count();i++)
    {
        if(Cur_Line(str).VirajTypes[i]==integer)
        {
            int error=0;
            int res=calc_simple_universal(Cur_Line(str).VirajList[i],
                                          Cur_Line(str).extraSubExpressions,
                                          Cur_Line(str).VirajList[i].last(),
                                          &error).asInt();
            if(error!=0)
            {
                //        if(!haltOnError)run_stack.push_back(str+1);
                return error;
            };
            sendOutput(QString::number(res));
        }

        if((Cur_Line(str).VirajTypes[i]==kumString)||(Cur_Line(str).VirajTypes[i]==charect))//09-03 ???
        {
            int err;
            QString res=calc_simple_universal(Cur_Line(str).VirajList[i],
                                              Cur_Line(str).extraSubExpressions,
                                              Cur_Line(str).VirajList[i].last(),
                                              &err).asString();
            //QString testrazb=Cur_Line(str).VirajList[i][2];
            if(err!=0){
                //                 if(!haltOnError)run_stack.push_back(str+1);
                return err;
            };
            sendOutput(res);
        };


        if(Cur_Line(str).VirajTypes[i]==real)
        {
            int error=0;
            double res=calc_simple_universal(Cur_Line(str).VirajList[i],
                                             Cur_Line(str).extraSubExpressions,
                                             Cur_Line(str).VirajList[i].last(),
                                             &error).asFloat();
            if(error!=0){
                //                 if(!haltOnError)run_stack.push_back(str+1);
                return error;
            };

            QString to_out = QString::number(res,'g',7);
            sendOutput(to_out);
        };

        if(Cur_Line(str).VirajTypes[i]==boolean)
        {
            int error=0;
            bool res=calc_simple_universal(Cur_Line(str).VirajList[i],
                                           Cur_Line(str).extraSubExpressions,
                                           Cur_Line(str).VirajList[i].last(),
                                           &error).asBool();
            if(error!=0){
                //                 if(!haltOnError)run_stack.push_back(str+1);
                return error;
            };

            if(res)sendOutput(trUtf8("да"));else sendOutput(trUtf8("нет"));
        };

        if(Cur_Line(str).VirajTypes[i]==none)
        {
            QStringList sList = Cur_Line(str).VirajList[i];
            if (
                    ( sList.count() == 1 )
                    &&
                    ( sList[0] == QString(QChar(KS_NS)) )
                    )
                sendOutput("\n");
        }
    }
    m_runStack.top().IP = str+1;
    return 0;
}







/**
 * Определение приоритета опреатора
 * @param oper Оператор
 * @return Приоритет от 0 до 100
 */
int Run::getOperPrior(QChar oper)
{
    if(oper=='0')
        return 99;
    if(oper=='!')
        return 100;
    if(oper=='(')
        return 90;
    if(oper==')')
        return 90;
    // begin NEW V.Y.
    if ( (oper==QChar(UNARY_MINUS)) || oper==(QChar(UNARY_PLUS)) )
        return 4;
    // end NEW V.Y.
    if(oper==QChar(KS_STEPEN))
        return 2; //Mordol 08-31
    if(oper=='*')
        return 3;
    if(oper=='/')
        return 3;
    if(oper=='+')
        return 4;
    if(oper=='-')
        return 4;
    if(oper=='>')
        return 5;
    if(oper=='<')
        return 5;
    if(oper=='=')
        return 5;
    if(oper==QChar(KS_BOLSHE_RAVNO))
        return 5;
    if(oper==QChar(KS_MENSHE_RAVNO))
        return 5;
    if(oper==QChar(KS_NE_RAVNO))
        return 5;

    if(oper==QChar(KS_OR))
        return 12;
    if(oper==QChar(KS_AND))
        return 11;
    if(oper==QChar(KS_NOT))
        return 10;

    return 0;
}


int Run::run_circle(int str)
{
    const QString line=Cur_Line(str).line;
    int ras_pos=line.indexOf(QChar(KS_RAZ));
    LoopType loopType = Free;
    if(line.length()>1) {
        if (ras_pos>2) {
            loopType = Times;
        }
        if(line[2]==QChar(KS_DLYA)) {
            loopType = For;
        }
        if(line[2]==QChar(KS_POKA)) {
            loopType = While;
        }
    }

    LoopContext context;
    bool isFirstTime = true;
    for (int i=0; i<m_runStack.top().loopStack.size(); i++) {
        LoopContext c = m_runStack.top().loopStack[i];
        if (c.IP_begin==str) {
            context = c;
            isFirstTime = false;
            break;
        }
    }

    int error = 0;

    if (isFirstTime) {

        context.IP_begin = str;

        switch (loopType) {

        case Times:
            {
                int timesValue = calc_simple_universal(Cur_Line(str).VirajList[0],
                                                       Cur_Line(str).extraSubExpressions,
                                                       Cur_Line(str).VirajList[0].last(),
                                                       &error).asInt();
                if (error)
                    return error;
//                if (timesValue<=0) {
//                    return 0; // Dummy loop
//                }
                context.fromValue = 0;
                context.loopValue = 0;
                context.toValue = timesValue;
                context.stepValue = 1;
                break;
            }
        case For:
            {
                int ot_pos=line.indexOf(QChar(KS_OT));
                QString perem=line.mid(5,ot_pos-5);
                int perem_t_id=perem.toInt();


                int ot = calc_simple_universal(Cur_Line(str).VirajList[0],
                                               Cur_Line(str).extraSubExpressions,
                                               Cur_Line(str).VirajList[0].last(),
                                               &error).asInt();
                int do_v = calc_simple_universal(Cur_Line(str).VirajList[1],
                                                 Cur_Line(str).extraSubExpressions,
                                                 Cur_Line(str).VirajList[1].last(),
                                                 &error).asInt();
                if(error!=0)
                    return error;
                int shag=1;
                int StepPosition = Cur_Line(str).line.indexOf(QChar(KS_SHAG));

                if(StepPosition > -1)
                {
                    shag=calc_simple_universal(Cur_Line(str).VirajList[2],
                                               Cur_Line(str).extraSubExpressions,
                                               Cur_Line(str).VirajList[2].last(),
                                               &error).asInt();
                    if(error!=0)
                        return error;
                }
                if (shag > 0)
                {
                    if ( (shag>0 && ot>do_v) || (shag<0 && ot<do_v) ) // Reverse loop
                    {
                        m_runStack.top().IP = Cur_Line(str).else_pos+1;
                        return 0;
                    }
                }
                context.fromValue = ot;
                context.toValue = do_v;
                context.stepValue = shag;
                int initialValue = ot-shag;
                tableForId(perem_t_id)->setValue(perem_t_id, initialValue);
                context.variableID= perem_t_id;
                context.loopValue = initialValue;
                break;
            }

        default:
            break;
        } // end switch loopType in first time initialization
        m_runStack.top().loopStack.push(context);
    } // end if firstTime


    switch (loopType) {
    case Times: {
            m_runStack.top().loopStack.top().loopValue ++;
            if (m_runStack.top().loopStack.top().loopValue>m_runStack.top().loopStack.top().toValue) {
                // Reached end of loop
                m_runStack.top().loopStack.pop();
                m_runStack.top().IP = Cur_Line(str).else_pos+1;
            }
            else {
                // Process loop body
                m_runStack.top().IP = str+1;
            }
            break;
        }
    case For: {
            int variableId = m_runStack.top().loopStack.top().variableID;
            int step = m_runStack.top().loopStack.top().stepValue;
            SymbolTable * table = tableForId(variableId);
            int prevValue = m_runStack.top().loopStack.top().loopValue;
            int newValue = prevValue + step;
            int stopValue = m_runStack.top().loopStack.top().toValue;
            m_runStack.top().loopStack.top().loopValue = newValue;
            table->setValue(variableId, newValue);
            bool reachedEnd = (newValue>stopValue && step>=0) || (newValue<stopValue && step<0);
            if (reachedEnd) {
                // Reached end of loop
                if (!isFirstTime)
                    table->setValue(variableId, prevValue);
                m_runStack.top().loopStack.pop();
                m_runStack.top().IP = Cur_Line(str).else_pos+1;
            }
            else {
                // Process loop body
                m_runStack.top().IP = str+1;
            }
            break;
        }
    case While: {
            bool res = calc_simple_universal(Cur_Line(str).VirajList[0],
                                             Cur_Line(str).extraSubExpressions,
                                             Cur_Line(str).VirajList[0].last(),
                                             &error).asBool();
            if(error!=0)
                return error;
            if (res) {
                // Process loop body
                m_runStack.top().IP = str+1;
            }
            else {
                // Reached end of loop
                m_runStack.top().loopStack.pop();
                m_runStack.top().IP = Cur_Line(str).else_pos+1;
            }
            break;
        }
    case Free: {
            m_runStack.top().IP = str + 1;
            break;
        }
    }

    return 0;

}


KumValueStackElem Run::calc_simple_universal(const QStringList &simple_viraj,
                                             const QStringList &extraExpressions,
                                             const QString &expression,
                                             int* error,
                                             KumValueStackElem *newValue,
                                             QString *debugString) // VY 07-16
{
    //														A. Пролог
    //									A1. Описание переменных
    // Знаки операций
    // 	QString opers_string="[+|*|\\-|)|(|/|>|<|=|"+QString(QChar(KS_OR))+"|"+QString(QChar(KS_NE_RAVNO))
    // 			+"|"+QString(QChar(KS_BOLSHE_RAVNO))+"|"+QString(QChar(KS_MENSHE_RAVNO))+"|"+QString(QChar(KS_NOT))+"|"+QString(QChar(KS_AND))+QString(QChar(UNARY_PLUS))+"|"+
    // 			QString(QChar(UNARY_MINUS))+"|"+
    // 			QString(QChar(KS_STEPEN))+"]";


    QString opers_string="[+*\\-)(/><="+QString(QChar(KS_OR))+QString(QChar(KS_NE_RAVNO))
                         +QString(QChar(KS_BOLSHE_RAVNO))+QString(QChar(KS_MENSHE_RAVNO))+QString(QChar(KS_NOT))+QString(QChar(KS_AND))+QString(QChar(UNARY_PLUS))+
                         QString(QChar(UNARY_MINUS))+
                         QString(QChar(KS_STEPEN))+"]";
    //	Они же - как регулярное выражение
    QRegExp oper_rx=QRegExp(opers_string);
    // Стек значений переменных
    QList<KumValueStackElem>         perems;
    // Стек операций
    QList<QChar>                               opers;
    //  Текущая позиция в исходной строке
    int pos=0;						// 0 - начало
    //  Текущий позиция оператора
    int  oper=0;						//  0 - начало
    //								A2.  Инициализация переменных.
    perems.clear();

    pos=0;
    oper=0;
    opers.append('0');            // заносим признак начала в стек операторов
    *error = 0;

    int op_id=-1;
    bool optimizFlag=false;
    //                               A3.   Основной цикл
    //  Цикл прерывается, когда больше не найдено операторов
    while(oper>-1)
    {
        QString perem; // подстрока, содержащая операнд
        // Ищем оператор начиная со строки pos
        oper=oper_rx.indexIn(expression,pos);

        if ((oper==-1)&&(pos==0))
        {
            // если оператор не найден и не достигнут конец строки,
            // то подразумевается, что все выражение состоит из операнда
            perem=expression;
        }
        else
        {
            // Если оператор найден, то
            // выделяем операнд (perem) как вырезку из строки выражения [pos,oper)
            perem=expression.mid(pos,oper-pos);
        }

        // ---- Определение текущего оператора

        // текущий оператор
        QChar operatorS;
        // если оператор не был найден,
        // то он равен псевдооператору '!', означающему "конец вычислений"
        if (oper==-1)
        {
            operatorS='!';
        }
        else
        {
            // если оператор был найден, то operatorS равен символу на позиции oper
            operatorS=expression[oper];

        }

        // ---------------------------------------- разбираемся с тем, что находится перед оператором

        // проверка на пустой операнд
        if  ( perem.isEmpty() )
        {
            pos = oper+1;
            opers.append(operatorS);
            continue;
        }

        // ------ ??????????????
        if(optimizFlag)
            perems.append(KumValueStackElem(dummy,0));

        // выделяем номер в таблице символов
        int perem_t_id=perem.mid(1).toInt();

        // Переменная или константа
        if((perem[0]=='^')&&(!optimizFlag))
        {
            SymbolTable *curTable = tableForId(perem_t_id);
            if (newValue!=NULL) {
                switch (newValue->getType()) {
                case integer:
                    curTable->setValue(perem_t_id, newValue->asInt());
                    break;
                case real:
                    curTable->setValue(perem_t_id, newValue->asFloat());
                    break;
                case bool_or_num:
                case bool_or_lit:
                case boolean:
                    curTable->setValue(perem_t_id, newValue->asBool());
                    break;
                case charect:
                    curTable->setValue(perem_t_id, newValue->asCharect());
                    break;
                case kumString:
                    curTable->setValue(perem_t_id, newValue->asString());
                    break;
                default:
                    *error=RUN_NO_VALUE;
                    break;
                }
                return KumValueStackElem(0);
            }
            else {
                // проверяем на наличие значения
                KumValueStackElem elem;
                if (!curTable->isInitialized(perem_t_id)) {
                    elem = KumValueStackElem(dummy,RUN_NO_VALUE);
                }
                else { //Есть ли значение у переменной
                    // если не отругались на ошибку ранее, то добавляем в стек значение перем.
                    QVariant val = curTable->value(perem_t_id);
                    if (!val.isValid()) {
                        *error=RUN_NO_VALUE;
                        return KumValueStackElem(0);
                    }
                    switch (curTable->type(perem_t_id)) {
                    case integer:
                        elem = KumValueStackElem(val.toInt());
                        break;
                    case real:
                        elem = KumValueStackElem(val.toDouble());
                        break;
                    case boolean:
                        elem = KumValueStackElem(val.toBool());
                        break;
                    case kumString:
                        elem = KumValueStackElem(val.toString());
                        break;
                    case charect:
                        elem = KumValueStackElem(val.toChar());
                        break;
                    default:
                        *error=RUN_NO_VALUE;
                        break;
                    }


                }

                perems.append(elem);
                if (operatorS==QChar(KS_OR) && elem.asBool()) {
                    return elem;
                }
                if (operatorS==QChar(KS_AND) && (!elem.asBool())) {
                    return elem;
                }
            }

        };

        // Подвыражения
        if((perem[0]=='&')&&(!optimizFlag))
        {
            // код ошибки
            int err=0;
            // выделяем номер в таблице символов
            int index=perem.mid(1).toInt();

            // ======== Блок, от которого хотим избавиться.
            // ======== Здесь определяется тип подвыражения,
            // ======== и в зависимости от типа вызываем
            // ======== соответствующий calc_simple_*.
            /*
   PeremType exprType = getSimpleExprType(simple_viraj,perem_id);
   // Заплатка :( По идее лучше чтоб calc_simple_bool возвращал
   // не bool а KumValueStackElem и с ним и работать без разбора типов.
   if ( exprType == real )
   perems.append(calc_simple_double(simple_viraj,perem_id,  &err));
   if ( exprType == boolean )
   perems.append(calc_simple_bool(simple_viraj,perem_id,  &err));
   if ( exprType == integer )
   perems.append(calc_simple_int(simple_viraj,perem_id,  &err));
   if ( (exprType == kumString) || (exprType == charect)  )
   perems.append(calc_simple_string(simple_viraj,perem_id,  &err));
  */
            // ======== Конец блока.

            // 	Здесь мы рекурсивно вызываем себя для вычисления подвыражения
            //    с номером perem_id и добавляем в стек результат вычисления.
            KumValueStackElem elem = calc_simple_universal(simple_viraj, extraExpressions, simple_viraj[index], &err);
            if (err) {
               *error = err;
               return elem;
            }
            if(elem.Error()!=0){*error=elem.Error();return elem;};
            perems.append(elem);
            if (operatorS==QChar(KS_OR) && elem.asBool()) {
                return elem;
            }
            if (operatorS==QChar(KS_AND) && (!elem.asBool())) {
                return elem;
            }

        };

        if (perem[0]=='#') {
            // Подвыражения из списка extraExpressions
            // (дополнительные псевдо-скобки для операций "или" и "и")
            int err=0;
            int index=perem.mid(1).toInt();
            KumValueStackElem elem = calc_simple_universal(simple_viraj, extraExpressions, extraExpressions[index], &err);
            if(elem.Error()!=0){*error=elem.Error();return elem;};
            perems.append(elem);
            if (operatorS==QChar(KS_OR) && elem.asBool()) {
                return elem;
            }
            if (operatorS==QChar(KS_AND) && (!elem.asBool())) {
                return elem;
            }
        }

        //Массивы
        if((perem[0]=='%')&&(!optimizFlag))
        {
            int err = 0;
            // Выделяем аргументы массива
            int emp_pos=perem.indexOf('&');
            if(emp_pos==-1)emp_pos=perem.length();
            // выделяем блок до &
            QString temp;
            temp=perem.mid(1,emp_pos-1);
            // номер массива в таблице символов
            int mass_t_id=temp.toInt();
            SymbolTable *curTable = tableForId(mass_t_id);

            if ( newValue!=NULL ) {
                const QString mass_args = perem.mid(emp_pos,perem.length()-emp_pos);
                setVectorData(curTable, mass_t_id,
                              mass_args, simple_viraj,
                              extraExpressions,
                              newValue->asVariant(),
                              &err,
                              debugString);
                if (err>0) {
                    *error = err;
                }
                return KumValueStackElem(0);
            }
            else {

                QVariant val = vectorData(curTable, mass_t_id,
                                          perem.mid(emp_pos,perem.length()-emp_pos),
                                          simple_viraj,
                                          extraExpressions,
                                          &err);

                if (err!=0) {
                    *error = err;
                    return KumValueStackElem(0);
                }

                if (!val.isValid()) {
                    *error = RUN_NO_MASS_VALUE;
                    return KumValueStackElem(0);
                }

                KumValueStackElem value = KumValueStackElem(val);
                perems.append(value);
                if (operatorS==QChar(KS_OR) && value.asBool()) {
                    return value;
                }
            }


            // Если при выделении из массива возникла ошибка -- выход

            if ( err > 0 )
            {
                perems.pop_back();
                perems.append(KumValueStackElem(dummy,err));
            }
        }

        // Функции
        if((perem[0]=='@')&&(!optimizFlag))
        {

            // номер функции в таблице функций
            int funct_id;
            int mod_id;
            getFuncParam(perem,&funct_id,&mod_id);
            // определяем имя функции
            QString f_name=Modules->module(mod_id)->Functions()->nameById(funct_id);
            KumValueStackElem value;
            // функция возвращает свое значение в одноименную глобальную перем.,
            // поэтому определяем ID этой переменной
            //int isp_id=ispolniteli->inTable(Cur_Functions()->func_table[funct_id].isp);

            //if(Modules->isIntsrument(mod_id))mod_id=0;
            //            int f_perem_id=Modules->module(mod_id)->Values()->inTable(f_name,"global");

            //            if(f_perem_id<0)qWarning("RUN::calc_simple_universal:f_perem_id<0!!!!");
            int err=0;
            if (mod_id==0 || mod_id==1) {
                QVariant val;
                err = evaluateBuiltInFunction(perem, simple_viraj, extraExpressions, val);
                if (err) {
                    *error = err;
                    return KumValueStackElem(0);
                }
                value = KumValueStackElem(val);
            }
            else {
                err = pushNewFunctionContext(perem, perem, simple_viraj, extraExpressions);
                if (err) {
                    *error = err;
                    return KumValueStackElem(0);
                }
                // TODO: Improve me!!!
                while (hasMoreInstructions()) {
                    evaluateNextInstruction();
                    err = lastErrorCode();
                    if (err) {
                        *error = err;
                        break;
                    }
                }
                // end todo
                if (!m_runStack.top().locals->returnValue().isValid()) {
                    *error=RUN_NO_VALUE;
                    return KumValueStackElem(0);
                }
                QVariant val = m_runStack.top().locals->returnValue();
                switch (m_runStack.top().locals->returnValueType()) {
                case integer:
                    value = KumValueStackElem(val.toInt());
                    break;
                case real:
                    value = KumValueStackElem(val.toDouble());
                    break;
                case boolean:
                    value = KumValueStackElem(val.toBool());
                    break;
                case kumString:
                    value = KumValueStackElem(val.toString());
                    break;
                case charect:
                    value = KumValueStackElem(val.toChar());
                    break;
                default:
                    break;
                }

                m_runStack.pop(); // We don't need launched context indformation after we know it's return value
            }
            perems.append(value);
        }

        //Вырезка
        if((perem[0]==QChar(TYPE_SKOBKA))&&(!optimizFlag))
        {
            // Количество подвыражений
            // если 1 -- то отдельный символ
            // если 2 -- то подстрока
            int diez_count = perem.count("&");

            //		Выделяем блок до &
            int diez_pos=perem.indexOf("&");
            // Определяем ID переменной строки
            QString id_str=perem.mid(1,diez_pos-1);
            int perem_t_id=id_str.toInt();

            SymbolTable *curTable = tableForId(perem_t_id);
            if(!curTable->isInitialized(perem_t_id)&&!newValue)
            {
                *error = RUN_NO_VALUE;
                return KumValueStackElem("");
            }

            // temp_string -- значение исходной строки
            QString temp_string = curTable->value(perem_t_id).toString();

            // индексы вырезки
            int index1, index2;

            //Обращение к конкретному символу.
            if ( diez_count == 1 )
            {
                // определяем ID подвыражения для первого аргумента &
                id_str = perem.mid(diez_pos+1,perem.length()-diez_pos-1);
                int var_id = id_str.toInt();
                //Подсчет индекса
                int err=0;
                index1 = calc_simple_universal(simple_viraj,extraExpressions,simple_viraj[var_id],&err).asInt();
                // Если ошибка -- выход
                if ( err > 0 ) {
                    perems.append(KumValueStackElem(dummy,err));
                }
                else { // тип None?

                    int templ = temp_string.length();

                    if (//Проверка правильности индекса
                            ( index1 > templ )
                            ||
                            ( index1 < 1 )
                            )
                    {

                        perems.append(KumValueStackElem(dummy,RUN_STRING_BOUND_1));
                    }
                    else if (newValue) {
                        // Добавляем в стек символ из строки
                        temp_string.replace(index1-1,1,newValue->asString());
                        curTable->setValue(perem_t_id, temp_string);
                        return KumValueStackElem(0);
                    }
                    else {
                        KumValueStackElem value = KumValueStackElem(QChar(temp_string[index1-1]));
                        perems.append(value);
                    }
                }
            }
            else //Вырезка
            {
                // ищем второе вхождение &
                int next_diez_pos = perem.indexOf("&",diez_pos+1);
                // выделяем блок между & и &
                id_str = perem.mid(diez_pos+1, next_diez_pos-diez_pos-1);
                // ID первого индекса вырезки
                int var_id = id_str.toInt();
                //Начало вырезки
                int err=0;
                index1 = calc_simple_universal(simple_viraj,extraExpressions,simple_viraj[var_id],&err).asInt();

                // выделяем блок после второго &
                id_str = perem.mid(next_diez_pos+1,perem.length()-next_diez_pos-1);
                // ID второго индекса вырезки
                var_id = id_str.toInt();
                //Конец вырезки

                index2 = calc_simple_universal(simple_viraj,extraExpressions,simple_viraj[var_id],&err).asInt();
                if ( err > 0 )
                {
                    perems.append(KumValueStackElem(dummy,err));
                }
                else {
                    int templ = temp_string.length();
                    if ( //Проверка правильности индексов
                            ( index2 > templ )
                            ||
                            ( index1 < 1 )
                            ||
                            ( index1 > index2 )
                            )
                    {
                        perems.append(KumValueStackElem(dummy,RUN_STRING_BOUND_2));
                    } else if (newValue) {
                        // Добавляем в стек подстроку [index1,index2]
                        temp_string.replace(index1-1,index2-index1+1,newValue->asString());
                        curTable->setValue(perem_t_id, temp_string);
                        return KumValueStackElem(0);
                    }
                    else {
                        KumValueStackElem value = KumValueStackElem(temp_string.mid(index1-1,index2-index1+1));
                        perems.append( value);
                    }
                }
            }
        }

        if(operatorS==QChar(KS_AND) && (!perems.last().asBool()) && !optimizFlag && (perems.last().getType()==boolean) )
        {
            optimizFlag=true;
            op_id=opers.count();
        };
        if(operatorS==QChar(KS_OR) && (perems.last().asBool()) && !optimizFlag && (perems.last().getType()==boolean) )
        {
            if (opers.last()!=QChar(KS_NOT)) {
                optimizFlag=true;
                op_id=opers.count();
            }
        };
        // --------------------------------------------- конец разбора операнда

        // getOperPrior возвращает порядок вычисления оператора,
        // бОльший порядок вычисления соответствует мЕньшему приоритету.

        // getOperPrior(operatorS) -- порядок текущего оператора
        // getOperPrior(opers.last()) -- порядок последнего оператора в стеке

        if(getOperPrior(operatorS)>=getOperPrior(opers.last()))
        {

            // Если текущий оператор менее приоритетный, чем последний в стеке.

            // Вычисление содержимого стека до тех пор, пока
            // там не останется операторов, менее приоритетных,
            // чем текущий
            while(getOperPrior(operatorS)>=getOperPrior(opers.last()))
            {
                // Поледней оператор из стека
                *error=0;
                QChar cur_op=opers.last();
                opers.pop_back();

                if(cur_op=='0')break; //Стек закончился.

                // Последнее значение в стеке
                KumValueStackElem firstP=perems.last();
                if (perems.empty())
                {
                    *error=2;
                    return KumValueStackElem(99);
                }
                perems.pop_back();

                // Предпоследнее значение в стеке
                KumValueStackElem secondP;

                // Берем значение из стека только в том случае,
                // если оператор бинарный
                if((cur_op!=QChar(UNARY_MINUS))
                    &&(cur_op!=QChar(UNARY_PLUS))
                    &&(cur_op!=QChar(KS_NOT))) //Для унарных операторов
                    {secondP=perems.last();
                    perems.pop_back();};

                //Вычисление.
                // В зависимости от оператора вызывается
                // соответствующая функция класса
                // KumValueStackElem, которая сама разбирается,
                // что делать с конкретными значениями различных типов данных.
                // На этом этапе предполагается, что соответствие типов
                // является корректным, т.к. было проверено на этапе разбора.

                if(cur_op=='+') {
                    perems.append(KumValueStackElem::Summ(firstP,secondP,error));
                }
                if(cur_op==QChar(UNARY_MINUS)) {
                    KumValueStackElem value = KumValueStackElem::Minus(KumValueStackElem(0),firstP,error);
                    perems.append(value);
                }
                if(cur_op==QChar(UNARY_PLUS)) {
                    perems.append(firstP);
                }
                if(cur_op=='-') {
                    perems.append(KumValueStackElem::Minus(secondP,firstP,error));
                }
                if(cur_op=='*') {
                    perems.append(KumValueStackElem::Multi(firstP,secondP,error));
                }
                if(cur_op=='/'){
                    if((firstP.Error()>0)&&(!optimizFlag)) {
                        *error=firstP.Error();
                        return KumValueStackElem(0);
                    }
                    //                  if((firstP.asFloat()==0)&&(!optimizFlag)) {
                    //                     perems.append(KumValueStackElem(dummy,RUN_DIV_ZERO));
                    //               }
                    if(secondP.Error()>0) {
                        perems.append(KumValueStackElem(dummy,secondP.Error()));
                    }
                    else if(firstP.Error()>0) {
                        perems.append(KumValueStackElem(dummy,firstP.Error()));
                    }
                    else {
                        perems.append(KumValueStackElem::Div(secondP, firstP,error));
                    }
                }
                if(cur_op==QChar(KS_STEPEN)){
                    perems.append(KumValueStackElem::Pow(secondP,firstP,error));
                }
                if(cur_op=='='){
                    perems.append(KumValueStackElem::compare(secondP,firstP));
                }

                if(cur_op=='>'){
                    perems.append(KumValueStackElem::More(secondP,firstP));
                }

                if(cur_op=='<'){
                    perems.append(KumValueStackElem::Less(secondP,firstP));
                }

                if(cur_op==QChar(KS_NE_RAVNO)){
                    perems.append(KumValueStackElem::NotEq(secondP,firstP));
                }


                if(cur_op==QChar(KS_BOLSHE_RAVNO))
                {
                    perems.append(KumValueStackElem::GEQ(secondP,firstP));
                }

                if(cur_op==QChar(KS_MENSHE_RAVNO))
                {
                    perems.append(KumValueStackElem::LEQ(secondP,firstP));
                }


                if(cur_op==QChar(KS_OR)){
                    KumValueStackElem value = KumValueStackElem::Or(secondP,firstP) ;

                    bool debug1, debug2;
                    debug1 = firstP.asBool();
                    debug2 = firstP.asBool();
                    perems.append( value );
                    bool secondValue = secondP.asBool();
                    bool firstIsDummy = firstP.getType()==dummy;

                    if(!secondValue && firstIsDummy)
                    {
                        *error=firstP.Error();
                        return KumValueStackElem(0);
                    };
                    if(op_id==opers.count())
                    {
                        optimizFlag=false;
                        op_id=-1;
                    };
                }

                if(cur_op==QChar(KS_AND)){
                    KumValueStackElem value = KumValueStackElem( KumValueStackElem::And(secondP,firstP) );

                    perems.append(value);
                    if(secondP.asBool() && (firstP.getType()==dummy))
                    {
                        *error=firstP.Error();
                        return KumValueStackElem(0);
                    };
                    if(op_id==opers.count())
                    {
                        optimizFlag=false;
                        op_id=-1;
                    };

                }

                if(cur_op==QChar(KS_NOT)){
                    bool value = firstP.asBool();
                    value = ! value;
                    perems.append(KumValueStackElem(value));
                }

                if(*error!=0)
                    return KumValueStackElem(0);
            };
            // конец вычисления
            opers.append(operatorS);
        }
        else
        {
            // Случай, когда текущий оператор более приоритетный,
            // чем последний в стеке.
            // Добавляем оператор в стек. Он будет вычислен позже
            // (когда встретится (псевдо-)оператор менее приоритетный)
            opers.append(operatorS);
        }
        // Устанавливаем текущую позицию в строке на след. после
        // оператора символ
        pos=oper+1;

    }//Конец основного цикла

    KumValueStackElem calc_result=perems.last();
    if(calc_result.getType()==dummy)*error=calc_result.Error();
    return calc_result; // ( без приведения к какому-либо конкретному типу )
    // Было:  return calc_result.asBool();
};


/**
 * Инициализация переменных
 * @param str 
 * @return 
 */
int Run::initPerem(int str,int module)
{
    proga p = Modules->module(module)->Proga_Value_Line(str);
    QString work_stri=p.line;

    int o_sk_pos=work_stri.indexOf('(');
    int z_sk_pos=work_stri.indexOf(')');
    if (p.VirajList.size()==0) {
        return 0;
    }
    work_stri=work_stri.mid(o_sk_pos+1,z_sk_pos-o_sk_pos-1);

    QStringList perems=work_stri.split(',',QString::SkipEmptyParts);
    for(int j=0;j<perems.count();j++)
    {
        const QString work_str=perems[j];
        int sharpPos = work_str.indexOf("#")==-1? work_str.length() : work_str.indexOf("#");
        const QString decl = work_str.mid(1,sharpPos-1);
        const int varId = decl.toInt();
        SymbolTable *curTable = tableForId(varId);
        PeremType type = curTable->type(varId);
        if((type==mass_integer)||(type==mass_string)||(type==mass_charect)||(type==mass_real)||
           (type==mass_bool))
        {
            QList<int> bounds;
            const QStringList terms = work_str.mid(sharpPos+1).split("#");
            for (int i=0; i<terms.size(); i++) {
                const ValueReference ref = curTable->reference(varId);
                int error=0;
                const QStringList mainExpressions = Modules->module(m_runStack.top().moduleId)->Proga_Value_Line(str).VirajList[terms[i].toInt()];
                const QStringList extrExpressions = Modules->module(m_runStack.top().moduleId)->Proga_Value_Line(str).extraSubExpressions;
                int res = calc_simple_universal(mainExpressions,
                                                extrExpressions,
                                                mainExpressions.last(),
                                                &error).asInt();

                if(error!=0)
                    return error;
                bounds << res;
            }

            Q_ASSERT (bounds.size()%2==0);
            int error = curTable->setBounds(varId, bounds);
            if (error)
                return error;
        }
    }
    return 0;
}

int Run::testVectorIndeces(const SymbolTable *table, int id, const QVector<int> &indeces)
{
//    if (indeces[0]<0)
//        return RUN_NEG_INDEX;
    QPair<int,int> bounds0 = table->vectorBounds(id,0);
    if (indeces[0]<bounds0.first || indeces[0]>bounds0.second)
        return RUN_ARRAY_BOUND;
    if (table->dimension(id)>1) {
        QPair<int,int> bounds1 = table->vectorBounds(id,1);
//        if (indeces[1]<0)
//            return RUN_NEG_INDEX_2;
        if (indeces[1]<bounds1.first || indeces[1]>bounds1.second)
            return RUN_ARRAY_BOUND_2;
    }
    if (table->dimension(id)>2) {
        QPair<int,int> bounds2 = table->vectorBounds(id,2);
//        if (indeces[2]<0)
//            return RUN_NEG_INDEX_3;
        if (indeces[2]<bounds2.first || indeces[1]>bounds2.second)
            return RUN_ARRAY_BOUND_3;
    }
    return 0;
}




int Run::calc_prisv(int str)
{
    // сслылки на подвыражения в левой и правой частях
    int leftIndex, rightIndex;
    QString line;
    line = Cur_Line(str).line;
    QStringList links = line.split(QChar(KS_PRISV));
    K_ASSERT( links.count()==2 );
    bool ok;
    leftIndex = links[0].mid(1).toInt(&ok);
    K_ASSERT(ok);
    rightIndex = links[1].mid(1).toInt(&ok);
    K_ASSERT(ok);


    // значение правой части выражения
    KumValueStackElem rightValue;

    // вычисляем значение правой части выражения
    int local_error = 0;
    rightValue = calc_simple_universal(Cur_Line(str).VirajList[0],
                                       Cur_Line(str).extraSubExpressions,
                                       Cur_Line(str).VirajList[0][rightIndex],
                                       &local_error);


    if(local_error!=0)
        return local_error;

    // устанавливаем новое значение в левой части

    QString debugString;
    calc_simple_universal(Cur_Line(str).VirajList[0],
                          Cur_Line(str).extraSubExpressions,
                          Cur_Line(str).VirajList[0][leftIndex],
                          &local_error,
                          &rightValue,
                          &debugString);

    if(local_error>0)
        return local_error;


    m_runStack.top().IP = str+1;
    return 0;
}



QList<int> Run::calcMassPos(const QString &mass_viraj,
                            const QStringList &simple_viraj,
                            const QStringList &extraSubexprList,
                            int *err)
{
    Q_UNUSED(err);
    QList<int> temp;
    int link_pos=mass_viraj.indexOf("&");
    // mainform->writeMain("MASS_VIRAJ="+mass_viraj);
    int mvLength = mass_viraj.length();
    while(link_pos<mvLength)
    {
        int pos=mass_viraj.indexOf("&",link_pos+1);
        if(pos==-1) pos=mass_viraj.length();
        QString str_id=mass_viraj.mid(link_pos+1,pos-link_pos-1);
        // mainform->writeMain("MASS_ID="+str_id);
        int error;
        int mass_pos=calc_simple_universal(simple_viraj,
                                           extraSubexprList,
                                           simple_viraj[str_id.toInt()],
                                           &error).asInt();
        temp.append(mass_pos);

        // mainform->writeMain("MASS_POS="+QString::number(mass_pos));

        link_pos=pos;
    };
    return temp;

};



QVector<int> Run::calculateVectorIndeces(const QString &mass_args,
                                         const QStringList &expressions,
                                         const QStringList &extraSubexprList,
                                         int *err, QString *debugString)
{
    Q_UNUSED(debugString);
    QStringList args=mass_args.split('&',QString::SkipEmptyParts);
    Q_ASSERT(args.size()<=3);
    QVector<int> result(3);
    for (int i=0; i<args.size(); i++) {
        int argId = args[i].toInt();
        result[i] = calc_simple_universal(expressions,
                                          extraSubexprList,
                                          expressions[argId],
                                          err).asInt();
    }
    return result;
}


void Run::setVectorData(SymbolTable *table,
                        int id,
                        const QString &mass_args,
                        const QStringList &viraj,
                        const QStringList &extraSubexprList,
                        const QVariant &value,
                        int *error,
                        QString *debugString)
{
    QVector<int> intArgs = calculateVectorIndeces(mass_args,
                                                  viraj,
                                                  extraSubexprList,
                                                  error,
                                                  debugString);
    if (*error!=0)
        return;
    *error = testVectorIndeces(table, id, intArgs);
    if (*error!=0)
        return;
    table->setValue(id, intArgs[0], intArgs[1], intArgs[2], value);
}

QVariant Run::vectorData(const SymbolTable *table, int id,
                         const QString &mass_args,
                         const QStringList &viraj,
                         const QStringList &extraSubexprList,
                         int *error)
{
    QVector<int> intArgs = calculateVectorIndeces(mass_args,
                                                  viraj,
                                                  extraSubexprList,
                                                  error);
    if (*error!=0)
        return QVariant();
    *error = testVectorIndeces(table, id, intArgs);
    if (*error!=0)
        return QVariant();
    return table->value(id, intArgs[0], intArgs[1], intArgs[2]);
}

/**
 * кц_при
 * @return код ошибки
 */
int Run::do_break(int str) //15-09-Mordol
{
    int error=0;

    bool res=calc_simple_universal(Cur_Line(str).VirajList[0],
                                   Cur_Line(str).extraSubExpressions,
                                   Cur_Line(str).VirajList[0].last(),
                                   &error).asBool();
    if(error!=0)
    {
        return error;
    }
    if(!res) {
        // NOT BREAK
        int ip = m_runStack.top().loopStack.top().IP_begin;
        m_runStack.top().IP = ip;

    }
    else {
        // BREAK
        m_runStack.top().loopStack.pop();
        m_runStack.top().IP = str + 1;

    }
    return 0;
}


/**
 * Выполнение оператора если
 * @param str Номер строки
 * @return Код ошибки
 */
int Run::test_if(int str)
{
    SwitchContext context;
    context.flag = false;
    m_runStack.top().switchStack.push(context);

    int error=0;
    bool res=calc_simple_universal(Cur_Line(str).VirajList[0],
                                   Cur_Line(str).extraSubExpressions,
                                   Cur_Line(str).VirajList[0].last(),
                                   &error).asBool();
    if(error!=0)
        return error;
    if(!res) {
        m_runStack.top().IP = Cur_Line(str).else_pos;
    }
    if(res)
    {
        m_runStack.top().IP = str+1;
    }
    m_runStack.top().switchStack.top().flag = res;
    return 0;
}




int Run::do_utv(int str)
{
    QStringList vl = Cur_Line(str).VirajList[0];
    //   QString dbg = Cur_Line(str).VirajList[0];
    //   qWarning("dddddd %s",dbg.utf8().data());
    int error=0;
    bool res=calc_simple_universal(Cur_Line(str).VirajList[0],
                                   Cur_Line(str).extraSubExpressions,
                                   Cur_Line(str).VirajList[0].last(),
                                   &error).asBool();

    if(error!=0)
        return error;
    if(!res) {
        return RUN_FALSE_UTV;
    }
    if(res)
    {
        m_runStack.top().IP = str+1;
    }

    return 0;
}

int Run::evaluateBuiltInFunction(const QString &name,
                                 const QStringList &exprList,
                                 const QStringList &extraSubexprList,
                                 QVariant &value)
{
    int funcId = -1;
    int moduleId = -1;
    getFuncParam(name, &funcId, &moduleId);
    int argsCount = Modules->module(moduleId)->Functions()->argCountById(funcId);
    const QString fname = Modules->module(moduleId)->Functions()->nameById(funcId);
    QVector<QVariant> args(argsCount);
    QVector<bool> calculated(argsCount, false);
    const QString vizov = name;
    QStringList f_arg_list_link=vizov.mid(vizov.indexOf('&')+1).split("&",QString::SkipEmptyParts);
    QList<int> vizov_id_list;

    for(int i=0;i<f_arg_list_link.count();i++)
        vizov_id_list.append(f_arg_list_link[i].toInt());//Список ссылок на выражения

    for (int i=0; i<argsCount; i++) {
        bool skipCalculation = Modules->module(moduleId)->Functions()->isRes(funcId, i) ||
                               Modules->module(moduleId)->Functions()->isArgRes(funcId, i) ||
                               Modules->module(moduleId)->Functions()->argTypeById(funcId, i) == mass_bool ||
                               Modules->module(moduleId)->Functions()->argTypeById(funcId, i) == mass_integer ||
                               Modules->module(moduleId)->Functions()->argTypeById(funcId, i) == mass_real ||
                               Modules->module(moduleId)->Functions()->argTypeById(funcId, i) == mass_charect ||
                               Modules->module(moduleId)->Functions()->argTypeById(funcId, i) == mass_string;
        if (!skipCalculation) {
            QString expr = exprList[vizov_id_list[i]];
            int error = 0;
            args[i] = calc_simple_universal(exprList, extraSubexprList, expr, &error).asVariant();
            if (error) {
                return error;
            }
            calculated[i] = true;
        }
    }
    if (moduleId==1) {
        int err = processBuiltInFunc(fname, args, value);
        if (err) {
            return err;
        }
        for (int i=0; i<argsCount; i++) {
            if (Modules->module(moduleId)->Functions()->isRes(funcId, i) ||
                Modules->module(moduleId)->Functions()->isArgRes(funcId, i))
            {
                int factVarId = exprList[vizov_id_list[i]].mid(1).toInt();
                tableForId(factVarId)->setValue(factVarId, args[i]);
            }
        }
    }
    return 0;
}

int Run::pushNewFunctionContext(const QString &name, const QString &callExpr,
                                const QStringList &exprList,
                                const QStringList &extraSubexprList)
{
    if (m_runStack.size()>MAX_CALL_DEEP) {
        return RUN_MANY_CALLS;
    }
    int funcId = -1;
    int moduleId = -1;
    getFuncParam(name, &funcId, &moduleId);

    int argsCount = Modules->module(moduleId)->Functions()->argCountById(funcId);
    QVector<QVariant> args(argsCount);
    QVector<bool> calculated(argsCount, false);
    const QString vizov = callExpr;
    QStringList f_arg_list_link=vizov.mid(vizov.indexOf('&')+1).split("&",QString::SkipEmptyParts);
    QList<int> vizov_id_list;

    for(int i=0;i<f_arg_list_link.count();i++)
        vizov_id_list.append(f_arg_list_link[i].toInt());//Список ссылок на выражения

    for (int i=0; i<argsCount; i++) {
        bool skipCalculation = Modules->module(moduleId)->Functions()->isRes(funcId, i) ||
                               Modules->module(moduleId)->Functions()->isArgRes(funcId, i) ||
                               Modules->module(moduleId)->Functions()->argTypeById(funcId, i) == mass_bool ||
                               Modules->module(moduleId)->Functions()->argTypeById(funcId, i) == mass_integer ||
                               Modules->module(moduleId)->Functions()->argTypeById(funcId, i) == mass_real ||
                               Modules->module(moduleId)->Functions()->argTypeById(funcId, i) == mass_charect ||
                               Modules->module(moduleId)->Functions()->argTypeById(funcId, i) == mass_string;
        if (!skipCalculation) {
            QString expr = exprList[vizov_id_list[i]];
            int error = 0;
            args[i] = calc_simple_universal(exprList, extraSubexprList, expr, &error).asVariant();
            if (error) {
                return error;
            }
            calculated[i] = true;
        }
    }
    SymbolTable *table = Modules->module(moduleId)->Functions()->locals(funcId)->clone();
    for (int i=0; i<argsCount; i++) {
        PeremType argType = Modules->module(moduleId)->Functions()->argTypeById(funcId, i);
        int varId = Modules->module(moduleId)->Functions()->argPeremId(funcId, i);
        bool isVector = argType==mass_bool || argType==mass_integer || argType==mass_real || argType==mass_charect || argType==mass_string;
        bool isReference = Modules->module(moduleId)->Functions()->isRes(funcId, i) ||
                           Modules->module(moduleId)->Functions()->isArgRes(funcId, i);
        if (calculated[i]) {
            table->setValue(varId, args[i]);
        }
        else {
            int factVarId = exprList[vizov_id_list[i]].mid(1).toInt();
//            ValueReference ref = locals()->hasMappedId(factVarId) ?
//                                 locals()->reference(factVarId) : globals()->reference(factVarId);
            ValueReference ref = tableForId(factVarId)->reference(factVarId);
            int err = 0;
            if (isVector && !isReference) {
                err = table->setVectorValue(varId, ref);
            }
            else {
                err = table->setReference(varId, ref);
            }
            if (err)
                return err;
        }
    }

    RunContext context;
    context.IP = Modules->module(moduleId)->Functions()->posById(funcId);
    context.algName = name;
    context.locals = table;
    context.moduleId = moduleId;
    context.finished = false;
    context.initializationBlock = false;

    m_runStack.push(context);

    return 0;
}



int Run::do_funct(int str)
{

    QList<KumValueStackElem> arguments;
    QString line=Cur_Line(str).line;

    QString inskobk,name;
    name=line;
    name=name.trimmed();

    int funct_id,mod_id;
    getFuncParam(name,&funct_id,&mod_id);
    const QString debug_fname = Modules->module(mod_id)->Functions()->nameById(funct_id);
    Q_UNUSED(debug_fname);
    proga p = Cur_Line(str);

    QString vizov="@"+line;
    for(int i=0; i<Modules->module(mod_id)->Functions()->argCountById(funct_id);i++)
        vizov=vizov+"&"+QString::number(i);

    QList<QStringList> exprs = p.VirajList;

    //    KumValueStackElem retval;
//    int error = 0;
    //    calc_simple_universal(exprs[0], p.extraSubExpressions, vizov, &error, &retval);
    m_runStack.top().IP = str+1;
    QStringList argumentsLine;
    if (exprs.size()>0) {
        argumentsLine = exprs[0];
    }

    if(Modules->module(mod_id)->isSystem())
    {
        if (Modules->module(mod_id)->Functions()->nameById(funct_id)==trUtf8("пауза")){
            m_runStack.top().IP = str+1;
            return 0;
        }
        if (Modules->module(mod_id)->Functions()->nameById(funct_id)==trUtf8("стоп"))
        {
            m_runStack.resize(1);
            m_runStack[0].finished = true;
            return 0;
        }
        qFatal("Unknown system function!");
        return 0;
    }
    else if ( !Modules->module(mod_id)->isInternal() &&
              !Modules->module(mod_id)->isNetwork() &&
              !Modules->module(mod_id)->isFriend())
    {
        //Исполнители написанные на екимр
        QStringList viraj;
        if (Cur_Line(str).VirajList.count()>0)
            viraj = Cur_Line(str).VirajList[0];
        m_runStack.top().IP = str+1;
        int error = pushNewFunctionContext(name,
                                           p.VirajList.size()>0? p.VirajList[0].last() : vizov,
                                           viraj,
                                           Cur_Line(str).extraSubExpressions);
        //        int error=prepare_arguments("@"+name,"",viraj,Cur_Line(str).extraSubExpressions,f_arguments,&arguments);
        if(error!=0)
        {
            return error;
        }


//        RunContext newContext;
//        newContext.moduleId = mod_id;
//        newContext.algName = name;
//        newContext.IP = Modules->module(mod_id)->Functions()->posById(funct_id);
//        newContext.finished = false;
//        m_runStack.push(newContext);
    }
    else //internal
    {
        int err=0;
        QStringList viraj;
        if(Modules->module(mod_id)->Functions()->argCountById(funct_id)>0){
            viraj=Cur_Line(str).VirajList[0];
            //            err=prepare_arguments("@"+name,"  ",viraj,Cur_Line(str).extraSubExpressions,f_arguments,&arguments);
            if(err!=0)return err;};

        if(!Modules->module(mod_id)->isKumIsp())
        {
            int error=0;
            if(!Modules->module(mod_id)->Graphics() && !Modules->module(mod_id)->isNetwork()){
                Modules->module(mod_id)->instrument()->runFunc(
                        Modules->module(mod_id)->Functions()->nameById(funct_id),
                        &arguments,
                        &error);

                if(error>0)
                    return error;//09-06-Mordol
            }
            else //Исполнители с графикой
            {
                qFatal("Attempt to call external module algorhitm");
            }
            if ( str>=0 ) {
                m_runStack.top().IP = str+1;
            }
        }
    }
    return 0;
}

/**
 * Выполнение оператора выбор
 * @param str 
 * @return 
 */
int Run::do_vibor(int str)
{
    SwitchContext context;
    context.flag = false;
    m_runStack.top().switchStack.push(context);
    m_runStack.top().IP = Cur_Line(str).else_pos;
    return 0;
}


/**
 * Выполнение оператора при.
 * @param str 
 * @return 
 */
int Run::do_pri(int str)
{
    Q_ASSERT (m_runStack.top().switchStack.size()>0);

    int elsepos=Cur_Line(str).else_pos;
    if (m_runStack.top().switchStack.top().flag) {
        m_runStack.top().IP = elsepos;
//        m_runStack.top().switchStack.pop();
        return 0;
    }

    QStringList virajList=Cur_Line(str).VirajList[0];


    int error=0;
    bool res=calc_simple_universal(Cur_Line(str).VirajList[0],
                                   Cur_Line(str).extraSubExpressions,
                                   Cur_Line(str).VirajList[0].last(),
                                   &error).asBool();

    if(error!=0)
        return error;

//    if(!res) {
//        if((Cur_Line(elsepos).str_type==Kum::Else)&&
//           (m_runStack.top().switchStack.top().value==1))
//        {
//            m_runStack.top().IP = Cur_Line(str).else_pos+1;
//            m_runStack.top().switchStack.pop();
//        }
//        else
//        {
//            m_runStack.top().IP = Cur_Line(str).else_pos;
//        }
//    }
    if(res) {
        m_runStack.top().IP = str+1;
        m_runStack.top().switchStack.top().flag = true;
    }
    else {
        m_runStack.top().IP = Cur_Line(str).else_pos;
    }
    return 0;
}

int Run::do_dano(int str)
{
    QStringList vl = Cur_Line(str).VirajList[0];
    //   QString dbg = Cur_Line(str).VirajList[0];
    //   qWarning("dddddd %s",dbg.utf8().data());
    int error=0;
    bool res=calc_simple_universal(Cur_Line(str).VirajList[0],
                                   Cur_Line(str).extraSubExpressions,
                                   Cur_Line(str).VirajList[0].last(),
                                   &error).asBool();

    if(error!=0)return error;

    if(!res) return RUN_BAD_DANO; // NEW V.Y. 01.09
    if(res)
    {
//        qWarning("Dano pos:%i",Cur_Line(str).else_pos);
        m_runStack.top().IP = Cur_Line(str).else_pos;
    }
    return 0;
}


int Run::do_nado(int str)
{
    QStringList vl = Cur_Line(str).VirajList[0];
    //   QString dbg = Cur_Line(str).VirajList[0];
    //   qWarning("dddddd %s",dbg.utf8().data());
    int error=0;
    bool res=calc_simple_universal(Cur_Line(str).VirajList[0],
                                   Cur_Line(str).extraSubExpressions,
                                   Cur_Line(str).VirajList[0].last(),
                                   &error).asBool();

    if(error!=0)return error;

    if(!res) return RUN_BAD_NADO; // NEW V.Y. 01.09


    return 0;
};


/**
 * Выполнение ввода. 
 * @param str 
 * @return 
 */
int Run::do_vvod ( int str )
{
    proga p = Cur_Line(str);
    QString format;

    QVector<PeremType> types(p.VirajList.size());
    QVector<int> ids(p.VirajList.size());
    QVector<SymbolTable*> tables(p.VirajList.size());
    QVector<QString> extras(p.VirajList.size());

    for (int i=0; i<p.VirajList.size(); i++) {
        const QStringList terms = p.VirajList[i];
        const QString mainExpr = p.VirajList[i].last();
        SymbolTable *table = 0;
        int varId = -1;
        int endPos = mainExpr.indexOf("&");
        const QString idStr = mainExpr.mid(1, endPos==-1? mainExpr.size() : endPos-1 );
        varId = idStr.toInt();
        table = tableForId(varId);
        PeremType type = table->type(varId);
        if (type==integer || type==mass_integer) {
            format += "%i";
        }
        else if (type==real || type==mass_real) {
            format += "%f";
        }
        else if (type==boolean || type==mass_bool) {
            format += "%b";
        }
        else if (type==charect || type==mass_charect) {
            if (table->isConstant(varId)) {
                QString txt = table->value(varId).toString();
                txt.replace("\\","\\\\");
                txt.replace("%","\\%");
                format += txt;
                type = none;
            }
            else {
                format += "%c";
            }
        }
        else if (type==kumString || type==mass_string) {
            if (table->isConstant(varId)) {
                QString txt = table->value(varId).toString();
                txt.replace("\\","\\\\");
                txt.replace("%","\\%");
                format += txt;
                type = none;
            }
            else {
                format += "%s";
            }
        }
        else {
            qFatal("Internal error!");
            break;
        }
        types[i] = type;
        tables[i] = table;
        ids[i] = varId;
        extras[i] = mainExpr.mid(endPos);
    }

    QStringList inp = sendInput(format);
    for (int i=0; i<types.size(); i++) {
        if (inp.isEmpty())
            return 65533;
        const QString in = inp.first();
        inp.pop_front();
        QVariant result;
        if (types[i]==integer || types[i]==mass_integer) {
            result = QVariant(in.toInt());
        }
        else if (types[i]==real || types[i]==mass_real) {
            result = QVariant(in.toDouble());
        }
        else if (types[i]==charect || types[i]==mass_charect) {
            result = QVariant(in[0]);
        }
        else if (types[i]==kumString || types[i]==mass_string) {
            result = QVariant(in);
        }
        else if (types[i]==boolean || types[i]==mass_bool) {
            result = QVariant(in.toLower()==QString::fromUtf8("да"));
        }
        if (types[i]!=none) {
            int index0 = 0;
            int index1 = 0;
            int index2 = 0;
            if (tables[i]->dimension(ids[i])>0) {
                QStringList ind = extras[i].split("&");
                int error = 0;
                QVector<int> indeces = calculateVectorIndeces(extras[i], p.VirajList[i], p.extraSubExpressions, &error);
                if (error) {
                    return error;
                }
                index0 = indeces[0];
                index1 = indeces[1];
                index2 = indeces[2];
            }
            tables[i]->setValue(ids[i], index0, index1, index2, result);
        }
    }
    m_runStack.top().IP = str+1;
    return 0;
}



/**
 * Выполнение ввода.из файла 
 * @param str 
 * @return 
 */

int Run::do_use(int str)
{
    Q_UNUSED(str);
    return 0;
}


QList<proga> * Run::Cur_Proga_Value()
{
    return Modules->module(m_runStack.top().moduleId)->Proga_Value();
}

proga Run::Cur_Line(int str)
{
    if (str>=0) {
        KumSingleModule *module = Modules->module(m_runStack.top().moduleId);
        proga value = module->Proga_Value_Line(str);
        return value;
    }
    else {
        str = -str;
        int baseStr = str / 10;
        int subStr = (str % 10) - 1;
        return Modules->module(m_runStack.top().moduleId)->Proga_Value_Line(baseStr).generatedLinesAfter[subStr];
    }
}


QStringList Run::sendInput(const QString &format)
{
    QString error;
    QStringList result = Application::instance()->handleConsoleInput(format, error);
    if (error.size()>0) {
        result.clear();
    }
    return result;
}

void Run::sendError(const QString &text)
{
    Application::instance()->handleConsoleError(text);
}

bool Run::hasMoreInstructions() const
{
    return !m_runStack.isEmpty() &&
            m_runStack.top().IP < Modules->module(m_runStack.top().moduleId)->Proga_Value()->size() &&
            !m_runStack.top().finished &&
            i_lastError==0;
}

void Run::evaluateNextInstruction()
{
    Q_ASSERT(!m_runStack.isEmpty());
    int IP = m_runStack.top().IP;
    i_lastErrorLine = Cur_Line(IP).real_line.lineNo;
    i_lastError= do_line(IP);
}

int Run::lastErrorCode() const
{
    return i_lastError;
}

int Run::lastErrorLine() const
{
    return i_lastErrorLine;
}

void Run::sendOutput(const QString &text)
{
    Application::instance()->handleConsoleOutput(text);
}


int Run::processBuiltInFunc(const QString &name,
                            QVector<QVariant> &arguments,
                            QVariant &retval)
{
    if ( name == trUtf8("юникод") )
    {
        QChar chr = arguments[0].toString()[0];
        int res = chr.unicode();
        retval = res;
        return 0;
    }
    else if ( name == trUtf8("код") ) {
        QTextCodec *codec = QTextCodec::codecForName("KOI8-R");
        QString str = arguments[0].toString();
        if ( str.length() != 1 )
        {
            return INTFUN_NOT_KOI8_RANGE;
        }
        if ( ( ! codec->canEncode(str) ) && str[0] != '\0' ) {
            return INTFUN_NOT_KOI8_RANGE;
        }
        QByteArray KOI8string = codec->fromUnicode(str);
        char code;
        if ( str.isEmpty() )
            code = '\0';
        else
            code = KOI8string[0];
        int res = code;
        if ( res < 0 )
            res += 256;
        retval = res;
    }
    else if ( name == trUtf8("символ") ) {

        int x = arguments[0].toInt();
        if((x<0)||(x>255)){return INTFUN_NOT_KOI8_RANGE;};
        QChar simbol;
        switch(x) //Таблица кодировки
        {

        case 32:simbol=' ';break;
        case 33:simbol='!';break;
        case 34:simbol='"';break;
        case 35:simbol='#';break;
        case 36:simbol='$';break;
        case 37:simbol='%';break;
        case 38:simbol='&';break;

        case 39:simbol=QChar(0x0027 );break;
        case 40:simbol=QChar(0x0028 );break;
        case 41:simbol=QChar(0x0029 );break;
        case 42:simbol=QChar(0x002A );break;
        case 43:simbol=QChar(0x002B );break;
        case 44:simbol=QChar(0x002C );break;
        case 45:simbol=QChar(0x002D );break;
        case 46:simbol=QChar(0x002E );break;
        case 47:simbol=QChar(0x002F );break;
        case 48:simbol=QChar(0x0030 );break;
        case 49:simbol=QChar(0x0031 );break;
        case 50:simbol=QChar(0x0032 );break;
        case 51:simbol=QChar(0x0033 );break;
        case 52:simbol=QChar(0x0034 );break;
        case 53:simbol=QChar(0x0035 );break;
        case 54:simbol=QChar(0x0036 );break;
        case 55:simbol=QChar(0x0037 );break;
        case 56:simbol=QChar(0x0038 );break;
        case 57:simbol=QChar(0x0039 );break;
        case 58:simbol=QChar(0x003A );break;
        case 59:simbol=QChar(0x003B );break;
        case 60:simbol=QChar(0x003C );break;
        case 61:simbol=QChar(0x003D );break;
        case 62:simbol=QChar(0x003E );break;
        case 63:simbol=QChar(0x003F );break;
        case 64:simbol=QChar(0x0040 );break;
        case 65:simbol=QChar(0x0041 );break;
        case 66:simbol=QChar(0x0042 );break;
        case 67:simbol=QChar(0x0043 );break;
        case 68:simbol=QChar(0x0044 );break;
        case 69:simbol=QChar(0x0045 );break;
        case 70:simbol=QChar(0x0046 );break;
        case 71:simbol=QChar(0x0047 );break;
        case 72:simbol=QChar(0x0048 );break;
        case 73:simbol=QChar(0x0049 );break;
        case 74:simbol=QChar(0x004A );break;
        case 75:simbol=QChar(0x004B );break;
        case 76:simbol=QChar(0x004C );break;
        case 77:simbol=QChar(0x004D );break;
        case 78:simbol=QChar(0x004E );break;
        case 79:simbol=QChar(0x004F );break;
        case 80:simbol=QChar(0x0050 );break;
        case 81:simbol=QChar(0x0051 );break;
        case 82:simbol=QChar(0x0052 );break;
        case 83:simbol=QChar(0x0053 );break;
        case 84:simbol=QChar(0x0054 );break;
        case 85:simbol=QChar(0x0055 );break;
        case 86:simbol=QChar(0x0056 );break;
        case 87:simbol=QChar(0x0057 );break;
        case 88:simbol=QChar(0x0058 );break;
        case 89:simbol=QChar(0x0059 );break;
        case 90:simbol=QChar(0x005A );break;
        case 91:simbol=QChar(0x005B );break;
        case 92:simbol=QChar(0x005C );break;
        case 93:simbol=QChar(0x005D );break;
        case 94:simbol=QChar(0x005E );break;
        case 95:simbol=QChar(0x005F );break;
        case 96:simbol=QChar(0x0060 );break;
        case 97:simbol=QChar(0x0061 );break;
        case 98:simbol=QChar(0x0062 );break;
        case 99:simbol=QChar(0x0063 );break;
        case 100:simbol=QChar(0x0064 );break;
        case 101:simbol=QChar(0x0065 );break;
        case 102:simbol=QChar(0x0066 );break;
        case 103:simbol=QChar(0x0067 );break;
        case 104:simbol=QChar(0x0068 );break;
        case 105:simbol=QChar(0x0069 );break;
        case 106:simbol=QChar(0x006A );break;
        case 107:simbol=QChar(0x006B );break;
        case 108:simbol=QChar(0x006C );break;
        case 109:simbol=QChar(0x006D );break;
        case 110:simbol=QChar(0x006E );break;
        case 111:simbol=QChar(0x006F );break;
        case 112:simbol=QChar(0x0070 );break;
        case 113:simbol=QChar(0x0071 );break;
        case 114:simbol=QChar(0x0072 );break;
        case 115:simbol=QChar(0x0073 );break;
        case 116:simbol=QChar(0x0074 );break;
        case 117:simbol=QChar(0x0075 );break;
        case 118:simbol=QChar(0x0076 );break;
        case 119:simbol=QChar(0x0077 );break;
        case 120:simbol=QChar(0x0078 );break;
        case 121:simbol=QChar(0x0079 );break;
        case 122:simbol=QChar(0x007A );break;
        case 123:simbol=QChar(0x007B );break;
        case 124:simbol=QChar(0x007C );break;
        case 125:simbol=QChar(0x007D );break;
        case 126:simbol=QChar(0x007E );break;
        case 127:simbol=QChar(0x007F );break;
        case 128:simbol=QChar(0x2500 );break;
        case 129:simbol=QChar(0x2502 );break;
        case 130:simbol=QChar(0x250C );break;
        case 131:simbol=QChar(0x2510 );break;
        case 132:simbol=QChar(0x2514 );break;
        case 133:simbol=QChar(0x2518 );break;
        case 134:simbol=QChar(0x251C );break;
        case 135:simbol=QChar(0x2524 );break;
        case 136:simbol=QChar(0x252C );break;
        case 137:simbol=QChar(0x2534 );break;
        case 138:simbol=QChar(0x253C );break;
        case 139:simbol=QChar(0x2580 );break;
        case 140:simbol=QChar(0x2584 );break;
        case 141:simbol=QChar(0x2588 );break;
        case 142:simbol=QChar(0x258C );break;
        case 143:simbol=QChar(0x2590 );break;
        case 144:simbol=QChar(0x2591 );break;
        case 145:simbol=QChar(0x2592 );break;
        case 146:simbol=QChar(0x2593 );break;
        case 147:simbol=QChar(0x2320 );break;
        case 148:simbol=QChar(0x25A0 );break;
        case 149:simbol=QChar(0x2219 );break;
        case 150:simbol=QChar(0x221A );break;
        case 151:simbol=QChar(0x2248 );break;
        case 152:simbol=QChar(0x2264 );break;
        case 153:simbol=QChar(0x2265 );break;
        case 154:simbol=QChar(0x00A0 );break;
        case 155:simbol=QChar(0x2321 );break;
        case 156:simbol=QChar(0x00B0 );break;
        case 157:simbol=QChar(0x00B2 );break;
        case 158:simbol=QChar(0x00B7 );break;
        case 159:simbol=QChar(0x00F7 );break;
        case 160:simbol=QChar(0x2550 );break;
        case 161:simbol=QChar(0x2551 );break;
        case 162:simbol=QChar(0x2552 );break;
        case 163:simbol=QChar(0x0451 );break;
        case 164:simbol=QChar(0x2553 );break;
        case 165:simbol=QChar(0x2554 );break;
        case 166:simbol=QChar(0x2555 );break;
        case 167:simbol=QChar(0x2556 );break;
        case 168:simbol=QChar(0x2557 );break;
        case 169:simbol=QChar(0x2558 );break;
        case 170:simbol=QChar(0x2559 );break;
        case 171:simbol=QChar(0x255A );break;
        case 172:simbol=QChar(0x255B );break;
        case 173:simbol=QChar(0x255C );break;
        case 174:simbol=QChar(0x255D );break;
        case 175:simbol=QChar(0x255E );break;
        case 176:simbol=QChar(0x255F );break;
        case 177:simbol=QChar(0x2560 );break;
        case 178:simbol=QChar(0x2561 );break;
        case 179:simbol=QChar(0x0401 );break;
        case 180:simbol=QChar(0x2562 );break;
        case 181:simbol=QChar(0x2563 );break;
        case 182:simbol=QChar(0x2564 );break;
        case 183:simbol=QChar(0x2565 );break;
        case 184:simbol=QChar(0x2566 );break;
        case 185:simbol=QChar(0x2567 );break;
        case 186:simbol=QChar(0x2568 );break;
        case 187:simbol=QChar(0x2569 );break;
        case 188:simbol=QChar(0x256A );break;
        case 189:simbol=QChar(0x256B );break;
        case 190:simbol=QChar(0x256C );break;
        case 191:simbol=QChar(0x00A9 );break;
        case 192:simbol=QChar(0x044E );break;
        case 193:simbol=QChar(0x0430 );break;
        case 194:simbol=QChar(0x0431 );break;
        case 195:simbol=QChar(0x0446 );break;
        case 196:simbol=QChar(0x0434 );break;
        case 197:simbol=QChar(0x0435 );break;
        case 198:simbol=QChar(0x0444 );break;
        case 199:simbol=QChar(0x0433 );break;
        case 200:simbol=QChar(0x0445 );break;
        case 201:simbol=QChar(0x0438 );break;
        case 202:simbol=QChar(0x0439 );break;
        case 203:simbol=QChar(0x043A );break;
        case 204:simbol=QChar(0x043B );break;
        case 205:simbol=QChar(0x043C );break;
        case 206:simbol=QChar(0x043D );break;
        case 207:simbol=QChar(0x043E );break;
        case 208:simbol=QChar(0x043F );break;
        case 209:simbol=QChar(0x044F );break;
        case 210:simbol=QChar(0x0440 );break;
        case 211:simbol=QChar(0x0441 );break;
        case 212:simbol=QChar(0x0442 );break;
        case 213:simbol=QChar(0x0443 );break;
        case 214:simbol=QChar(0x0436 );break;
        case 215:simbol=QChar(0x0432 );break;
        case 216:simbol=QChar(0x044C );break;
        case 217:simbol=QChar(0x044B );break;
        case 218:simbol=QChar(0x0437 );break;
        case 219:simbol=QChar(0x0448 );break;
        case 220:simbol=QChar(0x044D );break;
        case 221:simbol=QChar(0x0449 );break;
        case 222:simbol=QChar(0x0447 );break;
        case 223:simbol=QChar(0x044A );break;
        case 224:simbol=QChar(0x042E );break;
        case 225:simbol=QChar(0x0410 );break;
        case 226:simbol=QChar(0x0411 );break;
        case 227:simbol=QChar(0x0426 );break;
        case 228:simbol=QChar(0x0414 );break;
        case 229:simbol=QChar(0x0415 );break;
        case 230:simbol=QChar(0x0424 );break;
        case 231:simbol=QChar(0x0413 );break;
        case 232:simbol=QChar(0x0425 );break;
        case 233:simbol=QChar(0x0418 );break;
        case 234:simbol=QChar(0x0419 );break;
        case 235:simbol=QChar(0x041A );break;
        case 236:simbol=QChar(0x041B );break;
        case 237:simbol=QChar(0x041C );break;
        case 238:simbol=QChar(0x041D );break;
        case 239:simbol=QChar(0x041E );break;
        case 240:simbol=QChar(0x041F );break;
        case 241:simbol=QChar(0x042F );break;
        case 242:simbol=QChar(0x0420 );break;
        case 243:simbol=QChar(0x0421 );break;
        case 244:simbol=QChar(0x0422 );break;
        case 245:simbol=QChar(0x0423 );break;
        case 246:simbol=QChar(0x0416 );break;
        case 247:simbol=QChar(0x0412 );break;
        case 248:simbol=QChar(0x042C );break;
        case 249:simbol=QChar(0x042B );break;
        case 250:simbol=QChar(0x0417 );break;
        case 251:simbol=QChar(0x0428 );break;
        case 252:simbol=QChar(0x042D );break;
        case 253:simbol=QChar(0x0429 );break;
        case 254:simbol=QChar(0x0427 );break;
        case 255:simbol=QChar(0x042A );break;

        }
        if(x<32)
            simbol=QChar(x);
        retval = simbol;

    }
    else if ( name == trUtf8("символ2") ) {
        int x = arguments[0].toInt();
        QChar simbol=QChar(x);
        if (!KumTools::instance()->isValidChar(simbol)) {
            return RUN_SYMBOL_NOT_ALLOWED;

        }
        retval = simbol;
    }
    else if(name=="sqrt") {
        if (arguments[0].toDouble()<0) {
            return RUN_ROOT_OF_NEGATIVE;
        }
        double res=sqrtf(arguments[0].toDouble());
        if(isnan(res)!=0) {
            return RUN_NO_VALUE;
        }
        retval = res;
    }
    if (name==trUtf8("модуль"))
    {
        int x = arguments[0].toInt();
        retval = abs(x);
    }
    else if (name=="abs") {
        double x = arguments[0].toDouble();
        retval = double(fabs(x));
    }
    else if (name==trUtf8("знак")) {
        int x = arguments[0].toInt();
        if ( x < 0 )
        {
            retval = -1;
        }
        else if ( x > 0 )
        {
            retval = 0;
        }
        else
        {
            retval = 1;
        }
    }
    else if (name=="sign") {
        double x = arguments[0].toDouble();
        if ( x < -EPSILON )
        {
            retval = -1;
        }
        else if ( x > EPSILON )
        {
            retval = 1;
        }
        else
        {
            retval = 0;
        }
    }
    else if (name==trUtf8("минимум")) {
        int x = arguments[0].toInt();
        int y = arguments[1].toInt();
        if ( x > y )
            retval = y;
        else
            retval = x;
    }
    else if (name==trUtf8("максимум")) {
        int x = arguments[0].toInt();
        int y = arguments[1].toInt();
        if ( x < y )
            retval = y;
        else
            retval = x;
    }
    else if (name=="min")
        {
        double x = arguments[0].toDouble();
        double y = arguments[1].toDouble();
        if ( x > y )
            retval = y;
        else
            retval = x;
    }
    else if (name=="max") {
        double x = arguments[0].toDouble();
        double y = arguments[1].toDouble();
        if ( x < y )
            retval = y;
        else
            retval = x;
    }
    else if (name==trUtf8("случайное число")) {
        int x = arguments[0].toInt();
        long int rndValue = qrand(); // in range [ 0; 2^32 ]
        double stdValue = double(rndValue)/RAND_MAX; // as R(0;1)
        int result = int(double(x)*stdValue);
        retval = result;
    }
    else if (name=="rnd") {
        double x = arguments[0].toDouble();
        long int rndValue = qrand();
        double stdValue = double(rndValue)/RAND_MAX;
        double result = double(x*stdValue);
        retval = result;
    }
    if (name=="sin")
    {
        double x = arguments[0].toDouble();
        retval = QVariant(double(sinf(x)));
    }
    else if (name=="cos") {
        double x = arguments[0].toDouble();
        retval = QVariant(double(cosf(x)));
    }
    else if (name=="tg") {
        double x = arguments[0].toDouble();
        retval = QVariant(double(tanf(x)));
    }
    else if (name=="ctg") {
        double x = arguments[0].toDouble();
        double tg = tanf(x);
        double ctg = 1.0 / tg;
        if(isinf(ctg)!=0) {
            return RUN_NO_VALUE;
        }
        retval = QVariant(ctg);
    }
    else if (name=="arcsin") {
        double x = arguments[0].toDouble();
        if(fabs(x)>1) {
            return RUN_BAG_INVERSE_TRIGONOMETRIC_ARGUMENT;
        }
        double res=asinf(x);
        if(isnan(res)!=0) {
            return RUN_NO_VALUE;
        }
        if(isinf(res)!=0) {
            return RUN_NO_VALUE;
        }
        retval = res;
    }
    else if (name=="arccos") {
        double x = arguments[0].toDouble();
        if(fabs(x)>1) {
            return RUN_BAG_INVERSE_TRIGONOMETRIC_ARGUMENT;
        }
        double res=acosf(x);
        if(isnan(res)!=0) {
            return RUN_NO_VALUE;
        }
        retval = res;
    }
    else if (name=="arctg") {
        double x = arguments[0].toDouble();
        retval = double(atanf(x));
    }
    else if (name=="arcctg") {
        double x = arguments[0].toDouble();
        double tg = 1.0 / x;
        double result = atanf(tg);
        retval = result;
    }
    else if (name=="ln") {
        double x = arguments[0].toDouble();
        if ( x <= 0. ) {
            return INTFUN_LN_ZERO;
        }
        retval = double(logf(x));
    }
    else if (name=="lg") {
        double x = arguments[0].toDouble();
        if ( x <= 0. )
        {return INTFUN_LN_ZERO;}
        double ln = logf(x);
        double result = ln / logf(10.0); // log_a(b) = log_c(b) / log_c(a)
        retval = result;
    }
    else if (name=="exp") {
        double x = arguments[0].toDouble();
        if(isnan(expf(x))!=0) {
            return RUN_NO_VALUE;
        }
        if(isinf(expf(x))!=0) {
            return CALC_FLOAT_OVERFLOW;
        }
        retval = double(expf(x));
    }
    else if (name=="iabs") {
        int x = arguments[0].toInt();
        retval = abs(x);
    }
    else if (name=="mod") {
        int x = arguments[0].toInt();
        int y = arguments[1].toInt();
        if ( y <= 0 ) {
            return INTFUN_MOD_BAD_Y;
        }
        while ( x < 0 )
            x += y;
        int res = x % y;
        retval = res;
    }
    else if (name=="div")
        {
        int x = arguments[0].toInt();
        int y = arguments[1].toInt();
        if ( y <= 0 ) {
            return INTFUN_DIV_BAD_Y;
        }
        int xx = x;
        while ( xx < 0 )
            xx += y;
        int mod = xx % y;
        int result = ( x - mod ) / y;
        retval = result;
    }
    else if (name=="int")
    {
        double x = arguments[0].toDouble();
        double y = floor(x);
        bool ok = IntegerOverflowChecker::checkFromReal(y);
        int result = int(y);
        if ( !ok ) {
            return CALC_OVERFLOW;

        }
        retval = result;
    }
    else if (name==trUtf8("МАКСВЕЩ")) {
        double value = DoubleOverflowChecker::MAX_DOUBLE_VALUE;
        retval = value;
    }
    else if (name==trUtf8("МАКСЦЕЛ")) {
        retval = 2147483647;
    }
    else if (name==trUtf8("длин"))
    {
        QString s = arguments[0].toString();
        retval = s.length();
    }
    else if (name==trUtf8("время")) {
        QTime time = QTime::currentTime();
        int sec = time.hour()*3600 + time.minute()*60 + time.second();
        int csec = sec * 100;
        csec += time.msec() / 10;
        int result = csec;
        retval = result;
    }
    else if ( name==trUtf8("лит_в_цел") ) {
        QString literal = arguments[0].toString();
        bool ok;
        int result = literal.toInt(&ok);
        retval = result;
        arguments[1] = ok;
    }
    else if ( name==trUtf8("лит_в_вещ") ) {
        QString literal = arguments[0].toString();
        bool ok;
        double result = literal.toDouble(&ok);
        retval = result;
        arguments[1] = ok;
    }
    else if ( name==trUtf8("вещ_в_лит") ) {
        double value = arguments[0].toDouble();
        QString result = QString("%1").arg(value,0,'f');
        QStringList pair = result.split(".",QString::KeepEmptyParts);
        if ( pair.count() == 2 ) {
            while ( pair[1].endsWith("0") )
                pair[1].remove(pair[1].length()-1,1);
            result = pair.join(".");
        }
        if ( result.endsWith(".") )
            result.remove(result.length()-1,1);
        retval = result;
    }
    else if ( name==trUtf8("цел_в_лит") ) {
        int value = arguments[0].toInt();
        QString result = QString("%1").arg(value);
        retval = result;
    }
    else if(name==trUtf8("лит")) {
        retval = QString::number(arguments[0].toDouble());
    }
//    else if (name.startsWith("!memfill_")) {
//        int mod_id = arguments[1].toInt();
//        int var_id = arguments[0].toInt();
//        symbol_table *st = m_modules->module(mod_id)->Values();
//        QString s_data = arguments[2].toString();
//        for (int i=0; i<s_data.length(); i++) {
//            int code = s_data[i].unicode();
//            code += STR_HIDE_OFFSET;
//            s_data[i] = QChar(code);
//        }

//        int dim = name.mid(name.length()-1,1).toInt();
//        PeremType baseType = none;
//        if (name[name.length()-2]=='i')
//            baseType = integer;
//        else if (name[name.length()-2]=='d')
//            baseType = real;
//        else if (name[name.length()-2]=='b')
//            baseType = boolean;
//        else if (name[name.length()-2]=='c')
//            baseType = charect;
//        else if (name[name.length()-2]=='s')
//            baseType = kumString;

//        int x_max = st->symb_table[var_id].size0_end-st->symb_table[var_id].size0_start + 1;
//        int y_max = st->symb_table[var_id].size1_end-st->symb_table[var_id].size1_start + 1;
//        int z_max = st->symb_table[var_id].size2_end-st->symb_table[var_id].size2_start + 1;
//        if ( dim==1 ) {
//            QList<QVariant> data = extract1DArray(s_data);
//            QVariant value;
//            int y = 0;
//            int z = 0;
//            for ( int x=0; x<data.count(); x++ ) {
//                if (x>=x_max) {
//                    return RUN_CONST_ARRAY_OUT_OF_BOUNDS;

//                }
//                value = data[x];
//                if (baseType==integer) {
//                    st->symb_table[var_id].value->setIntMassValue(value.toInt(),x,y,z);
//                }
//                else if (baseType==real) {
//                    st->symb_table[var_id].value->setFloatMassValue(value.toDouble(),x,y,z);
//                }
//                else if (baseType==boolean) {
//                    st->symb_table[var_id].value->setBoolMassValue(value.toBool(),x,y,z);
//                }
//                else if (baseType==charect) {
//                    st->symb_table[var_id].value->setCharMassValue(value.toChar(),x,y,z);
//                }
//                else if (baseType==kumString) {
//                    st->symb_table[var_id].value->setStringMassValue(value.toString(),x,y,z);
//                }
//            }
//        }
//        if ( dim==2 ) {
//            QList< QList<QVariant> > data = extract2DArray(s_data);
//            QVariant value;
//            int z = 0;
//            for ( int y=0; y<data.count(); y++ ) {
//                for ( int x=0; x<data[y].count(); x++ ) {
//                    if (x>=x_max || y>=y_max) {
//                        return RUN_CONST_ARRAY_OUT_OF_BOUNDS;

//                    }
//                    value = data[y][x];
//                    if (baseType==integer) {
//                        st->symb_table[var_id].value->setIntMassValue(value.toInt(),x,y,z);
//                    }
//                    else if (baseType==real) {
//                        st->symb_table[var_id].value->setFloatMassValue(value.toDouble(),x,y,z);
//                    }
//                    else if (baseType==boolean) {
//                        st->symb_table[var_id].value->setBoolMassValue(value.toBool(),x,y,z);
//                    }
//                    else if (baseType==charect) {
//                        st->symb_table[var_id].value->setCharMassValue(value.toChar(),x,y,z);
//                    }
//                    else if (baseType==kumString) {
//                        st->symb_table[var_id].value->setStringMassValue(value.toString(),x,y,z);
//                    }
//                }
//            }
//        }
//        if ( dim==3 ) {
//            QList< QList< QList<QVariant> > > data = extract3DArray(s_data);
//            QVariant value;
//            for ( int z=0; z<data.size(); z++ ) {
//                for ( int y=0; y<data[z].count(); y++ ) {
//                    for ( int x=0; x<data[z][y].count(); x++ ) {
//                        if (x>=x_max || y>=y_max || z>=z_max) {
//                            return RUN_CONST_ARRAY_OUT_OF_BOUNDS;

//                        }
//                        value = data[z][y][x].toInt();
//                        if (baseType==integer) {
//                            st->symb_table[var_id].value->setIntMassValue(value.toInt(),x,y,0);
//                        }
//                        else if (baseType==real) {
//                            st->symb_table[var_id].value->setFloatMassValue(value.toDouble(),x,y,0);
//                        }
//                        else if (baseType==boolean) {
//                            st->symb_table[var_id].value->setBoolMassValue(value.toBool(),x,y,0);
//                        }
//                        else if (baseType==charect) {
//                            st->symb_table[var_id].value->setCharMassValue(value.toChar(),x,y,0);
//                        }
//                        else if (baseType==kumString) {
//                            st->symb_table[var_id].value->setStringMassValue(value.toString(),x,y,0);
//                        }
//                    }
//                }
//            }
//        }
//    }
    return 0;
}
