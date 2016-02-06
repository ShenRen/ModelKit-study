#include "self_recognition.h"


bool PanDuan(int a, std::vector<int> b)  //用了判断内部直骨架半边结构是否已经使用过的函数。
{
	if (b.size()==0)
	{
		return true;
	} 
	else
	{
		for (int i=0;i!=b.size();i++)
		{
			if (a==b[i])
			{
				return false;
			}
		}
	}
	return true;
}

bool isRepetition(const xd::outlines & outL,const xd::outline & line ) //用来判断这根线是否已经在外轮廓中扫描过了的函数。
{
	for (int i=0;i!=outL.size();++i)
	{
		for (int j=0;j!=outL[i].size();++j)
		{
			if (line[0].x==outL[i][j].x&&line[0].y==outL[i][j].y)  //如果某一点等于填充line的其中一点
			{
				if (j==0)
				{
					if ((line[1].x==outL[i].back().x&&line[1].y==outL[i].back().y)||(line[1].x==outL[i][j+1].x&&line[1].y==outL[i][j+1].y))
					{
						return true;
					}
				}
				else if (j==outL[i].size()-1)
				{
					if ((line[1].x==outL[i][j-1].x&&line[1].y==outL[i][j-1].y)||(line[1].x==outL[i].front().x&&line[1].y==outL[i].front().y))
					{
						return true;
					}
				}
				else
				{
					if ((line[1].x==outL[i][j-1].x&&line[1].y==outL[i][j-1].y)||(line[1].x==outL[i][j+1].x&&line[1].y==outL[i][j+1].y))
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool IsContain(const xd::outlines &Contours,const xd::xdpoint &P)
{
	for (int i=0;i!=Contours.size();++i)
	{
		for(int j=0;j!=Contours[i].size();++j)
		{
			if (Contours[i][j]==P)
			{
				return true;
			}
		}
	}
	return false;
}

void self_identifying(xd::outlines TheOutline,xd::outlines & TheResult,double width,double stagger,float degree)
{
	if (TheOutline.empty())
		return;
	//将轮廓数据旋转一个角度
	for (int i=0;i!=TheOutline.size();++i)
	{
		for (int j=0;j!=TheOutline[i].size();++j)
		{
			TheOutline[i][j].argument(TheOutline[i][j].argument()-degree);
		}
	}
	//设置各个阈值，自己定义
	double SheZhiKuanDu=25;
	double SheZhiChangDu=30;
	double SheZhiJiaoDu=30;   //0~90的范围内。

	Polygon A;
	for (int i=0;i!=TheOutline[0].size()-1;++i)  //CGAL添加多边形数据的时候不要加上和第一个点重复的最后一个点
	{
		A.push_back(Point(TheOutline[0][i].x,TheOutline[0][i].y));
	}
	Polygon_with_holes C(A);    //这一步产生一个带有洞的多边形，分区域后的多边形第一个轮廓是外轮廓。
	if (TheOutline.size()>1)
	{
		for (int i=1;i!=TheOutline.size();++i)
		{
			Polygon B;
			for (int j=0;j!=TheOutline[i].size()-1;++j)
			{
				B.push_back(Point(TheOutline[i][j].x,TheOutline[i][j].y));
			}
			C.add_hole(B);
		}					
	}
	//print_polygon_with_holes(C);
	SsPtr iss = CGAL::create_interior_straight_skeleton_2(C);  //生成直骨架

	Halfedge_handle HEH;
	Pgn_container RemovePolygon;   //typedef std::vector<std::pair<Polygon,double>>     Pgn_container;

	std::vector<int> PanduanInt;   //用来存储用过的半边结构的id号
	for (HEH=iss->halfedges_begin();HEH!=iss->halfedges_end();++HEH)
	{
		if (HEH->is_inner_bisector()&&PanDuan(HEH->id(),PanduanInt))
		{
			PanduanInt.push_back(HEH->id());
			PanduanInt.push_back(HEH->opposite()->id());
			double inner_bisector_x1=HEH->vertex()->point().x();
			double inner_bisector_y1=HEH->vertex()->point().y();
			double inner_bisector_x2=HEH->opposite()->vertex()->point().x();
			double inner_bisector_y2=HEH->opposite()->vertex()->point().y();
			double Line_Degree=std::atan2(inner_bisector_y2-inner_bisector_y1,inner_bisector_x2-inner_bisector_x1);
			double Line_Length=std::sqrt(std::pow(inner_bisector_x1-inner_bisector_x2,2)+std::pow(inner_bisector_y1-inner_bisector_y2,2));			
			if (std::abs(Line_Degree)>SheZhiJiaoDu/180*xd::pi&&std::abs(Line_Degree)<xd::pi-SheZhiJiaoDu/180*xd::pi&&Line_Length>10) //先判断(长度和)角度是否满足要求。(&&Line_Length>SheZhiChangDu)
			{
				double border_x1=HEH->defining_contour_edge()->vertex()->point().x();
				double border_y1=HEH->defining_contour_edge()->vertex()->point().y();
				double border_x2=HEH->opposite()->defining_contour_edge()->opposite()->vertex()->point().x();
				double border_y2=HEH->opposite()->defining_contour_edge()->opposite()->vertex()->point().y();
				double border_x3=HEH->opposite()->defining_contour_edge()->vertex()->point().x();
				double border_y3=HEH->opposite()->defining_contour_edge()->vertex()->point().y();
				double border_x4=HEH->defining_contour_edge()->opposite()->vertex()->point().x();
				double border_y4=HEH->defining_contour_edge()->opposite()->vertex()->point().y();
				//std::cout<<border_x1<<" "<<border_y1<<" "<<border_x2<<" "" "<<border_y2<<" "<<border_x3<<" "<<border_y3<<" "<<border_x4<<" "<<border_y4<<std::endl;
				double d1=std::abs(std::sqrt(std::pow(border_x1,2)+std::pow(border_y1,2))*std::sin(std::atan2(border_y1,border_x1)-Line_Degree)-std::sqrt(std::pow(inner_bisector_x2,2)+std::pow(inner_bisector_y2,2))*std::sin(std::atan2(inner_bisector_y2,inner_bisector_x2)-Line_Degree));
				double d2=std::abs(std::sqrt(std::pow(border_x2,2)+std::pow(border_y2,2))*std::sin(std::atan2(border_y2,border_x2)-Line_Degree)-std::sqrt(std::pow(inner_bisector_x2,2)+std::pow(inner_bisector_y2,2))*std::sin(std::atan2(inner_bisector_y2,inner_bisector_x2)-Line_Degree));
				double d3=std::abs(std::sqrt(std::pow(border_x3,2)+std::pow(border_y3,2))*std::sin(std::atan2(border_y3,border_x3)-Line_Degree)-std::sqrt(std::pow(inner_bisector_x2,2)+std::pow(inner_bisector_y2,2))*std::sin(std::atan2(inner_bisector_y2,inner_bisector_x2)-Line_Degree));
				double d4=std::abs(std::sqrt(std::pow(border_x4,2)+std::pow(border_y4,2))*std::sin(std::atan2(border_y4,border_x4)-Line_Degree)-std::sqrt(std::pow(inner_bisector_x2,2)+std::pow(inner_bisector_y2,2))*std::sin(std::atan2(inner_bisector_y2,inner_bisector_x2)-Line_Degree));
				//std::cout<<d1<<" "<<d2<<" "<<d3<<" "<<d4<<std::endl;
				double D=d1;
				if (D<d2)
				{
					D=d2;
				}
				if (D<d3)
				{
					D=d3;
				}
				if (D<d4)
				{
					D=d4;
				}
				if (Line_Length/(D*2)>2)   //再判断长细比是否大于一个值。
				{
					std::pair<Polygon,double> tem;
					Polygon temP;
					temP.push_back(Point(border_x1,border_y1));
					temP.push_back(Point(border_x2,border_y2));
					temP.push_back(Point(border_x3,border_y3));
					temP.push_back(Point(border_x4,border_y4));
					tem.first=temP;
					tem.second=Line_Degree;
					RemovePolygon.push_back(tem);
				}
			}
			//std::cout<<Line_Degree<<"  "<<Line_Length<<std::endl;
		}		
	}
	std::cout<<"挑出了"<<RemovePolygon.size()<<"个小四边形"<<std::endl;
	if (RemovePolygon.size()==0)
	{
        xd::InfillLine(TheOutline,TheResult,width,0.0,0,0.1,width);
	} 
	else
	{
		//for (int i=0;i!=RemovePolygon.size();++i)   //这一步是把识别后的小四边形的轮廓加入的简单实现
		//{
		//	xd::outlines TemI;
		//	xd::outline temR,temX1,temX2;
		//	for (auto j=RemovePolygon[i].first.vertices_begin(); j!= RemovePolygon[i].first.vertices_end(); ++j)
		//	{			
		//		temR.push_back(xd::xdpoint(j->x(),j->y()));
		//	}
		//	if ((std::pow(temR[0].x-temR[1].x,2)+std::pow(temR[0].y-temR[1].y,2))>(std::pow(temR[1].x-temR[2].x,2)+std::pow(temR[1].y-temR[2].y,2)))
		//	{
		//		temX1.push_back(temR[1]);
		//		temX1.push_back(temR[2]);
		//		temX2.push_back(temR[3]);
		//		temX2.push_back(temR[0]);
		//	} 
		//	else
		//	{
		//		temX1.push_back(temR[0]);
		//		temX1.push_back(temR[1]);
		//		temX2.push_back(temR[2]);
		//		temX2.push_back(temR[3]);
		//	}
		//	if (!isRepetition(TheOutline,temX1))
		//	{
		//		TemI.push_back(temX1);
		//	}
		//	if (!isRepetition(TheOutline,temX2))
		//	{
		//		TemI.push_back(temX2);
		//	}			
		//	for (int j=0;j!=TemI.size();++j)
		//	{
		//		TheResult.push_back(TemI[j]);
		//	}
		//}		
		
		//方案二：用clipper库进行裁剪运算，没有错误
		ClipperLib::Paths totalOutlines;
		ClipperLib::Paths littleRectangles;
		xd::OutlinesToClipperPaths(TheOutline,& totalOutlines);
		for (int i=0;i!=RemovePolygon.size();++i)
		{
			ClipperLib::Path SR;
			for (auto j=RemovePolygon[i].first.vertices_begin(); j!= RemovePolygon[i].first.vertices_end(); ++j)
			{
				SR.push_back(ClipperLib::IntPoint(j->x()*1000000,j->y()*1000000));
			}
			littleRectangles.push_back(SR);
		}
		ClipperLib::ClipperOffset offsetLittleRectangle;
		offsetLittleRectangle.AddPaths(littleRectangles,ClipperLib::jtSquare, ClipperLib::etClosedPolygon);
		littleRectangles.clear();
		
		offsetLittleRectangle.Execute(littleRectangles, stagger*1000000);  //暂时使用扩大staggermm的方案 
		
		ClipperLib::Clipper splitArea;
		splitArea.AddPaths(totalOutlines,ClipperLib::ptSubject,true); //被裁剪的主多边形
		splitArea.AddPaths(littleRectangles,ClipperLib::ptClip,true); //进行裁剪的小矩形
		ClipperLib::Paths intersectionResult; //存放clipper结果
		xd::outlines xdResult;  //标准xd结果
		splitArea.Execute(ClipperLib::ctDifference, intersectionResult,ClipperLib::pftEvenOdd,ClipperLib::pftEvenOdd);  //执行布尔差
		xd::ClipperPathsToOutlines(intersectionResult,& xdResult);
		for (int i=0;i!=xdResult.size();++i)  //记得加上最后一个点！
		{
			if (xdResult[i][0]!=xdResult[i][xdResult[i].size()-1])
			{
				xdResult[i].push_back(xdResult[i][0]);
			}
		}
		xd::outlines infillResult;  //存放普通直线填充结果
        xd::InfillLine(xdResult,infillResult,width,0,0,0.1,width);
		for (int i=0;i!=infillResult.size();++i)
		{
			TheResult.push_back(infillResult[i]);
		}
		
		//下面是把小矩形的填充路径加上的程序
		offsetLittleRectangle.Clear();
		splitArea.Clear();
		for (int i=0;i!=RemovePolygon.size();++i)
		{
			xd::outlines TemI,TemO;
			xd::outline temR;
			for (auto j=RemovePolygon[i].first.vertices_begin(); j!= RemovePolygon[i].first.vertices_end(); ++j)
			{			
				temR.push_back(xd::xdpoint(j->x(),j->y()));
			}
			ClipperLib::Path temoffset;
			ClipperLib::Paths solution;
			xd::OutlineToClipperPath(temR,&temoffset);
			offsetLittleRectangle.AddPath(temoffset,ClipperLib::jtSquare, ClipperLib::etClosedPolygon);
			offsetLittleRectangle.Execute(solution, stagger*1000000);  //暂时使用扩大2mm的方案 
			splitArea.AddPaths(totalOutlines,ClipperLib::ptSubject,true); //被裁剪的主多边形
			splitArea.AddPaths(solution,ClipperLib::ptClip,true); //进行裁剪的小矩形
			solution.clear();
			splitArea.Execute(ClipperLib::ctIntersection, solution,ClipperLib::pftEvenOdd,ClipperLib::pftEvenOdd);  //执行布尔交
			xd::ClipperPathsToOutlines(solution,&TemI);	
			for (int j=0;j!=TemI.size();++j)
			{
				if (TemI[j].front()!=TemI[j].back())
				{
					TemI[j].push_back(TemI[j].front());
				}
			}
			assert(TemI.size()==1);  //相交后只有一个轮廓，两个以上还无法处理
			//temI代表放大后小矩形和原轮廓相交的多边形
			xd::outline AddLine;
			if(stagger!=0)
			{
				int startI=0;
				for (int j=0;j!=TemI[0].size()-1;++j)
				{
					if (!IsContain(TheOutline,TemI[0][j])&&!IsContain(TheOutline,TemI[0][j+1]))
					{
						AddLine.push_back(TemI[0][j]);
						AddLine.push_back(TemI[0][j+1]);
						startI=j;
						break;
					}
				}
				for (int j=startI+1;j!=TemI[0].size()-1;++j)  //j=startI+1,因为startI在上一次已经循环过了
				{
					if (!IsContain(TheOutline,TemI[0][j+1]))
					{
						AddLine.push_back(TemI[0][j+1]);
					}
					else
						break;
				}
				if (startI==0)   //只有第一条线段是需要增加的线时，才需要往前循环
				{
					for (int j=TemI[0].size()-1;j!=0;--j)    //有可能前面还有轮廓
					{
						if (!IsContain(TheOutline,TemI[0][j-1]))
						{
							AddLine.insert(AddLine.begin(),TemI[0][j-1]);
						}
						else
							break;
					}
				}
			}
			else
			{
				xd::outline aa,bb,cc,dd;
				aa.push_back(temR[0]);aa.push_back(temR[1]);
				bb.push_back(temR[1]);bb.push_back(temR[2]);
				cc.push_back(temR[2]);cc.push_back(temR[3]);
				dd.push_back(temR[3]);dd.push_back(temR[0]);
				if (!isRepetition(TheOutline,aa))
					AddLine.insert(AddLine.begin(),aa.begin(),aa.end());
				if (!isRepetition(TheOutline,bb))
					AddLine.insert(AddLine.begin(),bb.begin(),bb.end());
				if (!isRepetition(TheOutline,cc))
					AddLine.insert(AddLine.begin(),cc.begin(),cc.end());
				if (!isRepetition(TheOutline,dd))
					AddLine.insert(AddLine.begin(),dd.begin(),dd.end());
			}
			TheResult.push_back(AddLine);
            xd::InfillLine(TemI,TemO,width,xd::degrees(RemovePolygon[i].second),0,0.1,width);
			for (int j=0;j!=TemO.size();++j)
			{
				TheResult.push_back(TemO[j]);
			}
			splitArea.Clear();
			offsetLittleRectangle.Clear();
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

void self_identifying_A(xd::outlines TheOutline,xd::outlines & TheResult,double width,double stagger)
{
	//设置各个阈值，自己定义
	double SheZhiKuanDu=25;
	double SheZhiChangDu=30;
	double SheZhiJiaoDu=30;   //0~90的范围内。

	Polygon A;
	for (int i=0;i!=TheOutline[0].size()-1;++i)  //CGAL添加多边形数据的时候不要加上和第一个点重复的最后一个点
	{
		A.push_back(Point(TheOutline[0][i].x,TheOutline[0][i].y));
	}
	Polygon_with_holes C(A);    //这一步产生一个带有洞的多边形，分区域后的多边形第一个轮廓是外轮廓。
	if (TheOutline.size()>1)
	{
		for (int i=1;i!=TheOutline.size();++i)
		{
			Polygon B;
			for (int j=0;j!=TheOutline[i].size()-1;++j)
			{
				B.push_back(Point(TheOutline[i][j].x,TheOutline[i][j].y));
			}
			C.add_hole(B);
		}					
	}
	//print_polygon_with_holes(C);
	SsPtr iss = CGAL::create_interior_straight_skeleton_2(C);  //生成直骨架

	Halfedge_handle HEH;
	Pgn_container RemovePolygon;   //typedef std::vector<std::pair<Polygon,double>>     Pgn_container;

	std::vector<int> PanduanInt;   //用来存储用过的半边结构的id号
	for (HEH=iss->halfedges_begin();HEH!=iss->halfedges_end();++HEH)
	{
		if (HEH->is_inner_bisector()&&PanDuan(HEH->id(),PanduanInt))
		{
			PanduanInt.push_back(HEH->id());
			PanduanInt.push_back(HEH->opposite()->id());
			double inner_bisector_x1=HEH->vertex()->point().x();
			double inner_bisector_y1=HEH->vertex()->point().y();
			double inner_bisector_x2=HEH->opposite()->vertex()->point().x();
			double inner_bisector_y2=HEH->opposite()->vertex()->point().y();
			double Line_Degree=std::atan2(inner_bisector_y2-inner_bisector_y1,inner_bisector_x2-inner_bisector_x1);
			double Line_Length=std::sqrt(std::pow(inner_bisector_x1-inner_bisector_x2,2)+std::pow(inner_bisector_y1-inner_bisector_y2,2));			
			if (std::abs(Line_Degree)>SheZhiJiaoDu/180*xd::pi&&std::abs(Line_Degree)<xd::pi-SheZhiJiaoDu/180*xd::pi&&Line_Length>10) //先判断(长度和)角度是否满足要求。(&&Line_Length>SheZhiChangDu)
			{
				double border_x1=HEH->defining_contour_edge()->vertex()->point().x();
				double border_y1=HEH->defining_contour_edge()->vertex()->point().y();
				double border_x2=HEH->opposite()->defining_contour_edge()->opposite()->vertex()->point().x();
				double border_y2=HEH->opposite()->defining_contour_edge()->opposite()->vertex()->point().y();
				double border_x3=HEH->opposite()->defining_contour_edge()->vertex()->point().x();
				double border_y3=HEH->opposite()->defining_contour_edge()->vertex()->point().y();
				double border_x4=HEH->defining_contour_edge()->opposite()->vertex()->point().x();
				double border_y4=HEH->defining_contour_edge()->opposite()->vertex()->point().y();
				//std::cout<<border_x1<<" "<<border_y1<<" "<<border_x2<<" "" "<<border_y2<<" "<<border_x3<<" "<<border_y3<<" "<<border_x4<<" "<<border_y4<<std::endl;
				double d1=std::abs(std::sqrt(std::pow(border_x1,2)+std::pow(border_y1,2))*std::sin(std::atan2(border_y1,border_x1)-Line_Degree)-std::sqrt(std::pow(inner_bisector_x2,2)+std::pow(inner_bisector_y2,2))*std::sin(std::atan2(inner_bisector_y2,inner_bisector_x2)-Line_Degree));
				double d2=std::abs(std::sqrt(std::pow(border_x2,2)+std::pow(border_y2,2))*std::sin(std::atan2(border_y2,border_x2)-Line_Degree)-std::sqrt(std::pow(inner_bisector_x2,2)+std::pow(inner_bisector_y2,2))*std::sin(std::atan2(inner_bisector_y2,inner_bisector_x2)-Line_Degree));
				double d3=std::abs(std::sqrt(std::pow(border_x3,2)+std::pow(border_y3,2))*std::sin(std::atan2(border_y3,border_x3)-Line_Degree)-std::sqrt(std::pow(inner_bisector_x2,2)+std::pow(inner_bisector_y2,2))*std::sin(std::atan2(inner_bisector_y2,inner_bisector_x2)-Line_Degree));
				double d4=std::abs(std::sqrt(std::pow(border_x4,2)+std::pow(border_y4,2))*std::sin(std::atan2(border_y4,border_x4)-Line_Degree)-std::sqrt(std::pow(inner_bisector_x2,2)+std::pow(inner_bisector_y2,2))*std::sin(std::atan2(inner_bisector_y2,inner_bisector_x2)-Line_Degree));
				//std::cout<<d1<<" "<<d2<<" "<<d3<<" "<<d4<<std::endl;
				double D=d1;
				if (D<d2)
				{
					D=d2;
				}
				if (D<d3)
				{
					D=d3;
				}
				if (D<d4)
				{
					D=d4;
				}
				if (Line_Length/(D*2)>2)   //再判断长细比是否大于一个值。
				{
					std::pair<Polygon,double> tem;
					Polygon temP;
					temP.push_back(Point(border_x1,border_y1));
					temP.push_back(Point(border_x2,border_y2));
					temP.push_back(Point(border_x3,border_y3));
					temP.push_back(Point(border_x4,border_y4));
					tem.first=temP;
					tem.second=Line_Degree;
					RemovePolygon.push_back(tem);
				}
			}
			//std::cout<<Line_Degree<<"  "<<Line_Length<<std::endl;
		}		
	}
	std::cout<<"挑出了"<<RemovePolygon.size()<<"个小四边形"<<std::endl;
	if (RemovePolygon.size()==0)
	{
        xd::InfillLine(TheOutline,TheResult,width,0,1,0.1,width);
	} 
	else
	{
		//for (int i=0;i!=RemovePolygon.size();++i)   //这一步是把识别后的小四边形的轮廓加入的简单实现
		//{
		//	xd::outlines TemI;
		//	xd::outline temR,temX1,temX2;
		//	for (auto j=RemovePolygon[i].first.vertices_begin(); j!= RemovePolygon[i].first.vertices_end(); ++j)
		//	{			
		//		temR.push_back(xd::xdpoint(j->x(),j->y()));
		//	}
		//	if ((std::pow(temR[0].x-temR[1].x,2)+std::pow(temR[0].y-temR[1].y,2))>(std::pow(temR[1].x-temR[2].x,2)+std::pow(temR[1].y-temR[2].y,2)))
		//	{
		//		temX1.push_back(temR[1]);
		//		temX1.push_back(temR[2]);
		//		temX2.push_back(temR[3]);
		//		temX2.push_back(temR[0]);
		//	} 
		//	else
		//	{
		//		temX1.push_back(temR[0]);
		//		temX1.push_back(temR[1]);
		//		temX2.push_back(temR[2]);
		//		temX2.push_back(temR[3]);
		//	}
		//	if (!isRepetition(TheOutline,temX1))
		//	{
		//		TemI.push_back(temX1);
		//	}
		//	if (!isRepetition(TheOutline,temX2))
		//	{
		//		TemI.push_back(temX2);
		//	}			
		//	for (int j=0;j!=TemI.size();++j)
		//	{
		//		TheResult.push_back(TemI[j]);
		//	}
		//}		

		//方案二：用clipper库进行裁剪运算，没有错误
		ClipperLib::Paths totalOutlines;
		ClipperLib::Paths littleRectangles;
		xd::OutlinesToClipperPaths(TheOutline,& totalOutlines);
		for (int i=0;i!=RemovePolygon.size();++i)
		{
			ClipperLib::Path SR;
			for (auto j=RemovePolygon[i].first.vertices_begin(); j!= RemovePolygon[i].first.vertices_end(); ++j)
			{
				SR.push_back(ClipperLib::IntPoint(j->x()*1000000,j->y()*1000000));
			}
			littleRectangles.push_back(SR);
		}
		ClipperLib::Clipper splitArea;
		splitArea.AddPaths(totalOutlines,ClipperLib::ptSubject,true); //被裁剪的主多边形
		splitArea.AddPaths(littleRectangles,ClipperLib::ptClip,true); //进行裁剪的小矩形
		ClipperLib::Paths intersectionResult; //存放clipper结果
		xd::outlines xdResult;  //标准xd结果
		splitArea.Execute(ClipperLib::ctDifference, intersectionResult,ClipperLib::pftEvenOdd,ClipperLib::pftEvenOdd);  //执行布尔差
		ClipperLib::ClipperOffset offsetInter;
		offsetInter.AddPaths(intersectionResult,ClipperLib::jtSquare, ClipperLib::etClosedPolygon);
		ClipperLib::Paths solution;  //solution后面还要用
		offsetInter.Execute(solution, stagger*1000000);  //暂时使用扩大2mm的方案 
		splitArea.Clear();
		splitArea.AddPaths(totalOutlines,ClipperLib::ptSubject,true); //被裁剪的主多边形
		splitArea.AddPaths(solution,ClipperLib::ptClip,true); //进行布尔交的扩大后的多边形轮廓
		intersectionResult.clear();
		splitArea.Execute(ClipperLib::ctIntersection, intersectionResult,ClipperLib::pftEvenOdd,ClipperLib::pftEvenOdd);  //执行布尔交
		xd::ClipperPathsToOutlines(intersectionResult,& xdResult);
		for (int i=0;i!=xdResult.size();++i)  //记得加上最后一个点！
		{
			if (xdResult[i][0]!=xdResult[i][xdResult[i].size()-1])
			{
				xdResult[i].push_back(xdResult[i][0]);
			}
		}
		xd::outlines infillResult;  //存放普通直线填充结果
        xd::InfillLine(xdResult,infillResult,width,0,0,0.1,width);
		for (int i=0;i!=infillResult.size();++i)
		{
			TheResult.push_back(infillResult[i]);
		}

		//下面是把小矩形的填充路径加上的程序
		
		splitArea.Clear();
		for (int i=0;i!=RemovePolygon.size();++i)
		{
			xd::outlines TemI,TemO;
			xd::outline temR;
			for (auto j=RemovePolygon[i].first.vertices_begin(); j!= RemovePolygon[i].first.vertices_end(); ++j)
			{			
				temR.push_back(xd::xdpoint(j->x(),j->y()));
			}
			ClipperLib::Path temSubject;
			xd::OutlineToClipperPath(temR,&temSubject);
			splitArea.AddPath(temSubject,ClipperLib::ptSubject,true);  //被裁剪的小多边形
			splitArea.AddPaths(solution,ClipperLib::ptClip,true); //进行裁剪的整体
			ClipperLib::Paths temLittleRectangle;
			splitArea.Execute(ClipperLib::ctDifference, temLittleRectangle,ClipperLib::pftEvenOdd,ClipperLib::pftEvenOdd);  //执行布尔差
			xd::ClipperPathsToOutlines(temLittleRectangle,&TemI);
			//现在temR代表原来的小多边形，temI代表缩小后的多边形，都是4个点，需要挑出
			//assert(TemI.size()==1);   
			//assert(TemI[0].size()==4);
			assert(temR.size()==4);
			if (TemI.size()==1)   //有可能TemI里面没有任何东西，因为已经缩小到没有了
			{
				if (stagger!=0)
				{				
					xd::outline aa,bb,cc,dd;				
					aa.push_back(TemI[0][0]);
					aa.push_back(TemI[0][1]);
					bb.push_back(TemI[0][2]);
					bb.push_back(TemI[0][3]);				
					cc.push_back(TemI[0][1]);
					cc.push_back(TemI[0][2]);
					dd.push_back(TemI[0][3]);
					dd.push_back(TemI[0][0]);				
					if (aa[0]!=temR[0]&&aa[0]!=temR[1]&&aa[0]!=temR[2]&&aa[0]!=temR[3]&&aa[1]!=temR[0]&&aa[1]!=temR[1]&&aa[1]!=temR[2]&&aa[1]!=temR[3])
						TheResult.push_back(aa);
					if (bb[0]!=temR[0]&&bb[0]!=temR[1]&&bb[0]!=temR[2]&&bb[0]!=temR[3]&&bb[1]!=temR[0]&&bb[1]!=temR[1]&&bb[1]!=temR[2]&&bb[1]!=temR[3])
						TheResult.push_back(bb);
					if (cc[0]!=temR[0]&&cc[0]!=temR[1]&&cc[0]!=temR[2]&&cc[0]!=temR[3]&&cc[1]!=temR[0]&&cc[1]!=temR[1]&&cc[1]!=temR[2]&&cc[1]!=temR[3])
						TheResult.push_back(cc);
					if (dd[0]!=temR[0]&&dd[0]!=temR[1]&&dd[0]!=temR[2]&&dd[0]!=temR[3]&&dd[1]!=temR[0]&&dd[1]!=temR[1]&&dd[1]!=temR[2]&&dd[1]!=temR[3])
						TheResult.push_back(dd);
				}
				else
				{
					xd::outline aa,bb,cc,dd;
					aa.push_back(temR[0]);aa.push_back(temR[1]);
					bb.push_back(temR[1]);bb.push_back(temR[2]);
					cc.push_back(temR[2]);cc.push_back(temR[3]);
					dd.push_back(temR[3]);dd.push_back(temR[0]);
					if (!isRepetition(TheOutline,aa))
						TheResult.push_back(aa);
					if (!isRepetition(TheOutline,bb))
						TheResult.push_back(bb);
					if (!isRepetition(TheOutline,cc))
						TheResult.push_back(cc);
					if (!isRepetition(TheOutline,dd))
						TheResult.push_back(dd);
				}
			}
			for (int j=0;j!=TemI.size();++j)
			{
				if (TemI[j].front()!=TemI[j].back())
				{
					TemI[j].push_back(TemI[j].front());
				}
			}
            xd::InfillLine(TemI,TemO,width,xd::degrees(RemovePolygon[i].second),0,0.1,width);
			for (int j=0;j!=TemO.size();++j)
			{
				TheResult.push_back(TemO[j]);
			}
			splitArea.Clear();
		}			
	}

}
