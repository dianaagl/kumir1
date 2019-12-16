#ifndef UTIL_H
#define UTIL_H

#include <QtCore>
#include <QtGui>
#include <QtSvg>

#define PI 3.14159

#define DEG2RAD(x) (2*PI)*x/360.0


extern QTransform isometricTransform(const QString &side);
extern QPointF mapToIsometricCoordinates(qreal x, qreal y, qreal z);
extern QImage translatePixmap(const QImage &pixmap, const QPoint &gradient, const QColor &debug);
extern QPair<QImage,QImage> splitPixmap(const QImage &pixmap, const QRect &source, const QRect &target, qreal pos);
extern QImage normalizePixmap1(const QImage &pixmap);
extern QImage normalizePixmap1(const QByteArray &svgData);
extern QImage normalizePixmap2(const QImage &pixmap);

extern QByteArray getFileData(const QString &moduleRoot, const QString &fileName);
extern QStringList getEntryList(const QString &moduleRoot, const QString &path);
extern bool isFileExists(const QString &moduleRoot, const QString &path);



#endif // UTIL_H
