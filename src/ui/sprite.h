#ifndef _SPRITE_H
#define _SPRITE_H

#include <QEasingCurve>
#include <QGraphicsEffect>
#include <QGraphicsItem>
#include <QMap>
#include <QObject>
#include <QTimer>

#include "QSanSelectableItem.h"

class Sprite : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)
    Q_PROPERTY(qreal scale READ scale WRITE setScale)

public:
    Sprite(QGraphicsItem *parent = NULL)
        : QGraphicsPixmapItem(parent)
    {
    }
};

class QAnimatedEffect : public QGraphicsEffect
{
    Q_OBJECT
    Q_PROPERTY(int index READ getIndex WRITE setIndex)

public:
    void setStay(bool stay);
    void reset()
    {
        index = 0;
    }
    int getIndex()
    {
        return index;
    }
    void setIndex(int ind)
    {
        index = ind;
    }

protected:
    bool stay;
    int index;

signals:
    void loop_finished();
};

class EffectAnimation : public QObject
{
    Q_OBJECT

public:
    EffectAnimation(QObject *parent = NULL);

    void fade(QGraphicsItem *map);
    void emphasize(QGraphicsItem *map, bool stay = true);
    void sendBack(QGraphicsItem *map);
    void effectOut(QGraphicsItem *map);
    void deleteEffect(QAnimatedEffect *effect);

public slots:
    void deleteEffect();

private:
    QMap<QGraphicsItem *, QAnimatedEffect *> effects;
    QMap<QGraphicsItem *, QAnimatedEffect *> registered;
};

class EmphasizeEffect : public QAnimatedEffect
{
    Q_OBJECT

public:
    EmphasizeEffect(bool stay = false);

protected:
    virtual void draw(QPainter *painter);
    virtual QRectF boundingRectFor(const QRectF &sourceRect) const;
};

class SentbackEffect : public QAnimatedEffect
{
    Q_OBJECT

public:
    SentbackEffect(bool stay = false);

protected:
    virtual void draw(QPainter *painter);
    virtual QRectF boundingRectFor(const QRectF &sourceRect) const;

private:
    QImage *grayed;
};

class FadeEffect : public QAnimatedEffect
{
    Q_OBJECT

public:
    FadeEffect(bool stay = false);

protected:
    virtual void draw(QPainter *painter);
};

#endif
