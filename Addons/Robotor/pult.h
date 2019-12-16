#ifndef PULT_H
#define PULT_H

#include <QtGui>
//#include <QTcpSocket>

class QLabel;
class QWidget;
class QSpinBox;
class QTcpSocket;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;

#include "robot.h"
class Robot;

class Pult : public QWidget
{
    Q_OBJECT

public:
    Pult(Robot *rob);
//    ~Pult();

signals:

public slots:
    void setLabelText(const QString &text);

private slots:
    void typeChanged(const QString&);
    void setWall();
    void setParam();
    void setSize();
    void isWall();
    void isFilled();
    void isDotted();
    void setText();
    void setRad();
    void showTemp();
    void showRad();
    void showText();
    void setTemp();
    void openFile();
    void saveFile();

private:

    void createButton(QToolButton *bt, QSize size, QString text, QString addr,
                      QKeySequence key = 0);
    void createSpin(QSpinBox *sp, int up, int down, int val);

    void createButtons();
    void createLayouts();

    Robot *robot;
    QToolButton ml, mr, mu, md, open, save,
                iswall, isfilled, fill, setwall, setsize, setparam;

    QToolButton isdotted, dot, settext, settemp, setrad, temp, rad, text;
    QVBoxLayout vl, vl1, vl2, vl3, vl4, vl5, vl6;
    QHBoxLayout hl1, hl2, hl3, hl4, hl5, hl6, hl7, hl8, hl9, hl10, hl11, hl12, hl13, hl14;
    QRadioButton rl, ru, rd, rr;
    QSpinBox spxs, spys, spxl, spyl, spnx, spny;
    QComboBox cmb1;
    QLineEdit edit;

    QLabel      label;

};

#endif
