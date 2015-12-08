#include "point.h"
#include <QDebug>

using namespace XJRP;

Point::Point ()
    : m_x (NAN), m_y (NAN), m_z (NAN)
{}

Point::Point(qreal x, qreal y)
    : m_x (x), m_y (y), m_z (NAN)
{}

Point::Point (qreal x, qreal y, qreal z)
    : m_x (x), m_y (y), m_z (z)
{}

Point::Point (const Point &p)
    : m_x (p.m_x), m_y (p.m_y), m_z (p.m_z)
{}

void Point::setValue (qreal x, qreal y, qreal z)
{
    m_x = x;
    m_y = y;
    m_z = z;
}

void Point::setValue(const Point &p)
{
    m_x = p.m_x;
    m_y = p.m_y;
    m_z = p.m_z;
}

void Point::setX (qreal x)
{
    m_x = x;
}

void Point::setY (qreal y)
{
    m_y = y;
}

void Point::setZ (qreal z)
{
    m_z = z;
}

#ifdef use_color
void Point::setColor(int r,int g,int b,int a)
{
    this->m_color=QColor(r,g,b,a);
}
QColor Point::color() const
{
    return m_color;
}
#endif

qreal Point::length () const
{
    return sqrt(m_x * m_x + m_y * m_y + m_z * m_z);
}

qreal Point::x () const
{
    return m_x;
}

qreal Point::y () const
{
    return m_y;
}

qreal Point::z () const
{
    return m_z;
}

Point &Point::operator += (const Point &p)
{
    m_x += p.m_x;
    m_y += p.m_y;
    m_z += p.m_z;
    return *this;
}

Point &Point::operator -= (const Point &p)
{
    m_x -= p.m_x;
    m_y -= p.m_y;
    m_z -= p.m_z;
    return *this;
}

Point &Point::operator *= (const qreal scale)
{
    m_x *= scale;
    m_y *= scale;
    m_z *= scale;
    return *this;
}

Point &Point::operator *= (const Point &p)
{
    m_x *= p.m_x;
    m_y *= p.m_y;
    m_z *= p.m_z;
    return *this;
}

Point &Point::operator /= (const qreal scale)
{
    if (!fuzzyIsNull (scale))
    {
        m_x /= scale;
        m_y /= scale;
        m_z /= scale;
    }
    return *this;
}

Point &Point::operator /= (const Point &p)
{
    if (!fuzzyIsNull (p.m_x) && !fuzzyIsNull (p.m_y) && !fuzzyIsNull (p.m_z))
    {
        m_x /= p.m_x;
        m_y /= p.m_y;
        m_z /= p.m_z;
    }
    return *this;
}

bool Point::isValid () const
{
    bool valid = std::isfinite (m_x) && std::isfinite (m_y) && std::isfinite (m_z);
    return valid;
}

bool Point::isZero () const
{
    bool isZero = (fuzzyIsNull (m_x) && fuzzyIsNull (m_y) && fuzzyIsNull (m_z));
    return isZero;
}

const QString Point::toString () const
{
    QString line (QString ("(%1,%2,%3)").arg ((double) m_x).arg ((double) m_y).arg ((double) m_z));
    return line;
}

const Point Point::fromString (const QString &line)
{
    QString _line (line.trimmed ());
    Point p;
    if (_line.startsWith ('(') && _line.endsWith (')'))
    {
        _line.remove ('(');
        _line.remove (')');
        QStringList list (_line.split (','));
        if (list.size () == 3)
        {
            p.setValue (list.at (0).toDouble (),
                        list.at (1).toDouble (),
                        list.at (2).toDouble ());
        }
    }
    return p;
}

const Point Point::zero()
{
    return Point (0.0, 0.0, 0.0);
}

bool operator == (const Point &p, const Point &q)
{
    return (fuzzyIsNull (p.x () - q.x ()) && fuzzyIsNull (p.y () - q.y ()) && fuzzyIsNull (p.z () - q.z ()));
}

bool operator != (const Point &p, const Point &q)
{
    return ! operator == (p, q);
}

const Point operator+ (const Point &p, const Point &q)
{
    Point t (p);
    t.operator += (q);
    return t;
}

const Point operator- (const Point &p, const Point &q)
{
    Point t (p);
    t.operator -= (q);
    return t;
}

const Point operator* (const Point &p, const qreal scale)
{
    Point t (p);
    t.operator *=(scale);
    return t;
}

const Point operator* (const Point &p, const Point &q)
{
    Point t (p);
    t.operator *=(q);
    return t;
}

const Point operator/ (const Point &p, const qreal scale)
{
    Point t (p);
    t.operator /= (scale);
    return t;
}

const Point operator/ (const Point &p, const Point &q)
{
    Point t (p);
    t.operator /= (q);
    return t;
}

QDebug operator << (QDebug dbg, const Point &point)
{
    dbg.noquote () << point.toString ();
    return dbg.quote ();
}

QDataStream &operator << (QDataStream &stream, const XJRP::Point &p)
{
    stream << p.x();
    stream << p.y();
    stream << p.z();
    return stream;
}

QDataStream &operator >> (QDataStream &stream, XJRP::Point &p)
{
    qreal x,y,z;
    stream >> x;
    stream >> y;
    stream >> z;
    p.setValue(x,y,z);
    return stream;
}

