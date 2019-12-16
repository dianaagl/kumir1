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
/*
=== THIS FILE ENCODED IN UTF-8 LOCALE
*/


#include "textnormalizer.h"
#include "ksymbols.h"
#include "errorcodes.h"


void addToMap(QHash<QString,QString> *map,
              const QString &value, const QChar &ksymbol)
{
    QStringList variants = value.split("|");
    foreach (QString variant, variants) {
        variant.remove("\\s+");
        variant.remove("\\s*");
        variant.remove("\\s");
        variant.remove("_");
        (*map)[variant] = QString(ksymbol);
    }
}

void TextNormalizer::initNormalizator(const QString &fileName)
{

    QHash<QString,QString> *map = new QHash<QString,QString>();

    QFile kwdFile(fileName);
    if (kwdFile.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QTextStream ts(&kwdFile);
        ts.setCodec("UTF-8");
        const QStringList lines = ts.readAll().split("\n",QString::SkipEmptyParts);
        kwdFile.close();
        foreach (const QString &line, lines) {
            if (line.startsWith("#"))
                continue;
            const QStringList pair = line.split("=");
            if (pair.size()>=2) {
                const QString context = pair.at(0).simplified();
                const QString value = pair.at(1).simplified();
                if (context=="begin module") {
                    keyWords << value;
                    ispKwd = new QString(value);
                    addToMap(map, value, KS_MODULE);
                }
                else if (context=="end module") {
                    keyWords << value;
                    addToMap(map, value, KS_ENDMODULE);
                }
                else if (context=="use module") {
                    keyWords << value;
                    useKwd = new QString(value);
                    addToMap(map, value, KS_IMPORT);
                }
                else if (context=="algorhitm header") {
                    keyWords << value;
                    addToMap(map, value, KS_ALG);
                }
                else if (context=="begin algorhitm implementation") {
                    keyWords << value;
                    addToMap(map, value, KS_BEGIN);
                }
                else if (context=="end algorhitm implementation") {
                    keyWords << value;
                    addToMap(map, value, KS_END);
                }
                else if (context=="algorhitm pre-condition") {
                    keyWords << value;
                    addToMap(map, value, KS_PRE);
                }
                else if (context=="algorhitm post-condition") {
                    keyWords << value;
                    addToMap(map, value, KS_POST);
                }
                else if (context=="assertion") {
                    keyWords << value;
                    addToMap(map, value, KS_ASSERT);
                }
                else if (context=="begin loop") {
                    keyWords << value;
                    addToMap(map, value, KS_LOOP);
                }
                else if (context=="end loop") {
                    keyWords << value;
                    addToMap(map, value, KS_ENDLOOP);
                }
                else if (context=="conditional end loop") {
                    keyWords << value;
                    addToMap(map, value, KS_COND_ENDLOOP);
                }
                else if (context=="'for' loop type") {
                    keyWords << value;
                    addToMap(map, value, KS_FOR);
                }
                else if (context=="'while' loop type") {
                    keyWords << value;
                    addToMap(map, value, KS_WHILE);
                }
                else if (context=="'times' loop type") {
                    keyWords << value;
                    addToMap(map, value, KS_TIMES);
                }
                else if (context=="for loop 'from'") {
                    keyWords << value;
                    addToMap(map, value, KS_FROM);
                }
                else if (context=="for loop 'to'") {
                    keyWords << value;
                    addToMap(map, value, KS_TO);
                }
                else if (context=="for loop 'step'") {
                    keyWords << value;
                    addToMap(map, value, KS_STEP);
                }
                else if (context=="loop break and algorhitm return") {
                    keyWords << value;
                    addToMap(map, value, KS_EXIT);
                }
                else if (context=="if") {
                    keyWords << value;
                    addToMap(map, value, KS_IF);
                }
                else if (context=="then") {
                    keyWords << value;
                    addToMap(map, value, KS_THEN);
                }
                else if (context=="else") {
                    keyWords << value;
                    addToMap(map, value, KS_ELSE);
                }
                else if (context=="end of 'if' or 'switch' statement") {
                    keyWords << value;
                    addToMap(map, value, KS_DONE);
                }
                else if (context=="switch") {
                    keyWords << value;
                    addToMap(map, value, KS_SWITCH);
                }
                else if (context=="case") {
                    keyWords << value;
                    addToMap(map, value, KS_CASE);
                }
                else if (context=="terminal input") {
                    keyWords << value;
                    addToMap(map, value, KS_INPUT);
                }
                else if (context=="terminal output") {
                    keyWords << value;
                    addToMap(map, value, KS_OUTPUT);
                }
                else if (context=="file input") {
                    keyWords << value;
                    addToMap(map, value, KS_FINPUT);
                }
                else if (context=="file output") {
                    keyWords << value;
                    addToMap(map, value, KS_FOUTPUT);
                }
                else if (context=="new line symbol") {
                    keyWords << value;
                    addToMap(map, value, KS_NEWLINE);
                }
                else if (context=="logical 'not'") {
                    keyWords << value;
                    addToMap(map, value, KS_NOT);
                }
                else if (context=="logical 'and'") {
                    keyWords << value;
                    addToMap(map, value, KS_AND);
                }
                else if (context=="logical 'or'") {
                    keyWords << value;
                    addToMap(map, value, KS_OR);
                }
                else if (context=="'write only' parameter modifier") {
                    keyWords << value;
                    addToMap(map, value, KS_RES);
                }
                else if (context=="'read only' parameter modifier") {
                    keyWords << value;
                    addToMap(map, value, KS_ARG);
                }
                else if (context=="'read and write' parameter modifier") {
                    keyWords << value;
                    addToMap(map, value, KS_ARGRES);
                }
                else if (context=="'true' constant value") {
                    constNames << value;
                    addToMap(map, value, KS_TRUE);
                }
                else if (context=="'false' constant value") {
                    constNames << value;
                    addToMap(map, value, KS_FALSE);
                }
                else if (context=="integer type name") {
                    typeNames << value;
                    addToMap(map, value, KS_INT);
                }
                else if (context=="floating point type name") {
                    typeNames << value;
                    addToMap(map, value, KS_REAL);
                }
                else if (context=="character type name") {
                    typeNames << value;
                    addToMap(map, value, KS_CHAR);
                }
                else if (context=="string type name") {
                    typeNames << value;
                    addToMap(map, value, KS_STRING);
                }
                else if (context=="boolean type name") {
                    typeNames << value;
                    addToMap(map, value, KS_BOOL);
                }
                else if (context=="integer array type name") {
                    typeNames << value;
                    addToMap(map, value, KS_INT_TAB);
                }
                else if (context=="floating point array type name") {
                    typeNames << value;
                    addToMap(map, value, KS_REAL_TAB);
                }
                else if (context=="character array type name") {
                    typeNames << value;
                    addToMap(map, value, KS_CHAR_TAB);
                }
                else if (context=="string array type name") {
                    typeNames << value;
                    addToMap(map, value, KS_STRING_TAB);
                }
                else if (context=="boolean array type name") {
                    typeNames << value;
                    addToMap(map, value, KS_BOOL_TAB);
                }

            }
        }
    }



    compounds2 << tr("\\*\\*");
    (*map)["**"] = QString(QChar(KS_POWER));

    compounds2 << ">=";
    (*map)[">="] = QString(QChar(KS_GEQ));

    compounds2 << "<=";
    (*map)["<="] = QString(QChar(KS_LEQ));

    compounds2 << "<>";
    (*map)["<>"] = QString(QChar(KS_NEQ));

    compounds2 << "#";
    (*map)["#"] = QString(QChar(KS_DOC));

    compounds2 << ":=";
    (*map)[":="] = QString(QChar(KS_ASSIGN));

    compounds << "\\~";
    (*map)["~"] = QString(QChar(KS_TILDA));

    compounds2 << "&";
    (*map)["&"] = QString(QChar(KS_PAR));

    compounds2 << "%";
    (*map)["%"] = QString(QChar(KS_PERCENT));

    compounds2 << "\"";
    compounds2 << "'";
    compounds2 << QString::fromUtf8("”");
    compounds2 << "\\|";

    compounds += keyWords;
    compounds += typeNames;
    compounds += constNames;

    keyWords << tr("value");



    for ( int i=0; i<keyWords.count(); i++ )
        keyWords[i] = "\\b"+keyWords[i]+"\\b";

    rxKeyWords = new QRegExp(keyWords.join("|"));
    rxKeyWords->setMinimal(false);

    for ( int i=0; i<typeNames.count(); i++ )
        typeNames[i] = "\\b"+typeNames[i]+"\\b";

    for ( int i=0; i<constNames.count(); i++ )
        constNames[i] = "\\b"+constNames[i]+"\\b";

    rxTypes = new QRegExp(typeNames.join("|"));

    rxConst = new QRegExp(constNames.join("|"));

    kwdMap = map;

    rxCompound = new QRegExp(compounds2.join("|")+"|\\b"+compounds.join("\\b|\\b")+"\\b");
    rxCompound->setMinimal(false);
}

void TextNormalizer::init()
{
    KSIMS = new QRegExp ( "[\\x3E8-\\x3EF]+|[\\x1100-\\x1200]+" );
    Breaks = QString::fromUtf8 ( "[+|№|\\^|$|;|=|\\?|*|~|`|\\-|\\\\|/|<|>|\\(|\\)|\\[|\\]|\\,|\\:|]|(\\^\\d+)");
    qFatal("Unimplemented!");
    brks = QRegExp ( Breaks );
    rxLitBreaks = QRegExp(QString::fromUtf8(
            "\\b(цел[\\s]+таб|цел|целтаб)\\b|\\b(вещ[\\s]+таб|вещ|вещтаб)\\b|\\b(лит[\\s]+таб|лит[\\s]+|литтаб)\\b|\\b(сим[\\s]+таб|сим|симтаб)\\b|\\b(лог[\\s]+таб|лог|логтаб)\\b|<=|>=|\\$|:=|\\\\|#|\\*\\*|\\*|:|&|\\?|~|%|\\^|`|;|=|≠|№|<|≤|≥|>|<>|\\[|\\]|\\(|\\)|,|-|/|\\+|\\bвсе\\b|\\bалг\\b|\\bесли\\b|\\bто\\b|\\bиначе\\b|\\bнач\\b|\\bкон\\b|\\bшаг\\b|\\bнц\\b|\\bкц\\b|\\bдля\\b|\\bраз\\b|\\bпока\\b|\\bдо\\b|\\bот\\b|\\bарг\\s+рез\\b|\\bарг\\b|\\bрез\\b|\\bаргрез\\b|\\bввод\\b|\\bвывод\\b|\\bутв\\b|\\bдано\\b|\\bнадо\\b|\\bкц_при\\b|\\bвыбор\\b|\\bи\\b|\\bили\\b|\\bпри\\b|\\bнс\\b|\\bиспользовать\\b|\\bисп\\b|\\bкон_исп\\b"
            ));

    rx_NOT=QRegExp ( "\\b"+ trUtf8( "не" ) + "\\b" );
    rx_OR=QRegExp ( "\\b"+ trUtf8 ( "или" ) + "\\b" );
    rx_AND=QRegExp ( "\\b"+ trUtf8 ( "и" ) + "\\b" );
    rx_INT_TAB=QRegExp ( "\\b"+ trUtf8 ( "цел таб" ) + "\\b" );
    rx_STRING_TAB=QRegExp ( "\\b"+ trUtf8 ( "лит таб" ) + "\\b" );
    rx_REAL_TAB=QRegExp ( "\\b"+ trUtf8 ( "вещ таб" ) + "\\b" );
    rx_CHAR_TAB=QRegExp ( "\\b"+ trUtf8 ( "сим таб" ) + "\\b" );
    rx_BOOL_TAB=QRegExp ( "\\b"+ trUtf8 ( "лог таб" ) + "\\b" );
    rx_INT=QRegExp ( "\\b"+ trUtf8 ( "цел" ) + "\\b" );
    rx_STRING=QRegExp ( "\\b"+ trUtf8 ( "лит" ) + "\\b" );
    rx_REAL=QRegExp ( "\\b"+ trUtf8 ( "вещ" ) + "\\b" );
    rx_CHAR=QRegExp ( "\\b"+ trUtf8 ( "сим" ) + "\\b" );
    rx_BOOL=QRegExp ( "\\b"+ trUtf8 ( "лог" ) + "\\b" );
    rx_IF=QRegExp ( "\\b"+ trUtf8 ( "если" ) + "\\b" );
    rx_TO=QRegExp ( "\\b"+ trUtf8 ( "то" ) + "\\b" );
    rx_ELSE=QRegExp ( "\\b"+ trUtf8 ( "иначе" ) + "\\b" );
    rx_DONE=QRegExp ( "\\b"+ trUtf8 ( "все" ) + "\\b" );
    rx_SWITCH=QRegExp ( "\\b"+ trUtf8 ( "выбор" ) + "\\b" );
    rx_CASE=QRegExp ( "\\b"+ trUtf8 ( "при" ) + "\\b" );
    rx_ALG=QRegExp ( "\\b"+ trUtf8 ( "алг" ) + "\\b" );
    rx_ARG=QRegExp ( "\\b"+ trUtf8 ( "арг" ) + "\\b" );
    rx_BEGIN=QRegExp ( "\\b"+ trUtf8 ( "нач" ) + "\\b" );
    rx_END=QRegExp ( "\\b"+ trUtf8 ( "кон" ) + "\\b" );
    rx_LOOP=QRegExp ( "\\b"+ trUtf8 ( "нц" ) + "\\b" );
    rx_ENDLOOP=QRegExp ( "\\b"+ trUtf8 ( "кц" ) + "\\b" );
    rx_FOR=QRegExp ( "\\b"+ trUtf8 ( "для" ) + "\\b" );
    rx_TIMES=QRegExp ( "\\b"+ trUtf8 ( "раз" ) + "\\b" );
    rx_WHILE=QRegExp ( "\\b"+ trUtf8 ( "пока" ) + "\\b" );
    rx_PRE=QRegExp ( "\\b"+ trUtf8 ( "дано" ) + "\\b" );
    rx_POST=QRegExp ( "\\b"+ trUtf8 ( "надо" ) + "\\b" );
    rx_EXIT=QRegExp ( "\\b"+ trUtf8 ( "выход" ) + "\\b" );
    rx_NEWLINE=QRegExp ( "\\b"+ trUtf8 ( "нс" ) + "\\b" );
    rx_MODULE=QRegExp ( "\\b"+ trUtf8 ( "исп" ) +"\\b" );
    rx_ENDMODULE=QRegExp ( "\\b"+ trUtf8 ( "кон_исп" ) + "\\b" );
    rx_FROM = QRegExp ( "\\b"+trUtf8 ( "от" ) +"\\b" );
    rx_TO = QRegExp ( "\\b"+trUtf8 ( "до" ) +"\\b" );
    rx_STEP = QRegExp ( "\\b"+trUtf8 ( "шаг" ) +"\\b" );
    rx_RES = QRegExp ( "\\b"+trUtf8 ( "рез" ) +"\\b" );
    rx_ARGRES = QRegExp ( "\\b"+trUtf8 ( "аргрез" ) +"\\b" );
    rx_OUTPUT = QRegExp ( "\\b"+trUtf8 ( "вывод" ) +"\\b" );
    rx_TRUE = QRegExp ( "\\b"+trUtf8 ( "да" ) +"\\b" );
    rx_FALSE = QRegExp ( "\\b"+trUtf8 ( "нет" ) +"\\b" );
    rx_ASSERT = QRegExp ( "\\b"+trUtf8 ( "утв" ) +"\\b" );

    st_INT=QString ( QChar ( KS_INT ) ) +" ";
    st_STRING=QString ( QChar ( KS_STRING ) ) +" ";
    st_CHAR=QString ( QChar ( KS_CHAR ) ) +" ";
    st_REAL=QString ( QChar ( KS_REAL ) ) +" ";
    st_BOOL=QString ( QChar ( KS_BOOL ) ) +" ";

    kwdMap = NULL;
    rxCompound = NULL;
}

TextNormalizer::TextNormalizer()
{
    init();
    m_allowHiddenNames = false;
}

int TextNormalizer::getKSCount ( int pos,QString stroka )
{
    QString ksims="[+|,|=|~|**|*|\\-|/|<|>|\\(|\\)|\\[|\\]|";
    qFatal("Unimplemented!");
    QRegExp operators;
    operators=QRegExp ( ksims );
    int i=0;
    int pos_t=0;
    int count=0;
    if ( pos==-1 ) pos=stroka.length();
    while ( i<pos )
    {
        pos_t=operators.indexIn ( stroka,pos_t+1 );
        if ( pos_t==-1 ) return count;
        count++;
        i=pos_t;

    };
    return count;


};


int TextNormalizer::getPosByKS ( int ks_id,QString stroka )
{
    QString ksims="[+|,|=|~|*|\\-|/|<|>|\\(|\\)|\\[|\\]|";
    qFatal("Unimplemented!");

    QRegExp operators;
    operators=QRegExp ( ksims );
    int i=0;
    int pos=0;
    while ( i<ks_id )
    {
        pos=operators.indexIn ( stroka,pos+1 );
        if ( pos==-1 ) return -1;
        i++;

    };
    return pos;
};


/**
 * Замена ключевых слов на Ксимволы в Строке
 * @param string Строка
 * @return Код ошибки
 */
QString TextNormalizer::zamena ( QString string )
{


    // int comment=string.indexOf("|");
    // string=string.left(comment);

    string.replace ( "**",QChar ( KS_POWER ) );
    string.replace ( ">=",QChar ( KS_GEQ ) );
    string.replace ( "<=",QChar ( KS_LEQ ) );
    string.replace ( "<>",QChar ( KS_NEQ ) );
    string.replace ( "#",QChar ( KS_DOC ) );

    // Если строка документации,
    // то выход, иначе в тексте строки
    // документации появятся ксимволы
    if ( string.startsWith ( QChar ( KS_DOC ) ) )
        return string;



    QString t_string = "";
    string.replace ( rx_NOT,QChar ( KS_NOT ) );

    t_string = "\\b"+ trUtf8 ( "да" ) + "\\b";
    QRegExp rx=QRegExp ( t_string );
    string.replace ( rx,QChar ( KS_TRUE ) );

    t_string = "\\b"+ trUtf8 ( "использовать" ) + "\\b";
    rx=QRegExp ( t_string );
    string.replace ( rx,QChar ( KS_IMPORT ) );

    t_string = "\\b"+ trUtf8 ( "нет" ) + "\\b";
    rx=QRegExp ( t_string );
    string.replace ( rx,QChar ( KS_FALSE ) );

    t_string = "\\b"+ trUtf8 ( "вещтаб" ) + "\\b";
    rx=QRegExp ( t_string );
    string.replace ( rx,QChar ( KS_REAL_TAB ) );

    t_string = "\\b"+ trUtf8 ( "целтаб" ) + "\\b";
    rx=QRegExp ( t_string );
    string.replace ( rx,QChar ( KS_INT_TAB ) );

    t_string = "\\b"+ trUtf8 ( "логтаб" ) + "\\b";
    rx=QRegExp ( t_string );
    string.replace ( rx,QChar ( KS_BOOL_TAB ) );

    t_string = "\\b"+ trUtf8 ( "симтаб" ) + "\\b";
    rx=QRegExp ( t_string );
    string.replace ( rx,QChar ( KS_CHAR_TAB ) );

    t_string = "\\b"+ trUtf8 ( "литтаб" ) + "\\b";
    rx=QRegExp ( t_string );
    string.replace ( rx,QChar ( KS_STRING_TAB ) );


    t_string = "\\b"+ trUtf8 ( "ввод" ) + "\\b";
    rx=QRegExp ( t_string );
    string.replace ( rx,QChar ( KS_INPUT ) );

    t_string = "\\b"+ trUtf8 ( "ф_ввод" ) + "\\b";
    rx=QRegExp ( t_string );
    string.replace ( rx,QChar ( KS_FINPUT ) );

    t_string = "\\b"+ trUtf8 ( "ф_вывод" ) + "\\b";
    rx=QRegExp ( t_string );
    string.replace ( rx,QChar ( KS_FOUTPUT ) );

    t_string = "\\b"+ trUtf8 ( "кц_при" ) + "\\b";
    rx=QRegExp ( t_string );
    string.replace ( rx,QChar ( KS_COND_ENDLOOP ) );

    t_string = "\\b"+ trUtf8 ( "нс" ) + "\\b";
    rx=QRegExp ( t_string );
    string.replace ( rx,QChar ( KS_NEWLINE ) );


    string.replace ( ":=",QChar ( KS_ASSIGN ) );

    string.replace ( rx_OR,QChar ( KS_OR ) );

    string.replace ( rx_AND,QChar ( KS_AND ) );

    string.replace ( rx_INT_TAB,QChar ( KS_INT_TAB ) );

    string.replace ( rx_STRING_TAB,QChar ( KS_STRING_TAB ) );

    string.replace ( rx_REAL_TAB,QChar ( KS_REAL_TAB ) );

    string.replace ( rx_CHAR_TAB,QChar ( KS_CHAR_TAB ) );

    string.replace ( rx_BOOL_TAB,QChar ( KS_BOOL_TAB ) );

    string.replace ( rx_PRE,QChar ( KS_PRE ) );

    string.replace ( rx_POST,QChar ( KS_POST ) );

    int pos1 = 0;
    int len = 0;
    int pos2 = 0;

    string.replace ( rx_INT, st_INT );

    pos1 = 0;
    len = 0;
    pos2 = 0;
    QRegExp rx_STRING_Func = QRegExp ( trUtf8 ( "лит\\s*\\(" ) );
    int length = string.length();
    while ( pos1 < length )
    {
        pos1 = rx_STRING.indexIn ( string,pos1 );
        if ( pos1 == -1 )
            break;
        pos2 = rx_STRING_Func.indexIn ( string,pos1 );
        len = rx_STRING.matchedLength();
        if ( pos1 != pos2 )
        {
            string.replace ( pos1,len, st_STRING );
        }
        pos1 += len;
    }

    t_string = "\\b"+ trUtf8 ("арг рез" ) + "\\b";
    rx=QRegExp ( t_string );
    string.replace ( rx,QChar ( KS_ARGRES ) );

    string.replace ( rx_REAL, st_REAL );

    string.replace ( rx_CHAR, st_CHAR );

    string.replace ( rx_BOOL, st_BOOL );

    string.replace ( rx_IF,QChar ( KS_IF ) );

    string.replace ( rx_TO,QChar ( KS_THEN ) );

    string.replace ( rx_ELSE,QChar ( KS_ELSE ) );

    string.replace ( rx_DONE,QChar ( KS_DONE ) );

    string.replace ( rx_SWITCH,QChar ( KS_SWITCH ) );

    string.replace ( rx_CASE,QChar ( KS_CASE ) );

    string.replace ( rx_ALG,QChar ( KS_ALG ) );

    string.replace ( rx_ARG,QChar ( KS_ARG ) );

    string.replace ( rx_BEGIN,QChar ( KS_BEGIN ) );

    string.replace ( rx_END,QChar ( KS_END ) );

    string.replace ( rx_LOOP,QChar ( KS_LOOP ) );

    string.replace ( rx_ENDLOOP,QChar ( KS_ENDLOOP ) );

    string.replace ( rx_FOR,QChar ( KS_FOR ) );

    string.replace ( rx_TIMES,QChar ( KS_TIMES ) );

    string.replace ( rx_WHILE,QChar ( KS_WHILE ) );

    string.replace ( rx_EXIT,QChar ( KS_EXIT ) );

    string.replace ( rx_MODULE,QChar ( KS_MODULE ) );
    string.replace ( rx_ENDMODULE,QChar ( KS_ENDMODULE ) );

    string.replace ( rx_FROM,QChar ( KS_FROM ) );


    //string.replace("@",QChar(KS_AT));


    string.replace ( "~",QChar ( KS_TILDA ) );

    //string.replace("$",QChar(KS_TOLLAR));
    string.replace ( "&",QChar ( KS_PAR ) );

    string.replace ( "%",QChar ( KS_PERCENT ) );

    string.replace ( rx_TO,QChar ( KS_TO ) );

    string.replace ( rx_STEP,QChar ( KS_STEP ) );

    string.replace ( rx_RES,QChar ( KS_RES ) );

    string.replace ( rx_ARGRES,QChar ( KS_ARGRES ) );

    string.replace ( rx_OUTPUT,QChar ( KS_OUTPUT ) );

    string.replace ( rx_TRUE,QChar ( KS_TRUE ) );

    string.replace ( rx_FALSE,QChar ( KS_FALSE ) );

    string.replace ( rx_ASSERT,QChar ( KS_ASSERT ) );

    string.replace ( "; ",";" );


    //Замена русских букв на латинские схожие по начертанию



    return string;
};

int TextNormalizer::ks_count ( QString string,int begin,int end )
{
    string=string.mid ( begin,end-begin );
    QRegExp break_rx=QRegExp ( QString ( "[+|*|\\-|/|~|)|(|=|,|"+QString ( QChar ( KS_IF ) )
                                         +"|>|<|"+QString ( QChar ( KS_THEN ) ) +"|"+QString ( QChar ( KS_AND ) ) +"]" ) );

    return string.count ( break_rx );

};


/**
 * Нормализация строки
 * @param src IN: строка исходного Кумир-текста
 * @param P OUT: вектор начальных позиций нормализованных символов в исходной строке
 * @param L OUT: вектор длин нормализованных символов в исходной строке
 * @param error OUT: код ошибки при нормализации
 * @param err_start OUT: позиция начала ошибки в исходной строке
 * @param err_len OUT: длина ошибочной области в исходной строке
 * @return нормализованная строка
 */
QString TextNormalizer::normalize( const QString & src, QList<int> &P, QList<int> &L, int &error, int &err_start, int &err_len )
{
    // различные проверки

    Q_CHECK_PTR ( rxCompound );
    Q_CHECK_PTR ( kwdMap );
    Q_ASSERT ( rxCompound->isValid() );
    Q_ASSERT ( !rxCompound->pattern().isEmpty() );


    /*
        Имеется регулярное выражение (rxCompound, инициализируется в initNormalizator),
        описывающее все составные ксимволы, а также символы «кавычка» и «комментарий».

        В цикле по длине строки выполняется поиск позиций подстрок в соответствии с
        данным регулярным выражением, при этом каждый i-ый поиск даёт два числа:
        позиция найденного вхождения  p_i и его длина l_i.

        При этом строится нормализованная строка и соответствующие вектора [P] и [L]
        в соответствии с таблицей замен (kwdMap, инициализируется в initNormalizator).

        Если найденное вхождение находится в литеральной константе, то выполняется сдвиг
        кода символа на STR_HIDE_OFFSET.

        Если найден символ комментария, то цикл останавливается. Если позиция p_i
        отличается от p_{i-1} больше чем на 1, то нормализованная строка а также вектора
        заполняются символами исходной строки начиная с позиции p_{i-1} + l_{i-1} до позиции p_{i-1},
        после чего добавляется соответствующий ксимвол.
        */
    // знач
    QString result = "";
    // флаг "внутри литеральной константы"
    bool inLit = false;
    // символ используемой кавычки в литеральной константе
    QChar litSymb;
    // текущая и предыдущая позиции найденного вхождения
    int cur, prev;
    prev = -1;
    // флаг "пропускать пробелы"
    bool skipSpaces = true;
    // инициализация векторов (пустые)
    P = QList<int>();
    L = QList<int>();
    // инициализация переменных
    error = 0;
    err_start = 0;
    err_len = 0;
    // если строка пустая, то выходим
    if ( src.isEmpty() )
        return result;
    // бексонечный цикл (останавливается изнутри оператором break)
    forever {
        // поиск следующего вхождения составного ксимвола, кавычки или комментария
        cur = rxCompound->indexIn(src,prev+1);
        if ( cur != -1 ) {
            // если такой найден...
            // проверяем, нет ли между этим и предыдущим ксимволом чего-нибудь
            if ( cur - prev > 1 ) {
                // здесь обрабатываются все символы между текущим
                // и предыдущим ксимволом

                if ( inLit ) {
                    // случай, когда находимся в литеральной константе

                    // добавляем все символы из исходной строки, находящиеся
                    // в области [prev+1; cur)

                    // при этом заменяем коды их символов
                    for ( int j=prev+1; j<cur; j++ ) {
                        result += QString(QChar(src[j].unicode()+STR_HIDE_OFFSET));
                        P << j;
                        L << 1;
                    }
                }
                else {
                    // случай, когда вне литеральной константы

                    // всё то же самое, только коды символов оставляем незатронутыми,
                    // при этом нормализуем пробелы
                    for ( int j=prev+1; j<cur; j++ ) {
                        if ( src[j] == ' ' || src[j]=='\t') {
                            if ( !skipSpaces ) {
                                result += QString(src[j]);
                                P << j;
                                L << 1;
                            }
                            skipSpaces = true;
                        }
                        else {
                            result += QString(src[j]);
                            P << j;
                            L << 1;
                            if ( src[j]==';' )
                                skipSpaces = true;
                            else
                                skipSpaces = false;
                        }
                    }
                }
            }

            // Теперь разбираемся с текущим ксимволом.
            // Проверяем, не находится ли он в литеральной константе...
            if ( inLit ) {
                // если найденное вхождение находится в литеральной константе

                // сначала проверяем, не является ли данный символ закрывающей кавычкой
                QString symb = rxCompound->cap(0);
                if ( symb=="\"" || symb=="'" || symb==trUtf8("”") ) {
                    if ( symb[0]==litSymb ) {
                        // найденный символ является закрывающей кавычкой

                        // вне зависимости от исходной кавычки,
                        // в нормализованной строке кавычка -- это "
                        result += "\"";
                        P << cur;
                        L << 1;
                        // сбрасываем флаг "внутри литеральной константы"
                        inLit = false;
                    }
                    else {
                        // если найденная кавычка не является закрывающей,
                        // то добавляем её со сдвигом кода символа
                        // и флаг "внутри литеральной константы" НЕ сбрасываем
                        result += QString(QChar(src[cur].unicode()+STR_HIDE_OFFSET));
                        P << cur;
                        L << 1;
                    }
                }
                else {
                    // если найденный символ не является никакой кавычкой

                    // поскольку мы находимся внутри литеральной константы,
                    // то нормализовать ключевые слова и пр. не нужно

                    // добавляем их со сдвигом кода символа
                    // (при этом учитываем, что длина найденного вхождения
                    // может быть больше одного символа)
                    for ( int j=cur; j<cur+rxCompound->matchedLength(); j++ ) {
                        result += QString(QChar(src[j].unicode()+STR_HIDE_OFFSET));
                        P << j;
                        L << 1;
                    }
                }
            }
            else {
                // случай, когда найденное вхождение ВНЕ литеральной константы

                // проверяем, не является ли найденный символ признаком комментария
                QString symb = rxCompound->cap(0);
                if ( symb=="|" ) {
                    // если комментарий, то завершаем нормализацию этой строки
                    break;
                }

                else if ( symb=="#" ) {
                    result += QChar(KS_DOC) + src.mid(cur+1);
                    P << cur;
                    L << 1;
                    for (int j=cur+1; j<src.length(); j++) {
                        P << j;
                        L << 1;
                    }
                    break;
                }

                // проверяем на открывающую кавычку
                else if ( symb=="\"" || symb=="'" || symb==trUtf8("”") ) {
                    // запоминаем, какая кавычка у нас является открывающей
                    litSymb = symb[0];
                    // добавляем в нормализованную строку "
                    result += "\"";
                    P << cur;
                    L << 1;
                    // устанавливаем флаг "внутри литеральной константы"
                    inLit = true;
                }

                // самый общий случай -- не кавычка и не комментарий
                else {
                    // выполняем поиск в хеш-таблице замен и вставляем нужный символ
                    symb.remove(' ');
                    symb.remove('_');
                    const QString ksymbol = (*kwdMap)[symb];
                    Q_ASSERT ( ksymbol.length() > 0 );
                    result += ksymbol;
                    // запоминаем позицию найденного вхождения...
                    P << cur;
                    // ... и его длину в ИСХОДНОЙ строке
                    L << rxCompound->matchedLength();
                }
            }

            // в завершении всего, запоминаем позицию, откуда искать на следующей итерации цикла

            // запоминаем предыдущую позицию, как указывающую на ПОСЛЕДНИЙ символ текущего
            // найденного вхождения
            prev = cur + (rxCompound->matchedLength()-1);
        }


        else {
            // здесь обрабавывается остаток строки, когда составные символы уже не находятся

            // проверка на литеральную константу
            if ( inLit ) {
                // добавляем остаток строки со сдвигом кодов символов
                for ( int j=prev+1; j<src.length(); j++ ) {
                    result += QString(QChar(src[j].unicode()+STR_HIDE_OFFSET));
                    P << j;
                    L << 1;
                }

                // поскольку литеральная константа в конце -- это явная ошибка
                // (закрывающая кавычка уже точно не найдётся), то нужно отругаться

                // ищем с конца открывающую кавычку и отмечаем её ошибкой
                // "Непарная кавычка"
                err_start = 0;
                err_len = 1;
                error = BAD_KAVICHKA;
                for ( int j=src.length()-1; j>=0; j-- ) {
                    if ( src[j] == litSymb ) {
                        err_start = j;
                        break;
                    }
                }
            }
            else {

                // остаток строки ВНЕ литеральной константы

                // добавляем "как есть" в нормализованную строку
                // (с нормализацией пробелов)
                for ( int j=prev+1; j<src.length(); j++ ) {
                    if ( src[j] == ' ' || src[j] == '\t') {
                        if ( !skipSpaces ) {
                            QString sp = QString(src[j]);
                            if ( sp == "\t" )
                                sp = " ";
                            result += QString(src[j]);
                            P << j;
                            L << 1;
                        }
                        skipSpaces = true;
                    }
                    else {
                        result += QString(src[j]);
                        P << j;
                        L << 1;
                        if ( src[j]==';' )
                            skipSpaces = true;
                        else
                            skipSpaces = false;
                    }
                }
            }

            // поскольку больше искать нечего, прерываем основной цикл
            break;
        }
        skipSpaces = false;
    }



    // зачистка от начальных и конечных пробелов
    while ( result.startsWith(" ") || result.startsWith("\t") ) {
        result.remove(0,1);
        P.pop_front();
        L.pop_front();
    }

    while ( result.endsWith(" ") || result.endsWith("\t") ) {
        result.chop(1);
        P.pop_back();
        L.pop_back();
    }

    // возвращаем результат
    return result;
}



/**
 * Определение типа строки
 * @param string Строка
 * @return Тип
 */
Kumir::InstructionType TextNormalizer::getStringType ( const QString &string )
{
    QChar first_b=string[0];
    if ( first_b==KS_ALG )
        return Kumir::AlgorhitmDeclaration;
    else if ( first_b==KS_BEGIN )
        return Kumir::AlgorhitmBegin;
    else if ( first_b==KS_END )
        return Kumir::AlgorhitmEnd;
    else if ( first_b==KS_INT )
        return Kumir::VariableDeclaration;
    else if ( first_b==KS_INT_TAB )
        return Kumir::VariableDeclaration;
    else if ( first_b==KS_STRING )
        return Kumir::VariableDeclaration;
    else if ( first_b==KS_STRING_TAB )
        return Kumir::VariableDeclaration;
    else if ( first_b==KS_CHAR )
        return Kumir::VariableDeclaration;
    else if ( first_b==KS_CHAR_TAB )
        return Kumir::VariableDeclaration;
    else if ( first_b==KS_BOOL_TAB )
        return Kumir::VariableDeclaration;
    else if ( first_b==KS_BOOL )
        return Kumir::VariableDeclaration;
    else if ( first_b==KS_REAL )
        return Kumir::VariableDeclaration;
    else if ( first_b==KS_REAL_TAB )
        return Kumir::VariableDeclaration;
    else if ( first_b==KS_LOOP )
        return Kumir::LoopBegin;
    else if ( first_b==KS_ENDLOOP )
        return Kumir::LoopEnd;
    else if ( first_b==KS_IF )
        return Kumir::If;
    else if ( first_b==KS_SWITCH )
        return Kumir::Switch;
    else if ( first_b==KS_CASE )
        return Kumir::Case;
    else if ( first_b==KS_THEN )
        return Kumir::Then;
    else if ( first_b==KS_DONE )
        return Kumir::EndSwitchOrIf;
    else if ( first_b==KS_OUTPUT )
        return Kumir::Output;
    else if ( first_b==KS_FOUTPUT )
        return Kumir::FileOutput;
    else if ( first_b==KS_FINPUT )
        return Kumir::FileInput;
    else if ( first_b==KS_ELSE )
        return Kumir::Else;
    else if ( first_b==KS_ASSERT )
        return Kumir::Assert;
    else if ( first_b==KS_PRE )
        return Kumir::Pre;
    else if ( first_b==KS_POST )
        return Kumir::Post;
    else if ( first_b==KS_DOC )
        return Kumir::Documentation;
    else if ( first_b==KS_INPUT )
        return Kumir::Input;
    else if ( first_b==KS_COND_ENDLOOP )
        return Kumir::ConditionalEndLoop;
    else if ( first_b==KS_EXIT )
        return Kumir::Exit;
    else if ( first_b==KS_MODULE )
        return Kumir::ModuleDeclaration;
    else if ( first_b==KS_ENDMODULE )
        return Kumir::ModuleEnd;
    else if ( first_b==KS_IMPORT )
        return Kumir::Import;
    else if ( string.contains(KS_ASSIGN, Qt::CaseSensitive) )
        return Kumir::Assignment;

    else if ( string.startsWith ("|") )
        return Kumir::Comment;
    else if ( string.isEmpty() )
        return Kumir::Empty;
    else
        return Kumir::AlgorhitmCall;
}


int findMinSkobka ( QString stroka )
{
    int test=stroka.indexOf ( '(' );
    if ( test<0 ) return -1;
    int skobk_count=0,skobk_pos,min_skobk=-1;
    skobk_count=0;
    for ( int i=0;i<=stroka.length();i++ )
    {
        if ( stroka[i]==QChar ( '(' ) ) skobk_count++;
        if ( stroka[i]==QChar ( ')' ) ) skobk_count--;
        if ( min_skobk<skobk_count ) {min_skobk=skobk_count;skobk_pos=i;};
    };

    return skobk_pos;

};





int TextNormalizer::test_skobk ( QString stroka )
{
    int count=0;
    bool kavichka=false;
    for ( int i=0;i<=stroka.length();i++ )
    {
        if ( stroka[i]==QChar ( '\"' ) ) kavichka= !kavichka;
        if ( ( stroka[i]==QChar ( '(' ) ) && ( !kavichka ) ) count++;
        if ( ( stroka[i]==QChar ( ')' ) ) && ( !kavichka ) ) count--;
        if ( count<0 ) return -1;
    }
    if ( !kavichka ) return count;else return -1;

}

int TextNormalizer::test_name ( QString name, int & pos, int & len )
{

    pos = 0;
    len = 0;
    if ( name.isEmpty() )
        return 0;

    /*
        TN_BAD_NAME_1 "Плохой символ в имени"
        TN_BAD_NAME_1A "Плохой символ в имени"
        TN_BAD_NAME_2 "Имя не может нач. с цифры"
        TN_BAD_NAME_3 "Ключевое слово в имени"
        TN_BAD_NAME_4 "Непарная \""
        */

    QString pattern = QString::fromUtf8("[+\\-=:*&?/><#%()\\^$.,");
    if (!m_allowHiddenNames)
        pattern += "!";
    pattern += QString::fromUtf8("|№\\[\\]{}~`\\\\]");

    QRegExp rxSym = QRegExp (pattern);

    Q_ASSERT ( rxSym.isValid() );

    int st = 0;
    while  ( ( st < name.length() )&& ( name[st]==' ' || name[st] == '\t' ) ) {
        st ++ ;
    }

    if ( name[st].isDigit() )
    {
        pos = st;
        len = 0;
        for ( int i=st; i<name.length(); i++ ) {
            if ( name[i].isDigit() )
                len++;
            else
                break;
        }
        return TN_BAD_NAME_2;
    }

    if ( name.count ( "\"" ) % 2 )
    {
        pos = name.indexOf ( "\"" );
        len = 1;
        return TN_BAD_NAME_4;
    }

    QRegExp rxKwd = QRegExp(trUtf8("\\bзнач\\b|\\bтаб\\b"));

    int ps; // позиция первого найденного неправильного символа
    int pks; // позиция первого найденного ксимвола

    ps = rxSym.indexIn ( name );
    QString found = rxSym.cap();
    QRegExp KS = QRegExp ( "[\\x3E8-\\x3EF]+|[\\x1100-\\x1200]+" );
    pks = KS.indexIn ( name );

    int error = 0;

    if ( ps != -1 )
    {
        error = TN_BAD_NAME_1;
        pos = ps;
        len = rxSym.matchedLength();
    }

    if ( pks != -1 )
    {
        QChar debug = name[pks];
        ushort debug_code = debug.unicode();
        Q_UNUSED(debug_code);
        if ( error > 0 )
        {
            if ( pks < ps )
            {
                error = TN_BAD_NAME_3;
                pos = pks;
                len = KS.matchedLength();
            }
        }
        else
        {
            error = TN_BAD_NAME_3;
            pos = pks;
            len = KS.matchedLength();
        }
    }


    pks = rxKwd.indexIn(name);

    if ( pks != -1 )
    {
        QChar debug = name[pks];
        ushort debug_code = debug.unicode();
        Q_UNUSED(debug_code);
        if ( error > 0 )
        {
            if ( pks < ps )
            {
                error = TN_BAD_NAME_3;
                pos = pks;
                len = rxKwd.matchedLength();
            }
        }
        else
        {
            error = TN_BAD_NAME_3;
            pos = pks;
            len = rxKwd.matchedLength();
        }
    }


    if ( error == TN_BAD_NAME_3 )
    {
        uint code = KS.cap() [0].unicode();
        if ( code >= 4503 && code <= 4508 )
        {
            // Нормализованный обычный символ
            error = TN_BAD_NAME_1A;
        }
    }

    return error;

    if ( name.contains ( "\"" ) && !name.startsWith ( "\"" ) && ( name.count ( "\"" ) % 2 ) ==0 )
    {
        pos = name.indexOf ( "\"" );
        len = 1;
        return TN_BAD_NAME_5;
    }
    return error;
}

QString TextNormalizer::SplitPrisvBlock( const QStringList &p_List,
                                         QStringList &p_Result,
                                         int p_Num = -1, bool p_FuncParam = FALSE )
{
    if ( p_Num == -1 )
    {
        p_Result.clear();
    }
    QString l_MainString = p_Num == -1 ? p_List.last() : p_List[p_Num];
    l_MainString = l_MainString.simplified();
    if ( l_MainString[l_MainString.length()-1] == QChar ( 0 ) )
    {
        l_MainString = l_MainString.left ( l_MainString.length()-1 );
    }
    QString l_NotFunctSyms = "+-*/><&1234567890^()=\"\',";
    for ( int i = 1000; i < 1007; i++ )
    {
        l_NotFunctSyms += QChar ( i );
    }
    l_NotFunctSyms += QChar ( KS_ASSIGN );

    QString l_NewMainString = "";
    if ( ( l_MainString.indexOf ( ',' ) == -1 ) && ( l_MainString.indexOf ( '&' ) == -1 ) && p_FuncParam )
    {
        p_Result.append ( l_MainString );
        l_NewMainString += '&' + QString::number ( p_Result.count()-1 );
        return l_NewMainString;
    }
    bool l_IsFunction = false;
    for ( int i = 0 ; i < l_MainString.length(); i++ )
    {
        if ( l_MainString[i] == '&' )
        {
            i++;
            QString l_NumString = "";
            while ( l_MainString[i].isNumber() && i < l_MainString.length() )
            {
                l_NumString += l_MainString[i++];
            }
            i--;
            QString l_StringText = SplitPrisvBlock ( p_List, p_Result, l_NumString.toInt(), l_IsFunction );
            if ( l_IsFunction )
            {
                l_NewMainString += l_StringText;
            }
            else if ( p_FuncParam )
            {
                if ( l_StringText.contains ( '&' ) != 1 )
                {
                    p_Result.append ( l_StringText );
                    l_NewMainString += '&' + QString::number ( p_Result.count()-1 );
                }
                else
                {
                    l_NewMainString += l_StringText;
                }
            }
            else
            {
                l_NewMainString += '(' + l_StringText + ')';
            }
        }
        else
        {
            if ( !p_FuncParam || l_MainString[i] != ',' )
            {
                l_NewMainString += l_MainString[i];
            }
        }

        if ( l_MainString[i] == '\"' || l_MainString[i] == '\'' )
        {
            QChar l_QuoteChar = l_MainString[i];
            for ( ++i; l_MainString[i] != l_QuoteChar && i < l_MainString.length(); i++ )
            {
                l_NewMainString += l_MainString[i];
            }
            l_NewMainString += l_QuoteChar;
        }
        else
        {
            l_IsFunction = l_NotFunctSyms.indexOf ( l_MainString[i] ) == -1;
            if ( l_IsFunction )
            {
                for ( ++i; l_MainString[i] != '&' && i < l_MainString.length(); i++ )
                {
                    l_NewMainString += l_MainString[i];
                }
                i--;
            }
        }
    }
    if ( p_FuncParam && l_MainString.indexOf ( ',' ) == -1 )
    {
        p_Result.append ( l_NewMainString );
        return '&' + QString::number ( p_Result.count()-1 );
    }
    if ( p_Num == -1 )
    {
        p_Result.append ( l_NewMainString );
    }

    return l_NewMainString;

}

QStringList TextNormalizer::splitElse ( const QString &normal_else_string )
{
    QStringList temp_List;
    temp_List.clear();
    if ( normal_else_string.length() >1 )
    {
        temp_List.append ( QString ( normal_else_string[0] ) );
        int DONE_pos=normal_else_string.indexOf ( QChar ( KS_DONE ) );
        if ( DONE_pos>-1 )
        {
            temp_List.append ( normal_else_string.mid ( 2,DONE_pos-2 ) );
            //                  outPutWindow->append("Hvost:"+normal_else_string.mid(1,DONE_pos-1));
            temp_List.append ( QString ( QChar ( KS_DONE ) ) );
            return temp_List;
        }
        else
            temp_List.append ( normal_else_string.mid ( 2,9999 ) );
        return temp_List;
    }
    else
    {
        temp_List.append ( normal_else_string );
        return temp_List;
    };

    return temp_List;

}


QStringList TextNormalizer::splitTo ( const QString &normal_to_string )
{
    QStringList temp_List;
    temp_List.clear();
    QString normal_else_string;
    if ( normal_to_string.length() >1 )
    {
        temp_List.append ( QString ( normal_to_string[0] ) );
        int else_pos=normal_to_string.indexOf ( QChar ( KS_ELSE ) );
        //int DONE_pos=normal_to_string.indexOf(QChar(KS_TONE));
        if ( else_pos>-1 )
        {
            temp_List.append ( normal_to_string.mid ( 2,else_pos-2 ) );
            normal_else_string=normal_to_string.mid ( else_pos,9999 );
            //                  outPutWindow->append("ElseString:"+normal_else_string);
            temp_List+=splitElse ( normal_else_string );
            return temp_List;
        }

        int DONE_pos=normal_to_string.indexOf ( QChar ( KS_DONE ) );
        if ( DONE_pos>-1 )
        {
            temp_List << normal_to_string.mid ( 2,DONE_pos-2 ).split ( ";",QString::SkipEmptyParts );
            return temp_List;
        }
        else
            temp_List.append ( normal_to_string.mid ( 2,9999 ) );
        return temp_List;



    }
    else
    {
        temp_List.append ( normal_to_string );
        return temp_List;
    };

    return temp_List;

}


//int TextNormalizer::splitUserStr ( QString stroka,QList<NormalizedTextInstruction>* result,int str )
//{
//    int err=0;
//    int err_start = 0;
//    int err_end = 0;
//    NormalizedTextInstruction temp_proga;
//    QString norma_string=normalizeString ( stroka,&err,&err_start,&err_end );
//    int e1, e2, e3;
//    QList<int> P;
//    QList<int> L;
//    QString n2 = normalize(stroka, P, L, e1, e2, e3);
//    Q_ASSERT( e1>=0 );
//    Q_ASSERT( e2>=0 );
//    Q_ASSERT( e3>=0 );
//    Q_ASSERT ( norma_string == n2 );
//    if ( err>0 ) return err;
//    temp_proga.stroka=str;
//    temp_proga.x_offset=0;
//    int RealLineType=getStringType ( norma_string );
//    QStringList List;
//    List.clear();
//    switch ( RealLineType )
//    {

//    case A_IMPL:
//        temp_proga.text=norma_string.trimmed();
//        result->append ( temp_proga );
//        return 0;
//        break;

//    case A_OTKR:
//        {
//            int ns_len = norma_string.length();
//            if ( ns_len>1 )
//            {
//                List.append ( QString ( QChar ( KS_BEGIN ) ) );
//                QStringList tmp_lst = norma_string.mid ( 2 ).split ( ";",QString::SkipEmptyParts );
//                for ( int i=0;i<tmp_lst.count();i++ ) List.append ( tmp_lst[i].trimmed() );
//            }
//            else
//            {
//                temp_proga.text=norma_string;
//                result->append ( temp_proga );
//                return 0;
//            };
//        }
//        break;


//    case O_OTKR:
//        {
//            int TIMES_pos=norma_string.indexOf ( QChar ( KS_TIMES ) );
//            int ns_len = norma_string.length();
//            if ( TIMES_pos<ns_len-1 )
//            {
//                List.append ( norma_string.mid ( 0,TIMES_pos+1 ) );
//                QString comm_line=norma_string.mid ( TIMES_pos+1,9999 );
//                QString append_string="";
//                if ( norma_string[norma_string.length()-1]==QChar ( KS_ENDLOOP ) )
//                {
//                    comm_line=comm_line.mid ( 0,comm_line.length()-1 );
//                    append_string=QString ( QChar ( KS_ENDLOOP ) );
//                };

//                List += comm_line.split ( ";",QString::SkipEmptyParts );
//                if ( !append_string.isEmpty() ) List.append ( append_string );
//            }
//            else
//                List.append ( norma_string.trimmed() );
//        }
//        break; //exit

//    case O_ZAKR:

//        temp_proga.text=norma_string;
//        result->append ( temp_proga );
//        return 0;

//        break;

//    case ELSE:

//        List+=splitElse ( norma_string );

//        break; //else



//    case EXIT:
//        {
//            temp_proga.text=norma_string;
//            result->append ( temp_proga );
//            return 0;
//            break; //exit
//        }

//    case IF_BEGINS:
//        {
//            int to_pos=norma_string.indexOf ( QChar ( KS_THEN ) );
//            if ( to_pos>-1 )
//            {
//                List.append ( norma_string.mid ( 0,to_pos ) );
//                QString to_line=norma_string.mid ( to_pos,9999 );
//                //        outPutWindow->append("toLine:"+to_line);
//                List+=splitTo ( to_line );
//            }
//            else
//                List.append ( norma_string );
//        }

//        break;

//        /*    case SWITCH_BEGINS:
//                        if(!build)
//                        out=parce_SWITCH(instr,table,symbols,proga_str,&err_start,&err_end);
//                        else
//                        out=0;
//                        break; */

//    case CASE_BEGINS:
//        {
//            int dvoet_pos=norma_string.indexOf ( ':' );
//            int ns_len = norma_string.length();
//            if ( dvoet_pos<ns_len-1 )
//            {
//                List.append ( norma_string.mid ( 0,dvoet_pos+1 ) );
//                QString comm_line=norma_string.mid ( dvoet_pos+1,9999 );
//                comm_line=comm_line.trimmed();
//                List << comm_line.split ( ";",QString::SkipEmptyParts );
//            }
//            else
//                List.append ( norma_string.trimmed() );
//        }
//        break;

//    case TO_OTKR:
//        {
//            List+=splitTo ( norma_string );
//        }
//        break;

//    case IF_ZAKR:

//        temp_proga.text=norma_string;
//        result->append ( temp_proga );
//        return 0;
//        break;
//        /*
//                                case UTV:
//                                        if(!build)
//                                        out=parce_utv(instr,table,symbols,proga_str,&err_start,&err_end);
//                                        else
//                                        out=0;
//                                        break;
//                                case KC_CASE:
//                                        if(!build)
//                                        out=parce_kc_CASE(instr,table,symbols,proga_str,&err_start,&err_end);
//                                        else
//                                        out=0;
//                                        break;

//                                case DANO:
//                                        if(!build)
//                                        out=parce_dano(instr,table,symbols,proga_str,&err_start,&err_end);
//                                        else
//                                        out=0;
//                                        break;
//                                case NADO:
//                                        if(!build)
//                                        out=parce_nado(instr,table,symbols,proga_str,&err_start,&err_end);
//                                        else
//                                        out=0;
//                                        break;

//                                case OUTPUT:
//                                        if(!build)
//                                        out=parce_OUTPUT(instr,table,symbols,proga_str,&err_start,&err_end);
//                                        else
//                                        out=0;
//                                        break;

//                                case COMMENT:
//                                        out=0;
//                                        Str_Data.line=instr;
//                                        break; */


//    default:
//        List = norma_string.split ( ":",QString::SkipEmptyParts );
//    }
//    //switch




//    for ( int i=0;i<List.count();i++ )
//    {

//        temp_proga.text=List[i];
//        if ( i>0 )
//        {
//            //int zdvig= FindNomerNorm(List[i-1],List[i-1].length()-1,1,&null);
//            int zdvig;
//            zdvig = List[i-1].count ( QRegExp ( Breaks ) );
//            if ( zdvig<0 ) zdvig=0;
//            temp_proga.x_offset=temp_proga.x_offset+zdvig; //считаем в ксимволах
//            //       temp_proga.x_offset=temp_proga.x_offset+1;
//        }

//        result->append ( temp_proga );
//    }
//    if ( result->count() > 1 )
//    {
//        for ( int i=0;i<result->count()-1;i++ )
//        {
//            int debug = result->count();
//            int y_offset = ( *result ) [i+1].x_offset;
//            ( *result ) [i].y_offset = y_offset;
//            Q_UNUSED(debug);
//        }
//    }
//    if ( List.count() > 1 )
//        return RealLineType;
//    else
//        return EMPTY;
//}

//int TextNormalizer::splitUserIsp ( QStringList* stroki,QList<NormalizedTextInstruction>* result,QString file )
//{
//    int error=0;
//    int err_start = 0;
//    int err_end = 0;

//    for ( int i=0;i<stroki->count();i++ )
//    {
//        QString stroka=stroki->at ( i );
//        QString norma_string=normalizeString ( stroka,&error,&err_start,&err_end ); //Нормализация строки
//        QStringList FStroki = norma_string.split ( ";",QString::KeepEmptyParts );
//        QList< QList<int> > PP, LL;
//        error=splitFStringList ( FStroki,result,i,0,0,file,PP,LL ); //Разбиение строки.

//        //  if(error>0)return error;
//    };
//    int debug=result->count();
//    Q_UNUSED(debug);
//    return 0;
//}

void TextNormalizer::trim(QString &s, QList<int> &P, QList<int> &L )
{
    while ( s.startsWith(" ") ) {
        s.remove(0,1);
        P.pop_front();
        L.pop_front();
    }

    while ( s.endsWith(" ") ) {
        s.chop(1);
        P.pop_back();
        L.pop_back();
    }
}

///**
// * Разбиение списка Ф-строк. Вызывается рекурсивно.
// * @param FSpisok Список ф-строк
// * @param result Результат разбиения не отчищается, а добавляется.
// * @param realLine Номер строки в реальном редакторе
// * @return
// */
//int TextNormalizer::splitFStringList ( QStringList FSpisok,QList<NormalizedTextInstruction>* result,int realLine,int Last_F_ID,int  offset,QString file, const QList<QList<int> > &PPs, const QList<QList<int> > &LLs )
//{
//    int curLine=Last_F_ID;

//    QList< QList<int> > PP = PPs;
//    QList< QList<int> > LL = LLs;

//    while ( ( FSpisok[curLine] ).isEmpty() )
//    {
//        if ( curLine== ( FSpisok.count()-1 ) )
//            return 0;
//        curLine++;
//    }

//    int RealLineType=getStringType ( FSpisok[curLine] );

//    NormalizedTextInstruction temp_proga;//Временные результаты разбора.
//    temp_proga.stroka=realLine;
//    temp_proga.x_offset=offset;
//    temp_proga.errorCode = 0;
//    temp_proga.filename=file;
//    QString tmp;
//    int to_pos,CASE_pos,TIMES_pos;
//    int zdvig;
//    switch ( RealLineType )
//    {
//        //Неразделяемые строки
//    case A_IMPL:
//        temp_proga.text=FSpisok[curLine];
//        result->append ( temp_proga );
//        result->last().P = PP[curLine];
//        result->last().L = LL[curLine];
//        trim(result->last().text,result->last().P,result->last().L);

//        zdvig = FSpisok[curLine].count ( brks );
//        temp_proga.x_offset=temp_proga.x_offset+zdvig+1;
//        FSpisok[curLine]="";
//        PP[curLine].clear();
//        LL[curLine].clear();
//        break;



//    case DANO:
//        temp_proga.text=FSpisok[curLine];
//        result->append ( temp_proga );
//        result->last().P = PP[curLine];
//        result->last().L = LL[curLine];
//        trim(result->last().text,result->last().P,result->last().L);

//        zdvig = FSpisok[curLine].count ( brks );
//        temp_proga.x_offset=temp_proga.x_offset+zdvig+1;

//        FSpisok[curLine]="";
//        PP[curLine].clear();
//        LL[curLine].clear();
//        break;



//    case NADO:
//        temp_proga.text=FSpisok[curLine];
//        result->append ( temp_proga );
//        result->last().P = PP[curLine];
//        result->last().L = LL[curLine];
//        trim(result->last().text,result->last().P,result->last().L);

//        zdvig = FSpisok[curLine].count ( brks );
//        temp_proga.x_offset=temp_proga.x_offset+zdvig+1;

//        FSpisok[curLine]="";
//        PP[curLine].clear();
//        LL[curLine].clear();
//        break;


//    case ISP:
//        temp_proga.text=FSpisok[curLine];
//        result->append ( temp_proga );
//        result->last().P = PP[curLine];
//        result->last().L = LL[curLine];
//        trim(result->last().text,result->last().P,result->last().L);

//        zdvig = FSpisok[curLine].count ( brks );
//        temp_proga.x_offset=temp_proga.x_offset+zdvig+1;

//        FSpisok[curLine]="";
//        PP[curLine].clear();
//        LL[curLine].clear();
//        break;



//    case ISP_END:
//        temp_proga.text=FSpisok[curLine];
//        result->append ( temp_proga );
//        result->last().P = PP[curLine];
//        result->last().L = LL[curLine];
//        trim(result->last().text,result->last().P,result->last().L);

//        zdvig = FSpisok[curLine].count ( brks );
//        temp_proga.x_offset=temp_proga.x_offset+zdvig+1;

//        FSpisok[curLine]="";
//        PP[curLine].clear();
//        LL[curLine].clear();
//        break;


//    case IF_ZAKR:
//        temp_proga.text=FSpisok[curLine];
//        result->append ( temp_proga );
//        result->last().P = PP[curLine];
//        result->last().L = LL[curLine];
//        trim(result->last().text,result->last().P,result->last().L);


//        zdvig = FSpisok[curLine].count ( brks );
//        temp_proga.x_offset=temp_proga.x_offset+zdvig+1;

//        FSpisok[curLine]="";
//        PP[curLine].clear();
//        LL[curLine].clear();
//        break;

//    case KC_CASE:
//        temp_proga.text=FSpisok[curLine];
//        result->append ( temp_proga );
//        result->last().P = PP[curLine];
//        result->last().L = LL[curLine];
//        trim(result->last().text,result->last().P,result->last().L);


//        zdvig = FSpisok[curLine].count ( brks );
//        temp_proga.x_offset=zdvig+1;

//        FSpisok[curLine]="";
//        PP[curLine].clear();
//        LL[curLine].clear();
//        break;

//    case O_ZAKR:
//        temp_proga.text=FSpisok[curLine];
//        result->append ( temp_proga );
//        result->last().P = PP[curLine];
//        result->last().L = LL[curLine];
//        trim(result->last().text,result->last().P,result->last().L);


//        zdvig = FSpisok[curLine].count ( brks );
//        temp_proga.x_offset=temp_proga.x_offset+zdvig+1;

//        FSpisok[curLine]="";
//        PP[curLine].clear();
//        LL[curLine].clear();
//        break;



//    case A_ZAKR:
//        temp_proga.text=FSpisok[curLine];
//        result->append ( temp_proga );
//        result->last().P = PP[curLine];
//        result->last().L = LL[curLine];
//        trim(result->last().text,result->last().P,result->last().L);

//        FSpisok[curLine]="";
//        PP[curLine].clear();
//        LL[curLine].clear();
//        break;


//        //НАЧ
//    case A_OTKR:
//        temp_proga.text=QString ( QChar ( KS_BEGIN ) );
//        result->append ( temp_proga );
//        result->last().P.clear();
//        result->last().P << PP[curLine].first();
//        result->last().L.clear();
//        result->last().L << LL[curLine].first();
//        PP[curLine].pop_front();
//        LL[curLine].pop_front();

//        tmp=FSpisok[curLine].mid ( 1,9999 );
//        temp_proga.x_offset++;
//        trim(tmp,PP[curLine],LL[curLine]);
//        FSpisok[curLine]=tmp;
//        break;

//        //при
//    case CASE_BEGINS:
//        CASE_pos=FSpisok[curLine].indexOf ( ":" );
//        if ( CASE_pos>-1 )
//        {
//            temp_proga.text=FSpisok[curLine].mid ( 0,CASE_pos+1 );
//            result->append ( temp_proga );
//            result->last().P.clear();
//            result->last().L.clear();
//            for ( int j=0; j<result->last().text.length(); j++ ) {
//                result->last().P << PP[curLine].first();
//                result->last().L << LL[curLine].first();
//                PP[curLine].pop_front();
//                LL[curLine].pop_front();
//            }

//            //         zdvig = FSpisok[curLine].count(brks);
//            zdvig = temp_proga.text.count ( brks );
//            temp_proga.x_offset=temp_proga.x_offset+zdvig;
//            QString CASE_line=FSpisok[curLine].mid ( CASE_pos+1,9999 );
//            trim(CASE_line,PP[curLine],LL[curLine]);
//            FSpisok[curLine]=CASE_line;
//        }
//        else
//        {
//            temp_proga.text=FSpisok[curLine];
//            result->append ( temp_proga );
//            result->last().P = PP[curLine];
//            result->last().L = LL[curLine];
//            trim(result->last().text,result->last().P,result->last().L);

//            FSpisok[curLine]="";
//            PP[curLine].clear();
//            LL[curLine].clear();
//        };
//        break;


//                case TO_OTKR:
//        temp_proga.text=QString ( QChar ( KS_THEN ) );
//        result->append ( temp_proga );
//        result->last().P.clear();
//        result->last().P << PP[curLine].first();
//        PP[curLine].pop_front();
//        result->last().L.clear();
//        result->last().L << LL[curLine].first();
//        LL[curLine].pop_front();

//        temp_proga.x_offset++;
//        tmp=FSpisok[curLine].mid ( 1,9999 );
//        trim(tmp,PP[curLine],LL[curLine]);
//        FSpisok[curLine]=tmp;
//        break;

//                case ELSE:
//        temp_proga.text=QString ( QChar ( KS_ELSE ) );
//        result->append ( temp_proga );
//        result->last().P.clear();
//        result->last().P << PP[curLine].first();
//        PP[curLine].pop_front();
//        result->last().L.clear();
//        result->last().L << LL[curLine].first();
//        LL[curLine].pop_front();

//        temp_proga.x_offset++;
//        tmp=FSpisok[curLine].mid ( 1,9999 );
//        trim(tmp,PP[curLine],LL[curLine]);
//        FSpisok[curLine]=tmp;
//        break;


//                case SWITCH_BEGINS:
//        temp_proga.text=QString ( QChar ( KS_SWITCH ) );
//        result->append ( temp_proga );
//        result->last().P.clear();
//        result->last().P << PP[curLine].first();
//        PP[curLine].pop_front();
//        result->last().L.clear();
//        result->last().L << LL[curLine].first();
//        LL[curLine].pop_front();

//        temp_proga.x_offset++;
//        tmp=FSpisok[curLine].mid ( 1,9999 );
//        trim(tmp,PP[curLine],LL[curLine]);
//        FSpisok[curLine]=tmp;
//        break;

//                case IF_BEGINS:
//        to_pos=FSpisok[curLine].indexOf ( QChar ( KS_THEN ) ); //Ищем то
//        if ( to_pos>-1 )
//        {
//            temp_proga.text=FSpisok[curLine].mid ( 0,to_pos );
//            result->append ( temp_proga );
//            result->last().P.clear();
//            result->last().L.clear();
//            for ( int j=0; j<result->last().text.length(); j++ ) {
//                result->last().P << PP[curLine].first();
//                result->last().L << LL[curLine].first();
//                PP[curLine].pop_front();
//                LL[curLine].pop_front();
//            }


//            zdvig = temp_proga.text.count ( brks );
//            temp_proga.x_offset=temp_proga.x_offset+zdvig;

//            temp_proga.text=QString ( QChar ( KS_THEN ) );
//            result->append ( temp_proga );
//            result->last().P.clear();
//            result->last().L.clear();
//            result->last().P << PP[curLine].first();
//            result->last().L << LL[curLine].first();
//            PP[curLine].pop_front();
//            LL[curLine].pop_front();

//            temp_proga.x_offset++;
//            QString to_line=FSpisok[curLine].mid ( to_pos+1,9999 );
//            trim(to_line,PP[curLine],LL[curLine]);
//            FSpisok[curLine]=to_line;
//        }
//        else
//        {
//            temp_proga.text=FSpisok[curLine];
//            result->append ( temp_proga );
//            result->last().P = PP[curLine];
//            result->last().L = LL[curLine];
//            trim(result->last().text,result->last().P,result->last().L);


//            zdvig = temp_proga.text.count ( brks );
//            temp_proga.x_offset=temp_proga.x_offset+zdvig+1;

//            FSpisok[curLine]="";
//            PP[curLine].clear();
//            LL[curLine].clear();
//        };
//        break;

//                case O_OTKR:
//        TIMES_pos=FSpisok[curLine].indexOf ( QChar ( KS_TIMES ) ); //Ищем раз
//        if ( TIMES_pos>-1 )
//        {
//            temp_proga.text=FSpisok[curLine].mid ( 0,TIMES_pos+1 );
//            result->append ( temp_proga );
//            for ( int j=0; j<result->last().text.length(); j++ ) {
//                result->last().P << PP[curLine].first();
//                result->last().L << LL[curLine].first();
//                PP[curLine].pop_front();
//                LL[curLine].pop_front();
//            }


//            zdvig = temp_proga.text.count ( brks );
//            temp_proga.x_offset=temp_proga.x_offset+zdvig;

//            QString to_line=FSpisok[curLine].mid ( TIMES_pos+1,9999 );
//            trim(to_line,PP[curLine],LL[curLine]);
//            FSpisok[curLine]=to_line;
//        }
//        else
//        {
//            if ( ( FSpisok[curLine].indexOf ( QChar ( KS_FOR ) ) >-1 ) || ( FSpisok[curLine].indexOf ( QChar ( KS_WHILE ) ) >-1 ) )
//            {
//                temp_proga.text=FSpisok[curLine];
//                result->append ( temp_proga );
//                result->last().P = PP[curLine];
//                result->last().L = LL[curLine];
//                trim(result->last().text,result->last().P,result->last().L);

//                zdvig = temp_proga.text.count ( brks );
//                temp_proga.x_offset=zdvig+1;
//                FSpisok[curLine]="";
//                PP[curLine].clear();
//                LL[curLine].clear();
//            }
//            else
//            {
//                temp_proga.text=QString ( QChar ( KS_LOOP ) );
//                result->append ( temp_proga );
//                result->last().P.clear();
//                result->last().L.clear();
//                result->last().P << PP[curLine].first();
//                result->last().L << LL[curLine].first();
//                PP[curLine].pop_front();
//                LL[curLine].pop_front();

//                temp_proga.x_offset++;
//                tmp=FSpisok[curLine].mid ( 1,9999 );
//                trim(tmp,PP[curLine],LL[curLine]);
//                FSpisok[curLine]=tmp;
//            };
//        };
//        break;




//                default: //Простая строка или ошибка.
//                    tmp=FSpisok[curLine];
//                    bool flag=FALSE;  //Разделители
//                    for ( int i=0;i<tmp.length();i++ )
//                    {
//                        if ( flag ) break;//Что-то уже нашли
//                        switch ( tmp[i].unicode() )
//                        {
//                        case KS_ELSE:
//                            flag=TRUE;
//                            temp_proga.text=FSpisok[curLine].mid ( 0,i );
//                            result->append ( temp_proga );
//                            result->last().P.clear();
//                            result->last().L.clear();
//                            for ( int j=0; j<result->last().text.length(); j++ ) {
//                                result->last().P << PP[curLine].first();
//                                result->last().L << LL[curLine].first();
//                                PP[curLine].pop_front();
//                                LL[curLine].pop_front();
//                            }

//                            zdvig = temp_proga.text.count ( brks );
//                            temp_proga.x_offset=temp_proga.x_offset+zdvig;
//                            FSpisok[curLine]=FSpisok[curLine].mid ( i,9999 );
//                            break;

//                                        case KS_TONE:
//                            flag=TRUE;
//                            temp_proga.text=FSpisok[curLine].mid ( 0,i );
//                            result->append ( temp_proga );
//                            result->last().P.clear();
//                            result->last().L.clear();
//                            for ( int j=0; j<result->last().text.length(); j++ ) {
//                                result->last().P << PP[curLine].first();
//                                result->last().L << LL[curLine].first();
//                                PP[curLine].pop_front();
//                                LL[curLine].pop_front();
//                            }


//                            zdvig = temp_proga.text.count ( brks );
//                            temp_proga.x_offset=temp_proga.x_offset+zdvig;


//                            FSpisok[curLine]=FSpisok[curLine].mid ( i,9999 );
//                            break;

//                                        case KS_ENDLOOP:
//                            flag=TRUE;
//                            temp_proga.text=FSpisok[curLine].mid ( 0,i );
//                            result->append ( temp_proga );
//                            result->last().P.clear();
//                            result->last().L.clear();
//                            for ( int j=0; j<result->last().text.length(); j++ ) {
//                                result->last().P << PP[curLine].first();
//                                result->last().L << LL[curLine].first();
//                                PP[curLine].pop_front();
//                                LL[curLine].pop_front();
//                            }

//                            zdvig = temp_proga.text.count ( brks );
//                            temp_proga.x_offset=temp_proga.x_offset+zdvig;


//                            FSpisok[curLine]=FSpisok[curLine].mid ( i,9999 );
//                            break;

//                                        case KS_CASE:
//                            flag=TRUE;
//                            temp_proga.text=FSpisok[curLine].mid ( 0,i );
//                            result->append ( temp_proga );
//                            result->last().P.clear();
//                            result->last().L.clear();
//                            for ( int j=0; j<result->last().text.length(); j++ ) {
//                                result->last().P << PP[curLine].first();
//                                result->last().L << LL[curLine].first();
//                                PP[curLine].pop_front();
//                                LL[curLine].pop_front();
//                            }


//                            zdvig = temp_proga.text.count ( brks );
//                            temp_proga.x_offset=temp_proga.x_offset+zdvig;


//                            FSpisok[curLine]=FSpisok[curLine].mid ( i,9999 );
//                            break;

//                                        case KS_COND_ENDLOOP:
//                            flag=TRUE;
//                            temp_proga.text=FSpisok[curLine].mid ( 0,i );
//                            result->append ( temp_proga );
//                            result->last().P.clear();
//                            result->last().L.clear();
//                            for ( int j=0; j<result->last().text.length(); j++ ) {
//                                result->last().P << PP[curLine].first();
//                                result->last().L << LL[curLine].first();
//                                PP[curLine].pop_front();
//                                LL[curLine].pop_front();
//                            }


//                            zdvig = temp_proga.text.count ( brks );
//                            temp_proga.x_offset=temp_proga.x_offset+zdvig;

//                            FSpisok[curLine]=FSpisok[curLine].mid ( i,9999 );
//                            break;

//                        };
//                    };
//                    if ( !flag )
//                    {
//                        temp_proga.text=FSpisok[curLine];
//                        result->append ( temp_proga );
//                        result->last().P = PP[curLine];
//                        result->last().L = LL[curLine];
//                        zdvig = FSpisok[curLine].count ( brks );
//                        temp_proga.x_offset=temp_proga.x_offset+zdvig+1;
//                        FSpisok[curLine]="";
//                        PP[curLine].clear();
//                        LL[curLine].clear();
//                    };
//                    break;

//                };
//    int err=0;
//    int fCount = FSpisok.count();

//    // различные проверки, -- убрать или закомментировать после тестирования,
//    // т.к. сильно снижает скорость работы

//    for ( int i=0; i<result->count(); i++ ) {
//        int a,b,c;
//        a = result->at(i).text.length();
//        b = result->at(i).P.count();
//        c = result->at(i).L.count();
//        Q_ASSERT ( a == b );
//        Q_ASSERT ( b == c );
//    }

//    for ( int i=0; i<FSpisok.count(); i++ ) {
//        Q_ASSERT ( FSpisok[i].length() == PP[i].count() );
//        Q_ASSERT ( PP[i].count() == LL[i].count() );
//    }

//    if ( curLine<fCount )
//        err=splitFStringList ( FSpisok,result,realLine,curLine,temp_proga.x_offset,file, PP, LL );
//    return err;
//}

int TextNormalizer::splitUserFile ( QFile &file,
                                    QList< QList<NormalizedTextInstruction> > &result )
{
    QList<NormalizedTextInstruction> all;
    if ( file.open(QIODevice::ReadOnly|QIODevice::Text) ) {
        QTextStream ts ( &file );
        ts.setCodec("UTF-16LE");


        QStringList lines = ts.readAll().split("\n",QString::KeepEmptyParts);
        for ( int i=0; i<lines.count(); i++ ) {
            QPair<int,int> errorPosition;
            splitUserLine(lines[i], errorPosition, all);
        }
        for ( int i=0; i<all.count(); i++ ) {
            all[i].filename= QFileInfo(file).absoluteFilePath();
        }
        file.close();
        bool inIsp = false;
        bool ispFound = false;
        bool inInnerIsp = false;
        result.clear();
        QList<NormalizedTextInstruction> curModule;
        for ( int i=0; i<all.count(); i++ ) {
            if ( getStringType(all[i].text) == Kumir::ModuleDeclaration ) {
                if ( inIsp ) {
                    all[i].errorCode= TOS_ISP_IN_ISP;
                    all[i].errorStart = 0;
                    all[i].errorLength = all[i].text.length();
                    inInnerIsp = true;
                }
                else {
                    if ( !curModule.isEmpty() )
                        result.append(curModule);
                    curModule.clear();
                }
                inIsp = true;
                ispFound = true;
            }
            else if ( getStringType(all[i].text) == Kumir::ModuleEnd ) {
                inIsp = false;
                if ( inInnerIsp ) {
                    all[i].errorCode = TOS_ISP_IN_ISP;
                    all[i].errorStart = 0;
                    all[i].errorLength = all[i].text.length();
                    inInnerIsp = false;
                }
            }
            else {
                if ( ispFound && !inIsp ) {
                    all[i].errorCode = TOS_AFTER_ISP;
                    all[i].errorStart = 0;
                    all[i].errorLength = all[i].text.length();
                }
                if ( inInnerIsp ) {
                    all[i].errorCode = TOS_ISP_IN_ISP;
                    all[i].errorStart = 0;
                    all[i].errorLength = all[i].text.length();
                }
            }
            curModule << all[i];
        }
        if ( !curModule.isEmpty() )
            result.append(curModule);
        return 0;
    }
    else {
        return -1;
    }
}

QStringList splitStringList(
        const QString &src, // IN: исходная строка
        const QList<int> &P, // IN: вектор P
        const QList<int> &L, // IN: вектор L
        QList< QList<int> > &PP, // OUT: список векторов P
        QList< QList<int> > &LL // OUT: список векторов L
        )
{
    QStringList result;
    QString cur = "";
    QList<int> CP;
    QList<int> CL;
    PP.clear();
    LL.clear();
    for ( int i=0; i<src.length(); i++ ) {
        if ( src[i] == ';' ) {
            result << cur;
            cur = "";
            PP << CP;
            CP.clear();
            LL << CL;
            CL.clear();
        }
        else {
            cur += src[i];
            CP << P[i];
            CL << L[i];
        }
    }
    result << cur;
    PP << CP;
    LL << CL;
    return result;
}

int TextNormalizer::splitUserLine ( const QString &text,
                                    QPair<int,int> &errorPosition,
                                    QList<NormalizedTextInstruction> &result )
{
    int err=0;
    int err_start = -1;
    int err_end = -1;
    int other_err = 0;
    QString norma_string;
    QString n2;
    int e1, e2, e3;
    QList<int> P, L;
    n2 = normalize(text,P,L,e1,e2,e3);
    Q_ASSERT( e1>=0 );
    Q_ASSERT( e2>=0 );
    Q_ASSERT( e3>=0 );
    // 	qDebug() << "n2: -" << n2.toUtf8();
    // 	qDebug() << "P: --" << P;
    // 	qDebug() << "L: --" << L;
    Q_ASSERT ( (n2.length() == P.count()) && (P.count()== L.count()) );


    norma_string = n2;
    err = e1;
    err_start = e2;
    err_end = e2+e3-1;


    if ( err > 0 ) {
        errorPosition = QPair<int,int>(err_start,err_end);
    }
    QStringList FStroki;
    QList< QList<int> > PP, LL;

    FStroki=splitStringList(norma_string,P,L,PP,LL);

    other_err=splitFStringList ( FStroki,result,0,0,0,"\'USER", PP, LL ); //Разбиение строки.
    if ( other_err > 0 && err == 0 ) {
        err = other_err;
        errorPosition = QPair<int,int>(0,text.length()-1);
    }
    // 	int idbg = result->count();
    // 	qDebug() << "RESULT.count(): " << idbg;
    // 	for ( int i=0; i<idbg; i++ ) {
    // 		qDebug("RESULT[%i] = %s",i,(*result)[i].text.toUtf8().data());
    // 	}
    return err;
}

/**
 * Проверяет, находится ли позиция pos внутри литеральной константы
 * @param s исходная ненормализованная строка
 * @param pos позиция
 * @return true, если внутри литеральной константы или false, если нет
 */

bool TextNormalizer::inLit ( const QString &s, int pos )
{
    bool isDoubleQuote;
    bool inL = false;
    int cur_pos = -1;
    int len = s.length();
    int pos_DQ, pos_SQ;
    while ( cur_pos < len )
    {
        pos_DQ = s.indexOf ( "\"",cur_pos+1 );
        pos_SQ = s.indexOf ( "'",cur_pos+1 );
        if ( ( pos_DQ==-1 ) && ( pos_SQ==-1 ) )
        {
            return inL;
        }
        if (
                ( pos_DQ==-1 )
                ||
                ( ( pos_SQ<pos_DQ ) && ( pos_SQ!=-1 ) )
                ) // next is '
        {
            cur_pos = pos_SQ;
            if ( pos<cur_pos ) return inL;
            if ( !inL ) // begin lit
            {
                inL = true;
                isDoubleQuote = false;
            }
            else
            {
                if ( !isDoubleQuote )
                {
                    inL = false;
                }
            }
        }
        else // next is "
        {
            cur_pos = pos_DQ;
            if ( pos<cur_pos ) return inL;
            if ( !inL )
            {
                isDoubleQuote = true;
                inL = true;
            }
            else
            {
                if ( isDoubleQuote )
                    inL = false;
            }
        }
    }
    return false;
}


QVector<bool> TextNormalizer::recalculateErrorPositions ( const QString &text, const QList<NormalizedTextInstruction> &ptl )
{
    int resultSize = text.length();
    QVector<bool> result = QVector<bool>(resultSize,false);
    foreach ( NormalizedTextInstruction pt, ptl ) {
        if ( pt.errorCode > 0 ) {
            int pc = pt.P.count();
            int lc = pt.L.count();
            Q_UNUSED(pc);
            Q_UNUSED(lc);
            int tl = pt.text.length();
            Q_UNUSED(tl);
            Q_ASSERT ( pt.P.count() == pt.L.count() );
            Q_ASSERT ( pt.P.count() == pt.text.length() );
            for ( int i=pt.errorStart; i<pt.errorStart+pt.errorLength; i++ ) {
                int start = pt.P[i];
                int len = pt.L[i];
                for ( int j = start; j<start+len; j++ ) {
                    result[j] = true;
                }
            }
        }
    }
    int start = result.indexOf(true);
    if ( start != -1 ) {
        for ( int i=start; i<text.length(); i++ ) {
            if ( text[i]==' ' || text[i]=='\t' )
                result[i] = false;
            else
                break;
        }
    }
    int end = result.lastIndexOf(true);
    if ( end != -1 ) {
        for ( int i=end; i>=0; i-- ) {
            if ( text[i]==' ' || text[i]=='\t' )
                result[i] = false;
            else
                break;
        }
    }
    return result;
}



bool TextNormalizer::allowIndent(const int str_type, const int err_code )
{
    Q_UNUSED(str_type);
    if ( err_code == 0 )
        return true;
    else if ( (err_code >= 1101 && err_code <= 1199) || (err_code >= 11100 && err_code <= 11105 ) || (err_code >= 12001 && err_code <= 12030 ) )
        return false;
    else return true;
}

QPair<int,int> TextNormalizer::getIndentRank( const NormalizedTextInstruction &pt )
{
    QPair<int,int> result;
    Kumir::InstructionType st = TextNormalizer::getStringType(pt.text);
    if ( st == Kumir::AlgorhitmBegin ) // 13, нач
    {
        result = QPair<int,int>(0,1);
    }
    else if ( st == Kumir::AlgorhitmEnd ) // 14, кон
    {
        result = QPair<int,int>(-1,0);
    }
    else if ( st == Kumir::LoopBegin ) // 15, нц
    {
        result = QPair<int,int>(0,1);
    }
    else if ( st == Kumir::LoopEnd ) // 16, кц
    {
        result = QPair<int,int>(-1,0);
    }
    else if ( st == Kumir::ConditionalEndLoop ) // 33, кц_при
    {
        result = QPair<int,int>(-1,0);
    }
    else if ( st == Kumir::Else ) // 17, иначе
    {
        result = QPair<int,int>(-1,1);
    }
    else if ( st == Kumir::If ) // 20, если
    {
        result = QPair<int,int>(0,2);
    }
    else if ( st == Kumir::Then ) // 21, то
    {
        result = QPair<int,int>(-1,1);
    }
    else if ( st == Kumir::EndSwitchOrIf ) // 22, все
    {
        result = QPair<int,int>(-2,0);
    }
    else if ( st == Kumir::Switch ) // 23, выбор
    {
        result = QPair<int,int>(0,2);
    }
    else if ( st == Kumir::Case ) // 24, при
    {
        result = QPair<int,int>(-1,1);
    }
    else if ( st == Kumir::Pre ) // 30, дано
    {
        result = QPair<int,int>(1,-1);
    }
    else if ( st == Kumir::Post ) // 31, надо
    {
        result = QPair<int,int>(1,-1);
    }
    else if ( st == Kumir::ModuleDeclaration ) // 34, исп
    {
        result = QPair<int,int>(0,1);
    }
    else if ( st == Kumir::ModuleEnd ) // 35, кон_исп
    {
        result = QPair<int,int>(-1,0);
    }
    else if ( st == Kumir::Documentation ) // 98, #
    {
        result = QPair<int,int>(1,-1);
    }
    else
    {
        result = QPair<int,int>(0,0);
    }
    int error = pt.errorCode;
    if ( allowIndent(st,error) )
        return result;
    else
        return QPair<int,int>(0,0);
}

QPair<int,int> TextNormalizer::getIndentRank(const QList<NormalizedTextInstruction> &ptl)
{
    if ( ptl.isEmpty() )
        return QPair<int,int>(0,0);

    int start = 0;
    int end = 0;

    for ( int i=0; i<ptl.count(); i++ ) {
        QPair<int,int> ir = ptl[i].indentRank;
        if ( i==0 ) {
            start = ir.first;
            end = ir.second;
        }
        else {
            end += ir.first + ir.second;
        }
    }
    return QPair<int,int>(start,end);
}



bool TextNormalizer::isBooleanConstant(const QString &text) const
{
    return constNames.contains(text);
}

void TextNormalizer::setExtraCharset(const QString &charsetSpec)
{
    m_extraCharRanges.clear();
    m_extraChars.clear();
    bool ok = false;
    bool ok1 = false;
    bool ok2 = false;
    ushort unicode;
    ushort start;
    ushort end;
    QStringList specs = charsetSpec.split(",");
    foreach (const QString &spec, specs) {
        if ( spec.contains("-") ) {
            QStringList p = spec.split("-");
            if (p.count()>1) {
                start = p[0].simplified().toUShort(&ok1,16);
                end = p[1].simplified().toUShort(&ok2,16);
                if (ok1 && ok2) {
                    m_extraCharRanges << QPair<ushort,ushort>(start,end);
                }
            }
        }
        else {
            unicode = spec.simplified().toUShort(&ok,16);
            if (ok) {
                m_extraChars << unicode;
            }
        }
    }
}

bool TextNormalizer::isValidChar(const QChar &ch) const
{
    if ( ch.unicode()<128 )
        return true;
    for ( int i=0; i<m_extraChars.count(); i++ ) {
        if ( ch.unicode()==m_extraChars[i] )
            return true;
    }
    for ( int i=0; i<m_extraCharRanges.count(); i++ ) {
        QPair<ushort,ushort> range = m_extraCharRanges[i];
        if ( ch.unicode() >= range.first && ch.unicode() <= range.second )
            return true;
    }
    return false;
}

QStringList TextNormalizer::getKeyWords()
{
	return keyWords;
}

QStringList TextNormalizer::getConstNames()
{
	return constNames;
}

QStringList TextNormalizer::getTypeNames()
{
	return typeNames;
}
