//2015_8_13 ������ ������ֱ������㷨�м�����������̫������ȥ���㷨������ֱ������㷨�������ƣ���������ȱһ���ǹ��սǵ��������ܳ���������
#include "generate_line.h"
#define NDEBUG  
#include <assert.h>   

namespace xd
{

float degrees(float fudu) //����ת��Ϊ�Ƕȵĺ���
{
    return(fudu/pi*180);
}
float radians(float jiaodu) //�Ƕ�ת��Ϊ���ȵĺ���
{
    return(jiaodu/180*pi);
}


float xdpoint::modulus() const
{
    return std::sqrt(x*x+y*y);
}
xdpoint & xdpoint::modulus(float new_modulus)
{
    float const old_modulus(modulus());
    float const scale(new_modulus/old_modulus);
    x*=scale;
    y*=scale;
    return *this;
}
float xdpoint::argument() const
{
    return degrees(atan2(y,x));
}
xdpoint & xdpoint::argument(float new_argument)
{
    float const mod(modulus());
    x=mod*std::cos(radians(new_argument));
    y=mod*std::sin(radians(new_argument));
    return *this;
}

typedef std::vector<xdpoint> outline;
typedef std::vector<outline> outlines;

void ClipperPathToOutline(const ClipperLib::Path & input,outline * output)
{
	//output->resize(input.size()+1);   �Σ�������push_back������ǧ�����resize����
	for(int i=0;i!=input.size();++i)
	{
		output->push_back(xd::xdpoint(input[i].X/1000000.0,input[i].Y/1000000.0));
	}
}
void ClipperPathsToOutlines(const ClipperLib::Paths & input,outlines * output)
{
	//output->resize(input.size());
	for(int i=0;i!=input.size();++i)
	{
		outline temOutput;
		ClipperPathToOutline(input[i],&temOutput);
		output->push_back(temOutput);
	}
}
void OutlineToClipperPath(const outline & input,ClipperLib::Path * output)
{
	//output->resize(input.size());
	for(int i=0;i!=input.size();++i)
	{
		output->push_back(ClipperLib::IntPoint(input[i].x*1000000.0,input[i].y*1000000.0));
	}
}
void OutlinesToClipperPaths(const outlines & input,ClipperLib::Paths * output)
{
	//output->resize(input.size());
	for(int i=0;i!=input.size();++i)
	{
		ClipperLib::Path temOutput;
		OutlineToClipperPath(input[i],&temOutput);
		output->push_back(temOutput);
	}
}

bool IsContainPoint(std::vector<std::pair<float,float> > &a, xdpoint b)  //�ж���ֵ���б����Ƿ����ĳһ����ĺ�����
{
    //ע�⣬list����ΪΪ�գ�
    for (int i=0;i!=a.size();i++)
    {
        if ((a[i].first==b.x)&&(a[i].second==b.y))
        {
            return true;
        }
    }
    return false;
}

int IsLeft(xdpoint p1,xdpoint p2, xdpoint p) //�ж�p���Ƿ��������߶�p1,p2���ĺ�����
{
    float x1=p1.x;
    float y1=p1.y;
    float x2=p2.x;
    float y2=p2.y;
    float x=p.x;
    float y=p.y;
    float det=x*y1-x*y2+y*x2-y*x1+x1*y2-x2*y1;
    if(det>0)
    {
        return(1); //p����ࡣ
    }
    else if(det<0)
    {
        return(-1); //p���Ҳࡣ
    }
    else
    {
        return(0); //p��p1,p2���ߡ�
    }
}
bool IsEmpty(std::vector<std::pair<float,std::list<float> > > tem,int & firstV)  //�ж�ɨ�����е�Ԫ���Ƿ�ȫ��ȡ��ĺ�����
{
    firstV=0;
    for (int j=0;j<tem.size();j++)
    {
        if (!tem[j].second.empty())
        {
            firstV=j;
            return 0;
        }
    }
    return 1;
}

void DeleteOddDate(std::vector<std::pair<float,std::list<float> > > & tem)   //��������������ݵ�ɨ����ɾ���ĺ���������������ò��Ŵ˺�����
{                                                           //��Ϊ��������¾Ͳ����ܴ�������������ݡ�
    int Num=0;
    for (int i=0;i!=tem.size();i++)
    {
        if (tem[i].second.size()%2==1)
        {
            Num++;
        }
    }
    for (int i=0;i!=Num;i++)
    {
        for (int j=0;j!=tem.size();j++)
        {
            if (tem[j].second.size()%2==1)
            {
                auto aaa=tem.begin()+j;
                tem.erase(aaa);
                break;
            }
        }
    }
}

bool IsDealFinish(const std::vector<xdpoint> & l,int & i)  //�ж�ƽ����x���ֱ�����Ƿ��ж���������ĺ�����
{
    int n=(int)l.size();          //�����n��ȡֵǧ���ܼ�ȥ1����Ϊ����β���Ѿ�δ������ˣ�
    for (i=0;i!=l.size();i++)     //ע�⣬������벻�������һ���㣬��Ϊ���һ����͵�һ���������ȣ�Ҫ��ȥ2��
    {                               //����Ϊʲô���Ǻǣ���������ֵ����񣡣��ڴ˴�ѭ��Ҫȫ��������
        int benshen=i%n;
        int nextP=(i+1)%n;
        int nextnP=(i+2)%n;
		float d1=sqrt(pow(l[benshen].x-l[nextP].x,2)+pow(l[benshen].y-l[nextP].y,2));  //��һ�������ĳ���
		float d2=sqrt(pow(l[nextnP].x-l[nextP].x,2)+pow(l[nextnP].y-l[nextP].y,2));  //�ڶ��������ĳ���
		float cross=std::abs(((l[nextP].y-l[benshen].y)*(l[nextnP].x-l[nextP].x)-(l[nextnP].y-l[nextP].y)*(l[nextP].x-l[benshen].x))); //������˵ľ���ֵ
        if (cross/(d1*d2)<sin(1.0/180*pi))   //���1�Ⱦ�ȥ�����ǵ���1.0
        {
            i=nextP; //Ҫɾ����Ԫ�صĽǱꡣ
            return 0;
        }
    }
    return 1;
}

void DealOneLayer(std::vector<xdpoint> & l)
{
	if (l.size() <= 4)
		return;
	std::vector<xdpoint>::const_iterator benshen = l.begin();
	std::vector<xdpoint>::const_iterator nextP = benshen + 1;
	std::vector<xdpoint>::const_iterator nextnP = benshen + 2;
	float d1 = sqrt(pow(benshen->x - nextP->x, 2) + pow(benshen->y - nextP->y, 2));  //��һ�������ĳ���
	float d2 = sqrt(pow(nextnP->x - nextP->x, 2) + pow(nextnP->y - nextP->y, 2));  //�ڶ��������ĳ���
	float cross = std::abs(((nextP->y - benshen->y)*(nextnP->x - nextP->x) - (nextnP->y - nextP->y)*(nextP->x - benshen->x))); //������˵ľ���ֵ
	while (l.size()>3)
	{
		if (cross / (d1*d2)<sin(1.0 / 180 * pi))   //���1�Ⱦ�ȥ�����ǵ���1.0
		{
			l.erase(nextP);
			nextP = benshen + 1;
			if (nextP == l.end())
			{
				nextP = l.begin();
				nextnP = nextP + 1;
				d1 = sqrt(pow(benshen->x - nextP->x, 2) + pow(benshen->y - nextP->y, 2));  //��һ�������ĳ���
				d2 = sqrt(pow(nextnP->x - nextP->x, 2) + pow(nextnP->y - nextP->y, 2));  //�ڶ��������ĳ���
				cross = std::abs(((nextP->y - benshen->y)*(nextnP->x - nextP->x) - (nextnP->y - nextP->y)*(nextP->x - benshen->x))); //������˵ľ���ֵ
				continue;
			}
			nextnP = nextP + 1;
			if (nextnP == l.end())
			{
				nextnP = l.begin();
				d1 = sqrt(pow(benshen->x - nextP->x, 2) + pow(benshen->y - nextP->y, 2));  //��һ�������ĳ���
				d2 = sqrt(pow(nextnP->x - nextP->x, 2) + pow(nextnP->y - nextP->y, 2));  //�ڶ��������ĳ���
				cross = std::abs(((nextP->y - benshen->y)*(nextnP->x - nextP->x) - (nextnP->y - nextP->y)*(nextP->x - benshen->x))); //������˵ľ���ֵ
				continue;
			}
			d1 = sqrt(pow(benshen->x - nextP->x, 2) + pow(benshen->y - nextP->y, 2));  //��һ�������ĳ���
			d2 = sqrt(pow(nextnP->x - nextP->x, 2) + pow(nextnP->y - nextP->y, 2));  //�ڶ��������ĳ���
			cross = std::abs(((nextP->y - benshen->y)*(nextnP->x - nextP->x) - (nextnP->y - nextP->y)*(nextP->x - benshen->x))); //������˵ľ���ֵ
			continue;
		}
		else
		{
			benshen += 1;
			if (benshen == l.end())
				break;
			nextP += 1;
			if (nextP == l.end())
			{
				nextP = l.begin();
				nextnP = nextP + 1;
				d1 = sqrt(pow(benshen->x - nextP->x, 2) + pow(benshen->y - nextP->y, 2));  //��һ�������ĳ���
				d2 = sqrt(pow(nextnP->x - nextP->x, 2) + pow(nextnP->y - nextP->y, 2));  //�ڶ��������ĳ���
				cross = std::abs(((nextP->y - benshen->y)*(nextnP->x - nextP->x) - (nextnP->y - nextP->y)*(nextP->x - benshen->x))); //������˵ľ���ֵ
				continue;
			}
			nextnP += 1;
			if (nextnP == l.end())
			{
				nextnP = l.begin();
				d1 = sqrt(pow(benshen->x - nextP->x, 2) + pow(benshen->y - nextP->y, 2));  //��һ�������ĳ���
				d2 = sqrt(pow(nextnP->x - nextP->x, 2) + pow(nextnP->y - nextP->y, 2));  //�ڶ��������ĳ���
				cross = std::abs(((nextP->y - benshen->y)*(nextnP->x - nextP->x) - (nextnP->y - nextP->y)*(nextP->x - benshen->x))); //������˵ľ���ֵ
				continue;
			}
		}
	}
}

bool exceedExtremum(float bijiaoY1,float bijiaoY2,std::list<float> maxY,std::list<float> minY)  //�����ж�ǰ������ֱ���Ƿ�Խ���˾ֲ���ֵ��ĺ���
{
    if (bijiaoY1>bijiaoY2)
    {
        return false;
    }
    for (auto i=maxY.begin();i!=maxY.end();++i)
    {
        if ((*i-bijiaoY1)*(*i-bijiaoY2)<0)
        {
            return true;
        }
    }
    for (auto i=minY.begin();i!=minY.end();++i)
    {
        if ((*i-bijiaoY1)*(*i-bijiaoY2)<0)
        {
            return true;
        }
    }
    return false;
}



void DealCompensate(outline DealData,outline & ResultData,float R) //�������������ݣ�ʹ���Ϊ�в����뾶�����������ݡ� ���뵽�Լ����ڳ����У�2015/3/10 �����д�
{
    float layerZ=DealData[0].z;
    auto subscript1=DealData.size()-2;  //�����������һ�������һ�����ظ�����ʱ�̼��ţ�����
    auto subscript2=0;
    auto subscript3=1;
    CTwoDimensionVector L1(DealData[subscript2].x-DealData[subscript1].x,DealData[subscript2].y-DealData[subscript1].y);
    CTwoDimensionVector L2(DealData[subscript3].x-DealData[subscript2].x,DealData[subscript3].y-DealData[subscript2].y);

  /*  if ((L1.CrossProduct(L2)<1)&&(L1.DotProduct(L2)>=-1))
    {
        ResultData.push_back(xdpoint((L2.x-L1.x)*R/L1.CrossProduct(L2)+DealData[subscript2].x,(L2.y-L1.y)*R/L1.CrossProduct(L2)+DealData[subscript2].y,layerZ));

    }
    else if((L1.CrossProduct(L2)>-5)) //������Ҫ�ǿ��ܳ��ַǳ�ƽ�е������Ӱ���жϣ�����
    {
        ResultData.push_back(xdpoint((L2.x-L1.x)*R/L1.CrossProduct(L2)+DealData[subscript2].x,(L2.y-L1.y)*R/L1.CrossProduct(L2)+DealData[subscript2].y,layerZ));
    }
    else if (L1.CrossProduct(L2)==0)
    {

    }
    else //if((L1.CrossProduct(L2)<0)&&(L1.DotProduct(L2)<0))
    {
        ResultData.push_back(xdpoint((L1.x-L1.y)*R+DealData[subscript2].x,(L1.x+L1.y)*R+DealData[subscript2].y,layerZ));
       ResultData.push_back(xdpoint(-(L2.x+L2.y)*R+DealData[subscript2].x,(L2.x-L2.y)*R+DealData[subscript2].y,layerZ));  //�����������ʱ���ۻ�һ����ҪŪ���ˣ�
    } */

    ResultData.push_back(xdpoint((L2.x-L1.x)*R/L1.CrossProduct(L2)+DealData[subscript2].x,(L2.y-L1.y)*R/L1.CrossProduct(L2)+DealData[subscript2].y,layerZ));
    for (int i=1;i<DealData.size()-1;i++)
    {
        subscript1=i-1;
        subscript2=i;
        subscript3=i+1;
        CTwoDimensionVector L1(DealData[subscript2].x-DealData[subscript1].x,DealData[subscript2].y-DealData[subscript1].y);
        CTwoDimensionVector L2(DealData[subscript3].x-DealData[subscript2].x,DealData[subscript3].y-DealData[subscript2].y);
        /*
        if(i==8)
        {
            double tem1=L1.CrossProduct(L2);
            double tem2=L1.DotProduct(L2);
            std::cout<<std::endl;
        }
        */
      /*  if ((L1.CrossProduct(L2)<1)&&(L1.DotProduct(L2)>=-1))
        {
            ResultData.push_back(xdpoint((L2.x-L1.x)*R/L1.CrossProduct(L2)+DealData[subscript2].x,(L2.y-L1.y)*R/L1.CrossProduct(L2)+DealData[subscript2].y,layerZ));

        }
        else if((L1.CrossProduct(L2)>-5))   //������Ҫ�ǿ��ܳ��ַǳ�ƽ�е������Ӱ���жϣ�����
        {
            ResultData.push_back(xdpoint((L2.x-L1.x)*R/L1.CrossProduct(L2)+DealData[subscript2].x,(L2.y-L1.y)*R/L1.CrossProduct(L2)+DealData[subscript2].y,layerZ));
        }
        else if (L1.CrossProduct(L2)==0)
        {
             //������������ܷ�������Ϊǰ���ߵĿ������Ѿ���ɾ���ˣ�
        }
        else //if((L1.CrossProduct(L2)<0)&&(L1.DotProduct(L2)<0))
        {
           ResultData.push_back(xdpoint((L1.x-L1.y)*R+DealData[subscript2].x,(L1.x+L1.y)*R+DealData[subscript2].y,layerZ));
           ResultData.push_back(xdpoint(-(L2.x+L2.y)*R+DealData[subscript2].x,(L2.x-L2.y)*R+DealData[subscript2].y,layerZ));  //�����������ʱ���ۻ�һ����ҪŪ���ˣ�

        } */

         ResultData.push_back(xdpoint((L2.x-L1.x)*R/L1.CrossProduct(L2)+DealData[subscript2].x,(L2.y-L1.y)*R/L1.CrossProduct(L2)+DealData[subscript2].y,layerZ));
    }
    ResultData.push_back(xdpoint(ResultData[0].x,ResultData[0].y,layerZ));  //��֤���һ����͵�һ������ͬ��
}

void InfillLine(outlines TheOutline, outlines & TheResult, float width, float degree, int lunkuo,  float shrinkDistance,float offsetWidth) //�Լ���д����������ɺ�����
{
	if(!TheOutline.empty())  //���ȱ��뱣֤������
	{
		//������������תһ���Ƕ�
		for (int i=0;i!=TheOutline.size();++i)
		{
			for (int j=0;j!=TheOutline[i].size();++j)
			{
				TheOutline[i][j].argument(TheOutline[i][j].argument()-degree);
			}
		}

		//��Ҫһ������ƫ����ʹ���������Ƚ�ƽ��
		std::vector<outlines> dataOffsets;  //�洢����ƫ������������
		if(lunkuo!=0)
		{
			ClipperLib::ClipperOffset temO;
			for (int i=0;i!=TheOutline.size();++i)
			{
				ClipperLib::Path temP;
				for (int j=0;j!=TheOutline[i].size();++j)
				{
					temP<<ClipperLib::IntPoint(TheOutline[i][j].x*1000000,TheOutline[i][j].y*1000000);
				}
				temO.AddPath(temP,ClipperLib::jtSquare, ClipperLib::etClosedPolygon);
			}
		
			for(int temi=0;temi!=lunkuo;++temi)
			{
				ClipperLib::Paths solution; 		
				temO.Execute(solution, -offsetWidth*1000000*(temi+1)); 

				xd::outlines dataOffset;
				for (int i=0;i!=solution.size();++i)
				{
					xd::outline temData;
					for (int j=0;j!=solution[i].size();++j)
					{
						temData.push_back(xd::xdpoint((float)solution[i][j].X/1000000.0,(float)solution[i][j].Y/1000000.0));
					}
					temData.push_back(xd::xdpoint((float)solution[i][0].X/1000000.0,(float)solution[i][0].Y/1000000.0));//�������һ���㣬��֤���
					dataOffset.push_back(temData);
				}
				dataOffsets.push_back(dataOffset);
			}		
			TheOutline.clear();
			TheOutline=dataOffsets[dataOffsets.size()-1];        
		}
		if(TheOutline.size()!=0)     
		{
			//��һ����������ľֲ�����Сֵ�㣬����һ��˫�������С�ע�⣺ѡ��˫�������ԭ����������λ��ɾ�������Ԫ�طǳ���ݣ�����ʱ�临�Ӷȡ�
			std::list<float> maxY;
			std::list<float> minY;
			std::vector<std::pair<float,float>> maxPoint;
			std::vector<std::pair<float,float>> minPoint;
			for (int i=0;i!=TheOutline.size();i++)
			{
				for (int j=1;j!=TheOutline[i].size();j++)
				{
					int n=(int)TheOutline[i].size()-1;  //�������ݵ����һ�����ǵ�һ���㣬���Ҫ��ȥ1��
					int beforeP=(j-1+n)%n;
					int beforebP=(j-2+n)%n;
					int nextP=(j+1+n)%n;
					int nextnP=(j+2+n)%n;
					if ((TheOutline[i][j].y>TheOutline[i][beforeP].y)&&(TheOutline[i][j].y>TheOutline[i][nextP].y))  //������yֵ����ǰ��ĵ��yֵ��
					{
						maxY.push_back(TheOutline[i][j].y);
						std::pair<float,float> tem;
						tem.first=TheOutline[i][j].x;
						tem.second=TheOutline[i][j].y;
						maxPoint.push_back(tem);
					}
					else if ((TheOutline[i][j].y<TheOutline[i][beforeP].y)&&(TheOutline[i][j].y<TheOutline[i][nextP].y)) //������yֵС��ǰ��ĵ��yֵ��
					{
						minY.push_back(TheOutline[i][j].y);
						std::pair<float,float> tem;
						tem.first=TheOutline[i][j].x;
						tem.second=TheOutline[i][j].y;
						minPoint.push_back(tem);
					}
					else if (TheOutline[i][j].y==TheOutline[i][beforeP].y)
					{
						if ((TheOutline[i][beforeP].y>TheOutline[i][beforebP].y)&&(TheOutline[i][j].y>TheOutline[i][nextP].y)) //������ǰһ�����yֵ��
						{
							maxY.push_back(TheOutline[i][j].y);
						}
						if ((TheOutline[i][beforeP].y<TheOutline[i][beforebP].y)&&(TheOutline[i][j].y<TheOutline[i][nextP].y)) //������ǰһ�����yֵС��
						{
							minY.push_back(TheOutline[i][j].y);
						}
					}
					else if (TheOutline[i][j].y==TheOutline[i][nextP].y)
					{
						if ((TheOutline[i][nextP].y>TheOutline[i][nextnP].y)&&(TheOutline[i][j].y>TheOutline[i][beforeP].y))  //�����ͺ�һ�����yֵ��
						{
							maxY.push_back(TheOutline[i][j].y);
						}
						if ((TheOutline[i][nextP].y<TheOutline[i][nextnP].y)&&(TheOutline[i][j].y<TheOutline[i][beforeP].y))  //�����ͺ�һ�����yֵС��
						{
							minY.push_back(TheOutline[i][j].y);
						}
					}
				}
			}
			maxY.sort();
			minY.sort();
			maxY.unique();    //���������ظ��ĵ�ɾ���ĺ���������STL�ı����㷨��
			minY.unique();	  //ͬ�ϡ�
			float MaxY=*max_element(maxY.begin(),maxY.end());   //�����������������Ԫ�صĺ�����
			float MinY=*min_element(minY.begin(),minY.end());
			//�ڶ���������ÿһ����������������ߵĽ��㣬�ֱ�洢��˫�������С�
			std::vector<std::pair<float,std::list<float>>> Linedate;
			for (int i=1;i<(MaxY-MinY)/width-1;i++)
			{
				std::pair<float,std::list<float>> tem;
				tem.first=MinY+width*i;
				Linedate.push_back(tem);
			}
			if(!Linedate.empty())     //��ס��Linedate�����ǿյģ� 2015_6_17
			{
				if ((MaxY-Linedate[Linedate.size()-1].first)>width*3/2)//Ϊ��ʹ�����һ������߲��������������̫Զ��Ҫ��һ���Ƿ���Ҫ����һ���߶��ж�
				{

					std::pair<float,std::list<float>> tem;
					tem.first=(MaxY+Linedate[Linedate.size()-1].first)/2;
					Linedate.push_back(tem);
				}
				else if ((MaxY-Linedate[Linedate.size()-1].first)<=width*3/2&&(MaxY-Linedate[Linedate.size()-1].first)>width)  //��ÿһ���ߵļ�඼����һ�������Ӧ�߿���
				{
					for (int i=0;i!=Linedate.size();++i)
					{
						Linedate[i].first+=(MaxY-Linedate[Linedate.size()-1].first-width)/Linedate.size();
					}
				}

				for (int i=0;i!=TheOutline.size();i++)
				{

					for (int j=1;j!=TheOutline[i].size();j++)  //j=1��˵��Ҫ�ӵڶ����㿪ʼѭ����ֱ����һ���㡣
					{
						int n=(int)TheOutline[i].size()-1;  //�������ݵ����һ�����ǵ�һ���㣬���Ҫ��ȥ1��
						int beforeP=(j-1+n)%n;
						int beforebP=(j-2+n)%n;
						int nextP=(j+1+n)%n;
						int nextnP=(j+2+n)%n;
						if (TheOutline[i][j].y!=TheOutline[i][nextP].y) //�����߶β�ƽ����x��������
						{
							for (int k=0;k!=Linedate.size();k++)
							{
								if ((Linedate[k].first-TheOutline[i][j].y)*(Linedate[k].first-TheOutline[i][nextP].y)<0) //���������߶��ཻ�������
								{
									float x1=TheOutline[i][j].x;
									float x2=TheOutline[i][nextP].x;
									float y1=TheOutline[i][j].y;
									float y2=TheOutline[i][nextP].y;
									Linedate[k].second.push_back((x2-x1)/(y2-y1)*(Linedate[k].first-y1)+x1);
								}
								else if ((Linedate[k].first==TheOutline[i][j].y))    //���������߶�ǰһ�������ཻ�������
								{
									if ((!IsContainPoint(maxPoint,TheOutline[i][j]))&&(!IsContainPoint(minPoint,TheOutline[i][j]))&&(TheOutline[i][j].y!=TheOutline[i][beforeP].y))
									{
										//�������ͬʱ�ֲ��Ǽ�ֵ��ʱ��ͬʱ�������ǰһ�����yֵ������Ȳ��У���Ϊ���ʱ������Ѿ��Ѿ���������߼����ǹ��ˣ�����
										Linedate[k].second.push_back(TheOutline[i][j].x);
									}
								}
								else if ((Linedate[k].first==TheOutline[i][nextP].y))   //���������߶κ�һ�������ཻ�������
								{
									if ((!IsContainPoint(maxPoint,TheOutline[i][nextP]))&&(!IsContainPoint(minPoint,TheOutline[i][nextP]))&&(TheOutline[i][nextP].y!=TheOutline[i][nextnP].y))
									{
										//�������ͬʱ�ֲ��Ǽ�ֵ��ʱ��ͬʱ��һ��������һ�����yֵ������Ȳ��У���Ϊ���ʱ������Ѿ��Ѿ���������߼����ǹ��ˣ�����
										Linedate[k].second.push_back(TheOutline[i][nextP].x);
									}
								}
							}
						}
						else if(TheOutline[i][j].y==TheOutline[i][nextP].y) //�����߶�ƽ����x��������
						{
							for (int k=0;k!=Linedate.size();k++)
							{
								if (Linedate[k].first==TheOutline[i][j].y)  //�������ǡ����ƽ����x����������ཻ��
								{
									if((IsLeft(TheOutline[i][beforeP],TheOutline[i][j],TheOutline[i][nextP])==-1)&&(IsLeft(TheOutline[i][j],TheOutline[i][nextP],TheOutline[i][nextnP])==-1))  //��������ߵ�����ͬʱ���ڵ�ʱ��
									{   //˳ʱ������ת���ڵ㡣
										Linedate[k].second.push_back(TheOutline[i][j].x);
										Linedate[k].second.push_back(TheOutline[i][nextP].x);
									}
									else if((IsLeft(TheOutline[i][beforeP],TheOutline[i][j],TheOutline[i][nextP])==1)&&(IsLeft(TheOutline[i][j],TheOutline[i][nextP],TheOutline[i][nextnP])==1))
									{
										//�����㶼�����ڵ㣬���㶼��ȥ�����仰˵����ʲôҲ��������ʵ����д��δ��룬Ϊ���Ժ���������д���ˡ�
									}

									else if((IsLeft(TheOutline[i][beforeP],TheOutline[i][j],TheOutline[i][nextP])==-1)&&(IsLeft(TheOutline[i][j],TheOutline[i][nextP],TheOutline[i][nextnP])==1))
									{
										Linedate[k].second.push_back(TheOutline[i][j].x);
									}
									else if((IsLeft(TheOutline[i][beforeP],TheOutline[i][j],TheOutline[i][nextP])==1)&&(IsLeft(TheOutline[i][j],TheOutline[i][nextP],TheOutline[i][nextnP])==-1))
									{
										Linedate[k].second.push_back(TheOutline[i][nextP].x);
									}
								}
							}
						}
					}

				}

				for(int i=0;i!=Linedate.size();i++)  //���洢�õ������ظ���Ԫ��ɾ������������
				{
					Linedate[i].second.sort();    //����
					Linedate[i].second.unique();  //������ĵ�ȡ������
				}
				DeleteOddDate(Linedate);    //��ʱ���ϣ����������

				//�������������ݷ������š�
				int FirstNonZero;
				while(!IsEmpty(Linedate,FirstNonZero))
				{
					outline tem;
					int j=0;  //�����ж��������л���ż���еĲ�����
					auto firstIn=Linedate[FirstNonZero].second.begin();
					float bijiao1=*firstIn;   //��һ���ߵĵ�һ���㡣
					tem.push_back(xdpoint(*firstIn,Linedate[FirstNonZero].first,TheOutline[0][0].z));
					tem.push_back(xdpoint(*(++firstIn),Linedate[FirstNonZero].first,TheOutline[0][0].z));					
					int i=(FirstNonZero+1)%Linedate.size();  //���뱣֤����һ���������ݵ��߸պ������һ����ʱҲ�����1Խ�磡����Ҫģһ�£�
					float bijiao2=*firstIn;  //��һ���ߵĵڶ����㡣
					float bijiaoY1=Linedate[FirstNonZero].first;  //��һ���ߵ�Y����ֵ��
					auto tem1s=Linedate[FirstNonZero].second.begin();
					auto tem1e=tem1s;
					tem1e++;
					tem1e++;
					Linedate[FirstNonZero].second.erase(tem1s,tem1e);
					if (std::abs(tem[1].x - tem[0].x)<shrinkDistance*2)  //���������ľ���̫���ˣ���������ˣ���important������
						continue;
					while ((!Linedate[i].second.empty()))
					{
						auto Line2first=Linedate[i].second.begin();  //�ڶ����ߵĵ�һ���㡣

						float bijiaoY2=Linedate[i].first; //�ڶ�����ֱ�ߵ�Y����ֵ��
						if (exceedExtremum(bijiaoY1,bijiaoY2,maxY,minY))
						{
							//ǰ������ֱ�������Խ�˾ֲ���ֵ�㣬Ҳ��Ҫ�˳�������Ҫ������
							break;
						}
						bijiaoY1=bijiaoY2;
						if(*Line2first>bijiao2)
						{
							//�ڶ���ֱ�ߵĵ�һ������ڵ�һ��ֱ�ߵĵڶ��������˳�������Ҫ����
							break;
						}
						if (*(++Line2first)<bijiao1)   //ע�⣺����++������ǰ����Ϊ���������ţ�++�ں�Ļ�Ҳ��Ƚ�����++������
						{
							//�ڶ���ֱ�ߵĵڶ�����С�ڵ�һ��ֱ�ߵĵ�һ�������˳�������Ҫ����
							break;
						}
						auto tem2s=Linedate[i].second.begin();
						auto tem2e=tem2s;
						tem2e++;
						if (std::abs(*tem2e - *tem2s)<shrinkDistance*2)//���������ľ���̫���ˣ���������ˣ���important������
							break;
						if (j%2==0)
						{
							/*auto tem2s=Linedate[i].second.begin();
							auto tem2e=tem2s;
							tem2e++;*/
							float vectorY=Linedate[i].first-tem.back().y;
							float vectorX=*tem2e-tem.back().x;
							if (abs(vectorY/sqrt(pow(vectorX,2)+pow(vectorY,2)))<sin(5.0/180*pi))   //�����ߺ�ˮƽ�����1�Ⱦͷ������ǵ���1.0
							{
								break;
							}
							tem.push_back(xdpoint(*tem2e,Linedate[i].first,TheOutline[0][0].z));
							tem.push_back(xdpoint(*tem2s,Linedate[i].first,TheOutline[0][0].z));
						}
						else
						{
							/*auto tem2s=Linedate[i].second.begin();
							auto tem2e=tem2s;
							tem2e++;*/
							float vectorY=Linedate[i].first-tem.back().y;
							float vectorX=*tem2s-tem.back().x;
							if (abs(vectorY/sqrt(pow(vectorX,2)+pow(vectorY,2)))<sin(5.0/180*pi))   //�����ߺ�ˮƽ�����1�Ⱦͷ������ǵ���1.0
							{
								break;
							}
							tem.push_back(xdpoint(*tem2s,Linedate[i].first,TheOutline[0][0].z));
							tem.push_back(xdpoint(*tem2e,Linedate[i].first,TheOutline[0][0].z));
						}
						auto tem3s=Linedate[i].second.begin();
						auto tem3e=tem3s;
						bijiao1=*tem3e;
						tem3e++;
						bijiao2=*tem3e;
						tem3e++;
						Linedate[i].second.erase(tem3s,tem3e);
						++j;
						++i;
						if (i==Linedate.size())
						{
							break;
						}
					}
					TheResult.push_back(tem);
				}

				//����һ����Ϊ����ӦFDM�ĸ�ʽ��ÿ���߶�Ҫ����һ���뾶����
				for (int i=0;i!=TheResult.size();++i)
				{
					for (int j=0;j!=TheResult[i].size();++j)
					{

						if (j%4==3||j%4==0)
						{
							TheResult[i][j].x += shrinkDistance;    //�˴���Ĭ��ֵ��

						}
						else
						{
							TheResult[i][j].x -= shrinkDistance;
						}
					}
				}
			}
		}
			
		//�ǵ�������ƫ��������
		if(lunkuo!=0)
		{
			for(int i=0;i!=dataOffsets.size();++i)
			{
				for(int j=0;j!=dataOffsets[i].size();++j)
				{
					TheResult.insert(TheResult.begin(),dataOffsets[i][j]);
				}
			}
		}
		//����ߵĽ��ת��ѡ��Ƕ�
		for (int i=0;i!=TheResult.size();++i)
		{
			for (int j=0;j!=TheResult[i].size();++j)
			{
				TheResult[i][j].argument(TheResult[i][j].argument()+degree);
			}
		}
	}
}

void InfillLine(outlines TheOutline, outlines & TheResult, outlines & TheOutlineResult, float width, float degree, int lunkuo, float shrinkDistance, float offsetWidth)
{
	if (!TheOutline.empty())  //���ȱ��뱣֤������
	{
		//������������תһ���Ƕ�
		for (int i = 0; i != TheOutline.size(); ++i)
		{
			for (int j = 0; j != TheOutline[i].size(); ++j)
			{
				TheOutline[i][j].argument(TheOutline[i][j].argument() - degree);
			}
		}

		//��Ҫһ������ƫ����ʹ���������Ƚ�ƽ��
		std::vector<outlines> dataOffsets;  //�洢����ƫ������������
		if (lunkuo != 0)
		{
			ClipperLib::ClipperOffset temO;
			for (int i = 0; i != TheOutline.size(); ++i)
			{
				ClipperLib::Path temP;
				for (int j = 0; j != TheOutline[i].size(); ++j)
				{
					temP << ClipperLib::IntPoint(TheOutline[i][j].x * 1000000, TheOutline[i][j].y * 1000000);
				}
				temO.AddPath(temP, ClipperLib::jtSquare, ClipperLib::etClosedPolygon);
			}

			for (int temi = 0; temi != lunkuo; ++temi)
			{
				ClipperLib::Paths solution;
				temO.Execute(solution, -offsetWidth * 1000000 * (temi + 1));

				xd::outlines dataOffset;
				for (int i = 0; i != solution.size(); ++i)
				{
					xd::outline temData;
					for (int j = 0; j != solution[i].size(); ++j)
					{
						temData.push_back(xd::xdpoint((float)solution[i][j].X / 1000000.0, (float)solution[i][j].Y / 1000000.0));
					}
					temData.push_back(xd::xdpoint((float)solution[i][0].X / 1000000.0, (float)solution[i][0].Y / 1000000.0));//�������һ���㣬��֤���
					dataOffset.push_back(temData);
				}
				dataOffsets.push_back(dataOffset);
			}
			TheOutline.clear();
			TheOutline = dataOffsets[dataOffsets.size() - 1];
		}
		if (TheOutline.size() != 0)
		{
			//��һ����������ľֲ�����Сֵ�㣬����һ��˫�������С�ע�⣺ѡ��˫�������ԭ����������λ��ɾ�������Ԫ�طǳ���ݣ�����ʱ�临�Ӷȡ�
			std::list<float> maxY;
			std::list<float> minY;
			std::vector<std::pair<float, float>> maxPoint;
			std::vector<std::pair<float, float>> minPoint;
			for (int i = 0; i != TheOutline.size(); i++)
			{
				for (int j = 1; j != TheOutline[i].size(); j++)
				{
					int n = (int)TheOutline[i].size() - 1;  //�������ݵ����һ�����ǵ�һ���㣬���Ҫ��ȥ1��
					int beforeP = (j - 1 + n) % n;
					int beforebP = (j - 2 + n) % n;
					int nextP = (j + 1 + n) % n;
					int nextnP = (j + 2 + n) % n;
					if ((TheOutline[i][j].y>TheOutline[i][beforeP].y) && (TheOutline[i][j].y>TheOutline[i][nextP].y))  //������yֵ����ǰ��ĵ��yֵ��
					{
						maxY.push_back(TheOutline[i][j].y);
						std::pair<float, float> tem;
						tem.first = TheOutline[i][j].x;
						tem.second = TheOutline[i][j].y;
						maxPoint.push_back(tem);
					}
					else if ((TheOutline[i][j].y<TheOutline[i][beforeP].y) && (TheOutline[i][j].y<TheOutline[i][nextP].y)) //������yֵС��ǰ��ĵ��yֵ��
					{
						minY.push_back(TheOutline[i][j].y);
						std::pair<float, float> tem;
						tem.first = TheOutline[i][j].x;
						tem.second = TheOutline[i][j].y;
						minPoint.push_back(tem);
					}
					else if (TheOutline[i][j].y == TheOutline[i][beforeP].y)
					{
						if ((TheOutline[i][beforeP].y>TheOutline[i][beforebP].y) && (TheOutline[i][j].y>TheOutline[i][nextP].y)) //������ǰһ�����yֵ��
						{
							maxY.push_back(TheOutline[i][j].y);
						}
						if ((TheOutline[i][beforeP].y<TheOutline[i][beforebP].y) && (TheOutline[i][j].y<TheOutline[i][nextP].y)) //������ǰһ�����yֵС��
						{
							minY.push_back(TheOutline[i][j].y);
						}
					}
					else if (TheOutline[i][j].y == TheOutline[i][nextP].y)
					{
						if ((TheOutline[i][nextP].y>TheOutline[i][nextnP].y) && (TheOutline[i][j].y>TheOutline[i][beforeP].y))  //�����ͺ�һ�����yֵ��
						{
							maxY.push_back(TheOutline[i][j].y);
						}
						if ((TheOutline[i][nextP].y<TheOutline[i][nextnP].y) && (TheOutline[i][j].y<TheOutline[i][beforeP].y))  //�����ͺ�һ�����yֵС��
						{
							minY.push_back(TheOutline[i][j].y);
						}
					}
				}
			}
			maxY.sort();
			minY.sort();
			maxY.unique();    //���������ظ��ĵ�ɾ���ĺ���������STL�ı����㷨��
			minY.unique();	  //ͬ�ϡ�
			float MaxY = *max_element(maxY.begin(), maxY.end());   //�����������������Ԫ�صĺ�����
			float MinY = *min_element(minY.begin(), minY.end());
			//�ڶ���������ÿһ����������������ߵĽ��㣬�ֱ�洢��˫�������С�
			std::vector<std::pair<float, std::list<float>>> Linedate;
			for (int i = 1; i<(MaxY - MinY) / width - 1; i++)
			{
				std::pair<float, std::list<float>> tem;
				tem.first = MinY + width*i;
				Linedate.push_back(tem);
			}
			if (!Linedate.empty())     //��ס��Linedate�����ǿյģ� 2015_6_17
			{
				if ((MaxY - Linedate[Linedate.size() - 1].first)>width * 3 / 2)//Ϊ��ʹ�����һ������߲��������������̫Զ��Ҫ��һ���Ƿ���Ҫ����һ���߶��ж�
				{

					std::pair<float, std::list<float>> tem;
					tem.first = (MaxY + Linedate[Linedate.size() - 1].first) / 2;
					Linedate.push_back(tem);
				}
				else if ((MaxY - Linedate[Linedate.size() - 1].first) <= width * 3 / 2 && (MaxY - Linedate[Linedate.size() - 1].first)>width)  //��ÿһ���ߵļ�඼����һ�������Ӧ�߿���
				{
					for (int i = 0; i != Linedate.size(); ++i)
					{
						Linedate[i].first += (MaxY - Linedate[Linedate.size() - 1].first - width) / Linedate.size();
					}
				}

				for (int i = 0; i != TheOutline.size(); i++)
				{

					for (int j = 1; j != TheOutline[i].size(); j++)  //j=1��˵��Ҫ�ӵڶ����㿪ʼѭ����ֱ����һ���㡣
					{
						int n = (int)TheOutline[i].size() - 1;  //�������ݵ����һ�����ǵ�һ���㣬���Ҫ��ȥ1��
						int beforeP = (j - 1 + n) % n;
						int beforebP = (j - 2 + n) % n;
						int nextP = (j + 1 + n) % n;
						int nextnP = (j + 2 + n) % n;
						if (TheOutline[i][j].y != TheOutline[i][nextP].y) //�����߶β�ƽ����x��������
						{
							for (int k = 0; k != Linedate.size(); k++)
							{
								if ((Linedate[k].first - TheOutline[i][j].y)*(Linedate[k].first - TheOutline[i][nextP].y)<0) //���������߶��ཻ�������
								{
									float x1 = TheOutline[i][j].x;
									float x2 = TheOutline[i][nextP].x;
									float y1 = TheOutline[i][j].y;
									float y2 = TheOutline[i][nextP].y;
									Linedate[k].second.push_back((x2 - x1) / (y2 - y1)*(Linedate[k].first - y1) + x1);
								}
								else if ((Linedate[k].first == TheOutline[i][j].y))    //���������߶�ǰһ�������ཻ�������
								{
									if ((!IsContainPoint(maxPoint, TheOutline[i][j])) && (!IsContainPoint(minPoint, TheOutline[i][j])) && (TheOutline[i][j].y != TheOutline[i][beforeP].y))
									{
										//�������ͬʱ�ֲ��Ǽ�ֵ��ʱ��ͬʱ�������ǰһ�����yֵ������Ȳ��У���Ϊ���ʱ������Ѿ��Ѿ���������߼����ǹ��ˣ�����
										Linedate[k].second.push_back(TheOutline[i][j].x);
									}
								}
								else if ((Linedate[k].first == TheOutline[i][nextP].y))   //���������߶κ�һ�������ཻ�������
								{
									if ((!IsContainPoint(maxPoint, TheOutline[i][nextP])) && (!IsContainPoint(minPoint, TheOutline[i][nextP])) && (TheOutline[i][nextP].y != TheOutline[i][nextnP].y))
									{
										//�������ͬʱ�ֲ��Ǽ�ֵ��ʱ��ͬʱ��һ��������һ�����yֵ������Ȳ��У���Ϊ���ʱ������Ѿ��Ѿ���������߼����ǹ��ˣ�����
										Linedate[k].second.push_back(TheOutline[i][nextP].x);
									}
								}
							}
						}
						else if (TheOutline[i][j].y == TheOutline[i][nextP].y) //�����߶�ƽ����x��������
						{
							for (int k = 0; k != Linedate.size(); k++)
							{
								if (Linedate[k].first == TheOutline[i][j].y)  //�������ǡ����ƽ����x����������ཻ��
								{
									if ((IsLeft(TheOutline[i][beforeP], TheOutline[i][j], TheOutline[i][nextP]) == -1) && (IsLeft(TheOutline[i][j], TheOutline[i][nextP], TheOutline[i][nextnP]) == -1))  //��������ߵ�����ͬʱ���ڵ�ʱ��
									{   //˳ʱ������ת���ڵ㡣
										Linedate[k].second.push_back(TheOutline[i][j].x);
										Linedate[k].second.push_back(TheOutline[i][nextP].x);
									}
									else if ((IsLeft(TheOutline[i][beforeP], TheOutline[i][j], TheOutline[i][nextP]) == 1) && (IsLeft(TheOutline[i][j], TheOutline[i][nextP], TheOutline[i][nextnP]) == 1))
									{
										//�����㶼�����ڵ㣬���㶼��ȥ�����仰˵����ʲôҲ��������ʵ����д��δ��룬Ϊ���Ժ���������д���ˡ�
									}

									else if ((IsLeft(TheOutline[i][beforeP], TheOutline[i][j], TheOutline[i][nextP]) == -1) && (IsLeft(TheOutline[i][j], TheOutline[i][nextP], TheOutline[i][nextnP]) == 1))
									{
										Linedate[k].second.push_back(TheOutline[i][j].x);
									}
									else if ((IsLeft(TheOutline[i][beforeP], TheOutline[i][j], TheOutline[i][nextP]) == 1) && (IsLeft(TheOutline[i][j], TheOutline[i][nextP], TheOutline[i][nextnP]) == -1))
									{
										Linedate[k].second.push_back(TheOutline[i][nextP].x);
									}
								}
							}
						}
					}

				}

				for (int i = 0; i != Linedate.size(); i++)  //���洢�õ������ظ���Ԫ��ɾ������������
				{
					Linedate[i].second.sort();    //����
					Linedate[i].second.unique();  //������ĵ�ȡ������
				}
				DeleteOddDate(Linedate);    //��ʱ���ϣ����������

				//�������������ݷ������š�
				int FirstNonZero;
				while (!IsEmpty(Linedate, FirstNonZero))
				{
					outline tem;
					int j = 0;  //�����ж��������л���ż���еĲ�����
					auto firstIn = Linedate[FirstNonZero].second.begin();
					float bijiao1 = *firstIn;   //��һ���ߵĵ�һ���㡣
					tem.push_back(xdpoint(*firstIn, Linedate[FirstNonZero].first, TheOutline[0][0].z));
					tem.push_back(xdpoint(*(++firstIn), Linedate[FirstNonZero].first, TheOutline[0][0].z));
					int i = (FirstNonZero + 1) % Linedate.size();  //���뱣֤����һ���������ݵ��߸պ������һ����ʱҲ�����1Խ�磡����Ҫģһ�£�
					float bijiao2 = *firstIn;  //��һ���ߵĵڶ����㡣
					float bijiaoY1 = Linedate[FirstNonZero].first;  //��һ���ߵ�Y����ֵ��
					auto tem1s = Linedate[FirstNonZero].second.begin();
					auto tem1e = tem1s;
					tem1e++;
					tem1e++;
					Linedate[FirstNonZero].second.erase(tem1s, tem1e);
					if (std::abs(tem[1].x - tem[0].x)<shrinkDistance * 2)  //���������ľ���̫���ˣ���������ˣ���important������
						continue;
					while ((!Linedate[i].second.empty()))
					{
						auto Line2first = Linedate[i].second.begin();  //�ڶ����ߵĵ�һ���㡣

						float bijiaoY2 = Linedate[i].first; //�ڶ�����ֱ�ߵ�Y����ֵ��
						if (exceedExtremum(bijiaoY1, bijiaoY2, maxY, minY))
						{
							//ǰ������ֱ�������Խ�˾ֲ���ֵ�㣬Ҳ��Ҫ�˳�������Ҫ������
							break;
						}
						bijiaoY1 = bijiaoY2;
						if (*Line2first>bijiao2)
						{
							//�ڶ���ֱ�ߵĵ�һ������ڵ�һ��ֱ�ߵĵڶ��������˳�������Ҫ����
							break;
						}
						if (*(++Line2first)<bijiao1)   //ע�⣺����++������ǰ����Ϊ���������ţ�++�ں�Ļ�Ҳ��Ƚ�����++������
						{
							//�ڶ���ֱ�ߵĵڶ�����С�ڵ�һ��ֱ�ߵĵ�һ�������˳�������Ҫ����
							break;
						}
						auto tem2s = Linedate[i].second.begin();
						auto tem2e = tem2s;
						tem2e++;
						if (std::abs(*tem2e - *tem2s)<shrinkDistance * 2)//���������ľ���̫���ˣ���������ˣ���important������
							break;
						if (j % 2 == 0)
						{
							/*auto tem2s=Linedate[i].second.begin();
							auto tem2e=tem2s;
							tem2e++;*/
							float vectorY = Linedate[i].first - tem.back().y;
							float vectorX = *tem2e - tem.back().x;
							if (abs(vectorY / sqrt(pow(vectorX, 2) + pow(vectorY, 2)))<sin(5.0 / 180 * pi))   //�����ߺ�ˮƽ�����1�Ⱦͷ������ǵ���1.0
							{
								break;
							}
							tem.push_back(xdpoint(*tem2e, Linedate[i].first, TheOutline[0][0].z));
							tem.push_back(xdpoint(*tem2s, Linedate[i].first, TheOutline[0][0].z));
						}
						else
						{
							/*auto tem2s=Linedate[i].second.begin();
							auto tem2e=tem2s;
							tem2e++;*/
							float vectorY = Linedate[i].first - tem.back().y;
							float vectorX = *tem2s - tem.back().x;
							if (abs(vectorY / sqrt(pow(vectorX, 2) + pow(vectorY, 2)))<sin(5.0 / 180 * pi))   //�����ߺ�ˮƽ�����1�Ⱦͷ������ǵ���1.0
							{
								break;
							}
							tem.push_back(xdpoint(*tem2s, Linedate[i].first, TheOutline[0][0].z));
							tem.push_back(xdpoint(*tem2e, Linedate[i].first, TheOutline[0][0].z));
						}
						auto tem3s = Linedate[i].second.begin();
						auto tem3e = tem3s;
						bijiao1 = *tem3e;
						tem3e++;
						bijiao2 = *tem3e;
						tem3e++;
						Linedate[i].second.erase(tem3s, tem3e);
						++j;
						++i;
						if (i == Linedate.size())
						{
							break;
						}
					}
					TheResult.push_back(tem);
				}

				//����һ����Ϊ����ӦFDM�ĸ�ʽ��ÿ���߶�Ҫ����һ���뾶����
				for (int i = 0; i != TheResult.size(); ++i)
				{
					for (int j = 0; j != TheResult[i].size(); ++j)
					{

						if (j % 4 == 3 || j % 4 == 0)
						{
							TheResult[i][j].x += shrinkDistance;    //�˴���Ĭ��ֵ��

						}
						else
						{
							TheResult[i][j].x -= shrinkDistance;
						}
					}
				}
			}
		}

		//�ǵ�������ƫ��������
		if (lunkuo != 0)
		{
			for (int i = 0; i != dataOffsets.size(); ++i)
			{
				for (int j = 0; j != dataOffsets[i].size(); ++j)
				{
					TheOutlineResult.insert(TheOutlineResult.begin(), dataOffsets[i][j]);
				}
			}
		}
		//����ߵĽ��ת��ѡ��Ƕ�
		for (int i = 0; i != TheResult.size(); ++i)
		{
			for (int j = 0; j != TheResult[i].size(); ++j)
			{
				TheResult[i][j].argument(TheResult[i][j].argument() + degree);
			}
		}
		for (int i = 0; i != TheOutlineResult.size(); ++i)
		{
			for (int j = 0; j != TheOutlineResult[i].size(); ++j)
			{
				TheOutlineResult[i][j].argument(TheOutlineResult[i][j].argument() + degree);
			}
		}
	}
}

void notInfillLine(outlines TheOutline, outlines & TheResult, outlines & TheOutlineResult, float width, float degree, int lunkuo, float shrinkDistance, float offsetWidth)//������������㷨��Ϊ�˼���
{
	if (!TheOutline.empty())  //���ȱ��뱣֤������
	{
		//������������תһ���Ƕ�
		for (int i = 0; i != TheOutline.size(); ++i)
		{
			for (int j = 0; j != TheOutline[i].size(); ++j)
			{
				TheOutline[i][j].argument(TheOutline[i][j].argument() - degree);
			}
		}

		//��Ҫһ������ƫ����ʹ���������Ƚ�ƽ��
		std::vector<outlines> dataOffsets;  //�洢����ƫ������������
		if (lunkuo != 0)
		{
			ClipperLib::ClipperOffset temO;
			for (int i = 0; i != TheOutline.size(); ++i)
			{
				ClipperLib::Path temP;
				for (int j = 0; j != TheOutline[i].size(); ++j)
				{
					temP << ClipperLib::IntPoint(TheOutline[i][j].x * 1000000, TheOutline[i][j].y * 1000000);
				}
				temO.AddPath(temP, ClipperLib::jtSquare, ClipperLib::etClosedPolygon);
			}

			for (int temi = 0; temi != lunkuo; ++temi)
			{
				ClipperLib::Paths solution;
				temO.Execute(solution, -offsetWidth * 1000000 * (temi + 1));

				xd::outlines dataOffset;
				for (int i = 0; i != solution.size(); ++i)
				{
					xd::outline temData;
					for (int j = 0; j != solution[i].size(); ++j)
					{
						temData.push_back(xd::xdpoint((float)solution[i][j].X / 1000000.0, (float)solution[i][j].Y / 1000000.0));
					}
					temData.push_back(xd::xdpoint((float)solution[i][0].X / 1000000.0, (float)solution[i][0].Y / 1000000.0));//�������һ���㣬��֤���
					dataOffset.push_back(temData);
				}
				dataOffsets.push_back(dataOffset);
			}
			TheOutline.clear();
			TheOutline = dataOffsets[dataOffsets.size() - 1];
		}
		if (TheOutline.size() != 0)
		{
			//��һ����������ľֲ�����Сֵ�㣬����һ��˫�������С�ע�⣺ѡ��˫�������ԭ����������λ��ɾ�������Ԫ�طǳ���ݣ�����ʱ�临�Ӷȡ�
			std::list<float> maxY;
			std::list<float> minY;
			std::vector<std::pair<float, float>> maxPoint;
			std::vector<std::pair<float, float>> minPoint;
			for (int i = 0; i != TheOutline.size(); i++)
			{
				for (int j = 1; j != TheOutline[i].size(); j++)
				{
					int n = (int)TheOutline[i].size() - 1;  //�������ݵ����һ�����ǵ�һ���㣬���Ҫ��ȥ1��
					int beforeP = (j - 1 + n) % n;
					int beforebP = (j - 2 + n) % n;
					int nextP = (j + 1 + n) % n;
					int nextnP = (j + 2 + n) % n;
					if ((TheOutline[i][j].y > TheOutline[i][beforeP].y) && (TheOutline[i][j].y > TheOutline[i][nextP].y))  //������yֵ����ǰ��ĵ��yֵ��
					{
						maxY.push_back(TheOutline[i][j].y);
						std::pair<float, float> tem;
						tem.first = TheOutline[i][j].x;
						tem.second = TheOutline[i][j].y;
						maxPoint.push_back(tem);
					}
					else if ((TheOutline[i][j].y < TheOutline[i][beforeP].y) && (TheOutline[i][j].y<TheOutline[i][nextP].y)) //������yֵС��ǰ��ĵ��yֵ��
					{
						minY.push_back(TheOutline[i][j].y);
						std::pair<float, float> tem;
						tem.first = TheOutline[i][j].x;
						tem.second = TheOutline[i][j].y;
						minPoint.push_back(tem);
					}
					else if (TheOutline[i][j].y == TheOutline[i][beforeP].y)
					{
						if ((TheOutline[i][beforeP].y>TheOutline[i][beforebP].y) && (TheOutline[i][j].y > TheOutline[i][nextP].y)) //������ǰһ�����yֵ��
						{
							maxY.push_back(TheOutline[i][j].y);
						}
						if ((TheOutline[i][beforeP].y < TheOutline[i][beforebP].y) && (TheOutline[i][j].y<TheOutline[i][nextP].y)) //������ǰһ�����yֵС��
						{
							minY.push_back(TheOutline[i][j].y);
						}
					}
					else if (TheOutline[i][j].y == TheOutline[i][nextP].y)
					{
						if ((TheOutline[i][nextP].y>TheOutline[i][nextnP].y) && (TheOutline[i][j].y > TheOutline[i][beforeP].y))  //�����ͺ�һ�����yֵ��
						{
							maxY.push_back(TheOutline[i][j].y);
						}
						if ((TheOutline[i][nextP].y < TheOutline[i][nextnP].y) && (TheOutline[i][j].y < TheOutline[i][beforeP].y))  //�����ͺ�һ�����yֵС��
						{
							minY.push_back(TheOutline[i][j].y);
						}
					}
				}
			}
			maxY.sort();
			minY.sort();
			maxY.unique();    //���������ظ��ĵ�ɾ���ĺ���������STL�ı����㷨��
			minY.unique();	  //ͬ�ϡ�
			float MaxY = *max_element(maxY.begin(), maxY.end());   //�����������������Ԫ�صĺ�����
			float MinY = *min_element(minY.begin(), minY.end());
			//�ڶ���������ÿһ����������������ߵĽ��㣬�ֱ�洢��˫�������С�
			std::vector<std::pair<float, std::list<float>>> Linedate;
			for (int i = 1; i < (MaxY - MinY) / width - 1; i++)
			{
				std::pair<float, std::list<float>> tem;
				tem.first = MinY + width*i;
				Linedate.push_back(tem);
			}
			if (!Linedate.empty())     //��ס��Linedate�����ǿյģ� 2015_6_17
			{
				if ((MaxY - Linedate[Linedate.size() - 1].first) > width * 3 / 2)//Ϊ��ʹ�����һ������߲��������������̫Զ��Ҫ��һ���Ƿ���Ҫ����һ���߶��ж�
				{

					std::pair<float, std::list<float>> tem;
					tem.first = (MaxY + Linedate[Linedate.size() - 1].first) / 2;
					Linedate.push_back(tem);
				}
				else if ((MaxY - Linedate[Linedate.size() - 1].first) <= width * 3 / 2 && (MaxY - Linedate[Linedate.size() - 1].first) > width)  //��ÿһ���ߵļ�඼����һ�������Ӧ�߿���
				{
					for (int i = 0; i != Linedate.size(); ++i)
					{
						Linedate[i].first += (MaxY - Linedate[Linedate.size() - 1].first - width) / Linedate.size();
					}
				}

				for (int i = 0; i != TheOutline.size(); i++)
				{

					for (int j = 1; j != TheOutline[i].size(); j++)  //j=1��˵��Ҫ�ӵڶ����㿪ʼѭ����ֱ����һ���㡣
					{
						int n = (int)TheOutline[i].size() - 1;  //�������ݵ����һ�����ǵ�һ���㣬���Ҫ��ȥ1��
						int beforeP = (j - 1 + n) % n;
						int beforebP = (j - 2 + n) % n;
						int nextP = (j + 1 + n) % n;
						int nextnP = (j + 2 + n) % n;
						if (TheOutline[i][j].y != TheOutline[i][nextP].y) //�����߶β�ƽ����x��������
						{
							for (int k = 0; k != Linedate.size(); k++)
							{
								if ((Linedate[k].first - TheOutline[i][j].y)*(Linedate[k].first - TheOutline[i][nextP].y) < 0) //���������߶��ཻ�������
								{
									float x1 = TheOutline[i][j].x;
									float x2 = TheOutline[i][nextP].x;
									float y1 = TheOutline[i][j].y;
									float y2 = TheOutline[i][nextP].y;
									Linedate[k].second.push_back((x2 - x1) / (y2 - y1)*(Linedate[k].first - y1) + x1);
								}
								else if ((Linedate[k].first == TheOutline[i][j].y))    //���������߶�ǰһ�������ཻ�������
								{
									if ((!IsContainPoint(maxPoint, TheOutline[i][j])) && (!IsContainPoint(minPoint, TheOutline[i][j])) && (TheOutline[i][j].y != TheOutline[i][beforeP].y))
									{
										//�������ͬʱ�ֲ��Ǽ�ֵ��ʱ��ͬʱ�������ǰһ�����yֵ������Ȳ��У���Ϊ���ʱ������Ѿ��Ѿ���������߼����ǹ��ˣ�����
										Linedate[k].second.push_back(TheOutline[i][j].x);
									}
								}
								else if ((Linedate[k].first == TheOutline[i][nextP].y))   //���������߶κ�һ�������ཻ�������
								{
									if ((!IsContainPoint(maxPoint, TheOutline[i][nextP])) && (!IsContainPoint(minPoint, TheOutline[i][nextP])) && (TheOutline[i][nextP].y != TheOutline[i][nextnP].y))
									{
										//�������ͬʱ�ֲ��Ǽ�ֵ��ʱ��ͬʱ��һ��������һ�����yֵ������Ȳ��У���Ϊ���ʱ������Ѿ��Ѿ���������߼����ǹ��ˣ�����
										Linedate[k].second.push_back(TheOutline[i][nextP].x);
									}
								}
							}
						}
						else if (TheOutline[i][j].y == TheOutline[i][nextP].y) //�����߶�ƽ����x��������
						{
							for (int k = 0; k != Linedate.size(); k++)
							{
								if (Linedate[k].first == TheOutline[i][j].y)  //�������ǡ����ƽ����x����������ཻ��
								{
									if ((IsLeft(TheOutline[i][beforeP], TheOutline[i][j], TheOutline[i][nextP]) == -1) && (IsLeft(TheOutline[i][j], TheOutline[i][nextP], TheOutline[i][nextnP]) == -1))  //��������ߵ�����ͬʱ���ڵ�ʱ��
									{   //˳ʱ������ת���ڵ㡣
										Linedate[k].second.push_back(TheOutline[i][j].x);
										Linedate[k].second.push_back(TheOutline[i][nextP].x);
									}
									else if ((IsLeft(TheOutline[i][beforeP], TheOutline[i][j], TheOutline[i][nextP]) == 1) && (IsLeft(TheOutline[i][j], TheOutline[i][nextP], TheOutline[i][nextnP]) == 1))
									{
										//�����㶼�����ڵ㣬���㶼��ȥ�����仰˵����ʲôҲ��������ʵ����д��δ��룬Ϊ���Ժ���������д���ˡ�
									}

									else if ((IsLeft(TheOutline[i][beforeP], TheOutline[i][j], TheOutline[i][nextP]) == -1) && (IsLeft(TheOutline[i][j], TheOutline[i][nextP], TheOutline[i][nextnP]) == 1))
									{
										Linedate[k].second.push_back(TheOutline[i][j].x);
									}
									else if ((IsLeft(TheOutline[i][beforeP], TheOutline[i][j], TheOutline[i][nextP]) == 1) && (IsLeft(TheOutline[i][j], TheOutline[i][nextP], TheOutline[i][nextnP]) == -1))
									{
										Linedate[k].second.push_back(TheOutline[i][nextP].x);
									}
								}
							}
						}
					}

				}

				for (int i = 0; i != Linedate.size(); i++)  //���洢�õ������ظ���Ԫ��ɾ������������
				{
					Linedate[i].second.sort();    //����
					Linedate[i].second.unique();  //������ĵ�ȡ������
				}
				DeleteOddDate(Linedate);    //��ʱ���ϣ����������

				for (int i = 0; i != Linedate.size(); ++i)  //ɾ�������Ļ���������ͺ�����ͬʱ�뾶����Ҳ����ɾ������
				{
					for (std::list<float>::const_iterator j = Linedate[i].second.begin(); j != Linedate[i].second.end(); ++j)
					{
						outline tem;
						tem.push_back(xd::xdpoint(*j + shrinkDistance, Linedate[i].first));
						++j;
						tem.push_back(xd::xdpoint(*j - shrinkDistance, Linedate[i].first));
						if (tem[0].x>tem[1].x)
							continue;
						TheResult.push_back(tem);
					}
				}
			}
		}

		//�ǵ�������ƫ��������
		if (lunkuo != 0)
		{
			for (int i = 0; i != dataOffsets.size(); ++i)
			{
				for (int j = 0; j != dataOffsets[i].size(); ++j)
				{
					TheOutlineResult.insert(TheOutlineResult.begin(), dataOffsets[i][j]);
				}
			}
		}
		//����ߵĽ��ת��ѡ��Ƕ�
		for (int i = 0; i != TheResult.size(); ++i)
		{
			for (int j = 0; j != TheResult[i].size(); ++j)
			{
				TheResult[i][j].argument(TheResult[i][j].argument() + degree);
			}
		}
		for (int i = 0; i != TheOutlineResult.size(); ++i)
		{
			for (int j = 0; j != TheOutlineResult[i].size(); ++j)
			{
				TheOutlineResult[i][j].argument(TheOutlineResult[i][j].argument() + degree);
			}
		}
	}
}

void InfillConcentric(outlines TheOutline, outlines & TheResult, outlines & TheOutlineResult, float width, int lunkuo,  float offsetWidth) //�Լ���д��ͬ����亯��
{
	if (!TheOutline.empty())  //���ȱ��뱣֤������
	{
		//��Ҫһ������ƫ����ʹ���������Ƚ�ƽ��
		std::vector<outlines> dataOffsets;  //�洢����ƫ������������
		if (lunkuo != 0)
		{
			ClipperLib::ClipperOffset temO;
			for (int i = 0; i != TheOutline.size(); ++i)
			{
				ClipperLib::Path temP;
				for (int j = 0; j != TheOutline[i].size(); ++j)
				{
					temP << ClipperLib::IntPoint(TheOutline[i][j].x * 1000000, TheOutline[i][j].y * 1000000);
				}
				temO.AddPath(temP, ClipperLib::jtSquare, ClipperLib::etClosedPolygon);
			}

			for (int temi = 0; temi != lunkuo; ++temi)
			{
				ClipperLib::Paths solution;
				temO.Execute(solution, -offsetWidth * 1000000 * (temi + 1));

				xd::outlines dataOffset;
				for (int i = 0; i != solution.size(); ++i)
				{
					xd::outline temData;
					for (int j = 0; j != solution[i].size(); ++j)
					{
						temData.push_back(xd::xdpoint((float)solution[i][j].X / 1000000.0, (float)solution[i][j].Y / 1000000.0));
					}
					temData.push_back(xd::xdpoint((float)solution[i][0].X / 1000000.0, (float)solution[i][0].Y / 1000000.0));//�������һ���㣬��֤���
					dataOffset.push_back(temData);
				}
				dataOffsets.push_back(dataOffset);
			}
			TheOutline.clear();
			TheOutline = dataOffsets[dataOffsets.size() - 1];
		}
		if (TheOutline.size() != 0)
		{
			InfillOffset(TheOutline, TheResult, width);
		}
		//�ǵ�������ƫ��������
		if (lunkuo != 0)
		{
			for (int i = 0; i != dataOffsets.size(); ++i)
			{
				for (int j = 0; j != dataOffsets[i].size(); ++j)
				{
					TheOutlineResult.insert(TheOutlineResult.begin(), dataOffsets[i][j]);
				}
			}
		}
	}
}

void InfillLineIn(outlines TheOutline,outlines & TheResult,float width,float degree ) //�Լ���д����������ɺ�����
{
    //������������תһ���Ƕ�
    for (int i=0;i!=TheOutline.size();++i)
    {
        for (int j=0;j!=TheOutline[i].size();++j)
        {
            TheOutline[i][j].argument(TheOutline[i][j].argument()-degree);
        }
    }

    //��һ����������ľֲ�����Сֵ�㣬����һ��˫�������С�ע�⣺ѡ��˫�������ԭ����������λ��ɾ�������Ԫ�طǳ���ݣ�����ʱ�临�Ӷȡ�
    std::list<float> maxY;
    std::list<float> minY;
    std::vector<std::pair<float,float>> maxPoint;
    std::vector<std::pair<float,float>> minPoint;
    for (int i=0;i!=TheOutline.size();i++)
    {
        for (int j=1;j!=TheOutline[i].size();j++)
        {
            int n=(int)TheOutline[i].size()-1;  //�������ݵ����һ�����ǵ�һ���㣬���Ҫ��ȥ1��
            int beforeP=(j-1+n)%n;
            int beforebP=(j-2+n)%n;
            int nextP=(j+1+n)%n;
            int nextnP=(j+2+n)%n;
            if ((TheOutline[i][j].y>TheOutline[i][beforeP].y)&&(TheOutline[i][j].y>TheOutline[i][nextP].y))  //������yֵ����ǰ��ĵ��yֵ��
            {
                maxY.push_back(TheOutline[i][j].y);
                std::pair<float,float> tem;
                tem.first=TheOutline[i][j].x;
                tem.second=TheOutline[i][j].y;
                maxPoint.push_back(tem);
            }
            else if ((TheOutline[i][j].y<TheOutline[i][beforeP].y)&&(TheOutline[i][j].y<TheOutline[i][nextP].y)) //������yֵС��ǰ��ĵ��yֵ��
            {
                minY.push_back(TheOutline[i][j].y);
                std::pair<float,float> tem;
                tem.first=TheOutline[i][j].x;
                tem.second=TheOutline[i][j].y;
                minPoint.push_back(tem);
            }
            else if (TheOutline[i][j].y==TheOutline[i][beforeP].y)
            {
                if ((TheOutline[i][beforeP].y>TheOutline[i][beforebP].y)&&(TheOutline[i][j].y>TheOutline[i][nextP].y)) //������ǰһ�����yֵ��
                {
                    maxY.push_back(TheOutline[i][j].y);
                }
                if ((TheOutline[i][beforeP].y<TheOutline[i][beforebP].y)&&(TheOutline[i][j].y<TheOutline[i][nextP].y)) //������ǰһ�����yֵС��
                {
                    minY.push_back(TheOutline[i][j].y);
                }
            }
            else if (TheOutline[i][j].y==TheOutline[i][nextP].y)
            {
                if ((TheOutline[i][nextP].y>TheOutline[i][nextnP].y)&&(TheOutline[i][j].y>TheOutline[i][beforeP].y))  //�����ͺ�һ�����yֵ��
                {
                    maxY.push_back(TheOutline[i][j].y);
                }
                if ((TheOutline[i][nextP].y<TheOutline[i][nextnP].y)&&(TheOutline[i][j].y<TheOutline[i][beforeP].y))  //�����ͺ�һ�����yֵС��
                {
                    minY.push_back(TheOutline[i][j].y);
                }
            }
        }
    }
    maxY.sort();
    minY.sort();
    maxY.unique();    //���������ظ��ĵ�ɾ���ĺ���������STL�ı����㷨��
    minY.unique();	  //ͬ�ϡ�
    float MaxY=*max_element(maxY.begin(),maxY.end());   //�����������������Ԫ�صĺ�����
    float MinY=*min_element(minY.begin(),minY.end());
    //�ڶ���������ÿһ����������������ߵĽ��㣬�ֱ�洢��˫�������С�
    std::vector<std::pair<float,std::list<float>>> Linedate;
    for (int i=1;i<(MaxY-MinY)/width-1;i++)
    {
        std::pair<float,std::list<float>> tem;
        tem.first=MinY+width*i;
        Linedate.push_back(tem);
    }
	if(!Linedate.empty())       //2015_6_17   ��ס��Linedate�����ǿյģ���
	{
		if ((MaxY-Linedate[Linedate.size()-1].first)>width*3/2)//Ϊ��ʹ�����һ������߲��������������̫Զ��Ҫ��һ���Ƿ���Ҫ����һ���߶��ж�
		{

			std::pair<float,std::list<float>> tem;
			tem.first=(MaxY+Linedate[Linedate.size()-1].first)/2;
			Linedate.push_back(tem);
		}
		else if ((MaxY-Linedate[Linedate.size()-1].first)<=width*3/2&&(MaxY-Linedate[Linedate.size()-1].first)>width)  //��ÿһ���ߵļ�඼����һ�������Ӧ�߿���
		{
			for (int i=0;i!=Linedate.size();++i)
			{
				Linedate[i].first+=(MaxY-Linedate[Linedate.size()-1].first-width)/Linedate.size();
			}
		}

		for(int i=0;i!=TheOutline.size();i++)
		{

			for (int j=1;j!=TheOutline[i].size();j++)  //j=1��˵��Ҫ�ӵڶ����㿪ʼѭ����ֱ����һ���㡣
			{
				int n=(int)TheOutline[i].size()-1;  //�������ݵ����һ�����ǵ�һ���㣬���Ҫ��ȥ1��
				int beforeP=(j-1+n)%n;
				int beforebP=(j-2+n)%n;
				int nextP=(j+1+n)%n;
				int nextnP=(j+2+n)%n;
				if (TheOutline[i][j].y!=TheOutline[i][nextP].y) //�����߶β�ƽ����x��������
				{
					for (int k=0;k!=Linedate.size();k++)
					{
						if ((Linedate[k].first-TheOutline[i][j].y)*(Linedate[k].first-TheOutline[i][nextP].y)<0) //���������߶��ཻ�������
						{
							float x1=TheOutline[i][j].x;
							float x2=TheOutline[i][nextP].x;
							float y1=TheOutline[i][j].y;
							float y2=TheOutline[i][nextP].y;
							Linedate[k].second.push_back((x2-x1)/(y2-y1)*(Linedate[k].first-y1)+x1);
						}
						else if ((Linedate[k].first==TheOutline[i][j].y))    //���������߶�ǰһ�������ཻ�������
						{
							if ((!IsContainPoint(maxPoint,TheOutline[i][j]))&&(!IsContainPoint(minPoint,TheOutline[i][j]))&&(TheOutline[i][j].y!=TheOutline[i][beforeP].y))
							{
								//�������ͬʱ�ֲ��Ǽ�ֵ��ʱ��ͬʱ�������ǰһ�����yֵ������Ȳ��У���Ϊ���ʱ������Ѿ��Ѿ���������߼����ǹ��ˣ�����
								Linedate[k].second.push_back(TheOutline[i][j].x);
							}
						}
						else if ((Linedate[k].first==TheOutline[i][nextP].y))   //���������߶κ�һ�������ཻ�������
						{
							if ((!IsContainPoint(maxPoint,TheOutline[i][nextP]))&&(!IsContainPoint(minPoint,TheOutline[i][nextP]))&&(TheOutline[i][nextP].y!=TheOutline[i][nextnP].y))
							{
								//�������ͬʱ�ֲ��Ǽ�ֵ��ʱ��ͬʱ��һ��������һ�����yֵ������Ȳ��У���Ϊ���ʱ������Ѿ��Ѿ���������߼����ǹ��ˣ�����
								Linedate[k].second.push_back(TheOutline[i][nextP].x);
							}
						}
					}
				}
				else if(TheOutline[i][j].y==TheOutline[i][nextP].y) //�����߶�ƽ����x��������
				{
					for (int k=0;k!=Linedate.size();k++)
					{
						if (Linedate[k].first==TheOutline[i][j].y)  //�������ǡ����ƽ����x����������ཻ��
						{
							if((IsLeft(TheOutline[i][beforeP],TheOutline[i][j],TheOutline[i][nextP])==-1)&&(IsLeft(TheOutline[i][j],TheOutline[i][nextP],TheOutline[i][nextnP])==-1))  //��������ߵ�����ͬʱ���ڵ�ʱ��
							{   //˳ʱ������ת���ڵ㡣
								Linedate[k].second.push_back(TheOutline[i][j].x);
								Linedate[k].second.push_back(TheOutline[i][nextP].x);
							}
							else if((IsLeft(TheOutline[i][beforeP],TheOutline[i][j],TheOutline[i][nextP])==1)&&(IsLeft(TheOutline[i][j],TheOutline[i][nextP],TheOutline[i][nextnP])==1))
							{
								//�����㶼�����ڵ㣬���㶼��ȥ�����仰˵����ʲôҲ��������ʵ����д��δ��룬Ϊ���Ժ���������д���ˡ�
							}

							else if((IsLeft(TheOutline[i][beforeP],TheOutline[i][j],TheOutline[i][nextP])==-1)&&(IsLeft(TheOutline[i][j],TheOutline[i][nextP],TheOutline[i][nextnP])==1))
							{
								Linedate[k].second.push_back(TheOutline[i][j].x);
							}
							else if((IsLeft(TheOutline[i][beforeP],TheOutline[i][j],TheOutline[i][nextP])==1)&&(IsLeft(TheOutline[i][j],TheOutline[i][nextP],TheOutline[i][nextnP])==-1))
							{
								Linedate[k].second.push_back(TheOutline[i][nextP].x);
							}
						}
					}
				}
			}

		}

		for(int i=0;i!=Linedate.size();i++)  //���洢�õ������ظ���Ԫ��ɾ������������
		{
			Linedate[i].second.sort();    //����
			Linedate[i].second.unique();  //������ĵ�ȡ������
		}
		//	DeleteOddDate(Linedate);    //��ʱ���ϣ����������

		//�������������ݷ������š�
		int FirstNonZero;
		while(!IsEmpty(Linedate,FirstNonZero))
		{
			outline tem;
			int j=0;  //�����ж��������л���ż���еĲ�����
			auto firstIn=Linedate[FirstNonZero].second.begin();
			float bijiao1=*firstIn;   //��һ���ߵĵ�һ���㡣
			tem.push_back(xdpoint(*firstIn,Linedate[FirstNonZero].first,TheOutline[0][0].z));
			tem.push_back(xdpoint(*(++firstIn),Linedate[FirstNonZero].first,TheOutline[0][0].z));
			int i=(FirstNonZero+1)%Linedate.size();  //���뱣֤����һ���������ݵ��߸պ������һ����ʱҲ�����1Խ�磡����Ҫģһ�£�
			float bijiao2=*firstIn;  //��һ���ߵĵڶ����㡣
			float bijiaoY1=Linedate[FirstNonZero].first;  //��һ���ߵ�Y����ֵ��
			auto tem1s=Linedate[FirstNonZero].second.begin();
			auto tem1e=tem1s;
			tem1e++;
			tem1e++;
			Linedate[FirstNonZero].second.erase(tem1s,tem1e);
			while ((!Linedate[i].second.empty()))
			{
				auto Line2first=Linedate[i].second.begin();  //�ڶ����ߵĵ�һ���㡣

				float bijiaoY2=Linedate[i].first; //�ڶ�����ֱ�ߵ�Y����ֵ��
				if (exceedExtremum(bijiaoY1,bijiaoY2,maxY,minY))
				{
					//ǰ������ֱ�������Խ�˾ֲ���ֵ�㣬Ҳ��Ҫ�˳�������Ҫ������
					break;
				}
				bijiaoY1=bijiaoY2;

				if(*Line2first>bijiao2)
				{
					//�ڶ���ֱ�ߵĵ�һ������ڵ�һ��ֱ�ߵĵڶ��������˳�������Ҫ����
					break;
				}
				if (*(++Line2first)<bijiao1)   //ע�⣺����++������ǰ����Ϊ���������ţ�++�ں�Ļ�Ҳ��Ƚ�����++������
				{
					//�ڶ���ֱ�ߵĵڶ�����С�ڵ�һ��ֱ�ߵĵ�һ�������˳�������Ҫ����
					break;
				}
				if (j%2==0)
				{
					auto tem2s=Linedate[i].second.begin();
					auto tem2e=tem2s;
					tem2e++;
					tem.push_back(xdpoint(*tem2e,Linedate[i].first,TheOutline[0][0].z));
					tem.push_back(xdpoint(*tem2s,Linedate[i].first,TheOutline[0][0].z));
				}
				else
				{
					auto tem2s=Linedate[i].second.begin();
					auto tem2e=tem2s;
					tem2e++;
					tem.push_back(xdpoint(*tem2s,Linedate[i].first,TheOutline[0][0].z));
					tem.push_back(xdpoint(*tem2e,Linedate[i].first,TheOutline[0][0].z));
				}
				auto tem3s=Linedate[i].second.begin();
				auto tem3e=tem3s;
				bijiao1=*tem3e;
				tem3e++;
				bijiao2=*tem3e;
				tem3e++;
				Linedate[i].second.erase(tem3s,tem3e);
				++j;
				++i;
				if (i==Linedate.size())
				{
					break;
				}
			}
			TheResult.push_back(tem);
		}
	}
   

    //����һ����Ϊ����ӦFDM�ĸ�ʽ��ÿ���߶�Ҫ����һ���뾶����,��������������Ӳ����뾶��
    //for (int i=0;i!=TheResult.size();++i)
    //{
    //	for (int j=0;j!=TheResult[i].size();++j)
    //	{

    //		if (j%4==3||j%4==0)
    //		{
    //			TheResult[i][j].x+=0.28;    //�˴���Ĭ��ֵ��

    //		}
    //		else
    //		{
    //			TheResult[i][j].x-=0.28;
    //		}
    //	}
    //}

    //����ߵĽ��ת��ѡ��Ƕ�
    for (int i=0;i!=TheResult.size();++i)
    {
        for (int j=0;j!=TheResult[i].size();++j)
        {
            TheResult[i][j].argument(TheResult[i][j].argument()+degree);
        }
    }
}

void InfillBMP(outlines TheOutline,std::vector<std::vector<int>> & TheResult,int piex,int size ) //�Լ���д��bmp���ɺ�����
{
    float amplificationFactor=(float)piex/size;  //����Ŵ�����140�Ǽӹ����޳ߴ硣ע�⣬һ��Ҫ��ʾ��Ϊfloat������ó�����һ��������
    //��һ�����Ŵ����ݡ�
    for (int i=0;i!=TheOutline.size();++i)
    {
        for (int j=0;j!=TheOutline[i].size();++j)
        {
            TheOutline[i][j].x*=amplificationFactor;
            TheOutline[i][j].y*=amplificationFactor;
        }
    }
    //�ڶ�����ȡ�������������ġ�
    xd::xdpoint limitPositionA(TheOutline[0][0].x,TheOutline[0][0].y),limitPositionB(TheOutline[0][0].x,TheOutline[0][0].y);
    for (int i=0;i!=TheOutline.size();++i)
    {
        for (int j=0;j!=TheOutline[i].size();++j)
        {
            if (limitPositionA.x<TheOutline[i][j].x)
            {
                limitPositionA.x=TheOutline[i][j].x;
            }
            if (limitPositionA.y<TheOutline[i][j].y)
            {
                limitPositionA.y=TheOutline[i][j].y;
            }
            if (limitPositionB.x>TheOutline[i][j].x)
            {
                limitPositionB.x=TheOutline[i][j].x;
            }
            if (limitPositionB.y>TheOutline[i][j].y)
            {
                limitPositionB.y=TheOutline[i][j].y;
            }
        }
    }
    float centroidX=(limitPositionA.x+limitPositionB.x)/2;
    float centroidY=(limitPositionA.y+limitPositionB.y)/2;
    float offsetX=piex/2-centroidX;
    float offsetY=piex/2-centroidY;
    //��������ƽ�����ݡ�
    for (int i=0;i!=TheOutline.size();++i)
    {
        for (int j=0;j!=TheOutline[i].size();++j)
        {
            TheOutline[i][j].x+=offsetX;
            TheOutline[i][j].y+=offsetY;
        }
    }
    //���Ĳ���������ľֲ�����Сֵ�㣬����һ��˫�������С�ע�⣺ѡ��˫�������ԭ����������λ��ɾ�������Ԫ�طǳ���ݣ�����ʱ�临�Ӷȡ�
    std::list<float> maxY;
    std::list<float> minY;
    std::vector<std::pair<float,float>> maxPoint;
    std::vector<std::pair<float,float>> minPoint;
    for (int i=0;i!=TheOutline.size();i++)
    {
        for (int j=1;j!=TheOutline[i].size();j++)
        {
            int n=(int)TheOutline[i].size()-1;  //�������ݵ����һ�����ǵ�һ���㣬���Ҫ��ȥ1��
            int beforeP=(j-1+n)%n;
            int beforebP=(j-2+n)%n;
            int nextP=(j+1+n)%n;
            int nextnP=(j+2+n)%n;
            if ((TheOutline[i][j].y>TheOutline[i][beforeP].y)&&(TheOutline[i][j].y>TheOutline[i][nextP].y))  //������yֵ����ǰ��ĵ��yֵ��
            {
                maxY.push_back(TheOutline[i][j].y);
                std::pair<float,float> tem;
                tem.first=TheOutline[i][j].x;
                tem.second=TheOutline[i][j].y;
                maxPoint.push_back(tem);
            }
            else if ((TheOutline[i][j].y<TheOutline[i][beforeP].y)&&(TheOutline[i][j].y<TheOutline[i][nextP].y)) //������yֵС��ǰ��ĵ��yֵ��
            {
                minY.push_back(TheOutline[i][j].y);
                std::pair<float,float> tem;
                tem.first=TheOutline[i][j].x;
                tem.second=TheOutline[i][j].y;
                minPoint.push_back(tem);
            }
            else if (TheOutline[i][j].y==TheOutline[i][beforeP].y)
            {
                if ((TheOutline[i][beforeP].y>TheOutline[i][beforebP].y)&&(TheOutline[i][j].y>TheOutline[i][nextP].y)) //������ǰһ�����yֵ��
                {
                    maxY.push_back(TheOutline[i][j].y);
                }
                if ((TheOutline[i][beforeP].y<TheOutline[i][beforebP].y)&&(TheOutline[i][j].y<TheOutline[i][nextP].y)) //������ǰһ�����yֵС��
                {
                    minY.push_back(TheOutline[i][j].y);
                }
            }
            else if (TheOutline[i][j].y==TheOutline[i][nextP].y)
            {
                if ((TheOutline[i][nextP].y>TheOutline[i][nextnP].y)&&(TheOutline[i][j].y>TheOutline[i][beforeP].y))  //�����ͺ�һ�����yֵ��
                {
                    maxY.push_back(TheOutline[i][j].y);
                }
                if ((TheOutline[i][nextP].y<TheOutline[i][nextnP].y)&&(TheOutline[i][j].y<TheOutline[i][beforeP].y))  //�����ͺ�һ�����yֵС��
                {
                    minY.push_back(TheOutline[i][j].y);
                }
            }
        }
    }
    maxY.sort();
    minY.sort();
    maxY.unique();    //���������ظ��ĵ�ɾ���ĺ���������STL�ı����㷨��
    minY.unique();	  //ͬ�ϡ�
    float MaxY=*max_element(maxY.begin(),maxY.end());   //�����������������Ԫ�صĺ�����
    float MinY=*min_element(minY.begin(),minY.end());
    //���岽������ÿһ������������������ߵĽ��㣬�ֱ�洢��˫�������С�
    std::vector<std::pair<float,std::list<float>>> Linedate;
    for (int i=1;i<=piex;i++)
    {
        std::pair<float,std::list<float>> tem;
        tem.first=(i+i-1)/2;    //������������ص��е���Ϊɨ���ߵ�yֵ
        Linedate.push_back(tem);
    }

    for (int i=0;i!=TheOutline.size();i++)
    {
        for (int j=1;j!=TheOutline[i].size();j++)  //j=1��˵��Ҫ�ӵڶ����㿪ʼѭ����ֱ����һ���㡣
        {
            int n=(int)TheOutline[i].size()-1;  //�������ݵ����һ�����ǵ�һ���㣬���Ҫ��ȥ1��
            int beforeP=(j-1+n)%n;
            int beforebP=(j-2+n)%n;
            int nextP=(j+1+n)%n;
            int nextnP=(j+2+n)%n;
            if (TheOutline[i][j].y!=TheOutline[i][nextP].y) //�����߶β�ƽ����x��������
            {
                for (int k=0;k!=Linedate.size();k++)
                {
                    if ((Linedate[k].first-TheOutline[i][j].y)*(Linedate[k].first-TheOutline[i][nextP].y)<0) //���������߶��ཻ�������
                    {
                        float x1=TheOutline[i][j].x;
                        float x2=TheOutline[i][nextP].x;
                        float y1=TheOutline[i][j].y;
                        float y2=TheOutline[i][nextP].y;
                        Linedate[k].second.push_back((x2-x1)/(y2-y1)*(Linedate[k].first-y1)+x1);
                    }
                    else if ((Linedate[k].first==TheOutline[i][j].y))    //���������߶�ǰһ�������ཻ�������
                    {
                        if ((!IsContainPoint(maxPoint,TheOutline[i][j]))&&(!IsContainPoint(minPoint,TheOutline[i][j]))&&(TheOutline[i][j].y!=TheOutline[i][beforeP].y))
                        {
                            //�������ͬʱ�ֲ��Ǽ�ֵ��ʱ��ͬʱ�������ǰһ�����yֵ������Ȳ��У���Ϊ���ʱ������Ѿ��Ѿ���������߼����ǹ��ˣ�����
                            Linedate[k].second.push_back(TheOutline[i][j].x);
                        }
                    }
                    else if ((Linedate[k].first==TheOutline[i][nextP].y))   //���������߶κ�һ�������ཻ�������
                    {
                        if ((!IsContainPoint(maxPoint,TheOutline[i][nextP]))&&(!IsContainPoint(minPoint,TheOutline[i][nextP]))&&(TheOutline[i][nextP].y!=TheOutline[i][nextnP].y))
                        {
                            //�������ͬʱ�ֲ��Ǽ�ֵ��ʱ��ͬʱ��һ��������һ�����yֵ������Ȳ��У���Ϊ���ʱ������Ѿ��Ѿ���������߼����ǹ��ˣ�����
                            Linedate[k].second.push_back(TheOutline[i][nextP].x);
                        }
                    }
                }
            }
            else if(TheOutline[i][j].y==TheOutline[i][nextP].y) //�����߶�ƽ����x��������
            {
                for (int k=0;k!=Linedate.size();k++)
                {
                    if (Linedate[k].first==TheOutline[i][j].y)  //�������ǡ����ƽ����x����������ཻ��
                    {
                        if((IsLeft(TheOutline[i][beforeP],TheOutline[i][j],TheOutline[i][nextP])==-1)&&(IsLeft(TheOutline[i][j],TheOutline[i][nextP],TheOutline[i][nextnP])==-1))  //��������ߵ�����ͬʱ���ڵ�ʱ��
                        {   //˳ʱ������ת���ڵ㡣
                            Linedate[k].second.push_back(TheOutline[i][j].x);
                            Linedate[k].second.push_back(TheOutline[i][nextP].x);
                        }
                        else if((IsLeft(TheOutline[i][beforeP],TheOutline[i][j],TheOutline[i][nextP])==1)&&(IsLeft(TheOutline[i][j],TheOutline[i][nextP],TheOutline[i][nextnP])==1))
                        {
                            //�����㶼�����ڵ㣬���㶼��ȥ�����仰˵����ʲôҲ��������ʵ����д��δ��룬Ϊ���Ժ���������д���ˡ�
                        }

                        else if((IsLeft(TheOutline[i][beforeP],TheOutline[i][j],TheOutline[i][nextP])==-1)&&(IsLeft(TheOutline[i][j],TheOutline[i][nextP],TheOutline[i][nextnP])==1))
                        {
                            Linedate[k].second.push_back(TheOutline[i][j].x);
                        }
                        else if((IsLeft(TheOutline[i][beforeP],TheOutline[i][j],TheOutline[i][nextP])==1)&&(IsLeft(TheOutline[i][j],TheOutline[i][nextP],TheOutline[i][nextnP])==-1))
                        {
                            Linedate[k].second.push_back(TheOutline[i][nextP].x);
                        }
                    }
                }
            }
        }

    }

    for(int i=0;i!=Linedate.size();i++)  //���洢�õ������ظ���Ԫ��ɾ������������
    {
        Linedate[i].second.sort();    //����
        Linedate[i].second.unique();  //������ĵ�ȡ������
    }
    //	DeleteOddDate(Linedate);    //��ʱ���ϣ����������
    //����������������Ҫ�������к���ȡ�������ȥ��
    for (int i=0;i!=piex;++i)   //�Ƚ����س�ʼ��Ϊ0
    {
        std::vector<int> temData;
        for (int j=0;j!=piex;++j)
        {
            temData.push_back(0);
        }
        TheResult.push_back(temData);
    }
    for (int i=0;i!=piex;++i)
    {
        if (Linedate[i].second.size()==0)
        {
            continue;
        }
        else if(Linedate[i].second.size()==1)
        {
            auto a=Linedate[i].second.begin();
            TheResult[i][*a]=1;
        }
        else
        {
            while(!Linedate[i].second.empty())
            {
                auto a=Linedate[i].second.begin();
                auto b=a;
                b++;
                //std::cout<<*b<<"    OK";
                for(int j=*a;j<=*b;++j)
                {
                    //std::cout<<j<<"   OK";
                    TheResult[i][j]=1;
                }
                b++;
                Linedate[i].second.erase(a,b);
            }
        }
    }
}

void InfillOffset(outlines theOutline,outlines & theResult,float width)  //����ƫ�����ĺ�����  δ��֤
{
	if (!theOutline.empty())
	{
		ClipperLib::ClipperOffset temO;
		for (int i=0;i!=theOutline.size();++i)
		{
			ClipperLib::Path temP;
			for (int j=0;j!=theOutline[i].size();++j)
			{
				temP<<ClipperLib::IntPoint(theOutline[i][j].x*1000000,theOutline[i][j].y*1000000);
			}
			temO.AddPath(temP,ClipperLib::jtMiter, ClipperLib::etClosedPolygon);
		}
		int temi=1; //ÿ��ƫ�ö�һ�εĺ���
		bool flag=true; //ƫ���Ƿ�����ı�־
		while(flag)
		{
			ClipperLib::Paths solution; 		
			temO.Execute(solution, -width*1000000*temi); 
			temi+=1;
			xd::outlines dataOffset;  //�����洢��ƫ�ú����������
			for (int i=0;i!=solution.size();++i)
			{
				xd::outline temData;
				for (int j=0;j!=solution[i].size();++j)
				{
					temData.push_back(xd::xdpoint((float)solution[i][j].X/1000000.0,(float)solution[i][j].Y/1000000.0));
				}
				temData.push_back(xd::xdpoint((float)solution[i][0].X/1000000.0,(float)solution[i][0].Y/1000000.0));//�������һ���㣬��֤���
				dataOffset.push_back(temData);
				theResult.push_back(temData);
			}			
			if (dataOffset.empty())
			{
				flag=false;  //���ƫ������û���ˣ���˵������Ҫ�ٴ�ƫ���ˡ�
			}		   
		}	
	}				  	
}

void InfillOffsetIn(ClipperLib::Paths theOutline,outlines & theResult,float width)  //����ƫ�����ĺ�����  δ��֤
{
	if (!theOutline.empty())
	{
		ClipperLib::ClipperOffset temO;
		ClipperLib::CleanPolygons(theOutline,1.415);    //ƫ��ǰ��ü�����仰
		ClipperLib::SimplifyPolygons(theOutline,ClipperLib::pftEvenOdd);   //����֣�ƫ��ǰ��ñ�֤�Ǽ򵥶���Σ����������Զѭ����
		temO.AddPaths(theOutline,ClipperLib::jtMiter, ClipperLib::etClosedPolygon);
		int temi=1; //ÿ��ƫ�ö�һ�εı�־
		bool flag=true; //ƫ���Ƿ�����ı�־
		while(flag)
		{
			ClipperLib::Paths solution; 		
			temO.Execute(solution, -width*1000000*temi); 
			temi+=1;
			xd::outlines dataOffset;  //�����洢��ƫ�ú����������
			for (int i=0;i!=solution.size();++i)
			{
				xd::outline temData;
				for (int j=0;j!=solution[i].size();++j)
				{
					temData.push_back(xd::xdpoint((float)solution[i][j].X/1000000.0,(float)solution[i][j].Y/1000000.0));
				}
				temData.push_back(xd::xdpoint((float)solution[i][0].X/1000000.0,(float)solution[i][0].Y/1000000.0));//�������һ���㣬��֤���
				dataOffset.push_back(temData);
				theResult.push_back(temData);
			}			
			if (dataOffset.empty())
			{
				flag=false;  //���ƫ������û���ˣ���˵������Ҫ�ٴ�ƫ���ˡ�
			}		   
		}	
	}				  	
}

void PickUpLayer(std::vector<xd::outlines> theOutline,std::vector<int> & theResult)
{	
	//����Ҫ����ÿ������������бȽ�
	for (int i=0;i!=theOutline.size()-1;++i)
	{	
		ClipperLib::Paths subject;
		ClipperLib::Paths clip;
		for (int j=0;j!=theOutline[i].size();++j)
		{
			ClipperLib::Path temP;
			for (int k=0;k!=theOutline[i][j].size();++k)
			{
				temP<<ClipperLib::IntPoint(theOutline[i][j][k].x*1000000,theOutline[i][j][k].y*1000000);
			}
			subject.push_back(temP);
		}
		for (int j=0;j!=theOutline[i+1].size();++j)
		{
			ClipperLib::Path temP;
			for (int k=0;k!=theOutline[i+1][j].size();++k)
			{
				temP<<ClipperLib::IntPoint(theOutline[i+1][j][k].x*1000000,theOutline[i+1][j][k].y*1000000);
			}
			clip.push_back(temP);
		}
		ClipperLib::Clipper clipper(ClipperLib::ioStrictlySimple | ClipperLib::ioPreserveCollinear); 
		clipper.AddPaths(subject,ClipperLib::ptSubject,true);
		clipper.AddPaths(clip,ClipperLib::ptClip,true);
		ClipperLib::Paths solution; //���·�����
		clipper.Execute(ClipperLib::ctXor,solution,ClipperLib::pftEvenOdd,ClipperLib::pftEvenOdd);
		double areaSum=0;
		for (int i=0;i!=solution.size();++i)
		{
			areaSum+=ClipperLib::Area(solution[i]);
		}
		if (areaSum>100*1000000000000)   //100ƽ������
		{
			theResult.push_back(i+1);
		}
	}
}

bool IsSingleInfill(int layerNumber,std::vector<int> needInfillLayer)  //�жϸ������Ƿ��ǵ������ĺ���
{
	if (needInfillLayer.empty())
	{
		return false;
	} 
	else
	{
		for (int i=0;i!=needInfillLayer.size();++i)
		{
			if (layerNumber==needInfillLayer[i])
			{
				return true;
			} 			
		}
		return false;
	}
}
void OutlinesOffsetMethod(std::vector<xd::outlines> theOutline,std::vector<xd::outputOutlines> &result,float width,float shrinkDistance)
{
	unsigned int N=theOutline.size();    //һ����N�����һ�Ρ�
	//��һ�����Ƚ������ݴ��浽clipperר�õ�paths��	
	std::vector<ClipperLib::Paths> nPaths;
	for (int i=0;i!=N;++i)
	{
		ClipperLib::Paths temPaths;
		for (int j=0;j!=theOutline[i].size();++j)
		{
			ClipperLib::Path temPath;
			for (int k=0;k!=theOutline[i][j].size();++k)
			{
				temPath<<ClipperLib::IntPoint(theOutline[i][j][k].x*1000000,theOutline[i][j][k].y*1000000);
			}
			temPaths.push_back(temPath);
		}
		nPaths.push_back(temPaths);
	}
	//�ڶ�������N�����Ľ�������
	ClipperLib::Paths areaA; 
	ClipperLib::Clipper clipper(ClipperLib::ioStrictlySimple | ClipperLib::ioPreserveCollinear);
	clipper.AddPaths(nPaths[0],ClipperLib::ptSubject,true);
	clipper.AddPaths(nPaths[1],ClipperLib::ptClip,true);
	clipper.Execute(ClipperLib::ctIntersection, areaA,ClipperLib::pftEvenOdd,ClipperLib::pftEvenOdd); 
	clipper.Clear();
	if (N>2)
	{
		for (int i=2;i!=N;++i)
		{
			clipper.AddPaths(nPaths[i],ClipperLib::ptSubject,true);
			clipper.AddPaths(areaA,ClipperLib::ptClip,true);
			areaA.clear();
			clipper.Execute(ClipperLib::ctIntersection, areaA,ClipperLib::pftEvenOdd,ClipperLib::pftEvenOdd); 
			clipper.Clear();
		}
	}
	//����������N�������ȺͲ������������֮���N��ƫ��һ�κ�Ͳ��������
	std::vector<ClipperLib::Paths> offsetResult;   //�������ƫ�ý��
	ClipperLib::Paths xorResult;                   //�������Ľ��
	clipper.AddPaths(nPaths[N-1],ClipperLib::ptSubject,true);
	clipper.AddPaths(areaA,ClipperLib::ptClip,true);
	clipper.Execute(ClipperLib::ctXor, xorResult,ClipperLib::pftEvenOdd,ClipperLib::pftEvenOdd);
	clipper.Clear();
	double xorAreaFormer=0;  //���ǰһ����������
	double xorAreaLater=0;   //��ź�һ����������
	for (int i=0;i!=xorResult.size();++i)
	{
		xorAreaFormer+=ClipperLib::Area(xorResult[i]);
	}
	ClipperLib::ClipperOffset clipperOffset;     //����ƫ�õ���
	ClipperLib::Paths onceOffset;     //���ÿһ��ƫ�õĽ��
	clipperOffset.AddPaths(nPaths[N-1], ClipperLib::jtMiter, ClipperLib::etClosedPolygon);  //ʹ����򻯵�ƫ��
	clipperOffset.Execute(onceOffset,-width);
	clipperOffset.Clear();
	clipper.AddPaths(onceOffset,ClipperLib::ptSubject,true);
	clipper.AddPaths(areaA,ClipperLib::ptClip,true);
	xorResult.clear();
	clipper.Execute(ClipperLib::ctXor, xorResult,ClipperLib::pftEvenOdd,ClipperLib::pftEvenOdd);
	clipper.Clear();
	for (int i=0;i!=xorResult.size();++i)
	{
		xorAreaLater+=ClipperLib::Area(xorResult[i]);
	}
	//���Ĳ������йؼ���ѭ�����֣�����ȷ����Ҫƫ�ö��ٴΡ�
	while(xorAreaLater<xorAreaFormer)
	{
		offsetResult.push_back(onceOffset);   //�ȴ洢ƫ��·����Ϊ�����
		xorAreaFormer=xorAreaLater;     //ǰһ�ε������ɺ�һ�ε����
		xorAreaLater=0;					//��һ�ε����Ҫ���������
		clipperOffset.AddPaths(onceOffset, ClipperLib::jtMiter, ClipperLib::etClosedPolygon);  //ʹ���ϴ�ƫ��·����Ϊ�������·����
		onceOffset.clear();             //����ɾ��ƫ��·����
		clipperOffset.Execute(onceOffset,-width);
		clipperOffset.Clear();
		clipper.AddPaths(onceOffset,ClipperLib::ptSubject,true);  //ƫ�úͲ��������
		clipper.AddPaths(areaA,ClipperLib::ptClip,true);
		xorResult.clear();
		clipper.Execute(ClipperLib::ctXor, xorResult,ClipperLib::pftEvenOdd,ClipperLib::pftEvenOdd);
		clipper.Clear();
		for (int i=0;i!=xorResult.size();++i)
		{
			xorAreaLater+=ClipperLib::Area(xorResult[i]);
		}
	}
	//���岽�����һ�ε�ƫ��û����䣬���Ҫʹ�ô洢ƫ�õ����һ������������Ϊ���������򡣣���ת��Ϊ��ͨ·�����ǵü������һ���㣩
	if (!offsetResult.empty())
	{
		for (int i=0;i!=offsetResult.size();++i)
		{
			xd::outputOutlines temOutputOutlines;
			for (int j=0;j!=offsetResult[i].size();++j)
			{
				xd::outline temOutline;
				for (int k=0;k!=offsetResult[i][j].size();++k)
				{
					temOutline.push_back(xd::xdpoint(offsetResult[i][j][k].X/1000000.0,offsetResult[i][j][k].Y/1000000.0));
				}
				temOutline.push_back(xd::xdpoint(offsetResult[i][j][0].X/1000000.0,offsetResult[i][j][0].Y/1000000.0));  //�������һ����
				std::pair<outline,unsigned int> temPair;
				temPair.first=temOutline;
				temPair.second=1;
				temOutputOutlines.push_back(temPair);
			}
			result.push_back(temOutputOutlines);
		}
	}
	//�������������Ҫ������������·��
	if (!result.empty())
	{
		if (!result[result.size()-1].empty())
		{
			xd::outlines temOutlines;
			for (int i=0;i!=result[result.size()-1].size();++i)
			{				
				temOutlines.push_back(result[result.size()-1][i].first);
			}
			xd::outlines temResult;
			xd::InfillLine(temOutlines, temResult, width, 0, 0, shrinkDistance,width);
			xd::outputOutlines temOutputOutlines;
			for(int i=0;i!=temResult.size();++i)
			{
				std::pair<outline,unsigned int> temPair;
				temPair.first=temResult[i];
				temPair.second=N;
				temOutputOutlines.push_back(temPair);
			}
			result.push_back(temOutputOutlines);
		}
	} 
	else
	{
		xd::outlines temResult;
		xd::InfillLine(theOutline[N - 1], temResult, width, 0, 0, shrinkDistance,width);
		xd::outputOutlines temOutputOutlines;
		for(int i=0;i!=temResult.size();++i)
		{
			std::pair<outline,unsigned int> temPair;
			temPair.first=temResult[i];
			temPair.second=N;
			temOutputOutlines.push_back(temPair);
		}
		result.push_back(temOutputOutlines);
		//������ݽṹ�����⣬��Ҫʹ�õ������ݣ���������Ҳ�кô����ֿ�������ƫ������ֱ����䡣
	}
}

void OutlinesClipperMethod(std::vector<xd::outlines> theOutline,std::vector<xd::outputOutlines> &result,float width,float shrinkDistance,float degree)
{
	unsigned int N=theOutline.size();    //һ����N�����һ�Ρ�
	if(1==N)   //�������1�㣬��ֻ��ֱ��ȫ������ˡ� ��û����ⲿ�ֳ���
	{
		return; 
		assert(1!=N);
	}
	//��һ�����Ƚ������ݴ��浽clipperר�õ�paths��	
	std::vector<ClipperLib::Paths> nPaths;
	for (int i=0;i!=N;++i)
	{
		ClipperLib::Paths temPaths;
		for (int j=0;j!=theOutline[i].size();++j)
		{
			ClipperLib::Path temPath;
			for (int k=0;k!=theOutline[i][j].size();++k)
			{
				temPath<<ClipperLib::IntPoint(theOutline[i][j][k].x*1000000,theOutline[i][j][k].y*1000000);
			}
			temPaths.push_back(temPath);
		}
		nPaths.push_back(temPaths);
	}
	//�ڶ�������N�����Ľ�������
	ClipperLib::Paths areaA;   //���彻������A
	ClipperLib::Clipper clipper(ClipperLib::ioStrictlySimple | ClipperLib::ioPreserveCollinear);
	clipper.AddPaths(nPaths[0],ClipperLib::ptSubject,true);
	clipper.AddPaths(nPaths[1],ClipperLib::ptClip,true);
	clipper.Execute(ClipperLib::ctIntersection, areaA,ClipperLib::pftEvenOdd,ClipperLib::pftEvenOdd); 
	clipper.Clear();
	if (N>2)
	{
		for (int i=2;i!=N;++i)
		{
			clipper.AddPaths(nPaths[i],ClipperLib::ptSubject,true);
			clipper.AddPaths(areaA,ClipperLib::ptClip,true);
			areaA.clear();
			clipper.Execute(ClipperLib::ctIntersection, areaA,ClipperLib::pftEvenOdd,ClipperLib::pftEvenOdd); 
			clipper.Clear();
		}
	}
	//��������ÿһ�㣨�������һ�㣩�Ͳ����󲼶������㣬���ʹ������ƫ����䷨�����ڶ�Ӧ������·�����档
	for (int i=0;i!=N-1;++i)
	{
		ClipperLib::Paths offsetOutline;
		clipper.AddPaths(nPaths[i],ClipperLib::ptSubject,true);
		clipper.AddPaths(areaA,ClipperLib::ptClip,true);
		clipper.Execute(ClipperLib::ctDifference, offsetOutline,ClipperLib::pftEvenOdd,ClipperLib::pftEvenOdd);
		xd::outlines offsetResult;
		xd::InfillOffsetIn(offsetOutline,offsetResult,width);
		xd::outputOutlines temOutputOutlines;   //���һ��Ĳü����ƫ������������
		for (int j=0;j!=offsetResult.size();++j)
		{
			std::pair<outline,unsigned int> temPair;
			temPair.first=offsetResult[j];
			temPair.second=1;
			temOutputOutlines.push_back(temPair);
		}
		result.push_back(temOutputOutlines);
		clipper.Clear();
	}
	//���Ĳ����������һ���������ݣ������������Ͷ����䡣
	ClipperLib::Paths offsetOutline;
	clipper.AddPaths(nPaths[N-1],ClipperLib::ptSubject,true);
	clipper.AddPaths(areaA,ClipperLib::ptClip,true);
	clipper.Execute(ClipperLib::ctDifference, offsetOutline,ClipperLib::pftEvenOdd,ClipperLib::pftEvenOdd);
	xd::outlines offsetResult;
	xd::InfillOffsetIn(offsetOutline,offsetResult,width);
	xd::outputOutlines temOutputOutlines;   //���һ��Ĳü����ƫ������������
	for (int j=0;j!=offsetResult.size();++j)
	{
		std::pair<outline,unsigned int> temPair;
		temPair.first=offsetResult[j];
		temPair.second=1;
		temOutputOutlines.push_back(temPair);
	}
	//result.push_back(temOutputOutlines);
	clipper.Clear();
	for (int i=0;i!=areaA.size();++i) //��Ҫʹ�ö������������A������·��
	{
		std::pair<outline,unsigned int> temPair;
		xd::outline temOutline;
		for(int j=0;j!=areaA[i].size();++j)
		{
			temOutline.push_back(xd::xdpoint(areaA[i][j].X/1000000.0,areaA[i][j].Y/1000000.0));
		}
		temOutline.push_back(xd::xdpoint(areaA[i][0].X/1000000.0,areaA[i][0].Y/1000000.0));   //�������һ���㣬��֤���
		temPair.first=temOutline;
		temPair.second=N;
		temOutputOutlines.push_back(temPair);
	}
	xd::outlines outlinesIn,outlinesO;
	for (int i=0;i!=areaA.size();++i)  //��Ҫʹ�ö������������A�����·��  ��Ҫ�ȷֿ�������䣡
	{
		xd::outline temPush;
		for (int j=0;j!=areaA[i].size();++j)
		{
			temPush.push_back(xd::xdpoint(areaA[i][j].X/1000000.0,areaA[i][j].Y/1000000.0));
		}
		temPush.push_back(xd::xdpoint(areaA[i][0].X/1000000.0,areaA[i][0].Y/1000000.0));  //�������һ���㣬��֤���
		outlinesIn.push_back(temPush);
	}
	xd::InfillLine(outlinesIn,outlinesO,width,degree,0,shrinkDistance,width);
	for (int i=0;i!=outlinesO.size();++i)
	{
		std::pair<outline,unsigned int> temPair;
		temPair.first=outlinesO[i];
		temPair.second=N;
		temOutputOutlines.push_back(temPair);
	}
	result.push_back(temOutputOutlines);   //������·�������
}

void SplitMNArea(outlines TheOutline,std::vector<xd::outlines> & theResult,std::vector<outlines> & dataOffsets,float width,int M,int N,int lunkuo,float overlap)
{
	if(!TheOutline.empty())  //���ȱ��뱣֤������
	{
		//��Ҫһ������ƫ����ʹ���������Ƚ�ƽ��
		//std::vector<outlines> dataOffsets;  //�洢����ƫ������������
		if(lunkuo!=0)
		{
			ClipperLib::ClipperOffset temO;
			for (int i=0;i!=TheOutline.size();++i)
			{
				ClipperLib::Path temP;
				for (int j=0;j!=TheOutline[i].size();++j)
				{
					temP<<ClipperLib::IntPoint(TheOutline[i][j].x*1000000,TheOutline[i][j].y*1000000);
				}
				temO.AddPath(temP,ClipperLib::jtSquare, ClipperLib::etClosedPolygon);
			}
		
			for(int temi=0;temi!=lunkuo;++temi)
			{
				ClipperLib::Paths solution; 		
				temO.Execute(solution, -width*1000000*(temi+1)); 

				xd::outlines dataOffset;
				for (int i=0;i!=solution.size();++i)
				{
					xd::outline temData;
					for (int j=0;j!=solution[i].size();++j)
					{
						temData.push_back(xd::xdpoint((float)solution[i][j].X/1000000.0,(float)solution[i][j].Y/1000000.0));
					}
					temData.push_back(xd::xdpoint((float)solution[i][0].X/1000000.0,(float)solution[i][0].Y/1000000.0));//�������һ���㣬��֤���
					dataOffset.push_back(temData);
				}
				dataOffsets.push_back(dataOffset);
			}		
			TheOutline.clear();
			TheOutline=dataOffsets[dataOffsets.size()-1];        
		}
		if(TheOutline.size()!=0)     
		{			
			ClipperLib::Clipper splitArea;
			ClipperLib::Paths subjectPaths;
			for(int i=0;i!=TheOutline.size();++i)
			{
				ClipperLib::Path temPath;
				for(int j=0;j!=TheOutline[i].size();++j)
				{
					temPath<<ClipperLib::IntPoint(TheOutline[i][j].x*1000000,TheOutline[i][j].y*1000000);
				}
				subjectPaths.push_back(temPath);
			}
			splitArea.AddPaths(subjectPaths,ClipperLib::ptSubject,true);
			ClipperLib::IntRect AABB=splitArea.GetBounds();			
			if(((AABB.bottom-AABB.top)<10*1000000)||(AABB.right-AABB.left<10*1000000))  //������Χ�г����С��10mm���򲻻����ˣ��˴������޸�
			{
				theResult.push_back(TheOutline);
				return;
			}
			ClipperLib::cInt lineUnit=(AABB.bottom-AABB.top)/(ClipperLib::cInt)M;  //�е�Ԫ�Ĵ�С��ע�⣺clipper��bottom>top! y�����£�
			ClipperLib::cInt rowUnit=(AABB.right-AABB.left)/(ClipperLib::cInt)N;   //�е�Ԫ�Ĵ�С
			for(int i=0;i!=M;++i)	
			{
				for(int j=0;j!=N;++j)
				{
					//��ʼ������Ҫ�߼�����������ȡ������һ������İ�Χ�е�λ������
					ClipperLib::cInt Left=AABB.left+(ClipperLib::cInt)j*rowUnit;
					ClipperLib::cInt Top=AABB.bottom-(ClipperLib::cInt)i*lineUnit;   //top��Ϊbottom!  y�����£�
					ClipperLib::cInt Right=AABB.left+(ClipperLib::cInt)(j+1)*rowUnit;
					ClipperLib::cInt Bottom=AABB.bottom-(ClipperLib::cInt)(i+1)*lineUnit;  //top��Ϊbottom!
					//������Ҫע���ص�
					if(i!=0)
						Top+=overlap*1000000;
					if(j!=0)
						Left-=overlap*1000000;
					if(i!=M-1)
						Bottom-=overlap*1000000;
					if(j!=N-1)
						Right+=overlap*1000000;
					//���湹��ü�С����
					ClipperLib::Path clipRectangle;
					clipRectangle<<ClipperLib::IntPoint(Left,Top)<<ClipperLib::IntPoint(Left,Bottom)<<ClipperLib::IntPoint(Right,Bottom)<<ClipperLib::IntPoint(Right,Top)<<ClipperLib::IntPoint(Left,Top);
					splitArea.AddPath(clipRectangle,ClipperLib::ptClip,true);					
					ClipperLib::Paths intersectionResult;
					splitArea.Execute(ClipperLib::ctIntersection, intersectionResult,ClipperLib::pftEvenOdd,ClipperLib::pftEvenOdd);  //ִ�в�����					
					xd::outlines temOutlines;    					
					for(int k=0;k!=intersectionResult.size();++k)
					{
						xd::outline temOutline;
						for(int l=0;l!=intersectionResult[k].size();++l)
						{
							temOutline.push_back(xd::xdpoint(intersectionResult[k][l].X/1000000.0,intersectionResult[k][l].Y/1000000.0));
						}
						temOutline.push_back(xd::xdpoint(intersectionResult[k][0].X/1000000.0,intersectionResult[k][0].Y/1000000.0)); //�������һ������
						temOutlines.push_back(temOutline);
					}
					theResult.push_back(temOutlines);
					splitArea.Clear();  //����һ��Ҫ���
					splitArea.AddPaths(subjectPaths,ClipperLib::ptSubject,true);  //Ҫ�ټ����������
				}
			}
			//����ѭ��M*N�Σ�������ϴ�ŵ�theResult��
		}
		
	}

}

int SplitLWArea(outlines TheOutline, std::vector<xd::outlines> & theResult, std::vector<outlines> & dataOffsets, float width, int Length, int Width, int lunkuo, float overlap, float threshold)
{
	if(!TheOutline.empty())  //���ȱ��뱣֤������
	{
		//��Ҫһ������ƫ����ʹ���������Ƚ�ƽ��
		//std::vector<outlines> dataOffsets;  //�洢����ƫ������������
		if(lunkuo!=0)
		{
			ClipperLib::ClipperOffset temO;
			for (int i=0;i!=TheOutline.size();++i)
			{
				ClipperLib::Path temP;
				for (int j=0;j!=TheOutline[i].size();++j)
				{
					temP<<ClipperLib::IntPoint(TheOutline[i][j].x*1000000,TheOutline[i][j].y*1000000);
				}
				temO.AddPath(temP,ClipperLib::jtSquare, ClipperLib::etClosedPolygon);
			}
		
			for(int temi=0;temi!=lunkuo;++temi)
			{
				ClipperLib::Paths solution; 		
				temO.Execute(solution, -width*1000000*(temi+1)); 

				xd::outlines dataOffset;
				for (int i=0;i!=solution.size();++i)
				{
					xd::outline temData;
					for (int j=0;j!=solution[i].size();++j)
					{
						temData.push_back(xd::xdpoint((float)solution[i][j].X/1000000.0,(float)solution[i][j].Y/1000000.0));
					}
					temData.push_back(xd::xdpoint((float)solution[i][0].X/1000000.0,(float)solution[i][0].Y/1000000.0));//�������һ���㣬��֤���
					dataOffset.push_back(temData);
				}
				dataOffsets.push_back(dataOffset);
			}		
			TheOutline.clear();
			TheOutline=dataOffsets[dataOffsets.size()-1];        
		}
		if(TheOutline.size()!=0)     
		{			
			ClipperLib::Clipper splitArea;
			ClipperLib::Paths subjectPaths;
			for(int i=0;i!=TheOutline.size();++i)
			{
				ClipperLib::Path temPath;
				for(int j=0;j!=TheOutline[i].size();++j)
				{
					temPath<<ClipperLib::IntPoint(TheOutline[i][j].x*1000000,TheOutline[i][j].y*1000000);
				}
				subjectPaths.push_back(temPath);
			}
			splitArea.AddPaths(subjectPaths,ClipperLib::ptSubject,true);
			ClipperLib::IntRect AABB=splitArea.GetBounds();			
			//if(((AABB.bottom-AABB.top)<10*1000000)||((AABB.right-AABB.left)<10*1000000))  //������Χ�г����С��10mm���򲻻����ˣ��˴������޸�
			//{
			//	theResult.push_back(TheOutline);
			//	return;
			//}

			ClipperLib::cInt lineUnit=Width*1000000;  //�е�Ԫ�Ĵ�С��ע�⣺clipper��bottom>top! y�����£�
			ClipperLib::cInt rowUnit=Length*1000000;   //�е�Ԫ�Ĵ�С
			int M=(AABB.bottom-AABB.top)/lineUnit;
			int N=(AABB.right-AABB.left)/rowUnit;
			//�������ʣ��������ڸ�������һ�룬���ٷ�һ�Σ������Ժ���Ը�����Ҫ�޸ľ���
			ClipperLib::cInt widthRemainder=(AABB.bottom-AABB.top)%lineUnit;
			ClipperLib::cInt lengthRemainder=(AABB.right-AABB.left)%rowUnit;
			/*if(widthRemainder>lineUnit/2||M==0)
				M+=1;
			if(lengthRemainder>rowUnit/2||N==0)
				N+=1;*/
			if (widthRemainder>threshold*1000000 || M == 0)
				M += 1;
			if (lengthRemainder>threshold*1000000 || N == 0)
				N += 1;
			for(int i=0;i!=M;++i)	
			{
				for(int j=0;j!=N;++j)
				{
					//��ʼ������Ҫ�߼�����������ȡ������һ������İ�Χ�е�λ������
					ClipperLib::cInt Left=AABB.left+(ClipperLib::cInt)j*rowUnit;
					ClipperLib::cInt Top=AABB.bottom-(ClipperLib::cInt)i*lineUnit;   //top��Ϊbottom!  y�����£�
					ClipperLib::cInt Right=AABB.left+(ClipperLib::cInt)(j+1)*rowUnit;
					ClipperLib::cInt Bottom=AABB.bottom-(ClipperLib::cInt)(i+1)*lineUnit;  //top��Ϊbottom!
					//������Ҫע���ص�
					if(i!=0)
						Top+=overlap*1000000;
					if(j!=0)
						Left-=overlap*1000000;
					if(i!=M-1)
						Bottom-=overlap*1000000;
					else
						Bottom=AABB.top;
					if(j!=N-1)
						Right+=overlap*1000000;
					else
						Right=AABB.right;
					//���湹��ü�С����
					ClipperLib::Path clipRectangle;
					clipRectangle<<ClipperLib::IntPoint(Left,Top)<<ClipperLib::IntPoint(Left,Bottom)<<ClipperLib::IntPoint(Right,Bottom)<<ClipperLib::IntPoint(Right,Top)<<ClipperLib::IntPoint(Left,Top);
					splitArea.AddPath(clipRectangle,ClipperLib::ptClip,true);					
					ClipperLib::Paths intersectionResult;
					splitArea.Execute(ClipperLib::ctIntersection, intersectionResult,ClipperLib::pftEvenOdd,ClipperLib::pftEvenOdd);  //ִ�в�����					
					xd::outlines temOutlines;    					
					for(int k=0;k!=intersectionResult.size();++k)
					{
						xd::outline temOutline;
						for(int l=0;l!=intersectionResult[k].size();++l)
						{
							temOutline.push_back(xd::xdpoint(intersectionResult[k][l].X/1000000.0,intersectionResult[k][l].Y/1000000.0));
						}
						temOutline.push_back(xd::xdpoint(intersectionResult[k][0].X/1000000.0,intersectionResult[k][0].Y/1000000.0)); //�������һ������
						temOutlines.push_back(temOutline);
					}
					theResult.push_back(temOutlines);
					splitArea.Clear();  //����һ��Ҫ���
					splitArea.AddPaths(subjectPaths,ClipperLib::ptSubject,true);  //Ҫ�ټ����������
				}
			}
			//����ѭ��M*N�Σ�������ϴ�ŵ�theResult��
			return M;
		}
		return 0;
	}
	return 0;
}

void offsetReturnSingleRegion(outlines TheOutline,std::vector<xd::outlines> & theResult,std::vector<outlines> & dataOffsets,float width,int lunkuo)
{
	if(!TheOutline.empty())  //���ȱ��뱣֤������
	{
		//��Ҫһ������ƫ����ʹ���������Ƚ�ƽ��
		//std::vector<outlines> dataOffsets;  //�洢����ƫ������������
		if(lunkuo!=0)
		{
			ClipperLib::ClipperOffset temO;
			for (int i=0;i!=TheOutline.size();++i)
			{
				ClipperLib::Path temP;
				for (int j=0;j!=TheOutline[i].size();++j)
				{
					temP<<ClipperLib::IntPoint(TheOutline[i][j].x*1000000,TheOutline[i][j].y*1000000);
				}
				temO.AddPath(temP,ClipperLib::jtSquare, ClipperLib::etClosedPolygon);
			}
		
			for(int temi=0;temi!=lunkuo;++temi)
			{
				ClipperLib::Paths solution; 		
				temO.Execute(solution, -width*1000000*(temi+1)); 

				xd::outlines dataOffset;
				for (int i=0;i!=solution.size();++i)
				{
					xd::outline temData;
					for (int j=0;j!=solution[i].size();++j)
					{
						temData.push_back(xd::xdpoint((float)solution[i][j].X/1000000.0,(float)solution[i][j].Y/1000000.0));
					}
					temData.push_back(xd::xdpoint((float)solution[i][0].X/1000000.0,(float)solution[i][0].Y/1000000.0));//�������һ���㣬��֤���
					dataOffset.push_back(temData);
				}
				dataOffsets.push_back(dataOffset);
			}		
			TheOutline.clear();
			TheOutline=dataOffsets[dataOffsets.size()-1];        
		}
		if(!TheOutline.empty())
		{
			ClipperLib::Clipper clipper;
			clipper.Clear();    
			//�����÷�ֵ��ѧϰ����clipper���paths��Ϊpolytree�ķ���
			ClipperLib::Paths input;
			OutlinesToClipperPaths(TheOutline,&input);
			clipper.AddPaths(input, ClipperLib::ptSubject, true);
			ClipperLib::PolyTree polytree;
			clipper.Execute(ClipperLib::ctUnion, polytree, ClipperLib::pftEvenOdd, ClipperLib::pftEvenOdd);  // offset results work with both EvenOdd and NonZero
			//��ʼ��������������ֱ�ֵ�����
			theResult.clear();
			for(int i=0;i!=polytree.ChildCount();++i)
				AddOuterPolyNodeToResult(*polytree.Childs[i], theResult);   //ʹ�õݹ麯�������������ʹ�õ���ǰ�������
		}
	}
}

void offsetReturnSingleRegion(outlines TheOutline,std::vector<outlines> & theResult)
{	
	if(!TheOutline.empty()) //���ȱ�֤������
	{
		ClipperLib::Clipper clipper;
		clipper.Clear();    
		//�����÷�ֵ��ѧϰ����clipper���paths��Ϊpolytree�ķ���
		ClipperLib::Paths input;
		OutlinesToClipperPaths(TheOutline,&input);
		clipper.AddPaths(input, ClipperLib::ptSubject, true);
		ClipperLib::PolyTree polytree;
		clipper.Execute(ClipperLib::ctUnion, polytree, ClipperLib::pftEvenOdd, ClipperLib::pftEvenOdd);  // offset results work with both EvenOdd and NonZero
		//��ʼ��������������ֱ�ֵ�����
		theResult.clear();
		for(int i=0;i!=polytree.ChildCount();++i)
			AddOuterPolyNodeToResult(*polytree.Childs[i], theResult);   //ʹ�õݹ麯�������������ʹ�õ���ǰ�������
	}		
}

void AddOuterPolyNodeToResult(ClipperLib::PolyNode& polynode,std::vector<xd::outlines> & output)   
{	
	int lastSubscript=output.size();
	output.resize(lastSubscript + 1);  //��һ��resize���ں������һ���������������
	output[lastSubscript].resize(1+polynode.ChildCount());   //��һ������ʹ�ô��������һ���м�����������
	ClipperPathToOutline(polynode.Contour, & output[lastSubscript][0]);  //������϶���ε�����	
	if(output[lastSubscript][0][0]!=output[lastSubscript][0][output[lastSubscript][0].size()-1])  //����Ϊ��ʹ�ö�������һ������ڵ�һ����
		output[lastSubscript][0].push_back(output[lastSubscript][0][0]);
	for (int i = 0; i < polynode.ChildCount(); ++i)
	{
		ClipperPathToOutline(polynode.Childs[i]->Contour, & output[lastSubscript][1+i]);  //������϶���ο��ܳ��ֵ�n����
		if(output[lastSubscript][1+i][0]!=output[lastSubscript][1+i][output[lastSubscript][1+i].size()-1]) //����Ϊ��ʹ�ö�������һ������ڵ�һ����
			output[lastSubscript][1+i].push_back(output[lastSubscript][1+i][0]);
		for (int j = 0; j < polynode.Childs[i]->ChildCount(); ++j)   //�м����ӣ��ͻ��м����������������
			AddOuterPolyNodeToResult(*polynode.Childs[i]->Childs[j], output);  //��һ��ʹ�õݹ�
	}
}

}
