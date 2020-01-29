#include "newkursdialog.h"
#include "ui_newkursdialog.h"
#include <QFileDialog>

NewKursDialog::NewKursDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::NewKursDialog)
{
	ui->setupUi(this);
}

NewKursDialog::~NewKursDialog()
{
	delete ui;
	ui = 0;
}

QString NewKursDialog::name()
{
	return ui->nameEdit->text();
}

void NewKursDialog::showFileSelect()
{
	QFileDialog dialog(
		this,
		trUtf8("Cохранить файл курса"),
		"", "(*.kurs.xml)"
	);
	dialog.setAcceptMode(QFileDialog::AcceptSave);
	if (!dialog.exec()) {
		return;
	}

	QString fname = dialog.selectedFiles().first();
	if (fname.right(9) != ".kurs.xml") {
		fname = fname + ".kurs.xml";
	}
}

