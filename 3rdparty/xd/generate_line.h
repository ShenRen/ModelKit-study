//王晓东 2014/1/20 编制生成直线扫描线的头文件，为防止命名冲突，放在命名空间xd里面。
//王晓东 2015/4/7  增加了直线扫描在前后两条直线跨越极值点时的分区算法，否则FDM加工成型件可能会出现问题。
//王晓东 2015/8/12 注意！当出现轮廓曲里拐弯时，说明精度问题啊！！除以一个数一定要写成1000000.0！！！不是1000000！！！
#ifndef GENERATE_LINE_H
#define GENERATE_LINE_H

#include <algorithm>
#include <cmath>
#include <iostream>
#include <list>
#include <vector>
#include "../clipper/clipper.hpp"   //调用开源函数库clipper库实现多边形的轮廓偏置算法和布尔算法

namespace xd
{
    float const pi = 3.141592653589793238;
    float degrees(float fudu); //幅度转化为角度的函数
    float radians(float jiaodu); //角度转换为幅度的函数
    class xdpoint   //基本点类，包含平面内极坐标和笛卡尔坐标系的转换
    {
    public:
        float x;
        float y;
        float z;
        xdpoint():x(0),y(0),z(0){}
        xdpoint(float a,float b):x(a),y(b),z(0){}  //这里的重载函数z默认为0，以后可以改进
        xdpoint(float a, float b,float c):x(a),y(b),z(c){}
        //xdpoint(int a, int b) :x(a/1000000.0), y(b/1000000.0), z(0){}  //这里为了适应师兄程序
        float modulus() const;
        float argument() const;
        xdpoint & modulus(float new_modulus);
        xdpoint & argument(float new_argument);
        friend inline bool operator== (const xdpoint& a, const xdpoint& b)
        {
            return a.x == b.x && a.y == b.y;
        }
        friend inline bool operator!= (const xdpoint& a, const xdpoint& b)
        {
            return a.x != b.x  || a.y != b.y;
        }
    };


    typedef std::vector<xdpoint> outline;
    typedef std::vector<outline> outlines;
    typedef std::vector<std::pair<outline,unsigned int> > outputOutlines;   //用于输出带有扫描层数的路径。

    void ClipperPathToOutline(const ClipperLib::Path & input,outline * output);   //没加最后一个点，注意！
    void ClipperPathsToOutlines(const ClipperLib::Paths & input,outlines * output);  //没加最后一个点，注意！
    void OutlineToClipperPath(const outline & input, ClipperLib::Path * output);
    void OutlinesToClipperPaths(const outlines & input, ClipperLib::Paths * output);

    bool IsContainPoint(std::vector<std::pair<float,float> > &a, xdpoint b);  //判读极值点列表里是否包含某一个点的函数。

    int IsLeft(xdpoint p1,xdpoint p2, xdpoint p); //判断p点是否在有向线段p1,p2左侧的函数。

    bool IsEmpty(std::vector<std::pair<float,std::list<float> > > tem,int & firstV);  //判断扫描线中的元素是否全部取完的函数。

    void DeleteOddDate(std::vector<std::pair<float,std::list<float> > > & tem);   //将存放奇数点数据的扫描线删除的函数，正常情况下用不着此函数。

    bool IsDealFinish(const std::vector<xdpoint> & l,int & i);  //判断平行于x轴的直线上是否有多于两个点的函数。
    void DealOneLayer(std::vector<xdpoint> & l); //上面函数的简化实现，待测试！！
    bool exceedExtremum(float bijiaoY1,float bijiaoY2,std::list<float> maxY,std::list<float> minY);  //用来判断前后两条直线是否越过了局部极值点的函数

    class CTwoDimensionVector //构造单位矢量类，包含矢量的叉乘和点乘。
    {
    public:
        float x;
        float y;
        CTwoDimensionVector(float a, float b)
        {
            float m=sqrt(pow(a,2)+pow(b,2));
            x=a/m;
            y=b/m;
        }
        float CrossProduct(const CTwoDimensionVector & v)
        {
            return (x*v.y-y*v.x);
        }
        float DotProduct(const CTwoDimensionVector & v)
        {
            return (x*v.x-y*v.y);
        }
    };

    void DealCompensate(outline DealData,outline & ResultData,float R); //处理轮廓点数据，使其变为有补偿半径的轮廓点数据。 融入到自己后期程序中，2015/3/10 可能有错

    void InfillLine(outlines TheOutline, outlines & TheResult, float width, float degree, int lunkuo, float shrinkDistance,  float offsetWidth); //自己编写的填充线生成函数。
    void InfillLine(outlines TheOutline, outlines & TheResult, outlines & TheOutlineResult, float width, float degree, int lunkuo, float shrinkDistance, float offsetWidth); //自己编写的填充线生成函数可以分开轮廓填充数据。
    void notInfillLine(outlines TheOutline, outlines & TheResult, outlines & TheOutlineResult, float width, float degree, int lunkuo, float shrinkDistance, float offsetWidth);//不分区的填充算法，为了加速
    void InfillConcentric(outlines TheOutline, outlines & TheResult, outlines & TheOutlineResult, float width, int lunkuo, float offsetWidth); //自己编写的同学填充函数

    void InfillLineIn(outlines TheOutline,outlines & TheResult,float width,float degree ); //自己编写的填充线生成函数。

    void InfillBMP(outlines TheOutline,std::vector<std::vector<int> > & ResultData,int piex ,int size); //生成bmp格式的函数。

    void InfillOffset(outlines theOutline,outlines & theResult,float width);  //进行偏置填充的函数。

    void InfillOffsetIn(ClipperLib::Paths theOutline,outlines & theResult,float width);  //进行偏置填充的函数，输入就是放大后的数据

    void PickUpLayer(std::vector<xd::outlines> theOutline,std::vector<int> & theResult);    //挑选出突变截面的函数，实现方案一

    bool IsSingleInfill(int layerNumber,std::vector<int> needInfillLayer);      //找到单层填充的层数，实现方案一

    void OutlinesOffsetMethod(std::vector<xd::outlines> theOutline,std::vector<xd::outputOutlines> &result,float width,float shrinkDistance);  //单层偏置，多层填充，实现方案二

    void OutlinesClipperMethod(std::vector<xd::outlines> theOutline,std::vector<xd::outputOutlines> &result,float width,float shrinkDistance,float degree);  //每层都裁剪，最后填充，实现方案三

    void SplitMNArea(outlines TheOutline,std::vector<xd::outlines> & theResult,std::vector<outlines> & dataOffsets,float width,int M,int N,int lunkuo,float overlap);  //偏置N次后划分为m*n个区域的函数,偏置的所有轮廓,注意M、N≥1，且M*N>1

    int SplitLWArea(outlines TheOutline, std::vector<xd::outlines> & theResult, std::vector<outlines> & dataOffsets, float width, int Length, int Width, int lunkuo, float overlap, float threshold);  //偏置N次后划分为Length*Width长度区域的函数

    void offsetReturnSingleRegion(outlines TheOutline,std::vector<xd::outlines> & theResult,std::vector<outlines> & dataOffsets,float width,int lunkuo);  //输入轮廓返回偏置轮廓lunkou次后的轮廓数据，而且轮廓数据已经按照连通域分开！且分开后第一个是外轮廓
    void offsetReturnSingleRegion(outlines TheOutline,std::vector<outlines> & theResult);  //重载函数轮廓数据已经按照连通域分开！
    void AddOuterPolyNodeToResult(ClipperLib::PolyNode& polynode,std::vector<xd::outlines> & output); //上一个函数需要用的一个递归函数！！

}

#endif // GENERATE_LINE_H

