#include <QtGlobal>
#ifdef Q_OS_WIN32
#include "secondarywindow.h"
#include "application.h"
#include <windows.h>

SecondaryWindow::SecondaryWindow(QWidget * innerWidget, const QString &moduleName)
    : QWidget(0)
    , w_innerWidget(innerWidget)
    , s_moduleName(moduleName)
{
    innerWidget->setParent(this);
    QVBoxLayout *l = new QVBoxLayout;
    setLayout(l);
    l->setContentsMargins(0,0,0,0);
    l->addWidget(innerWidget);
    s_stayOnTop = tr("Stay on top");
    s_notStayOnTop = tr("Disable stay on top");
    ID_STAYONTOP = 9001;
    QPixmap stayOnTop0(":/stayontop0.png");
    QPixmap stayOnTop1(":/stayontop1.png");
    h_image0 = stayOnTop0.toWinHBITMAP(QPixmap::Alpha);
    h_image1 = stayOnTop1.toWinHBITMAP(QPixmap::Alpha);
    bool onTop = app()->settings->value("Windows/"+s_moduleName+"/StayOnTop", false).toBool();
    startTimer(500);
    b_created = false;
    QRect r = app()->settings->value("Windows/"+s_moduleName+"/Geometry",
                                          QRect(0,0,0,0)).toRect();
    QSize minimumWindowSize = innerWidget->minimumSize();
    r.setWidth(qMax(r.width(), minimumWindowSize.width()));
    r.setHeight(qMax(r.height(), minimumWindowSize.height()));

    bool isFixedW = innerWidget->sizePolicy().horizontalPolicy()==QSizePolicy::Fixed;
    bool isFixedH = innerWidget->sizePolicy().verticalPolicy()==QSizePolicy::Fixed;

    if (isFixedW) {
        setFixedWidth(r.width());
    }
    if (isFixedH) {
        setFixedHeight(r.height());
    }

    resize(r.size());
    move(r.topLeft());
    setStayOnTop(onTop);
    b_created = true;
}

void SecondaryWindow::setStayOnTop(bool v)
{
    setWindowFlags(v? Qt::WindowStaysOnTopHint | Qt::Window : Qt::Window);
    setVisible(b_created);
    h_windowMenu = GetSystemMenu(winId(), false);
    MENUITEMINFO mi;
    mi.cbSize = sizeof(MENUITEMINFO);
    mi.fMask = MIIM_STRING | MIIM_FTYPE | MIIM_ID | MIIM_CHECKMARKS | MIIM_STATE;
    mi.fType = MFT_STRING;
    mi.dwTypeData = v? (TCHAR*)s_notStayOnTop.utf16() : (TCHAR*)s_stayOnTop.utf16();
    mi.wID = ID_STAYONTOP;
    mi.cch = v? s_notStayOnTop.length() : s_stayOnTop.length();
    mi.hbmpUnchecked = h_image0;
    mi.hbmpChecked = h_image1;
    mi.fState = v? MFS_CHECKED : MFS_UNCHECKED;
    InsertMenuItem(h_windowMenu, 0, true , &mi);
}

bool SecondaryWindow::isStayOnTop() const
{
    return windowFlags() & Qt::WindowStaysOnTopHint;
}

bool SecondaryWindow::winEvent(MSG *message, long *result)
{
    Q_UNUSED(result);
    bool filtered = false;
    if (message->message==WM_SYSCOMMAND) {
        if (LOWORD(message->wParam)==9001) {
            filtered = true;
            setStayOnTop(!isStayOnTop());
//            if (windowFlags() & Qt::WindowStaysOnTopHint) {
//                setWindowFlags(0);
//                setVisible(true);
//                CheckMenuItem(h_windowMenu, 9001, MF_BYCOMMAND | MF_UNCHECKED);
//            }
//            else {
//                setWindowFlags(Qt::WindowStaysOnTopHint);
//                setVisible(true);
//                CheckMenuItem(h_windowMenu, 9001, MF_BYCOMMAND | MF_CHECKED);
//            }
        }
    }
    return filtered;
}

void SecondaryWindow::timerEvent(QTimerEvent *e)
{
    e->accept();
    QString newTitle = w_innerWidget->windowTitle();
    QString oldTitle = windowTitle();
    if (newTitle!=oldTitle) {
        setWindowTitle(newTitle);
    }
    QSize newSize(w_innerWidget->size());
    if (size()!=newSize) {
        resize(newSize);
    }
}

void SecondaryWindow::showEvent(QShowEvent *e)
{
    QWidget::showEvent(e);
    emit shown();
}

void SecondaryWindow::closeEvent(QCloseEvent *e)
{
    QRect r(x(), y(), width(), height());
    if (r.width()>0 && r.height()>0)
        app()->settings->setValue("Windows/"+s_moduleName+"/Geometry",r);
    app()->settings->setValue("Windows/"+s_moduleName+"/StayOnTop", isStayOnTop());
    QWidget::closeEvent(e);
    emit closed();
}
#endif
