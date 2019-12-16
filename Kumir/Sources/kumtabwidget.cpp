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
#include "kumtabwidget.h"
#include "kumtabbar.h"

KumTabWidget::KumTabWidget(QWidget *parent)
 : QTabWidget(parent)
{
	tabBar = new KumTabBar(this);
	setTabBar(tabBar);
	connect ( tabBar, SIGNAL(tabCloseClicked(int)), this, SLOT(closeTab(int)) );
}

void KumTabWidget::closeTab(int no)
{
	// TODO сделать запрос на сохранение
	emit closeTab1(no);
	//removeTab(no);
}

KumTabWidget::~KumTabWidget()
{
}

void KumTabWidget::hideTabBar()
{
	tabBar->hide();
}

void KumTabWidget::showTabBar()
{
	tabBar->show();
}
