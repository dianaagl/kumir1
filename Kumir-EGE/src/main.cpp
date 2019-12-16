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

#include "application.h"
#include <stdio.h>
#include "tools.h"
#include <QtCore>

#define ERR_WRONG_PARAMETERS 127
#define ERR_WRONG_CODING_NAME 126
#define ERR_FILE_NOT_FOUND 125
#define ERR_FILE_ACCESS_ERROR 124


int main_check();
int main_run();
int main_usage();

int main(int argc, char *argv[])
{

	new QCoreApplication(argc, argv);

	QCoreApplication::setOrganizationName(QString::fromUtf8("НИИСИ РАН"));
	QCoreApplication::setApplicationName(QString::fromUtf8("Кумир"));
	QCoreApplication::setApplicationVersion("1.8");
	Application::createInstance();
	Application::instance()->createSettings();
	Application::instance()->createMessagesProvider();
	Application::instance()->setLanguage();
	Application::instance()->createCompiler();
	Application::instance()->createRuntime();

	if (qApp->arguments().contains("--check") || qApp->arguments().contains("-c")) {
		return main_check();
	}
	else if (qApp->arguments().contains("--run") || qApp->arguments().contains("-r")) {
		return main_run();
	}
	else if (qApp->arguments().contains("--help") || qApp->arguments().contains("-h")) {
		return main_usage();
	}
	else if (qApp->arguments().contains("--version")) {
		fprintf(stdout, "ckumir version 2010-11-23, based on Kumir 1.7.2\n");
		return 0;
	}
	else {
		fprintf(stderr, "Wrong paramaters to launch.\n");
		main_usage();
		return ERR_WRONG_PARAMETERS;
	}
}
typedef QTextCodec* PQTextCodec;
int read_program(QString &data, PQTextCodec &codec)
{
	QString codingName = "UTF-16LE";
	QString programName = "";
	for (int i=1; i<qApp->arguments().size(); i++) {
		const QString arg = qApp->arguments()[i];
		if (arg.startsWith("--program-encoding=")) {
			codingName = arg.mid(QString("--program-encoding=").length()).toUpper();
		}
		else if (!arg.startsWith("--")) {
			programName = arg;
		}
	}
	if (programName.isEmpty()) {
		fprintf(stderr, "File name not specified.\n");
		main_usage();
		return ERR_WRONG_PARAMETERS;
	}
	if (!QFile::exists(programName)) {
		fprintf(stderr, "File not found: %s\n", programName.toLocal8Bit().data());
		return ERR_FILE_NOT_FOUND;
	}
	QFile program(programName);
	if (!program.open(QIODevice::ReadOnly|QIODevice::Text)) {
		fprintf(stderr, "Error accessing file: %s\n", programName.toLocal8Bit().data());
		return ERR_FILE_ACCESS_ERROR;
	}
	QTextStream ts(&program);
	codec = QTextCodec::codecForName(codingName.toUpper().toAscii().data());
	if (!codec) {
		fprintf(stderr, "Wrong codec: %s\n", codingName.toAscii().data());
		program.close();
		return ERR_WRONG_CODING_NAME;
	}
	ts.setCodec(codec);
	data = ts.readAll();
	program.close();
	return 0;
}

int main_check()
{
	QString data;
	PQTextCodec codec;
	int status = read_program(data, codec);
	if (status) {
		return status;
	}
	else {
		QString errEnc = "WINDOWS-1251";
		QTextCodec *errCodec = 0;
		for (int i=1; i<QCoreApplication::instance()->arguments().size(); i++) {
			const QString arg = QCoreApplication::instance()->arguments()[i];
			if (arg.startsWith("--messages-encoding=")) {
				errEnc = arg.mid(QString("--messages-encoding=").size());
			}
		}
		errCodec = QTextCodec::codecForName(errEnc.toUpper().toAscii());
		if (!errCodec) {
			fprintf(stderr, "Wrong codec: %s\n", errEnc.toAscii().data());
			return ERR_WRONG_CODING_NAME;
		}
		return Application::instance()->compileText(data, errCodec);
	}
}

int main_run()
{
	QString data;
	PQTextCodec codec;
	int status = read_program(data, codec);
	if (status)
		return status;
	else {
		QString inEnc = "WINDOWS-1251";
		QString outEnc = "WINDOWS-1251";
		QString errEnc = "WINDOWS-1251";
		QTextCodec *inCodec = 0;
		QTextCodec *outCodec = 0;
		QTextCodec *errCodec = 0;
		QFile *inFile = 0;
		QString basename;
		for (int i=1; i<QCoreApplication::instance()->arguments().size(); i++) {
			const QString arg = QCoreApplication::instance()->arguments()[i];
			if (arg.startsWith("--input-encoding=")) {
				inEnc = arg.mid(QString("--input-encoding=").size());
			}
			if (arg.startsWith("--output-encoding=")) {
				outEnc = arg.mid(QString("--output-encoding=").size());
			}
			if (arg.startsWith("--messages-encoding=")) {
				errEnc = arg.mid(QString("--messages-encoding=").size());
			}
			if (arg.startsWith("--input-source=")) {
				inFile = new QFile(arg.mid(QString("--input-source=").size()));
				inFile->open(QIODevice::ReadOnly);
			}
			if (!arg.startsWith("-")) {
				basename = arg;
			}
		}
		inCodec = QTextCodec::codecForName(inEnc.toUpper().toAscii());
		if (!inCodec) {
			fprintf(stderr, "Wrong codec: %s\n", inEnc.toAscii().data());
			return ERR_WRONG_CODING_NAME;
		}
		outCodec = QTextCodec::codecForName(outEnc.toUpper().toAscii());
		if (!outCodec) {
			fprintf(stderr, "Wrong codec: %s\n", outEnc.toAscii().data());
			return ERR_WRONG_CODING_NAME;
		}
		errCodec = QTextCodec::codecForName(errEnc.toUpper().toAscii());
		if (!errCodec) {
			fprintf(stderr, "Wrong codec: %s\n", errEnc.toAscii().data());
			return ERR_WRONG_CODING_NAME;
		}
		status = Application::instance()->compileText(data, errCodec);
		if (status) {
			return status;
		}
		return Application::instance()->evaluateProgram(inCodec, outCodec, errCodec, inFile);
	}
}

int main_usage()
{
	QFileInfo fi(qApp->arguments().at(0));
	const QString arg0 = fi.fileName();
	fprintf(stdout, "Usage:\n\n");
	fprintf(stdout, "\t%s --help\n\t\tPrint this message and exit.\n\n",
			arg0.toLocal8Bit().data());
	fprintf(stdout, "\t%s --check [--program-encoding=ENC] [--output-encoding=ENC] [--input-encoding=ENC] [--messages-encoding=ENC] FILENAME\n\t\tCheck program in FILENAME and exit. Use ENC ecoding for file I/O.\n\n",
			arg0.toLocal8Bit().data());
	fprintf(stdout, "\t%s --run [--program-encoding=ENC] [--output-encoding=ENC] [--input-encoding=ENC] [--messages-encoding=ENC] FILENAME\n\t\tCheck program in FILENAME and exit. . Use ENC ecoding for file I/O.\n\n",
			arg0.toLocal8Bit().data());
	fprintf(stdout, "Important notice: default input/output/messages encoding is Windows-1251\n");
	fprintf(stdout, "Important notice: default program encoding is UTF-16LE\n");
	return 0;
}
