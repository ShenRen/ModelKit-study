#ifndef DRAW_H
#define DRAW_H

#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QWidget>
#include "layer.h"
#include "slcmodel.h"

class draw : public QWidget
{
    Q_OBJECT

public:

    draw(QWidget *parent = 0);
    QSize minimumSizeHint() const Q_DECL_OVERRIDE;   //定义本身widget的最小尺寸
    QSize sizeHint() const Q_DECL_OVERRIDE;   //返回本身widget的推荐值
    void setModel(const XJRP::SLCModel & M);
    void setLayer(const XJRP::Layer & L);
public slots:

    void setPen(const QPen &pen);
    void setBrush(const QBrush &brush);
    void centering();

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);

private:
    QPointF lastPos;
    qreal moveX;
    qreal moveY;
    qreal scale;
    QPen pen;
    QBrush brush;
    XJRP::SLCModel Model;
    XJRP::Layer layer;
};
//! [0]

#endif // DRAW_H

