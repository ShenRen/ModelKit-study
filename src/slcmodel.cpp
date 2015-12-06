#include "slcmodel.h"
#include <QFile>
#include <QDataStream>
#include <QDebug>
#include "xd/generate_line.h"

using namespace XJRP;

const Boundary SLCModel::boundary() const
{
    Boundary boundary;
    for (const Layer &layer : *this)
    {
        boundary.refer (layer.boundary ());
    }
    return boundary;
}

const Point SLCModel::center() const
{
    Boundary boundary (this->boundary ());
    Point center (boundary.maxX () + boundary.minX (),
                  boundary.maxY () + boundary.minY (),
                  boundary.maxZ () + boundary.minZ ());
    center /= 2.0;
    return center;
}

const Point SLCModel::dimension() const
{
    Boundary boundary (this->boundary ());
    return Point (qAbs (boundary.maxX () - boundary.minX ()),
                  qAbs (boundary.maxY () - boundary.minY ()),
                  qAbs (boundary.maxZ () - boundary.minZ ()));
}

const QList<qreal> SLCModel::heights() const
{
    return layerHeights;
}

const Layer SLCModel::layerAtIndex (const int index) const
{
    if (index >= 0 && index < count ())
        return at (index);
    else
        return Layer ();
}

const Layer SLCModel::layerAtHeight(const qreal height) const
{
    int index = layerHeights.indexOf (height);
    return layerAtIndex (index);
}

void SLCModel::sort()
{
    layerHeights.clear ();
    std::sort (this->begin (), this->end ());

    for (const Layer &layer : *this)
    {
        layerHeights.append (layer.height ());
    }
}

void SLCModel::merge(const SLCModel &other)
{
    sort ();

    for (const Layer &layer : other)
    {
        qreal height = layer.height ();
        if (layerHeights.contains (height))
        {
            int index = layerHeights.indexOf (height);
            operator [] (index) += layer;
        }
        else
        {
            append (layer);
        }
    }

    sort ();
}

bool SLCModel::readFile(const QString &filename)
{
    bool ok (false);
    do
    {
        QFile slcFile(filename);
        if(! slcFile.open(QIODevice::ReadOnly) )
        {
            // qDebug()<<"Open SLC file error:" << filename;
            break;
        }

        QDataStream dataStream(&slcFile);
        dataStream.setByteOrder(QDataStream::LittleEndian);
        dataStream.setFloatingPointPrecision(QDataStream::SinglePrecision);

        /****************************************************************/
        /*-----------------------header section-------------------------*/
        /****************************************************************/
        qint8 headerChar;
        QByteArray headerData;
        while(!dataStream.atEnd() && dataStream.status() == QDataStream::Status::Ok)
        {
            dataStream >> headerChar;
            if(headerChar == 0x0d)
            {
                dataStream >> headerChar;
                if(headerChar == 0x0a)
                {
                    dataStream >> headerChar;
                    if(headerChar == 0x1a)
                    {
                        // break header reading
                        break;
                    }
                }
            }
            headerData.append (headerChar);
        }

        // split header with whitespace
        QList <QByteArray> headerList (headerData.split (' '));

        // extract SLCVER, UNIT, TYPE info in the header
        int versionIndex (headerList.indexOf ("-SLCVER"));
        int unitIndex (headerList.indexOf ("-UNIT"));
        int typeIndex (headerList.indexOf ("-TYPE"));

        if (versionIndex < 0 || unitIndex < 0 || typeIndex < 0)
        {
            // break loading
            break;
        }

        QByteArray versionString (headerList.at (versionIndex + 1));
        QByteArray unitString (headerList.at (unitIndex + 1));
        QByteArray typeString (headerList.at (typeIndex + 1));

        // validate header info

        // requires SLCVER >= 2.0 to proceed
        // since i never know other version there :P
        if (versionString.toDouble () < 2.0)
        {
            // break loading
            break;
        }

        // the unit might be INCH in some case.
        // then a unitscale was multiplied
        // which means the internal unit shall always be MM
        qreal unitScale (1.0);
        if (unitString != "MM")
        {
            unitScale = 2.54;
        }

        // in case the model file specified was a support type part
        // the polygons of layers of this model should be of Support type.
        bool isSupport (false);
        if (typeString != "PART")
        {
            isSupport = true;
        }

        /****************************************************************/
        /*-----------------------reserve section------------------------*/
        /****************************************************************/
        dataStream.skipRawData(256);

        /****************************************************************/
        /*-----------------------sampling table*------------------------*/
        /****************************************************************/
        quint8 samplingTableSize;
        dataStream >> samplingTableSize;
        for(quint8 i=0; i<samplingTableSize; i++)
        {
            /*
                Minimum Z Level             1 Float
                Layer Thickness             1 Float
                Line Width Compensation     1 Float
                Reserved                    1 Float
            */
            dataStream.skipRawData(sizeof (float) * 4);    //1 float = 4 byte
        }

        /****************************************************************/
        /*-----------------------contour data---------------------------*/
        /****************************************************************/
        /*
            Minimum Z Layer                                 float
            Number of Boundaries                            unsigned int 4byte

            Number of Vertices for the 1st Boundary         unsigned int
            Number of Gaps     for the 1st Boundary         unsigned int
            Vertex List for 1st Boundary                    (x/y) 2float

            Number of Vertices for the 2st Boundary
            Number of Gaps     for the 2st Boundary
            Vertex List        for 2st Boundary
         */
        while(!dataStream.atEnd() && dataStream.status () == QDataStream::Status::Ok)
        {
            float minZlevel;
            quint32 numberOfBoundary;
            dataStream >> minZlevel;
            dataStream >> numberOfBoundary;
            //judge for termination
            if( numberOfBoundary == 0XFFFFFFFF )
            {
                ok = true;
                // qDebug() << "Reading SLC file finished.";
                break;
            }
            //process the new layer
            Layer layer;

            quint32 numberOfVertices , numberOfGaps;
            //process every polygon in this layer
            for( quint32 boundaryId = 0; boundaryId < numberOfBoundary ; boundaryId++ )
            {
                Polygon polygon;
                dataStream >> numberOfVertices;
                dataStream >> numberOfGaps;

                float verticeX, verticsY;
                for( quint32 verticeId = 0; verticeId <numberOfVertices; verticeId ++)
                {
                    dataStream >> verticeX >> verticsY;
                    Point point(verticeX * unitScale, verticsY * unitScale, minZlevel * unitScale);
                    polygon.append( point );
                }
                if (isSupport)
                    polygon.setType (Polygon::PolygonType::Support);
                layer.append(polygon);
            }
            layer.setHeight (minZlevel * unitScale);
            append(layer);
        }
        slcFile.close();

        sort ();
    }
    while (false);
    return ok;
}

void SLCModel::skin_core_infill(int interval_num,float space,float shrinkDistance,float angle_start,float angle_delta,qreal LaserPower,qreal ScanSpeed)   //测试用函数
{
    int startIndex=0;
    bool flag(false);
    for(const Layer & L : *this)
    {
        for(const Polygon & P : L)
        {
            if(P.type()== Polygon::PolygonType::Contour)
            {
                startIndex=this->indexOf(L);
                flag=true;
                break;
            }
            if(P.type()== Polygon::PolygonType::Infill)
                return;   //说明已经填充过了
        }
        if(flag)
            break;
    }
    if(!flag)
        return;  //说明没有需要填充的轮廓数据
    for(int i=startIndex; i < this->size()-interval_num + 1 ; i+=interval_num)  //这里必须是<符号！
    {
        std::vector<xd::outlines> theInput;
        std::vector<xd::outputOutlines> theOutput;
        for (int j = i; j != interval_num + i; ++j)
        {
            xd::outlines temOutlines;
            //第一步：读取数据，并转换成xdpoint格式
            for(const Polygon & P : this->operator [](j))
            {
                if((P.type()== Polygon::PolygonType::Contour)&&(!P.empty()))
                {
                    xd::outline temOutline;
                    for(const Point & D : P)
                    {
                        temOutline.push_back(xd::xdpoint(D.x(), D.y()));
                    }
                    temOutlines.push_back(temOutline);
                }
            }
            theInput.push_back(temOutlines);
        }
		
        float infillDegree = angle_start + (i - startIndex) / interval_num * angle_delta ;
        //第二步：调用填充函数处理interval_num层数据
        xd::OutlinesClipperMethod(theInput, theOutput, space,shrinkDistance,infillDegree);
        //第三步：将输出路径转换成Point类型并加入
         for (int j = i; j != interval_num + i; ++j)
         {
            Layer temL;
            temL.setHeight(this->layerHeights.operator [](j));   //感觉没有必要，最后都加到这一层了！
            for(const std::pair<xd::outline,unsigned int>  & xo : theOutput[j-i])  //这里必须是j-i，注意！
            {
                Polygon temP;
                temP.setType(Polygon::PolygonType::Infill);
                for(const xd::xdpoint & xp : xo.first)
                {
                    temP.append(Point(xp.x,xp.y,this->layerHeights.operator [](j)));
                }
                if(j == interval_num + i-1)
                {//这里要自己设置了！！！
                    temP.setLaserPower(LaserPower);
                    temP.setScanSpeed(ScanSpeed);
                }
                temL.append(temP);
            }
            this->operator [](j) += temL;   //  += 操作符真牛！佩服！！
         }
    }
    int residue=(this->size()-startIndex)%interval_num;
    if(!residue)
        return;  //刚好能够除尽！
    float infillDegree = angle_start + (this->size() - startIndex ) / interval_num * angle_delta ;
    for(int i = this->layerHeights.size() - residue ; i != this->layerHeights.size() ; ++i )
    {
        infillDegree += angle_delta;
        Layer::InfillSpecification spec;
        spec.type = Layer::InfillSpecification::InfillType::Unidirectional;
        spec.lineWidth = space;
        spec.interval = 0.0;
        spec.angle = infillDegree;
        spec.shrinkWidth = shrinkDistance;
        spec.extraContourCount = 1;
        spec.extraContourWidth = space;
        this->operator [](i).infill(spec);
    }
}

void SLCModel::line_infill(qint8 ContourCount, qreal ContourWidth,float space,float shrinkDistance,float angle_start,float angle_delta,qreal LaserPower,qreal ScanSpeed)   //测试用函数
{
    int startIndex=0;
    bool flag(false);
    for(const Layer & L : *this)
    {
        for(const Polygon & P : L)
        {
            if(P.type()== Polygon::PolygonType::Contour)
            {
                startIndex=this->indexOf(L);
                flag=true;
                break;
            }
            if(P.type()== Polygon::PolygonType::Infill)
                return;   //说明已经填充过了
        }
        if(flag)
            break;
    }
    if(!flag)
        return;  //说明没有需要填充的轮廓数据
    for(int i=startIndex; i != this->size() ; i++)  //这里必须是<符号！
    {
        //第一步，计算角度
        float infillDegree = angle_start + (i - startIndex) * angle_delta ;
        //第二步，设置填充参数
        Layer::InfillSpecification spec;
        spec.type = Layer::InfillSpecification::InfillType::Unidirectional;
        spec.lineWidth = space;
        spec.interval = 0.0;
        spec.angle = infillDegree;
        spec.shrinkWidth = shrinkDistance;
        spec.extraContourCount = ContourCount;
        spec.extraContourWidth = ContourWidth;
        this->operator [](i).infill(spec);
    }
    for(int i=0 ; i!= this->size();++i)
    {
        for(int j=0;j!=this->operator [](i).size();++j)
        {
            if(this->operator [](i)[j].type()== Polygon::PolygonType::Infill)
            {
                this->operator [](i)[j].setLaserPower(LaserPower);
                this->operator [](i)[j].setScanSpeed(ScanSpeed);
            }
        }
    }
}
