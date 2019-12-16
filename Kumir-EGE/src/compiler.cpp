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
#include "compiler.h"
#include "text_analiz.h"
#include "application.h"
#include "messagesprovider.h"
#include "tools.h"
#include "pushdownautomata.h"
#include "pwrulesnormalizer.h"
#include "kumsinglemodule.h"
#include "kum_instrument.h"

#define PD_AUTOMATA_ERROR 100

using namespace Kum;

Compiler::Compiler ( QObject *parent )
    : QObject ( parent )
{
    analizer = new text_analiz();

    analizer->setModules(&Modules);

//    symbols = new symbol_table();
//    functions = new function_table();

    int system_module_id=Modules.appendInternalModule("system",NULL);
    KumSingleModule* system=Modules.module(system_module_id);
    system->Functions()->free(0);
    system->Functions()->addInternalFunction( trUtf8 ( "пауза" ),none,0 );
    system->Functions()->addInternalFunction( trUtf8 ( "стоп" ),none,0 );

    system->Functions()->addInternalFunction( trUtf8 ( "НАЗНАЧИТЬ ВЫВОД" ),none,1 );
    system->Functions()->setArgType(system->Functions()->size()-1, 0, kumString);
    system->Functions()->addInternalFunction( trUtf8 ( "НАЗНАЧИТЬ ВВОД" ),none,1 );
    system->Functions()->setArgType(system->Functions()->size()-1, 0, kumString);

    system->setType(KumSystem);

    KumInstrument* stdFunc=new KumStandartFunctions(&Modules);
    int internal_id=Modules.appendInternalModule("internal",stdFunc);
    KumSingleModule* internal=Modules.module(internal_id);

    qobject_cast<KumStandartFunctions*>(stdFunc)->getFunctionList ( internal->Functions(),internal->Values());

    main_id = Modules.appendMainModule();
    main = Modules.module(main_id);
    

    pdAutomata = new PushdownAutomata(this);
    QString rulesPath;

    rulesPath = QDir::cleanPath(QCoreApplication::applicationDirPath()+"/../share/kumir/pdscripts/");


    QDir rulesDir = QDir(rulesPath);

    QStringList rulesFileList = rulesDir.entryList(QStringList() << "*.rules");
    QBuffer normalizedRules;
    QList<int> priorities;
    for ( int i=0; i<rulesFileList.count(); i++ ) {
        rulesFileList[i] = rulesDir.absoluteFilePath(rulesFileList[i]);
        priorities << i;
    }

    PWRulesNormalizer::process(rulesFileList, &normalizedRules, priorities);
    //    QFile debugFile(rulesPath+"/normalized.out");

    normalizedRules.open(QIODevice::ReadOnly);
    QPoint errCode = pdAutomata->loadRules(&normalizedRules);
    normalizedRules.close();
    normalizedRules.open(QIODevice::ReadOnly);
    //    debugFile.open(QIODevice::WriteOnly|QIODevice::Text);
    //    debugFile.write(normalizedRules.readAll());
    //    debugFile.close();
    normalizedRules.close();

    if ( errCode.x() > 0 ) {
        normalizedRules.open(QIODevice::ReadOnly);
        QTextStream ts(&normalizedRules);
        ts.setCodec("UTF-8");
        QStringList lines = ts.readAll().split("\n",QString::KeepEmptyParts);
        normalizedRules.close();
//        qWarning() << "Error processing rules at line "<< errCode.y() << ". Error code: " << errCode.x() << ", line: " << lines[errCode.y()-1];
    }


}


//QStringList Compiler::getIspText ( QString* FileName,int *err,int use_isp_no )
//{
//    Q_UNUSED(use_isp_no);
//    QStringList ispLines;

//    QFile * ispFile=new QFile ( *FileName ); //Ищем рядом

//    if ( !ispFile ) {*err=2;return ispLines;}
//    if ( *FileName=="" ) {*err=ISP_USE_NOFILE;return ispLines;}
//    if ( !ispFile->exists() ) //Ищем в стандартных
//    {
//        *FileName=QDir::currentPath()+"/"+*FileName;
//        ispFile->setFileName ( *FileName );
//    };
//    QFileInfo ifinfo ( *ispFile );
//    *FileName=ifinfo.absoluteFilePath();




//    if ( !ispFile->exists() ) //В стандартных тоже нет
//    {
//        *err=ISP_USE_NOFILE;
//        delete ispFile;
//        return ispLines;
//    };


//    *FileName=ifinfo.absoluteFilePath();
//    ispFile->setFileName ( *FileName );
//    QString DEBUG=*FileName;
//    if ( ispFile->open ( QIODevice::ReadOnly ) )
//    {
//        IspFiles.append ( ispFile );
//        QTextStream ts ( ispFile );
//        ts.setCodec ( "UTF-16LE" );
//        *err=0;
//        ispLines.clear();
//        while ( !ts.atEnd() )
//        {
//            ispLines.append ( ts.readLine() );
//        };
//        return ispLines;
//    }
//    else
//    {
//        *err=ISP_READ_ERROR;
//        return ispLines;
//    };
//    *err=2;
//    return ispLines;
//}

void Compiler::appendProgaValue()
{

}
int Compiler::compareLists ( QList<IspLine> *now_list,QList<IspLine> *old_list )
{

    int i_count;
    bool wasUser=false;
    int err_start,err_end;
    QString filename_now="";
    QString filename_old="";
    QString unloadFileName="";
    if ( now_list->count() >old_list->count() ) i_count=old_list->count();else i_count=now_list->count();

    for ( int i=0;i<i_count;i++ )
    {


        int state_old=analizer->parce_use_isp ( old_list->at ( i ).line,i,&filename_old,&err_start,&err_end );
        int state_new=analizer->parce_use_isp ( now_list->at ( i ).line,i,&filename_now,&err_start,&err_end );
        if ( ( state_old==USER_USE ) || ( state_new==USER_USE ) )
        {
            if ( wasUser ) return i;
            wasUser=true;
        }
        //Зплатка пользовательских исполнителей
        if ( ( state_old==state_new ) && ( filename_old==filename_now ) ) continue; //Ничего не поменялось
        unloadFileName=filename_old;
        if ( filename_old=="" ) unloadFileName=filename_now;

        if ( ( state_old!=state_new ) )
        {

            return i;
        }
        if ( ( filename_now!=filename_old ) )
        {

            return i;
        }

    }

    if ( now_list->count() <old_list->count() )
    {


        //		int state_new=analizer->parce_use_isp ( old_list->at ( now_list->count() ).line,now_list->count(),&filename_now,&err_start,&err_end );
        if ( unloadFileName=="" ) unloadFileName=filename_now;

        return i_count;
    }


    return i_count;
}


int Compiler::compile(const QString &text, QList<CompileError> &errs)
{


    KumTools *t = KumTools::instance();
    const QStringList lines = text.split("\n", QString::KeepEmptyParts);
    QVector<int> errorCodes;
    errorCodes.resize(lines.size());
    errorCodes.fill(0, lines.size());
    QList<ProgaText> pt;
    QMap<int, CompileError> errors;
    for (int i=0; i<lines.size(); i++) {
        const QString line = lines[i];
        QPoint errPos;
        int a = pt.size();
        int err = t->splitUserLine(line, errPos, &pt);
        for (int j=a; j<pt.size(); j++)
            pt[j].stroka = i;
        if (err>0) {
            errorCodes[i] = err;
            CompileError error;
            error.code = err;
            error.line = i;
            error.start = errPos.x();
            error.end = errPos.x() + errPos.y();
            errors[i] = error;
        }

    }
    for (int i=0; i<pt.size(); i++) {
        pt[i].Base_type = KumTools::instance()->getStringType(pt[i].Text);
    }

    userModIds.clear();


    // Инициализируем анализатор
    analizer->debugMode = false;
    analizer->nestedAlg = 0;
    analizer->ispName ="main";
    analizer->firstAlg = true;
    analizer->firstAlgorhitmText = "";

    Q_CHECK_PTR(main);

//    symbols=main->Values();
//    functions=main->Functions();

    // Очищаем таблицы

    main->Clean();

    int debug = pt.count();

    pdAutomata->init(pt, "\'USER");
    int errorCode = pdAutomata->process();
    if ( errorCode == 1 ) {        
        return PD_AUTOMATA_ERROR;
    }

    if ( errorCode == 2 ) {
        return PD_AUTOMATA_ERROR;
    }

    if ( !pdAutomata->postProcess() ) {
        return PD_AUTOMATA_ERROR;
    }

    debug = pdAutomata->mainModule().count();
    debug = pdAutomata->loads().count();

    Isp mainModule = pdAutomata->mainModule();

    pt = pdAutomata->text();
    for (int i=0; i<mainModule.size(); i++) {
        if (mainModule.at(i).error) {
            mainModule[i].err_start = 0;
            mainModule[i].err_length = mainModule.at(i).line.size();
        }
    }
    main->setProgaValue(mainModule);


    // Получаем текст из редактора, если компилируем только один текст
    userModIds.append(main_id);


    analizer->buildTablesForModule(&Modules,main_id);
    analizer->USER_BASE_ALG_ID=0;

    // Разбор программы

    analizer->analizeModule(main_id);
    analizer->USER_BASE_ALG_ID=0;

    main->removeDummyLines();

    for (int i=0 ; i<main->progaValueCount() ; i++) {
        if (main->Proga_Value_Line(i).error) {
            if (main->Proga_Value_Line(i).real_line.lineNo>-1) {
                Q_ASSERT(i<pt.size());
                QList<int> P = pt[i].P;
                QList<int> L = pt[i].L;
                CompileError error;
                error.line = main->Proga_Value_Line(i).real_line.lineNo;
                error.code = main->Proga_Value_Line(i).error;
                int start = main->Proga_Value_Line(i).err_start;
                if (start<P.size())
                    error.start = P[start];
                else if (P.size()>0)
                    error.start = P.last();
                else
                    error.start = 0;
                int len = main->Proga_Value_Line(i).err_length;
                error.end = error.start;
                if (error.code==65534) {
                    error.start = 0;
                    error.end = main->Proga_Value_Line(i).line.length()-1;
                }
                else {
                    for (int j=start; j<start+len; j++) {
                        if (j<L.size())
                            error.end += L[j];
                    }
                }
                if (!errors.contains(error.line))
                    errors[error.line] = error;
            }
        }
    }

    if (qApp->arguments().contains("-b")) {
        QList< QList<ProgaText> > groupedTerms;
        int curLineNo = 0;
        groupedTerms << QList<ProgaText>();
        for (int i=0; i<pt.size(); i++) {
            pt[i].Error = main->Proga_Value_Line(i).error;
            pt[i].errstart = main->Proga_Value_Line(i).err_start;
            pt[i].errlen = main->Proga_Value_Line(i).err_length;
            if (pt[i].stroka==curLineNo) {
                groupedTerms[curLineNo] << pt[i];
            }
            else if (pt[i].stroka>curLineNo) {
                groupedTerms << QList<ProgaText>();
                curLineNo++;
                groupedTerms[curLineNo] << pt[i];
            }
        }
        QVector< QVector<bool> > errorMasks(groupedTerms.size());
        QVector< QPoint > indentRanks(groupedTerms.size());
        QString outText;
        QString marginText;
        int currentIndent = 0;
        for (int i=0; i<groupedTerms.size(); i++) {
            errorMasks[i] = KumTools::instance()->recalculateErrorPositions(lines[i], groupedTerms[i]);
            indentRanks[i] = KumTools::instance()->getIndentRang(groupedTerms[i]);
            currentIndent += indentRanks[i].x();
            for (int j=0; j<currentIndent; j++) {
                outText += ". ";
            }
            Q_ASSERT(lines[i].size()==errorMasks[i].size());
            bool inErr = false;
            for (int j=0; j<lines[i].length(); j++) {
                if (!inErr && errorMasks[i][j]) {
                    outText += "<u>";
                    inErr = true;
                }
                outText += lines[i][j];
                if (inErr && !errorMasks[i][j]) {
                    outText += "</u>";
                    inErr = false;
                }
            }
            if (inErr)
                outText += "</u>";
            outText += "\n";
            currentIndent += indentRanks[i].y();
            int firstErrorCode = 0;
            int errorsCount = 0;
            for (int j=0; j<groupedTerms[i].size(); j++) {
                if (groupedTerms[i][j].error()) {
                    if (!firstErrorCode) {
                        firstErrorCode = groupedTerms[i][j].error();
                    }
                    errorsCount += 1;
                }
            }
            if (firstErrorCode) {
                for (int j=1; j<errorsCount; j++) {
                    marginText += "> ";
                }
                marginText += app()->instance()->messagesProvider->errorMessage(firstErrorCode);
                marginText += " ("+QString::number(firstErrorCode)+")";
            }
            marginText += "\n";
        }
        QString out1, out2;
        for (int i=1; i<qApp->arguments().size(); i++) {
            if (qApp->arguments()[i].startsWith("--o1=")) {
                out1 = qApp->arguments()[i].mid(QString("--o1=").size());
                break;
            }
            if (qApp->arguments()[i].startsWith("--o2=")) {
                out2 = qApp->arguments()[i].mid(QString("--o2=").size());
                break;
            }
        }
        QFile outFile(out1);
        if (outFile.open(QIODevice::WriteOnly|QIODevice::Text)) {
            QTextStream ts(&outFile);
            ts.setCodec("UTF-8");
            ts << outText;
            outFile.close();
        }
        else {
            qDebug() << "Can't open out1 file: " << out1;
        }
        QFile marginsFile(out2);
        if (marginsFile.open(QIODevice::WriteOnly|QIODevice::Text)) {
            QTextStream ts(&marginsFile);
            ts.setCodec("UTF-8");
            ts << marginText;
            marginsFile.close();
        }
        else {
            qDebug() << "Can't open out2 file: " << out2;
        }
    }

    for (int i=0; i<lines.size(); i++) {
        if (errors.contains(i)) {
            errs << errors[i];
        }
    }

    return 0;
}


Compiler::~Compiler()
{
}


QString Compiler::firstAlgorhitmText() const
{
    return analizer->firstAlgorhitmText;
}


bool Compiler::isEmpty(int moduleNo)
{
    KumSingleModule *module = Modules.module(moduleNo);
    QList<proga> * data = module->Proga_Value();
    return data->size()==0;
}

proga Compiler::compiledLine(int str, int module_id)
{
    return Modules.module(module_id)->Proga_Value_Line(str);
}

int Compiler::StartPos()
{
    return main->initialIP();
}
