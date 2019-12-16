#include "pult.h"

void Pult::createButton(QToolButton *bt, QSize size, QString text,
                        QString addr, QKeySequence key)
{
    bt->setFixedSize(size);
    bt->setShortcut(key);
    if (text == "") {
        bt->setIcon(QIcon(addr));
        bt->setIconSize(size);
    }
    else bt->setText(text);
}

void Pult::createSpin(QSpinBox *sp, int up, int down, int val)
{
    sp->setValue(val);
    sp->setRange(down, up);
}

void Pult::createButtons()
{
    QSize sizet(60, 60), sizeb(90, 30);

    createButton(&ml, sizet, "", ":/images/ml.png", Qt::Key_Left);
    createButton(&mr, sizet, "", ":/images/mr.png", Qt::Key_Right);
    createButton(&mu, sizet, "", ":/images/mu.png", Qt::Key_Up);
    createButton(&md, sizet, "", ":/images/md.png", Qt::Key_Down);

    createButton(&open, sizet, "", ":/images/open.PNG", Qt::CTRL + Qt::Key_O);
    createButton(&save, sizet, "", ":/images/save.PNG", Qt::CTRL + Qt::Key_S);

    createButton(&iswall,   sizeb, "Is &wall?",      "");
    createButton(&isfilled, sizeb, "&Is filled?",    "");
    createButton(&setwall,  sizeb, "&Set wall",      "");
    createButton(&fill,     sizeb, "&Fill",          "");
    createButton(&setparam, sizeb, "Set &param",     "");
    createButton(&setsize,  sizeb, "Set si&ze",      "");

    createButton(&isdotted, sizeb, "Is dotted?",    "");
    createButton(&dot,      sizeb, "Dot",           "");
    createButton(&settext,  sizeb, "Set text",      "");
    createButton(&settemp,  sizeb, "Set temp",      "");
    createButton(&setrad,   sizeb, "Set rad",       "");

    createButton(&text,   sizeb, "Text",       "");
    createButton(&temp,   sizeb, "Temp",       "");
    createButton(&rad,    sizeb, "Rad",       "");

    //radio
    rl.setText("&cyan");
    rr.setText("&yellow");
    ru.setText("&red");
    rd.setText("&blue");
    rl.setChecked(1);

    //spin
    createSpin(&spnx, 1000, 0, 10);
    createSpin(&spny, 1000, 0, 10);
    createSpin(&spxs, spnx.value(), 0, 0);
    createSpin(&spys, spny.value(), 0, 0);
    createSpin(&spxl, 1000, 0, spnx.value());
    createSpin(&spyl, 1000, 0, spny.value());

}


void Pult::createLayouts()
{
    hl1.addWidget(&ml);
    hl1.addWidget(&mr);
    hl3.addWidget(&mu);
    hl4.addWidget(&md);

    vl1.addWidget(&ru);
    vl1.addWidget(&rd);
    vl1.addWidget(&rl);
    vl1.addWidget(&rr);

    vl2.addWidget(&iswall);
    vl2.addWidget(&setwall);

    hl2.addLayout(&vl1);
    hl2.addLayout(&vl2);

    hl5.addWidget(&open);
    hl5.addWidget(&save);

    hl6.addWidget(&spnx);
    hl6.addWidget(&spny);
    hl7.addWidget(&spxs);
    hl7.addWidget(&spys);
    hl7.addWidget(&spxl);
    hl7.addWidget(&spyl);

    hl8.addWidget(&setparam);
    hl8.addWidget(&setsize);


    hl13.addWidget(&isfilled);
    hl13.addWidget(&fill);
    hl14.addWidget(&isdotted);
    hl14.addWidget(&dot);

    vl.addLayout(&hl3);
    vl.addLayout(&hl1);
    vl.addLayout(&hl4);
    vl.addLayout(&hl2);

    hl10.addWidget(&temp);
    hl10.addWidget(&settemp);
    hl11.addWidget(&rad);
    hl11.addWidget(&setrad);
    hl12.addWidget(&text);
    hl12.addWidget(&settext);

    vl.addLayout(&hl13);
    vl.addLayout(&hl14);
    vl.addLayout(&hl10);
    vl.addLayout(&hl11);
    vl.addLayout(&hl12);

    vl.addWidget(&edit);

    vl.addWidget(&label);
    vl.addLayout(&hl5);
    vl.addLayout(&hl8);
    vl.addLayout(&hl6);
    vl.addLayout(&hl7);

    cmb1.addItem(QIcon(), "Klein");
    cmb1.addItem(QIcon(), "Torus");
    vl.addWidget(&cmb1);


}


Pult::Pult(Robot *rob)
{
    robot = rob;

    createButtons();
    createLayouts();

    label.setText("Status label");
    setLayout (&vl);

    connect (&ml,    SIGNAL(clicked()), robot, SLOT(moveLeft()));
    connect (&mr,    SIGNAL(clicked()), robot, SLOT(moveRight()));
    connect (&mu,    SIGNAL(clicked()), robot, SLOT(moveUp()));
    connect (&md,    SIGNAL(clicked()), robot, SLOT(moveDown()));
    connect (&fill, SIGNAL(clicked()), robot, SLOT(fill()));

    connect (&open, SIGNAL(clicked()), SLOT(openFile()));
    connect (&save, SIGNAL(clicked()), SLOT(saveFile()));

    connect (&isfilled, SIGNAL(clicked()), SLOT(isFilled()));
    connect (&setwall,  SIGNAL(clicked()), SLOT(setWall()));
    connect (&iswall, SIGNAL(clicked()), SLOT(isWall()));

    connect (&setparam, SIGNAL(clicked()), this, SLOT(setParam()));
    connect (&setsize, SIGNAL(clicked()), this, SLOT(setSize()));
    connect (&cmb1, SIGNAL(currentIndexChanged(const QString&)),
                this, SLOT(typeChanged(const QString&)));

    connect (&isdotted, SIGNAL(clicked()), this, SLOT(isDotted()));
    connect (&dot, SIGNAL(clicked()), robot, SLOT(dot()));
    connect (&settemp, SIGNAL(clicked()), this, SLOT(setTemp()));
    connect (&setrad, SIGNAL(clicked()), this, SLOT(setRad()));
    connect (&settext, SIGNAL(clicked()), this, SLOT(setText()));

    connect (&temp, SIGNAL(clicked()), this, SLOT(showTemp()));
    connect (&rad, SIGNAL(clicked()), this, SLOT(showRad()));
    connect (&text, SIGNAL(clicked()), this, SLOT(showText()));

    this->setWindowTitle(QString::fromUtf8("Роботор пульт"));
}


void Pult::isDotted()
{
    if (robot->isDotted()) setLabelText(QString("Yes"));
    else setLabelText(QString("No"));
}

void Pult::setTemp()
{
    bool res;
    int t = edit.text().toInt(&res);
    if (!res)
        setLabelText(QString("Bad input"));
    else robot->setTemp(t);
}

void Pult::setRad()
{
    bool res;
    int t = edit.text().toInt(&res);
    if (!res)
        setLabelText(QString("Bad input"));
    else robot->setRad(t);
}

void Pult::showTemp()
{
    setLabelText(QString::number(robot->temp()));
}

void Pult::showText()
{
    setLabelText(robot->text());
}

void Pult::showRad()
{
    setLabelText(QString::number(robot->rad()));
}

void Pult::setText()
{
    robot->setText(edit.text());
}

void Pult::typeChanged(const QString& string)
{
    if (string == "Klein") robot->setType(Robot::klein);
    if (string == "Torus") robot->setType(Robot::torus);
}

void Pult::isWall()
{
    bool res;
    if (rl.isChecked()) res = robot->wallLeft();
    else if (rr.isChecked()) res = robot->wallRight();
    else if (ru.isChecked()) res = robot->wallUp();
    else res = robot->wallDown();
    if (res) setLabelText(QString("Yes"));
    else setLabelText(QString("No"));
}

void Pult::setWall()
{
    if (rl.isChecked())         robot->setWallLeft();
    else if (rr.isChecked())    robot->setWallRight();
    else if (ru.isChecked())    robot->setWallUp();
    else                        robot->setWallDown();
}

void Pult::isFilled()
{
    if (robot->isFilled()) setLabelText(QString("Yes"));
    else setLabelText(QString("No"));
}

void Pult::setLabelText(const QString &text)
{
    label.setText(text);
}

void Pult::openFile()
{
    QString name = QFileDialog::getOpenFileName(this, "Open...", "./",
                                                "Robot files(*.fil);;Text files(*.txt)");
    int res = robot->openFile(name);
    qDebug() << res;
}

void Pult::saveFile()
{
    QString name = QFileDialog::getSaveFileName(this, "Save...", "./", "*.fil");
    robot->saveFile(name);
}

void Pult::setParam()
{
    robot->setViewPart(spxs.value(), spys.value(), spxl.value(), spyl.value());
}

void Pult::setSize()
{
    robot->setSize(spnx.value(), spny.value());
}
