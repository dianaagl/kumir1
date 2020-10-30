#include "pult_webkit.h"

class PultWebPage:
        public QWebEnginePage
{
protected:
    virtual void javaScriptAlert ( QWebEnginePage * frame, const QString & msg );
};

void PultWebPage::javaScriptAlert(QWebEnginePage *frame, const QString &msg)
{
    QWidget *p = frame->view();
    const QString t = frame->title();
    QMessageBox::information(p,
                             t,
                             msg);
}

RobotPultWK::RobotPultWK(QWidget *parent)
    : QWebEngineView(parent)
{
    qDebug() << "RobotPultWK::RobotPultWK; Line = " << __LINE__;
    setPage(new PultWebPage);
    qDebug() << "RobotPultWK::RobotPultWK; Line = " << __LINE__;
    b_hasLink = true;
    connect(page(),
            SIGNAL(javaScriptWindowObjectCleared()),
            this,
            SLOT(setJavaScriptObjects()));
    connect(page(),
            SIGNAL(titleChanged(QString)),
            this,
            SLOT(setWindowTitle(QString)));
    QUrl url;
#ifdef Q_OS_MAC
    url = QUrl::fromLocalFile(QApplication::applicationDirPath()+
                              "/Kumir/RobotPult/pult.html");
#else
    url = QUrl::fromLocalFile(QApplication::applicationDirPath()+
                              "/Kumir/RobotPult/pult.html");
#endif
    qDebug() << "RobotPultWK::RobotPultWK; Line = " << __LINE__;
    load(url);
    qDebug() << "RobotPultWK::RobotPultWK; Line = " << __LINE__;
}

void RobotPultWK::setWindowSize(int w, int h)
{
    setFixedSize(w,h);
}

void RobotPultWK::setJavaScriptObjects()
{
//    QWebEngineSettings::globalSettings()->setAttribute(
//            QWebSettings::DeveloperExtrasEnabled, true);
//    this->addToJavaScriptWindowObject("pult", this);
}

void RobotPultWK::evaluateCommand(const QString &text)
{
    s_lastCommand = text;
    if (text==QString::fromUtf8("влево")) {
        emit goLeft();
    }
    else if (text==QString::fromUtf8("вправо")) {
        emit goRight();
    }
    else if (text==QString::fromUtf8("вверх")) {
        emit goUp();
    }
    else if (text==QString::fromUtf8("вниз")) {
        emit goDown();
    }
    else if (text==QString::fromUtf8("закрасить")) {
        emit Color();
    }
    else if (text==QString::fromUtf8("клетка закрашена")) {
        emit Colored();
    }
    else if (text==QString::fromUtf8("клетка чистая")) {
        emit Clean();
    }
    else if (text==QString::fromUtf8("температура")) {
        emit Temp();
    }
    else if (text==QString::fromUtf8("радиация")) {
        emit Rad();
    }
    else if (text==QString::fromUtf8("сверху стена")) {
        emit hasUpWall();
    }
    else if (text==QString::fromUtf8("снизу стена")) {
        emit hasDownWall();
    }
    else if (text==QString::fromUtf8("слева стена")) {
        emit hasLeftWall();
    }
    else if (text==QString::fromUtf8("справа стена")) {
        emit hasRightWall();
    }
    else if (text==QString::fromUtf8("сверху свободно")) {
        emit noUpWall();
    }
    else if (text==QString::fromUtf8("снизу свободно")) {
        emit noDownWall();
    }
    else if (text==QString::fromUtf8("слева свободно")) {
        emit noLeftWall();
    }
    else if (text==QString::fromUtf8("справа свободно")) {
        emit noRightWall();
    }
}

void RobotPultWK::setHasLink(bool hasLink)
{
    b_hasLink = hasLink;
    if (hasLink)
        page()->runJavaScript("onSetLink(true)");
    else
        page()->runJavaScript("onSetLink(false)");
}

void RobotPultWK::copyToClipboard(const QString &data)
{
    QClipboard *clipboard = qApp->clipboard();
    QMimeData *d = new QMimeData();
    d->setText(data);
    clipboard->setMimeData(d);
}

void RobotPultWK::setCommandResult(const QString &text)
{
    const QString instruction =
            QString("onCommandFinished(\"%1\", \"%2\");")
            .arg(s_lastCommand)
            .arg(text);

    page()->runJavaScript(instruction);
}
