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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QFile>
#include <QFileInfo>
#include <QMenu>
#include <QModelIndex>
class QLineEdit;

#include "../course_changes.h"
#include "../taskControlInterface.h"

class NewKursDialog;
class EditDialog;
class CSInterface;
class CourseModel;

namespace Ui
{
class MainWindowTask;
}

class MainWindowTask : public QMainWindow
{
	Q_OBJECT

public:
	MainWindowTask(QWidget *parent = 0);
	~MainWindowTask();

	void setInterface(CSInterface *csInterface) { interface = csInterface; }
	void setCS(QString cs) { CS = cs; }
	KumZadanie *getTask() { return &task; }

	void setup();
	QString getFileName(QString fileName);
	void setTeacher(bool mode);

public slots:
	void aboutToQuit();

	void loadCourse();
	void returnTested();
	void saveCourse();
	void saveCourseFile();
	void saveBaseKurs();
	void showText(const QModelIndex &index);
	void loadHtml(QString fileName);
	void startTask();
	void checkTask();
	void lockCheck();
	void setMark(int mark);
	void resetTask();
	void Close();
	void customContextMenuRequested(QPoint pos);
	void addTask();
	void addDeepTask();
	void deleteTask();
	void saveKurs();//TEACHER
	void saveKursAs();//TEACHER
	void editTask();//Teacher
	void setEditTaskEnabled(bool flag);
	void moveUp();
	void moveDown();
	void newKurs();
	void endRootEdit();
	void cancelRootEdit();

protected:
	void changeEvent(QEvent *e);
	void closeEvent(QCloseEvent *event);

private:
	void markProgChange();
	void createMoveMenu();
	void setUpDown(QModelIndex index);
	QString loadScript(QString file_name);
	QString loadTestAlg(QString file_name);
	void loadCourseData(QString filename);
	void loadMarks(QString fileName);

	bool onTask;
	bool isTeacher;
	QString curDir;
	QString cursFile;
	QString CS;
	KumZadanie task;
	CourseModel *course;
	CSInterface *interface;
	EditDialog *editDialog;
	NewKursDialog *newDialog;
	Ui::MainWindowTask *ui;
	courseChanges changes;

	QModelIndex curTaskIdx;
	QList<int> progChange;
	QMenu customMenu;
	QSettings *settings;
	QLineEdit *editRoot;
	QFile cursWorkFile;
	QFileInfo baseKursFile; //4 mode
};

#endif // MAINWINDOW_H
