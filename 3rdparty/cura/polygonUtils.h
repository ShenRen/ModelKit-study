/** Copyright (C) 2015 Ultimaker - Released under terms of the AGPLv3 License */
#ifndef UTILS_POLYGON_UTILS_H
#define UTILS_POLYGON_UTILS_H

#include "polygon.h"

namespace cura 
{

class PolygonUtils 
{
private:
    static const int64_t offset_safe_allowance = 20; // make all offset safe operations a bit less safe to allow for small variations in walls which are supposed to be exactly x perimeters thick
    static const int64_t in_between_min_dist_half = 10;

public:
    //! performs an offset compared to an adjacent inset/outset and also computes the area created by gaps between the two consecutive insets/outsets
    static void offsetExtrusionWidth(const Polygons& poly, bool inward, int extrusionWidth, Polygons& result, Polygons* in_between, bool removeOverlappingPerimeters)
    {
        int direction = (inward)? -1 : 1;
        int distance = (inward)? -extrusionWidth : extrusionWidth;
        if (!removeOverlappingPerimeters)
        {
            result = poly.offset(distance);
            return;
        }
        else
        {
            result = poly.offset(distance*3/2 - direction*offset_safe_allowance).offset(-distance/2 + direction*offset_safe_allowance); // overshoot by half the extrusionWidth
            if (in_between) // if a pointer for in_between is given
                in_between->add(poly.offset(distance/2 + direction*in_between_min_dist_half).difference(result.offset(-distance/2 - direction*in_between_min_dist_half)));
        }
    }

    /*!
    * performs an offset compared to an adjacent inset/outset and also computes the area created by gaps between the two consecutive insets/outsets.
    * This function allows for different extrusion widths between the two insets.
    */
    static void offsetSafe(const Polygons& poly, int distance, int offset_first_boundary, int extrusion_width, Polygons& result, Polygons* in_between, bool removeOverlappingPerimeters)
    {
        int direction = (distance > 0)? 1 : -1;
        if (!removeOverlappingPerimeters)
        {
            result = poly.offset(distance);
            return;
        }
        else
        {
            result = poly.offset(distance + direction*extrusion_width / 2 - direction*offset_safe_allowance).offset(-direction*extrusion_width/2 + direction*offset_safe_allowance); // overshoot by half the extrusionWidth
            if (in_between) // if a pointer for in_between is given
                in_between->add(poly.offset(offset_first_boundary + direction*in_between_min_dist_half).difference(result.offset(-direction * extrusion_width/2 - direction*in_between_min_dist_half)));
        }
    }

    //! performs an offset and makes sure the lines don't overlap (ignores any area between the original poly and the resulting poly)
    static void offsetSafe(const Polygons& poly, int distance, int extrusionWidth, Polygons& result, bool removeOverlappingPerimeters)
    {
        int direction = (distance > 0)? 1 : -1;
        if (!removeOverlappingPerimeters)
        {
            result = poly.offset(distance);
            return;
        }
        else
        {
            result = poly.offset(distance + direction*extrusionWidth/2 - direction*offset_safe_allowance).offset(-direction * extrusionWidth/2 + direction*offset_safe_allowance);
        }
    }
};

}//namespace cura

#endif//POLYGON_OPTIMIZER_H
