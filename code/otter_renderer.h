/* date = October 23rd 2020 8:50 pm */

#ifndef OTTER_RENDERER_H
#define OTTER_RENDERER_H

//~ STRUCTURES

typedef struct {
    f32 x;
    f32 y;
    f32 z;
} Point3f, Vecf;

typedef struct {
    i32 x;
    i32 y;
    i32 z;
} Point3i, Vec3i;

typedef struct {
    f32 x;
    f32 y;
} Point2f, Vec2f;

typedef struct {
    i32 x;
    i32 y;
} Point2i, Vec2i;

typedef struct {
    union {
        struct {
            Point2i a;
            Point2i b;
            Point2i c;
        } 
        Point2i[3];
    };
} Triangle2i;

typedef struct {
    union {
        struct {
            Point3i a;
            Point3i b;
            Point3i c;
        } 
        Point3i[3];
    };
} Triangle3i;

typedef struct {
    union {
        struct {
            Point3f a;
            Point3f b;
            Point3f c;
        } 
        Point3f[3];
    };
} Triangle3f;

typedef struct {
    Triangle3f* triangles;
} Mesh;

//~ FUNCTIONS

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

internal inline void
otter_paintPixel(otter_OffscreenBuffer* videoBackbuffer,
                 Point2i point,
                 u32 colour) {
    
    if ((point.x >= 0)
        &&(point.x < (i32)videoBackbuffer->width)
        && (point.y >= 0)
        &&(point.y < (i32)videoBackbuffer->height)) {
        
        u32* pixel = (u32*)((u8*)videoBackbuffer->pixels
                            + (u32)point.x * videoBackbuffer->pixelStride
                            + (u32)point.y * videoBackbuffer->pitch);
        
        *pixel = colour;
    }
}\

internal void
otter_drawLineBresenham(otter_OffscreenBuffer* videoBackbuffer,
                        u32 colour,
                        Point2i a, Point2i b) {
    
    i32 dx = 2 * (b.x - a.x);
    i32 dxSign = (dx < 0) ? -1 : 1;
    dx *= dxSign;
    
    i32 dy = 2 * (b.y - a.y);
    i32 dySign = (dy < 0) ? -1 : 1;
    dy *= dySign;
    
    Point2i drawPoint = a;
    
    if (dx > dy) {
        
        i32 fault = dx / 2;
        while (true) {
            
            otter_paintPixel(videoBackbuffer,
                             drawPoint,
                             colour);
            
            if (drawPoint.x == b.x) {
                if (drawPoint.y == b.y) {
                    
                    break;
                }
            }
            drawPoint.x += dxSign;
            
            fault -= dy;
            if (fault < 0) {
                
                drawPoint.y += dySign;
                fault += dx;
            }
        }
    }
    else {
        
        i32 fault = dy / 2;
        while (true){
            
            otter_paintPixel(videoBackbuffer,
                             drawPoint,
                             colour);
            
            if (drawPoint.x == b.x) {
                if (drawPoint.y == b.y) {
                    
                    break;
                }
            }
            
            drawPoint.y += dySign;
            
            fault -= dx;
            if (fault < 0) {
                
                drawPoint.x += dxSign;
                fault += dy;
            }
        }
        
    }
}

internal void
otter_drawTriangle(otter_OffscreenBuffer* videoBackbuffer,
                   Point2f point1_float, Point2f point2_float, Point2f point3_float,
                   f32 red, f32 green, f32 blue) {
    
    if (red < 0.0f) {
        red = 0.0f;
    }
    if (green < 0.0f) {
        green = 0.0f;
    }
    if (blue < 0.0f) {
        blue = 0.0f;
    }
    
    Point2i point1 = Point2fToPoint2i(point1_float);
    Point2i point2 = Point2fToPoint2i(point2_float);
    Point2i point3 = Point2fToPoint2i(point3_float);
    
    u32 colour = (u32)round_floatToI32(red * 255.0f) << 16
        | (u32)round_floatToI32(green * 255.0f) << 8
        | (u32)round_floatToI32(blue * 255.0f)  << 0;
    
    otter_drawLineBresenham(videoBackbuffer,
                            colour,
                            point1, point2);
    otter_drawLineBresenham(videoBackbuffer,
                            colour,
                            point1, point3);
    otter_drawLineBresenham(videoBackbuffer,
                            colour,
                            point2, point3);
}

internal void
otter_fillTriangle(otter_OffscreenBuffer* videoBackbuffer,
                   Point2f point1_float, Point2f point2_float, Point2f point3_float,
                   f32 red, f32 green, f32 blue) {
    
}

internal void
otter_drawCircleSimple(otter_OffscreenBuffer* videoBackbuffer,
                       Point2f centre_float,
                       f32 radius_float,
                       f32 red, f32 green, f32 blue) {
    
    i32 radius = round_floatToI32(radius_float);
    Point2i centre = Point2fToPoint2i(centre_float);
    
    i32 x, y;
    u32 colour = (u32)round_floatToI32(red * 255.0f) << 16
        | (u32)round_floatToI32(green * 255.0f) << 8
        | (u32)round_floatToI32(blue * 255.0f)  << 0;
    
    
    for (x = -radius; x <= radius; ++x) {
        
        y = (i32)(sqrt((radius * radius) - (x * x)));
        Point2i drawPoint = {
            .x = x + centre.x,
            .y = y + centre.y
        };
        otter_paintPixel(videoBackbuffer,
                         drawPoint,
                         colour);
        
        drawPoint.y -= 2 * y;
        otter_paintPixel(videoBackbuffer,
                         drawPoint,
                         colour);
    }
}


internal void
otter_drawCircleBresenham(otter_OffscreenBuffer* videoBackbuffer,
                          Point2f centre_float,
                          f32 radius_float,
                          f32 red, f32 green, f32 blue) {
    
    Point2i centre = Point2fToPoint2i(centre_float);
    i32 radius = round_floatToI32(radius_float);
    
    i32 x = 0;
    i32 y = radius;
    i32 decider = 1 - radius;
    
    u32 colour = (u32)round_floatToI32(red * 255.0f) << 16
        | (u32)round_floatToI32(green * 255.0f) << 8
        | (u32)round_floatToI32(blue * 255.0f)  << 0;
    
    while (x <= y) {
        
        Point2i drawPoint = {
            .x = centre.x + x,
            .y = centre.y + y
        };
        
        otter_paintPixel(videoBackbuffer,
                         drawPoint,
                         colour);
        
        drawPoint.y = centre.y - y;
        otter_paintPixel(videoBackbuffer,
                         drawPoint,
                         colour);
        
        drawPoint.x = centre.x - x;
        drawPoint.y = centre.y + y;
        otter_paintPixel(videoBackbuffer,
                         drawPoint,
                         colour);
        
        drawPoint.y = centre.y - y;
        otter_paintPixel(videoBackbuffer,
                         drawPoint,
                         colour);
        
        drawPoint.x = centre.x + y;
        drawPoint.y = centre.y + x;
        otter_paintPixel(videoBackbuffer,
                         drawPoint,
                         colour);
        
        drawPoint.y = centre.y - x;
        otter_paintPixel(videoBackbuffer,
                         drawPoint,
                         colour);
        
        drawPoint.x = centre.x - y;
        drawPoint.y = centre.y + x;
        otter_paintPixel(videoBackbuffer,
                         drawPoint,
                         colour);
        
        drawPoint.y = centre.y - x;
        otter_paintPixel(videoBackbuffer,
                         drawPoint,
                         colour);
        
        if (decider < 0) {
            
            decider += 2 * x + 3;
            x += 1;
        } else {
            
            decider += (2 * x) - (2 * y) + 5;
            x += 1;
            y -= 1;
        }
    }
}

#endif //OTTER_RENDERER_H
