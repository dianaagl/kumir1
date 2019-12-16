#ifndef ACTOR25D_H
#define ACTOR25D_H

#include "abstractactor.h"

class Scene25D;
class GraphicsImageItem;

class Actor25D :
        public AbstractActor
{
Q_OBJECT;
Q_PROPERTY(quint16 currentFrame READ currentFrame WRITE setCurrentFrame);
public:
    explicit Actor25D(QObject *parent,
                      QGraphicsScene *scene,
                      ScriptEnvironment *env,
                      const QString &moduleRoot,
                      const QString &className,
                      const QString &instanceName);

signals:

public slots:

    void setScene(Scene25D *sc);
    void loadActorFromDir(const QString &actorName);
    void setPosition(quint16 x, quint16 y, quint16 z);
    void setCurrentFrame(quint16 frameNo);
    quint16 currentFrame() const { return i_currentFrame; }
    void beginMove(quint16 x, quint16 y, quint16 z);
    void moveBy(qreal pathPercentage);
    void endMove();
    virtual bool visible() const;
    virtual void setVisible(bool v);


protected:
    void setPixmapPosition(GraphicsImageItem *item, const QSize &pixmapSize, quint16 x, quint16 y, quint16 z);

    Scene25D *m_scene;
    QGraphicsScene *m_graphicsScene;

    GraphicsImageItem *g_pixmap;
    GraphicsImageItem *g_targetPixmap;
    qreal r_moveStatus;


    QMap<quint16, QImage> m_frames;
    QList< QPair<QImage, QImage> > m_moveFrames;
    quint16 i_currentMoveFrame;
    quint16 i_currentFrame;

    quint16 i_tx, i_ty, i_tz;
    bool b_visible;
    bool b_moving;




};

#endif // ACTOR25D_H
