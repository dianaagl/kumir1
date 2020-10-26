//
// C++ Implementation: printDialog
//
// Description: 
//
//
// Author: Denis Khachko <mordol@mail.ru>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "dialog.h"




Dialog::Dialog(QWidget* parent, Qt::WindowFlags fl ): QDialog ( parent, fl ), Ui::Dialog()
{
setupUi(this);
connect(BaseA,SIGNAL(valueChanged ( int  )),this,SLOT(setMax()));
connect(BaseB,SIGNAL(valueChanged ( int  )),this,SLOT(setMax()));
connect(pushButton,SIGNAL(clicked(   )),this,SLOT(addFlag()));
connect(pushButton_2,SIGNAL(clicked(   )),this,SLOT(removeFlag()));
connect(listWidget,SIGNAL(itemClicked (QListWidgetItem*)),this,SLOT(enableDelete()));
connect(spinBox,SIGNAL(valueChanged ( int)),this,SLOT(setMax()));
connect(StartPos,SIGNAL(valueChanged ( int)),this,SLOT(setMax()));
connect(spinBox_2,SIGNAL(valueChanged ( int)),this,SLOT(setMax()));
NeedA->setEnabled(true);

};
void Dialog::setMax()
 {


		if(spinBox_2->value()>StartPos->value())
			spinBox->setMaximum(spinBox_2->value());
		else
			spinBox->setMaximum(StartPos->value());

		spinBox_2->setMinimum(spinBox->value()+1);

	
 };

void Dialog::addFlag()
  {
    QList<QListWidgetItem *> flags=listWidget->findItems(NeedA->cleanText(),Qt::MatchExactly);
    if(flags.count()>0)return;
    listWidget->addItem(NeedA->cleanText()); 
  };
void Dialog::enableDelete()
 {
	pushButton_2->setEnabled(true);
 };
void Dialog::removeFlag()
 {
 QList<QListWidgetItem *>  selectedItems=listWidget->selectedItems ();
	qDebug()<<"Item Count"<<selectedItems.count();
	for(int i=0;i<selectedItems.count();i++)listWidget->takeItem (listWidget->row(selectedItems[i]));
 if(listWidget->selectedItems ().count()==0)pushButton_2->setEnabled(false);
 };
