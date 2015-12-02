#include "infillplotwidget.h"
#include <QPainter>

using namespace XJRP;

void _drawPolygons(QPainter *p, const Polygon &polygon, qreal scale)
{
    //temp
    QPolygonF qp;

    for (const Point &point : polygon)
    {
        qp.append (QPointF (point.x () * scale, point.y () * scale));
    }
    p->drawPolygon(qp);
}

InfillPlotWidget::InfillPlotWidget(QWidget *parent) : QWidget(parent)
{

}

void InfillPlotWidget::setLayer(const Layer &layer)
{
    this->layer = layer;
    //this->update();
}

void InfillPlotWidget::paintEvent(QPaintEvent *e)
{
    Q_UNUSED (e);
    QPainter p (this);
    QSize size (this->size ());
    Boundary boundary (layer.boundary ());
    qreal scale ((boundary.maxX () - boundary.minX ()) / 640 / 540 * size.width ());
    //scale=1;
    p.setRenderHint (QPainter::Antialiasing);
    for (const Polygon &polygon : layer)
    {
        switch (polygon.type ())
        {
        case Polygon::Infill :
            p.setPen (QPen (Qt::blue));
            _drawPolygons (&p,polygon,scale);
            break;
        case Polygon::Contour :
            p.setPen (QPen (Qt::red));
            _drawPolygons (&p,polygon,scale);
            break;
        case Polygon::Support :
            p.setPen (QPen (Qt::darkGreen));
            _drawPolygons (&p,polygon,scale);
            break;
        }
    }
}

