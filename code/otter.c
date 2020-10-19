#include "otter_platform.h"

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
}

internal void
otter_bresenhamDrawLine(otter_OffscreenBuffer* videoBackbuffer,
                        u32 colour,
                        Point2i a, Point2i b) {
    
    i32 dx = 2 * (b.x - a.x);
    i32 dxSign = (dx < 0) ? -1 : 1;
    dx *= dxSign;
    
    i32 dy = 2 * (b.y - a.y);
    i32 dySign = (dy < 0) ? -1 : 1;
    dy *= dySign;
    
    if (dx > dy) {
        
        i32 fault = dx / 2;
        
        Point2i drawPoint = a;
        
        for (;;) {
            
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
        
        Point2i drawPoint = a;
        
        for (;;) {
            
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
    
    otter_bresenhamDrawLine(videoBackbuffer,
                            colour,
                            point1, point2);
    otter_bresenhamDrawLine(videoBackbuffer,
                            colour,
                            point1, point3);
    otter_bresenhamDrawLine(videoBackbuffer,
                            colour,
                            point2, point3);
}


//void
//otter_updateAndRender(otter_Memory* memory,
//                      otter_OffscreenBuffer* videoBackbuffer) 
OTTER_UPDATE_AND_RENDER(otterUpdateAndRender) {
    
    
}
