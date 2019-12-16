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

void Pult::createButtons()
{
    QSize sizet(60, 60), sizeb(90, 30);

    QString s;
    QString s1 = "", s2 = "  ", s3 = "    ";

    s = QString("Q");
    createButton(&hr,    sizet, "", ":/images/headright.png",   QKeySequence(QString(s)));
    s1 += QString(s) + ", ";
    s = QString("W");
    createButton(&hu,    sizet, "", ":/images/headup.png",      QKeySequence(QString(s)));
    s1 += QString(s) + ", ";
    s = QString("E");
    createButton(&hl,    sizet, "", ":/images/headleft.png",    QKeySequence(QString(s)));
    s1 += QString(s) + ", ";
    s = QString("R");
    createButton(&k1, sizet, "", ":/images/k1.png", QKeySequence(QString(s)));
    s1 += QString(s);
    l1.setText(s1);

    s = QString("A");
    createButton(&rfd,   sizet, "", ":/images/rfootdown.png",   QKeySequence(QString(s)));
    s2 += QString(s) + ", ";
    s = QString("S");
    createButton(&wt,    sizet, "", ":/images/wait.png",        QKeySequence(QString(s)));
    s2 += QString(s) + ", ";
    s = QString("D");
    createButton(&rfu,   sizet, "", ":/images/rfootup.png",     QKeySequence(QString(s)));
    s2 += QString(s) + ", ";
    s = QString("F");
    createButton(&nfo,  sizet, "Reset K1", "",              QKeySequence(QString(s)));
    s3 += QString(s);

    l2.setText(s2);

    s = QString("Z");
    createButton(&lfu,   sizet, "", ":/images/lfootup.png",     QKeySequence(QString(s)));
    s3 += QString(s) + ", ";
    s = QString("X");
    createButton(&stp,   sizet, "", ":/images/stop.png",        QKeySequence(QString(s)));
    s3 += QString(s) + ", ";
    s = QString("C");
    createButton(&lfd,   sizet, "", ":/images/lfootdown.png",   QKeySequence(QString(s)));
    s3 += QString(s);
/*
    s = QString("V");
    createButton(&take, sizet, "", ":/images/take.png", QKeySequence(QString(s)));
    s2 += QString(s);
*/
    l3.setText(s3);

    createButton(&rst,  sizet, "", ":/images/reset.png",    Qt::CTRL + Qt::Key_R);
    createButton(&opn,  sizet, "", ":/images/open.png",     Qt::CTRL + Qt::Key_O);
}

void Pult::createLayouts()
{
    hl1.addWidget(&hr);
    hl1.addWidget(&hu);
    hl1.addWidget(&hl);
    hl1.addWidget(&k1);

    hl2.addWidget(&rfd);
    hl2.addWidget(&wt);
    hl2.addWidget(&rfu);
    hl2.addWidget(&nfo);

    hl3.addWidget(&lfu);
    hl3.addWidget(&stp);
    hl3.addWidget(&lfd);
    hl3.addWidget(&rst);

    vl1.addWidget(&l1);
    vl1.addWidget(&l2);
    vl1.addWidget(&l3);
    hl4.addWidget(&opn);
    hl4.addLayout(&vl1);

    vl.addLayout(&hl1);
    vl.addLayout(&hl2);
    vl.addLayout(&hl3);
    vl.addLayout(&hl4);
    vl.addWidget(&label);
    vl.addStretch(1);
}


Pult::Pult(DwunogWorld *world):world(world)
{
    createButtons();
    createLayouts();

    setLayout (&vl);
    this->setWindowTitle(QString::fromUtf8("Пульт"));
    connect (&opn,  SIGNAL(clicked()), this, SLOT(open()));
    connect (&hl,   SIGNAL(clicked()), this, SLOT(headLeft()));
    connect (&hr,   SIGNAL(clicked()), this, SLOT(headRight()));
    connect (&hu,   SIGNAL(clicked()), this, SLOT(headUp()));
    connect (&lfu,  SIGNAL(clicked()), this, SLOT(lFootUp()));
    connect (&lfd,  SIGNAL(clicked()), this, SLOT(lFootDown()));
    connect (&rfu,  SIGNAL(clicked()), this, SLOT(rFootUp()));
    connect (&rfd,  SIGNAL(clicked()), this, SLOT(rFootDown()));
    connect (&wt,   SIGNAL(clicked()), this, SLOT(wait()));
    connect (&rst,  SIGNAL(clicked()), this, SLOT(reset()));
    connect (&nfo,  SIGNAL(clicked()), this, SLOT(getInfo()));
    connect (&stp,  SIGNAL(clicked()), this, SLOT(sstop()));
    connect (&k1,   SIGNAL(clicked()), this, SLOT(dok1()));
    connect (&take, SIGNAL(clicked()), this, SLOT(dotake()));

    k1set = false;
}

void Pult::dok1()
{
    world->doCommand(QString("k1"));
}

void Pult::dotake()
{
    world->doCommand(QString("take"));
}

const int ANGLE = 360;
void Pult::headLeft()
{
    world->doCommand(QString("hturn"), ANGLE);
}

void Pult::headRight()
{
    world->doCommand(QString("hturn"), -ANGLE);
}

void Pult::headUp()
{
    world->doCommand(QString("headup"));
}

void Pult::lFootUp()
{
    world->doCommand(QString("lfturn"), -ANGLE);
}

void Pult::lFootDown()
{
    world->doCommand(QString("lfturn"), ANGLE);
}

void Pult::rFootUp()
{
    world->doCommand(QString("rfturn"), ANGLE);
}

void Pult::rFootDown()
{
    world->doCommand(QString("rfturn"), -ANGLE);
}

void Pult::wait()
{
    world->doCommand(QString("wait"), ANGLE);
}

void Pult::reset()
{
    world->doCommand(QString("reset"), ANGLE);
}

void Pult::getInfo()
{
    if (k1set) {
        k1.setIcon(QIcon(":/images/k1"));
        nfo.setText("Reset K1");
        k1set = false;
    }
    else {
        k1.setIcon(QIcon(":/images/reck1"));
        nfo.setText("Ready!");
        k1set = true;
    }
    world->doCommand(QString("savek1"), ANGLE);
}

void Pult::open()
{
    QString filename = QFileDialog::getOpenFileName(this,
                                 "Choose file...",
                                 "default.def",
                                 "Dwunog Editor Files (*.def);;Text Files (*.txt)"
                                 );
    world->openf(filename);
}


void Pult::setLabelText(const QString &text)
{
    label.setText(text);
}


void Pult::sstop()
{
    world->doCommand(QString("stop"));
}

Pult::~Pult()
{
    qDebug("Pult destructor\n");
}
