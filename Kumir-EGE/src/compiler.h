/****************************************************************************
**
** Copyright (C) 2004-2008 IMPB RAS. All rights reserved.
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
#ifndef COMPILER_H
#define COMPILER_H

#include <QtCore>
class text_analiz;
class PushdownAutomata;

#include "kum_tables.h"
#include "int_proga.h"
#include "kummodules.h"


#define USER_USE 0
#define ROBO_USE -2
#define ROBO_ST_USE -3
#define DRAW_USE -5

#define FILES_USE -4



namespace Kum {

    struct IspLine
    {
        int type;
        QString line;
        int error;
        proga p;
    };

    struct CompileError {
        int code;
        int line;
        int start;
        int end;
    };

    /** Класс потока-компилятора */
    class Compiler : public QObject
    {
        Q_OBJECT
    public:
        /**
     * Конструктор
     * @param parent сслыка на объект-владелец
     */
        Compiler(QObject *parent = 0);
        /** Возвращает true, если поток выполняется или false в противном случае */
        QString firstAlgorhitmText() const;
        void initInternalFunctions();
        int compile(const QString &text, QList<CompileError> &errors);

        inline PushdownAutomata *getPdAutomata() const { return this->pdAutomata ;}
        inline text_analiz *getAnalizer() const { return this->analizer;}
        inline KumSingleModule *mainModule() const { return main; }
        inline int mainModuleId() const {return main_id;}
        /** Деструктор класса */
        ~Compiler();

//        /** Ссылка на таблицу символов */
//        symbol_table *symbols;//TODO Modules
//        /** Ссылка на таблицу функций */
//        function_table *functions;
        /** Ссылка на таблицу исполнителей */


        inline KumModules* modules()
        {
            return &Modules;
        }

        KumModules Modules;



        bool isEmpty(int moduleNo);

        proga compiledLine(int str,int module_id);

        int StartPos();

    protected slots:
    protected:

        /** Добавляет встроенную функцию в таблицы */
        void addInternalFunct(QString description, QString ns = "internal");
        /** Ссылка на анализатор программ */
        text_analiz *analizer;
        /** Флаг выполнения */
        QList<proga> useIspErrors;

        /** Парсировка исп файла*/
        int parceModulesInfoFile(QIODevice *source );
        /** Нормализованный текст программы */
        //QList<ProgaText> *Proga_Text;



        QList<QFile*> IspFiles;
        QList<IspLine> uses_list;
        int oldTabNo;
        int fileTabNo(QString FileName);
        void UnloadByUseType(int Use_Type,QString fileName);
        void appendProgaValue();
        void removeIsp(int first_str_id);
        /**
   * Полная перегрузка исполнителей если изменились встроенные.
   * @return -1 если исполнители перегружены 0 если нет;
   */
        int compareLists(QList<IspLine> *uses_list,QList<IspLine> *cur_list);


        KumSingleModule* main;
        int main_id;
        QList< QList< ProgaText > > normalizedText;
        QList<int> userModIds;

        PushdownAutomata *pdAutomata;
        QString modulesInfoPath;

    };

}

#endif
