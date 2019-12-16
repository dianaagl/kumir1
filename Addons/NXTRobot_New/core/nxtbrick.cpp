#include "nxtbrick.h"
#include <QtCore>
#include "nxt.h"
#include "connection.h"


#ifdef Q_OS_UNIX
typedef nxt::Nxt_exception NxtException;
#endif
#ifdef Q_OS_WIN32
typedef Nxt_exception NxtException;
#endif

#ifdef Q_OS_UNIX

NxtBrick::NxtBrick(void *connection)
{
    nxt::Connection *conn = static_cast<nxt::Connection*>(connection);
    Q_CHECK_PTR(conn);
    m_brick = new nxt::Brick(conn);
}

QString NxtBrick::playSound()
{
    QString error;
    nxt::Brick *brick = static_cast<nxt::Brick*>(m_brick);
    Q_CHECK_PTR(brick);
    try {
        std::string sound_name = "Laughing 02.rso";
//        brick->play_tone(440, 100, false);
        brick->play_soundfile(sound_name,false,false);

    }
    catch (NxtException &e) {
        error = "Error "+QString::number(e.error_code())+": "+QString::fromAscii(e.what());
    }
    return error;
}
#endif

#ifdef Q_OS_WIN32

NxtBrick::NxtBrick(void *connection)
{
    Connection *conn = static_cast<Connection*>(connection);
    Q_CHECK_PTR(conn);
    m_brick = new Brick(conn);
}

QString NxtBrick::playSound()
{
    QString error;
    Brick *brick = static_cast<Brick*>(m_brick);
    Q_CHECK_PTR(brick);
    try {
        std::string sound_name = "Laughing 02.rso";
//        brick->play_tone(440, 100, false);
        brick->play_soundfile(sound_name,false,false);

    }
    catch (NxtException &e) {
        error = "Error "+QString::number(e.error_code())+": "+QString::fromAscii(e.what());
    }
    return error;
}
#endif
