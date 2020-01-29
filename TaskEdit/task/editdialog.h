#ifndef EDITDIALOG_H
#define EDITDIALOG_H

#include <QStringList>
#include <QDialog>
#include <QListWidgetItem>

namespace Ui
{
class EditDialog;
}

class EditDialog : public QDialog
{
	Q_OBJECT

public:
	explicit EditDialog(QWidget *parent = 0);
	~EditDialog();
	void setTitle(QString title);
	void setDesc(QString dec);
	void setProgram(QString prg);
	void setUseIsps(QString isp);
	void setEnvs(QStringList);

	QString getTitle();
	QString getDesc();
	QString getProgram();
	QStringList getUseIsps();
	QStringList getEnvs();
	void setCurDir(QString dir) { curDir = dir; }

public slots:
	void setPrg();
	void removeEnv();
	void addEnv();

private:
	QStringList items2StringList(QList<QListWidgetItem *> inp);

private:
	Ui::EditDialog *ui;
	QString curDir;

};

#endif // EDITDIALOG_H
