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
#ifndef _KUMIR_RUN_H
#define _KUMIR_RUN_H
#include <QtCore>
#include "int_proga.h"
#include "error.h" // NEW V.Y. 01.09
#include "messagesprovider.h"
#include "kum_tables.h"

namespace Kum {

    class SymbolTable;
    struct ValueReference;

    struct LoopContext {
        /** Loop begin instruction point */
        int IP_begin;
        /** Loop value or variable ID in symbol table */
        int loopValue;
        /** Loop start value */
        int fromValue;
        /** Loop end value */
        int toValue;
        /** Loop step value */
        int stepValue;
        /** Variable ID */
        int variableID;
    };

    struct SwitchContext {
        bool flag;
    };

    struct RunContext {
        /** Instruction point */
        int IP;
        /** Current module ID*/
        int moduleId;
        /** Current algorhitm name */
        QString algName;
        /** Loop control stack */
        QStack<LoopContext> loopStack;
        /** Switch-case control stack */
        QStack<SwitchContext> switchStack;
        /** Local symbol table */
        SymbolTable *locals;
        /** Evaluation done */
        bool finished;
        /** Current context is initialization block */
        bool initializationBlock;

    };

    class Run:
            public QObject
    {
        Q_OBJECT
    public:
        Run(QObject *parent = 0);
        void init(KumModules* moduleTable, int mainModuleId, int mainModuleStartIp);
        void evaluateNextInstruction();
        bool hasMoreInstructions() const;
        int lastErrorCode() const;
        int lastErrorLine() const;


    signals:

        void runError(int error,int stroka,int module_id);
        void debugMessage(QString text,int stroka,int module_id);
        void waitForInput(QString format);
        void pause(int str,int module_id);
        void GraphIspFunc(QString name,QList<KumValueStackElem>* arguments,int *err);
        void Step();
        void waitForKey(bool ignoreModifiers);
        void sendMessage(const QString &txt,int);
        void SwitchIspMode(int mode);
        void sendNetwMessage(QString message);
        void sendServReplay(QString  text,int client_id);
        void resetPlugin(int module_id);
    private:
        void sendError(const QString &text);

        void sendDebug(QString text,int stroka,int module_id);
        QStringList sendInput(const QString &format);
        void sendInputKey(bool ignoreModifiers);
        void sendStep();
        void Pause(int str,int module_id);
        void waitForSync();
        void switchIspMode(int);
        int do_line(int str);
        int do_vivod(int str);
        int getOperPrior(QChar oper);
        KumValueStackElem calc_simple_universal(const QStringList &simple_viraj,
                                                const QStringList &extraExpressions,
                                                const QString &expression,
                                                int* error,
                                                KumValueStackElem *newValue=NULL,
                                                QString *debugString=NULL);
        int run_circle(int str);
        int initPerem(int str,int module);

        int testVectorIndeces(const SymbolTable *table,
                              int id,
                              const QVector<int> &indeces);

        int test_if(int str);
        int do_utv(int str);
        int calc_prisv(int str);

        QList<int> calcMassPos(const QString &mass_viraj,
                               const QStringList &simple_viraj,
                               const QStringList &extraSubexprList,
                               int *err);

        QVector<int> calculateVectorIndeces(const QString &mass_args,
                                            const QStringList &expressions,
                                            const QStringList &extraSubexprList,
                                            int *err,QString *debugString=NULL);

        void setVectorData(SymbolTable *table, int id,
                           const QString &mass_args,
                           const QStringList &viraj,
                           const QStringList &extraSubexprList,
                           const QVariant &value,
                           int *error = 0,
                           QString *debugString = 0
                                                  );

        QVariant vectorData(const SymbolTable *table, int id,
                            const QString &mass_args,
                            const QStringList &viraj,
                            const QStringList &extraSubexprList,
                            int *error = 0
                                         );

        int pushNewFunctionContext(const QString &name,
                                   const QString &callExpr,
                                   const QStringList &exprList,
                                   const QStringList &extraSubexprList);
        int evaluateBuiltInFunction(const QString &name,
                                    const QStringList &exprList,
                                    const QStringList &extraSubexprList,
                                    QVariant &value);
        int processBuiltInFunc(const QString &name,
                               QVector<QVariant> &args,
                               QVariant &retval);

        int run_f(const QString &name,
                  const QString &vizov,
                  const QStringList &simple_viraj,
                  const QStringList &extraSubexprList,
                  int *err);


        int do_break(int str);
        int do_funct(int str);
        int do_vibor(int str);
        int do_pri(int str);
        int do_nado(int str);
        int do_dano(int str);
        int do_vvod(int str);
        int do_use(int str);

        bool charPrintable(QChar chr)
        {
            if((chr.unicode()>32)&&(chr.unicode()<255))return true;
            else return false;
        }

        QList<proga>* Cur_Proga_Value();

        proga  Cur_Line(int str);
        function_table*  Cur_Functions();

        //        symbol_table*  Cur_Symbols();
        SymbolTable * locals();
        SymbolTable * globals();
        SymbolTable * tableForId(const int varId);


        KumValueStackElem calc_simple_universal_in_Module(int module_id,
                                                          const QStringList &simple_viraj,
                                                          const QStringList &extraExpressions,
                                                          const QString &expression,
                                                          int* error)
        {
            int old_mod_id=m_runStack.top().moduleId;
            m_runStack.top().moduleId=module_id;
            KumValueStackElem result=calc_simple_universal(simple_viraj,extraExpressions,expression,error);
            m_runStack.top().moduleId=old_mod_id;
            return result;
        }


        KumModules * Modules;
        int start_str;

        void getFuncParam(QString Vizov,int * func_id,int *module_id)
        {
            *func_id=-1;
            int emp_pos=Vizov.indexOf('&');
            int break_pos=Vizov.indexOf('|');
            if(emp_pos==-1)emp_pos=Vizov.length();
            // выделяем блок до &
            QString temp;
            temp=Vizov.mid(0,break_pos);
            if(temp[0]=='@')temp=temp.mid(1);
            // номер функции в таблице функций
            bool ok;
            *func_id=temp.toInt(&ok);
            if(!ok)
            {
//                qWarning("getFuncParam:cant get func_id");
            };
            temp=Vizov.mid(break_pos+1,emp_pos-1-break_pos);
            *module_id=temp.toInt(&ok);

            if(!ok)
            {
//                qWarning("getFuncParam:cant get module_id");
            };
        }

        QStack<RunContext> m_runStack;

        bool keyInputFinished;
        uint keyInputCode;
        bool keyInputShift;
        bool keyInputControl;
        bool keyInputAlt;
        bool keyInputMeta;

        int i_lastError;
        int i_lastErrorLine;

        void sendOutput(const QString &text);
    };

} // end namespace Kum

#endif
