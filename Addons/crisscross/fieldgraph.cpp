#include "fieldgraph.h"
#include "QtGui"
#include <limits.h>


FieldGraph::FieldGraph(QWidget *parent) :
    QWidget(parent)
{
    qDebug() << "constructor";
    const QString appName = qApp->applicationName().toLower();
    QString path;
    if (appName!="proga3") {
        path = QApplication::applicationDirPath()+"/Addons/crisscross/images/";
    }
    else {
        path = QApplication::applicationDirPath()+"/../proga3/images/";
    }
    const QString cross = path + "state_0x01.png";
    const QString null = path + "state_0x02.png";
    const QString backGround = path + "state_0x00.png";
    const QString current = path + "state_0x04.png";
    const QString Cross = path + "state_0x16.png";
    const QString Null = path + "state_0x08.png";
    FieldGraph::setFocusPolicy(Qt::StrongFocus);
    pxCross = QPixmap(cross);
    pxNull = QPixmap(null);
    pxBackGround = QPixmap(backGround);
    pxCurrent = QPixmap(current);
    pxcross = QPixmap(Cross);
    pxnull = QPixmap(Null);
    winner = false;
    count = -1;
//    size = 30;
//    coord.x = size / 2;
//    coord.y = size / 2;
    setFieldSize(20);

    for (int i = 0; i < 100; i++)
        for (int j = 0; j< 100; j++) field[i][j] = '.';
    dx[0] = 1; dx[1] =-1; dx[2] = 0; dx[3] = 0; dx[4] = 1; dx[5] = -1; dx[6] = 1; dx[7] = -1;
    dy[0] = 0; dy[1] = 0; dy[2] = 1; dy[3] =-1; dy[4] = 1; dy[5] = -1; dy[6] = -1; dy[7] = 1;
    setLevelHard();
    QObject::connect(this, SIGNAL(fieldChanged()), this, SLOT(update()));
}

void FieldGraph::setFieldSize(int i)
{
    qDebug() << "Setting Field Size" << "i";
    size = i + 10;
    coord.x = coord.y = size / 2;
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            field[i][j] = '.';
    winner = false;
    setFixedSize(pxBackGround.width() * (size - 10) / 2 + 1, pxBackGround.height() * (size - 10) / 2 + 1);
    emit fieldChanged();
}

int FieldGraph::go(int destination, int x, int y)
{
    if (field[y][x] == '.') return 0;
    x += dx[destination];
    y += dy[destination];

    int res = 1;
    while ((y < size)&&(x < size)&&(x > 0)&&(y > 0)&&(field[y][x] == field[y - dy[destination]][x - dx[destination]])) {
        res++;
        x += dx[destination];
        y += dy[destination];
    }
    return res;
}
void FieldGraph::cheatEnable()
{
    cheat = true;
}

void FieldGraph::checker()
{
    for (int i = 5; i < size + 10; i++)
        for (int j = 5; j < size + 10; j++)
            for (int k = 0; k < 8; k++) {
                bool b = true;
                for (int z = 0; z < 5; z++) b = b && (field[i + dy[k] * z][j + dx[k] * z] == 'X');
                if (b)
                    for (int z = 0; z < 5; z++) field[i + dy[k] * z][j + dx[k] * z] = 'x';
    }
    for (int i = 5; i < size + 10; i++)
        for (int j = 5; j < size + 10; j++)
            for (int k = 0; k < 8; k++) {
                bool b = true;
                for (int z = 0; z < 5; z++) b = b && (field[i + dy[k] * z][j + dx[k] * z] == 'O');
                if (b)
                    for (int z = 0; z < 5; z++) field[i + dy[k] * z][j + dx[k] * z] = 'o';
    }
}

void FieldGraph::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    QPainter p(this);
    qDebug() << "painting";

    int y = 1;
    size-=10;
    int size_pix = pxBackGround.height()/2;
    for (int i = 0; i < size; i++) {
        int x = 1;
        for (int j = 0; j < size; j++) {
            p.drawPixmap(x, y, size_pix, size_pix, pxBackGround);
            if (field[i+5][j+5] == 'X') {
                p.drawPixmap(x, y, size_pix, size_pix, pxCross);
            }
            if (field[i+5][j+5] == 'O') {
                p.drawPixmap(x, y, size_pix, size_pix, pxNull);
            }
            if ((i+5 == coord.y) && (j+5 == coord.x)) {
                p.drawPixmap(x, y, size_pix, size_pix, pxCurrent);
            }
            if (field[i+5][j+5] == 'x') {
                p.drawPixmap(x, y, size_pix, size_pix, pxcross);
            }
            if (field[i+5][j+5] == 'o') {
                p.drawPixmap(x, y, size_pix, size_pix, pxnull);
            }
            x += size_pix;
        }
        y += size_pix;
    }
    p.end();

    event->accept();
    size+=10;
}

void FieldGraph::up()
{
    if (coord.y != 0) {
        coord.y--;
        emit fieldChanged();
    }
}
void FieldGraph::down()
{
    if (coord.y < size - 1) {
        coord.y++;
        emit fieldChanged();
    }
}
void FieldGraph::left()
{
    if (coord.x != 0) {
        coord.x--;
        emit fieldChanged();
    }
}
void FieldGraph::right()
{
    if (coord.x < size - 1) {
        coord.x++;
        emit fieldChanged();
    }
}

int FieldGraph::can(int destination, int x, int y)
{
    if (field[y][x] == '.') return 0;
    char ch = field[y][x];
    while ((y < size)&&(x < size)&&(x >= 0)&&(y >= 0)&&((field[y][x] == ch)||(field[y][x] == '.'))) {
        y -= dy[destination];
        x -= dx[destination];
    }
    int res = 0;
    do  {
        y += dy[destination];
        x += dx[destination];
        res++;
    } while ((y < size)&&(x < size)&&(x >= 0)&&(y >=  0)&&((field[y][x] == ch)||(field[y][x] == '.')));
    if (res < 5) {
        return 0;
    } else {
        return 1;
    }
}

bool FieldGraph::check(int i)
{
    return (i > -1)&&(i < size);
}

TPoint FieldGraph::analyze()
{
    size -=15;
    long long table[size+20][size+20];
    for (int i = 5; i < size+10; i++)
        for (int j = 5; j < size+10; j++)
            table[i][j] = 1;


    for (int x = 5; x < size+10; x++)
        for (int y = 5; y < size+10; y++)
            for (int i = 0; i < 8; i++) {
                int went = go(i, x, y);
                int could = can(i, x, y);
                if ((went > 0)&&(could > 0)) {
                    if (field[y][x] == 'X') {
                        if (went < 3) {
                            table[y - dy[i]][x - dx[i]] += FieldGraph::power(10, went) / 10 + 1;
                        } else {
                            if (went == 3) {
                                if ((field[y + dy[i] * 4][x + dx[i] * 4] == '.') && (field[y - dy[i] * 2][x - dx[i] * 2]))
                                    if (table[y - dy[i]][x - dx[i]] < 3000) table[y - dy[i]][x - dx[i]] *= 4000; else
                                                                          table[y - dy[i]][x - dx[i]] += 4000; else
                                                                          table[y - dy[i]][x - dx[i]] += 100;
                            } else {
                                table[y - dy[i]][x - dx[i]] = 20000000;
                            }
                        }
                    } else {
                        if (went < 3) {
                            table[y - dy[i]][x - dx[i]] += FieldGraph::power(10, went) / 10 + 5;
                        } else {
                            if (went == 3) {
                                if ((field[y + dy[i] * 4][x + dx[i] * 4] == '.') && (field[y - dy[i] * 2][x - dx[i] * 2]))
                                    if (table[y - dy[i]][x - dy[i]] < 3000) table[y - dy[i]][x - dx[i]] *= 1000000; else
                                                                            table[y - dy[i]][x - dx[i]] += 1000000; else
                                                                            table[y - dy[i]][x - dx[i]] *= 100;
                            } else {
                                table[y - dy[i]][x - dx[i]] = 80000000;
                            }
                        }

                    }
                }
            }


    for (int x = 5; x < size + 10; x++)
        for (int y = 5; y < size + 10; y++)
            for (int i = 0; i < 8; i++) {
                 if (FieldGraph::check(x + dx[i] * 4) && FieldGraph::check(y + dy[i] * 4)) {
                     if (((field[y][x] == field[y + dy[i]][x + dx[i]]) && (field[y][x] == field[y + dy[i] * 3][x + dx[i] * 3]))&&((field[y - dy[i]][x - dx[i]] == '.')||(field[y + dy[i] * 4][x + dx[i] * 4] == '.'))) {
                         if (field[y][x] == 'X') {
                             table[y + dy[i] * 2][x + dx[i] * 2] += 6000 * can(i, x, y);
                         } else
                         if (field[y][x] == 'O') {
                             table[y + dy[i] * 2][x + dx[i] * 2] += 8000 * can(i, x, y) * 5;
                         }
                     }
                 }
            }

    for (int x = 5; x < size + 10; x++)
        for (int y = 5; y < size + 10; y++)
            for (int i = 0; i < 8; i++) {
                if ((field[y][x] != '.') && (field[y][x] == field[y + dy[i] * 2][x + dx[i] * 2])) {
                    table[y + dy[i]][x + dx[i]] += 12;
                }
    }

    TPoint res[size * size];
    int max = 0;
    int count = -1;
    size-=10;
    for (int i = 5; i < size+10; i++)
        for (int j = 5; j < size+10; j++)
            if (field[i][j] != '.') table[i][j] = 0;
    size+=10;
    for (int i = 5; i < size + 10; i++)
        for (int j = 5; j < size + 10; j++)
            if (table[i][j] > max) {
                count = 0;
                res[count].x = j;
                res[count].y = i;
                max = table[i][j];
            } else
                if (table[i][j] == max) {
                    count++;
                    res[count].x = j;
                    res[count].y = i;
                }
    count = rand() % (count + 1);
    coord.x = res[count].x;
    coord.y = res[count].y;
    qDebug() << table[coord.y][coord.x];
    size += 15;
    return coord;
}

TPoint FieldGraph::analyze2()
{
    size -=4;
    int table[100][100];
    for (int i = 0; i < 100; i++)
        for (int j = 0; j<100; j++) table[i][j] = 0;

    for (int i = 5; i < size; i++)
        for (int j = 5; j < size; j++)
            for (int k = 0; k < 8; k++)
                if (field[i + dy[k]][j + dx[k]] == 'O') table[i][j] += 5; else
                if (field[i + dy[k]][j + dx[k]] == 'X') table[i][j]++;

    for (int i = 5; i < size; i++)
        for (int j = 5; j < size; j++)
            for (int k = 0; k < 8; k++)
                if (go(k, j + dx[k], i + dy[k]) >= 4)
                    if (table[i][j] == 'O') table[i][j] += 1000000000; else
                                            table[i][j] += 100000000;

    for (int i = 5; i < size; i++)
        for (int j = 5; j < size; j++)
            for (int k = 0; k < 8; k++)
                if ((go(k, j + dx[k], i + dy[k]) >= 3) && (field[i - dy[k]][j - dx[k]] == '.') && (field[i + dy[k] * 4][j + dx[k] * 4] == '.'))
                    if (field[i + dy[k]][j + dx[k]] == 'O') table[i][j] += 100000; else
                                                            table[i][j] += 50000;

    for (int i = 5; i < size; i++)
        for (int j = 5; j < size; j++)
            for (int k = 0; k < 8; k++)
                if ((field[i + dy[k]][j + dx[k]] == 'O') && (field[i + dy[k] * 2][j + dx[k] * 2] == 'O') &&
                   (field[i + dy[k] * 3][j * dx[k] * 3] != 'X')&&(field[i - dy[k]][j - dx[k]] != 'X') &&
                   ((field[i + dy[k] * 4][j * dx[k] * 4] != 'X')||(field[i - dy[k] * 2][j - dx[k] * 2] != 'X')))
                            table[i][j] += 100;

    for (int i = 5; i < size; i++)
        for (int j = 5; j < size; j++)
            for (int k = 0; k < 8; k++)
                if ((field[i + dy[k]][j + dx[k]] == 'X') && (field[i + dy[k] * 2][j + dx[k] * 2] == 'X') &&
                   (field[i + dy[k] * 3][j * dx[k] * 3] != 'O')&&(field[i - dy[k]][j - dx[k]] != 'O') &&
                   ((field[i + dy[k] * 4][j * dx[k] * 4] != 'O')||(field[i - dy[k]][j - dx[k]] != 'O')))
                        table[i][j] += 50;

    for (int i = 5; i < size; i++)
        for (int j = 5; j < size; j++)
            for (int k = 0; k < 8; k++)
                if ((field[i - dy[k]][j - dx[k]] == 'O') && (field[i + dy[k]][j + dx[k]] == 'O') && (field[i + dy[k] * 2][j + dx[k] * 2] != 'X')
                    && (field[i - dy[k] * 2][j - dx[k] * 2] != 'X') && ((field[i - dy[k] * 3][j - dx[k] * 3] != 'X') || (field[i + dy[k] * 3][j + dx[k] * 3] != 'X')))
                            table[i][j] += 102;

    for (int i = 5; i < size; i++)
        for (int j = 5; j < size; j++)
            for (int k = 0; k < 8; k++)
                if ((field[i - dy[k]][j - dx[k]] == 'X') && (field[i + dy[k]][j + dx[k]] == 'X') && (field[i + dy[k] * 2][j + dx[k] * 2] != 'O')
                    && (field[i - dy[k] * 2][j - dx[k] * 2] != 'O') && ((field[i - dy[k] * 3][j - dx[k] * 3] != 'O') || (field[i + dy[k] * 3][j + dx[k] * 3] != 'O')))
                            table[i][j] += 51;

    for (int i = 5; i < size; i++)
        for (int j = 5; j < size; j++)
            for (int k = 0; k < 8; k++)
                if ((field[i - dy[k]][j - dx[k]] == 'O') && (field[i + dy[k]][j + dx[k]] == 'O') && (field[i + dy[k] * 2][j + dx[k] * 2] == 'O') &&
                    ((field[i - dy[k] * 2][j - dx[k] * 2] != 'X')&&(field[i + dy[k] * 3][j + dx[k] * 3] != 'X'))) table[i][j] += 100000;

    for (int i = 5; i < size; i++)
        for (int j = 5; j < size; j++)
            for (int k = 0; k < 8; k++)
                if ((field[i - dy[k]][j - dx[k]] == 'X') && (field[i + dy[k]][j + dx[k]] == 'X') && (field[i + dy[k] * 2][j + dx[k] * 2] == 'X') &&
                    ((field[i - dy[k] * 2][j - dx[k] * 2] != 'O')&&(field[i + dy[k] * 3][j + dx[k] * 3] != 'O'))) table[i][j] += 50000;

    for (int i = 5; i < size; i++)
        for (int j = 5; j < size; j++)
            for (int k = 0; k < 8; k++)
                if ((field[i - dy[k]][j - dx[k]] == 'X') && (field[i + dy[k]][j + dx[k]] == 'X') && (field[i + dy[k] * 2][j + dx[k] * 2] == 'X') &&
                    ((field[i - dy[k] * 2][j - dx[k] * 2] != 'O')||(field[i + dy[k] * 3][j + dx[k] * 3] != 'O'))) table[i][j] += 50;

    for (int i = 5; i < size; i++)
        for (int j = 5; j < size; j++)
            for (int k = 0; k < 8; k++)
                if ((field[i - dy[k]][j - dx[k]] == 'O') && (field[i + dy[k]][j + dx[k]] == 'O') && (field[i + dy[k] * 2][j + dx[k] * 2] == 'O') &&
                    ((field[i - dy[k] * 2][j - dx[k] * 2] != 'X')||(field[i + dy[k] * 3][j + dx[k] * 3] != 'X'))) table[i][j] += 100;
    TPoint res[size * size];
    int max = 0;
    int count = -1;
    for (int i = 5; i < size; i++)
        for (int j = 5; j < size; j++)
            if (field[i][j] != '.') table[i][j] = 0;
    for (int i = 5; i < size - 1; i++)
        for (int j = 5; j < size - 1; j++)
            if (table[i][j] > max) {
                count = 0;
                res[count].x = j;
                res[count].y = i;
                max = table[i][j];
            } else
                if (table[i][j] == max) {
                    count++;
                    res[count].x = j;
                    res[count].y = i;
                }
    count = rand() % (count + 1);
    coord.x = res[count].x;
    coord.y = res[count].y;
    qDebug() << table[coord.y][coord.x];
    size += 4;
    return coord;
}

void FieldGraph::attack()
{
    if (((field[coord.y][coord.x]=='.')||(cheat))&&(!winner)) {
        cheat = false;
        field[coord.y][coord.x] = 'X';
        count++;
        history[count] = coord;
        FieldGraph::checker();
        emit fieldChanged();
        for (int x = 0; x < size; x++) {
           for (int y = 0; y < size; y++) {
             for (int i = 0; i < 8; i++) {
               if (FieldGraph::go(i, x, y) > 4) {
                 qDebug() << "Player won!!!";
                 QMessageBox::about(this, QString("Proga3"), tr("Gamer won..."));
                 winner = true;
                 return;
               }
             }
           }
        }

        TPoint p;
        if (level == 0) p = FieldGraph::analyze2();
        if (level == 2) p = FieldGraph::analyze();
        field[p.y][p.x] = 'O';
        count++;
        history[count] = p;
        FieldGraph::checker();
        emit fieldChanged();
        for (int x = 0; x < size; x++) {
           for (int y = 0; y < size; y++) {
             for (int i = 0; i < 8; i++) {
               if (FieldGraph::go(i, x, y) > 4) {
                 qDebug() << "Computer won!!!";
                 QMessageBox::about(this, QString("Proga3"), tr("Computer won!!!"));
                 winner = true;
                 return;
               }
             }
           }
        }
    }
}

void FieldGraph::ctrlz()
{
    if (count > 0) {
        field[history[count].y][history[count].x] = '.';
        count--;
        field[history[count].y][history[count].x] = '.';
        count--;
        winner = false;
        for (int i = 0; i < size; i++)
            for (int j = 0; j < size; j++)
                if (field[i][j] == 'x') field[i][j] = 'X'; else
                    if (field[i][j] == 'o') field[i][j] = 'O';
        emit fieldChanged();
    }
}

void FieldGraph::newgame()
{
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            field[i][j] = '.';
    winner = false;
    emit fieldChanged();
}

int FieldGraph::power(int a, int i)
{
    int res = a;
    for (int j = 1; j < i; j++) {
        res *= a;
    }
    return res;
}

void FieldGraph::keyPressEvent(QKeyEvent *event)
{
    if (ignore) return;
    if (event->key()==Qt::Key_Up) {
        FieldGraph::up();
        event->accept();
    }
    else if (event->key()==Qt::Key_Down) {
        FieldGraph::down();
        event->accept();
    }
    else if (event->key()==Qt::Key_Left) {
        FieldGraph::left();
        event->accept();
    }
    else if (event->key()==Qt::Key_Right) {
        FieldGraph::right();
        event->accept();
    }
    else if (event->key()==Qt::Key_Space) {
        FieldGraph::attack();
        event->accept();
    }
    else {
        event->ignore();
    }

    emit fieldChanged();
}
void FieldGraph::mousePressEvent(QMouseEvent *event)
{
    if (ignore) return;
    qDebug() << event->x() << " " << event->y();
    int x = (event->x() + pxBackGround.width() - 1) / (pxBackGround.width() / 2) - 2;
    int y = (event->y() + pxBackGround.width() - 1) / (pxBackGround.width() / 2) - 2;
    qDebug() << x << " " << y;
    if ((coord.x == x+5) && (coord.y == y+5)) {
        FieldGraph::attack();
    } else {
        coord.x = x+5;
        coord.y = y+5;
    }
    emit fieldChanged();
}

void FieldGraph::saveField()
{
    QSettings s;
    const QString searchPath = s.value("LastSaveDir", QDir::homePath()).toString();
    const QString fileName = QFileDialog::getSaveFileName(this,
                                     tr("Save file..."),
                                     searchPath,
                                     tr("Proga3 files (*)"));
    s.setValue("LastSaveDir", QFileInfo(fileName).absoluteDir().absolutePath());

    QFile file(fileName);
    qDebug() << "saving field";
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {return;} else
      {
        QTextStream out(&file);
        out << "proga3 file format\n";
        out << level << "\n";
        out << size << "\n";
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                out << field[i + 5][j + 5];
            }
            out << "\n";
        }
    }
}

void FieldGraph::loadField()
{
    count = 0;
    QSettings s;
    const QString searchPath = s.value("LastSaveDir", QDir::homePath()).toString();
    const QString fileName = QFileDialog::getOpenFileName(this,
                                     tr("Load file..."),
                                     searchPath,
                                     tr("Proga3 files (*)"));
    if (!fileName.isEmpty()) {
        s.setValue("LastSaveDir", QFileInfo(fileName).absoluteDir().absolutePath());
    } else return;

    QFile file(fileName);
    qDebug() << "loading field";
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {return;} else
      {
        QTextStream in(&file);
        QString line = in.readLine();
        if (line != "proga3 file format") {
            return;
        }
        line = in.readLine();
        bool ok = false;
        level = line.toInt(&ok, 10);
        line = in.readLine();
        int n = line.toInt(&ok, 10);
        if (!ok) {
            return;
        } else {
            size = n;
            for (int i = 0; i < size; i++) {
                line = in.readLine();
                for (int j = 0; j < size; j++) {
                    if (line[j] == '.') field[i + 5][j + 5] = '.'; else
                    if (line[j] == 'X') field[i + 5][j + 5] = 'X'; else
                                        field[i + 5][j + 5] = 'O';
                }
            }
            emit fieldChanged();
            return;
        }
    }

}

void FieldGraph::inversion()
{
    if (winner) return;
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            if (field[i][j] == 'O') field[i][j] = 'X'; else
            if (field[i][j] == 'X') field[i][j] = 'O';
}

TPoint FieldGraph::helpMeSimple()
{
    FieldGraph::inversion();
    coord = FieldGraph::analyze();
    FieldGraph::inversion();
    return coord;
    emit fieldChanged();
}

TPoint FieldGraph::helpMeHard()
{
    FieldGraph::inversion();
    coord = analyze2();
    FieldGraph::inversion();
    return coord;
    emit fieldChanged();
}

void FieldGraph::setLevelHard()
{
    level = 0;
}

void FieldGraph::setLevelSimple()
{
    level = 2;
}

void FieldGraph::defend(int x, int y)
{
    field[y + 4][x + 4] = 'X';
    TPoint p;
    if (level == 0) p = analyze2();
    if (level == 2) p = analyze();
    field[p.y][p.x] = 'O';
    emit fieldChanged();
}

int FieldGraph::whoWon()
{
    for (int i = 5; i < size - 5; i++)
        for (int j = 5; j < size - 5; j++) {
            bool b = true;
            for (int k = 0; k < 5; k++)
                b = b && (field[i + dy[i]][j + dx[j]] == 'X');
            if (b) return 1;
        }

    for (int i = 5; i < size - 5; i++)
        for (int j = 5; j < size - 5; j++) {
            bool b = true;
            for (int k = 0; k < 5; k++)
                b = b && (field[i + dy[i]][j + dx[j]] == 'O');
            if (b) return 2;
        }
    return 0;
}

int FieldGraph::lastX()
{
    return history[count - 1].x;
}

int FieldGraph::lastY()
{
    return history[count - 1].y;
}
