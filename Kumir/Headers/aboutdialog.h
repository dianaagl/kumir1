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

#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include "ui_about.h"

class AboutDialog : public QDialog, public Ui::AboutDialog
{
		Q_OBJECT

	public:
		AboutDialog ( QWidget* parent = 0, Qt::WindowFlags fl = 0 );
		~AboutDialog();
		/*$PUBLIC_FUNCTIONS$*/

	public slots:
		/*$PUBLIC_SLOTS$*/

	protected:
		/*$PROTECTED_FUNCTIONS$*/

	protected slots:
		/*$PROTECTED_SLOTS$*/

};

#endif

