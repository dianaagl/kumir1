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

#ifndef KEYWORDSWINDOW_H
#define KEYWORDSWINDOW_H

#include <QWidget>
#include "ui_KeywordsWindow.h"

class KeywordsWindow : public QWidget, Ui::KeywordsWindow
{
	Q_OBJECT

	public:
		KeywordsWindow ( QWidget* parent = NULL, Qt::WFlags fl = 0 );
		~KeywordsWindow();

	public slots:
		void show();

	protected:
		void closeEvent ( QCloseEvent * event );

	protected slots:

};

#endif
