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

#include "tab.h"
#include "textedit.h"

void Tab::setFileName(const QString & name)
{
	m_fileName = name;
}

QString Tab::getFileName()
{
	return m_fileName;
}

void Tab::setModified(bool flag)
{
	editor()->setModified(flag);
}

bool Tab::isModified()
{
	return editor()->isModified();
}

