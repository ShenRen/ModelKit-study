#include <QCoreApplication>
#include <QElapsedTimer>
#include "layer.h"

using namespace XJRP;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QElapsedTimer t;
    t.start ();
    Layer l;
    Polygon p;
//    p.push_back (Point (5, 5, 5));
//    p.push_back (Point (95, 0, 5));
//    p.push_back (Point (95, 95, 5));
//    p.push_back (Point (0, 95, 5));
//    p.close ();
    const int count = 10;
    for (int i = 0; i < count; ++i)
    {
        p.push_back (Point (100 + 100 * cos (double (i) * 2 * PI / count), 100 + 100 * sin (double (i) * 2 * PI / count), 100));
    }
    l.push_back (p);

    Layer o = l.offsetted (10);
    qDebug () << o;

    o.offset (-10);
    qDebug () << o;

    qDebug () << "time elapsed:" << double (t.nsecsElapsed ()) / 1e9 << "s";

    return 0;
}

