#ifndef NEWKURSDIALOG_H
#define NEWKURSDIALOG_H
#include <QDialog>

namespace Ui
{
class NewKursDialog;
}

class NewKursDialog : public QDialog
{
	Q_OBJECT

public:
	explicit NewKursDialog(QWidget *parent = 0);
	~NewKursDialog();

	QString name();

public slots:
void showFileSelect();

private:
	Ui::NewKursDialog *ui;
};

#endif // NEWKURSDIALOG_H
