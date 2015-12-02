#ifndef POINT_H
#define POINT_H

#include "math.hpp"
#include <QDebug>
#include <QDataStream>

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
