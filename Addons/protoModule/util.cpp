#include "util.h"
#include "quazip/quazip.h"
#include "quazip/quazipfile.h"
#include "quazip/quazipfileinfo.h"

QTransform isometricTransform(const QString &side)
{
    QTransform scaleTransform;
    QTransform shearTransform;
    QTransform rotateTransform;

    if (side=="top" || side=="bottom") {
        shearTransform.shear(sin( DEG2RAD(-45)  ), 0);
        rotateTransform.rotate(15);
        scaleTransform.scale(1.0, sin( DEG2RAD(45) ));
    }
    if (side=="left" || side=="right") {
        shearTransform.shear(0, sin( DEG2RAD(-55)  ));
        rotateTransform.rotate(0);
        scaleTransform.scale(1.0, sin( DEG2RAD(45) ));
    }
    if (side=="front" || side=="back") {
        shearTransform.shear(0, sin( DEG2RAD(15)  ));
        rotateTransform.rotate(0);
        scaleTransform.scale(1.0, sin( DEG2RAD(45) ));
    }
    return shearTransform * rotateTransform * scaleTransform;
}

QPointF mapToIsometricCoordinates(qreal x, qreal y, qreal z)
{
    QPointF plainCoordinates = isometricTransform("bottom").map(QPointF(x,-y));
    QPointF zOffset(0, -z);
    return plainCoordinates + zOffset;
}

QImage translatePixmap(const QImage &pixmap, const QPoint &gradient, const QColor &debug)
{
    QImage img(pixmap.width(), pixmap.height(), QImage::Format_ARGB32);
    img.fill(0);
    int x_offset = gradient.x();
    int y_offset = gradient.y();
    QPainter painter(&img);

    if (debug.isValid()) {
        painter.setBrush(QColor(debug.red(), debug.green(), debug.blue(), 31));
        painter.drawRect(0,0,img.width()-1,img.height()-1);
    }


    painter.drawImage(x_offset, y_offset, pixmap);
    painter.end();
    QImage result = img;
    return result;
}

QPair<QImage,QImage> splitPixmap(const QImage &pixmap, const QRect &source, const QRect &target, qreal pos)
{
    QRect global = source | target;
    QImage img(global.size(), QImage::Format_ARGB32);
    img.fill(0);
    QPainter painter(&img);
    QPointF offset = QPointF(target.bottomLeft() - source.bottomLeft())*pos;
    QPoint position = offset.toPoint();
    if (target.x() < source.x())
        position += QPoint(source.x()-target.x(),0);
    if (target.y() < source.y())
        position += QPoint(0,source.y()-target.y());
    QRect sourcePart = source.translated(-global.topLeft());
    QRect targetPart = target.translated(-global.topLeft());
//    painter.setBrush(QColor(255,0,0,31));
//    painter.drawRect(sourcePart);
//    painter.setBrush(QColor(0,255,0,31));
//    painter.drawRect(targetPart);

    painter.drawImage(position, pixmap);
    QImage pxSource = img.copy(sourcePart);
    QImage pxTarget = img.copy(targetPart);
    return QPair<QImage,QImage>(pxSource, pxTarget);
//    return QPair<QImage, QImage>(QImage::fromImage(img), QImage());
}

QImage normalizePixmap1(const QImage &pixmap)
{
    QRect baseRect(0,0,50,50);
    QRect plainRect = isometricTransform("bottom").mapRect(baseRect);
    double scaleX = 1.0;
    double scaleY = 1.0;
    if (pixmap.width()>plainRect.width())
        scaleX = (double)plainRect.width()/pixmap.width();
    if (pixmap.height()>50)
        scaleY = 50.0/pixmap.height();
    double scale = qMin(scaleX, scaleY);
    QTransform tr;
    tr.scale(scale, scale);
    QImage result = pixmap.transformed(tr, Qt::SmoothTransformation);
    return result;
}

QImage normalizePixmap1(const QByteArray &svgData)
{
    QSvgRenderer renderer(svgData);
    QRect baseRect(0,0,50,50);
    QRect plainRect = isometricTransform("bottom").mapRect(baseRect);
    double scaleX = 1.0;
    double scaleY = 1.0;
    if (renderer.defaultSize().width()>plainRect.width())
        scaleX = (double)plainRect.width()/renderer.defaultSize().width();
    if (renderer.defaultSize().height()>50)
        scaleY = 50.0/renderer.defaultSize().height();
    double scale = qMin(scaleX, scaleY);
    QSizeF size(renderer.defaultSize());
    size *= scale;
    QImage img(size.toSize(), QImage::Format_ARGB32);
    QPainter painter(&img);
    renderer.render(&painter);
    QImage result = img;
    return result;
}

QImage normalizePixmap2(const QImage &pixmap)
{
    QRect baseRect(0,0,50,50);
    QRect plainRect = isometricTransform("bottom").mapRect(baseRect);
    QImage result;
    int addLeft = 0;
    int addRight = 0;
    int addTop = 0;
    int addBottom = 0;
    if (plainRect.width()>pixmap.width()) {
        int difference = plainRect.width()-pixmap.width();
        addLeft = difference / 2;
        addRight = difference - addLeft;
    }
    if (pixmap.height()<50) {
        int difference = 50-pixmap.height();
        if (pixmap.height()<plainRect.height()) {
            int diff2 = plainRect.height()-pixmap.height();
            addBottom = diff2 / 2;
        }
        else {
            addBottom = qMin(plainRect.height()/3, difference);
        }
        addTop = difference - addBottom;
    }
    if (addLeft || addRight || addBottom || addTop ) {
        QImage img(pixmap.width()+addLeft+addRight,
                   pixmap.height()+addTop+addBottom,
                   QImage::Format_ARGB32);
        img.fill(0);
        QPainter painter(&img);
//        painter.setBrush(QColor(0,0,255,31));
//        painter.drawRect(0,0,img.width()-1,img.height()-1);
        painter.drawImage(addLeft, addBottom, pixmap);
        result = img;
//        qDebug() << "Actor pixmap expanded to size: " << img.size();
    }
    else {
        result = pixmap;
    }
    return result;
}

QByteArray getFileData(const QString &moduleRoot, const QString &fileName)
{
    QByteArray result;
    bool fileFromBundle = false;
    if (!moduleRoot.isEmpty()) {
        QFileInfo fi(moduleRoot);
        if (fi.isDir()) {
            fileFromBundle = false;
        }
        else if (moduleRoot.endsWith(".jar") || moduleRoot.endsWith(".zip")) {
            fileFromBundle = true;
        }
    }

    if (!fileFromBundle) {
        QFile f(moduleRoot+"/"+fileName);
        if (f.open(QIODevice::ReadOnly)) {
            result = f.readAll();
            f.close();
        }
    }
    else {
        QuaZip zip(moduleRoot);
        if (zip.open(QuaZip::mdUnzip)) {
            if (zip.setCurrentFile(fileName)) {
                QuaZipFile f(&zip);
                if (f.open(QIODevice::ReadOnly)) {
                    result = f.readAll();
                    f.close();
                }
            }
            zip.close();
        }
    }
    return result;
}

QStringList getEntryList(const QString &moduleRoot, const QString &path)
{
    QStringList result;
    QFileInfo fi(moduleRoot);
    if (fi.isDir()) {
        QDir dir(moduleRoot+"/"+path);
        result = dir.entryList();
        result.removeAll(".");
        result.removeAll("..");
    }
    else if (moduleRoot.endsWith(".jar") || moduleRoot.endsWith(".zip")) {
        QuaZip zip(moduleRoot);
        if (zip.open(QuaZip::mdUnzip)) {
            if (zip.goToFirstFile()) {
                do {
                    QuaZipFileInfo zfi;
                    if (zip.getCurrentFileInfo(&zfi)) {
                        if (zfi.name.startsWith(path)) {
                            result << zfi.name.mid(path.length()+1);
                        }
                    }
                } while (zip.goToNextFile());
            }
            zip.close();
        }
    }
    return result;
}

bool isFileExists(const QString &moduleRoot, const QString &path)
{
    bool result = false;
    QFileInfo fi(moduleRoot);
    if (fi.isDir()) {
        result = QFile::exists(moduleRoot+"/"+path);
    }
    else if (moduleRoot.endsWith(".jar") || moduleRoot.endsWith(".zip")) {
        QuaZip zip(moduleRoot);
        if (zip.open(QuaZip::mdUnzip)) {
            result = zip.setCurrentFile(path);
            zip.close();
        }
    }
    return result;
}
