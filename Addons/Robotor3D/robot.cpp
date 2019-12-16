#include "robot.h"
#include "fstream"
using namespace std;

Robot::Robot(QWidget *parent)
    : QWidget(parent)
{

    field = new Point[1];
    rfield = new Point[1];

    x = 0; rx = 0;
    y = 0; ry = 0;
    len = 40;
    lastPos = QPoint (0, 0);
    type = torus;

    setSize(3, 3);
    setViewPart(0, 0, 3, 3);

    this->resize(len * (xlen + 1), len * (ylen + 1));
    this->setWindowTitle(QString::fromUtf8("Роботор Плоскость"));
    update();

    tleft   =	QString::fromUtf8("влево");
    tright  =	QString::fromUtf8("вправо");
    tup	    =	QString::fromUtf8("вверх");
    tdown   =	QString::fromUtf8("вниз");

    tfill   =	QString::fromUtf8("закрасить");
    tfilled =	QString::fromUtf8("закрашено");
    tnfilled=	QString::fromUtf8("чисто");
    tunfill=	QString::fromUtf8("очистить");

    twleft  =	QString::fromUtf8("слева стена");
    twright =	QString::fromUtf8("справа стена");
    twup    =	QString::fromUtf8("сверху стена");
    twdown  =	QString::fromUtf8("снизу стена");

    tnwleft  =	QString::fromUtf8("слева свободно");
    tnwright =	QString::fromUtf8("справа свободно");
    tnwup    =	QString::fromUtf8("сверху свободно");
    tnwdown  =	QString::fromUtf8("снизу свободно");

    ttemp  =	QString::fromUtf8("температура");
    trad  =	QString::fromUtf8("радиация");
    ttext  =	QString::fromUtf8("текст");



    tsuc    =	QString::fromUtf8("ок");
    tdec    =	QString::fromUtf8("отказ");
    tyes    =	QString::fromUtf8("да");
    tno	    =	QString::fromUtf8("нет");


}

QString Robot::declineText() const
{
    return tdec;
}


void Robot::setType(TYPE newtype)
{
    if (type != newtype) {
	type = newtype;
        if (type == cyllinder || type == mebius) {
            for (int i = 0; i < nx; i++) {
                field[i].wallUp = 1;
                field[nx*ny - 1 - i].wallDown = 1;
            }
        }
	emit update3D();
	update();
    }
}

void Robot::setSize(int xn, int yn)
{
    nx = xn;
    ny = yn;
//    if (field) delete [] field;
    field = new Point[nx*ny];
    for (int i = 0; i < nx*ny; i++) {
        field[i].wallUp    = 0;
        field[i].wallDown  = 0;
        field[i].wallLeft  = 0;
        field[i].wallRight = 0;
        field[i].filled    = 0;
        field[i].dotted    = 0;
        field[i].temp      = 0;
        field[i].rad       = 0;
        field[i].text      = "";
    }

    if (type == cyllinder || type == mebius) {
        for (int i = 0; i < nx; i++) {
            field[i].wallUp = 1;
            field[nx*ny - 1 - i].wallDown = 1;
        }
    }

//    if (rfield) delete [] rfield;
    rfield = new Point[nx*ny];
    for (int i = 0; i < nx*ny; i++)
        rfield[i] = field[i];

    this->resize((nx + 1)*len, (ny + 1)*len);
    update();
}

void Robot::setViewPart(int xs, int ys, int xl, int yl)
{
    xlen = xl;
    ylen = yl;
    xstart = xs;
    ystart = ys;
    this->resize(len*(xlen + 1), len*(ylen + 1));
    update();
}

//coordinate translation
//returns inversion (1 - klein, mebius, 0 - torus, plane, cyllinder)
int Robot::f(int i, int j, int *p, int *q)
{
    *p = (i%ny) + (i<0)*ny;
    *q = (j%nx) + (j<0)*nx;
    if (type == klein || type == mebius) {
        if ( (j < 0) || ((j/nx)%2) != 0 ) {
            *p = ny - 1 - *p;
            return 1;
        }
        return 0;
    }

    return 0;
}


// MOVEMENT


void Robot::moveLeft()
{
    if (field[y*nx + x].wallLeft) {
	emit result(tleft, tleft, tdec);
        return;
    }
    f(y, x-1, &y, &x);
    emit result(tleft, tleft, tsuc);
    update();
}

void Robot::moveRight()
{
    if (field[y*nx + x].wallRight) {
	emit result(tright, tright, tdec);
        return;
    }
    f(y, x+1, &y, &x);
    emit result(tright, tright, tsuc);
    update();
}

void Robot::moveUp()
{
    if (field[y*nx + x].wallUp) {
	emit result(tup, tup, tdec);
        return;
    }
    f(y-1, x, &y, &x);
    emit result(tup, tup, tsuc);
    update();
}

void Robot::moveDown()
{
    if (field[y*nx + x].wallDown) {
	emit result(tdown, tdown, tdec);
        return;
    }
    f(y+1, x, &y, &x);
    emit result(tdown, tdown, tsuc);
    update();
}


// FILLS & BORDERS


void Robot::fill()
{
    if (field[y*nx + x].filled) {
        field[y*nx + x].filled = 0;
	emit result(tfill, tunfill, tsuc);
    }
    else {
        field[y*nx + x].filled = 1;
	emit result(tfill, tfill, tsuc);
    }
    update();
}

void Robot::dot()
{
    if (field[y*nx + x].dotted) {
        field[y*nx + x].dotted = 0;
//        emit sendmsg(QString::fromUtf8("Метка снята"));
    }
    else {
        field[y*nx + x].dotted = 1;
//        emit sendmsg(QString::fromUtf8("Метка установлена"));
    }
    update();
}


void Robot::setWallLeft()
{
    int p, q;
    f(y, x-1, &p, &q);
    if (wallLeft()) {
        field[y*nx + x].wallLeft = 0;
        field[p*nx + q].wallRight = 0;
        emit sendmsg(QString::fromUtf8("Стена снята"));
    }
    else {
        field[y*nx + x].wallLeft = 1;
        field[p*nx + q].wallRight = 1;
        emit sendmsg(QString::fromUtf8("Стена установлена"));
    }
    update();
}

void Robot::setWallRight()
{
    int p, q;
    f(y, x+1, &p, &q);
    if (wallRight()) {
        field[y*nx + x].wallRight = 0;
        field[p*nx + q].wallLeft = 0;
        emit sendmsg(QString::fromUtf8("Стена снята"));
    }
    else {
        field[y*nx + x].wallRight = 1;
        field[p*nx + q].wallLeft = 1;
        emit sendmsg(QString::fromUtf8("Стена установлена"));
    }
    update();
}

void Robot::setWallUp()
{
    int p, q;
    f(y - 1, x, &p, &q);
    if (wallUp()) {
        field[y*nx + x].wallUp = 0;
        field[p*nx + q].wallDown = 0;
        emit sendmsg(QString::fromUtf8("Стена снята"));
    }
    else {
        field[y*nx + x].wallUp = 1;
        field[p*nx + q].wallDown = 1;
        emit sendmsg(QString::fromUtf8("Стена установлена"));
    }
    update();
}

void Robot::setWallDown()
{
    int p, q;
    f(y+1, x, &p, &q);
    if (wallDown()) {
        field[y*nx + x].wallDown = 0;
        field[p*nx + q].wallUp = 0;
        emit sendmsg(QString::fromUtf8("Стена снята"));
    }
    else {
        field[y*nx + x].wallDown = 1;
        field[p*nx + q].wallUp = 1;
        emit sendmsg(QString::fromUtf8("Стена установлена"));
    }
    update();
}

void Robot::setTemp(int val)
{
    field[y*nx + x].temp = val;
}

void Robot::setRad(int val)
{
    field[y*nx + x].rad = val;
}

void Robot::setText(const QString &val)
{
    field[y*nx + x].text = val;
    update();
}



bool Robot::isFilled()
{
    if (field[y*nx + x].filled) emit result(tfilled, tfilled, tyes);
    else emit result(tfilled, tfilled, tno);
    return field[y*nx + x].filled;
}

bool Robot::isClean()
{
    if (!field[y*nx + x].filled) emit result(tnfilled, tnfilled, tyes);
    else emit result(tnfilled, tnfilled, tno);
    return !field[y*nx + x].filled;
}

bool Robot::isDotted()
{
    return field[y*nx + x].dotted;
}

bool Robot::wallUp()
{
    if (field[y*nx + x].wallUp) emit result(twup, twup, tyes);
    else emit result(twup, twup, tno);
    return field[y*nx + x].wallUp;
}

bool Robot::wallDown()
{
    if (field[y*nx + x].wallDown) emit result(twdown, twdown, tyes);
    else emit result(twdown, twdown, tno);
    return field[y*nx + x].wallDown;
}

bool Robot::wallLeft()
{
    if (field[y*nx + x].wallLeft) emit result(twleft, twleft, tyes);
    else emit result(twleft, twleft, tno);
    return field[y*nx + x].wallLeft;
}

bool Robot::wallRight()
{
    if (field[y*nx + x].wallRight) emit result(twright, twright, tyes);
    else emit result(twright, twright, tno);
    return field[y*nx + x].wallRight;
}

bool Robot::noWallUp()
{
    if (!field[y*nx + x].wallUp) emit result(twup, tnwup, tyes);
    else emit result(twup, tnwup, tno);
    return !field[y*nx + x].wallUp;
}

bool Robot::noWallDown()
{
    if (!field[y*nx + x].wallDown) emit result(twdown, tnwdown, tyes);
    else emit result(twdown, tnwdown, tno);
    return !field[y*nx + x].wallDown;
}

bool Robot::noWallLeft()
{
    if (!field[y*nx + x].wallLeft) emit result(twleft, tnwleft, tyes);
    else emit result(twleft, tnwleft, tno);
    return !field[y*nx + x].wallLeft;
}

bool Robot::noWallRight()
{
    if (!field[y*nx + x].wallRight) emit result(twright, tnwright, tyes);
    else emit result(twright, tnwright, tno);
    return !field[y*nx + x].wallRight;
}



int Robot::temp()
{
    emit result(ttemp, ttemp, QString::number(field[y*nx + x].temp));
    return field[y*nx + x].temp;
}

int Robot::rad()
{
    emit result(trad, trad, QString::number(field[y*nx + x].rad));
    return field[y*nx + x].rad;
}

QString Robot::text()
{
    emit result(ttext, ttext, field[y*nx + x].text);
    return field[y*nx + x].text;
}

// OPEN & SAVE


enum {LEFT_WALL     = 0x1,
      RIGHT_WALL    = 0x2,
      DOWN_WALL     = 0x4,
      UP_WALL       = 0x8};

int Robot::saveFile(const QString &name)
{
    QFile f(name.trimmed());
    if (!f.open(QIODevice::WriteOnly|QIODevice::Text))
        return 1;

    QString s, s1;
    f.write("; Field size: x, y; figure\n");
    if (type == klein) s1 = "klein";
    else if (type == torus) s1 = "torus";
    else s1 = "cyllinder";
    s.sprintf("%d %d %s\n", nx, ny, qPrintable(s1));
    f.write(qPrintable(s));

    f.write("; Robot pos: x, y\n");
    s.sprintf("%d %d\n", x, y);
    f.write(qPrintable(s));

    f.write("; A set of special Fields: x, y, Wall, Color, Radiation, Temperature, Symbol, Symbol1, Point\n");
    for (int i = 0; i < nx*ny; i++)
        if (field[i].wallUp || field[i].wallDown || field[i].wallLeft ||
            field[i].wallRight || field[i].filled || field[i].dotted ||
            field[i].temp != 0 || field[i].rad != 0 || field[i].text != "") {
        int walls = 0;
        if (field[i].wallUp) walls = walls | UP_WALL;
        if (field[i].wallDown) walls = walls | DOWN_WALL;
        if (field[i].wallLeft) walls = walls | LEFT_WALL;
        if (field[i].wallRight) walls = walls | RIGHT_WALL;
        qDebug("walls = %d\n", walls);
        s1 = field[i].text;
        if (s1 == "") s1 = "$";
        s.sprintf("%d %d %d %d %lf %lf %s %s %d\n", i%nx, i/nx, walls,
                  field[i].filled, field[i].temp, field[i].rad,
                  qPrintable(s1), "$", field[i].dotted);
        qDebug("s = %s\n", qPrintable(s));
        f.write(qPrintable(s));
    }
    f.close();

    for (int i = 0; i < nx*ny; i++)
	rfield[i] = field[i];
    rx = x;
    ry = y;

    return 0;
}

int Robot::openFile(const QString &name)
{
    QFile f(name.trimmed());
    if (!f.open(QIODevice::ReadOnly|QIODevice::Text))
        return 1;

    bool sizeRead = false;
    bool roboPosRead = false;
    QStringList lines = QString::fromAscii(f.readAll().data()).split('\n');
    f.close();

    foreach (QString line, lines) {
        if (line.trimmed().startsWith(";") || line.trimmed().isEmpty())
            continue;
        if (!sizeRead) {
            QStringList sVals = line.split(QRegExp("\\s+"));
            if (sVals.count()<2)
                return 1;
            bool ok;
            nx = sVals.at(0).toInt(&ok);
            if (!ok)
                return 1;
            ny = sVals.at(1).toInt(&ok);
            if (!ok)
                return 1;

            if (sVals.count() > 2) {
                if (sVals.at(2) == "klein") type = klein;
                else if (sVals.at(2) == "torus") type = torus;
                else if (sVals.at(2) == "cyllinder") type = cyllinder;
            }
            else type = plane;
            sizeRead = true;
            setSize(nx, ny);
        }
        else if (!roboPosRead) {
            QStringList sVals = line.split(QRegExp("\\s+"));
            if (sVals.count()<2)
                return 1;
            bool ok;
            x = sVals[0].toInt(&ok);
            if (!ok)
                return 1;
            y = sVals[1].toUInt(&ok);
            if (!ok)
                return 1;
            roboPosRead = true;
        }
        else {
            QStringList sVals = line.split(QRegExp("\\s+"));
            int numparams = sVals.count();
            if (numparams < 4)
                return 1;
            bool ok;
            int xx = sVals[0].toInt(&ok);
            if (!ok || xx < 0 || xx >= nx)
                return 1;
            int yy = sVals[1].toInt(&ok);
            if (!ok || yy < 0 || yy >= ny)
                return 1;
            int wall = sVals[2].toInt(&ok);
            if (!ok)
                return 1;
            bool wallUp     = ((wall&UP_WALL) == UP_WALL);
            bool wallDown   = ((wall&DOWN_WALL) == DOWN_WALL);
            bool wallLeft   = ((wall&LEFT_WALL) == LEFT_WALL);
            bool wallRight  = ((wall&RIGHT_WALL) == RIGHT_WALL);

            field[yy*nx + xx].wallUp      |= wallUp;
            field[yy*nx + xx].wallDown    |= wallDown;
            field[yy*nx + xx].wallLeft    |= wallLeft;
            field[yy*nx + xx].wallRight   |= wallRight;
            field[yy*nx + xx].filled = (sVals[3] != "0");
            field[((yy - 1 + ny)%ny)*nx + xx].wallDown =
                    field[yy*nx + xx].wallUp;
            field[((yy + 1)%ny)*nx + xx].wallUp =
                    field[yy*nx + xx].wallDown;
            field[yy*nx + (xx + 1) % nx].wallLeft =
                  field[yy*nx + xx].wallRight;
            field[yy*nx + (xx - 1 + nx) % nx].wallRight =
                  field[yy*nx + xx].wallLeft;

            if (numparams > 4) {
                double temp = sVals[4].toDouble(&ok);
                if (ok) field[yy*nx + xx].temp = temp;
            }
            if (numparams > 5) {
                double rad = sVals[5].toDouble(&ok);
                if (ok) field[yy*nx + xx].rad = rad;
            }
            if (numparams > 6) {
                QString s1 = sVals[6];
                if (s1 != "$") field[yy*nx + xx].text = s1;
            }
            if (numparams > 8) {
                int d = sVals[8].toInt(&ok);
                if (ok) if (d == 1) field[yy*nx + xx].dotted = 1;
            }

        }
    }

    setViewPart(0, 0, qMin(20, nx), qMin(20, ny));

//rfield defined in setSize;
    rfield = new Point[nx*ny];
    for (int i = 0; i < nx*ny; i++)
        rfield[i] = field[i];
    rx = x;
    ry = y;
    return 0;
}

void Robot::reset()
{
    for (int i = 0; i < nx*ny; i++)
        field[i] = rfield[i];
    x = rx;
    y = ry;
    update();
}

// EVENTS


void Robot::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}


void Robot::mouseMoveEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();
    if (fabs(pos.x() - lastPos.x()) > len ||
        fabs(pos.y() - lastPos.y()) > len) {
            xstart = (xstart - (event->pos().x() - lastPos.x()) / len + nx) % nx;
            ystart = (ystart - (event->pos().y() - lastPos.y()) / len + ny) % ny;
        lastPos = pos;
        update();
    }
}

void Robot::resizeEvent(QResizeEvent *event)
{
    xlen = this->width() / len;
    ylen = this->height() / len;
}

const qreal PI = 3.1415926;
const QColor linec   = Qt::yellow;
const QColor backc   = Qt::darkGreen;
const QColor textc   = Qt::black;
const QColor fillc   = Qt::darkGray;
const QColor rc      = Qt::yellow;
//const QColor borderc = QColor(160, 82, 45); //brown
const QColor borderc = Qt::yellow;

const QColor leftc   = Qt::cyan;
const QColor rightc  = Qt::yellow;
const QColor upc     = Qt::red;
const QColor downc   = Qt::blue;

void Robot::paintEvent(QPaintEvent *event)
{
    int i, j, p, q;
    bool invy;
    QPainter painter;

    painter.begin(this);
    painter.setPen (linec);
    painter.setBrush (backc);
    painter.drawRect(0, 0, this->width(), this->height());

    for (i = 0; i < xlen + 1; i++)
        painter.drawLine(len/2 + len*i, 0, len/2 + len * i, this->height());
    for (i = 0; i < ylen + 1; i++)
        painter.drawLine(0, len/2 + len * i, this->width(), len/2 + len * i);

    QPen borderpen = QPen(borderc);
    borderpen.setWidth(8);
    painter.setBrush(fillc);

    for (i = -1; i < ylen + 1; i++)
        for (j = -1; j < xlen + 1; j++) {
            invy = f(i+ystart, j+xstart, &p, &q);
            painter.setPen(borderpen);

            if (field[p*nx + q].wallUp)
                painter.drawLine(len/2 + j*len, len/2 + (i+invy)*len,
                                 len/2 + (j+1)*len, len/2 + (i+invy)*len);
            if (field[p*nx + q].wallLeft)
                painter.drawLine(len/2 + j*len, len/2 + i*len,
                                 len/2 + j*len, len/2 + (i+1)*len);

            painter.setPen(fillc);
            painter.setBrush(fillc);
            if (field[p*nx + q].filled)
                painter.drawRect(len/2 + j*len + 1, len/2 + i*len + 1,
                                 len - 2, len - 2);

            painter.setPen(textc);
            if (field[p*nx + q].text != "")
                painter.drawText(QRect(len/2 + j*len + 1, len/2 + i*len + 1,
                                        len/2, len/2),
                                 Qt::AlignLeft,
                                 field[p*nx + q].text);

            painter.setBrush(textc);
            if (field[p*nx + q].dotted)
                painter.drawEllipse(QPointF((j+1)*len + 1, (i + 1)*len + 1),
                                    len/8, len/8);


            if (q == x && p == y) {
                painter.save();
                painter.setPen(rc);
                painter.setBrush(rc);
                painter.translate((j + 1) * len, (i + 1)*len);
                painter.rotate(45);

                if (invy) painter.setBrush(downc);
                else painter.setBrush(upc);
                painter.drawRect(QRect(-len/4, -len/4, len/4, len/4));

                painter.setBrush(leftc);
                painter.drawRect(QRect(-len/4, 0, len/4, len/4));
                painter.setBrush(rightc);
                painter.drawRect(QRect(0, -len/4, len/4, len/4));

                if (invy) painter.setBrush(upc);
                else painter.setBrush(downc);
                painter.drawRect(QRect(0, 0, len/4, len/4));

                painter.restore();
                painter.save();

                painter.translate((j + 1) * len, (i + 1)*len);
                painter.setPen(linec);
                if (field[p*nx + q].dotted) painter.setBrush(textc);
                else if (field[p*nx + q].filled) painter.setBrush(fillc);
                else painter.setBrush(backc);
                double z = 6*sqrt(2);
                painter.drawRect(QRect(-len/z, -len/z, 2*len/z, 2*len/z));

                painter.restore();
            }
        }
    painter.end();
    emit update3D();
}

Robot::~Robot()
{
    delete [] field;
    delete [] rfield;
}

