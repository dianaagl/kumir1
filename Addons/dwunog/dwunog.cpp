#include <cmath>
using namespace std;

#include "dwunog.h"

const int flen = 60;
const int nlen = 60;
const int hrad = 10;
const double PI = 3.1415926;

void Dwunog::setParameters(int lfang, int rfang, int nang, int slen,
                    QPointF cps, QList <QLineF> wrld, int fuelm)
{
    lang = lfang;
    rang = rfang;
    hang = nang;
    dl = lang;
    dr = rang;
    dh = hang;
    dpos = cps;
    world = wrld;
    fuelmax = fuelm;
    fuel = fuelmax;

    flen = 5*slen;
    nlen = 3*flen/2;
    hrad = 3*slen/2;
    prepareGeometryChange();

    speed = 20;
    savek1 = false;

    qDebug("flen = %d, lang = %d", flen, lang);

    lpos = cps+QPointF(flen*cos(1.0*lang*PI/180), flen*sin(1.0*lang*PI/180));;
    rpos = cps+QPointF(flen*cos(1.0*rang*PI/180), flen*sin(1.0*rang*PI/180));;
    cpos = QPointF(0, 0);
    setPos(cps);

    if (donecmd != 0) delete donecmd;
    if (cmdk1 != 0) delete cmdk1;
    donecmd = new QList <Command>;
    cmdk1 = new QList <Command>;
    indexk1 = -1;
    savehead = 0;
    finish();
}

void Dwunog::setID(int newid)
{
    id = newid;
}

QPointF Dwunog::hPos() const
{
    return QPointF(nlen*cos(1.0*hang*PI/180), nlen*sin(1.0*hang*PI/180));
}

//COLLISION

bool Dwunog::lfCollides()
{
    int i;
    QPointF p;
    QLineF line(mapToScene(cpos), lpos);

    for (i = 0; i < world.size(); i++) {
        if (world[i].intersect(line, &p) == QLineF::BoundedIntersection) {
            qDebug("\nLeft Foot Collides!");
            qDebug("Left Foot: (%lf, %lf) - (%lf, %lf)", line.p1().x(), line.p1().y(),
                   line.p2().x(), line.p2().y());
            qDebug("Line%d: (%lf, %lf) - (%lf, %lf)", i, world[i].p1().x(), world[i].p1().y(),
               world[i].p2().x(), world[i].p2().y());
            qDebug("Collision! Point = (%lf, %lf)", p.x(), p.y());
            qDebug("LPos = (%lf, %lf)", lpos.x(), lpos.y());
            return 1;
        }
    }
    return 0;
}

bool Dwunog::rfCollides()
{
    int i;
    QPointF p;
    QLineF line(mapToScene(cpos), rpos);

    for (i = 0; i < world.size(); i++) {
        if (world[i].intersect(line, &p) == QLineF::BoundedIntersection) {
            qDebug() <<"\nRight Foot Collides!";
            qDebug("Right Foot: (%lf, %lf) - (%lf, %lf)", line.p1().x(), line.p1().y(),
                       line.p2().x(), line.p2().y());
            qDebug("Line%d: (%lf, %lf) - (%lf, %lf)", i, world[i].p1().x(), world[i].p1().y(),
                   world[i].p2().x(), world[i].p2().y());
            qDebug("Collision! Point = (%lf, %lf)", p.x(), p.y());
            qDebug("RPos = (%lf, %lf)", rpos.x(), rpos.y());
            return 1;
        }
    }
    return 0;
}

bool Dwunog::hCollides()
{
    int i;
    QPointF p;
    QLineF line(mapToScene(cpos), mapToScene(hPos()));
    QPainterPath head;

    head.addEllipse(QRectF(hPos().x() - hrad, hPos().y() - hrad, 2*hrad, 2*hrad));
    for (i = 0; i < world.size(); i++) {
        QPainterPath linep;
        linep.moveTo(world[i].p1());
        linep.lineTo(world[i].p2());
        linep.lineTo(world[i].p1());

        if (world[i].intersect(line, &p) == QLineF::BoundedIntersection)
            return true;
        if (head.intersects(mapFromScene(linep)))
            return true;
    }
    return false;
}

// ACTIONS
void Dwunog::setAction(Actions newact, int arg)
{
    count = 0;
    angle = arg;
    if (angle > 0) sign = 1;
    else {
        sign = -1;
        angle *= -1;
    }
    action = newact;
    stop = false;
}

void Dwunog::doCommand(QString cmd, int arg)
{
    if (fuel <= 0)
        return;
    if (cmd == "stop") {
        indexk1 = -1;
        stop = true;
    }
    else if (cmd == "info") {

    }

    else if (cmd == "savek1") {
        savek1 = !savek1;
        if (savek1) cmdk1->clear();
    }

    else if (cmd == "k1") {
        if (savek1 || cmdk1->size() == 0) return;
        if (indexk1 >= 0) doCommand("stop");
        else {
            indexk1 = 0;
            doCommand(cmdk1->at(0).cmd, cmdk1->at(0).arg);
        }
    }

    else if (cmd == "lfturn"){
        if (action == lfoot || action == waitl) stop = true;
        else if (stop && (rstand || cstand)) {
            lstand = 0; lang %= 360;
            setAction(lfoot, arg);
            lfturn();
        }
        else if (stop && lstand) {
            setAction(waitl, arg);
            lfwait();
        }
    }

    else if (cmd == "rfturn") {
        if (action == rfoot || action == waitr) stop = true;
        else if (stop && (lstand || cstand)) {
            rstand = 0; rang %= 360;
            setAction(rfoot, arg);
            rfturn();
        }
        else if (stop && rstand) {
            setAction(waitr, arg);
            rfwait();
        }
    }

    else if (cmd == "hturn")  {
        if (action == head) stop = true;
        else if (stop) {
            hang %= 360;
            setAction(head, arg);
            hturn();
        }
    }

    else if (cmd == "headup") {
        if (action == head) stop = true;
        else if (stop) {
            if (savek1) savehead = 1;
            hang %= 360;
            if (90 > hang) setAction(head, -hang - 90);
            else setAction(head, 270 - hang);
            hturn();
        }
    }

    else if (cmd == "wait") {
        if ((lstand && rstand)) return;
        if (lstand) {
            if (action == waitl) stop = true;
            else if (stop) {
                if (hPos().x() > foot(lang).x()) setAction(waitl, arg);
                else setAction(waitl, -arg);
                rstand = 0; cstand = 0;
                lfwait();
            }
        }
        else if (rstand) {
            if (action == waitr) stop = true;
            else if (stop) {
                if (hPos().x() > foot(rang).x()) setAction(waitr, arg);
                else setAction(waitr, -arg);
                lstand = 0; cstand = 0;
                rfwait();
            }
        }
    }

}

QPointF Dwunog::foot(int a) const
{
    return QPointF(flen*cos(1.0*a*PI/180), flen*sin(1.0*a*PI/180));
}

void Dwunog::lfturn()
{
    if (fuel <= 0)
        return;
    if (this->collidingItems().size() > 0)
        emit collision();
    if (count < angle && (!stop)) {
        lang += sign; lpos = pos() + foot(lang);
        if (lfCollides()) {
            lang -= sign; lpos = pos() + foot(lang);
            finish("LF Collides!");
            return;
        }
        count++;
        update();
        timer.singleShot(speed, this, SLOT(lfturn()));
        emit fuelchange(--fuel);
    }
    else {
        finish("Cmd done!");
    }
}

void Dwunog::rfturn()
{
    if (fuel <= 0)
        return;
    if (this->collidingItems().size() > 0)
        emit collision();
    if (count < angle && (!stop)) {
        rang += sign; rpos = pos() + foot(rang);
        if (rfCollides()) {
            rang -= sign; rpos = pos() + foot(rang);
            finish("RF Collides!");
            return;
        }
        count++;
        update();
        timer.singleShot(speed, this, SLOT(rfturn()));
        emit fuelchange(--fuel);
    }
    else {
        finish("Cmd done!");
    }
}

void Dwunog::hturn()
{
    if (fuel <= 0)
        return;
    if (this->collidingItems().size() > 0)
        emit collision();
    if (count < angle && (!stop)) {
        hang += sign;
        if (hCollides()) {
            hang -= sign;
            finish("H Collides!");
            return;
        }
        count++;
        update();
        timer.singleShot(speed, this, SLOT(hturn()));
        emit fuelchange(--fuel);
    }
    else {
        finish("Cmd done!");
    }
}

void Dwunog::lfwait()
{
    if (this->collidingItems().size() > 0)
        emit collision();
    if (count < angle && (!stop)) {
        lang+=sign; rang+=sign; hang+=sign;
        rpos = pos() + foot(rang);
        qDebug("lPos = (%lf, %lf), lang = %d", lpos.x(), lpos.y(), lang);
        if (lfCollides() || rfCollides() || hCollides()) {
            while (lfCollides() || rfCollides() || hCollides()) {
                lang-=sign; rang-=sign; hang-=sign;
                rpos = pos() + foot(rang);
                setPos(lpos - foot(lang));
            }
            finish("Collision!");
            return;
        }
        else setPos(lpos - foot(lang));
        count++;
        update();
        timer.singleShot(speed, this, SLOT(lfwait()));
    }
    else {
        finish("Cmd done!");
    }
}

void Dwunog::rfwait()
{
    if (this->collidingItems().size() > 0)
        emit collision();
    if (count < angle && (!stop)) {
        lang+=sign; rang+=sign; hang+=sign;
        lpos = pos() + foot(lang);
        if (lfCollides() || rfCollides() || hCollides()) {
            while (lfCollides() || rfCollides() || hCollides()) {
                qDebug("In while! lang = %d\n", lang);
                lang-=sign; rang-=sign; hang-=sign;
                lpos = pos() + foot(lang);
                setPos(rpos - foot(rang));
            }
            finish("Collision!");
            return;
        }
        else setPos(rpos - foot(rang));
        count++;
        update();
        timer.singleShot(speed, this, SLOT(rfwait()));
    }
    else {
        finish("Cmd done!");
    }
}

bool Dwunog::isStand(int ind) const
{
    QPointF p, res;
    if (ind == 1) p = lpos;
    if (ind == 2) p = rpos;
    if (ind == 3) p = mapToScene(cpos);

    const int DIST = 3;
    for (int i = 0; i < world.size(); i++)
        if (world[i].intersect(QLineF(p.x()-DIST, p.y()-DIST, p.x()+DIST, p.y()-DIST), &res) == QLineF::BoundedIntersection ||
            world[i].intersect(QLineF(p.x()-DIST, p.y()-DIST, p.x()-DIST, p.y()+DIST), &res) == QLineF::BoundedIntersection ||
            world[i].intersect(QLineF(p.x()+DIST, p.y()-DIST, p.x()+DIST, p.y()+DIST), &res) == QLineF::BoundedIntersection ||
            world[i].intersect(QLineF(p.x()-DIST, p.y()+DIST, p.x()+DIST, p.y()+DIST), &res) == QLineF::BoundedIntersection ) return 1;
    return 0;
}


void Dwunog::addToList(QList<Command> *list)
{
    Command c;
    switch(action) {
        case lfoot: c.cmd = "lfturn"; break;
        case rfoot: c.cmd = "rfturn"; break;
        case head: c.cmd = "hturn"; break;
        case waitl: c.cmd = "wait"; break;
        case waitr: c.cmd = "wait"; break;
        //case stop: c.cmd = "stop"; break;
        default: break;
    }
    c.arg = sign*count;
    list->append(c);
}

void Dwunog::saveToFile(const QString &name)
{
    FILE *f = fopen("addons/test.txt", "w");
    for (int i = 0; i < donecmd->size(); i++) {
        QString s = cmdk1->at(i).cmd + " " +
                    QString::number(cmdk1->at(i).arg) +
                    "\n";
        fprintf(f, qPrintable(s));
    }
    fclose(f);
}

void Dwunog::finish(QString message)
{
    lstand = isStand(1);
    rstand = isStand(2);
    cstand = isStand(3);

    if (message != "") emit decline(message);

    addToList(donecmd);
    if (savek1) {
        if (savehead && action != stop) {
            Command cmd;
            cmd.cmd = "headup";
            cmdk1->append(cmd);
            savehead = 0;
        }
        else addToList(cmdk1);
    }

    stop = true;
    action = none;

    if (indexk1 >= 0) {
        if (indexk1 + 1 < cmdk1->size()) {
            indexk1++;
            doCommand(cmdk1->at(indexk1).cmd, cmdk1->at(indexk1).arg);
        }
        else {
            indexk1 = -1;
            emit sync();
        }
    }
    else emit sync();

    update();
}



void Dwunog::reset()
{
    int slen = QApplication::desktop()->width() / 100;
    setParameters(dl, dr, dh, slen, dpos, world, fuelmax);
}

// EVENTS

const QColor lfcolor = Qt::red;
const QColor rfcolor = Qt::green;
const QColor ncolor  = Qt::black;

const int R = 3;
const QColor C1 = Qt::darkRed;
const QColor C2 = Qt::darkGreen;
const QColor C3 = Qt::red;
const QColor C4 = Qt::green;

QRectF Dwunog::boundingRect() const
{
    return QRectF(-nlen-hrad, -nlen-hrad, 2*(nlen+hrad), 2*(nlen+hrad));
}

QPainterPath Dwunog::shape() const
{
    QPainterPath path;
    path.addEllipse(hPos(), hrad, hrad);
    return path;
}

bool Dwunog::collidesWithItem(const QGraphicsItem *other,
                              Qt::ItemSelectionMode) const
{
    //UserType + 4 == Apple.type()
    if (other->type() != UserType + 4)
        return false;
    return shape().intersects(mapFromItem(other, other->shape()));
}

void Dwunog::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->save();

//draw feet
    painter->setPen(Qt::red);
    painter->drawLine(cpos, foot(lang));
    painter->setPen(Qt::green);
    painter->drawLine(cpos, foot(rang));
    painter->setPen(Qt::black);
    painter->drawLine(cpos, hPos());

//draw points
    if (lstand) { painter->setPen(C1); painter->setBrush(C3); }
    else        { painter->setPen(C2); painter->setBrush(C4); }
    painter->drawEllipse(foot(lang), R, R);
    if (rstand) { painter->setPen(C1); painter->setBrush(C3); }
    else        { painter->setPen(C2); painter->setBrush(C4); }
    painter->drawEllipse(foot(rang), R, R);
    if (cstand) { painter->setPen(C1); painter->setBrush(C3); }
    else        { painter->setPen(C2); painter->setBrush(C4); }
    painter->drawEllipse(cpos, R, R);

//draw head
    painter->setBrush(Qt::SolidPattern);
    painter->setBrush(Qt::white);
    painter->drawEllipse(QRectF(nlen*cos(1.0*hang*PI/180) - hrad, nlen*sin(1.0*hang*PI/180) - hrad,
                                2*hrad, 2*hrad).normalized());
    painter->drawPoint(QPointF(nlen*cos(1.0*hang*PI/180) + hrad/3, nlen*sin(1.0*hang*PI/180) - hrad/3));
    painter->drawPoint(QPointF(nlen*cos(1.0*hang*PI/180) - hrad/3, nlen*sin(1.0*hang*PI/180) - hrad/3));

//draw smile
    /*
    if (hIntersect) painter->drawArc(QRectF(nLen*cos(1.0*hAng*PI/180) - hRad/2,
                                            nLen*sin(1.0*hAng*PI/180) - 5*hRad/4,
                                            hRad, hRad).normalized(),
                                            -16*30, -16*120);
    else
    */
        painter->drawArc(QRectF(nlen*cos(1.0*hang*PI/180) - hrad/2,
                                nlen*sin(1.0*hang*PI/180) - hrad/2,
                                hrad, hrad).normalized(),
                                -16*150, 16*120);

    painter->restore();
}

//------
Dwunog::~Dwunog()
{
    qDebug("Dwunog [%d] destructor\n", id);
    delete donecmd;
    delete cmdk1;
}
