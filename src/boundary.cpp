#include "boundary.h"

using namespace XJRP;

Boundary::Boundary () :
    m_min (Point (INFINITY, INFINITY, INFINITY)), m_max (Point (-INFINITY, -INFINITY, -INFINITY))
{}

Boundary::Boundary (const Point &initial)
{
    if (initial.isValid ())
    {
        m_min.setValue (initial);
        m_max.setValue (initial);
    }
}

qreal Boundary::minX() const
{
    return m_min.x ();
}

qreal Boundary::maxX () const
{
    return m_max.x ();
}

qreal Boundary::minY () const
{
    return m_min.y ();
}

qreal Boundary::maxY () const
{
    return m_max.y ();
}

qreal Boundary::minZ () const
{
    return m_min.z ();
}

qreal Boundary::maxZ () const
{
    return m_max.z ();
}

void Boundary::setMinX (const qreal value)
{
    m_min.setX (value);
}

void Boundary::setMaxX (const qreal value)
{
    m_max.setX (value);
}

void Boundary::setMinY (const qreal value)
{
    m_min.setY (value);
}

void Boundary::setMaxY (const qreal value)
{
    m_max.setY (value);
}

void Boundary::setMinZ (const qreal value)
{
    m_min.setZ (value);
}

void Boundary::setMaxZ (const qreal value)
{
    m_max.setZ (value);
}

void Boundary::refer(const Point &point)
{
    if (point.x () < minX ())
        setMinX (point.x ());
    if (point.x () > maxX ())
        setMaxX (point.x ());

    if (point.y () < minY ())
        setMinY (point.y ());
    if (point.y () > maxY ())
        setMaxY (point.y ());

    if (point.z () < minZ ())
        setMinZ (point.z ());
    if (point.z () > maxZ ())
        setMaxZ (point.z ());
}

void Boundary::refer(const Boundary &boundary)
{
    if (boundary.minX () < minX ())
        setMinX (boundary.minX ());
    if (boundary.maxX () > maxX ())
        setMaxX (boundary.maxX ());

    if (boundary.minY () < minY ())
        setMinY (boundary.minY ());
    if (boundary.maxY () > maxY ())
        setMaxY (boundary.maxY ());

    if (boundary.minZ () < minZ ())
        setMinZ (boundary.minZ ());
    if (boundary.maxZ () > maxZ ())
        setMaxZ (boundary.maxZ ());
}

bool Boundary::isValid() const
{
    bool valid = std::isfinite (minX ()) &&
                 std::isfinite (maxX ()) &&
                 std::isfinite (minY ()) &&
                 std::isfinite (maxY ()) &&
                 std::isfinite (minZ ()) &&
                 std::isfinite (maxZ ());
    return valid;
}

QDebug operator <<(QDebug dbg, const Boundary &boundary)
{
    dbg.nospace () << '<';
    dbg.space () << boundary.minX () << boundary.maxX ()
                 << boundary.minY () << boundary.maxY ()
                 << boundary.minZ () << boundary.maxZ ();
    dbg.nospace () << '>';
    return dbg.space ();
}

QDataStream &operator <<(QDataStream &stream, const Boundary &boundary)
{
    stream << Point(boundary.minX(), boundary.minY(), boundary.minZ());
    stream << Point(boundary.maxX(), boundary.maxY(), boundary.maxZ());
    return stream;
}

QDataStream &operator >>(QDataStream &stream, Boundary &boundary)
{
    Point min, max;
    stream >> min;
    stream >> max;
    boundary.setMinX(min.x());
    boundary.setMinY(min.y());
    boundary.setMinZ(min.z());
    boundary.setMaxX(max.x());
    boundary.setMaxY(max.y());
    boundary.setMaxZ(max.z());
    return stream;
}

