//������ 2015/1/22  ����ֱ�Ǽ��Զ��ж�����߶�����ɨ���߶εĳ����ͷ�ļ���
#ifndef SELF_RECOGNITION
#define SELF_RECOGNITION

#include <vector>
#include <cmath>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Boolean_set_operations_2.h>
typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef Kernel::Point_2                            Point;
typedef CGAL::Polygon_2<Kernel>                    Polygon;
typedef CGAL::Polygon_with_holes_2<Kernel>         Polygon_with_holes;
typedef std::vector<Polygon_with_holes>            Pgn_with_holes_container;
typedef std::vector<std::pair<Polygon,double>>     Pgn_container;
 
#include <CGAL/Polygon_set_2.h>
#include<CGAL/create_straight_skeleton_from_polygon_with_holes_2.h>
typedef CGAL::Polygon_set_2<Kernel>             Polygon_set;
typedef CGAL::Straight_skeleton_2<Kernel>       Ss ;
typedef Ss::Halfedge_iterator                   Halfedge_iterator;
typedef Ss::Halfedge_handle                     Halfedge_handle;
typedef Ss::Vertex_handle                       Vertex_handle;

/*#include <CGAL/Arr_segment_traits_2.h>
#include <CGAL/Arrangement_2.h>
typedef  CGAL::Arr_segment_traits_2<Kernel>    Traits;
typedef  CGAL::Arrangement_2<Traits>           Arrangement;*/   

#include "generate_line.h"
#include "pgn_print.h"
#include "../clipper/clipper.hpp"
typedef boost::shared_ptr<Ss> SsPtr ;

bool PanDuan(int a, std::vector<int> b);  //�����ж��ڲ�ֱ�Ǽܰ�߽ṹ�Ƿ��Ѿ�ʹ�ù��ĺ�����

bool isRepetition(const xd::outlines & outL,const xd::outline & line );  //�����ж�������Ƿ��Ѿ�����������ɨ����˵ĺ�����

bool IsContain(const xd::outlines &Contours,const xd::xdpoint &P);

void self_identifying(xd::outlines TheOutline,xd::outlines & TheResult,double width,double stagger,float degree);    //С�ı����������ʶ����������壡

void self_identifying_A(xd::outlines TheOutline,xd::outlines & TheResult,double width,double stagger);  //С�ı�����С����ʶ��

#endif
