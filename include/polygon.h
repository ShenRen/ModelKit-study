#ifndef POLYGON_H
#define POLYGON_H

#include <QList>
#include "point.h"
#include "boundary.h"

namespace XJRP
{

class Polygon : public QList<Point>
{
public:
	enum PolygonType
	{
		Contour,
		Infill,
		Support,
    };

    void setType (PolygonType type);
    PolygonType type () const;

    void setLaserPower (const qreal power);
    double laserPower() const;

    void setScanSpeed (const qreal speed);
    double scanSpeed () const;

    void simplify ();
    Polygon simplified () const;

    void reverse ();
    Polygon reversed () const;

    void close ();
    Polygon closed () const;
    bool isClosed () const;

    const Boundary boundary () const;

    const QString toString () const;
    static const Polygon fromString (const QString &line);
	
private:
    PolygonType m_type = Contour;
    qreal m_laserPower = -1.0;
    qreal m_scanSpeed = -1.0;
};

}

QDebug operator << (QDebug dbg, const XJRP::Polygon &polygon);

QDataStream &operator << (QDataStream &stream, const XJRP::Polygon &polygon);
QDataStream &operator >> (QDataStream &stream, XJRP::Polygon &polygon);

Q_DECLARE_METATYPE (XJRP::Polygon)

#endif // POLYGON_H
