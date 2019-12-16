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

#ifndef TEXTANALIZER_H
#define TEXTANALIZER_H

#include <QtCore>

#include "enums.h"
#include "kumirinstruction.h"
#include "kumirerrors.h"

class SymbolTable;
class FunctionTable;


struct ExpressionElement
{
    bool isOper;
    Kumir::ValueType type;
    QChar oper;
};

class TextAnalizer
    : public QObject
{
public:
    enum ArgModifer
    {
        arg,
        res,
        argres
    };
    enum StructType
    {
        NoStruct,
        Constant,
        Variable,
        Array,
        ArrayElement,
        StringElement,
        Substring,
        Function,
        Complex
    };
    TextAnalizer(class TextNormalizer *normalizer, QObject *parent = 0);



    QString ispName;
    bool debugMode;
    bool firstAlg;
    int nestedAlg;
    QString algName;
    QString firstAlgorhitmText;
    int parceFuncList(int module_id,QStringList funcList);
    void buildTables(int moduleId, QList<KumirInstruction>&, SymbolTable&, FunctionTable&);
    void analizeModule(int module_id, QList<KumirInstruction>&, SymbolTable&, FunctionTable&);

    static QVariant createConstValue(const QString &str, const Kumir::ValueType pt);


    Kumir::ValueType parceExpression (
            QString expression,
            FunctionTable &functions,
            SymbolTable &symbols,
            QString algorhitm,
            QStringList &parsedExpression,
            StructType  &expressionType,
            Kumir::CompileError &err,
            QStringList &extraSubExpressions,
            QString &xmlRoot
            );


    Kumir::ValueType resType(int *err,Kumir::ValueType type1,Kumir::ValueType type2,QChar oper);

    QString dropSpace(QString viraj, QList<int> &P, QList<int> &L);

    Kumir::CompileError parceAlgorhitmHeader(int moduleId, KumirInstruction &pv, int str, SymbolTable &symbols, FunctionTable &functions);
    Kumir::CompileError parceVariables( const QString &source,
                        const QString &algName,
                        ArgModifer mod,
                        int str,
                        SymbolTable &symbols,
                        FunctionTable &functions,
                        QList<int> & ids,
                        QList<QStringList> & exprList,
                        QStringList &extraSubExpressions,
                        QStringList &xmlList,
                        bool isArg,
                        QList<KumirInstruction> &pv
                        );

    Kumir::CompileError parceVariablesDeclaration(KumirInstruction &pv,
                                                  int str,
                                                  const QString &alg,
                                                  SymbolTable &symbols,
                                                  FunctionTable &table,
                                                  QString &xml);

    Kumir::CompileError parceAssignment(KumirInstruction &pv, FunctionTable &table, SymbolTable &symbols, int str, QString &root);
    Kumir::CompileError parseLoopBegin(KumirInstruction &pv, SymbolTable &symbols,FunctionTable &functions,int str, QString &xml);
    Kumir::CompileError parceIf(KumirInstruction &pv, SymbolTable &symbols,FunctionTable &functions,int str, QString &xml);
    Kumir::CompileError parceOutput(KumirInstruction &pv,FunctionTable &table,SymbolTable &symbols,int str,QString &xml);
    Kumir::CompileError parceFileOutput(KumirInstruction &pv,FunctionTable &table,SymbolTable &symbols,int str,QString &xml);
    Kumir::CompileError parceAssert(KumirInstruction &pv,FunctionTable &table,SymbolTable &symbols,int str,QString &xml);

    Kumir::CompileError parceConditionalEndloop(KumirInstruction &pv, FunctionTable &functions, SymbolTable &symbols, int str, QString &xml);

    Kumir::CompileError parcePre(KumirInstruction &pv, FunctionTable &table, SymbolTable &symbols,int str, QString &xml);
    Kumir::CompileError parcePost(KumirInstruction &pv,FunctionTable &table, SymbolTable &symbols,int str, QString &xml);

    Kumir::CompileError parceInput(KumirInstruction &pv,FunctionTable &table,SymbolTable &symbols,int str, QString &xml);

    Kumir::CompileError parceFileInput(KumirInstruction &pv, FunctionTable &table, SymbolTable &symbols, int str, QString &xml);

    Kumir::CompileError parceSwitch(KumirInstruction &pv, FunctionTable &table, SymbolTable &symbols, int str);
    Kumir::CompileError parceCase(KumirInstruction &pv, FunctionTable &table, SymbolTable &symbols, int str, QString &xml);
    Kumir::CompileError parceModuleDeclaration(const QString &instr,
                                               FunctionTable &table,
                                               SymbolTable &symbols,
                                               int str,
                                               const QString &fileName,
                                               QString &returnName);
    Kumir::CompileError parceEndModule(const QString &instr);

    Kumir::CompileError parceDocString(const QString &instr,
                                       FunctionTable &functions,
                                       SymbolTable &symbols,
                                       int str,
                                       QString &xml);

    Kumir::CompileError parceImport(const QString &instr, QString &fileName);

    Kumir::CompileError parceImport(KumirInstruction &pv, QString &fileName);

    Kumir::ValueType retUnMassType(Kumir::ValueType inType);
    QStringList splitMassArgs(QString mass_args,QList<int> &starts, int *err);


    Kumir::ValueType getObjType(
            QString					work_block,	// In: Obj descriptor
            Kumir::CompileError     &err,
            FunctionTable			&table,		// In: Function table
            SymbolTable			&symbols,	// In: Symbol table
            QList<Kumir::ValueType>	type_list,	// In: Types of subexpressions
            QList<StructType>,			// In: Descriptors of subexpressions
            QStringList exprList,
            StructType				&StrNew,
            bool &include_bounds_in_error,
            QStringList &parsedNodes,
            QString &root
                            );


    int USER_BASE_ALG_ID;


private:
    QList<QRegExp> Omega;
    QList<KumirInstruction> m_hiddenAlgorhitm;
    QString Delim;
    QString DelimNOT;
    QString Breaks;
    QRegExp operators;
    QRegExp operatorsNOT;
    QRegExp brks;
    int m_denyVariableDeclaration;
    void generateEntryPoint(const QStringList &argGroups,
                            const QList<ArgModifer> &argMods,
                            SymbolTable &symbols,
                            FunctionTable &functions,
                            const QString &callName,
                            const Kumir::ValueType &callReturnType);
    Kumir::CompileError parceAlgorhitmCall(KumirInstruction &pv, FunctionTable &table, SymbolTable &symbols, int str, QString &xml);
    Kumir::CompileError parceConstant(const QString &constant, const Kumir::ValueType pt, int &maxDim, QString &value);

    int shift;
    int prev_str;
    int L(QString s);
    Kumir::ValueType G(QString s,
                       FunctionTable &table,
                       SymbolTable &symbols,
                       const QList<Kumir::ValueType> &type_list,
                       const QList<StructType> &struct_list,
                       const QStringList &expr_list,
                       StructType &strNew,
                       Kumir::CompileError &err,
                       bool &include_bounds_in_error,
                       QStringList &parsedNodes,
                       QString &root);

    void F(const QString &s, QStringList &EL, Kumir::CompileError &error, QList< QList<int> > &P, QList< QList<int> > &L);
    bool isRecursive(QString block, QStringList exprList, int moduleNo, int funcNo, bool firstCall, int& exprNo);
    QString splitLogicalExpressions(const QString &s, QStringList &exprList);

public:

    Kumir::CompileError parceThen(const QString &instr);
    Kumir::CompileError parceElse(const QString &instr);
    Kumir::CompileError parceDone(const QString &instr);
    Kumir::CompileError parceEndLoop(const QString &instr);
    Kumir::CompileError parceEnd(const QString &instr);
    Kumir::CompileError parceAlgorhitmBegin(const QString &instr);
    Kumir::CompileError parceExit(const QString &instr);

    bool isBooleanConstant(const QString &text) const;
    Kumir::ValueType testConst(QString , int*);


public:

    static QString xmlTypeName(const Kumir::ValueType &type);
    static QString xmlOperatorName(const QChar &op);
    static QString xmlEncodeConstantData(const QVariant &data);


private:
    int m_firstAlgorhitmIP;
    class TextNormalizer *m_textNormalizer;
    class ModulesPool *m_modulesPool;
    int i_moduleId;

};

#endif // TEXTANALIZER_H
