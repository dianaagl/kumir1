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

#include "keywordswindow.h"
#include "application.h"
#include "tools.h"

KeywordsWindow::KeywordsWindow (QWidget* parent, Qt::WFlags fl) : QWidget(parent, fl), Ui::KeywordsWindow()
{
	setupUi ( this );
	QFont bigFont = listWidget->font();
	bigFont.setPointSize(bigFont.pointSize()+1);
	bigFont.setBold(true);
	listWidget->setFont(bigFont);
	listWidget->setSortingEnabled(true);
}

KeywordsWindow::~KeywordsWindow()
{
}

void KeywordsWindow::show()
{
	restoreGeometry(app()->settings->value("KeywordsWindow/Geometry","").toByteArray());
	listWidget->clear();
	QStringList list;
	list << KumTools::instance()->getTypeNames();
	list << KumTools::instance()->getConstNames();
	list << KumTools::instance()->getKeyWords();
	foreach(QString s, list)
	{
		listWidget->addItem( s.remove("\\b").replace("\\s*","").replace("(\\s+|_)","_") );
	}
	QWidget::show();
}

void KeywordsWindow::closeEvent(QCloseEvent *event)
{
	app()->settings->setValue("KeywordsWindow/Geometry",saveGeometry());
	event->accept();
}
