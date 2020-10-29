#ifndef KUMSCENE_H
#define KUMSCENE_H
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QtCore>
#include <QtGui>

class KumScene: public QGraphicsScene
{
    Q_OBJECT
public:
    KumScene(QObject *parent = 0);

public:
signals:
    void MousePress(qreal x,qreal y, bool Flag, qreal xScene,qreal yScene);
    void MouseRelease(qreal x,qreal y, bool Flag);
    void MouseMove(qreal x,qreal y, bool Flag);
    void MouseWheel(int Delta);
    //void MousePress1(qreal x,qreal y, bool Flag);
private:
    void mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent );
    void mouseReleaseEvent ( QGraphicsSceneMouseEvent * mouseEvent );
    void mouseMoveEvent ( QGraphicsSceneMouseEvent * mouseEvent );
    void wheelEvent ( QGraphicsSceneWheelEvent * wheelEvent );



};

#endif // KUMSCENE_H
