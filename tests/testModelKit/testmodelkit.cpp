#include <QString>

#include <QElapsedTimer>
#include <iostream>
#include <time.h>
#include "slcmodel.h"

using namespace XJRP;

int _rand (int max)
{
    std::srand (time (0));
    // range: 1 ~ max
    return std::rand () % (max - 1) + 1;
}

int main (/*int argc, char* argv[]*/)
{
    double a1 = 10.003;
    double a2 = 9.988;
    double a3 = -9.4999;
    double a4 = -9.5000;

    qDebug () << a1 << round(a1);
    qDebug () << a2 << round(a2);
    qDebug () << a3 << round(a3);
    qDebug () << a4 << round(a4);

    Point p1 (10,100,0);
    Point p2 (30,20,0);

    Point p3 = p1 + p2;
    Point p4 = p1 - p2;
    Point p5 = p3 * 10;
    Point p6 = p4 / 10;

    qDebug () << p1;
    qDebug () << p2;
    qDebug () << p3;
    qDebug () << p4;
    qDebug () << p5;
    qDebug () << p6;

    p3 += p4;
    qDebug () << p3;
    p3 /= 0.000000000001;
    qDebug () << p3;
    p3 /= 3;
    qDebug () << p3;

    Polygon c1;
    c1.push_back(p1);
    c1.push_back(p2);
    c1.push_back(p3);
    c1.push_back(p4);
    c1.push_back(p5);
    c1.push_back(p6);

    Polygon c2 (c1);
    c2.reverse();

    Polygon c3 = c2.reversed();

    qDebug () << c1;
    qDebug () << c2;
    qDebug () << c3;

    qDebug () << c3.closed();
    qDebug () << Polygon().boundary ();

    Layer layer;
    layer.push_back(c2);
    layer.push_back(c3);

    qDebug () << layer;
    qDebug () << layer.boundary ();

//    qDebug () << "In Polygon:";
//    unsigned int count = 0;
//    for (Polygon2D::const_iterator cit = c1.begin();
//         cit != c1.end();
//         ++cit, ++count)
//    {
//        qDebug () << count << ". " << (*cit);
//    }

    //Point2D<Point2D> some;
    const unsigned int count = 8000000;
    {
        QElapsedTimer t;
        t.start ();
        Polygon c4;
        c4.reserve(count);
        qDebug () << "elasped time:" << double (t.nsecsElapsed ())/1e9;
        for (unsigned int i = 0; i < count; ++i)
        {
            c4.append(Point(rand(), rand(), rand()));
        }
        layer.push_back(c4);
        qDebug () << "elasped time:" << double (t.nsecsElapsed ())/1e9;
        qDebug () << c4.size();
        qDebug () << layer.boundary ();
        qDebug () << "elasped time:" << double (t.nsecsElapsed ())/1e9;
    }

    QElapsedTimer t;
    t.start ();
    unsigned int contourCountMax = 100;
    unsigned int pointCountMax = 100;
    unsigned int points = 0;
    SLCModel m;
    m.reserve (100);
    for (unsigned int layerIndex = 0;
         layerIndex < 100;
         ++ layerIndex)
    {
        unsigned int contourCount = _rand (contourCountMax);
        Layer l;
        l.reserve (contourCount);
        for (unsigned int contourIndex = 0;
             contourIndex < contourCount;
             ++ contourIndex)
        {
            unsigned int pointCount = _rand (pointCountMax);
            Polygon c;
            c.reserve (pointCount);
            for (unsigned int pointIndex = 0;
                 pointIndex < pointCount;
                 ++ pointIndex)
            {
                Point p(rand(), rand(), rand());
                c.append (p);
                points ++;
            }
            l.append (c);
        }
        m.append (l);

        QDataStream s;
        s << l;
        Layer other;
        s >> other;
    }
    qDebug () << m.front ().boundary ();
    qDebug () << "elasped time:" << double (t.nsecsElapsed ())/1e9;
    qDebug () << points << "points inserted to model";

    Polygon redundant;
    redundant.push_back (Point (0, 0, 0));
    redundant.push_back (Point (50, 50, 0));
    redundant.push_back (Point (100, 100, 0));
    redundant.push_back (Point (100, 100, 0));
    redundant.push_back (Point (100, 100, 0));
    redundant.push_back (Point (100, 100, 0));
    redundant.push_back (Point (200, 100, 0));
    redundant.push_back (Point (300, 100, 0));
    redundant.push_back (Point (400, 100, 0));

    qDebug () << redundant;
    qDebug () << redundant.simplified ();

    qDebug () << "TEST FINISHED.";
    std::cin.get ();
    return 0;
}
