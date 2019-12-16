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

#include "textedit.h"
#include "undocommands.h"
#include "lineprop.h"
#include "kumiredit.h"
#include "application.h"

KeyCharectCommand::KeyCharectCommand ( TextEdit *editor, const QString text )
{
	hasSelection = editor->textCursor().hasSelection();
	selectedText = editor->textCursor().selectedText();
	selectionStart = editor->textCursor().selectionStart();
	selectionEnd = editor->textCursor().selectionEnd();
	position = editor->textCursor().position();
	removeSize = text.length();
	this->text = text;
	this->editor = editor;
	setText ( "KeyCharectCommand" );
	overwrite = editor->overwriteMode();
}

int KeyCharectCommand::id()
{
	return 0;
}

void KeyCharectCommand::undo()
{
	LineProp *lp = NULL;
	QTextCursor cursor = editor->textCursor();
	if ( hasSelection )
		cursor.setPosition( selectionStart );
	else
		cursor.setPosition ( position );
	for ( uint i=0; i<removeSize; i++ )
	{
		lp = static_cast<LineProp*>(cursor.block().userData());
		if ( lp != NULL )
			lp->lineChanged = true;
		cursor.deleteChar();
		lp = static_cast<LineProp*>(cursor.block().userData());
		if ( lp != NULL )
			lp->lineChanged = true;
	}
	if ( overwrite ) {
		cursor.insertText(prevCharect);
		lp = static_cast<LineProp*>(cursor.block().userData());
		if ( lp != NULL )
			lp->lineChanged = true;
	}
	if ( hasSelection )
	{
		editor->textCursor().insertText ( selectedText );
		lp = static_cast<LineProp*>(cursor.block().userData());
		if ( lp != NULL )
			lp->lineChanged = true;
	}
}

void KeyCharectCommand::redo()
{
	QTextCursor cursor = editor->textCursor();
	LineProp *lp = NULL;
	if ( hasSelection )
	{
		cursor.setPosition ( selectionStart );
		for ( uint i=selectionStart; i<selectionEnd; i++ )
		{
			lp = static_cast<LineProp*>(cursor.block().userData());
			if ( lp != NULL )
				lp->lineChanged = true;
			cursor.deleteChar();
			lp = static_cast<LineProp*>(cursor.block().userData());
			if ( lp != NULL )
				lp->lineChanged = true;
		}
	}
	if ( hasSelection ) {
		cursor.setPosition( selectionStart );
	}
	else {
		cursor.setPosition ( position );
		if ( overwrite ) {
			QChar nextCharect = editor->toPlainText()[cursor.position()];
			if ( nextCharect != '\n' ) {
				cursor.movePosition(QTextCursor::NextCharacter,QTextCursor::KeepAnchor);
				if ( cursor.hasSelection() )
				{
					prevCharect = editor->toPlainText().mid(cursor.selectionStart(),cursor.selectionEnd()-cursor.selectionStart());
					cursor.removeSelectedText();
					lp = static_cast<LineProp*>(cursor.block().userData());
					if ( lp != NULL )
						lp->lineChanged = true;
				}
			}
		}
	}
	lp = static_cast<LineProp*>(cursor.block().userData());
	if ( lp != NULL )
		lp->lineChanged = true;
	cursor.insertText ( text );
	lp = static_cast<LineProp*>(cursor.block().userData());
	if ( lp != NULL )
		lp->lineChanged = true;
}

bool KeyCharectCommand::mergeWith ( const QUndoCommand * other )
{
	if ( other->id() != id() )
	{
		return false;
	}
	else
	{
		QRegExp spacer = QRegExp ( "\\W+" );
		const KeyCharectCommand *command = static_cast<const KeyCharectCommand*> ( other );
		if ( spacer.exactMatch ( command->text ) )
		{
			text += command->text;
			return true;
		}
		else
		{
			return false;
		}
	}
}

KeyEnterCommand::KeyEnterCommand ( TextEdit *editor )
{
	hasSelection = editor->textCursor().hasSelection();
	selectedText = editor->textCursor().selectedText();
	selectionStart = editor->textCursor().selectionStart();
	selectionEnd = editor->textCursor().selectionEnd();
	position = editor->textCursor().position();
	this->editor = editor;
	editor->forceEmitCompilation = true;
	setText ( "KeyEnterCommand" );
}

int KeyEnterCommand::id()
{
	return 1;
}

void KeyEnterCommand::undo()
{
	KumirEdit *ke = dynamic_cast<KumirEdit*>(editor);
	LineProp *lp = NULL;
	QTextCursor cursor = editor->textCursor();
	cursor.setPosition ( position );
	if ( ke != NULL ) {
		ke->stopListeningEvents();
	}
	for ( int i=1; i<toInsert.length(); i++ ) {
		lp = static_cast<LineProp*>(cursor.block().userData());
		if ( lp != NULL )
			lp->lineChanged = true;
		cursor.deleteChar();
		lp = static_cast<LineProp*>(cursor.block().userData());
		if ( lp != NULL )
			lp->lineChanged = true;
	}
	if ( ke != NULL ) {
		ke->startListeningEvents();
	}
	lp = static_cast<LineProp*>(cursor.block().userData());
	if ( lp != NULL )
		lp->lineChanged = true;
	cursor.deleteChar();
	lp = static_cast<LineProp*>(cursor.block().userData());
	if ( lp != NULL )
		lp->lineChanged = true;
	if ( hasSelection )
	{
		lp = static_cast<LineProp*>(cursor.block().userData());
		if ( lp != NULL )
			lp->lineChanged = true;
		editor->textCursor().insertText ( selectedText );
		lp = static_cast<LineProp*>(cursor.block().userData());
		if ( lp != NULL )
			lp->lineChanged = true;
	}
	editor->initLineProps();
}

void KeyEnterCommand::redo()
{
	LineProp *lp = NULL;
	QTextCursor cursor = editor->textCursor();
	if ( hasSelection )
	{
		cursor.setPosition ( selectionStart );
		for ( uint i=selectionStart; i<selectionEnd; i++ )
		{
			lp = static_cast<LineProp*>(cursor.block().userData());
			if ( lp != NULL )
				lp->lineChanged = true;
			cursor.deleteChar();
			lp = static_cast<LineProp*>(cursor.block().userData());
			if ( lp != NULL )
				lp->lineChanged = true;
		}
	}
	KumirEdit *ke = dynamic_cast<KumirEdit*>(editor);
	toInsert = "\n";
	if ( ke != NULL ) {
		QString line = cursor.block().text();
		while ( line.startsWith("|") || line.startsWith(" ") ) {
			toInsert += line[0];
			line.remove(0,1);
		}
	}	
	cursor.setPosition ( position );
	lp = static_cast<LineProp*>(cursor.block().userData());
	if ( lp != NULL )
		lp->lineChanged = true;
	cursor.insertText ( toInsert );
	lp = static_cast<LineProp*>(cursor.block().userData());
	if ( lp != NULL )
		lp->lineChanged = true;
	editor->initLineProps();
}

KeyDeleteCommand::KeyDeleteCommand ( TextEdit *editor )
{
	hasSelection = editor->textCursor().hasSelection();
	selectedText = editor->textCursor().selectedText();
	selectionStart = editor->textCursor().selectionStart();
	selectionEnd = editor->textCursor().selectionEnd();
	position = editor->textCursor().position();
	this->editor = editor;
	if ( hasSelection ) {
		if ( selectedText.contains("\n"))
			editor->forceEmitCompilation = true;
	}
	else {
		QString line = editor->textCursor().block().text();
		int pos = editor->textCursor().columnNumber();
		if ( pos==line.length()-1 )
			editor->forceEmitCompilation = true;
	}
	
	
	text = editor->toPlainText() [position];
	setText ( "KeyDeleteCommand" );
}
int KeyDeleteCommand::id() { return 2; }

void KeyDeleteCommand::undo()
{
	LineProp *lp = NULL;
	QTextCursor cursor = editor->textCursor();
	if ( hasSelection )
	{
		cursor.setPosition ( selectionStart );
		lp = static_cast<LineProp*>(cursor.block().userData());
		if ( lp != NULL )
			lp->lineChanged = true;
		cursor.insertText ( selectedText );
		lp = static_cast<LineProp*>(cursor.block().userData());
		if ( lp != NULL )
			lp->lineChanged = true;
	}
	else
	{
		cursor.setPosition ( position );
		lp = static_cast<LineProp*>(cursor.block().userData());
		if ( lp != NULL )
			lp->lineChanged = true;
		cursor.insertText ( text );
		lp = static_cast<LineProp*>(cursor.block().userData());
		if ( lp != NULL )
			lp->lineChanged = true;
		cursor.movePosition ( QTextCursor::PreviousCharacter );
	}
	editor->setTextCursor ( cursor );
	editor->initLineProps();
}
void KeyDeleteCommand::redo()
{
	LineProp *lp;
	KumirEdit *ke = dynamic_cast<KumirEdit*>(editor);
// 	if ( ke != NULL ) {
// 		ke->stopListeningEvents();
// 	}
	
	QTextCursor cursor = editor->textCursor();
	if ( hasSelection )
	{
		
// 		cursor.setPosition ( selectionStart );
// 		cursor.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor,selectionEnd-selectionStart);
// 		cursor.deleteChar();
		QString selectedText = editor->toPlainText().mid(selectionStart, selectionEnd-selectionStart);
		if ( ke != NULL )
			ke->forceEmitCompilation = selectedText.contains("\n");	
		lp = static_cast<LineProp*>(cursor.block().userData());
		if ( lp != NULL )
			lp->lineChanged = true;
		cursor.removeSelectedText();
		
	}
	if ( hasSelection ) {
		cursor.setPosition(selectionStart);
	}
	else
		cursor.setPosition ( position );
	if ( !hasSelection ) {
		lp = static_cast<LineProp*>(cursor.block().userData());
		if ( lp != NULL )
			lp->lineChanged = true;
		cursor.deleteChar();
		lp = static_cast<LineProp*>(cursor.block().userData());
		if ( lp != NULL )
			lp->lineChanged = true;
	}
	editor->initLineProps();
// 	if ( ke != NULL ) {
// 		ke->startListeningEvents();
// 	}
	
}

CutCommand::CutCommand ( TextEdit *editor )
{
	selectedText = editor->textCursor().selectedText();
	selectionStart = editor->textCursor().selectionStart();
	selectionEnd = editor->textCursor().selectionEnd();
	position = editor->textCursor().position();
	this->editor = editor;
	if ( selectedText.contains("\n") )
		editor->forceEmitCompilation = true;
	setText ( "CutCommand" );
}
int CutCommand::id() { return 3; }
void CutCommand::undo()
{
	LineProp *lp = NULL;
	QTextCursor cursor = editor->textCursor();
	cursor.setPosition ( selectionStart );
	lp = static_cast<LineProp*>(cursor.block().userData());
	if ( lp != NULL )
		lp->lineChanged = true;
	cursor.insertText ( selectedText );
	lp = static_cast<LineProp*>(cursor.block().userData());
	if ( lp != NULL )
		lp->lineChanged = true;
	editor->setTextCursor ( cursor );
	editor->initLineProps();
}
void CutCommand::redo()
{
	LineProp *lp = NULL;
	QTextCursor cursor = editor->textCursor();
	cursor.setPosition ( selectionStart );
	for ( uint i=selectionStart; i<selectionEnd; i++ )
	{
		lp = static_cast<LineProp*>(cursor.block().userData());
		if ( lp != NULL )
			lp->lineChanged = true;
		cursor.deleteChar();
		lp = static_cast<LineProp*>(cursor.block().userData());
		if ( lp != NULL )
			lp->lineChanged = true;
	}
	cursor.setPosition ( position );
	editor->initLineProps();
}

KeyBackspaceCommand::KeyBackspaceCommand ( TextEdit *editor )
{
	hasSelection = editor->textCursor().hasSelection();
	selectedText = editor->textCursor().selectedText();
	selectionStart = editor->textCursor().selectionStart();
	selectionEnd = editor->textCursor().selectionEnd();
	position = editor->textCursor().position();
	this->editor = editor;
	text = editor->toPlainText() [position-1];
	setText ( "KeyBackspaceCommand" );
	if ( hasSelection ) {
		if ( selectedText.contains("\n"))
			editor->forceEmitCompilation = true;
	}
	else {
		QString line = editor->textCursor().block().text();
		int pos = editor->textCursor().columnNumber();
		if ( pos==line.length()-1 )
			editor->forceEmitCompilation = true;
	}
}
int KeyBackspaceCommand::id() { return 4; }
void KeyBackspaceCommand::undo()
{
	LineProp *lp;
	QTextCursor cursor = editor->textCursor();
	if ( hasSelection )
	{
		cursor.setPosition ( selectionStart );
		lp = static_cast<LineProp*>(cursor.block().userData());
		if ( lp != NULL )
			lp->lineChanged = true;
		cursor.insertText ( selectedText );
		lp = static_cast<LineProp*>(cursor.block().userData());
		if ( lp != NULL )
			lp->lineChanged = true;
	}
	else
	{
		cursor.setPosition ( position-1 );
		lp = static_cast<LineProp*>(cursor.block().userData());
		if ( lp != NULL )
			lp->lineChanged = true;
		cursor.insertText ( text );
		lp = static_cast<LineProp*>(cursor.block().userData());
		if ( lp != NULL )
			lp->lineChanged = true;
// 				cursor.movePosition(QTextCursor::PreviousCharacter);
	}
	editor->setTextCursor ( cursor );
	editor->initLineProps();
}

void KeyBackspaceCommand::redo()
{
	LineProp *lp = NULL;
	QTextCursor cursor = editor->textCursor();
	if ( hasSelection )
	{
		if ( editor->isKumirEditor() ) {
			KumirEdit *ke = dynamic_cast<KumirEdit*>(editor);
			ke->stopListeningEvents();
			cursor.setPosition ( selectionStart );
			bool NLdeleted = false;
			for ( uint i=selectionStart; i<selectionEnd; i++ )
			{
				if ( editor->toPlainText()[cursor.position()]=='\n' )
					NLdeleted = true;
				lp = static_cast<LineProp*>(cursor.block().userData());
				if ( lp != NULL )
					lp->lineChanged = true;
				cursor.deleteChar();
				lp = static_cast<LineProp*>(cursor.block().userData());
				if ( lp != NULL )
					lp->lineChanged = true;
			}
			if (NLdeleted&&ke->tabNo>-1) {
				app()->doCompilation(ke->tabNo);
			}
			ke->startListeningEvents();
		}
		else {
			cursor.setPosition ( selectionStart );
			for ( uint i=selectionStart; i<selectionEnd; i++ )
			{
				lp = static_cast<LineProp*>(cursor.block().userData());
				if ( lp != NULL )
					lp->lineChanged = true;
				cursor.deleteChar();
				lp = static_cast<LineProp*>(cursor.block().userData());
				if ( lp != NULL )
					lp->lineChanged = true;
			}
		}
	}
	if ( hasSelection ) {
		cursor.setPosition(selectionStart);
	}
	else
		cursor.setPosition ( position );
	if ( !hasSelection ) {
		lp = static_cast<LineProp*>(cursor.block().userData());
		if ( lp != NULL )
			lp->lineChanged = true;
		cursor.deletePreviousChar();
		lp = static_cast<LineProp*>(cursor.block().userData());
		if ( lp != NULL )
			lp->lineChanged = true;
	}
	editor->initLineProps();
}

InsertCommand::InsertCommand ( TextEdit *editor, QString txt )
{
	this->editor = editor;
	this->txt = txt;
	QTextCursor cursor = editor->textCursor();
	if ( cursor.hasSelection() ) {
		hasSelection = true;
		selectionStart = cursor.selectionStart();
		selectionEnd = cursor.selectionEnd();
		position = selectionStart;
	}
	else {
		hasSelection = false;
		position = cursor.position();
	}
	setText ( "InsertCommand" );
}
int InsertCommand::id() { return 5; }
void InsertCommand::undo()
{
	LineProp *lp = NULL;
	KumirEdit *ke = qobject_cast<KumirEdit*>(editor);
	if ( ke != NULL )
		ke->stopListeningEvents();
	QTextCursor cursor = editor->textCursor();
	cursor.setPosition ( position );
	for ( int i=0; i<txt.length(); i++ )
	{
		lp = static_cast<LineProp*>(cursor.block().userData());
		if ( lp != NULL )
			lp->lineChanged = true;
		cursor.deleteChar();
		lp = static_cast<LineProp*>(cursor.block().userData());
		if ( lp != NULL )
			lp->lineChanged = true;
	}
	if ( hasSelection ) {
		lp = static_cast<LineProp*>(cursor.block().userData());
		if ( lp != NULL )
			lp->lineChanged = true;
		cursor.insertText(selText);
		lp = static_cast<LineProp*>(cursor.block().userData());
		if ( lp != NULL )
			lp->lineChanged = true;
		if ( position == selectionStart ) {
			cursor.setPosition(selectionEnd);
			for ( uint j=selectionEnd; j>selectionStart; j-- )
				cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
		}
		else {
			cursor.setPosition(selectionEnd);
			for ( uint j=selectionStart; j<selectionEnd; j++ )
				cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
		}
	}
	editor->setTextCursor ( cursor );
	if ( ke != NULL )
		ke->startListeningEvents();
}
void InsertCommand::redo()
{
	LineProp *lp = NULL;
	KumirEdit *ke = qobject_cast<KumirEdit*>(editor);
	if ( ke != NULL )
		ke->stopListeningEvents();
	QTextCursor cursor = editor->textCursor();
	if ( hasSelection ) {
		selText = cursor.selectedText();
		cursor.removeSelectedText();
	}
	
	cursor.setPosition ( position );
	cursor.insertText ( txt );
	editor->setTextCursor ( cursor );
	if ( ke != NULL )
		ke->startListeningEvents();
}

CommentCommand::CommentCommand ( TextEdit *editor, uint from, uint to )
{
	this->editor = editor;
	this->from = from;
	this->to = to;
	QTextCursor cursor = editor->textCursor();
	position = cursor.position();
	setText ( "CommentCommand" );
}

int CommentCommand::id() { return 6; }

void CommentCommand::undo()
{
	LineProp *lp = NULL;
	QTextBlock block = editor->document()->begin();
	uint i = 0;
	while ( block.isValid() && ( i<from ) )
	{
		block = block.next();
		i++;
	}
	for ( uint i = from; i <= to; i++ )
	{
		QTextCursor c ( block );
		lp = static_cast<LineProp*>(c.block().userData());
		if ( lp != NULL )
			lp->lineChanged = true;
		c.deleteChar();
		lp = static_cast<LineProp*>(c.block().userData());
		if ( lp != NULL )
			lp->lineChanged = true;
		c.deleteChar();
		lp = static_cast<LineProp*>(c.block().userData());
		if ( lp != NULL )
			lp->lineChanged = true;
		block = block.next();
	}
}

void CommentCommand::redo()
{
	LineProp *lp = NULL;
	QTextBlock block = editor->document()->begin();
	uint i = 0;
	while ( block.isValid() && ( i<from ) )
	{
		block = block.next();
		i++;
	}
	for ( uint i = from; i <= to; i++ )
	{
		QTextCursor c ( block );
		lp = static_cast<LineProp*>(c.block().userData());
		if ( lp != NULL )
			lp->lineChanged = true;
		c.insertText ( "| " );
		block = block.next();
	}
}

UncommentCommand::UncommentCommand ( TextEdit *editor, uint from, uint to )
{
	this->editor = editor;
	this->from = from;
	this->to = to;
	QTextCursor cursor = editor->textCursor();
	position = cursor.position();
	setText ( "UncommentCommand" );
}

int UncommentCommand::id() { return 7; }

void UncommentCommand::undo()
{
	LineProp *lp = NULL;
	QTextBlock block = editor->document()->begin();
	uint i = 0;
	while ( block.isValid() && ( i<from ) )
	{
		block = block.next();
		i++;
	}
	for ( uint i = from; i <= to; i++ )
	{
		QTextCursor c ( block );
		lp = static_cast<LineProp*>(c.block().userData());
		if ( lp != NULL )
			lp->lineChanged = true;
		c.insertText ( "| " );
		block = block.next();
	}
}

void UncommentCommand::redo()
{
	LineProp *lp = NULL;
	QTextBlock block = editor->document()->begin();
	uint i = 0;
	while ( block.isValid() && ( i<from ) )
	{
		block = block.next();
		i++;
	}
	for ( uint i = from; i <= to; i++ )
	{
		QTextCursor c ( block );
		if ( block.text().startsWith ( "|" ) ) {
			c.deleteChar();
			lp = static_cast<LineProp*>(c.block().userData());
			if ( lp != NULL )
				lp->lineChanged = true;
		}
		while ( block.text().startsWith ( " " ) )
		{ 
			c.deleteChar();
			lp = static_cast<LineProp*>(c.block().userData());
			if ( lp != NULL )
				lp->lineChanged = true;
		}
		block = block.next();
	}
}

SetTextCommand::SetTextCommand(TextEdit *editor, const QString & txt)
{
	this->editor = editor;
	newText = txt;
}

int SetTextCommand::id()
{
	return 8;
}

void SetTextCommand::undo()
{
	QList<LineProp> lineProps;
	for ( QTextBlock b = editor->document()->begin(); b.isValid(); b = b.next() )
	{
		if ( b.userData() != NULL )
		{
			LineProp *line = static_cast<LineProp*>(b.userData());
			line->lineChanged = true;
			lineProps << *(line);
		}
		else
		{
			LineProp line;
			line.lineChanged = true;
			lineProps << line;
		}
	}
	editor->setPlainText(oldText);
	editor->initLineProps();
	for ( QTextBlock b = editor->document()->begin(); b.isValid(); b = b.next() )
	{
		LineProp *line = static_cast<LineProp*>(b.userData());
		if ( line != NULL )
			line->lineChanged = true;
		if ( ! lineProps.isEmpty() )
		{
			*line = lineProps.first();
			lineProps.pop_front();
		}
	}
	editor->applyIndentation();
	editor->textCursor().setPosition(oldPos);
}

void SetTextCommand::redo()
{
	oldText = editor->toPlainText();
	oldPos = editor->textCursor().position();
	QList<LineProp> lineProps;
	for ( QTextBlock b = editor->document()->begin(); b.isValid(); b = b.next() )
	{
		if ( b.userData() != NULL )
		{
			LineProp *line = static_cast<LineProp*>(b.userData());
			lineProps << *(line);
		}
		else
		{
			lineProps << LineProp();
		}
	}
	editor->setPlainText(newText);
	editor->initLineProps();
	for ( QTextBlock b = editor->document()->begin(); b.isValid(); b = b.next() )
	{
		LineProp *line = static_cast<LineProp*>(b.userData());
		if ( line != NULL )
			line->lineChanged = true;
		if ( ! lineProps.isEmpty() )
		{
			*line = lineProps.first();
			lineProps.pop_front();
		}
	}
	editor->applyIndentation();
	if ( oldPos < newText.length() )
	{
		newPos = oldPos;
	}
	else
	{
		newPos = newText.length()-1;
	}
	editor->textCursor().setPosition(newPos);
}

InsertLineCommand::InsertLineCommand( TextEdit *editor, uint no, const QString & txt )
{
	this->editor = editor;
	this->txt = txt;
	lineNo = no;
}

int InsertLineCommand::id()
{
	return 9;
}

void InsertLineCommand::undo()
{
	LineProp *lp = NULL;
	KumirEdit *ke = qobject_cast<KumirEdit*>(editor);
	if ( ke != NULL )
		ke->stopListeningEvents();
	QTextBlock block = editor->document()->begin();
	for ( int i=0; i<lineNo; i++ )
		block = block.next();
	QTextCursor c(block);
	c.movePosition(QTextCursor::EndOfBlock,QTextCursor::KeepAnchor);
	c.movePosition(QTextCursor::NextCharacter,QTextCursor::KeepAnchor);
	c.removeSelectedText();
	lp = static_cast<LineProp*>(c.block().userData());
	if ( lp != NULL )
		lp->lineChanged = true;
	editor->setTextCursor(cursor);
	if ( ke != NULL )
		ke->startListeningEvents();
}

void InsertLineCommand::redo()
{
	LineProp *lp = NULL;
	KumirEdit *ke = qobject_cast<KumirEdit*>(editor);
	if ( ke != NULL )
		ke->stopListeningEvents();
	cursor = editor->textCursor();
	QTextBlock block = editor->document()->begin();
	for ( int i=0; i<lineNo; i++ )
		block = block.next();
	QTextCursor c(block);
	lp = static_cast<LineProp*>(c.block().userData());
	if ( lp != NULL )
		lp->lineChanged = true;
	c.insertText(txt+"\n");
	lp = static_cast<LineProp*>(c.block().userData());
	if ( lp != NULL )
		lp->lineChanged = true;
	if ( ke != NULL )
		ke->startListeningEvents();
}

ChangeLineTextCommand::ChangeLineTextCommand( TextEdit *editor, uint no, const QString & txt )
{
	this->editor = editor;
	this->txt = txt;
	lineNo = no;
}

int ChangeLineTextCommand::id()
{
	return 10;
}

void ChangeLineTextCommand::undo()
{
	LineProp *lp = NULL;
	KumirEdit *ke = qobject_cast<KumirEdit*>(editor);
	if ( ke != NULL )
		ke->stopListeningEvents();
	QTextBlock block = editor->document()->begin();
	for ( int i=0; i<lineNo; i++ )
		block = block.next();
	QTextCursor c(block);
	c.movePosition(QTextCursor::EndOfBlock,QTextCursor::KeepAnchor);
	lp = static_cast<LineProp*>(c.block().userData());
	if ( lp != NULL )
		lp->lineChanged = true;
	c.removeSelectedText();
	lp = static_cast<LineProp*>(c.block().userData());
	if ( lp != NULL )
		lp->lineChanged = true;
	c.insertText(oldText);
	lp = static_cast<LineProp*>(c.block().userData());
	if ( lp != NULL )
		lp->lineChanged = true;
	editor->setTextCursor(cursor);
	if ( ke != NULL )
		ke->startListeningEvents();
}

void ChangeLineTextCommand::redo()
{
	LineProp *lp = NULL;
	KumirEdit *ke = qobject_cast<KumirEdit*>(editor);
	if ( ke != NULL )
		ke->stopListeningEvents();
	cursor = editor->textCursor();
	QTextBlock block = editor->document()->begin();
	for ( int i=0; i<lineNo; i++ )
		block = block.next();
	oldText = block.text();
	QTextCursor c(block);
	c.movePosition(QTextCursor::EndOfBlock,QTextCursor::KeepAnchor);
	lp = static_cast<LineProp*>(c.block().userData());
	if ( lp != NULL )
		lp->lineChanged = true;
	c.removeSelectedText();
	lp = static_cast<LineProp*>(c.block().userData());
	if ( lp != NULL )
		lp->lineChanged = true;
	c.insertText(txt);
	lp = static_cast<LineProp*>(c.block().userData());
	if ( lp != NULL )
		lp->lineChanged = true;
	if ( ke != NULL )
		ke->startListeningEvents();
}

FastInsertTextCommand::FastInsertTextCommand(TextEdit *editor, const QString & txt, int removeCount) 
{
	this->editor = editor;
	this->txt = txt;
	this->removeCount = removeCount;
}

int FastInsertTextCommand::id()
{
	return 11;
}


void FastInsertTextCommand::redo()
{
	LineProp *lp = NULL;
	cursor = editor->textCursor();
	for ( int i=0; i<removeCount; i++ ) {
		cursor.movePosition(QTextCursor::NextCharacter,QTextCursor::KeepAnchor);
	}
	if ( cursor.hasSelection() ) {
		removeTxt = editor->toPlainText().mid(cursor.selectionStart(), cursor.selectionEnd()-cursor.selectionStart());
		lp = static_cast<LineProp*>(cursor.block().userData());
		if ( lp != NULL )
			lp->lineChanged = true;
		cursor.removeSelectedText();
		lp = static_cast<LineProp*>(cursor.block().userData());
		if ( lp != NULL )
			lp->lineChanged = true;
	}
	lp = static_cast<LineProp*>(cursor.block().userData());
	if ( lp != NULL )
		lp->lineChanged = true;
	cursor.insertText(txt);
	lp = static_cast<LineProp*>(cursor.block().userData());
	if ( lp != NULL )
		lp->lineChanged = true;
}

void FastInsertTextCommand::undo()
{
	LineProp *lp = NULL;
	editor->setTextCursor(cursor);
	for ( int i=0; i<txt.length(); i++ ) {
		editor->textCursor().movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
	}
	lp = static_cast<LineProp*>(cursor.block().userData());
	if ( lp != NULL )
		lp->lineChanged = true;
	editor->textCursor().removeSelectedText();
	lp = static_cast<LineProp*>(cursor.block().userData());
	if ( lp != NULL )
		lp->lineChanged = true;
	editor->textCursor().insertText(removeTxt);
	lp = static_cast<LineProp*>(cursor.block().userData());
	if ( lp != NULL )
		lp->lineChanged = true;
}
