#ifndef LAYER_H
#define LAYER_H

#include <QList>
#include "polygon.h"

namespace XJRP
{

class Layer : public QList<Polygon>
{
public:
    class InfillSpecification
    {
    public:
        enum InfillType
        {
            Unidirectional,
            Orthogonal,
            Concentric,
            //SelfRecognition
        };

        InfillType type;
        qreal lineWidth;
        qreal interval;
        qreal angle;
        qreal shrinkWidth;
        qint8 extraContourCount;
        qreal extraContourWidth;
    };

    void setThickness (const qreal thickness);
    qreal thickness () const;

    void setHeight (const qreal height);
    qreal height () const;

    void setCoatingParameter (const QVariant& para);
    QVariant coatingParameter () const;

    const Layer offsetted (const qreal delta) const;
    void offset (const qreal delta);

    const Boundary boundary () const;

    const QString toString () const;
    static const Layer fromString (const QString &line);

    bool hasInfill () const;

    void infill (const InfillSpecification &spec);
    const Layer infilled (const InfillSpecification &spec) const;

    void unfill ();
    const Layer unfilled () const;

private:
    qreal m_thickness = 0.0;
    qreal m_height = 0.0;

    QVariant m_coatingParameter;
};

}

bool operator < (const XJRP::Layer &layer, const XJRP::Layer &other);

QDebug operator << (QDebug dbg, const XJRP::Layer &layer);

QDataStream &operator << (QDataStream &stream, const XJRP::Layer &layer);
QDataStream &operator >> (QDataStream &stream, XJRP::Layer &layer);

Q_DECLARE_METATYPE (XJRP::Layer)

#endif // LAYER_H
