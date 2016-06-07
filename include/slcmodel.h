#ifndef SLCMODEL_H
#define SLCMODEL_H

#include <QList>
#include "layer.h"

namespace XJRP
{

class SLCModel :public QList<Layer>
{
public:
    const Boundary boundary () const;
    const Point center () const;
    const Point dimension () const;
    const QList <qreal> heights () const;
    const Layer layerAtIndex (const int index) const;
    const Layer layerAtHeight (const qreal height) const;
    void sort ();
    void merge(const SLCModel &other);
    bool readFile (const QString &filename);
    void skin_core_infill(int interval_num,float space,float shrinkDistance,float angle_start,float angle_delta,qreal LaserPower=-1.0,qreal ScanSpeed=-1.0);   //测试用函数
    void line_infill(qint8 ContourCount, qreal ContourWidth,float space,float shrinkDistance,float angle_start,float angle_delta,qreal LaserPower=-1.0,qreal ScanSpeed=-1.0);   //测试用函数
    void concentric_infill(qint8 ContourCount, qreal ContourWidth,float space,float shrinkDistance,float angle_start,float angle_delta,qreal LaserPower=-1.0,qreal ScanSpeed=-1.0);   //测试用函数
    //void selfRecognition_infill(float space,float stagger,float angle_start,float angle_delta,qreal LaserPower=-1.0,qreal ScanSpeed=-1.0);

private:
    QList<qreal> layerHeights;
};

}

#endif // SLCMODEL_H
