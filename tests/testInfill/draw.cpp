#include "draw.h"
#include <QPainter>
#include <QWheelEvent>
#include "slcmodel.h"
#include "polygon.h"
#include "layer.h"

using namespace XJRP;

void _drawPolygon(QPainter *p, const Polygon &polygon, qreal scale)  //������
{
    //temp
    QPolygonF qp;
    switch (polygon.type ())
    {
    case Polygon::Infill :
        p->setPen (QPen (Qt::blue));
        break;
    case Polygon::Contour :
        p->setPen (QPen (Qt::red));
        break;
    case Polygon::Support :
        p->setPen (QPen (Qt::darkGreen));
        break;
    }
    for (const Point &point : polygon)
    {
        qp.append (QPointF (point.x () * scale, point.y () * scale));
    }
    p->drawPolygon(qp);
}

void _drawLayer(QPainter *p, const Layer & L,qreal scale)
{
     for(const Polygon & po : L)
     {
         _drawPolygon(p,po,scale);
     }
}

draw::draw(QWidget *parent)
    : QWidget(parent)
{
    moveX=this->width()/2;
    moveY=this->height()/2;
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
}

QSize draw::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize draw::sizeHint() const
{
    return QSize(800, 400);
}

void draw::setPen(const QPen &pen)
{
    this->pen = pen;
    update();
}

void draw::setBrush(const QBrush &brush)
{
    this->brush = brush;
    update();
}

void draw::setLayer(const Layer &layer)
{
    moveX=this->width()/2;
    moveY=this->height()/2;
    this->layer = layer;
    Boundary boundary (layer.boundary ());
    scale=(this->height()/(boundary.maxX () - boundary.minX ()));
    update();
}

void draw::paintEvent(QPaintEvent * /* event */)
{

    QPainter painter(this);
    painter.setPen(pen);
    painter.setBrush(brush);
    painter.translate(moveX,moveY);
    _drawLayer(&painter, layer, scale);

//    painter.setRenderHint(QPainter::Antialiasing, false);
//    painter.setPen(palette().dark().color());
//    painter.setBrush(Qt::NoBrush);
//    painter.drawRect(QRect(0, 0, width() - 1, height() - 1));
}

void draw::wheelEvent(QWheelEvent *e)
{
    double numDegrees = -e->delta() / 8.0;
    double numSteps = numDegrees / 15.0;
    scale /= pow(1.125, numSteps);
    update();
}

void draw::mousePressEvent(QMouseEvent *e)
{
    lastPos = e->pos();
}

void draw::mouseMoveEvent(QMouseEvent *e)
{
    qreal dx = qreal(e->x() - lastPos.x()) / scale;
    qreal dy = qreal(e->y() - lastPos.y()) / scale;
    if (e->buttons() & Qt::LeftButton) {
        moveX+=dx*15;
        moveY+=dy*15;
        update();
    } else if (e->buttons() & Qt::RightButton) {

        update();
    }
    else if(e->buttons() & Qt::MidButton)
    {

        update();
    }
    lastPos = e->pos();
}
