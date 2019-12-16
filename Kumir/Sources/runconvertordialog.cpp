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


#include "runconvertordialog.h"
#include "convertorprogressdialog.h"
#include "application.h"
#include <QtXml>
#include "compiler.h"
#include "int_proga.h"
#include "kum_tables.h"

#include "mainwindow.h"


RunConvertorDialog::RunConvertorDialog ( QWidget* parent, Qt::WFlags fl )
		: QDialog ( parent, fl ), Ui::RunConvertorDialog()
{
	setupUi ( this );
	connect ( btnBrowse, SIGNAL(clicked()), this, SLOT(browseTargetDirectory()) );
	setModal(true);
	progressDialog = new ConvertorProgressDialog(this,Qt::Dialog);
	targetDirectory->setText(app()->settings->value("Directories/ConvertorTarget",QDir::homePath()).toString());
}

int RunConvertorDialog::exec()
{
	QString gcc = app()->settings->value("ExternalTools/GCCPath","").toString();
	if ( gcc.isEmpty() )
	{
		invokeCompiler->setCheckState(Qt::Unchecked);
		invokeCompiler->setEnabled(false);
	}
	else {
		invokeCompiler->setChecked(app()->settings->value("CPPConveror/RunCompiler",false).toBool());;
		invokeCompiler->setEnabled(true);
	}
	return QDialog::exec();
}

void RunConvertorDialog::accept()
{
	app()->settings->setValue("Directories/ConvertorTarget",targetDirectory->text());
	app()->settings->setValue("CPPConveror/RunCompiler",invokeCompiler->isChecked());
	progressDialog->reset();
	progressDialog->setTargetDirectory(targetDirectory->text());
	QString convertor = app()->settings->value("ExternalTools/ConvertorPath","").toString();
	if ( convertor.isEmpty() )
	{
		QMessageBox::critical(this,tr("Cannot run convertor"),
													tr("Convertor location has not been set. \nPlease specify path to convertor execuable or JAR in\n Tools->Settings->External tools->Path to convertor."));
		return;
	}
	QString JRE = app()->settings->value("ExternalTools/JREPath","").toString();
	if (convertor.endsWith(".jar")&&JRE.isEmpty()) {
		QMessageBox::critical(this,tr("Cannot run convertor"),
													tr("JRE location is not set. \nPlease specify path to Java Runtime Environment >= 1.5 in\n Tools->Settings->External tools->Path to convertor."));
		return;
	}
	QString pathSeparator;
	QString envSeparator;
#ifdef WIN32
	pathSeparator = "\\";
	envSeparator = ";";
#else
	pathSeparator = "/";
	envSeparator = ":";
#endif
	QStringList env = QProcess::systemEnvironment();
	for ( int i=0; i<env.count(); i++ ) {
		if ( env[i].startsWith("PATH") ) {
			QStringList pair = env[i].split("=");
			QString newEnv = "PATH="+JRE;
			if (!JRE.endsWith(pathSeparator))
				newEnv += pathSeparator;
			newEnv += "bin"+envSeparator+pair[1];
			
			env[i] = newEnv;
		}
		if ( env[i].startsWith("JRE_HOME") ) {
			QStringList pair = env[i].split("=");
			QString newEnv = "JRE_HOME="+JRE;
			env[i] = newEnv;
		}
		if ( env[i].startsWith("JAVA_HOME") ) {
			QStringList pair = env[i].split("=");
			QString newEnv = "JAVA_HOME="+JRE;
			env[i] = newEnv;
		}
		if ( env[i].startsWith("JDK_HOME") ) {
			QStringList pair = env[i].split("=");
			QString newEnv = "JDK_HOME="+JRE;
			env[i] = newEnv;
		}
		if ( env[i].startsWith("JAVA_ROOT") ) {
			QStringList pair = env[i].split("=");
			QString newEnv = "JAVA_ROOT="+JRE;
			env[i] = newEnv;
		}
		if ( env[i].startsWith("JAVA_BINDIR") ) {
			QStringList pair = env[i].split("=");
			QString newEnv = "JAVA_BINDIR="+JRE+pathSeparator+"bin";
			env[i] = newEnv;
		}
// 		if ( env[i].startsWith("LC_") || env[i].startsWith("LANG") ) {
// 			QStringList pair = env[i].split("=");
// 			QString newEnv = pair[0]+"="+"ru_RU."+targetEncoding->currentText();
// 			env[i] = newEnv;
// 		}
	}
	QString javaOptions;
	QString pauseText;
	if ( pause->isChecked() )
		pauseText = "yes";
	else
		pauseText = "no";
	javaOptions = " -Dindent.size="+QString::number(indentSize->value())+" -Dtarget.encoding="+targetEncoding->currentText() + " -Dpause="+pauseText;
	if (!convertor.endsWith(".jar"))
	{
		env << "CONVERTOR_INDENT_SIZE="+QString::number(indentSize->value());
		env << "CONVERTOR_TARGET_ENCODING="+targetEncoding->currentText();
		javaOptions = "";
	}
	QString options = "";
	QStringList batch;
	QStringList descr;
	QString help = "file:///"+QDir::currentPath()+"/help/compound_instructions/";
	QString java_html = help+"java.html";
	QString dIndent_html = help+"convertor-indent.html";
	QString dEncoding_html = help+"convertor-encoding.html";
	QString jar_html = help+"convertor-jar.html";
	QString conv_html = help+"convertor.html";
	
	
	QString dscr;
	dscr =  "<a href='"+java_html+"'>java</a> ";
	dscr += "<a href='"+dIndent_html+"'>-Dindent.size="+QString::number(indentSize->value())+"</a> ";
	dscr += "<a href='"+dEncoding_html+"'>-Dtarget.encoding="+targetEncoding->currentText()+"</a> ";
	dscr += "-Dpause="+pauseText+" ";
	dscr += "<a href='"+jar_html+"'>-jar</a> ";
	dscr += "<a href='"+conv_html+"'>"+convertor+"</a>";
	if (convertor.endsWith(".jar")) {
		batch << "java "+javaOptions+" -jar " + convertor + options;
		descr << dscr;
	}
	else {
		batch << convertor + options;
	}
	
	progressDialog->setCompile(invokeCompiler->isChecked());	
	
	if (invokeCompiler->isChecked()) {
		
	}
	progressDialog->reset();
	progressDialog->setEnvironment(env);
	progressDialog->setBatch(batch,descr);
	QString data = xmlData();
	if (data.isEmpty()) {
		hide();
		return;
	}
	progressDialog->setInputData(data);
	hide();
	progressDialog->show();
	progressDialog->startProcessing();
}

RunConvertorDialog::~RunConvertorDialog()
{
}

void RunConvertorDialog::browseTargetDirectory()
{
	QString newTarget = QFileDialog::getExistingDirectory(this, tr("Browse target directory"),
			targetDirectory->text(),QFileDialog::ShowDirsOnly);
	if (!newTarget.isEmpty()) {
		targetDirectory->setText(newTarget);
	}
}

QString RunConvertorDialog::xmlData()
{
	QDomDocument in = QDomDocument("kumircode-1.6");
	app()->compiler->modules()->saveToXML(in);
	return in.toString();
}




