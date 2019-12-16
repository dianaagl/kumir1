#ifndef NXTBRICK_H
#define NXTBRICK_H

#include <QtCore>

class NxtBrick
{
public:
    explicit NxtBrick(void *connection);
    QString playSound();
private:
    void *m_brick;
};

#endif // NXTBRICK_H
