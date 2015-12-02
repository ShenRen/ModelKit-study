#include "polygon.h"
#include <algorithm>

using namespace XJRP;

void Polygon::setType (Polygon::PolygonType type)
{
    m_type = type;
}

Polygon::PolygonType Polygon::type () const
{
    return m_type;
}

void Polygon::setLaserPower(const qreal power)
{
    m_laserPower = power;
}

double Polygon::laserPower() const
{
    return m_laserPower;
}

void Polygon::setScanSpeed(const qreal speed)
{
    m_scanSpeed = speed;
}

double Polygon::scanSpeed() const
{
    return m_scanSpeed;
}

void Polygon::simplify()
{
    for (int i = count () - 2; i > 0; --i)
    {
        if (count () < 3)
            break;

        Point prevDelta ( (*this)[i] - (*this)[i-1]);
        Point nextDelta ( (*this)[i+1] - (*this)[i]);

        if (nextDelta.isZero ())
        {
            removeAt (i);
            continue;
        }

        qreal prevDeltaLength (prevDelta.length ());
        qreal prevDeltaSlopeX (prevDelta.x () / prevDeltaLength);
        qreal prevDeltaSlopeY (prevDelta.y () / prevDeltaLength);
        qreal prevDeltaSlopeZ (prevDelta.z () / prevDeltaLength);

        qreal nextDeltaLength (nextDelta.length ());
        qreal nextDeltaSlopeX (nextDelta.x () / nextDeltaLength);
        qreal nextDeltaSlopeY (nextDelta.y () / nextDeltaLength);
        qreal nextDeltaSlopeZ (nextDelta.z () / nextDeltaLength);

        if (fuzzyIsNull (prevDeltaSlopeX - nextDeltaSlopeX) &&
            fuzzyIsNull (prevDeltaSlopeY - nextDeltaSlopeY) &&
            fuzzyIsNull (prevDeltaSlopeZ - nextDeltaSlopeZ))
        {
            removeAt (i);
        }
    }
}

Polygon Polygon::simplified() const
{
    Polygon other (*this);
    other.simplify ();
    return other;
}

void Polygon::reverse ()
{
    std::reverse(this->begin(), this->end());
}

Polygon Polygon::reversed () const
{
    Polygon other (*this);
    other.reverse();
    return other;
}

void Polygon::close ()
{
    if (!isClosed())
    {
        this->push_back (this->front());
    }
}

Polygon Polygon::closed () const
{
    Polygon other (*this);
    other.close();
    return other;
}

bool Polygon::isClosed () const
{
    // 空路径亦是闭合的
    bool isClosed (true);
    if (!isEmpty ())
        isClosed = (this->front() == this->back());
    return isClosed;
}

const Boundary Polygon::boundary() const
{
    Boundary boundary;
    for (const Point &point : *this)
    {
        boundary.refer (point);
    }
    return boundary;
}

const QString Polygon::toString () const
{
    QString line;
    line.append ('[');

    QString typeString;
    switch (type ())
    {
    default:
    case Polygon::Contour :
        typeString.append ('C');
        break;
    case Polygon::Infill :
        typeString.append ('I');
        break;
    case Polygon::Support :
        typeString.append ('S');
        break;
    }
    line.append (typeString);

    for (const Point &point : *this)
    {
        line.append (point.toString ());
    }
    line.append (']');
    return line;
}

const Polygon Polygon::fromString (const QString &line)
{
    QString _line (line.trimmed ());
    Polygon polygon;

    if (_line.startsWith ('[') && _line.endsWith (']'))
    {
        _line.remove ('[');
        _line.remove (']');
        _line = _line.trimmed ();

        // C(...,...,...)(...,...,...)

        if (_line.size () > 0)
        {
            char c (_line.at (0).toUpper ().toLatin1 ());
            _line.remove (0, 1);
            PolygonType type;
            switch (c)
            {
            default:
            case 'C':
                type = PolygonType::Contour;
                break;
            case 'I':
                type = PolygonType::Infill;
                break;
            case 'S':
                type = PolygonType::Support;
                break;
            }
            polygon.setType (type);
        }

        if (_line.startsWith ('(') && _line.endsWith (')'))
        {
            _line.remove (0, 1);
            _line.remove (_line.size () - 1, 1);
            QStringList list (_line.split (")("));
            for (const QString &string : list)
            {
                QString _string (string);
                _string.prepend ('(');
                _string.append (')');
                Point p (Point::fromString (_string));
                if (p.isValid ())
                    polygon.append (p);
            }
        }
    }
    return polygon;
}

QDebug operator << (QDebug dbg, const Polygon &polygon)
{
    dbg.noquote () << polygon.toString ();
    return dbg.quote ();
}

QDataStream &operator << (QDataStream &stream, const Polygon &polygon)
{
    stream << polygon.type();
    stream << polygon.laserPower();
    stream << polygon.scanSpeed();
    stream << *((QList<Point>*)&polygon);
    return stream;
}

QDataStream &operator >> (QDataStream &stream, Polygon &polygon)
{
    int type(0);
    double laserPower(0.0), scanSpeed(0.0);
    stream >> type;
    stream >> laserPower;
    stream >> scanSpeed;
    stream >> *((QList<Point>*)&polygon);
    polygon.setType((Polygon::PolygonType)type);
    polygon.setLaserPower(laserPower);
    polygon.setScanSpeed(scanSpeed);
    return stream;
}
