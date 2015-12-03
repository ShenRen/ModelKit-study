#include <QApplication>
#include "infillplotwidget.h"
#include "slcmodel.h"
#include <QElapsedTimer>
//#include <qstring.h>
#include "draw.h"
#include "window.h"

using namespace XJRP;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    SLCModel m;
    QElapsedTimer t2;
    t2.start ();
//    for (int i = 0; i < 4; ++i)
//    {
//        SLCModel _model;
//        _model.readFile (QString ("C:/Users/Administrator/Desktop/test-data/lucy_molding01/%1.slc").arg (i+1));
//        qDebug () << _model.boundary ();
//        m.merge (_model);
//    }

//    for (int i = 0; i < 4; ++i)
//    {
//        SLCModel _model;
//        _model.readFile (QString ("/data/Share/RPBuild_examples/时代天使牙模比对/s_%1.slc").arg (i+1));
//        qDebug () << _model.boundary ();
//        m.merge (_model);
//    }
    m.readFile (QString ("C:/Users/Administrator/Desktop/test-data/lucy_molding01.slc"));

    qDebug () << double (t2.nsecsElapsed ()) / 1e9;

    qDebug () << m.boundary ();
    qDebug () << m.size();

    t2.restart ();
    Layer l (m.layerAtIndex (40));
    //声明cura填充形式
    Layer::InfillSpecification spec;
    spec.type = Layer::InfillSpecification::InfillType::Unidirectional;
    spec.lineWidth = 0.1;
    spec.interval = 0.1;
    spec.angle = 0;
    spec.shrinkWidth = 0.1;
    spec.extraContourCount = 0;
    spec.extraContourWidth = 0;
    l.infill (spec);

    qDebug () << double (t2.nsecsElapsed ()) / 1e9;
    qDebug() << "finished record time.";
    draw w;
    w.setLayer (l);
    w.show ();
    Window W(&m);
    W.show();

    return a.exec ();
}

