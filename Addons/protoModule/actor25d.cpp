#include "actor25d.h"
#include "scene25d.h"
#include "util.h"

#include "graphicsimageitem.h"

#include <QtSvg>

Actor25D::Actor25D(QObject *parent,
                   QGraphicsScene *scene,
                   ScriptEnvironment *env,
                   const QString &moduleRoot,
                   const QString &className,
                   const QString &instanceName) :
        AbstractActor(parent, env, moduleRoot, className, instanceName),
        m_graphicsScene(scene)
{
    m_scene = 0;
    g_pixmap = new GraphicsImageItem();
    g_targetPixmap = new GraphicsImageItem();
    scene->addItem(g_pixmap);
    scene->addItem(g_targetPixmap);
    b_visible = false;
    b_moving = false;
    r_moveStatus = 0.0;


    Q_CHECK_PTR(m_graphicsScene);
    Q_CHECK_PTR(m_scriptEngine);

}

void Actor25D::setScene(Scene25D *sc)
{
    m_scene = sc;
}

void Actor25D::setPosition(quint16 xx, quint16 yy, quint16 zz)
{
    i_x = xx; i_y = yy; i_z = zz;
    if (m_scene && g_pixmap) {
        setPixmapPosition(g_pixmap, g_pixmap->image().size(), xx, yy, zz);
    }
}

void Actor25D::setPixmapPosition(GraphicsImageItem *item, const QSize& pixmapSize, quint16 x, quint16 y, quint16 z)
{

    Q_UNUSED(pixmapSize);

    QPointF apos = mapToIsometricCoordinates( (x-m_scene->width()/2.0)*50,
                                              (y-m_scene->length()/2.0)*50,
                                              (z-m_scene->height()/2.0)*50);
//    QPointF offset(-pixmapSize.width()/4.0, 50-pixmapSize.height());
   QPointF offset(0, -22);

    item->setPos(apos+offset);

    item->setZValue(m_scene->baseZOrder(x, y, z)+0.3);
//    qDebug() << "setPixmapPosition: " << apos+offset;
//    qDebug() << "apos: " << apos;
//    qDebug() << "offset: " << offset;
}

void Actor25D::setCurrentFrame(quint16 frameNo)
{
    i_currentFrame = frameNo;
    if (m_frames.contains(frameNo)) {
        if (!b_moving) {
//            if (g_pixmap==NULL) {
//                g_pixmap = new GraphicsImageItem();
//                g_pixmap->setVisible(b_visible);
//                m_graphicsScene->addItem(g_pixmap);
//            }
//            if (g_targetPixmap==NULL) {
//                g_pixmap->setImage(m_frames[frameNo]);
//                setPixmapPosition(g_pixmap, m_frames[frameNo].size(), i_x, i_y, i_z);
//            }
            QImage image = m_frames[frameNo];
            qDebug() << image.size();
            g_pixmap->setImage(image);
            setPixmapPosition(g_pixmap, image.size(), i_x, i_y, i_z);
        }
        else {
            if (r_moveStatus>1.0)
                r_moveStatus = 1.0;
            QImage pix = m_frames[frameNo];
            QImage pix1;
            QImage pix2;

            setPixmapPosition(g_pixmap, pix.size(), i_x, i_y, i_z);
            setPixmapPosition(g_targetPixmap, pix.size(), i_tx, i_ty, i_tz);

            QPoint sourceGPos = g_pixmap->pos().toPoint();
            QPoint targetGPos = g_targetPixmap->pos().toPoint();

            QRect sourceRect(sourceGPos.x(),
                             sourceGPos.y(),
                             pix.width(),
                             pix.height());
            QRect targetRect(targetGPos.x(),
                             targetGPos.y(),
                             pix.width(),
                             pix.height());
            QPair<QImage, QImage> pixmaps = splitPixmap(pix,
                                                          sourceRect,
                                                          targetRect,
                                                          r_moveStatus);
            pix1 = pixmaps.first;
            pix2 = pixmaps.second;
            g_pixmap->setImage(pix1);
            g_targetPixmap->setImage(pix2);
        }
    }
    else {
        qDebug() << "No frame: " << frameNo;
    }
//    if (g_pixmap)
//        g_pixmap->update();
//    if (g_targetPixmap)
//        g_targetPixmap->update();
//    emit updateRequest();
//    QApplication::processEvents();
}

void Actor25D::loadActorFromDir(const QString &actorName)
{
    if (m_scriptEngine->isEvaluating()) {
        m_scriptEngine->abortEvaluation();
    }
    m_scriptEngine->clearExceptions();
    // 1. Load pixmaps
    m_frames.clear();
//    if (g_pixmap)
//        delete g_pixmap;
    QRegExp rxFrame("frame_\\d+\\.(png|gif|bmp|jpg|svg)");
    QStringList framesList = getEntryList(s_moduleRoot, actorName+"/2.5D/frames");
    for (int i=0; i<framesList.count(); i++) {
        QString entry = framesList[i];
        if (entry=="." || entry=="..")
            continue;
        if (rxFrame.exactMatch(entry)) {
            QImage frame;
            if (entry.endsWith(".svg")) {
                QByteArray data = getFileData(s_moduleRoot, actorName+"/2.5D/frames/"+entry);
                frame = normalizePixmap2(normalizePixmap1(data));
            }
            else {
                QByteArray data = getFileData(s_moduleRoot, actorName+"/2.5D/frames/"+entry);
                frame = normalizePixmap2(normalizePixmap1(QImage::fromData(data)));
            }
            quint16 frameNo = (quint16)entry.mid(6,entry.length()-10).toUInt();
            m_frames[frameNo] = frame;
        }
    }

    // 2. Load scripts
    QStringList scripts25DList = getEntryList(s_moduleRoot, actorName+"/2.5D/scripts");
    for (int i=0; i<scripts25DList.count(); i++) {
        QString entry = scripts25DList[i];
        if (entry=="." || entry=="..")
            continue;
        if (entry.endsWith(".js")) {
            QByteArray rawData = getFileData(s_moduleRoot, actorName+"/2.5D/scripts/"+entry);

            QTextStream ts(rawData);
            ts.setCodec("UTF-8");
            QString data = ts.readAll();
            QScriptValue ret = m_scriptEngine->evaluate(data, actorName+"/2.5D/scripts/"+entry);
            if (ret.isError()) {
                qDebug() << "Execution error while loading: " << actorName+"/2.5D/scripts/"+entry;
                qDebug() << ret.toString();
            }

        }
    }

    QStringList scriptsList = getEntryList(s_moduleRoot, actorName+"/scripts");
    for (int i=0; i<scriptsList.count(); i++) {
        QString entry = scriptsList[i];
        if (entry=="." || entry=="..")
            continue;
        if (entry.endsWith(".js")) {
            QByteArray rawData = getFileData(s_moduleRoot, actorName+"/scripts/"+entry);

            QTextStream ts(rawData);
            ts.setCodec("UTF-8");
            QString data = ts.readAll();
            QScriptValue ret = m_scriptEngine->evaluate(data, actorName+"/scripts/"+entry);
            if (ret.isError()) {
                qDebug() << "Execution error while loading: " << actorName+"/scripts/"+entry;
                qDebug() << ret.toString();
            }

        }
    }
}

void Actor25D::beginMove(quint16 x, quint16 y, quint16 z)
{
//    qDebug() << "Actor: begin move: " << x << ", " << y;
    i_tx = x; i_ty = y; i_tz = z;
    g_targetPixmap->setVisible(true);
//    m_graphicsScene->addItem(g_targetPixmap);
    b_moving = true;
    r_moveStatus = 0.0;

}

void Actor25D::moveBy(qreal pathPercentage)
{
    if (r_moveStatus>1.0)
        return;
    r_moveStatus += pathPercentage;
    setCurrentFrame(currentFrame());
}

void Actor25D::endMove()
{
//    qDebug() << "Actor: end move";
//    m_graphicsScene->removeItem(g_pixmap);
    g_pixmap->setVisible(false);
//    delete g_pixmap;
    GraphicsImageItem *tmp = g_pixmap;
    g_pixmap = g_targetPixmap;
    g_targetPixmap = tmp;
    r_moveStatus = 0.0;
    b_moving = false;
    i_x = i_tx;
    i_y = i_ty;
    i_z = i_tz;
    setCurrentFrame(currentFrame());
//    g_pixmap->update();
//    emit updateRequest();
}

bool Actor25D::visible() const
{
    if (g_pixmap)
        return g_pixmap->isVisible();
    else
        return b_visible;
}

void Actor25D::setVisible(bool v)
{
    b_visible = v;
    if (g_pixmap) {
        g_pixmap->setVisible(v);
//        g_pixmap->update();
//        emit updateRequest();
    }
}
