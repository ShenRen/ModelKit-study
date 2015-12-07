#include "draw.h"
#include <QPainter>
#include <QWheelEvent>
#include "slcmodel.h"
#include <QtMath>
#include "polygon.h"
#include "layer.h"

using namespace XJRP;

void _drawArrow(QPainter *p, const QPointF & A, const QPointF & B)
{
    QPointF temB(std::sqrt(std::pow((A.x()-B.x()), 2) + std::pow(A.y()-B.y(), 2)),0);
    p->save();
    p->setPen (QPen (Qt::darkYellow));
    //想不通，要先平移旋转！
    p->translate(A);
    p->rotate(qRadiansToDegrees(std::atan2(B.y()-A.y(),B.x()-A.x())));

    p->drawLine(temB.x(),temB.y(),temB.x()-5,temB.y()+4);
    p->drawLine(temB.x(),temB.y(),temB.x()-5,temB.y()-4);
    p->restore();
}

void _drawAxis(QPainter *p)
{
    QPointF xNegative(-100,0);
    QPointF xPositive(100,0);
    QPointF yNegative(0,-100);
    QPointF yPositive(0,100);
    p->setPen(Qt::darkMagenta);
    p->drawLine(xNegative,xPositive);
    p->drawLine(yNegative,yPositive);
    _drawArrow(p,xNegative,xPositive);
    _drawArrow(p,yNegative,yPositive);
    QPen pen(Qt::darkRed);
    pen.setWidth(5);
    p->setPen(pen);
    p->drawPoint(QPointF(0,0));
}

void _drawPolygon(QPainter *p, const QPolygonF &polygon, qreal scale)  //画QPolygonF
{
    //temp
    p->drawPolygon(polygon);
    QPointF A=polygon.operator [](polygon.size()-2);
    QPointF B=polygon.operator [](polygon.size()-1);
    _drawArrow(p,A,B);
    //下面是强调起点的标志
    QPen pen(Qt::darkYellow);
    pen.setWidth(4);
    p->setPen(pen);
    p->drawPoint(polygon.front());
}

void _drawLayer(QPainter *p, const Layer & L,qreal scale)   //画Layer
{

     for(const Polygon & po : L)
     {
        QPolygonF qp;
        switch (po.type ())
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
        for (const Point &point : po)
        {
            qp.append (QPointF (point.x () * scale, point.y () * scale));
        }
        _drawPolygon(p,qp,scale);
     }
}

draw::draw(QWidget *parent)
    : QWidget(parent)
{
//    moveX=this->width()/2*devicePixelRatio();
//    moveY=this->height()/2*devicePixelRatio();
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

void draw::centering()
{
    moveX=this->width()/2*devicePixelRatio();
    moveY=this->height()/2*devicePixelRatio();
    update();
}

void draw::setLayer(const Layer &L)
{   
    this->layer = L;
    update();
}

void draw::setModel(const SLCModel &M)
{
    this->Model=M;
    moveX=this->width()/2*devicePixelRatio();
    moveY=this->height()/2*devicePixelRatio();
    Boundary boundary (this->Model.boundary());
    scale=(this->height()/(boundary.maxX () - boundary.minX ()));
}

void draw::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);
    painter.setPen(pen);
    painter.setBrush(brush);
    painter.translate(moveX,moveY);
    painter.rotate(-90);  //坐标屏幕翻转
    _drawAxis(&painter);
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
