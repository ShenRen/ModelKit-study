#include "layer.h"
#include "../3rdparty/clipper/clipper.hpp"
#include "../3rdparty/xd/generate_line.h"
#include "../3rdparty/cura/infill.h"

// #define USE_CURA

using namespace XJRP;

void Layer::setThickness (const qreal thickness)
{
    m_thickness = thickness;
}

qreal Layer::thickness () const
{
    return m_thickness;
}

void Layer::setHeight(const qreal height)
{
    m_height = height;
}

qreal Layer::height() const
{
    return m_height;
}

const Layer Layer::offsetted(const qreal delta) const
{
    double z (height ());

    ClipperLib::Paths paths;
    for (const Polygon &polygon : *this)
    {
        Polygon poly (polygon.closed ());
        ClipperLib::Path path;
        for (const Point &point : poly)
        {
            ClipperLib::IntPoint p (point.x () * PREC_RANGE, point.y () * PREC_RANGE);
            path.push_back (p);
        }
        paths.push_back (path);
    }

    ClipperLib::ClipperOffset offsetter;
    offsetter.AddPaths (paths, ClipperLib::JoinType::jtSquare, ClipperLib::EndType::etClosedPolygon);

    ClipperLib::Paths solution;
    offsetter.Execute (solution, delta * PREC_RANGE);

    Layer layer;
    for (const ClipperLib::Path &path : solution)
    {
        Polygon polygon;
        for (const ClipperLib::IntPoint &point : path)
        {
            polygon.push_back (Point (double (point.X) / PREC_RANGE, double (point.Y) / PREC_RANGE, z));
        }
        layer.push_back (polygon);
    }
    return layer;
}

void Layer::offset(const qreal delta)
{
    Layer offsetedLayer (offsetted (delta));
    clear ();
    *this = offsetedLayer;
}

const Boundary Layer::boundary () const
{
    Boundary boundary;
    for (const Polygon& polygon : *this)
    {
        boundary.refer (polygon.boundary ());
    }
    return boundary;
}

const QString Layer::toString () const
{
    QString line;
    line.append('{');
    for (const Polygon &polygon : *this)
    {
        line.append (polygon.toString ());
    }
    line.append ('}');
    return line;
}

const Layer Layer::fromString (const QString &line)
{
    QString _line (line.trimmed ());
    Layer l;
    if (_line.startsWith ('{') && _line.endsWith ('}'))
    {
        _line.remove ('{');
        _line.remove ('}');
        _line = _line.trimmed ();
        if (_line.startsWith ('[') && _line.endsWith (']'))
        {
            // [C(...,...,...)(...,...,...)][I()()]
            _line.remove (0, 1);
            _line.remove (_line.size () - 1, 1);
            QStringList list (_line.split ("]["));
            for (const QString &string : list)
            {
                QString _string (string);
                _string.prepend ('[');
                _string.append (']');
                Polygon c (Polygon::fromString (_string));
                l.append (c);
            }
        }
    }
    return l;
}

bool Layer::hasInfill() const
{
    bool hasInfill (false);
    for (const Polygon &polygon : *this)
    {
        if (polygon.type () == Polygon::PolygonType::Infill)
        {
            hasInfill = true;
            break;
        }
    }
    return hasInfill;
}

void Layer::infill(const Layer::InfillSpecification &spec)
{
    Layer infilled;

#ifdef USE_CURA

    cura::Polygons _polygons;
    cura::Polygons _result1;
    cura::Polygons _result2;

    qreal z (height ());

    for (const Polygon &polygon : *this)
    {
        if (polygon.isEmpty ())
            continue;

        // skip non-contour polygons
        if (polygon.type () != Polygon::PolygonType::Contour)
        {
            continue;
        }

        cura::Polygon p;
        for (const Point &point : polygon)
        {
            p.add (cura::Point (point.x () * PREC_RANGE, point.y () * PREC_RANGE));
        }

        _polygons.add (p);
    }

    cura::EFillMethod pattern = (spec.type == Layer::InfillSpecification::InfillType::Orthogonal)
                                ? (cura::EFillMethod::GRID)
                                : (cura::EFillMethod::LINES);
    cura::Infill infill (pattern,
                         _polygons,
                         0,
                         false,
                         0,
                         spec.interval * PREC_RANGE,
                         0.0 - spec.shrinkWidth / spec.lineWidth * 100.0,
                         0.0 - spec.angle + 90,
                         false,
                         false);

    infill.generate (_result1, _result2, nullptr);

    for (uint i = 0; i < _result2.size (); ++i)
    {
        const cura::Polygon &_p = _result2[i];
        Polygon p;
        for (uint j = 0; j < _p.size (); ++j)
        {
            const cura::Point &point = _p[j];
            p.push_back (Point (double (point.X) * PREC, double (point.Y) * PREC, z));
        }
        p.setType (Polygon::PolygonType::Infill);
        infilled.push_back (p);
    }

#else

    if (!this->empty())
    {
        xd::outlines inputValue;
        xd::outlines outputValue;
        xd::outlines outputContour;
        for (const Polygon &P : *this)
        {
            if (P.type() == Polygon::PolygonType::Contour&&!P.empty())  //如果多边形是轮廓且不为空
            {
                xd::outline temOutline;
                for (const Point &p : P)
                {
                    temOutline.push_back(xd::xdpoint(p.x(), p.y(),0));
                }

                xd::DealOneLayer(temOutline);      //化简

                if (temOutline.front()!=temOutline.back())
                {
                    temOutline.push_back(temOutline.front());
                }

                inputValue.push_back(temOutline);
            }
        }
        xd::notInfillLine(inputValue,
                          outputValue,
                          outputContour,
                          spec.lineWidth + spec.interval,
                          0 - spec.angle,
                          spec.extraContourCount,
                          spec.shrinkWidth,
                          spec.extraContourWidth);

        qreal zValue (height ());

        for (int i = 0; i != outputContour.size(); ++i)   //这里不能用spec.extraContourCount，因为可能没有偏置轮廓
        {
            Polygon temPolygon;
            for (uint j = 0; j != outputContour[i].size(); ++j)
            {
                temPolygon.push_back(Point(outputContour[i][j].x, outputContour[i][j].y, zValue));
            }
            temPolygon.setType(Polygon::PolygonType::Contour);
            infilled.push_back(temPolygon);
        }
        for (uint i = 0; i != outputValue.size(); ++i)
        {
            Polygon temPolygon;
            for (uint j = 0; j != outputValue[i].size(); ++j)
            {
                temPolygon.push_back(Point(outputValue[i][j].x, outputValue[i][j].y, zValue));
            }
            temPolygon.setType(Polygon::PolygonType::Infill);
            infilled.push_back(temPolygon);
        }
    }
#endif

    unfill ();

    append (infilled);
}

const Layer Layer::infilled(const Layer::InfillSpecification &spec) const
{
    Layer layer (*this);
    layer.infill (spec);
    return layer;
}

void Layer::unfill()
{
    for (int i = 0; i < count (); ++i)
    {
        Polygon &polygon ((*this)[i]);
        if (polygon.type () == Polygon::PolygonType::Infill)
        {
            removeAt (i);
        }
    }
}

const Layer Layer::unfilled() const
{
    Layer layer (*this);
    layer.unfill ();
    return layer;
}

bool operator <(const Layer &layer, const Layer &other)
{
    return (layer.height () < other.height ());
}

QDebug operator << (QDebug dbg, const Layer &layer)
{
    dbg.noquote ()<< layer.toString ();
    return dbg.quote ();
}

QDataStream &operator << (QDataStream &stream, const Layer &layer)
{
    stream << layer.height ();
    stream << layer.thickness();
    stream << *((QList<Polygon>*)&layer);
    return stream;
}

QDataStream &operator >> (QDataStream &stream, Layer &layer)
{
    qreal height, thickness;
    stream >> height;
    stream >> thickness;
    stream >> *((QList<Polygon>*)&layer);
    layer.setHeight (height);
    layer.setThickness(thickness);
    return stream;
}
