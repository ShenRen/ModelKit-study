//2015_8_13 王晓东 今天在直线填充算法中加上了填充距离太短则舍去的算法，至此直线填充算法基本完善！！！！还缺一点是过拐角点连续会跑出集合区域！
#include "generate_line.h"
#define NDEBUG  
#include <assert.h>   

namespace xd
{

float degrees(float fudu) //幅度转化为角度的函数
{
    return(fudu/pi*180);
}
float radians(float jiaodu) //角度转换为幅度的函数
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
	//output->resize(input.size()+1);   晕，后面用push_back，这里千万别用resize！！
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

bool IsContainPoint(std::vector<std::pair<float,float> > &a, xdpoint b)  //判读极值点列表里是否包含某一个点的函数。
{
    //注意，list可能为为空！
    for (int i=0;i!=a.size();i++)
    {
        if ((a[i].first==b.x)&&(a[i].second==b.y))
        {
            return true;
        }
    }
    return false;
}

int IsLeft(xdpoint p1,xdpoint p2, xdpoint p) //判断p点是否在有向线段p1,p2左侧的函数。
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
        return(1); //p在左侧。
    }
    else if(det<0)
    {
        return(-1); //p在右侧。
    }
    else
    {
        return(0); //p与p1,p2共线。
    }
}
bool IsEmpty(std::vector<std::pair<float,std::list<float> > > tem,int & firstV)  //判断扫描线中的元素是否全部取完的函数。
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

void DeleteOddDate(std::vector<std::pair<float,std::list<float> > > & tem)   //将存放奇数点数据的扫描线删除的函数，正常情况下用不着此函数。
{                                                           //因为正常情况下就不可能存在奇数点的数据。
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

bool IsDealFinish(const std::vector<xdpoint> & l,int & i)  //判断平行于x轴的直线上是否有多于两个点的函数。
{
    int n=(int)l.size();          //这里的n的取值千万不能减去1，因为起点和尾点已经未必相等了！
    for (i=0;i!=l.size();i++)     //注意，这里必须不经过最后一个点，因为最后一个点和第一个点必须相等，要减去2？
    {                               //想想为什么？呵呵！出现了奇怪的现像！！在此处循环要全部遍历！
        int benshen=i%n;
        int nextP=(i+1)%n;
        int nextnP=(i+2)%n;
		float d1=sqrt(pow(l[benshen].x-l[nextP].x,2)+pow(l[benshen].y-l[nextP].y,2));  //第一个向量的长度
		float d2=sqrt(pow(l[nextnP].x-l[nextP].x,2)+pow(l[nextnP].y-l[nextP].y,2));  //第二个向量的长度
		float cross=std::abs(((l[nextP].y-l[benshen].y)*(l[nextnP].x-l[nextP].x)-(l[nextnP].y-l[nextP].y)*(l[nextP].x-l[benshen].x))); //向量叉乘的绝对值
        if (cross/(d1*d2)<sin(1.0/180*pi))   //相差1度就去除，记得用1.0
        {
            i=nextP; //要删除的元素的角标。
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
	float d1 = sqrt(pow(benshen->x - nextP->x, 2) + pow(benshen->y - nextP->y, 2));  //第一个向量的长度
	float d2 = sqrt(pow(nextnP->x - nextP->x, 2) + pow(nextnP->y - nextP->y, 2));  //第二个向量的长度
	float cross = std::abs(((nextP->y - benshen->y)*(nextnP->x - nextP->x) - (nextnP->y - nextP->y)*(nextP->x - benshen->x))); //向量叉乘的绝对值
	while (l.size()>3)
	{
        if (cross / (d1*d2)<sin(3.0 / 180 * pi))   //相差1度就去除，记得用1.0
		{
			l.erase(nextP);
			nextP = benshen + 1;
			if (nextP == l.end())
			{
				nextP = l.begin();
				nextnP = nextP + 1;
				d1 = sqrt(pow(benshen->x - nextP->x, 2) + pow(benshen->y - nextP->y, 2));  //第一个向量的长度
				d2 = sqrt(pow(nextnP->x - nextP->x, 2) + pow(nextnP->y - nextP->y, 2));  //第二个向量的长度
				cross = std::abs(((nextP->y - benshen->y)*(nextnP->x - nextP->x) - (nextnP->y - nextP->y)*(nextP->x - benshen->x))); //向量叉乘的绝对值
				continue;
			}
			nextnP = nextP + 1;
			if (nextnP == l.end())
			{
				nextnP = l.begin();
				d1 = sqrt(pow(benshen->x - nextP->x, 2) + pow(benshen->y - nextP->y, 2));  //第一个向量的长度
				d2 = sqrt(pow(nextnP->x - nextP->x, 2) + pow(nextnP->y - nextP->y, 2));  //第二个向量的长度
				cross = std::abs(((nextP->y - benshen->y)*(nextnP->x - nextP->x) - (nextnP->y - nextP->y)*(nextP->x - benshen->x))); //向量叉乘的绝对值
				continue;
			}
			d1 = sqrt(pow(benshen->x - nextP->x, 2) + pow(benshen->y - nextP->y, 2));  //第一个向量的长度
			d2 = sqrt(pow(nextnP->x - nextP->x, 2) + pow(nextnP->y - nextP->y, 2));  //第二个向量的长度
			cross = std::abs(((nextP->y - benshen->y)*(nextnP->x - nextP->x) - (nextnP->y - nextP->y)*(nextP->x - benshen->x))); //向量叉乘的绝对值
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
				d1 = sqrt(pow(benshen->x - nextP->x, 2) + pow(benshen->y - nextP->y, 2));  //第一个向量的长度
				d2 = sqrt(pow(nextnP->x - nextP->x, 2) + pow(nextnP->y - nextP->y, 2));  //第二个向量的长度
				cross = std::abs(((nextP->y - benshen->y)*(nextnP->x - nextP->x) - (nextnP->y - nextP->y)*(nextP->x - benshen->x))); //向量叉乘的绝对值
				continue;
			}
			nextnP += 1;
			if (nextnP == l.end())
			{
				nextnP = l.begin();
				d1 = sqrt(pow(benshen->x - nextP->x, 2) + pow(benshen->y - nextP->y, 2));  //第一个向量的长度
				d2 = sqrt(pow(nextnP->x - nextP->x, 2) + pow(nextnP->y - nextP->y, 2));  //第二个向量的长度
				cross = std::abs(((nextP->y - benshen->y)*(nextnP->x - nextP->x) - (nextnP->y - nextP->y)*(nextP->x - benshen->x))); //向量叉乘的绝对值
				continue;
			}
		}
	}
}

bool exceedExtremum(float bijiaoY1,float bijiaoY2,std::list<float> maxY,std::list<float> minY)  //用来判断前后两条直线是否越过了局部极值点的函数
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



void DealCompensate(outline DealData,outline & ResultData,float R) //处理轮廓点数据，使其变为有补偿半径的轮廓点数据。 融入到自己后期程序中，2015/3/10 可能有错
{
    float layerZ=DealData[0].z;
    auto subscript1=DealData.size()-2;  //轮廓数据最后一个点与第一个点重复！！时刻记着！！！
    auto subscript2=0;
    auto subscript3=1;
    CTwoDimensionVector L1(DealData[subscript2].x-DealData[subscript1].x,DealData[subscript2].y-DealData[subscript1].y);
    CTwoDimensionVector L2(DealData[subscript3].x-DealData[subscript2].x,DealData[subscript3].y-DealData[subscript2].y);

  /*  if ((L1.CrossProduct(L2)<1)&&(L1.DotProduct(L2)>=-1))
    {
        ResultData.push_back(xdpoint((L2.x-L1.x)*R/L1.CrossProduct(L2)+DealData[subscript2].x,(L2.y-L1.y)*R/L1.CrossProduct(L2)+DealData[subscript2].y,layerZ));

    }
    else if((L1.CrossProduct(L2)>-5)) //这里主要是可能出现非常平行的情况，影响判断！！！
    {
        ResultData.push_back(xdpoint((L2.x-L1.x)*R/L1.CrossProduct(L2)+DealData[subscript2].x,(L2.y-L1.y)*R/L1.CrossProduct(L2)+DealData[subscript2].y,layerZ));
    }
    else if (L1.CrossProduct(L2)==0)
    {

    }
    else //if((L1.CrossProduct(L2)<0)&&(L1.DotProduct(L2)<0))
    {
        ResultData.push_back(xdpoint((L1.x-L1.y)*R+DealData[subscript2].x,(L1.x+L1.y)*R+DealData[subscript2].y,layerZ));
       ResultData.push_back(xdpoint(-(L2.x+L2.y)*R+DealData[subscript2].x,(L2.x-L2.y)*R+DealData[subscript2].y,layerZ));  //出现两个点的时候累积一定不要弄反了！
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
        else if((L1.CrossProduct(L2)>-5))   //这里主要是可能出现非常平行的情况，影响判断！！！
        {
            ResultData.push_back(xdpoint((L2.x-L1.x)*R/L1.CrossProduct(L2)+DealData[subscript2].x,(L2.y-L1.y)*R/L1.CrossProduct(L2)+DealData[subscript2].y,layerZ));
        }
        else if (L1.CrossProduct(L2)==0)
        {
             //这种情况不可能发生，因为前后共线的可能性已经被删除了！
        }
        else //if((L1.CrossProduct(L2)<0)&&(L1.DotProduct(L2)<0))
        {
           ResultData.push_back(xdpoint((L1.x-L1.y)*R+DealData[subscript2].x,(L1.x+L1.y)*R+DealData[subscript2].y,layerZ));
           ResultData.push_back(xdpoint(-(L2.x+L2.y)*R+DealData[subscript2].x,(L2.x-L2.y)*R+DealData[subscript2].y,layerZ));  //出现两个点的时候累积一定不要弄反了！

        } */

         ResultData.push_back(xdpoint((L2.x-L1.x)*R/L1.CrossProduct(L2)+DealData[subscript2].x,(L2.y-L1.y)*R/L1.CrossProduct(L2)+DealData[subscript2].y,layerZ));
    }
    ResultData.push_back(xdpoint(ResultData[0].x,ResultData[0].y,layerZ));  //保证最后一个点和第一个点相同。
}

void InfillLine(outlines TheOutline, outlines & TheResult, float width, float degree, int lunkuo,  float shrinkDistance,float offsetWidth) //自己编写的填充线生成函数。
{
	if(!TheOutline.empty())  //首先必须保证有数据
	{
		//将轮廓数据旋转一个角度
		for (int i=0;i!=TheOutline.size();++i)
		{
			for (int j=0;j!=TheOutline[i].size();++j)
			{
				TheOutline[i][j].argument(TheOutline[i][j].argument()-degree);
			}
		}

		//需要一次轮廓偏置来使得外轮廓比较平滑
		std::vector<outlines> dataOffsets;  //存储所有偏置轮廓的数据
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
					temData.push_back(xd::xdpoint((float)solution[i][0].X/1000000.0,(float)solution[i][0].Y/1000000.0));//加上最后一个点，保证封闭
					dataOffset.push_back(temData);
				}
				dataOffsets.push_back(dataOffset);
			}		
			TheOutline.clear();
			TheOutline=dataOffsets[dataOffsets.size()-1];        
		}
		if(TheOutline.size()!=0)     
		{
			//第一步，轮廓点的局部极大极小值点，放在一个双向链表中。注意：选择双向链表的原因是其任意位置删除和添加元素非常快捷，降低时间复杂度。
			std::list<float> maxY;
			std::list<float> minY;
			std::vector<std::pair<float,float>> maxPoint;
			std::vector<std::pair<float,float>> minPoint;
			for (int i=0;i!=TheOutline.size();i++)
			{
				for (int j=1;j!=TheOutline[i].size();j++)
				{
					int n=(int)TheOutline[i].size()-1;  //轮廓数据的最后一个点是第一个点，因此要减去1。
					int beforeP=(j-1+n)%n;
					int beforebP=(j-2+n)%n;
					int nextP=(j+1+n)%n;
					int nextnP=(j+2+n)%n;
					if ((TheOutline[i][j].y>TheOutline[i][beforeP].y)&&(TheOutline[i][j].y>TheOutline[i][nextP].y))  //这个点的y值大于前后的点的y值。
					{
						maxY.push_back(TheOutline[i][j].y);
						std::pair<float,float> tem;
						tem.first=TheOutline[i][j].x;
						tem.second=TheOutline[i][j].y;
						maxPoint.push_back(tem);
					}
					else if ((TheOutline[i][j].y<TheOutline[i][beforeP].y)&&(TheOutline[i][j].y<TheOutline[i][nextP].y)) //这个点的y值小于前后的点的y值。
					{
						minY.push_back(TheOutline[i][j].y);
						std::pair<float,float> tem;
						tem.first=TheOutline[i][j].x;
						tem.second=TheOutline[i][j].y;
						minPoint.push_back(tem);
					}
					else if (TheOutline[i][j].y==TheOutline[i][beforeP].y)
					{
						if ((TheOutline[i][beforeP].y>TheOutline[i][beforebP].y)&&(TheOutline[i][j].y>TheOutline[i][nextP].y)) //这个点和前一个点的y值大。
						{
							maxY.push_back(TheOutline[i][j].y);
						}
						if ((TheOutline[i][beforeP].y<TheOutline[i][beforebP].y)&&(TheOutline[i][j].y<TheOutline[i][nextP].y)) //这个点和前一个点的y值小。
						{
							minY.push_back(TheOutline[i][j].y);
						}
					}
					else if (TheOutline[i][j].y==TheOutline[i][nextP].y)
					{
						if ((TheOutline[i][nextP].y>TheOutline[i][nextnP].y)&&(TheOutline[i][j].y>TheOutline[i][beforeP].y))  //这个点和后一个点的y值大。
						{
							maxY.push_back(TheOutline[i][j].y);
						}
						if ((TheOutline[i][nextP].y<TheOutline[i][nextnP].y)&&(TheOutline[i][j].y<TheOutline[i][beforeP].y))  //这个点和后一个点的y值小。
						{
							minY.push_back(TheOutline[i][j].y);
						}
					}
				}
			}
			maxY.sort();
			minY.sort();
			maxY.unique();    //将链表中重复的点删除的函数，属于STL的变易算法。
			minY.unique();	  //同上。
			float MaxY=*max_element(maxY.begin(),maxY.end());   //调用求链表中求最大元素的函数。
			float MinY=*min_element(minY.begin(),minY.end());
			//第二步，生成每一条线填充线与轮廓线的交点，分别存储在双向链表中。
			std::vector<std::pair<float,std::list<float>>> Linedate;
			for (int i=1;i<(MaxY-MinY)/width-1;i++)
			{
				std::pair<float,std::list<float>> tem;
				tem.first=MinY+width*i;
				Linedate.push_back(tem);
			}
			if(!Linedate.empty())     //记住，Linedate可能是空的！ 2015_6_17
			{
				if ((MaxY-Linedate[Linedate.size()-1].first)>width*3/2)//为了使得最后一根填充线不至于与轮廓离得太远，要加一个是否需要增加一条线段判断
				{

					std::pair<float,std::list<float>> tem;
					tem.first=(MaxY+Linedate[Linedate.size()-1].first)/2;
					Linedate.push_back(tem);
				}
				else if ((MaxY-Linedate[Linedate.size()-1].first)<=width*3/2&&(MaxY-Linedate[Linedate.size()-1].first)>width)  //让每一条线的间距都增加一点的自适应线宽补偿
				{
					for (int i=0;i!=Linedate.size();++i)
					{
						Linedate[i].first+=(MaxY-Linedate[Linedate.size()-1].first-width)/Linedate.size();
					}
				}

				for (int i=0;i!=TheOutline.size();i++)
				{

					for (int j=1;j!=TheOutline[i].size();j++)  //j=1，说明要从第二个点开始循环，直到第一个点。
					{
						int n=(int)TheOutline[i].size()-1;  //轮廓数据的最后一个点是第一个点，因此要减去1。
						int beforeP=(j-1+n)%n;
						int beforebP=(j-2+n)%n;
						int nextP=(j+1+n)%n;
						int nextnP=(j+2+n)%n;
						if (TheOutline[i][j].y!=TheOutline[i][nextP].y) //轮廓线段不平行于x轴的情况。
						{
							for (int k=0;k!=Linedate.size();k++)
							{
								if ((Linedate[k].first-TheOutline[i][j].y)*(Linedate[k].first-TheOutline[i][nextP].y)<0) //线与轮廓线段相交的情况。
								{
									float x1=TheOutline[i][j].x;
									float x2=TheOutline[i][nextP].x;
									float y1=TheOutline[i][j].y;
									float y2=TheOutline[i][nextP].y;
									Linedate[k].second.push_back((x2-x1)/(y2-y1)*(Linedate[k].first-y1)+x1);
								}
								else if ((Linedate[k].first==TheOutline[i][j].y))    //线与轮廓线段前一个顶点相交的情况。
								{
									if ((!IsContainPoint(maxPoint,TheOutline[i][j]))&&(!IsContainPoint(minPoint,TheOutline[i][j]))&&(TheOutline[i][j].y!=TheOutline[i][beforeP].y))
									{
										//这个顶点同时又不是极值点时。同时这个点与前一个点的y值都不相等才行，因为相等时的情况已经已经被后面的逻辑考虑过了！！！
										Linedate[k].second.push_back(TheOutline[i][j].x);
									}
								}
								else if ((Linedate[k].first==TheOutline[i][nextP].y))   //线与轮廓线段后一个顶点相交的情况。
								{
									if ((!IsContainPoint(maxPoint,TheOutline[i][nextP]))&&(!IsContainPoint(minPoint,TheOutline[i][nextP]))&&(TheOutline[i][nextP].y!=TheOutline[i][nextnP].y))
									{
										//这个顶点同时又不是极值点时。同时后一个点与后后一个点的y值都不相等才行，因为相等时的情况已经已经被后面的逻辑考虑过了！！！
										Linedate[k].second.push_back(TheOutline[i][nextP].x);
									}
								}
							}
						}
						else if(TheOutline[i][j].y==TheOutline[i][nextP].y) //轮廓线段平行于x轴的情况。
						{
							for (int k=0;k!=Linedate.size();k++)
							{
								if (Linedate[k].first==TheOutline[i][j].y)  //有填充线恰好与平行于x轴的轮廓线相交。
								{
									if((IsLeft(TheOutline[i][beforeP],TheOutline[i][j],TheOutline[i][nextP])==-1)&&(IsLeft(TheOutline[i][j],TheOutline[i][nextP],TheOutline[i][nextnP])==-1))  //这个轮廓线的两点同时是内点时。
									{   //顺时针向右转是内点。
										Linedate[k].second.push_back(TheOutline[i][j].x);
										Linedate[k].second.push_back(TheOutline[i][nextP].x);
									}
									else if((IsLeft(TheOutline[i][beforeP],TheOutline[i][j],TheOutline[i][nextP])==1)&&(IsLeft(TheOutline[i][j],TheOutline[i][nextP],TheOutline[i][nextnP])==1))
									{
										//两个点都不是内点，两点都舍去，换句话说就是什么也不做，其实不用写这段代码，为了以后好理解所以写上了。
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

				for(int i=0;i!=Linedate.size();i++)  //将存储好的数据重复的元素删除，并且排序。
				{
					Linedate[i].second.sort();    //排序。
					Linedate[i].second.unique();  //将多余的点取出来。
				}
				DeleteOddDate(Linedate);    //暂时加上，看看情况！

				//第三步，将数据分区域存放。
				int FirstNonZero;
				while(!IsEmpty(Linedate,FirstNonZero))
				{
					outline tem;
					int j=0;  //用来判断是奇数行还是偶数行的参数。
					auto firstIn=Linedate[FirstNonZero].second.begin();
					float bijiao1=*firstIn;   //第一条线的第一个点。
					tem.push_back(xdpoint(*firstIn,Linedate[FirstNonZero].first,TheOutline[0][0].z));
					tem.push_back(xdpoint(*(++firstIn),Linedate[FirstNonZero].first,TheOutline[0][0].z));					
					int i=(FirstNonZero+1)%Linedate.size();  //必须保证当第一个非零数据的线刚好是最后一条线时也不会加1越界！所以要模一下！
					float bijiao2=*firstIn;  //第一条线的第二个点。
					float bijiaoY1=Linedate[FirstNonZero].first;  //第一条线的Y坐标值。
					auto tem1s=Linedate[FirstNonZero].second.begin();
					auto tem1e=tem1s;
					tem1e++;
					tem1e++;
					Linedate[FirstNonZero].second.erase(tem1s,tem1e);
					if (std::abs(tem[1].x - tem[0].x)<shrinkDistance*2)  //如果两个点的距离太近了，则不用填充了！！important！！！
						continue;
					while ((!Linedate[i].second.empty()))
					{
						auto Line2first=Linedate[i].second.begin();  //第二条线的第一个点。

						float bijiaoY2=Linedate[i].first; //第二个条直线的Y坐标值。
						if (exceedExtremum(bijiaoY1,bijiaoY2,maxY,minY))
						{
							//前后两条直线如果跨越了局部极值点，也需要退出，即需要分区。
							break;
						}
						bijiaoY1=bijiaoY2;
						if(*Line2first>bijiao2)
						{
							//第二条直线的第一个点大于第一条直线的第二个点则退出，即需要分区
							break;
						}
						if (*(++Line2first)<bijiao1)   //注意：这里++必须在前，因为就算有括号，++在后的话也会比较完再++！！！
						{
							//第二条直线的第二个点小于第一条直线的第一个点则退出，即需要分区
							break;
						}
						auto tem2s=Linedate[i].second.begin();
						auto tem2e=tem2s;
						tem2e++;
						if (std::abs(*tem2e - *tem2s)<shrinkDistance*2)//如果两个点的距离太近了，则不用填充了！！important！！！
							break;
						if (j%2==0)
						{
							/*auto tem2s=Linedate[i].second.begin();
							auto tem2e=tem2s;
							tem2e++;*/
							float vectorY=Linedate[i].first-tem.back().y;
							float vectorX=*tem2e-tem.back().x;
							if (abs(vectorY/sqrt(pow(vectorX,2)+pow(vectorY,2)))<sin(5.0/180*pi))   //连接线和水平线相差1度就分区，记得用1.0
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
							if (abs(vectorY/sqrt(pow(vectorX,2)+pow(vectorY,2)))<sin(5.0/180*pi))   //连接线和水平线相差1度就分区，记得用1.0
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

				//增加一步，为了适应FDM的格式，每个线段要缩短一个半径补偿
				for (int i=0;i!=TheResult.size();++i)
				{
					for (int j=0;j!=TheResult[i].size();++j)
					{

						if (j%4==3||j%4==0)
						{
							TheResult[i][j].x += shrinkDistance;    //此处是默认值。

						}
						else
						{
							TheResult[i][j].x -= shrinkDistance;
						}
					}
				}
			}
		}
			
		//记得最后加上偏置轮廓。
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
		//填充线的结果转回选择角度
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
	if (!TheOutline.empty())  //首先必须保证有数据
	{
		//将轮廓数据旋转一个角度
		for (int i = 0; i != TheOutline.size(); ++i)
		{
			for (int j = 0; j != TheOutline[i].size(); ++j)
			{
				TheOutline[i][j].argument(TheOutline[i][j].argument() - degree);
			}
		}

		//需要一次轮廓偏置来使得外轮廓比较平滑
		std::vector<outlines> dataOffsets;  //存储所有偏置轮廓的数据
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
					temData.push_back(xd::xdpoint((float)solution[i][0].X / 1000000.0, (float)solution[i][0].Y / 1000000.0));//加上最后一个点，保证封闭
					dataOffset.push_back(temData);
				}
				dataOffsets.push_back(dataOffset);
			}
			TheOutline.clear();
			TheOutline = dataOffsets[dataOffsets.size() - 1];
		}
		if (TheOutline.size() != 0)
		{
			//第一步，轮廓点的局部极大极小值点，放在一个双向链表中。注意：选择双向链表的原因是其任意位置删除和添加元素非常快捷，降低时间复杂度。
			std::list<float> maxY;
			std::list<float> minY;
			std::vector<std::pair<float, float>> maxPoint;
			std::vector<std::pair<float, float>> minPoint;
			for (int i = 0; i != TheOutline.size(); i++)
			{
				for (int j = 1; j != TheOutline[i].size(); j++)
				{
					int n = (int)TheOutline[i].size() - 1;  //轮廓数据的最后一个点是第一个点，因此要减去1。
					int beforeP = (j - 1 + n) % n;
					int beforebP = (j - 2 + n) % n;
					int nextP = (j + 1 + n) % n;
					int nextnP = (j + 2 + n) % n;
					if ((TheOutline[i][j].y>TheOutline[i][beforeP].y) && (TheOutline[i][j].y>TheOutline[i][nextP].y))  //这个点的y值大于前后的点的y值。
					{
						maxY.push_back(TheOutline[i][j].y);
						std::pair<float, float> tem;
						tem.first = TheOutline[i][j].x;
						tem.second = TheOutline[i][j].y;
						maxPoint.push_back(tem);
					}
					else if ((TheOutline[i][j].y<TheOutline[i][beforeP].y) && (TheOutline[i][j].y<TheOutline[i][nextP].y)) //这个点的y值小于前后的点的y值。
					{
						minY.push_back(TheOutline[i][j].y);
						std::pair<float, float> tem;
						tem.first = TheOutline[i][j].x;
						tem.second = TheOutline[i][j].y;
						minPoint.push_back(tem);
					}
					else if (TheOutline[i][j].y == TheOutline[i][beforeP].y)
					{
						if ((TheOutline[i][beforeP].y>TheOutline[i][beforebP].y) && (TheOutline[i][j].y>TheOutline[i][nextP].y)) //这个点和前一个点的y值大。
						{
							maxY.push_back(TheOutline[i][j].y);
						}
						if ((TheOutline[i][beforeP].y<TheOutline[i][beforebP].y) && (TheOutline[i][j].y<TheOutline[i][nextP].y)) //这个点和前一个点的y值小。
						{
							minY.push_back(TheOutline[i][j].y);
						}
					}
					else if (TheOutline[i][j].y == TheOutline[i][nextP].y)
					{
						if ((TheOutline[i][nextP].y>TheOutline[i][nextnP].y) && (TheOutline[i][j].y>TheOutline[i][beforeP].y))  //这个点和后一个点的y值大。
						{
							maxY.push_back(TheOutline[i][j].y);
						}
						if ((TheOutline[i][nextP].y<TheOutline[i][nextnP].y) && (TheOutline[i][j].y<TheOutline[i][beforeP].y))  //这个点和后一个点的y值小。
						{
							minY.push_back(TheOutline[i][j].y);
						}
					}
				}
			}
			maxY.sort();
			minY.sort();
			maxY.unique();    //将链表中重复的点删除的函数，属于STL的变易算法。
			minY.unique();	  //同上。
			float MaxY = *max_element(maxY.begin(), maxY.end());   //调用求链表中求最大元素的函数。
			float MinY = *min_element(minY.begin(), minY.end());
			//第二步，生成每一条线填充线与轮廓线的交点，分别存储在双向链表中。
			std::vector<std::pair<float, std::list<float>>> Linedate;
			for (int i = 1; i<(MaxY - MinY) / width - 1; i++)
			{
				std::pair<float, std::list<float>> tem;
				tem.first = MinY + width*i;
				Linedate.push_back(tem);
			}
			if (!Linedate.empty())     //记住，Linedate可能是空的！ 2015_6_17
			{
				if ((MaxY - Linedate[Linedate.size() - 1].first)>width * 3 / 2)//为了使得最后一根填充线不至于与轮廓离得太远，要加一个是否需要增加一条线段判断
				{

					std::pair<float, std::list<float>> tem;
					tem.first = (MaxY + Linedate[Linedate.size() - 1].first) / 2;
					Linedate.push_back(tem);
				}
				else if ((MaxY - Linedate[Linedate.size() - 1].first) <= width * 3 / 2 && (MaxY - Linedate[Linedate.size() - 1].first)>width)  //让每一条线的间距都增加一点的自适应线宽补偿
				{
					for (int i = 0; i != Linedate.size(); ++i)
					{
						Linedate[i].first += (MaxY - Linedate[Linedate.size() - 1].first - width) / Linedate.size();
					}
				}

				for (int i = 0; i != TheOutline.size(); i++)
				{

					for (int j = 1; j != TheOutline[i].size(); j++)  //j=1，说明要从第二个点开始循环，直到第一个点。
					{
						int n = (int)TheOutline[i].size() - 1;  //轮廓数据的最后一个点是第一个点，因此要减去1。
						int beforeP = (j - 1 + n) % n;
						int beforebP = (j - 2 + n) % n;
						int nextP = (j + 1 + n) % n;
						int nextnP = (j + 2 + n) % n;
						if (TheOutline[i][j].y != TheOutline[i][nextP].y) //轮廓线段不平行于x轴的情况。
						{
							for (int k = 0; k != Linedate.size(); k++)
							{
								if ((Linedate[k].first - TheOutline[i][j].y)*(Linedate[k].first - TheOutline[i][nextP].y)<0) //线与轮廓线段相交的情况。
								{
									float x1 = TheOutline[i][j].x;
									float x2 = TheOutline[i][nextP].x;
									float y1 = TheOutline[i][j].y;
									float y2 = TheOutline[i][nextP].y;
									Linedate[k].second.push_back((x2 - x1) / (y2 - y1)*(Linedate[k].first - y1) + x1);
								}
								else if ((Linedate[k].first == TheOutline[i][j].y))    //线与轮廓线段前一个顶点相交的情况。
								{
									if ((!IsContainPoint(maxPoint, TheOutline[i][j])) && (!IsContainPoint(minPoint, TheOutline[i][j])) && (TheOutline[i][j].y != TheOutline[i][beforeP].y))
									{
										//这个顶点同时又不是极值点时。同时这个点与前一个点的y值都不相等才行，因为相等时的情况已经已经被后面的逻辑考虑过了！！！
										Linedate[k].second.push_back(TheOutline[i][j].x);
									}
								}
								else if ((Linedate[k].first == TheOutline[i][nextP].y))   //线与轮廓线段后一个顶点相交的情况。
								{
									if ((!IsContainPoint(maxPoint, TheOutline[i][nextP])) && (!IsContainPoint(minPoint, TheOutline[i][nextP])) && (TheOutline[i][nextP].y != TheOutline[i][nextnP].y))
									{
										//这个顶点同时又不是极值点时。同时后一个点与后后一个点的y值都不相等才行，因为相等时的情况已经已经被后面的逻辑考虑过了！！！
										Linedate[k].second.push_back(TheOutline[i][nextP].x);
									}
								}
							}
						}
						else if (TheOutline[i][j].y == TheOutline[i][nextP].y) //轮廓线段平行于x轴的情况。
						{
							for (int k = 0; k != Linedate.size(); k++)
							{
								if (Linedate[k].first == TheOutline[i][j].y)  //有填充线恰好与平行于x轴的轮廓线相交。
								{
									if ((IsLeft(TheOutline[i][beforeP], TheOutline[i][j], TheOutline[i][nextP]) == -1) && (IsLeft(TheOutline[i][j], TheOutline[i][nextP], TheOutline[i][nextnP]) == -1))  //这个轮廓线的两点同时是内点时。
									{   //顺时针向右转是内点。
										Linedate[k].second.push_back(TheOutline[i][j].x);
										Linedate[k].second.push_back(TheOutline[i][nextP].x);
									}
									else if ((IsLeft(TheOutline[i][beforeP], TheOutline[i][j], TheOutline[i][nextP]) == 1) && (IsLeft(TheOutline[i][j], TheOutline[i][nextP], TheOutline[i][nextnP]) == 1))
									{
										//两个点都不是内点，两点都舍去，换句话说就是什么也不做，其实不用写这段代码，为了以后好理解所以写上了。
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

				for (int i = 0; i != Linedate.size(); i++)  //将存储好的数据重复的元素删除，并且排序。
				{
					Linedate[i].second.sort();    //排序。
					Linedate[i].second.unique();  //将多余的点取出来。
				}
				DeleteOddDate(Linedate);    //暂时加上，看看情况！

				//第三步，将数据分区域存放。
				int FirstNonZero;
				while (!IsEmpty(Linedate, FirstNonZero))
				{
					outline tem;
					int j = 0;  //用来判断是奇数行还是偶数行的参数。
					auto firstIn = Linedate[FirstNonZero].second.begin();
					float bijiao1 = *firstIn;   //第一条线的第一个点。
					tem.push_back(xdpoint(*firstIn, Linedate[FirstNonZero].first, TheOutline[0][0].z));
					tem.push_back(xdpoint(*(++firstIn), Linedate[FirstNonZero].first, TheOutline[0][0].z));
					int i = (FirstNonZero + 1) % Linedate.size();  //必须保证当第一个非零数据的线刚好是最后一条线时也不会加1越界！所以要模一下！
					float bijiao2 = *firstIn;  //第一条线的第二个点。
					float bijiaoY1 = Linedate[FirstNonZero].first;  //第一条线的Y坐标值。
					auto tem1s = Linedate[FirstNonZero].second.begin();
					auto tem1e = tem1s;
					tem1e++;
					tem1e++;
					Linedate[FirstNonZero].second.erase(tem1s, tem1e);
					if (std::abs(tem[1].x - tem[0].x)<shrinkDistance * 2)  //如果两个点的距离太近了，则不用填充了！！important！！！
						continue;
					while ((!Linedate[i].second.empty()))
					{
						auto Line2first = Linedate[i].second.begin();  //第二条线的第一个点。

						float bijiaoY2 = Linedate[i].first; //第二个条直线的Y坐标值。
						if (exceedExtremum(bijiaoY1, bijiaoY2, maxY, minY))
						{
							//前后两条直线如果跨越了局部极值点，也需要退出，即需要分区。
							break;
						}
						bijiaoY1 = bijiaoY2;
						if (*Line2first>bijiao2)
						{
							//第二条直线的第一个点大于第一条直线的第二个点则退出，即需要分区
							break;
						}
						if (*(++Line2first)<bijiao1)   //注意：这里++必须在前，因为就算有括号，++在后的话也会比较完再++！！！
						{
							//第二条直线的第二个点小于第一条直线的第一个点则退出，即需要分区
							break;
						}
						auto tem2s = Linedate[i].second.begin();
						auto tem2e = tem2s;
						tem2e++;
						if (std::abs(*tem2e - *tem2s)<shrinkDistance * 2)//如果两个点的距离太近了，则不用填充了！！important！！！
							break;
						if (j % 2 == 0)
						{
							/*auto tem2s=Linedate[i].second.begin();
							auto tem2e=tem2s;
							tem2e++;*/
							float vectorY = Linedate[i].first - tem.back().y;
							float vectorX = *tem2e - tem.back().x;
							if (abs(vectorY / sqrt(pow(vectorX, 2) + pow(vectorY, 2)))<sin(5.0 / 180 * pi))   //连接线和水平线相差1度就分区，记得用1.0
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
							if (abs(vectorY / sqrt(pow(vectorX, 2) + pow(vectorY, 2)))<sin(5.0 / 180 * pi))   //连接线和水平线相差1度就分区，记得用1.0
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

				//增加一步，为了适应FDM的格式，每个线段要缩短一个半径补偿
				for (int i = 0; i != TheResult.size(); ++i)
				{
					for (int j = 0; j != TheResult[i].size(); ++j)
					{

						if (j % 4 == 3 || j % 4 == 0)
						{
							TheResult[i][j].x += shrinkDistance;    //此处是默认值。

						}
						else
						{
							TheResult[i][j].x -= shrinkDistance;
						}
					}
				}
			}
		}

		//记得最后加上偏置轮廓。
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
		//填充线的结果转回选择角度
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

void InfillLineSLA(outlines TheOutline, outlines & TheResult, float width, float degree, int lunkuo,  float shrinkDistance,float offsetWidth) //自己编写的填充线生成函数。
{
    if(!TheOutline.empty())  //首先必须保证有数据
    {
        //将轮廓数据旋转一个角度
        for (int i=0;i!=TheOutline.size();++i)
        {
            for (int j=0;j!=TheOutline[i].size();++j)
            {
                TheOutline[i][j].argument(TheOutline[i][j].argument()-degree);
            }
        }

        //需要一次轮廓偏置来使得外轮廓比较平滑
        std::vector<outlines> dataOffsets;  //存储所有偏置轮廓的数据
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
                    temData.push_back(xd::xdpoint((float)solution[i][0].X/1000000.0,(float)solution[i][0].Y/1000000.0));//加上最后一个点，保证封闭
                    dataOffset.push_back(temData);
                }
                dataOffsets.push_back(dataOffset);
            }
            TheOutline.clear();
            TheOutline=dataOffsets[dataOffsets.size()-1];
        }
        if(TheOutline.size()!=0)
        {
            //第一步，轮廓点的局部极大极小值点，放在一个双向链表中。注意：选择双向链表的原因是其任意位置删除和添加元素非常快捷，降低时间复杂度。
            std::list<float> maxY;
            std::list<float> minY;
            std::vector<std::pair<float,float>> maxPoint;
            std::vector<std::pair<float,float>> minPoint;
            //注意，这里加了一个东西！！2015_12_6 22:28  气死啦，果然自己编的算法总是有问题！
            float temMax=TheOutline[0][0].y;
            float temMin=TheOutline[0][0].y;
            for (int i=0;i!=TheOutline.size();i++)
            {
                for (int j=0;j!=TheOutline[i].size();j++)
                {
                    if(temMax<TheOutline[i][j].y)
                        temMax=TheOutline[i][j].y;
                    if(temMin>TheOutline[i][j].y)
                        temMin=TheOutline[i][j].y;
                }
            }
            maxY.push_back(temMax);
            minY.push_back(temMin);

            for (int i=0;i!=TheOutline.size();i++)
            {
                for (int j=1;j!=TheOutline[i].size();j++)
                {
                    int n=(int)TheOutline[i].size()-1;  //轮廓数据的最后一个点是第一个点，因此要减去1。
                    int beforeP=(j-1+n)%n;
                    int beforebP=(j-2+n)%n;
                    int nextP=(j+1+n)%n;
                    int nextnP=(j+2+n)%n;
                    if ((TheOutline[i][j].y>TheOutline[i][beforeP].y)&&(TheOutline[i][j].y>TheOutline[i][nextP].y))  //这个点的y值大于前后的点的y值。
                    {
                        maxY.push_back(TheOutline[i][j].y);
                        std::pair<float,float> tem;
                        tem.first=TheOutline[i][j].x;
                        tem.second=TheOutline[i][j].y;
                        maxPoint.push_back(tem);
                    }
                    else if ((TheOutline[i][j].y<TheOutline[i][beforeP].y)&&(TheOutline[i][j].y<TheOutline[i][nextP].y)) //这个点的y值小于前后的点的y值。
                    {
                        minY.push_back(TheOutline[i][j].y);
                        std::pair<float,float> tem;
                        tem.first=TheOutline[i][j].x;
                        tem.second=TheOutline[i][j].y;
                        minPoint.push_back(tem);
                    }
                    else if (TheOutline[i][j].y==TheOutline[i][beforeP].y)
                    {
                        if ((TheOutline[i][beforeP].y>TheOutline[i][beforebP].y)&&(TheOutline[i][j].y>TheOutline[i][nextP].y)) //这个点和前一个点的y值大。
                        {
                            maxY.push_back(TheOutline[i][j].y);
                        }
                        if ((TheOutline[i][beforeP].y<TheOutline[i][beforebP].y)&&(TheOutline[i][j].y<TheOutline[i][nextP].y)) //这个点和前一个点的y值小。
                        {
                            minY.push_back(TheOutline[i][j].y);
                        }
                    }
                    else if (TheOutline[i][j].y==TheOutline[i][nextP].y)
                    {
                        if ((TheOutline[i][nextP].y>TheOutline[i][nextnP].y)&&(TheOutline[i][j].y>TheOutline[i][beforeP].y))  //这个点和后一个点的y值大。
                        {
                            maxY.push_back(TheOutline[i][j].y);
                        }
                        if ((TheOutline[i][nextP].y<TheOutline[i][nextnP].y)&&(TheOutline[i][j].y<TheOutline[i][beforeP].y))  //这个点和后一个点的y值小。
                        {
                            minY.push_back(TheOutline[i][j].y);
                        }
                    }
                }
            }
            maxY.sort();
            minY.sort();
            maxY.unique();    //将链表中重复的点删除的函数，属于STL的变易算法。
            minY.unique();	  //同上。
            float MaxY=*max_element(maxY.begin(),maxY.end());   //调用求链表中求最大元素的函数。
            float MinY=*min_element(minY.begin(),minY.end());
            //第二步，生成每一条线填充线与轮廓线的交点，分别存储在双向链表中。
            std::vector<std::pair<float,std::list<float>>> Linedate;
            for (int i=1;i<(MaxY-MinY)/width-1;i++)
            {
                std::pair<float,std::list<float>> tem;
                tem.first=MinY+width*i;
                Linedate.push_back(tem);
            }
            if(!Linedate.empty())     //记住，Linedate可能是空的！ 2015_6_17
            {
                if ((MaxY-Linedate[Linedate.size()-1].first)>width*3/2)//为了使得最后一根填充线不至于与轮廓离得太远，要加一个是否需要增加一条线段判断
                {

                    std::pair<float,std::list<float>> tem;
                    tem.first=(MaxY+Linedate[Linedate.size()-1].first)/2;
                    Linedate.push_back(tem);
                }
                else if ((MaxY-Linedate[Linedate.size()-1].first)<=width*3/2&&(MaxY-Linedate[Linedate.size()-1].first)>width)  //让每一条线的间距都增加一点的自适应线宽补偿
                {
                    for (int i=0;i!=Linedate.size();++i)
                    {
                        Linedate[i].first+=(MaxY-Linedate[Linedate.size()-1].first-width)/Linedate.size();
                    }
                }

                for (int i=0;i!=TheOutline.size();i++)
                {

                    for (int j=1;j!=TheOutline[i].size();j++)  //j=1，说明要从第二个点开始循环，直到第一个点。
                    {
                        int n=(int)TheOutline[i].size()-1;  //轮廓数据的最后一个点是第一个点，因此要减去1。
                        int beforeP=(j-1+n)%n;
                        int beforebP=(j-2+n)%n;
                        int nextP=(j+1+n)%n;
                        int nextnP=(j+2+n)%n;
                        if (TheOutline[i][j].y!=TheOutline[i][nextP].y) //轮廓线段不平行于x轴的情况。
                        {
                            for (int k=0;k!=Linedate.size();k++)
                            {
                                if ((Linedate[k].first-TheOutline[i][j].y)*(Linedate[k].first-TheOutline[i][nextP].y)<0) //线与轮廓线段相交的情况。
                                {
                                    float x1=TheOutline[i][j].x;
                                    float x2=TheOutline[i][nextP].x;
                                    float y1=TheOutline[i][j].y;
                                    float y2=TheOutline[i][nextP].y;
                                    Linedate[k].second.push_back((x2-x1)/(y2-y1)*(Linedate[k].first-y1)+x1);
                                }
                                else if ((Linedate[k].first==TheOutline[i][j].y))    //线与轮廓线段前一个顶点相交的情况。
                                {
                                    if ((!IsContainPoint(maxPoint,TheOutline[i][j]))&&(!IsContainPoint(minPoint,TheOutline[i][j]))&&(TheOutline[i][j].y!=TheOutline[i][beforeP].y))
                                    {
                                        //这个顶点同时又不是极值点时。同时这个点与前一个点的y值都不相等才行，因为相等时的情况已经已经被后面的逻辑考虑过了！！！
                                        Linedate[k].second.push_back(TheOutline[i][j].x);
                                    }
                                }
                                else if ((Linedate[k].first==TheOutline[i][nextP].y))   //线与轮廓线段后一个顶点相交的情况。
                                {
                                    if ((!IsContainPoint(maxPoint,TheOutline[i][nextP]))&&(!IsContainPoint(minPoint,TheOutline[i][nextP]))&&(TheOutline[i][nextP].y!=TheOutline[i][nextnP].y))
                                    {
                                        //这个顶点同时又不是极值点时。同时后一个点与后后一个点的y值都不相等才行，因为相等时的情况已经已经被后面的逻辑考虑过了！！！
                                        Linedate[k].second.push_back(TheOutline[i][nextP].x);
                                    }
                                }
                            }
                        }
                        else if(TheOutline[i][j].y==TheOutline[i][nextP].y) //轮廓线段平行于x轴的情况。
                        {
                            for (int k=0;k!=Linedate.size();k++)
                            {
                                if (Linedate[k].first==TheOutline[i][j].y)  //有填充线恰好与平行于x轴的轮廓线相交。
                                {
                                    if((IsLeft(TheOutline[i][beforeP],TheOutline[i][j],TheOutline[i][nextP])==-1)&&(IsLeft(TheOutline[i][j],TheOutline[i][nextP],TheOutline[i][nextnP])==-1))  //这个轮廓线的两点同时是内点时。
                                    {   //顺时针向右转是内点。
                                        Linedate[k].second.push_back(TheOutline[i][j].x);
                                        Linedate[k].second.push_back(TheOutline[i][nextP].x);
                                    }
                                    else if((IsLeft(TheOutline[i][beforeP],TheOutline[i][j],TheOutline[i][nextP])==1)&&(IsLeft(TheOutline[i][j],TheOutline[i][nextP],TheOutline[i][nextnP])==1))
                                    {
                                        //两个点都不是内点，两点都舍去，换句话说就是什么也不做，其实不用写这段代码，为了以后好理解所以写上了。
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

                for(int i=0;i!=Linedate.size();i++)  //将存储好的数据重复的元素删除，并且排序。
                {
                    Linedate[i].second.sort();    //排序。
                    Linedate[i].second.unique();  //将多余的点取出来。
                }
                DeleteOddDate(Linedate);    //暂时加上，看看情况！

                //第三步，将数据分区域存放。
                int FirstNonZero;
                while(!IsEmpty(Linedate,FirstNonZero))
                {
                    outline tem;
                    int j=0;  //用来判断是奇数行还是偶数行的参数。
                    auto firstIn=Linedate[FirstNonZero].second.begin();
                    float bijiao1=*firstIn;   //第一条线的第一个点。
                    tem.push_back(xdpoint(*firstIn,Linedate[FirstNonZero].first,TheOutline[0][0].z));
                    tem.push_back(xdpoint(*(++firstIn),Linedate[FirstNonZero].first,TheOutline[0][0].z));
                    int i=(FirstNonZero+1)%Linedate.size();  //必须保证当第一个非零数据的线刚好是最后一条线时也不会加1越界！所以要模一下！
                    float bijiao2=*firstIn;  //第一条线的第二个点。
                    float bijiaoY1=Linedate[FirstNonZero].first;  //第一条线的Y坐标值。
                    auto tem1s=Linedate[FirstNonZero].second.begin();
                    auto tem1e=tem1s;
                    tem1e++;
                    tem1e++;
                    Linedate[FirstNonZero].second.erase(tem1s,tem1e);
                    if (std::abs(tem[1].x - tem[0].x)<shrinkDistance*2)  //如果两个点的距离太近了，则不用填充了！！important！！！
                        continue;
                    while ((!Linedate[i].second.empty()))
                    {
                        auto Line2first=Linedate[i].second.begin();  //第二条线的第一个点。

                        float bijiaoY2=Linedate[i].first; //第二个条直线的Y坐标值。
//                        if (exceedExtremum(bijiaoY1,bijiaoY2,maxY,minY))
//                        {
//                            //前后两条直线如果跨越了局部极值点，也需要退出，即需要分区。
//                            break;
//                        }
//                        bijiaoY1=bijiaoY2;
//                        if(*Line2first>bijiao2)
//                        {
//                            //第二条直线的第一个点大于第一条直线的第二个点则退出，即需要分区
//                            break;
//                        }
//                        if (*(++Line2first)<bijiao1)   //注意：这里++必须在前，因为就算有括号，++在后的话也会比较完再++！！！
//                        {
//                            //第二条直线的第二个点小于第一条直线的第一个点则退出，即需要分区
//                            break;
//                        }
                        auto tem2s=Linedate[i].second.begin();
                        auto tem2e=tem2s;
                        tem2e++;
                        if (std::abs(*tem2e - *tem2s)<shrinkDistance*2)//如果两个点的距离太近了，则不用填充了！！important！！！
                            break;
                        if (j%2==0)
                        {
                            float vectorY=Linedate[i].first-tem.back().y;
                            float vectorX=*tem2e-tem.back().x;
//                            if (abs(vectorY/sqrt(pow(vectorX,2)+pow(vectorY,2)))<sin(5.0/180*pi))   //连接线和水平线相差1度就分区，记得用1.0
//                            {
//                                break;
//                            }
                            tem.push_back(xdpoint(*tem2e,Linedate[i].first,TheOutline[0][0].z));
                            tem.push_back(xdpoint(*tem2s,Linedate[i].first,TheOutline[0][0].z));
                        }
                        else
                        {
                            float vectorY=Linedate[i].first-tem.back().y;
                            float vectorX=*tem2s-tem.back().x;
//                            if (abs(vectorY/sqrt(pow(vectorX,2)+pow(vectorY,2)))<sin(5.0/180*pi))   //连接线和水平线相差1度就分区，记得用1.0
//                            {
//                                break;
//                            }
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

                //每个线段要缩短一个半径补偿
                for (int i=0;i!=TheResult.size();++i)
                {
                    for (int j=0;j!=TheResult[i].size();++j)
                    {

                        if (j%4==3||j%4==0)
                        {
                            TheResult[i][j].x += shrinkDistance;    //此处是默认值。

                        }
                        else
                        {
                            TheResult[i][j].x -= shrinkDistance;
                        }
                    }
                }
                //为了适应SLA，每个点都要分开加入！
                outlines temS;
                for(const outline & ol : TheResult)
                {
                    if(ol.size()==2)
                        temS.push_back(ol);
                    else
                        for(int i=0 ; i < ol.size() ; i+=2)
                        {
                            outline tem;
                            tem.push_back(ol[i]);
                            tem.push_back(ol[i+1]);
                            temS.push_back(tem);
                        }
                }
                TheResult.clear();
                TheResult.insert(TheResult.end(),temS.begin(),temS.end());
            }
        }

        //记得最后加上偏置轮廓。
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
        //填充线的结果转回选择角度
        for (int i=0;i!=TheResult.size();++i)
        {
            for (int j=0;j!=TheResult[i].size();++j)
            {
                TheResult[i][j].argument(TheResult[i][j].argument()+degree);
            }
        }
    }
}

void InfillLineSLA(outlines TheOutline, outlines & TheResult, outlines & TheOutlineResult, float width, float degree, int lunkuo, float shrinkDistance, float offsetWidth)
{
    if (!TheOutline.empty())  //首先必须保证有数据
    {
        //将轮廓数据旋转一个角度
        for (int i = 0; i != TheOutline.size(); ++i)
        {
            for (int j = 0; j != TheOutline[i].size(); ++j)
            {
                TheOutline[i][j].argument(TheOutline[i][j].argument() - degree);
            }
        }

        //需要一次轮廓偏置来使得外轮廓比较平滑
        std::vector<outlines> dataOffsets;  //存储所有偏置轮廓的数据
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
                    temData.push_back(xd::xdpoint((float)solution[i][0].X / 1000000.0, (float)solution[i][0].Y / 1000000.0));//加上最后一个点，保证封闭
                    dataOffset.push_back(temData);
                }
                dataOffsets.push_back(dataOffset);
            }
            TheOutline.clear();
            TheOutline = dataOffsets[dataOffsets.size() - 1];
        }
        if (TheOutline.size() != 0)
        {
            //第一步，轮廓点的局部极大极小值点，放在一个双向链表中。注意：选择双向链表的原因是其任意位置删除和添加元素非常快捷，降低时间复杂度。
            std::list<float> maxY;
            std::list<float> minY;
            std::vector<std::pair<float, float>> maxPoint;
            std::vector<std::pair<float, float>> minPoint;
            for (int i = 0; i != TheOutline.size(); i++)
            {
                for (int j = 1; j != TheOutline[i].size(); j++)
                {
                    int n = (int)TheOutline[i].size() - 1;  //轮廓数据的最后一个点是第一个点，因此要减去1。
                    int beforeP = (j - 1 + n) % n;
                    int beforebP = (j - 2 + n) % n;
                    int nextP = (j + 1 + n) % n;
                    int nextnP = (j + 2 + n) % n;
                    if ((TheOutline[i][j].y>TheOutline[i][beforeP].y) && (TheOutline[i][j].y>TheOutline[i][nextP].y))  //这个点的y值大于前后的点的y值。
                    {
                        maxY.push_back(TheOutline[i][j].y);
                        std::pair<float, float> tem;
                        tem.first = TheOutline[i][j].x;
                        tem.second = TheOutline[i][j].y;
                        maxPoint.push_back(tem);
                    }
                    else if ((TheOutline[i][j].y<TheOutline[i][beforeP].y) && (TheOutline[i][j].y<TheOutline[i][nextP].y)) //这个点的y值小于前后的点的y值。
                    {
                        minY.push_back(TheOutline[i][j].y);
                        std::pair<float, float> tem;
                        tem.first = TheOutline[i][j].x;
                        tem.second = TheOutline[i][j].y;
                        minPoint.push_back(tem);
                    }
                    else if (TheOutline[i][j].y == TheOutline[i][beforeP].y)
                    {
                        if ((TheOutline[i][beforeP].y>TheOutline[i][beforebP].y) && (TheOutline[i][j].y>TheOutline[i][nextP].y)) //这个点和前一个点的y值大。
                        {
                            maxY.push_back(TheOutline[i][j].y);
                        }
                        if ((TheOutline[i][beforeP].y<TheOutline[i][beforebP].y) && (TheOutline[i][j].y<TheOutline[i][nextP].y)) //这个点和前一个点的y值小。
                        {
                            minY.push_back(TheOutline[i][j].y);
                        }
                    }
                    else if (TheOutline[i][j].y == TheOutline[i][nextP].y)
                    {
                        if ((TheOutline[i][nextP].y>TheOutline[i][nextnP].y) && (TheOutline[i][j].y>TheOutline[i][beforeP].y))  //这个点和后一个点的y值大。
                        {
                            maxY.push_back(TheOutline[i][j].y);
                        }
                        if ((TheOutline[i][nextP].y<TheOutline[i][nextnP].y) && (TheOutline[i][j].y<TheOutline[i][beforeP].y))  //这个点和后一个点的y值小。
                        {
                            minY.push_back(TheOutline[i][j].y);
                        }
                    }
                }
            }
            maxY.sort();
            minY.sort();
            maxY.unique();    //将链表中重复的点删除的函数，属于STL的变易算法。
            minY.unique();	  //同上。
            float MaxY = *max_element(maxY.begin(), maxY.end());   //调用求链表中求最大元素的函数。
            float MinY = *min_element(minY.begin(), minY.end());
            //第二步，生成每一条线填充线与轮廓线的交点，分别存储在双向链表中。
            std::vector<std::pair<float, std::list<float>>> Linedate;
            for (int i = 1; i<(MaxY - MinY) / width - 1; i++)
            {
                std::pair<float, std::list<float>> tem;
                tem.first = MinY + width*i;
                Linedate.push_back(tem);
            }
            if (!Linedate.empty())     //记住，Linedate可能是空的！ 2015_6_17
            {
                if ((MaxY - Linedate[Linedate.size() - 1].first)>width * 3 / 2)//为了使得最后一根填充线不至于与轮廓离得太远，要加一个是否需要增加一条线段判断
                {

                    std::pair<float, std::list<float>> tem;
                    tem.first = (MaxY + Linedate[Linedate.size() - 1].first) / 2;
                    Linedate.push_back(tem);
                }
                else if ((MaxY - Linedate[Linedate.size() - 1].first) <= width * 3 / 2 && (MaxY - Linedate[Linedate.size() - 1].first)>width)  //让每一条线的间距都增加一点的自适应线宽补偿
                {
                    for (int i = 0; i != Linedate.size(); ++i)
                    {
                        Linedate[i].first += (MaxY - Linedate[Linedate.size() - 1].first - width) / Linedate.size();
                    }
                }

                for (int i = 0; i != TheOutline.size(); i++)
                {

                    for (int j = 1; j != TheOutline[i].size(); j++)  //j=1，说明要从第二个点开始循环，直到第一个点。
                    {
                        int n = (int)TheOutline[i].size() - 1;  //轮廓数据的最后一个点是第一个点，因此要减去1。
                        int beforeP = (j - 1 + n) % n;
                        int beforebP = (j - 2 + n) % n;
                        int nextP = (j + 1 + n) % n;
                        int nextnP = (j + 2 + n) % n;
                        if (TheOutline[i][j].y != TheOutline[i][nextP].y) //轮廓线段不平行于x轴的情况。
                        {
                            for (int k = 0; k != Linedate.size(); k++)
                            {
                                if ((Linedate[k].first - TheOutline[i][j].y)*(Linedate[k].first - TheOutline[i][nextP].y)<0) //线与轮廓线段相交的情况。
                                {
                                    float x1 = TheOutline[i][j].x;
                                    float x2 = TheOutline[i][nextP].x;
                                    float y1 = TheOutline[i][j].y;
                                    float y2 = TheOutline[i][nextP].y;
                                    Linedate[k].second.push_back((x2 - x1) / (y2 - y1)*(Linedate[k].first - y1) + x1);
                                }
                                else if ((Linedate[k].first == TheOutline[i][j].y))    //线与轮廓线段前一个顶点相交的情况。
                                {
                                    if ((!IsContainPoint(maxPoint, TheOutline[i][j])) && (!IsContainPoint(minPoint, TheOutline[i][j])) && (TheOutline[i][j].y != TheOutline[i][beforeP].y))
                                    {
                                        //这个顶点同时又不是极值点时。同时这个点与前一个点的y值都不相等才行，因为相等时的情况已经已经被后面的逻辑考虑过了！！！
                                        Linedate[k].second.push_back(TheOutline[i][j].x);
                                    }
                                }
                                else if ((Linedate[k].first == TheOutline[i][nextP].y))   //线与轮廓线段后一个顶点相交的情况。
                                {
                                    if ((!IsContainPoint(maxPoint, TheOutline[i][nextP])) && (!IsContainPoint(minPoint, TheOutline[i][nextP])) && (TheOutline[i][nextP].y != TheOutline[i][nextnP].y))
                                    {
                                        //这个顶点同时又不是极值点时。同时后一个点与后后一个点的y值都不相等才行，因为相等时的情况已经已经被后面的逻辑考虑过了！！！
                                        Linedate[k].second.push_back(TheOutline[i][nextP].x);
                                    }
                                }
                            }
                        }
                        else if (TheOutline[i][j].y == TheOutline[i][nextP].y) //轮廓线段平行于x轴的情况。
                        {
                            for (int k = 0; k != Linedate.size(); k++)
                            {
                                if (Linedate[k].first == TheOutline[i][j].y)  //有填充线恰好与平行于x轴的轮廓线相交。
                                {
                                    if ((IsLeft(TheOutline[i][beforeP], TheOutline[i][j], TheOutline[i][nextP]) == -1) && (IsLeft(TheOutline[i][j], TheOutline[i][nextP], TheOutline[i][nextnP]) == -1))  //这个轮廓线的两点同时是内点时。
                                    {   //顺时针向右转是内点。
                                        Linedate[k].second.push_back(TheOutline[i][j].x);
                                        Linedate[k].second.push_back(TheOutline[i][nextP].x);
                                    }
                                    else if ((IsLeft(TheOutline[i][beforeP], TheOutline[i][j], TheOutline[i][nextP]) == 1) && (IsLeft(TheOutline[i][j], TheOutline[i][nextP], TheOutline[i][nextnP]) == 1))
                                    {
                                        //两个点都不是内点，两点都舍去，换句话说就是什么也不做，其实不用写这段代码，为了以后好理解所以写上了。
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

                for (int i = 0; i != Linedate.size(); i++)  //将存储好的数据重复的元素删除，并且排序。
                {
                    Linedate[i].second.sort();    //排序。
                    Linedate[i].second.unique();  //将多余的点取出来。
                }
                DeleteOddDate(Linedate);    //暂时加上，看看情况！

                //第三步，将数据分区域存放。
                int FirstNonZero;
                while (!IsEmpty(Linedate, FirstNonZero))
                {
                    outline tem;
                    int j = 0;  //用来判断是奇数行还是偶数行的参数。
                    auto firstIn = Linedate[FirstNonZero].second.begin();
                    //float bijiao1 = *firstIn;   //第一条线的第一个点。
                    tem.push_back(xdpoint(*firstIn, Linedate[FirstNonZero].first, TheOutline[0][0].z));
                    tem.push_back(xdpoint(*(++firstIn), Linedate[FirstNonZero].first, TheOutline[0][0].z));
                    int i = (FirstNonZero + 1) % Linedate.size();  //必须保证当第一个非零数据的线刚好是最后一条线时也不会加1越界！所以要模一下！
                    //float bijiao2 = *firstIn;  //第一条线的第二个点。
                    //float bijiaoY1 = Linedate[FirstNonZero].first;  //第一条线的Y坐标值。
                    auto tem1s = Linedate[FirstNonZero].second.begin();
                    auto tem1e = tem1s;
                    tem1e++;
                    tem1e++;
                    Linedate[FirstNonZero].second.erase(tem1s, tem1e);
                    if (std::abs(tem[1].x - tem[0].x)<shrinkDistance * 2)  //如果两个点的距离太近了，则不用填充了！！important！！！
                        continue;
                    while ((!Linedate[i].second.empty()))
                    {
 //                       auto Line2first = Linedate[i].second.begin();  //第二条线的第一个点。
//
 //                       float bijiaoY2 = Linedate[i].first; //第二个条直线的Y坐标值。
//                        if (exceedExtremum(bijiaoY1, bijiaoY2, maxY, minY))
//                        {
//                            //前后两条直线如果跨越了局部极值点，也需要退出，即需要分区。
//                            break;
//                        }
//                        bijiaoY1 = bijiaoY2;
//                        if (*Line2first>bijiao2)
//                        {
//                            //第二条直线的第一个点大于第一条直线的第二个点则退出，即需要分区
//                            break;
//                        }
//                        if (*(++Line2first)<bijiao1)   //注意：这里++必须在前，因为就算有括号，++在后的话也会比较完再++！！！
//                        {
//                            //第二条直线的第二个点小于第一条直线的第一个点则退出，即需要分区
//                            break;
//                        }
                        auto tem2s = Linedate[i].second.begin();  //第二条线的第一个点。
                        auto tem2e = tem2s;   //第二条线的第二个点。
                        tem2e++;
                        if (std::abs(*tem2e - *tem2s)<shrinkDistance * 2)//如果两个点的距离太近了，则不用填充了！！important！！！
                            break;
                        if (j % 2 == 0)
                        {
                            //float vectorY = Linedate[i].first - tem.back().y;
                            //float vectorX = *tem2e - tem.back().x;
//                            if (abs(vectorY / sqrt(pow(vectorX, 2) + pow(vectorY, 2)))<sin(5.0 / 180 * pi))   //连接线和水平线相差1度就分区，记得用1.0
//                            {
//                                break;
//                            }
                            tem.push_back(xdpoint(*tem2e, Linedate[i].first, TheOutline[0][0].z));
                            tem.push_back(xdpoint(*tem2s, Linedate[i].first, TheOutline[0][0].z));
                        }
                        else
                        {
                           // float vectorY = Linedate[i].first - tem.back().y;
                            //float vectorX = *tem2s - tem.back().x;
//                            if (abs(vectorY / sqrt(pow(vectorX, 2) + pow(vectorY, 2)))<sin(5.0 / 180 * pi))   //连接线和水平线相差1度就分区，记得用1.0
//                            {
//                                break;
//                            }
                            tem.push_back(xdpoint(*tem2s, Linedate[i].first, TheOutline[0][0].z));
                            tem.push_back(xdpoint(*tem2e, Linedate[i].first, TheOutline[0][0].z));
                        }
                        auto tem3s = Linedate[i].second.begin();
                        auto tem3e = tem3s;
                        //bijiao1 = *tem3e;
                        tem3e++;
                        //bijiao2 = *tem3e;
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

                //每个线段要缩短一个半径补偿
                for (int i = 0; i != TheResult.size(); ++i)
                {
                    for (int j = 0; j != TheResult[i].size(); ++j)
                    {

                        if (j % 4 == 3 || j % 4 == 0)
                        {
                            TheResult[i][j].x += shrinkDistance;

                        }
                        else
                        {
                            TheResult[i][j].x -= shrinkDistance;
                        }
                    }
                }
                //为了适应SLA，每个点都要分开加入！
                outlines temS;
                for(const outline & ol : TheResult)
                {
                    if(ol.size()==2)
                        temS.push_back(ol);
                    else
                        for(int i=0 ; i < ol.size() ; i+=2)
                        {
                            outline tem;
                            tem.push_back(ol[i]);
                            tem.push_back(ol[i+1]);
                            temS.push_back(tem);
                        }
                }
                TheResult.clear();
                TheResult.insert(TheResult.end(),temS.begin(),temS.end());
            }
        }

        //记得最后加上偏置轮廓。
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
        //填充线的结果转回选择角度
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

void notInfillLine(outlines TheOutline, outlines & TheResult, outlines & TheOutlineResult, float width, float degree, int lunkuo, float shrinkDistance, float offsetWidth)//不分区的填充算法，为了加速
{
	if (!TheOutline.empty())  //首先必须保证有数据
	{
		//将轮廓数据旋转一个角度
		for (int i = 0; i != TheOutline.size(); ++i)
		{
			for (int j = 0; j != TheOutline[i].size(); ++j)
			{
				TheOutline[i][j].argument(TheOutline[i][j].argument() - degree);
			}
		}

		//需要一次轮廓偏置来使得外轮廓比较平滑
		std::vector<outlines> dataOffsets;  //存储所有偏置轮廓的数据
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
					temData.push_back(xd::xdpoint((float)solution[i][0].X / 1000000.0, (float)solution[i][0].Y / 1000000.0));//加上最后一个点，保证封闭
					dataOffset.push_back(temData);
				}
				dataOffsets.push_back(dataOffset);
			}
			TheOutline.clear();
			TheOutline = dataOffsets[dataOffsets.size() - 1];
		}
		if (TheOutline.size() != 0)
		{
			//第一步，轮廓点的局部极大极小值点，放在一个双向链表中。注意：选择双向链表的原因是其任意位置删除和添加元素非常快捷，降低时间复杂度。
			std::list<float> maxY;
			std::list<float> minY;
			std::vector<std::pair<float, float>> maxPoint;
			std::vector<std::pair<float, float>> minPoint;
			for (int i = 0; i != TheOutline.size(); i++)
			{
				for (int j = 1; j != TheOutline[i].size(); j++)
				{
					int n = (int)TheOutline[i].size() - 1;  //轮廓数据的最后一个点是第一个点，因此要减去1。
					int beforeP = (j - 1 + n) % n;
					int beforebP = (j - 2 + n) % n;
					int nextP = (j + 1 + n) % n;
					int nextnP = (j + 2 + n) % n;
					if ((TheOutline[i][j].y > TheOutline[i][beforeP].y) && (TheOutline[i][j].y > TheOutline[i][nextP].y))  //这个点的y值大于前后的点的y值。
					{
						maxY.push_back(TheOutline[i][j].y);
						std::pair<float, float> tem;
						tem.first = TheOutline[i][j].x;
						tem.second = TheOutline[i][j].y;
						maxPoint.push_back(tem);
					}
					else if ((TheOutline[i][j].y < TheOutline[i][beforeP].y) && (TheOutline[i][j].y<TheOutline[i][nextP].y)) //这个点的y值小于前后的点的y值。
					{
						minY.push_back(TheOutline[i][j].y);
						std::pair<float, float> tem;
						tem.first = TheOutline[i][j].x;
						tem.second = TheOutline[i][j].y;
						minPoint.push_back(tem);
					}
					else if (TheOutline[i][j].y == TheOutline[i][beforeP].y)
					{
						if ((TheOutline[i][beforeP].y>TheOutline[i][beforebP].y) && (TheOutline[i][j].y > TheOutline[i][nextP].y)) //这个点和前一个点的y值大。
						{
							maxY.push_back(TheOutline[i][j].y);
						}
						if ((TheOutline[i][beforeP].y < TheOutline[i][beforebP].y) && (TheOutline[i][j].y<TheOutline[i][nextP].y)) //这个点和前一个点的y值小。
						{
							minY.push_back(TheOutline[i][j].y);
						}
					}
					else if (TheOutline[i][j].y == TheOutline[i][nextP].y)
					{
						if ((TheOutline[i][nextP].y>TheOutline[i][nextnP].y) && (TheOutline[i][j].y > TheOutline[i][beforeP].y))  //这个点和后一个点的y值大。
						{
							maxY.push_back(TheOutline[i][j].y);
						}
						if ((TheOutline[i][nextP].y < TheOutline[i][nextnP].y) && (TheOutline[i][j].y < TheOutline[i][beforeP].y))  //这个点和后一个点的y值小。
						{
							minY.push_back(TheOutline[i][j].y);
						}
					}
				}
			}
			maxY.sort();
			minY.sort();
			maxY.unique();    //将链表中重复的点删除的函数，属于STL的变易算法。
			minY.unique();	  //同上。
			float MaxY = *max_element(maxY.begin(), maxY.end());   //调用求链表中求最大元素的函数。
			float MinY = *min_element(minY.begin(), minY.end());
			//第二步，生成每一条线填充线与轮廓线的交点，分别存储在双向链表中。
			std::vector<std::pair<float, std::list<float>>> Linedate;
			for (int i = 1; i < (MaxY - MinY) / width - 1; i++)
			{
				std::pair<float, std::list<float>> tem;
				tem.first = MinY + width*i;
				Linedate.push_back(tem);
			}
			if (!Linedate.empty())     //记住，Linedate可能是空的！ 2015_6_17
			{
				if ((MaxY - Linedate[Linedate.size() - 1].first) > width * 3 / 2)//为了使得最后一根填充线不至于与轮廓离得太远，要加一个是否需要增加一条线段判断
				{

					std::pair<float, std::list<float>> tem;
					tem.first = (MaxY + Linedate[Linedate.size() - 1].first) / 2;
					Linedate.push_back(tem);
				}
				else if ((MaxY - Linedate[Linedate.size() - 1].first) <= width * 3 / 2 && (MaxY - Linedate[Linedate.size() - 1].first) > width)  //让每一条线的间距都增加一点的自适应线宽补偿
				{
					for (int i = 0; i != Linedate.size(); ++i)
					{
						Linedate[i].first += (MaxY - Linedate[Linedate.size() - 1].first - width) / Linedate.size();
					}
				}

				for (int i = 0; i != TheOutline.size(); i++)
				{

					for (int j = 1; j != TheOutline[i].size(); j++)  //j=1，说明要从第二个点开始循环，直到第一个点。
					{
						int n = (int)TheOutline[i].size() - 1;  //轮廓数据的最后一个点是第一个点，因此要减去1。
						int beforeP = (j - 1 + n) % n;
						int beforebP = (j - 2 + n) % n;
						int nextP = (j + 1 + n) % n;
						int nextnP = (j + 2 + n) % n;
						if (TheOutline[i][j].y != TheOutline[i][nextP].y) //轮廓线段不平行于x轴的情况。
						{
							for (int k = 0; k != Linedate.size(); k++)
							{
								if ((Linedate[k].first - TheOutline[i][j].y)*(Linedate[k].first - TheOutline[i][nextP].y) < 0) //线与轮廓线段相交的情况。
								{
									float x1 = TheOutline[i][j].x;
									float x2 = TheOutline[i][nextP].x;
									float y1 = TheOutline[i][j].y;
									float y2 = TheOutline[i][nextP].y;
									Linedate[k].second.push_back((x2 - x1) / (y2 - y1)*(Linedate[k].first - y1) + x1);
								}
								else if ((Linedate[k].first == TheOutline[i][j].y))    //线与轮廓线段前一个顶点相交的情况。
								{
									if ((!IsContainPoint(maxPoint, TheOutline[i][j])) && (!IsContainPoint(minPoint, TheOutline[i][j])) && (TheOutline[i][j].y != TheOutline[i][beforeP].y))
									{
										//这个顶点同时又不是极值点时。同时这个点与前一个点的y值都不相等才行，因为相等时的情况已经已经被后面的逻辑考虑过了！！！
										Linedate[k].second.push_back(TheOutline[i][j].x);
									}
								}
								else if ((Linedate[k].first == TheOutline[i][nextP].y))   //线与轮廓线段后一个顶点相交的情况。
								{
									if ((!IsContainPoint(maxPoint, TheOutline[i][nextP])) && (!IsContainPoint(minPoint, TheOutline[i][nextP])) && (TheOutline[i][nextP].y != TheOutline[i][nextnP].y))
									{
										//这个顶点同时又不是极值点时。同时后一个点与后后一个点的y值都不相等才行，因为相等时的情况已经已经被后面的逻辑考虑过了！！！
										Linedate[k].second.push_back(TheOutline[i][nextP].x);
									}
								}
							}
						}
						else if (TheOutline[i][j].y == TheOutline[i][nextP].y) //轮廓线段平行于x轴的情况。
						{
							for (int k = 0; k != Linedate.size(); k++)
							{
								if (Linedate[k].first == TheOutline[i][j].y)  //有填充线恰好与平行于x轴的轮廓线相交。
								{
									if ((IsLeft(TheOutline[i][beforeP], TheOutline[i][j], TheOutline[i][nextP]) == -1) && (IsLeft(TheOutline[i][j], TheOutline[i][nextP], TheOutline[i][nextnP]) == -1))  //这个轮廓线的两点同时是内点时。
									{   //顺时针向右转是内点。
										Linedate[k].second.push_back(TheOutline[i][j].x);
										Linedate[k].second.push_back(TheOutline[i][nextP].x);
									}
									else if ((IsLeft(TheOutline[i][beforeP], TheOutline[i][j], TheOutline[i][nextP]) == 1) && (IsLeft(TheOutline[i][j], TheOutline[i][nextP], TheOutline[i][nextnP]) == 1))
									{
										//两个点都不是内点，两点都舍去，换句话说就是什么也不做，其实不用写这段代码，为了以后好理解所以写上了。
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

				for (int i = 0; i != Linedate.size(); i++)  //将存储好的数据重复的元素删除，并且排序。
				{
					Linedate[i].second.sort();    //排序。
					Linedate[i].second.unique();  //将多余的点取出来。
				}
				DeleteOddDate(Linedate);    //暂时加上，看看情况！

				for (int i = 0; i != Linedate.size(); ++i)  //删除分区的话加上这个就好啦！同时半径补偿也可以删除！！
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

		//记得最后加上偏置轮廓。
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
		//填充线的结果转回选择角度
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

void InfillConcentric(outlines TheOutline, outlines & TheResult, outlines & TheOutlineResult, float width, int lunkuo,  float offsetWidth) //自己编写的同心填充函数
{
	if (!TheOutline.empty())  //首先必须保证有数据
	{
		//需要一次轮廓偏置来使得外轮廓比较平滑
		std::vector<outlines> dataOffsets;  //存储所有偏置轮廓的数据
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
					temData.push_back(xd::xdpoint((float)solution[i][0].X / 1000000.0, (float)solution[i][0].Y / 1000000.0));//加上最后一个点，保证封闭
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
		//记得最后加上偏置轮廓。
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

void InfillLineIn(outlines TheOutline,outlines & TheResult,float width,float degree ) //自己编写的填充线生成函数。
{
    //将轮廓数据旋转一个角度
    for (int i=0;i!=TheOutline.size();++i)
    {
        for (int j=0;j!=TheOutline[i].size();++j)
        {
            TheOutline[i][j].argument(TheOutline[i][j].argument()-degree);
        }
    }

    //第一步，轮廓点的局部极大极小值点，放在一个双向链表中。注意：选择双向链表的原因是其任意位置删除和添加元素非常快捷，降低时间复杂度。
    std::list<float> maxY;
    std::list<float> minY;
    std::vector<std::pair<float,float>> maxPoint;
    std::vector<std::pair<float,float>> minPoint;
    for (int i=0;i!=TheOutline.size();i++)
    {
        for (int j=1;j!=TheOutline[i].size();j++)
        {
            int n=(int)TheOutline[i].size()-1;  //轮廓数据的最后一个点是第一个点，因此要减去1。
            int beforeP=(j-1+n)%n;
            int beforebP=(j-2+n)%n;
            int nextP=(j+1+n)%n;
            int nextnP=(j+2+n)%n;
            if ((TheOutline[i][j].y>TheOutline[i][beforeP].y)&&(TheOutline[i][j].y>TheOutline[i][nextP].y))  //这个点的y值大于前后的点的y值。
            {
                maxY.push_back(TheOutline[i][j].y);
                std::pair<float,float> tem;
                tem.first=TheOutline[i][j].x;
                tem.second=TheOutline[i][j].y;
                maxPoint.push_back(tem);
            }
            else if ((TheOutline[i][j].y<TheOutline[i][beforeP].y)&&(TheOutline[i][j].y<TheOutline[i][nextP].y)) //这个点的y值小于前后的点的y值。
            {
                minY.push_back(TheOutline[i][j].y);
                std::pair<float,float> tem;
                tem.first=TheOutline[i][j].x;
                tem.second=TheOutline[i][j].y;
                minPoint.push_back(tem);
            }
            else if (TheOutline[i][j].y==TheOutline[i][beforeP].y)
            {
                if ((TheOutline[i][beforeP].y>TheOutline[i][beforebP].y)&&(TheOutline[i][j].y>TheOutline[i][nextP].y)) //这个点和前一个点的y值大。
                {
                    maxY.push_back(TheOutline[i][j].y);
                }
                if ((TheOutline[i][beforeP].y<TheOutline[i][beforebP].y)&&(TheOutline[i][j].y<TheOutline[i][nextP].y)) //这个点和前一个点的y值小。
                {
                    minY.push_back(TheOutline[i][j].y);
                }
            }
            else if (TheOutline[i][j].y==TheOutline[i][nextP].y)
            {
                if ((TheOutline[i][nextP].y>TheOutline[i][nextnP].y)&&(TheOutline[i][j].y>TheOutline[i][beforeP].y))  //这个点和后一个点的y值大。
                {
                    maxY.push_back(TheOutline[i][j].y);
                }
                if ((TheOutline[i][nextP].y<TheOutline[i][nextnP].y)&&(TheOutline[i][j].y<TheOutline[i][beforeP].y))  //这个点和后一个点的y值小。
                {
                    minY.push_back(TheOutline[i][j].y);
                }
            }
        }
    }
    maxY.sort();
    minY.sort();
    maxY.unique();    //将链表中重复的点删除的函数，属于STL的变易算法。
    minY.unique();	  //同上。
    float MaxY=*max_element(maxY.begin(),maxY.end());   //调用求链表中求最大元素的函数。
    float MinY=*min_element(minY.begin(),minY.end());
    //第二步，生成每一条线填充线与轮廓线的交点，分别存储在双向链表中。
    std::vector<std::pair<float,std::list<float>>> Linedate;
    for (int i=1;i<(MaxY-MinY)/width-1;i++)
    {
        std::pair<float,std::list<float>> tem;
        tem.first=MinY+width*i;
        Linedate.push_back(tem);
    }
	if(!Linedate.empty())       //2015_6_17   记住，Linedate可能是空的！！
	{
		if ((MaxY-Linedate[Linedate.size()-1].first)>width*3/2)//为了使得最后一根填充线不至于与轮廓离得太远，要加一个是否需要增加一条线段判断
		{

			std::pair<float,std::list<float>> tem;
			tem.first=(MaxY+Linedate[Linedate.size()-1].first)/2;
			Linedate.push_back(tem);
		}
		else if ((MaxY-Linedate[Linedate.size()-1].first)<=width*3/2&&(MaxY-Linedate[Linedate.size()-1].first)>width)  //让每一条线的间距都增加一点的自适应线宽补偿
		{
			for (int i=0;i!=Linedate.size();++i)
			{
				Linedate[i].first+=(MaxY-Linedate[Linedate.size()-1].first-width)/Linedate.size();
			}
		}

		for(int i=0;i!=TheOutline.size();i++)
		{

			for (int j=1;j!=TheOutline[i].size();j++)  //j=1，说明要从第二个点开始循环，直到第一个点。
			{
				int n=(int)TheOutline[i].size()-1;  //轮廓数据的最后一个点是第一个点，因此要减去1。
				int beforeP=(j-1+n)%n;
				int beforebP=(j-2+n)%n;
				int nextP=(j+1+n)%n;
				int nextnP=(j+2+n)%n;
				if (TheOutline[i][j].y!=TheOutline[i][nextP].y) //轮廓线段不平行于x轴的情况。
				{
					for (int k=0;k!=Linedate.size();k++)
					{
						if ((Linedate[k].first-TheOutline[i][j].y)*(Linedate[k].first-TheOutline[i][nextP].y)<0) //线与轮廓线段相交的情况。
						{
							float x1=TheOutline[i][j].x;
							float x2=TheOutline[i][nextP].x;
							float y1=TheOutline[i][j].y;
							float y2=TheOutline[i][nextP].y;
							Linedate[k].second.push_back((x2-x1)/(y2-y1)*(Linedate[k].first-y1)+x1);
						}
						else if ((Linedate[k].first==TheOutline[i][j].y))    //线与轮廓线段前一个顶点相交的情况。
						{
							if ((!IsContainPoint(maxPoint,TheOutline[i][j]))&&(!IsContainPoint(minPoint,TheOutline[i][j]))&&(TheOutline[i][j].y!=TheOutline[i][beforeP].y))
							{
								//这个顶点同时又不是极值点时。同时这个点与前一个点的y值都不想等才行，因为相等时的情况已经已经被后面的逻辑考虑过了！！！
								Linedate[k].second.push_back(TheOutline[i][j].x);
							}
						}
						else if ((Linedate[k].first==TheOutline[i][nextP].y))   //线与轮廓线段后一个顶点相交的情况。
						{
							if ((!IsContainPoint(maxPoint,TheOutline[i][nextP]))&&(!IsContainPoint(minPoint,TheOutline[i][nextP]))&&(TheOutline[i][nextP].y!=TheOutline[i][nextnP].y))
							{
								//这个顶点同时又不是极值点时。同时后一个点与后后一个点的y值都不想等才行，因为相等时的情况已经已经被后面的逻辑考虑过了！！！
								Linedate[k].second.push_back(TheOutline[i][nextP].x);
							}
						}
					}
				}
				else if(TheOutline[i][j].y==TheOutline[i][nextP].y) //轮廓线段平行于x轴的情况。
				{
					for (int k=0;k!=Linedate.size();k++)
					{
						if (Linedate[k].first==TheOutline[i][j].y)  //有填充线恰好与平行于x轴的轮廓线相交。
						{
							if((IsLeft(TheOutline[i][beforeP],TheOutline[i][j],TheOutline[i][nextP])==-1)&&(IsLeft(TheOutline[i][j],TheOutline[i][nextP],TheOutline[i][nextnP])==-1))  //这个轮廓线的两点同时是内点时。
							{   //顺时针向右转是内点。
								Linedate[k].second.push_back(TheOutline[i][j].x);
								Linedate[k].second.push_back(TheOutline[i][nextP].x);
							}
							else if((IsLeft(TheOutline[i][beforeP],TheOutline[i][j],TheOutline[i][nextP])==1)&&(IsLeft(TheOutline[i][j],TheOutline[i][nextP],TheOutline[i][nextnP])==1))
							{
								//两个点都不是内点，两点都舍去，换句话说就是什么也不做，其实不用写这段代码，为了以后好理解所以写上了。
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

		for(int i=0;i!=Linedate.size();i++)  //将存储好的数据重复的元素删除，并且排序。
		{
			Linedate[i].second.sort();    //排序。
			Linedate[i].second.unique();  //将多余的点取出来。
		}
		//	DeleteOddDate(Linedate);    //暂时加上，看看情况！

		//第三步，将数据分区域存放。
		int FirstNonZero;
		while(!IsEmpty(Linedate,FirstNonZero))
		{
			outline tem;
			int j=0;  //用来判断是奇数行还是偶数行的参数。
			auto firstIn=Linedate[FirstNonZero].second.begin();
			float bijiao1=*firstIn;   //第一条线的第一个点。
			tem.push_back(xdpoint(*firstIn,Linedate[FirstNonZero].first,TheOutline[0][0].z));
			tem.push_back(xdpoint(*(++firstIn),Linedate[FirstNonZero].first,TheOutline[0][0].z));
			int i=(FirstNonZero+1)%Linedate.size();  //必须保证当第一个非零数据的线刚好是最后一条线时也不会加1越界！所以要模一下！
			float bijiao2=*firstIn;  //第一条线的第二个点。
			float bijiaoY1=Linedate[FirstNonZero].first;  //第一条线得Y坐标值。
			auto tem1s=Linedate[FirstNonZero].second.begin();
			auto tem1e=tem1s;
			tem1e++;
			tem1e++;
			Linedate[FirstNonZero].second.erase(tem1s,tem1e);
			while ((!Linedate[i].second.empty()))
			{
				auto Line2first=Linedate[i].second.begin();  //第二条线的第一个点。

				float bijiaoY2=Linedate[i].first; //第二个条直线的Y坐标值。
				if (exceedExtremum(bijiaoY1,bijiaoY2,maxY,minY))
				{
					//前后两条直线如果跨越了局部极值点，也需要退出，即需要分区。
					break;
				}
				bijiaoY1=bijiaoY2;

				if(*Line2first>bijiao2)
				{
					//第二条直线的第一个点大于第一条直线的第二个点则退出，即需要分区
					break;
				}
				if (*(++Line2first)<bijiao1)   //注意：这里++必须在前，因为就算有括号，++在后的话也会比较完再++！！！
				{
					//第二条直线的第二个点小于第一条直线的第一个点则退出，即需要分区
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
   

    //增加一步，为了适应FDM的格式，每个线段要缩短一个半径补偿,但是这个函数不加补偿半径！
    //for (int i=0;i!=TheResult.size();++i)
    //{
    //	for (int j=0;j!=TheResult[i].size();++j)
    //	{

    //		if (j%4==3||j%4==0)
    //		{
    //			TheResult[i][j].x+=0.28;    //此处是默认值。

    //		}
    //		else
    //		{
    //			TheResult[i][j].x-=0.28;
    //		}
    //	}
    //}

    //填充线的结果转回选择角度
    for (int i=0;i!=TheResult.size();++i)
    {
        for (int j=0;j!=TheResult[i].size();++j)
        {
            TheResult[i][j].argument(TheResult[i][j].argument()+degree);
        }
    }
}

void InfillBMP(outlines TheOutline,std::vector<std::vector<int>> & TheResult,int piex,int size ) //自己编写的bmp生成函数。
{
    float amplificationFactor=(float)piex/size;  //定义放大倍数，140是加工极限尺寸。注意，一定要显示变为float，否则得出的是一个整数。
    //第一步，放大数据。
    for (int i=0;i!=TheOutline.size();++i)
    {
        for (int j=0;j!=TheOutline[i].size();++j)
        {
            TheOutline[i][j].x*=amplificationFactor;
            TheOutline[i][j].y*=amplificationFactor;
        }
    }
    //第二步，取得轮廓数据型心。
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
    //第三步，平移数据。
    for (int i=0;i!=TheOutline.size();++i)
    {
        for (int j=0;j!=TheOutline[i].size();++j)
        {
            TheOutline[i][j].x+=offsetX;
            TheOutline[i][j].y+=offsetY;
        }
    }
    //第四步，轮廓点的局部极大极小值点，放在一个双向链表中。注意：选择双向链表的原因是其任意位置删除和添加元素非常快捷，降低时间复杂度。
    std::list<float> maxY;
    std::list<float> minY;
    std::vector<std::pair<float,float>> maxPoint;
    std::vector<std::pair<float,float>> minPoint;
    for (int i=0;i!=TheOutline.size();i++)
    {
        for (int j=1;j!=TheOutline[i].size();j++)
        {
            int n=(int)TheOutline[i].size()-1;  //轮廓数据的最后一个点是第一个点，因此要减去1。
            int beforeP=(j-1+n)%n;
            int beforebP=(j-2+n)%n;
            int nextP=(j+1+n)%n;
            int nextnP=(j+2+n)%n;
            if ((TheOutline[i][j].y>TheOutline[i][beforeP].y)&&(TheOutline[i][j].y>TheOutline[i][nextP].y))  //这个点的y值大于前后的点的y值。
            {
                maxY.push_back(TheOutline[i][j].y);
                std::pair<float,float> tem;
                tem.first=TheOutline[i][j].x;
                tem.second=TheOutline[i][j].y;
                maxPoint.push_back(tem);
            }
            else if ((TheOutline[i][j].y<TheOutline[i][beforeP].y)&&(TheOutline[i][j].y<TheOutline[i][nextP].y)) //这个点的y值小于前后的点的y值。
            {
                minY.push_back(TheOutline[i][j].y);
                std::pair<float,float> tem;
                tem.first=TheOutline[i][j].x;
                tem.second=TheOutline[i][j].y;
                minPoint.push_back(tem);
            }
            else if (TheOutline[i][j].y==TheOutline[i][beforeP].y)
            {
                if ((TheOutline[i][beforeP].y>TheOutline[i][beforebP].y)&&(TheOutline[i][j].y>TheOutline[i][nextP].y)) //这个点和前一个点的y值大。
                {
                    maxY.push_back(TheOutline[i][j].y);
                }
                if ((TheOutline[i][beforeP].y<TheOutline[i][beforebP].y)&&(TheOutline[i][j].y<TheOutline[i][nextP].y)) //这个点和前一个点的y值小。
                {
                    minY.push_back(TheOutline[i][j].y);
                }
            }
            else if (TheOutline[i][j].y==TheOutline[i][nextP].y)
            {
                if ((TheOutline[i][nextP].y>TheOutline[i][nextnP].y)&&(TheOutline[i][j].y>TheOutline[i][beforeP].y))  //这个点和后一个点的y值大。
                {
                    maxY.push_back(TheOutline[i][j].y);
                }
                if ((TheOutline[i][nextP].y<TheOutline[i][nextnP].y)&&(TheOutline[i][j].y<TheOutline[i][beforeP].y))  //这个点和后一个点的y值小。
                {
                    minY.push_back(TheOutline[i][j].y);
                }
            }
        }
    }
    maxY.sort();
    minY.sort();
    maxY.unique();    //将链表中重复的点删除的函数，属于STL的变易算法。
    minY.unique();	  //同上。
    float MaxY=*max_element(maxY.begin(),maxY.end());   //调用求链表中求最大元素的函数。
    float MinY=*min_element(minY.begin(),minY.end());
    //第五步，生成每一条像素填充线与轮廓线的交点，分别存储在双向链表中。
    std::vector<std::pair<float,std::list<float>>> Linedate;
    for (int i=1;i<=piex;i++)
    {
        std::pair<float,std::list<float>> tem;
        tem.first=(i+i-1)/2;    //这里近似用像素的中点作为扫描线得y值
        Linedate.push_back(tem);
    }

    for (int i=0;i!=TheOutline.size();i++)
    {
        for (int j=1;j!=TheOutline[i].size();j++)  //j=1，说明要从第二个点开始循环，直到第一个点。
        {
            int n=(int)TheOutline[i].size()-1;  //轮廓数据的最后一个点是第一个点，因此要减去1。
            int beforeP=(j-1+n)%n;
            int beforebP=(j-2+n)%n;
            int nextP=(j+1+n)%n;
            int nextnP=(j+2+n)%n;
            if (TheOutline[i][j].y!=TheOutline[i][nextP].y) //轮廓线段不平行于x轴的情况。
            {
                for (int k=0;k!=Linedate.size();k++)
                {
                    if ((Linedate[k].first-TheOutline[i][j].y)*(Linedate[k].first-TheOutline[i][nextP].y)<0) //线与轮廓线段相交的情况。
                    {
                        float x1=TheOutline[i][j].x;
                        float x2=TheOutline[i][nextP].x;
                        float y1=TheOutline[i][j].y;
                        float y2=TheOutline[i][nextP].y;
                        Linedate[k].second.push_back((x2-x1)/(y2-y1)*(Linedate[k].first-y1)+x1);
                    }
                    else if ((Linedate[k].first==TheOutline[i][j].y))    //线与轮廓线段前一个顶点相交的情况。
                    {
                        if ((!IsContainPoint(maxPoint,TheOutline[i][j]))&&(!IsContainPoint(minPoint,TheOutline[i][j]))&&(TheOutline[i][j].y!=TheOutline[i][beforeP].y))
                        {
                            //这个顶点同时又不是极值点时。同时这个点与前一个点的y值都不想等才行，因为相等时的情况已经已经被后面的逻辑考虑过了！！！
                            Linedate[k].second.push_back(TheOutline[i][j].x);
                        }
                    }
                    else if ((Linedate[k].first==TheOutline[i][nextP].y))   //线与轮廓线段后一个顶点相交的情况。
                    {
                        if ((!IsContainPoint(maxPoint,TheOutline[i][nextP]))&&(!IsContainPoint(minPoint,TheOutline[i][nextP]))&&(TheOutline[i][nextP].y!=TheOutline[i][nextnP].y))
                        {
                            //这个顶点同时又不是极值点时。同时后一个点与后后一个点的y值都不想等才行，因为相等时的情况已经已经被后面的逻辑考虑过了！！！
                            Linedate[k].second.push_back(TheOutline[i][nextP].x);
                        }
                    }
                }
            }
            else if(TheOutline[i][j].y==TheOutline[i][nextP].y) //轮廓线段平行于x轴的情况。
            {
                for (int k=0;k!=Linedate.size();k++)
                {
                    if (Linedate[k].first==TheOutline[i][j].y)  //有填充线恰好与平行于x轴的轮廓线相交。
                    {
                        if((IsLeft(TheOutline[i][beforeP],TheOutline[i][j],TheOutline[i][nextP])==-1)&&(IsLeft(TheOutline[i][j],TheOutline[i][nextP],TheOutline[i][nextnP])==-1))  //这个轮廓线的两点同时是内点时。
                        {   //顺时针向右转是内点。
                            Linedate[k].second.push_back(TheOutline[i][j].x);
                            Linedate[k].second.push_back(TheOutline[i][nextP].x);
                        }
                        else if((IsLeft(TheOutline[i][beforeP],TheOutline[i][j],TheOutline[i][nextP])==1)&&(IsLeft(TheOutline[i][j],TheOutline[i][nextP],TheOutline[i][nextnP])==1))
                        {
                            //两个点都不是内点，两点都舍去，换句话说就是什么也不做，其实不用写这段代码，为了以后好理解所以写上了。
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

    for(int i=0;i!=Linedate.size();i++)  //将存储好的数据重复的元素删除，并且排序。
    {
        Linedate[i].second.sort();    //排序。
        Linedate[i].second.unique();  //将多余的点取出来。
    }
    //	DeleteOddDate(Linedate);    //暂时加上，看看情况！
    //第六步，将对于需要填充的序列号提取到结果中去。
    for (int i=0;i!=piex;++i)   //先将像素初始化为0
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

void InfillOffset(outlines theOutline,outlines & theResult,float width)  //进行偏置填充的函数。  未验证
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
		int temi=1; //每次偏置多一次的函数
		bool flag=true; //偏置是否结束的标志
		while(flag)
		{
			ClipperLib::Paths solution; 		
			temO.Execute(solution, -width*1000000*temi); 
			temi+=1;
			xd::outlines dataOffset;  //用来存储廓偏置后的轮廓数据
			for (int i=0;i!=solution.size();++i)
			{
				xd::outline temData;
				for (int j=0;j!=solution[i].size();++j)
				{
					temData.push_back(xd::xdpoint((float)solution[i][j].X/1000000.0,(float)solution[i][j].Y/1000000.0));
				}
				temData.push_back(xd::xdpoint((float)solution[i][0].X/1000000.0,(float)solution[i][0].Y/1000000.0));//加上最后一个点，保证封闭
				dataOffset.push_back(temData);
				theResult.push_back(temData);
			}			
			if (dataOffset.empty())
			{
				flag=false;  //如果偏置数据没有了，则说明不需要再次偏置了。
			}		   
		}	
	}				  	
}

void InfillOffsetIn(ClipperLib::Paths theOutline,outlines & theResult,float width)  //进行偏置填充的函数。  未验证
{
	if (!theOutline.empty())
	{
		ClipperLib::ClipperOffset temO;
		ClipperLib::CleanPolygons(theOutline,1.415);    //偏置前最好加上这句话
		ClipperLib::SimplifyPolygons(theOutline,ClipperLib::pftEvenOdd);   //很奇怪，偏置前最好保证是简单多边形，否则可能永远循环！
		temO.AddPaths(theOutline,ClipperLib::jtMiter, ClipperLib::etClosedPolygon);
		int temi=1; //每次偏置多一次的标志
		bool flag=true; //偏置是否结束的标志
		while(flag)
		{
			ClipperLib::Paths solution; 		
			temO.Execute(solution, -width*1000000*temi); 
			temi+=1;
			xd::outlines dataOffset;  //用来存储廓偏置后的轮廓数据
			for (int i=0;i!=solution.size();++i)
			{
				xd::outline temData;
				for (int j=0;j!=solution[i].size();++j)
				{
					temData.push_back(xd::xdpoint((float)solution[i][j].X/1000000.0,(float)solution[i][j].Y/1000000.0));
				}
				temData.push_back(xd::xdpoint((float)solution[i][0].X/1000000.0,(float)solution[i][0].Y/1000000.0));//加上最后一个点，保证封闭
				dataOffset.push_back(temData);
				theResult.push_back(temData);
			}			
			if (dataOffset.empty())
			{
				flag=false;  //如果偏置数据没有了，则说明不需要再次偏置了。
			}		   
		}	
	}				  	
}

void PickUpLayer(std::vector<xd::outlines> theOutline,std::vector<int> & theResult)
{	
	//首先要遍历每两层的轮廓进行比较
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
		ClipperLib::Paths solution; //存放路径结果
		clipper.Execute(ClipperLib::ctXor,solution,ClipperLib::pftEvenOdd,ClipperLib::pftEvenOdd);
		double areaSum=0;
		for (int i=0;i!=solution.size();++i)
		{
			areaSum+=ClipperLib::Area(solution[i]);
		}
		if (areaSum>100*1000000000000)   //100平方毫米
		{
			theResult.push_back(i+1);
		}
	}
}

bool IsSingleInfill(int layerNumber,std::vector<int> needInfillLayer)  //判断该填充层是否是单层填充的函数
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
	unsigned int N=theOutline.size();    //一共隔N层填充一次。
	//第一步：先进将数据储存到clipper专用的paths里	
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
	//第二步：求N层截面的交集区域
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
	//第三步：第N层填充层先和并集区域求异或，之后第N层偏置一次后和并集求异或
	std::vector<ClipperLib::Paths> offsetResult;   //存放所有偏置结果
	ClipperLib::Paths xorResult;                   //存放异或后的结果
	clipper.AddPaths(nPaths[N-1],ClipperLib::ptSubject,true);
	clipper.AddPaths(areaA,ClipperLib::ptClip,true);
	clipper.Execute(ClipperLib::ctXor, xorResult,ClipperLib::pftEvenOdd,ClipperLib::pftEvenOdd);
	clipper.Clear();
	double xorAreaFormer=0;  //存放前一次异或后的面积
	double xorAreaLater=0;   //存放后一次异或后的面积
	for (int i=0;i!=xorResult.size();++i)
	{
		xorAreaFormer+=ClipperLib::Area(xorResult[i]);
	}
	ClipperLib::ClipperOffset clipperOffset;     //进行偏置的类
	ClipperLib::Paths onceOffset;     //存放每一次偏置的结果
	clipperOffset.AddPaths(nPaths[N-1], ClipperLib::jtMiter, ClipperLib::etClosedPolygon);  //使用最简化的偏置
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
	//第四步：进行关键的循环部分，用来确定需要偏置多少次。
	while(xorAreaLater<xorAreaFormer)
	{
		offsetResult.push_back(onceOffset);   //先存储偏置路径，为了输出
		xorAreaFormer=xorAreaLater;     //前一次的面积变成后一次的面积
		xorAreaLater=0;					//后一次的面积要归零从新算
		clipperOffset.AddPaths(onceOffset, ClipperLib::jtMiter, ClipperLib::etClosedPolygon);  //使用上次偏置路径作为这次输入路径。
		onceOffset.clear();             //可以删除偏置路径了
		clipperOffset.Execute(onceOffset,-width);
		clipperOffset.Clear();
		clipper.AddPaths(onceOffset,ClipperLib::ptSubject,true);  //偏置和并集求异或
		clipper.AddPaths(areaA,ClipperLib::ptClip,true);
		xorResult.clear();
		clipper.Execute(ClipperLib::ctXor, xorResult,ClipperLib::pftEvenOdd,ClipperLib::pftEvenOdd);
		clipper.Clear();
		for (int i=0;i!=xorResult.size();++i)
		{
			xorAreaLater+=ClipperLib::Area(xorResult[i]);
		}
	}
	//第五步：最后一次的偏置没有填充，因此要使用存储偏置的最后一个轮廓数据作为多层填充区域。（先转换为普通路径，记得加上最后一个点）
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
				temOutline.push_back(xd::xdpoint(offsetResult[i][j][0].X/1000000.0,offsetResult[i][j][0].Y/1000000.0));  //加上最后一个点
				std::pair<outline,unsigned int> temPair;
				temPair.first=temOutline;
				temPair.second=1;
				temOutputOutlines.push_back(temPair);
			}
			result.push_back(temOutputOutlines);
		}
	}
	//第六步：最后需要加上填充区域的路径
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
		//结果数据结构有问题，需要使用单层数据，这样处理也有好处，分开了轮廓偏置填充和直线填充。
	}
}

void OutlinesClipperMethod(std::vector<xd::outlines> theOutline,std::vector<xd::outputOutlines> &result,float width,float shrinkDistance,float degree)
{
	unsigned int N=theOutline.size();    //一共隔N层填充一次。
	if(1==N)   //如果就是1层，那只能直接全部填充了。 还没编好这部分程序。
	{
		return; 
		assert(1!=N);
	}
	//第一步：先进将数据储存到clipper专用的paths里	
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
	//第二步：求N层截面的交集区域
	ClipperLib::Paths areaA;   //定义交集区域A
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
	//第三步：每一层（除了最后一层）和并集求布尔差运算，结果使用轮廓偏置填充法，放在对应层的填充路径里面。
	for (int i=0;i!=N-1;++i)
	{
		ClipperLib::Paths offsetOutline;
		clipper.AddPaths(nPaths[i],ClipperLib::ptSubject,true);
		clipper.AddPaths(areaA,ClipperLib::ptClip,true);
		clipper.Execute(ClipperLib::ctDifference, offsetOutline,ClipperLib::pftEvenOdd,ClipperLib::pftEvenOdd);
		xd::outlines offsetResult;
		xd::InfillOffsetIn(offsetOutline,offsetResult,width);
		xd::outputOutlines temOutputOutlines;   //存放一层的裁剪后的偏置轮廓的数据
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
	//第四步：加入最后一层的填充数据，包括单层填充和多层填充。
	ClipperLib::Paths offsetOutline;
	clipper.AddPaths(nPaths[N-1],ClipperLib::ptSubject,true);
	clipper.AddPaths(areaA,ClipperLib::ptClip,true);
	clipper.Execute(ClipperLib::ctDifference, offsetOutline,ClipperLib::pftEvenOdd,ClipperLib::pftEvenOdd);
	xd::outlines offsetResult;
	xd::InfillOffsetIn(offsetOutline,offsetResult,width);
    xd::outputOutlines temOutputOutlines;   //存放最后一层的裁剪后的偏置轮廓的数据和交集areaA的填充数据
	for (int j=0;j!=offsetResult.size();++j)
	{
		std::pair<outline,unsigned int> temPair;
		temPair.first=offsetResult[j];
		temPair.second=1;
		temOutputOutlines.push_back(temPair);
	}
	//result.push_back(temOutputOutlines);
	clipper.Clear();
//	for (int i=0;i!=areaA.size();++i) //需要使用多层填充加入区域A的轮廓路径
//	{
//		std::pair<outline,unsigned int> temPair;
//		xd::outline temOutline;
//		for(int j=0;j!=areaA[i].size();++j)
//		{
//			temOutline.push_back(xd::xdpoint(areaA[i][j].X/1000000.0,areaA[i][j].Y/1000000.0));
//		}
//		temOutline.push_back(xd::xdpoint(areaA[i][0].X/1000000.0,areaA[i][0].Y/1000000.0));   //加上最后一个点，保证封闭
//		temPair.first=temOutline;
//		temPair.second=N;
//		temOutputOutlines.push_back(temPair);
//	}
	xd::outlines outlinesIn,outlinesO;
	for (int i=0;i!=areaA.size();++i)  //需要使用多层填充加入区域A的填充路径  需要先分开，再填充！
	{
		xd::outline temPush;
		for (int j=0;j!=areaA[i].size();++j)
		{
			temPush.push_back(xd::xdpoint(areaA[i][j].X/1000000.0,areaA[i][j].Y/1000000.0));
		}
		temPush.push_back(xd::xdpoint(areaA[i][0].X/1000000.0,areaA[i][0].Y/1000000.0));  //加上最后一个点，保证封闭
		outlinesIn.push_back(temPush);
	}
    //使用直线算法前要保证一条线上没有多余点！
    for(int i=0;i!=outlinesIn.size();++i)
    {
        DealOneLayer(outlinesIn[i]);
        if(outlinesIn[i].front()!=outlinesIn[i].back())
            outlinesIn[i].push_back(outlinesIn[i].front());
    }
    xd::InfillLineSLA(outlinesIn,outlinesO,width,degree,0,shrinkDistance,width);
	for (int i=0;i!=outlinesO.size();++i)
	{
		std::pair<outline,unsigned int> temPair;
		temPair.first=outlinesO[i];
		temPair.second=N;
		temOutputOutlines.push_back(temPair);
	}
	result.push_back(temOutputOutlines);   //最后加入路径到结果
}

void SplitMNArea(outlines TheOutline,std::vector<xd::outlines> & theResult,std::vector<outlines> & dataOffsets,float width,int M,int N,int lunkuo,float overlap)
{
	if(!TheOutline.empty())  //首先必须保证有数据
	{
		//需要一次轮廓偏置来使得外轮廓比较平滑
		//std::vector<outlines> dataOffsets;  //存储所有偏置轮廓的数据
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
					temData.push_back(xd::xdpoint((float)solution[i][0].X/1000000.0,(float)solution[i][0].Y/1000000.0));//加上最后一个点，保证封闭
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
			if(((AABB.bottom-AABB.top)<10*1000000)||(AABB.right-AABB.left<10*1000000))  //如果外包围盒长或宽小于10mm，则不划分了！此处可以修改
			{
				theResult.push_back(TheOutline);
				return;
			}
			ClipperLib::cInt lineUnit=(AABB.bottom-AABB.top)/(ClipperLib::cInt)M;  //行单元的大小，注意：clipper库bottom>top! y轴向下！
			ClipperLib::cInt rowUnit=(AABB.right-AABB.left)/(ClipperLib::cInt)N;   //列单元的大小
			for(int i=0;i!=M;++i)	
			{
				for(int j=0;j!=N;++j)
				{
					//开始四行需要逻辑清晰，代表取到任意一个区域的包围盒的位置坐标
					ClipperLib::cInt Left=AABB.left+(ClipperLib::cInt)j*rowUnit;
					ClipperLib::cInt Top=AABB.bottom-(ClipperLib::cInt)i*lineUnit;   //top改为bottom!  y轴向下！
					ClipperLib::cInt Right=AABB.left+(ClipperLib::cInt)(j+1)*rowUnit;
					ClipperLib::cInt Bottom=AABB.bottom-(ClipperLib::cInt)(i+1)*lineUnit;  //top改为bottom!
					//下面需要注意重叠
					if(i!=0)
						Top+=overlap*1000000;
					if(j!=0)
						Left-=overlap*1000000;
					if(i!=M-1)
						Bottom-=overlap*1000000;
					if(j!=N-1)
						Right+=overlap*1000000;
					//下面构造裁剪小矩形
					ClipperLib::Path clipRectangle;
					clipRectangle<<ClipperLib::IntPoint(Left,Top)<<ClipperLib::IntPoint(Left,Bottom)<<ClipperLib::IntPoint(Right,Bottom)<<ClipperLib::IntPoint(Right,Top)<<ClipperLib::IntPoint(Left,Top);
					splitArea.AddPath(clipRectangle,ClipperLib::ptClip,true);					
					ClipperLib::Paths intersectionResult;
					splitArea.Execute(ClipperLib::ctIntersection, intersectionResult,ClipperLib::pftEvenOdd,ClipperLib::pftEvenOdd);  //执行布尔交					
					xd::outlines temOutlines;    					
					for(int k=0;k!=intersectionResult.size();++k)
					{
						xd::outline temOutline;
						for(int l=0;l!=intersectionResult[k].size();++l)
						{
							temOutline.push_back(xd::xdpoint(intersectionResult[k][l].X/1000000.0,intersectionResult[k][l].Y/1000000.0));
						}
						temOutline.push_back(xd::xdpoint(intersectionResult[k][0].X/1000000.0,intersectionResult[k][0].Y/1000000.0)); //加上最后一个点封闭
						temOutlines.push_back(temOutline);
					}
					theResult.push_back(temOutlines);
					splitArea.Clear();  //进行一次要清空
					splitArea.AddPaths(subjectPaths,ClipperLib::ptSubject,true);  //要再加上主多边形
				}
			}
			//到此循环M*N次，轮廓组合存放到theResult里
		}
		
	}

}

int SplitLWArea(outlines TheOutline, std::vector<xd::outlines> & theResult, std::vector<outlines> & dataOffsets, float width, int Length, int Width, int lunkuo, float overlap, float threshold)
{
	if(!TheOutline.empty())  //首先必须保证有数据
	{
		//需要一次轮廓偏置来使得外轮廓比较平滑
		//std::vector<outlines> dataOffsets;  //存储所有偏置轮廓的数据
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
					temData.push_back(xd::xdpoint((float)solution[i][0].X/1000000.0,(float)solution[i][0].Y/1000000.0));//加上最后一个点，保证封闭
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
			//if(((AABB.bottom-AABB.top)<10*1000000)||((AABB.right-AABB.left)<10*1000000))  //如果外包围盒长或宽小于10mm，则不划分了！此处可以修改
			//{
			//	theResult.push_back(TheOutline);
			//	return;
			//}

			ClipperLib::cInt lineUnit=Width*1000000;  //行单元的大小，注意：clipper库bottom>top! y轴向下！
			ClipperLib::cInt rowUnit=Length*1000000;   //列单元的大小
			int M=(AABB.bottom-AABB.top)/lineUnit;
			int N=(AABB.right-AABB.left)/rowUnit;
			//下面如果剩余区域大于给定长宽一半，则再分一次，这里以后可以根据需要修改距离
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
					//开始四行需要逻辑清晰，代表取到任意一个区域的包围盒的位置坐标
					ClipperLib::cInt Left=AABB.left+(ClipperLib::cInt)j*rowUnit;
					ClipperLib::cInt Top=AABB.bottom-(ClipperLib::cInt)i*lineUnit;   //top改为bottom!  y轴向下！
					ClipperLib::cInt Right=AABB.left+(ClipperLib::cInt)(j+1)*rowUnit;
					ClipperLib::cInt Bottom=AABB.bottom-(ClipperLib::cInt)(i+1)*lineUnit;  //top改为bottom!
					//下面需要注意重叠
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
					//下面构造裁剪小矩形
					ClipperLib::Path clipRectangle;
					clipRectangle<<ClipperLib::IntPoint(Left,Top)<<ClipperLib::IntPoint(Left,Bottom)<<ClipperLib::IntPoint(Right,Bottom)<<ClipperLib::IntPoint(Right,Top)<<ClipperLib::IntPoint(Left,Top);
					splitArea.AddPath(clipRectangle,ClipperLib::ptClip,true);					
					ClipperLib::Paths intersectionResult;
					splitArea.Execute(ClipperLib::ctIntersection, intersectionResult,ClipperLib::pftEvenOdd,ClipperLib::pftEvenOdd);  //执行布尔交					
					xd::outlines temOutlines;    					
					for(int k=0;k!=intersectionResult.size();++k)
					{
						xd::outline temOutline;
						for(int l=0;l!=intersectionResult[k].size();++l)
						{
							temOutline.push_back(xd::xdpoint(intersectionResult[k][l].X/1000000.0,intersectionResult[k][l].Y/1000000.0));
						}
						temOutline.push_back(xd::xdpoint(intersectionResult[k][0].X/1000000.0,intersectionResult[k][0].Y/1000000.0)); //加上最后一个点封闭
						temOutlines.push_back(temOutline);
					}
					theResult.push_back(temOutlines);
					splitArea.Clear();  //进行一次要清空
					splitArea.AddPaths(subjectPaths,ClipperLib::ptSubject,true);  //要再加上主多边形
				}
			}
			//到此循环M*N次，轮廓组合存放到theResult里
			return M;
		}
		return 0;
	}
	return 0;
}

void offsetReturnSingleRegion(outlines TheOutline,std::vector<xd::outlines> & theResult,std::vector<outlines> & dataOffsets,float width,int lunkuo)
{
	if(!TheOutline.empty())  //首先必须保证有数据
	{
		//需要一次轮廓偏置来使得外轮廓比较平滑
		//std::vector<outlines> dataOffsets;  //存储所有偏置轮廓的数据
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
					temData.push_back(xd::xdpoint((float)solution[i][0].X/1000000.0,(float)solution[i][0].Y/1000000.0));//加上最后一个点，保证封闭
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
			//这里用法值得学习，将clipper里的paths变为polytree的方法
			ClipperLib::Paths input;
			OutlinesToClipperPaths(TheOutline,&input);
			clipper.AddPaths(input, ClipperLib::ptSubject, true);
			ClipperLib::PolyTree polytree;
			clipper.Execute(ClipperLib::ctUnion, polytree, ClipperLib::pftEvenOdd, ClipperLib::pftEvenOdd);  // offset results work with both EvenOdd and NonZero
			//开始将轮廓按照区域分别赋值给结果
			theResult.clear();
			for(int i=0;i!=polytree.ChildCount();++i)
				AddOuterPolyNodeToResult(*polytree.Childs[i], theResult);   //使用递归函数遍历多叉树，使用的是前序遍历。
		}
	}
}

void offsetReturnSingleRegion(outlines TheOutline,std::vector<outlines> & theResult)
{	
	if(!TheOutline.empty()) //首先保证有数据
	{
		ClipperLib::Clipper clipper;
		clipper.Clear();    
		//这里用法值得学习，将clipper里的paths变为polytree的方法
		ClipperLib::Paths input;
		OutlinesToClipperPaths(TheOutline,&input);
		clipper.AddPaths(input, ClipperLib::ptSubject, true);
		ClipperLib::PolyTree polytree;
		clipper.Execute(ClipperLib::ctUnion, polytree, ClipperLib::pftEvenOdd, ClipperLib::pftEvenOdd);  // offset results work with both EvenOdd and NonZero
		//开始将轮廓按照区域分别赋值给结果
		theResult.clear();
		for(int i=0;i!=polytree.ChildCount();++i)
			AddOuterPolyNodeToResult(*polytree.Childs[i], theResult);   //使用递归函数遍历多叉树，使用的是前序遍历。
	}		
}

void AddOuterPolyNodeToResult(ClipperLib::PolyNode& polynode,std::vector<xd::outlines> & output)   
{	
	int lastSubscript=output.size();
	output.resize(lastSubscript + 1);  //这一步resize后即在后面加上一个（带洞）多边形
	output[lastSubscript].resize(1+polynode.ChildCount());   //这一步重置使得带洞多边形一共有几个轮廓的数
	ClipperPathToOutline(polynode.Contour, & output[lastSubscript][0]);  //这里加上多边形的轮廓	
	if(output[lastSubscript][0][0]!=output[lastSubscript][0][output[lastSubscript][0].size()-1])  //这里为了使得多边形最后一个点等于第一个点
		output[lastSubscript][0].push_back(output[lastSubscript][0][0]);
	for (int i = 0; i < polynode.ChildCount(); ++i)
	{
		ClipperPathToOutline(polynode.Childs[i]->Contour, & output[lastSubscript][1+i]);  //这里加上多边形可能出现的n个洞
		if(output[lastSubscript][1+i][0]!=output[lastSubscript][1+i][output[lastSubscript][1+i].size()-1]) //这里为了使得多边形最后一个点等于第一个点
			output[lastSubscript][1+i].push_back(output[lastSubscript][1+i][0]);
		for (int j = 0; j < polynode.Childs[i]->ChildCount(); ++j)   //有几个子，就会有几个（带洞）多边形
			AddOuterPolyNodeToResult(*polynode.Childs[i]->Childs[j], output);  //这一步使用递归
	}
}

}
