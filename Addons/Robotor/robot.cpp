#include "robot.h"

Robot::Robot(QWidget *parent)
    : QWidget(parent)
{
    field = 0;
    rfield = 0;
    setSize(10, 10);
    setViewPart(0, 0, 10, 10);

    x = 0;
    y = 0;
    len = 40;
    lastPos = QPoint (0, 0);
    type = klein;
    this->resize(len * (xlen + 1), len * (ylen + 1));
    this->setWindowTitle(QString::fromUtf8("Роботор"));
    side = 1;
    update();

}

void Robot::setType(TYPE newtype)
{
    type = newtype;
    update();
}

void Robot::setSize(int xn, int yn)
{
    nx = xn;
    ny = yn;
    if (field != 0) delete field;
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

//coordinate translation. 1 - inversion (klein, mebius), 0 - not (torus, plain)
int Robot::f(int i, int j, int *p, int *q)
{
    *p = (i%ny) + (i<0)*ny;
    *q = (j%nx) + (j<0)*nx;
    if (type == klein) {
        if ( (j < 0) || ((j/nx)%2) != 0 ) {
            *p = ny - 1 - *p;
            return 1;
        }
        return 0;
    }
    if (type == torus) {
        return 0;
    }
}


// MOVEMENT


void Robot::moveLeft()
{
    if (wallLeft()) {
        emit decline(QString::fromUtf8("Там стена!"));
        return;
    }
    f(y, x-1, &y, &x);
    if (x == nx - 1) {
        side *= -1;
        emit sendmsg(QString::fromUtf8("Side changed"));
    }
    else emit sendmsg(QString::fromUtf8("Успех!"));
    update();
}

void Robot::moveRight()
{
    if (wallRight()) {
        emit decline(QString::fromUtf8("Там стена!"));
        return;
    }
    f(y, x+1, &y, &x);
    if (x == 0) {
        side *= -1;
        emit sendmsg(QString::fromUtf8("Side changed"));
    }
    else emit sendmsg(QString::fromUtf8("Успех!"));

    update();
}

void Robot::moveUp()
{
    if (side == 1) {
        if (wallUp()) {
            emit decline(QString::fromUtf8("Там стена!"));
            return;
        }
        f(y-1, x, &y, &x);
        emit sendmsg(QString::fromUtf8("Успех!"));
        update();
    }
    else {
        if (wallDown()) {
            emit decline(QString::fromUtf8("Там стена!"));
            return;
        }
        f(y+1, x, &y, &x);
        emit sendmsg(QString::fromUtf8("Успех!"));
        update();
    }
}

void Robot::moveDown()
{
    if (side == 1) {
        if (wallDown()) {
            emit decline(QString::fromUtf8("Там стена!"));
            return;
        }
        f(y+1, x, &y, &x);
        emit sendmsg(QString::fromUtf8("Успех!"));
        update();
    }
    else {
        if (wallUp()) {
            emit decline(QString::fromUtf8("Там стена!"));
            return;
        }
        f(y-1, x, &y, &x);
        emit sendmsg(QString::fromUtf8("Успех!"));
        update();
    }

}


// FILLS & BORDERS


void Robot::fill()
{
    if (field[y*nx + x].filled) {
        field[y*nx + x].filled = 0;
        emit sendmsg(QString::fromUtf8("Клетка очищена"));
    }
    else {
        field[y*nx + x].filled = 1;
        emit sendmsg(QString::fromUtf8("Клетка закрашена"));
    }
    update();
}

void Robot::dot()
{
    if (field[y*nx + x].dotted) {
        field[y*nx + x].dotted = 0;
        emit sendmsg(QString::fromUtf8("Метка снята"));
    }
    else {
        field[y*nx + x].dotted = 1;
        emit sendmsg(QString::fromUtf8("Метка установлена"));
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



bool Robot::isFilled() const
{
    return field[y*nx + x].filled;
}

bool Robot::isDotted() const
{
    return field[y*nx + x].dotted;
}

bool Robot::wallUp() const
{
    return field[y*nx + x].wallUp;
}

bool Robot::wallDown() const
{
    return field[y*nx + x].wallDown;
}

bool Robot::wallLeft() const
{
    return field[y*nx + x].wallLeft;
}

bool Robot::wallRight() const
{
    return field[y*nx + x].wallRight;
}

int Robot::temp() const
{
    return field[y*nx + x].temp;
}

int Robot::rad() const
{
    return field[y*nx + x].rad;
}

QString Robot::text() const
{
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
    f.write("; Field size: x, y; figure\n");
    QString s, s1;
    if (type == klein) s1 = "klein";
    else s1 = "torus";
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
        qDebug("here");
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
            if (sVals.count() > 2)
                if (sVals.at(2) == "klein") type = klein;
                else type = torus;
            sizeRead = true;
            setSize(nx, ny);
        }
        else if (!roboPosRead) {
            QStringList sVals = line.split(QRegExp("\\s+"));
            if (sVals.count()<2)
                return false;
            bool ok;
            x = sVals[0].toInt(&ok);
            if (!ok)
                return false;
            y = sVals[1].toUInt(&ok);
            if (!ok)
                return false;
            roboPosRead = true;
        }
        else {
            QStringList sVals = line.split(QRegExp("\\s+"));
            if (sVals.count() < 4)
                return 1;
            bool ok;
            int xx = sVals[0].toInt(&ok);
            if (!ok || xx<0 || xx>=nx)
                return 1;
            int yy = sVals[1].toInt(&ok);
            if (!ok || yy<0 || yy>=ny)
                return 1;
            int wall = sVals[2].toInt(&ok);
            if (!ok)
                return 1;
            bool wallUp     = (wall&UP_WALL) == UP_WALL;
            bool wallDown   = (wall&DOWN_WALL) == DOWN_WALL;
            bool wallLeft   = (wall&LEFT_WALL) == LEFT_WALL;
            bool wallRight  = (wall&RIGHT_WALL) == RIGHT_WALL;
            field[yy*nx + xx].wallUp      |= wallUp;
            field[yy*nx + xx].wallDown    |= wallDown;
            field[yy*nx + xx].wallLeft    |= wallLeft;
            field[yy*nx + xx].wallRight   |= wallRight;
            field[yy*nx + xx].filled = sVals[3]!="0";
            field[((yy - 1 + nx)%nx)*nx + xx].wallDown =
                    field[yy*nx + xx].wallUp;
            field[((yy + 1)%nx)*nx + xx].wallUp =
                    field[yy*nx + xx].wallDown;
            field[yy*nx + (xx + 1) % nx].wallLeft =
                  field[yy*nx + xx].wallRight;
            field[yy*nx + (xx - 1 + nx) % nx].wallRight =
                  field[yy*nx + xx].wallLeft;

            double temp = sVals[4].toDouble(&ok);
            if (ok)
                field[yy*nx + xx].temp = temp;
            double rad = sVals[5].toDouble(&ok);
            if (ok)
                field[yy*nx + xx].rad = rad;
            QString s1 = sVals[6];
            if (s1 != "$")
                field[yy*nx + xx].text = s1;

            int d = sVals[8].toInt(&ok);
            if (ok)
                if (d == 1) field[yy*nx + xx].dotted = 1;
        }
    }
    if (rfield != 0) delete rfield;
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
/*
    lastPos = event->pos();
*/
}

void Robot::mouseMoveEvent(QMouseEvent *event)
{
/*
    xstart = (xstart - (event->pos().x() - lastPos.x()) / len) % nx;
    ystart = (ystart - (event->pos().y() - lastPos.y()) / len) % ny;
    if (xstart < 0) xstart += nx;
    if (ystart < 0) xstart += ny;
    lastPos.rx() += len * ((event->pos().x() - lastPos.x()) / len);
    lastPos.ry() += len * ((event->pos().y() - lastPos.y()) / len);

    update();
*/
}

void Robot::resizeEvent(QResizeEvent *event)
{
    xlen = this->width() / len;
    ylen = this->height() / len;
}

const qreal PI = 3.1415926;
const QColor linec   = Qt::gray;
const QColor backc   = Qt::darkGreen;
const QColor textc   = Qt::black;
const QColor fillc   = Qt::darkGray;
const QColor rc      = Qt::yellow;
const QColor borderc = Qt::yellow;
const QColor leftc   = Qt::cyan;
const QColor rightc  = Qt::yellow;
const QColor upc     = Qt::red;
const QColor downc   = Qt::blue;



//invy - "obst", side - inside or outside of klein bottle robotor is

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
    borderpen.setWidth(2);
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

                if (side == 1) {
                    if (invy) painter.setBrush(downc);
                    else painter.setBrush(upc);
                }
                else {
                    if (invy) painter.setBrush(upc);
                    else painter.setBrush(downc);
                }

                painter.drawRect(QRect(-len/4, -len/4, len/4, len/4));

                painter.setBrush(leftc);
                painter.drawRect(QRect(-len/4, 0, len/4, len/4));
                painter.setBrush(rightc);
                painter.drawRect(QRect(0, -len/4, len/4, len/4));

                if (side == 1) {
                    if (invy) painter.setBrush(upc);
                    else painter.setBrush(downc);
                }
                else {
                    if (invy) painter.setBrush(downc);
                    else painter.setBrush(upc);
                }
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
}

Robot::~Robot()
{
    delete [] field;
    delete [] rfield;
}

