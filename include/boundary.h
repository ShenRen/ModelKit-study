#ifndef BOUNDARY_H
#define BOUNDARY_H

#include "point.h"

namespace XJRP
{

class Boundary
{
public:
    Boundary ();
    Boundary (const Point &initial);

    qreal minX () const;
    qreal maxX () const;
    qreal minY () const;
    qreal maxY () const;
    qreal minZ () const;
    qreal maxZ () const;

    void setMinX (const qreal value);
    void setMaxX (const qreal value);
    void setMinY (const qreal value);
    void setMaxY (const qreal value);
    void setMinZ (const qreal value);
    void setMaxZ (const qreal value);

    void refer (const Point &point);
    void refer (const Boundary &boundary);

    bool isValid () const;

private:
    Point m_min;
    Point m_max;
};

}

QDebug operator << (QDebug dbg, const XJRP::Boundary &boundary);

QDataStream &operator << (QDataStream &stream, const XJRP::Boundary &boundary);
QDataStream &operator >> (QDataStream &stream, XJRP::Boundary &boundary);

Q_DECLARE_METATYPE (XJRP::Boundary)

#endif // BOUNDARY_H
