#include "scene25d.h"
#include "util.h"
#include "graphicsimageitem.h"

#include <QtSvg>

Scene25D::Scene25D(QObject *parent,
                   const QString &moduleRoot,
                   const QString &sceneClass,
                   QGraphicsScene *graphicsScene,
                   ScriptEnvironment *env):
        AbstractScene(parent, moduleRoot, sceneClass, env),
        m_scene(graphicsScene)
{
    Q_CHECK_PTR( m_scene );
    Q_CHECK_PTR( m_scriptEngine );
}

void Scene25D::setSceneRootDir(const QString &sceneRootDir)
{
    if (m_scriptEngine->isEvaluating()) {
        m_scriptEngine->abortEvaluation();
    }
    m_scriptEngine->clearExceptions();
    clearScene();
    m_cells.clear();
    s_sceneRootDir = sceneRootDir;
    Q_CHECK_PTR ( this );
    Q_CHECK_PTR ( m_scriptEngine );
//    m_debugger->action(QScriptEngineDebugger::InterruptAction)->trigger();
    QStringList scripts25D = getEntryList(s_moduleRoot,
                                          sceneRootDir.mid(s_moduleRoot.length()+1)+"/2.5D/scripts");
    for (int i=0; i<scripts25D.count(); i++) {
        QString entry = scripts25D[i];
        if (entry=="." || entry=="..")
            continue;
        if (entry.endsWith(".js")) {
            QByteArray rawData = getFileData(s_moduleRoot,
                                             sceneRootDir.mid(s_moduleRoot.length()+1)+"/2.5D/scripts/"+entry);

            QTextStream ts(rawData);
            ts.setCodec("UTF-8");
            QString data = ts.readAll();
            QScriptValue ret = m_scriptEngine->evaluate(data, "/2.5D/scripts/"+entry);
            if (ret.isError()) {
                qDebug() << "Execution error: ";
                qDebug() << ret.toString();
            }
        }
    }

    QStringList scripts = getEntryList(s_moduleRoot,
                                          sceneRootDir.mid(s_moduleRoot.length()+1)+"/scripts");
    for (int i=0; i<scripts.count(); i++) {
        QString entry = scripts[i];
        if (entry=="." || entry=="..")
            continue;
        if (entry.endsWith(".js")) {
            QByteArray rawData = getFileData(s_moduleRoot,
                                             sceneRootDir.mid(s_moduleRoot.length()+1)+"/scripts/"+entry);

            QTextStream ts(rawData);
            ts.setCodec("UTF-8");
            QString data = ts.readAll();
            QScriptValue ret = m_scriptEngine->evaluate(data, "/scripts/"+entry);
            if (ret.isError()) {
                qDebug() << "Execution error: ";
                qDebug() << ret.toString();
            }
        }
    }
}

void Scene25D::clearScene()
{
    QSet<QGraphicsItem*> pointers;
    for (int z=0; z<m_cells.size(); z++) {
        for (int y=0; y<m_cells[z].size(); y++) {
            for (int x=0; x<m_cells[z][y].size(); x++) {
                if (m_cells[z][y][x].view) {
                    CellView25D* v = static_cast<CellView25D*>(m_cells[z][y][x].view);
                    pointers.insert(v->pxBack);
                    pointers.insert(v->pxFront);
                    pointers.insert(v->pxBottom);
                    pointers.insert(v->pxTop);
                    pointers.insert(v->pxLeft);
                    pointers.insert(v->pxRight);
                    delete v;
                }
            }
        }
    }
    QList<QGraphicsItem*> plist = pointers.toList();
    for (int i=0; i<plist.size(); i++) {
        delete plist[i];
    }
}

void Scene25D::initScene()
{
    for (int z=0; z<m_cells.size(); z++) {
        for (int y=0; y<m_cells[z].size(); y++) {
            for (int x=0; x<m_cells[z][y].size(); x++) {
                CellView25D *view = new CellView25D;
                view->baseZOrder = 10.0*(m_cells[0].size()-y) + 10.0*x + 10.0*z;
                m_cells[z][y][x].view = view;
            }
        }
    }
}

EdgeStyle Scene25D::loadStyle(const QString &styleName, const QString &borderName)
{
    if (map_pixmaps.contains(styleName))
        return map_pixmaps.value(styleName);

    EdgeStyle style;

    QStringList suffices;
    suffices << ".css" << ".svg" << ".png" << ".gif" << ".jpg" << ".jpeg" << ".bmp";
    const QString relPath = s_sceneRootDir.mid(s_moduleRoot.length()+1)+"/2.5D/pixmaps";
    QStringList entries = getEntryList(s_moduleRoot, relPath);

    bool found = false;
    QString fullPath;
    foreach (const QString &suffix, suffices) {
        const QString fileName = styleName+suffix;
        if (entries.contains(fileName)) {
            found = true;
            fullPath = relPath+"/"+styleName+suffix;
            break;
        }
    }

    if (found) {
        if (fullPath.endsWith(".css")) {
            style = parseEdgeCSS(s_moduleRoot, fullPath);
            style.polygon = isometricTransform(borderName).mapToPolygon(QRect(0,0,51,51));
        }
        else if (fullPath.endsWith(".svg")) {
            QByteArray rawData = getFileData(s_moduleRoot, fullPath);
            QSvgRenderer renderer(rawData);
            QImage img(52,52,QImage::Format_ARGB32);
            QPainter p(&img);
            renderer.render(&p);
            style.pixmap = img.transformed(isometricTransform(borderName),Qt::SmoothTransformation);
        }
        else {
            QByteArray rawData = getFileData(s_moduleRoot, fullPath);
            style.pixmap = QImage::fromData(rawData).scaledToHeight(52,Qt::SmoothTransformation).scaledToWidth(52,Qt::SmoothTransformation);
            style.pixmap = style.pixmap.transformed(isometricTransform(borderName),Qt::SmoothTransformation);
        }
        map_pixmaps.insert(styleName, style);
    }
    else {
        style.brush = QBrush(QColor("lightgray"));
        style.pen = QPen(QColor("black"),1);
        style.polygon = isometricTransform(borderName).mapToPolygon(QRect(0,0,51,51));
    }
    return style;
}


EdgeStyle Scene25D::parseEdgeCSS(const QString &moduleRoot, const QString &fileName)
{
    EdgeStyle style;
    style.pen = QPen(QColor(0,0,0,0),1);
    QByteArray rawData = getFileData(moduleRoot, fileName);

    QStringList lines = QString::fromUtf8(rawData).split(QRegExp("[\n;]"));
    QString line;
    for (int i=0; i<lines.count(); i++) {
        line = lines[i].trimmed();
        if (line.startsWith("background-color:")) {
            style.brush = QColor(line.mid(17).trimmed());
        }
        if (line.startsWith("background-image:")) {
            QString relPath = line.mid(17).trimmed();
            QFileInfo fi(fileName);
            QString absPath = fi.absoluteDir().absoluteFilePath(relPath);
            QImage img(absPath);
            style.brush = QBrush(img);
        }
        if (line.startsWith("border-color:")) {
            style.pen.setColor(line.mid(13).trimmed());
        }
        if (line.startsWith("border-width:")) {
            style.pen.setWidthF(line.mid(13).trimmed().toDouble());
        }
    }

    return style;
}

qreal Scene25D::baseZOrder(quint16 x, quint16 y, quint16 z) const
{
    if (z>=m_cells.size() || y>=m_cells[0].size() || x>=m_cells[0][0].size()) {
        qDebug() << "setBorderPixmapState: index " << x << ", " << y << ", " << z << " out of scene, ignoring action!";
        return 0.0;
    }

    if (m_cells[z][y][x].view) {
        return static_cast<CellView25D*>(m_cells[z][y][x].view)->baseZOrder;
    }
    else {
        return 0.0;
    }

}

void Scene25D::setEdgeStyle(quint16 xx, quint16 yy, quint16 zz, const QString &borderName, const QString &styleFile)
{
    const QString border = borderName.trimmed().toLower();

    EdgeStyle style = loadStyle(styleFile, border);

    if (zz>=m_cells.size() || yy>=m_cells[0].size() || xx>=m_cells[0][0].size()) {
        qDebug() << "setBorderPixmapState: index " << xx << ", " << yy << ", " << zz << " out of scene, ignoring action!";
        return;
    }

    Q_ASSERT ( m_cells.size() > 0 );

    qreal x, y, z;

    int xMax = 0;
    int yMax = 0;
    int zMax = 0;

    zMax = m_cells.size();
    yMax = m_cells[0].size();
    xMax = m_cells[0][0].size();

    z = (zz-zMax/2.0) * 50;
    y = (yy-yMax/2.0) * 50;
    x = (xx-xMax/2.0) * 50;

    CellView25D* c = static_cast<CellView25D*>(m_cells[zz][yy][xx].view);

    QSet<QGraphicsItem*> itemsToDelete;

    if (border=="top") {
        if (c->pxTop) {
            itemsToDelete.insert(c->pxTop);
            c->pxTop = 0;
        }
        if (zz<(zMax-1)) {
            CellView25D *nc = static_cast<CellView25D*>(m_cells[zz+1][yy][xx].view);
            if (nc->pxBottom) itemsToDelete.insert(nc->pxBottom);
            nc->pxBottom = NULL;
        }
    }
    else if (border=="right") {
        if (c->pxRight) {
            itemsToDelete.insert(c->pxRight);
            c->pxRight = 0;
        }
        if (xx<(xMax-1)) {
            CellView25D *nc = static_cast<CellView25D*>(m_cells[zz][yy][xx+1].view);
            if (nc->pxLeft) itemsToDelete.insert(nc->pxLeft);
            nc->pxLeft = NULL;
        }
    }
    else if (border=="bottom") {
        if (c->pxBottom) {
            itemsToDelete.insert(c->pxBottom);
            c->pxBottom = 0;
        }
        if (zz>0) {
            CellView25D *nc = static_cast<CellView25D*>(m_cells[zz-1][yy][xx].view);
            if (nc->pxTop) itemsToDelete.insert(nc->pxTop);
            nc->pxTop = NULL;
        }
    }
    else if (border=="left") {
        if (c->pxLeft) {
            itemsToDelete.insert(c->pxLeft);
            c->pxLeft = 0;
        }
        if (xx>0) {
            CellView25D *nc = static_cast<CellView25D*>(m_cells[zz][yy][xx-1].view);
            if (nc->pxRight) itemsToDelete.insert(nc->pxRight);
            nc->pxRight = NULL;
        }
    }
    else if (border=="front") {
        if (c->pxFront) {
            itemsToDelete.insert(c->pxFront);
            c->pxFront = 0;
        }
        if (yy<(yMax-1)) {
            CellView25D *nc = static_cast<CellView25D*>(m_cells[zz][yy+1][xx].view);
            if (nc->pxBack) itemsToDelete.insert(nc->pxBack);
            nc->pxBack = NULL;
        }
    }
    else if (border=="back") {
        if (c->pxBack) {
            itemsToDelete.insert(c->pxBack);
            c->pxBack = 0;
        }
        if (yy>0) {
            CellView25D *nc = static_cast<CellView25D*>(m_cells[zz][yy-1][xx].view);
            if (nc->pxFront) itemsToDelete.insert(nc->pxFront);
            nc->pxFront = NULL;
        }
    }
    else {
        qDebug() << "Invalid border name: " << border;
        return;
    }

    QList<QGraphicsItem*> dellist = itemsToDelete.toList();
    foreach (QGraphicsItem *di, dellist) {
//        qDebug() << "Deleting item from scene";
        if (di->scene()) di->scene()->removeItem(di);
//        delete di;
    }

    QGraphicsItem *it = NULL;

    if (!style.pixmap.isNull()) {
        GraphicsImageItem *px = new GraphicsImageItem;
        px->setImage(style.pixmap);
        it = px;
    }
    else {
        QGraphicsPolygonItem *polygon = new QGraphicsPolygonItem;
        polygon->setPolygon(style.polygon);
        polygon->setBrush(style.brush);
        polygon->setPen(style.pen);
        it = polygon;
    }

    QPointF position;

    QGraphicsItem *alignmentBase = NULL;
    if (c->pxBottom)
        alignmentBase = c->pxBottom;

    if (border=="bottom" || border=="top") {
        position = mapToIsometricCoordinates(x, y, z);
    }
    else {
        {
            // выставляем эмпирически подогнанные значения положения
            if (border=="left") {
                position = mapToIsometricCoordinates(x-48, y+51, z);
            }
            else if (border=="back") {
                position = mapToIsometricCoordinates(x-10, y+10, z);
            }
            else if (border=="front") {
                position = mapToIsometricCoordinates(x-10, y+61, z);
            }
            else if (border=="right") {
                position = mapToIsometricCoordinates(x, y+50, z);
            }
            else {
                position = mapToIsometricCoordinates(x, y, z);
            }
        }
    }

    it->setPos(position);
    m_scene->addItem(it);

    if (border=="top") {
        c->pxTop = it;
        if (zz<zMax-1) {
           CellView25D *nc = static_cast<CellView25D*>(m_cells[zz+1][yy][xx].view);
           nc->pxBottom = it;
           it->setZValue(nc->baseZOrder);
        }
        else
            it->setZValue(c->baseZOrder+0.9);
    }
    else if (border=="right") {
        c->pxRight = it;
        if (xx<(xMax-1)) {
           CellView25D *nc = static_cast<CellView25D*>(m_cells[zz][yy][xx+1].view);
           nc->pxLeft = it;
           it->setZValue(nc->baseZOrder+0.1);
        }
        else
            it->setZValue(c->baseZOrder+0.4);
    }
    else if (border=="bottom") {
        c->pxBottom = it;
        if (zz>0) {
           CellView25D *nc = static_cast<CellView25D*>(m_cells[zz-1][yy][xx].view);
           nc->pxTop = it;
        }
        it->setZValue(c->baseZOrder);
    }
    else if (border=="left") {
        c->pxLeft = it;
        if (xx>0) {
           CellView25D *nc = static_cast<CellView25D*>(m_cells[zz][yy][xx-1].view);
           nc->pxRight = it;
        }
        it->setZValue(c->baseZOrder+0.1);

    }
    else if (border=="front") {
        c->pxFront = it;
        if (yy<(yMax-1)) {
           CellView25D *nc = static_cast<CellView25D*>(m_cells[zz][yy+1][xx].view);
           nc->pxBack = it;
           it->setZValue(nc->baseZOrder+0.5);
        }
        else
            it->setZValue(c->baseZOrder+0.2);

    }
    else if (border=="back") {
        c->pxBack = it;
        if (yy>0) {
           CellView25D *nc = static_cast<CellView25D*>(m_cells[zz][yy-1][xx].view);
           nc->pxFront = it;
           it->setZValue(nc->baseZOrder+0.2);
        }
        else
            it->setZValue(c->baseZOrder+0.5);
    }
    else {
        qDebug() << "Invalid border name: " << border;
        return;
    }

//    if (border=="left") {
//        QGraphicsTextItem *ti = new QGraphicsTextItem;
//        ti->setPlainText(QString("Z:%1").arg(it->zValue()));
//        ti->setFont(QFont("sans-serif",6,QFont::Bold,false));
//        ti->setZValue(1000000.0);
//        ti->setPos(position);
//        m_scene->addItem(ti);
//    }
}


void Scene25D::updateCellBorder(quint16 x, quint16 y, quint16 z, const QString &borderName)
{
    int frame;
    const QString border = borderName.trimmed().toLower();
    if (border=="top")
        frame = m_cells[z][y][x].topBorderState;
    else if (border=="left")
        frame = m_cells[z][y][x].leftBorderState;
    else if (border=="bottom")
        frame = m_cells[z][y][x].bottomBorderState;
    else if (border=="right")
        frame = m_cells[z][y][x].rightBorderState;
    else if (border=="front")
        frame = m_cells[z][y][x].frontBorderState;
    else if (border=="back")
        frame = m_cells[z][y][x].backBorderState;
    else {
        qDebug() << "Bad border name: " << border;
        return;
    }
    setEdgeStyle(x, y, z, borderName, QString("edge_%1_%2").arg(border).arg(frame));
}
