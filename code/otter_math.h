/* date = October 12th 2020 2:58 pm */

#ifndef OTTER_MATH_H
#define OTTER_MATH_H

#include "utils.h"

//*************************************************************************
// --------------------STRUCTURES------------------------------------------
//*************************************************************************

typedef struct {
    f32 x;
    f32 y;
    f32 z;
} Point3f;

typedef struct {
    i32 x;
    i32 y;
    i32 z;
} Point3i;

typedef struct {
    f32 x;
    f32 y;
} Point2f;

typedef struct {
    i32 x;
    i32 y;
} Point2i;

//*************************************************************************
// ---------------------FUNCTIONS------------------------------------------
//*************************************************************************

inline internal Point3i
Point3fToPoint3i(Point3f point_float) {
    Point3i result = {
        .x = round_floatToI32(point_float.x),
        .y = round_floatToI32(point_float.y),
        .z = round_floatToI32(point_float.z),
    };
    
    return result;
}

inline internal Point2i
Point2fToPoint2i(Point2f point_float) {
    Point2i result = {
        .x = round_floatToI32(point_float.x),
        .y = round_floatToI32(point_float.y),
    };
    
    return result;
}

#endif //OTTER_MATH_H
