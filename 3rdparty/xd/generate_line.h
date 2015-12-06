//������ 2014/1/20 ��������ֱ��ɨ���ߵ�ͷ�ļ���Ϊ��ֹ������ͻ�����������ռ�xd���档
//������ 2015/4/7  ������ֱ��ɨ����ǰ������ֱ�߿�Խ��ֵ��ʱ�ķ����㷨������FDM�ӹ����ͼ����ܻ�������⡣
//������ 2015/8/12 ע�⣡�����������������ʱ��˵���������Ⱑ��������һ����һ��Ҫд��1000000.0����������1000000������
#ifndef GENERATE_LINE_H
#define GENERATE_LINE_H

#include <algorithm>
#include <cmath>
#include <iostream>
#include <list>
#include <vector>
#include "../clipper/clipper.hpp"   //���ÿ�Դ������clipper��ʵ�ֶ���ε�����ƫ���㷨�Ͳ����㷨

namespace xd
{
    float const pi = 3.141592653589793238;
    float degrees(float fudu); //����ת��Ϊ�Ƕȵĺ���
    float radians(float jiaodu); //�Ƕ�ת��Ϊ���ȵĺ���
    class xdpoint   //�������࣬����ƽ���ڼ�����͵ѿ�������ϵ��ת��
    {
    public:
        float x;
        float y;
        float z;
        xdpoint():x(0),y(0),z(0){}
        xdpoint(float a,float b):x(a),y(b),z(0){}  //��������غ���zĬ��Ϊ0���Ժ���ԸĽ�
        xdpoint(float a, float b,float c):x(a),y(b),z(c){}
        //xdpoint(int a, int b) :x(a/1000000.0), y(b/1000000.0), z(0){}  //����Ϊ����Ӧʦ�ֳ���
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
    typedef std::vector<std::pair<outline,unsigned int> > outputOutlines;   //�����������ɨ�������·����

    void ClipperPathToOutline(const ClipperLib::Path & input,outline * output);   //û�����һ���㣬ע�⣡
    void ClipperPathsToOutlines(const ClipperLib::Paths & input,outlines * output);  //û�����һ���㣬ע�⣡
    void OutlineToClipperPath(const outline & input, ClipperLib::Path * output);
    void OutlinesToClipperPaths(const outlines & input, ClipperLib::Paths * output);

    bool IsContainPoint(std::vector<std::pair<float,float> > &a, xdpoint b);  //�ж���ֵ���б����Ƿ����ĳһ����ĺ�����

    int IsLeft(xdpoint p1,xdpoint p2, xdpoint p); //�ж�p���Ƿ��������߶�p1,p2���ĺ�����

    bool IsEmpty(std::vector<std::pair<float,std::list<float> > > tem,int & firstV);  //�ж�ɨ�����е�Ԫ���Ƿ�ȫ��ȡ��ĺ�����

    void DeleteOddDate(std::vector<std::pair<float,std::list<float> > > & tem);   //��������������ݵ�ɨ����ɾ���ĺ���������������ò��Ŵ˺�����

    bool IsDealFinish(const std::vector<xdpoint> & l,int & i);  //�ж�ƽ����x���ֱ�����Ƿ��ж���������ĺ�����
    void DealOneLayer(std::vector<xdpoint> & l); //���溯���ļ�ʵ�֣������ԣ���
    bool exceedExtremum(float bijiaoY1,float bijiaoY2,std::list<float> maxY,std::list<float> minY);  //�����ж�ǰ������ֱ���Ƿ�Խ���˾ֲ���ֵ��ĺ���

    class CTwoDimensionVector //���쵥λʸ���࣬����ʸ���Ĳ�˺͵�ˡ�
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

    void DealCompensate(outline DealData,outline & ResultData,float R); //�������������ݣ�ʹ���Ϊ�в����뾶�����������ݡ� ���뵽�Լ����ڳ����У�2015/3/10 �����д�

    void InfillLine(outlines TheOutline, outlines & TheResult, float width, float degree, int lunkuo, float shrinkDistance,  float offsetWidth); //�Լ���д����������ɺ�����
    void InfillLine(outlines TheOutline, outlines & TheResult, outlines & TheOutlineResult, float width, float degree, int lunkuo, float shrinkDistance, float offsetWidth); //�Լ���д����������ɺ������Էֿ�����������ݡ�
    void InfillLineSLA(outlines TheOutline, outlines & TheResult, float width, float degree, int lunkuo, float shrinkDistance,  float offsetWidth); //�Լ���д����������ɺ�����
    void InfillLineSLA(outlines TheOutline, outlines & TheResult, outlines & TheOutlineResult, float width, float degree, int lunkuo, float shrinkDistance, float offsetWidth); //�Լ���д����������ɺ������Էֿ�����������ݡ�
    void notInfillLine(outlines TheOutline, outlines & TheResult, outlines & TheOutlineResult, float width, float degree, int lunkuo, float shrinkDistance, float offsetWidth);//������������㷨��Ϊ�˼���
    void InfillConcentric(outlines TheOutline, outlines & TheResult, outlines & TheOutlineResult, float width, int lunkuo, float offsetWidth); //�Լ���д��ͬѧ��亯��

    void InfillLineIn(outlines TheOutline,outlines & TheResult,float width,float degree ); //�Լ���д����������ɺ�����

    void InfillBMP(outlines TheOutline,std::vector<std::vector<int> > & ResultData,int piex ,int size); //����bmp��ʽ�ĺ�����

    void InfillOffset(outlines theOutline,outlines & theResult,float width);  //����ƫ�����ĺ�����

    void InfillOffsetIn(ClipperLib::Paths theOutline,outlines & theResult,float width);  //����ƫ�����ĺ�����������ǷŴ�������,�ڲ��Զ���С��ʹ�����Ϊ��������

    void PickUpLayer(std::vector<xd::outlines> theOutline,std::vector<int> & theResult);    //��ѡ��ͻ�����ĺ�����ʵ�ַ���һ

    bool IsSingleInfill(int layerNumber,std::vector<int> needInfillLayer);      //�ҵ��������Ĳ�����ʵ�ַ���һ

    void OutlinesOffsetMethod(std::vector<xd::outlines> theOutline,std::vector<xd::outputOutlines> &result,float width,float shrinkDistance);  //����ƫ�ã������䣬ʵ�ַ�����

    void OutlinesClipperMethod(std::vector<xd::outlines> theOutline,std::vector<xd::outputOutlines> &result,float width,float shrinkDistance,float degree);  //ÿ�㶼�ü��������䣬ʵ�ַ�����

    void SplitMNArea(outlines TheOutline,std::vector<xd::outlines> & theResult,std::vector<outlines> & dataOffsets,float width,int M,int N,int lunkuo,float overlap);  //ƫ��N�κ󻮷�Ϊm*n������ĺ���,ƫ�õ���������,ע��M��N��1����M*N>1

    int SplitLWArea(outlines TheOutline, std::vector<xd::outlines> & theResult, std::vector<outlines> & dataOffsets, float width, int Length, int Width, int lunkuo, float overlap, float threshold);  //ƫ��N�κ󻮷�ΪLength*Width��������ĺ���

    void offsetReturnSingleRegion(outlines TheOutline,std::vector<xd::outlines> & theResult,std::vector<outlines> & dataOffsets,float width,int lunkuo);  //������������ƫ������lunkou�κ���������ݣ��������������Ѿ�������ͨ��ֿ����ҷֿ����һ����������
    void offsetReturnSingleRegion(outlines TheOutline,std::vector<outlines> & theResult);  //���غ������������Ѿ�������ͨ��ֿ���
    void AddOuterPolyNodeToResult(ClipperLib::PolyNode& polynode,std::vector<xd::outlines> & output); //��һ��������Ҫ�õ�һ���ݹ麯������

}

#endif // GENERATE_LINE_H

