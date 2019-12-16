/****************************************************************************
**
** Copyright (C) 2004-2009 NIISI RAS. All rights reserved.
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

#define APP_DEBUG 1
#define COMPILER_SEPARATE_THREAD 0
#define AUTOSAVE_PERIOD_IN_MINUTES 5

#include "application.h"
#include "compiler.h"
#include "messagesprovider.h"
#include "run.h"
#include "kassert.h"
#include "int_proga.h"
#include <QtCore>
#include <QtScript>
#include <iostream>
#include "tools.h"
#include <stdio.h>

#ifndef WIN32
#include "unistd.h"
#endif


Application::Application() : QObject()
{
    compiler = NULL;
    run = NULL;
    messagesProvider = NULL;
    errorText=QString::fromUtf8("ОШИБКА ИСПОЛНИТЕЛЯ");
}


void Application::createSettings()
{
}


void Application::createMessagesProvider()
{
    messagesProvider = new MessagesProvider();
}



void Application::createCompiler()
{
    compiler = new Kum::Compiler();
}

void Application::createRuntime()
{
    run = new Kum::Run();
}


void Application::setLanguage()
{

	QString languageName = "ru_RU";
	foreach (const QString &arg, qApp->arguments()) {
		if (arg.startsWith("--language=")) {
			languageName = arg.mid(11);
			break;
		}
	}

    if ( !currentLanguage().isEmpty() && currentLanguage() == languageName )
        return;

    const QString langPath = QDir::cleanPath(QCoreApplication::applicationDirPath()+"/../share/kumir/languages/");
    QDir curDir = QDir(langPath);

    QStringList langFilesFilter;
    langFilesFilter << "*.lang";

    curDir.setNameFilters(langFilesFilter);
    QStringList errfiles;
    QStringList posixNames;
    QStringList keyfiles;
    QStringList extraCharsets;

    foreach ( const QString &fileName, curDir.entryList() )
    {
        QFile f(langPath+"/"+fileName);
        QTextStream ts(&f);
        ts.setCodec("UTF-8");
        if ( f.open(QIODevice::ReadOnly) )
        {
            QString data = ts.readAll();
            f.close();
            QString errfile;
            QString posixname;
            QString extracharset;
            QString keyfile;
            QStringList lines = data.split("\n");
            foreach ( QString line, lines )
            {
                QStringList pair = line.split("=");
                if ( pair.count() == 2 )
                {                    
                    if ( pair[0].simplified() == "err_file" )
                        errfile = pair[1].simplified();
                    else if ( pair[0].simplified() == "posix_name" )
                        posixname = pair[1].simplified();
                    else if ( pair[0].simplified() == "extra_charset" )
                        extracharset = pair[1].simplified();
                    else if ( pair[0].simplified() == "key_file" )
                        keyfile = pair[1].simplified();
                }
            }
            m_languageList << posixname;
            errfiles << errfile;
            keyfiles << keyfile;
            posixNames << posixname;
            extraCharsets << extracharset;
        }
    }
    if ( m_languageList.contains(languageName) )
    {
        int i = m_languageList.indexOf(languageName);
        m_currentLanguage = languageName;
        KumTools::instance()->initNormalizator(langPath+"/"+keyfiles[i]);
        KumTools::instance()->setExtraCharset(extraCharsets[i]);
        messagesProvider->init(errfiles[i]);
    }

}



/** Возвращает список доступный языков */
QStringList Application::languageList()
{
    return m_languageList;
}

/** Возвращает текущий язык */
QString Application::currentLanguage()
{
    return m_currentLanguage;
}

/**
 * Вызов компиляции всего что есть. 
 * Вызывать перед выполнением или при изменении списка исполнителей 
 */
void Application::doCompleteCompilation()
{

}

void Application::doCompilation(const QString &)
{
    if ( APP_DEBUG )
        messagesProvider->setDebugMode(true);
    //compiler->compile(text);
}

int Application::compileText(const QString &text, const QTextCodec *messagesCodec)
{
    QList<Kum::CompileError> errors;
    int status = compiler->compile(text, errors);
    if (status) {
        return status;
    }

    const QStringList lines = text.split("\n",QString::KeepEmptyParts);

    foreach (const Kum::CompileError &error, errors) {
        QString line = lines[error.line].left(60);
        if (lines[error.line].size()>60) {
            line += "...";
        }
        const QString errorText = messagesProvider->errorMessage(uint(error.code)).trimmed();
        const QString message = QString::fromUtf8("Строка %1: %2\nМесто ошибки: от %3 до %4\n%5 (код %6)\n\n")
                                .arg(error.line+1).arg(line).arg(error.start+1).arg(error.end+1).arg(errorText).arg(error.code);
        fprintf(stderr, "%s", messagesCodec->fromUnicode(message).data());
    }
    return errors.size()>0? 1 : 0;
}



int Application::evaluateProgram(QTextCodec *inCodec,
								 QTextCodec *outCodec,
								 QTextCodec *errCodec,
								 QFile *inFile)
{
    m_inCodec = inCodec;
    m_outCodec = outCodec;
    m_errCodec = errCodec;

    if (inFile) {
        m_inputStream = new QTextStream(inFile);
    }
    else {
        m_inputStream = new QTextStream(stdin);
    }
    m_inputStream->setCodec(m_inCodec);

    run->init(compiler->modules(), compiler->mainModuleId(), compiler->StartPos());
    while (run->hasMoreInstructions()) {
        run->evaluateNextInstruction();
    }
    if (run->lastErrorCode()) {
        const QString err = messagesProvider->errorMessage(run->lastErrorCode()).trimmed();
        const QString message = QString::fromUtf8("Ошибка выполнения в строке %1:\n%2 (код %3)\n")
                                .arg(run->lastErrorLine()+1).arg(err).arg(run->lastErrorCode());

        fprintf(stderr, "%s", errCodec->fromUnicode(message).data());
    }
    QString out3;
    QString basename;
    for (int i=1; i<qApp->arguments().size(); i++) {
        if (qApp->arguments()[i].startsWith("--o3="))
            out3 = qApp->arguments()[i].mid(QString("--o3=").size());
        if (!qApp->arguments()[i].startsWith("-")) {
            basename = qApp->arguments()[i];
        }
    }
    if (qApp->arguments().contains("-b")) {
        QFile file3(out3);
        if (file3.open(QIODevice::WriteOnly|QIODevice::Text)) {
            QTextStream ts(&file3);
            ts.setCodec("UTF-8");
            ts << "1\n-1\n>> "+basename+"\n";
            ts << output;
            if (run->lastErrorCode()) {
                ts << QString::fromUtf8("\n>> ОШИБКА ВЫПОЛНЕНИЯ: %1 (%2)\n")
                        .arg(messagesProvider->errorMessage(run->lastErrorCode()))
                        .arg(run->lastErrorCode());
            }
            file3.close();
        }
        else {
            qDebug() << "Can't open out3 file: " << out3;
        }
    }
    m_inputStream->flush();
    if (inFile) {
        inFile->close();
    }
    delete m_inputStream;
    return run->lastErrorCode()? 2 : 0;
}


Application* Application::instance()
{
    Q_ASSERT(m_instance!=NULL);
    return m_instance;
}

Application* Application::m_instance = NULL;

void Application::createInstance()
{
    m_instance = new Application();
}

Application *app() {
    return Application::instance();
}

void Application::handleConsoleError(const QString &text)
{
    QTextStream ts(stderr);
    ts.setCodec(m_errCodec);
    ts << text;
}

QStringList Application::handleConsoleInput(const QString &format, QString &error)
{
    QString r;
    QString result;
    forever {
        if (m_inputStream->atEnd()) {
            break;
        }
        r = m_inputStream->read(1);
        if (r[0].unicode()==10) {
            break;
        }
        else if (r[0].unicode()==13) {
            // pass
        }
        else {
            result += r;
        }
    }
    QStringList inp;
    KumTools::instance()->parceInputString(format, result, inp, error);
    return inp;
}

void Application::handleConsoleOutput(const QString &text)
{
    QTextStream ts(stdout);
    ts.setCodec(m_outCodec);
    ts << text;
    output += text;
}
