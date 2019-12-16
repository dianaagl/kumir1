#ifndef SCENE25D_H
#define SCENE25D_H

#include "abstractscene.h"

struct EdgeStyle
{
    QImage pixmap;
    QPolygonF polygon;
    QBrush brush;
    QPen pen;
};

struct CellView25D:
        CellViewInterface
{
    QGraphicsItem *pxFront;
    QGraphicsItem *pxRight;
    QGraphicsItem *pxBack;
    QGraphicsItem *pxLeft;
    QGraphicsItem *pxTop;
    QGraphicsItem *pxBottom;
    qreal baseZOrder;
    inline CellView25D() { pxTop=pxRight=pxBottom=pxLeft=pxFront=pxBack = NULL; }

};

class Scene25D : public AbstractScene
{
    Q_OBJECT;
public:
    explicit Scene25D(QObject *parent,
                      const QString &moduleRoot,
                      const QString &sceneClass,
                      QGraphicsScene *graphicsScene,
                      ScriptEnvironment *env);
    void setSceneRootDir(const QString &sceneRootDir);
    qreal baseZOrder(quint16 x, quint16 y, quint16 z) const;
    inline QGraphicsScene* graphicsScene() const { return m_scene; }

signals:

public slots:

protected slots:

    void setEdgeStyle(quint16 x, quint16 y, quint16 z, const QString &borderName, const QString &styleFile);


protected:

    virtual void clearScene();
    virtual void initScene();
    virtual void updateCellBorder(quint16 x, quint16 y, quint16 z, const QString &borderName);

    EdgeStyle loadStyle(const QString &styleName, const QString &borderName);
    static EdgeStyle parseEdgeCSS(const QString &moduleRoot, const QString &fileName);

    QMap<QString,EdgeStyle> map_pixmaps;
    QString s_texture;
    QGraphicsScene *m_scene;
    QString s_sceneRootDir;

};

#endif // SCENE25D_H
