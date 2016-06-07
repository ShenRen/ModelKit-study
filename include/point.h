#ifndef POINT_H
#define POINT_H

//#define use_color

#include "math.hpp"
#include <QDebug>
#include <QDataStream>
#ifdef use_color
#include <QColor>
#endif
namespace XJRP
{

class Point
{
public:
    Point ();
    Point (qreal x, qreal y);
    Point (qreal x, qreal y, qreal z);
    Point (const Point &p);

    void setValue (qreal x, qreal y, qreal z);
    void setValue (const Point &p);
    void setX (qreal x);
    void setY (qreal y);
    void setZ (qreal z);
#ifdef use_color
    void setColor(int r,int g,int b,int a=255);
    QColor color() const;
#endif
    qreal length () const;
    qreal x() const;
    qreal y() const;
    qreal z() const;

    Point &operator+= (const Point &p);
    Point &operator-= (const Point &p);
    Point &operator*= (const qreal scale);
    Point &operator*= (const Point &p);
    Point &operator/= (const qreal scale);
    Point &operator/= (const Point &p);

    bool isValid () const;
    bool isZero () const;

    const QString toString () const;
    static const Point fromString (const QString &line);

    static const Point zero ();

private:
    qreal m_x, m_y, m_z;
#ifdef use_color
    QColor m_color;
#endif
};

}

bool operator== (const XJRP::Point &p, const XJRP::Point &q);
bool operator!= (const XJRP::Point &p, const XJRP::Point &q);

const XJRP::Point operator+ (const XJRP::Point &p, const XJRP::Point &q);
const XJRP::Point operator- (const XJRP::Point &p, const XJRP::Point &q);
const XJRP::Point operator* (const XJRP::Point &p, const qreal scale);
const XJRP::Point operator* (const XJRP::Point &p, const XJRP::Point &q);
const XJRP::Point operator/ (const XJRP::Point &p, const qreal scale);
const XJRP::Point operator/ (const XJRP::Point &p, const XJRP::Point &q);

QDebug operator << (QDebug dbg, const XJRP::Point &point);

QDataStream &operator << (QDataStream &stream, const XJRP::Point &p);
QDataStream &operator >> (QDataStream &stream, XJRP::Point &p);

Q_DECLARE_METATYPE (XJRP::Point)

#endif // POINT_H
