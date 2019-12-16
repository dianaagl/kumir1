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

#ifndef ASSISTANT_H
#define ASSISTANT_H

#include <QMainWindow>
#include "ui_Assistant.h"
#include "kum_tables.h"

class symbol_table;
class function_table;
class isp_table;
class KumModules;

class Assistant : public QMainWindow, private Ui::Assistant
{
	Q_OBJECT

	public:
		explicit Assistant ( QWidget* parent = 0 );
		~Assistant();
		void init(KumModules *I);
		void setStyleSheet(const QString &styleSheet);
		QString styleSheet() const;

	public slots:
		void updateAlgorhitms();
		void sortModules();

	protected:
		KumModules *I;
		QString getHtmlCode(int modId, int funcId) const;
		QString defaultPageHtml() const;
		QString modulePageHtml(const QString &moduleName) const;
		QString mainModuleUsage() const;
		bool isInternalFunction(const QString& name);
		QString getSyntaxFromTable(int modId, int funcId, QString &description) const;
		void showEvent(QShowEvent *e);
		void setEnableForUserAlgs();
		void getFuncByName(QString name, int& moduleId, int& id);
		QString s_styleSheet;
		QIcon icon_folder_enabled_open;
		QIcon icon_folder_enabled_closed;
		QIcon icon_folder_disabled_open;
		QIcon icon_folder_disabled_closed;
		QIcon icon_item_enabled;
		QIcon icon_item_disabled;

	protected slots:
		void updatePlaceholderText();
		void handleCheckBoxStateClicked(bool state);
		void performSearch(const QString &line);
		void showHtmlInBrowser(QTreeWidgetItem *item, int column);
		void setOpenedIcon(QTreeWidgetItem *item);
		void setClosedIcon(QTreeWidgetItem *item);

};

#endif

