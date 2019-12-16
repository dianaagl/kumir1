#include "customwindow.h"
#include "customwindow_p.h"

// class CustomWindow

CustomWindow::CustomWindow(QWidget *centralComponent,
                           const QString &moduleName,
                           bool resizableX, bool resizableY,QString pref) :
        QWidget(0, Qt::FramelessWindowHint)
{
    prefix="";
    d = new CustomWindowPrivate;
    s_moduleName = moduleName;
    d->q = this;
    d->init(centralComponent, moduleName, resizableX, resizableY,pref);

}
void CustomWindow::setPrefix(QString pref)
{
    prefix=pref;
    //d->init(centralComponent, moduleName, resizableX, resizableY,prefix);
};
void CustomWindow::setWindowTitle(const QString &title)
{
    d->lbl_title->setText(title);
    QWidget::setWindowTitle(title);
}

QString CustomWindow::windowTitle()
{
    return d->lbl_title->text();
}

void CustomWindow::setStayOnTop(bool v)
{
    if (v) {
        setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
        setVisible(true);
    }
    else {
        setWindowFlags(Qt::FramelessWindowHint);
        setVisible(true);
    }
    d->w_centralWidget->setFocus();
}


bool CustomWindow::isStayOnTop()
{
    return windowFlags() & Qt::WindowStaysOnTopHint;
}

void CustomWindow::show()
{
    setVisible(true);
    d->w_centralWidget->setFocus(Qt::ActiveWindowFocusReason);
}

void CustomWindow::close()
{
    emit closed();
    QWidget::close();
}

void CustomWindow::closeEvent(QCloseEvent *e)
{
//    QRect r(x(), y(), width(), height());
//    if (r.width()>0 && r.height()>0)
//        app()->settings->setValue("Windows/"+d->s_moduleName+"/Geometry",r);
    QWidget::closeEvent(e);
}


void CustomWindow::showMinimized()
{
    QWidget::showMinimized();
}

void CustomWindow::setVisible(bool visible)
{
    QWidget::setVisible(visible);
    d->w_centralWidget->setVisible(visible);
    if (visible)
        d->w_centralWidget->setFocus();
}

// class CustomWindowPrivate

void CustomWindowPrivate::init(QWidget *centralWidget,
                               const QString &moduleName,
                               bool resizableX, bool resizableY,QString prefix)
{
    w_centralWidget = centralWidget;
    s_moduleName = moduleName;
    QVBoxLayout *l0 = new QVBoxLayout;
    l0->setContentsMargins(0,0,0,0);
    l0->setSpacing(0);
    q->setLayout(l0);

#ifndef Q_OS_MAC
    QString pixmapsPath = QCoreApplication::applicationDirPath()+
                          "/PluginStarter/customwindow_pixmaps/";
#endif
#ifdef Q_OS_MAC
    QString pixmapsPath = QCoreApplication::applicationDirPath()+
                          "/../Resources/";
    qDebug()<<pixmapsPath;
#endif

    for (int i=0; i<6; i++) {
        if (i<3) {
            px_topBorder << new QPixmap(pixmapsPath+prefix+QString("top%1.png").arg(i));
            px_bottomBorder << new QPixmap(pixmapsPath+prefix+QString("bottom%1.png").arg(i));
            px_leftBorder << new QPixmap(pixmapsPath+prefix+QString("left%1.png").arg(i));
            px_rightBorder << new QPixmap(pixmapsPath+prefix+QString("right%1.png").arg(i));
        }

        px_stayOnTopButton << new QPixmap(pixmapsPath+QString("stayontop%1.png").arg(i));

        if (i<3) {
            px_minimizeButton << new QPixmap(pixmapsPath+QString("minimize%1.png").arg(i));
            px_closeButton << new QPixmap(pixmapsPath+QString("close%1.png").arg(i));
        }
    }


    w_topBorder = new BorderWidget(q, q, this, Top, px_topBorder, false);
    l0->addWidget(w_topBorder);

    QWidget *www = new QWidget;
    QHBoxLayout *l1 = new QHBoxLayout;
    l1->setContentsMargins(0,0,0,0);
    l1->setSpacing(0);
    www->setLayout(l1);
    l0->addWidget(www);

    w_leftBorder = new BorderWidget(q, q, this, Left, px_leftBorder, resizableX);
    l1->addWidget(w_leftBorder);
    l1->addWidget(centralWidget);

    w_rightBorder = new BorderWidget(q, q, this, Right, px_rightBorder, resizableX);
    l1->addWidget(w_rightBorder);

    w_bottomBorder = new BorderWidget(q, q, this, Bottom, px_bottomBorder, resizableY);
    l0->addWidget(w_bottomBorder);

    QHBoxLayout *ll = new QHBoxLayout;
    ll->setContentsMargins(4, 0, 4, 0);
    //ll->setAlignment(Qt::AlignVCenter);
    w_topBorder->setLayout(ll);

    btn_stayOnTop = new CustomButton(w_topBorder, true,
                                     "Stay on top",
                                     px_stayOnTopButton,
                                     q->tr("Toggle stay on top"));

    QObject::connect ( btn_stayOnTop, SIGNAL(clicked(bool)),
                       q, SLOT( setStayOnTop(bool)) );



    QString labelStyle;
    QFile labelStyleFile(pixmapsPath+"title.properties");
    if (labelStyleFile.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QTextStream ts(&labelStyleFile);
        ts.setAutoDetectUnicode(true);
        labelStyle = ts.readAll();
        labelStyleFile.close();
    }

#if QT_VERSION < 0x040600
    QString tmp;
    for (int i=0; i<labelStyle.length(); i++) {
        if (labelStyle[i].isPrint())
            tmp += labelStyle[i];
    }

    labelStyle = tmp;
#endif

    lbl_title = new CustomLabel(w_topBorder, labelStyle);

    ll->addWidget(lbl_title);
ll->addWidget(btn_stayOnTop);


    btn_minimize = new CustomButton(w_topBorder, false,
                                    "Minimize",
                                    px_minimizeButton,
                                    q->tr("Minimize"));

    QObject::connect ( btn_minimize, SIGNAL(clicked()),
                       q, SLOT(showMinimized()) );

    ll->addWidget(btn_minimize);

    btn_close = new CustomButton(w_topBorder, false,
                                 "Close",
                                 px_closeButton,
                                 q->tr("Close"));

    QObject::connect(btn_close, SIGNAL(clicked()),
                     q, SLOT(close()));

    ll->addWidget(btn_close);

    int maxButtonHeight = qMax(btn_stayOnTop->height(),
                               qMax(btn_minimize->height(), btn_close->height()));
    int topBorderHeight = w_topBorder->height();
    w_topBorder->setFixedHeight(qMax(maxButtonHeight, topBorderHeight));
    startTimer(500);

//    QRect r = app()->settings->value("Windows/"+s_moduleName+"/Geometry",
//                                          QRect(-1,-1,-1,-1)).toRect();
//    if (r.width()>0 && r.height()>0) {
//        q->resize(r.size());
//        q->move(r.topLeft());
//    }
}

void CustomWindowPrivate::timerEvent(QTimerEvent *e)
{
    e->accept();
    QString newTitle = w_centralWidget->windowTitle();
    QString oldTitle = lbl_title->text();
    if (newTitle!=oldTitle) {
        q->setWindowTitle(newTitle);
    }
    QSize newSize;
    newSize.setWidth(w_leftBorder->width()+
                     w_centralWidget->width()+
                     w_rightBorder->width());
    newSize.setHeight(w_topBorder->height()+
                      w_centralWidget->height()+
                      w_bottomBorder->height());
    if (q->size()!=newSize) {
        q->resize(newSize);
    }
//    qDebug() << "Custom window central widget size: " << w_centralWidget->size();
//    qDebug() << "This size" << q->size();
}

// class BorderWidget

BorderWidget::BorderWidget(QWidget *parent,
                           QWidget *window,
                           class CustomWindowPrivate *dd,
                           char border,
                           const QList<QPixmap*> &pixmaps, bool resizable)
                               : QWidget(parent)
                               , e_border(border)
                               , b_resizable(resizable)
                               , b_moving(false)
{
    d = dd;
    px_border = pixmaps;
    w_window = window;
    int minSize0, minSize1, minSize2, minSizeMax;
    if (border=='t' || border=='b') {
        minSize0 = pixmaps[0]->height();
        minSize1 = pixmaps[1]->height();
        minSize2 = pixmaps[2]->height();
    }
    else {
        minSize0 = pixmaps[0]->width();
        minSize1 = pixmaps[1]->width();
        minSize2 = pixmaps[2]->width();
    }
    minSizeMax = qMax(minSize0, qMax(minSize1, minSize2));
    if (border=='t' || border=='b') {
        setFixedHeight(minSizeMax);
    }
    else {
        setFixedWidth(minSizeMax);
    }
    if (resizable && (border=='l' || border=='r')) {
        setCursor(Qt::SizeHorCursor);
    }
    else if (resizable && border=='b') {
        setCursor(Qt::SizeVerCursor);
    }
}

void BorderWidget::paintEvent(QPaintEvent *e)
{
    QPainter p(this);

    // left or top corner
    int first_w = px_border[0]->width();
    int first_h = px_border[0]->height();
    QRect firstRect(0,0,first_w,first_h);
    if (e->rect().intersects(firstRect))
        p.drawPixmap(firstRect, *(px_border[0]));

    // middle of border
    QRect midRect;
    int w = width();
    int h = height();
    int x = (e_border=='t'||e_border=='b')? first_w : 0;
    int y = (e_border=='t'||e_border=='b')? 0 : first_h;
    int mid_w = px_border[1]->width();
    int mid_h = px_border[1]->height();
    while (x<w && y<h) {
        midRect = QRect(x,y,mid_w,mid_h);
        if (e->rect().intersects(midRect))
            p.drawPixmap(midRect, *(px_border[1]));
        if (e_border=='t'||e_border=='b') {
            x += mid_w;
        }
        else {
            y += mid_h;
        }
    }

    // right or bottom corner
    int last_w = px_border[2]->width();
    int last_h = px_border[2]->height();
    QRect lastRect;
    if (e_border=='t'||e_border=='b') {
        lastRect = QRect(w-last_w, 0, last_w, last_h);
    }
    else {
        lastRect = QRect(0, h-last_h, last_w, last_h);
    }
    if (e->rect().intersects(lastRect))
        p.drawPixmap(lastRect, *(px_border[2]));

    e->accept();
}

void BorderWidget::mousePressEvent(QMouseEvent *e)
{
    if (e_border=='t') {
        setCursor(Qt::SizeAllCursor);
        b_moving  = true;
        p_start = e->globalPos();
        e->accept();
    }
    else if (b_resizable) {
        b_moving  = true;
        p_start = e->globalPos();
        e->accept();
    }
    else {
        e->ignore();
    }
}

void BorderWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (b_moving) {
        QPoint d = e->globalPos() - p_start;
        int distance = (int) ( sqrt( (double) (d.x()*d.x() + d.y()*d.y()) ) );
        p_start = e->globalPos();
        if ( b_moving || distance >= QApplication::startDragDistance() ) {
            mouseDragAction(d);
        }
    }
    else {
        e->ignore();
    }
}

void BorderWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if (b_moving) {
        b_moving = false;
        if (e_border=='t')
            unsetCursor();
        e->accept();
    }
    else {
        e->ignore();
    }
}

void BorderWidget::mouseDragAction(const QPoint &offset)
{
    if (b_resizable) {
        Q_CHECK_PTR(w_window);
        if (e_border=='l') {
            QPoint newPos = w_window->pos();
            newPos.setX(newPos.x()+offset.x());
            w_window->move(newPos);
            w_window->setFixedWidth(w_window->width()-offset.x());
            d->w_centralWidget->setFixedWidth(w_window->width()-
                                              d->w_leftBorder->width()-
                                              d->w_rightBorder->width());
        }
        if (e_border=='r') {
            w_window->setFixedWidth(w_window->width()+offset.x());
            d->w_centralWidget->setFixedWidth(w_window->width()-
                                              d->w_leftBorder->width()-
                                              d->w_rightBorder->width());
        }
        if (e_border=='b') {
            w_window->setFixedHeight(w_window->height()+offset.y());
            d->w_centralWidget->setFixedHeight(w_window->height()-
                                               d->w_topBorder->height()-
                                               d->w_bottomBorder->height());
        }
        update();
    }
    else if (e_border=='t') {
        Q_CHECK_PTR(w_window);
        w_window->move(w_window->pos()+offset);
    }
}

CustomButton::CustomButton(QWidget *parent, bool checkable,
                           const QString &name,
                           const QList<QPixmap*> &pixmaps,
                           const QString &toolTip)
                               : QAbstractButton(parent)
{
    setText(name);
    setCheckable(checkable);
    setToolTip(toolTip);
    px = pixmaps;
    b_pressed = b_hovered = false;
    int maxW = 0, maxH = 0;
    for (int i=0; i<pixmaps.size(); i++) {
        maxW = qMax(maxW, pixmaps[i]->width());
        maxH = qMax(maxH, pixmaps[i]->height());
    }
    setFixedSize(maxW, maxH);
    setMouseTracking(true);
}

void CustomButton::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    int index;
    if (isPressed()) {
        index = 2;
    }
    else if (isHovered()) {
        index = 0;
    }
    else {
        index = 0;
    }
    if (isCheckable()) {
        if (!isChecked())
            index += 3;
    }

    if (index<px.size()) {
        QPixmap *pixmap = px[index];
        Q_CHECK_PTR(pixmap);
        p.drawPixmap(0, 0, pixmap->width(), pixmap->height(), *pixmap);
        e->accept();
    }
    else {
        e->ignore();
    }
}

void CustomButton::enterEvent(QEvent *e)
{
    e->accept();
    b_hovered = true;
}

void CustomButton::leaveEvent(QEvent *e)
{
    e->accept();
    b_hovered = false;
}

void CustomButton::mousePressEvent(QMouseEvent *e)
{
    b_pressed = true;
    QAbstractButton::mousePressEvent(e);
}

void CustomButton::mouseReleaseEvent(QMouseEvent *e)
{
    b_pressed = false;
    QAbstractButton::mouseReleaseEvent(e);
}

// class CustomLabel

CustomLabel::CustomLabel(QWidget *parent, const QString &style)
    : QWidget(parent)
{
    QStringList props = style.split(";");
    QString name, value;
    QStringList p;
    i_padding = 0;
    fl_textAlignment = Qt::AlignLeft;
    f_font = font();
    col_foreground = QColor("black");
    for (int i=0; i<props.size(); i++) {
        p = props.at(i).split(":");
        if (p.size()==2) {
            name = p[0].toLower().trimmed(); value = p[1].trimmed();
            if (name=="color") {
                col_foreground = QColor(value);
            }
            if (name=="size") {
                QString measure = "pt";
                if (value.endsWith("px")) {
                    measure = "px";
                }
                value.remove("pt");
                value.remove("px");
                int v = value.toInt();
                if (measure=="px")
                    f_font.setPixelSize(v);
                if (measure=="pt")
                    f_font.setPointSize(v);
            }
            if (name=="alignment") {
                value = value.toLower();
                if (value=="center")
                    fl_textAlignment = Qt::AlignHCenter;
                else if (value=="right")
                    fl_textAlignment = Qt::AlignRight;
                else
                    fl_textAlignment = Qt::AlignLeft;
            }
            if (name=="padding") {
                i_padding = value.toInt();
            }
        }
    }
}

void CustomLabel::setText(const QString &v)
{
    s_text = v;
    update();
}

void CustomLabel::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    f_font.setBold(true);
    f_font.setPointSize(28);
#ifdef WIN32
  f_font.setPointSize(22);
  qDebug()<<"WIN 32 FONTS";
#endif
    p.setFont(f_font);
    p.setPen(QColor(255,255,255));
    QTextOption opt;
    opt.setAlignment(fl_textAlignment|Qt::AlignVCenter);
    opt.setWrapMode(QTextOption::NoWrap);
    p.drawText(QRect(i_padding,0,width()-2*i_padding,height()),s_text,opt);
    e->accept();

}
