#ifndef OTTER_RENDERER_H
#define OTTER_RENDERER_H

#include "otter.h"
#include "otter_math.h"
#include "utils.h"

#if defined(OTTER_DEBUG)
#include "stdio.h"
#endif

inline internal P3i
P3f_to_P3i(P3f point_float) {
    P3i result = {
        .x = og_round_floatToI32(point_float.x),
        .y = og_round_floatToI32(point_float.y),
        .z = og_round_floatToI32(point_float.z),
    };
    
    return result;
}

inline internal P2i
P2f_to_P2i(P2f point_float) {
    P2i result = {
        .x = og_round_floatToI32(point_float.x),
        .y = og_round_floatToI32(point_float.y),
    };
    
    return result;
}

internal inline void
og_renderer_paintPixel(og_OffscreenBuffer* videoBackbuffer,
                       P2i point,
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

// NOTE(Jai): Similar to the bresenham function below,
// but implemented in 3D and more specific to be used with
// the 3D triangle drawing function
internal void
og_renderer_draw_line_depthBuffered(og_OffscreenBuffer* videoBackbuffer,
                                    i32* depthBuffer,
                                    P3i a, P3i b,
                                    i32 imageWidth,
                                    u32 colour) {
    i32 dx = 2 * (b.x - a.x);
    i32 dxSign = (dx < 0) ? -1 : 1;
    dx *= dxSign;
    
    i32 dy = 2 * (b.y - a.y);
    i32 dySign = (dy < 0) ? -1 : 1;
    dy *= dySign;
    
    // NOTE(Jai): Assumes that the passed in z-values are zoomed in by a
    // factor of 1024
    i32 dzZoomedIn = 2 * (b.z - a.z);
    i32 dzSign = (dzZoomedIn < 0) ? -1 : 1;
    dzZoomedIn *= dzSign;
    i32 dz = dzZoomedIn >> 10;
    i32 zIncrement = dzSign * 1024;
    
    P2i drawPoint = { a.x, a.y };
    i32 zValue = a.z;
    
    // x is the driving axis
    if ((dx >= dy) && (dx >= dz)) {
        i32 fault1 = dy / 2;
        i32 fault2 = dz / 2;
        while (true) {
            if (depthBuffer[drawPoint.x + (drawPoint.y * imageWidth)] > zValue) {
                depthBuffer[drawPoint.x + (drawPoint.y * imageWidth)] = zValue;
                og_renderer_paintPixel(videoBackbuffer,
                                       drawPoint,
                                       colour);
            }
            if (drawPoint.x == b.x) {
                if (drawPoint.y == b.y) { break; }
            }
            drawPoint.x += dxSign;
            fault1 -= dy;
            fault2 -= dz;
            if (fault1 < 0) {
                drawPoint.y += dySign;
                fault1 += dx;
            }
            if (fault2 < 0) {
                zValue += zIncrement;
                fault2 += dx;
            }
        }
    }
    // y is the driving ayis
    if ((dx >= dy) && (dy >= dz)) {
        i32 fault1 = dx / 2;
        i32 fault2 = dz / 2;
        while (true) {
            if (depthBuffer[drawPoint.x + (drawPoint.y * imageWidth)] > zValue) {
                depthBuffer[drawPoint.x + (drawPoint.y * imageWidth)] = zValue;
                og_renderer_paintPixel(videoBackbuffer,
                                       drawPoint,
                                       colour);
            }
            if (drawPoint.x == b.x) {
                if (drawPoint.y == b.y) { break; }
            }
            drawPoint.y += dySign;
            fault1 -= dx;
            fault2 -= dz;
            if (fault1 < 0) {
                drawPoint.x += dxSign;
                fault1 += dy;
            }
            if (fault2 < 0) {
                zValue += zIncrement;
                fault2 += dy;
            }
        }
    }
    // z is the driving axis
    if ((dz >= dy) && (dz >= dx)) {
        i32 fault1 = dy / 2;
        i32 fault2 = dx / 2;
        while (true) {
            if (depthBuffer[drawPoint.x + (drawPoint.y * imageWidth)] > zValue) {
                depthBuffer[drawPoint.x + (drawPoint.y * imageWidth)] = zValue;
                og_renderer_paintPixel(videoBackbuffer,
                                       drawPoint,
                                       colour);
            }
            if (drawPoint.x == b.x) {
                if (drawPoint.y == b.y) { break; }
            }
            drawPoint.x += dxSign;
            fault1 -= dy;
            fault2 -= dx;
            if (fault1 < 0) {
                drawPoint.y += dySign;
                fault1 += dz;
            }
            if (fault2 < 0) {
                drawPoint.x += dxSign;
                fault2 += dz;
            }
        }
    }
    
}

internal void
og_renderer_draw_line_bresenham(og_OffscreenBuffer* videoBackbuffer,
                                u32 colour,
                                P2i a, P2i b) {
    i32 dx = 2 * (b.x - a.x);
    i32 dxSign = (dx < 0) ? -1 : 1;
    dx *= dxSign;
    i32 dy = 2 * (b.y - a.y);
    i32 dySign = (dy < 0) ? -1 : 1;
    dy *= dySign;
    
    P2i drawPoint = a;
    
    if (dx > dy) {
        i32 fault = dx / 2;
        while (true) {
            og_renderer_paintPixel(videoBackbuffer,
                                   drawPoint,
                                   colour);
            
            if (drawPoint.x == b.x) {
                if (drawPoint.y == b.y) { break; }
            }
            drawPoint.x += dxSign;
            fault -= dy;
            if (fault < 0) {
                drawPoint.y += dySign;
                fault += dx;
            }
        }
    } else {
        i32 fault = dy / 2;
        while (true) {
            og_renderer_paintPixel(videoBackbuffer,
                                   drawPoint,
                                   colour);
            
            if (drawPoint.x == b.x) {
                if (drawPoint.y == b.y) { break; }
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
og_lerp(i32 independentStart, f32 dependentStart,
        i32 independentEnd, f32 dependentEnd,
        f32* interpolatedValues) {
    f32 slope = (dependentEnd - dependentStart) / ((f32)independentEnd - (f32)independentStart);
    f32 interpolatedValue = dependentStart;
    memoryIndex index = 0;
    for (i32 i = independentStart; i <= independentEnd; ++i) {
        interpolatedValues[index++] = interpolatedValue;
        interpolatedValue += slope;
    }
}

// STUDY(Jai): Look into whether this is a good option for interpolating
// NOTE(Jai): similiar to the bresenham line algorithm, but instead of painting pixels,
// adds the intermediate values into an array
internal void
og_interpolate_integers(i32 independentStart, i32 dependentStart,
                        i32 independentEnd, i32 dependentEnd,
                        i32* interpolatedValues) {
    i32 dIndependent = 2 * (independentEnd - independentStart);
    i32 dIndependentSign = (dIndependent < 0) ? -1 : 1;
    dIndependent *= dIndependentSign;
    
    i32 dDependent = 2 * (dependentEnd - dependentStart);
    i32 dDependentSign = (dDependent < 0) ? -1 : 1;
    dDependent *= dDependentSign;
    
    P2i intermediatePoint = {
        .coords[0] = independentStart,
        .coords[1] = dependentStart
    };
    memoryIndex index = 0;
    interpolatedValues[index++] = dependentStart;
    
    if (dIndependent > dDependent) {
        i32 fault = dIndependent / 2;
        while (true) {
            interpolatedValues[index++] = intermediatePoint.coords[1];
            if (intermediatePoint.coords[0] == independentEnd) {
                if (intermediatePoint.coords[1] == dependentEnd) { break; }
            }
            intermediatePoint.coords[0] += dIndependentSign;
            fault -= dDependent;
            if (fault < 0) {
                intermediatePoint.coords[1] += dDependentSign;
                fault += dIndependent;
            }
        }
    } else {
        i32 fault = dDependent / 2;
        while (true) {
            if (intermediatePoint.coords[0] == independentEnd) {
                if (intermediatePoint.coords[1] == dependentEnd) { break; }
            }
            intermediatePoint.coords[1] += dDependentSign;
            fault -= dIndependent;
            if (fault < 0) {
                intermediatePoint.coords[0] += dIndependentSign;
                interpolatedValues[index++] = intermediatePoint.coords[1];
                fault += dDependent;
            }
        }
    }
    
    interpolatedValues[index] = dependentEnd;
}

internal void
og_renderer_draw_triangle(og_OffscreenBuffer* videoBackbuffer,
                          Triangle2f triangle,
                          u32 colour) {
    P2i point1 = P2f_to_P2i(triangle.points[0]);
    P2i point2 = P2f_to_P2i(triangle.points[1]);
    P2i point3 = P2f_to_P2i(triangle.points[2]);
    
    og_renderer_draw_line_bresenham(videoBackbuffer,
                                    colour,
                                    point1, point2);
    og_renderer_draw_line_bresenham(videoBackbuffer,
                                    colour,
                                    point1, point3);
    og_renderer_draw_line_bresenham(videoBackbuffer,
                                    colour,
                                    point2, point3);
}

// sunshine2k.de
internal inline void
og_renderer_fill_triangle_flatside(og_OffscreenBuffer* videoBackbuffer,
                                   P2i point1, P2i point2, P2i point3,
                                   u32 colour) {
    P2i drawPoint_1 = point1;
    P2i drawPoint_2 = point1;
    
    // first edge
    i32 dx_1 = 2 * (point3.x - point1.x);
    i32 dxSign_1 = (dx_1 < 0) ? -1 : 1;
    dx_1 *= dxSign_1;
    i32 dy_1 = 2 * (point3.y - point1.y);
    i32 dySign_1 = (dy_1 < 0) ? -1 : 1;
    dy_1 *= dySign_1;
    b32 swapped_1 = false;
    if (dx_1 < dy_1) { SWAP(dx_1, dy_1); swapped_1 = true; }
    i32 fault_1 = 2 * dy_1 - dx_1;
    
    // second edge
    i32 dx_2 = 2 * (point2.x - point1.x);
    i32 dxSign_2 = (dx_2 < 0) ? -1 : 1;
    dx_2 *= dxSign_2;
    i32 dy_2 = 2 * (point2.y - point1.y);
    i32 dySign_2 = (dy_2 < 0) ? -1 : 1;
    dy_2 *= dySign_2;
    b32 swapped_2 = false;
    if (dx_2 < dy_2) { SWAP(dx_2, dy_2); swapped_2 = true; }
    i32 fault_2 = 2 * dy_2 - dx_2;
    
    i32 largestDistance = dx_1 / 2;
    
    // iterate from the topmost y to the lowermost y
    for (i32 i = 0; i <= largestDistance; ++i) {
        //draw a line between the two drawpoints
        og_renderer_draw_line_bresenham(videoBackbuffer,
                                        colour,
                                        drawPoint_1, drawPoint_2);
        
        // trace the first edge till we move once in y
        // TODO(Jai): Fix case where dx_1 = 0
        while (fault_1 > 0) {
            if (swapped_1) { drawPoint_1.x += dxSign_1; }
            else { drawPoint_1.y += dySign_2; }
            
            fault_1 -= 2 * dx_1;
        }
        
        if (swapped_1) { drawPoint_1.y += dySign_1; }
        else { drawPoint_1.x += dxSign_1; }
        
        fault_1 += 2 * dy_1;
        
        // trace the second edge till the drawpoint is in the same y as the first point
        while (drawPoint_2.y != drawPoint_1.y) {
            while (fault_2 >= 0) {
                if (swapped_2) { drawPoint_2.x += dxSign_2; }
                else { drawPoint_2.y += dySign_2; }
                
                fault_2 -= 2 * dx_2;
            }
            
            if (swapped_2) { drawPoint_2.y += dySign_2; }
            else { drawPoint_2.x += dxSign_2; }
            
            fault_2 += 2 * dy_2;
        }
    }
}

// NOTE(Jai): Same as regular flatside filling, but the call to the drawline is
// replaced by a call to a depthbuffered drawline function
internal void
og_renderer_fill_triangle_flatside_depthBuffered(og_OffscreenBuffer* videoBackbuffer,
                                                 i32* depthBuffer,
                                                 P3i point1, P3i point2, P3i point3,
                                                 i32 imageWidth,
                                                 u32 colour) {
    P3i drawPoint_1 = point1;
    P3i drawPoint_2 = point1;
    
    // first edge
    i32 dx_1 = 2 * (point3.x - point1.x);
    i32 dxSign_1 = (dx_1 < 0) ? -1 : 1;
    dx_1 *= dxSign_1;
    i32 dy_1 = 2 * (point3.y - point1.y);
    i32 dySign_1 = (dy_1 < 0) ? -1 : 1;
    dy_1 *= dySign_1;
    b32 swapped_1 = false;
    if (dx_1 < dy_1) { SWAP(dx_1, dy_1); swapped_1 = true; }
    i32 fault_1 = 2 * dy_1 - dx_1;
    
    // second edge
    i32 dx_2 = 2 * (point2.x - point1.x);
    i32 dxSign_2 = (dx_2 < 0) ? -1 : 1;
    dx_2 *= dxSign_2;
    i32 dy_2 = 2 * (point2.y - point1.y);
    i32 dySign_2 = (dy_2 < 0) ? -1 : 1;
    dy_2 *= dySign_2;
    b32 swapped_2 = false;
    if (dx_2 < dy_2) { SWAP(dx_2, dy_2); swapped_2 = true; }
    i32 fault_2 = 2 * dy_2 - dx_2;
    
    i32 largestDistance = dx_1 / 2;
    
    // iterate from the topmost y to the lowermost y
    for (i32 i = 0; i <= largestDistance; ++i) {
        //draw a line between the two drawpoints
        og_renderer_draw_line_depthBuffered(videoBackbuffer,
                                            depthBuffer,
                                            drawPoint_1, drawPoint_2,
                                            imageWidth,
                                            colour);
        
        // trace the first edge till we move once in y
        // TODO(Jai): Fix case where dx_1 = 0
        while (fault_1 > 0) {
            if (swapped_1) { drawPoint_1.x += dxSign_1; }
            else { drawPoint_1.y += dySign_2; }
            
            fault_1 -= 2 * dx_1;
        }
        
        if (swapped_1) { drawPoint_1.y += dySign_1; }
        else { drawPoint_1.x += dxSign_1; }
        
        fault_1 += 2 * dy_1;
        
        // trace the second edge till the drawpoint is in the same y as the first point
        while (drawPoint_2.y != drawPoint_1.y) {
            while (fault_2 >= 0) {
                if (swapped_2) { drawPoint_2.x += dxSign_2; }
                else { drawPoint_2.y += dySign_2; }
                
                fault_2 -= 2 * dx_2;
            }
            
            if (swapped_2) { drawPoint_2.y += dySign_2; }
            else { drawPoint_2.x += dxSign_2; }
            
            fault_2 += 2 * dy_2;
        }
    }
}

internal void
og_renderer_fill_triangle(og_OffscreenBuffer* videoBackbuffer,
                          Triangle2f triangle,
                          u32 colour) {
    // Order the triangles according position in the y-axis
    P2i point1 = P2f_to_P2i(triangle.points[0]);
    P2i point2 = P2f_to_P2i(triangle.points[1]);
    P2i point3 = P2f_to_P2i(triangle.points[2]);
    
    if (point3.y < point2.y) { SWAP(point3, point2); }
    if (point3.y < point1.y) { SWAP(point3, point1); }
    if (point2.y < point1.y) { SWAP(point2, point1); }
    
    // check for the trivial cases
    if (point2.y == point3.y) { og_renderer_fill_triangle_flatside(videoBackbuffer,
                                                                   point1, point2, point3,
                                                                   colour); }
    else if (point1.y == point2.y) { og_renderer_fill_triangle_flatside(videoBackbuffer,
                                                                        point3, point1, point2,
                                                                        colour); }
    // General case - split the triangle into two - one flat bottom and one flat top
    else {
        P2i tempPoint = {
            .x = point1.x
                + (i32)((f32)(point3.x - point1.x) * ((f32)(point2.y - point1.y) / (f32)(point3.y - point1.y))),
            .y = point2.y
        };
        
        // Flat Bottom
        og_renderer_fill_triangle_flatside(videoBackbuffer,
                                           point1, point2, tempPoint,
                                           colour);
        // Flat Top
        og_renderer_fill_triangle_flatside(videoBackbuffer,
                                           point3, point2, tempPoint,
                                           colour);
    }
}

internal void
og_renderer_fill_triangle_3D(og_OffscreenBuffer* videoBackbuffer,
                             Triangle3f triangle,
                             i32* depthBuffer,
                             u32 colour) {
    // Bringing some of the decimal points of the float into significance
    // before converting to int
    triangle.points[0].z *= 1024.0f;
    triangle.points[1].z *= 1024.0f;
    triangle.points[2].z *= 1024.0f;
    
    // Order the triangles according position in the y-axis
    P3i point1 = P3f_to_P3i(triangle.points[0]);
    P3i point2 = P3f_to_P3i(triangle.points[1]);
    P3i point3 = P3f_to_P3i(triangle.points[2]);
    
    if (point3.y < point2.y) { SWAP(point3, point2); }
    if (point3.y < point1.y) { SWAP(point3, point1); }
    if (point2.y < point1.y) { SWAP(point2, point1); }
    
    i32 imageWidth = (i32)videoBackbuffer->pitch / (i32)videoBackbuffer->pixelStride;
    // check for the trivial cases
    if (point2.y == point3.y) { og_renderer_fill_triangle_flatside_depthBuffered(videoBackbuffer, depthBuffer,
                                                                                 point1, point2, point3,
                                                                                 imageWidth,
                                                                                 colour); }
    else if (point1.y == point2.y) { og_renderer_fill_triangle_flatside_depthBuffered(videoBackbuffer, depthBuffer,
                                                                                      point3, point1, point2,
                                                                                      imageWidth,
                                                                                      colour); }
    // General case - split the triangle into two - one flat bottom and one flat top
    else {
        P3i tempPoint = {
            .x = point1.x
                + (i32)((f32)(point3.x - point1.x) * ((f32)(point2.y - point1.y) / (f32)(point3.y - point1.y))),
            .y = point2.y,
            .z = point1.z
                + (i32)((f32)(point3.z - point1.z) * ((f32)(point2.y - point1.y) / (f32)(point3.y - point1.y)))
        };
        
        // Flat Bottom
        og_renderer_fill_triangle_flatside_depthBuffered(videoBackbuffer, depthBuffer,
                                                         point1, point2, tempPoint,
                                                         imageWidth,
                                                         colour);
        // Flat Top
        og_renderer_fill_triangle_flatside_depthBuffered(videoBackbuffer, depthBuffer,
                                                         point3, point2, tempPoint,
                                                         imageWidth,
                                                         colour);
    }
}

internal void
og_renderer_draw_circle_simple(og_OffscreenBuffer* videoBackbuffer,
                               P2f centre_float,
                               f32 radius_float,
                               u32 colour) {
    i32 radius = og_round_floatToI32(radius_float);
    P2i centre = P2f_to_P2i(centre_float);
    
    i32 x, y;
    
    for (x = -radius; x <= radius; ++x) {
        y = (i32)(sqrt((radius * radius) - (x * x)));
        P2i drawPoint = {
            .x = x + centre.x,
            .y = y + centre.y
        };
        og_renderer_paintPixel(videoBackbuffer,
                               drawPoint,
                               colour);
        
        drawPoint.y -= 2 * y;
        og_renderer_paintPixel(videoBackbuffer,
                               drawPoint,
                               colour);
    }
}


internal void
og_renderer_draw_circle_bresenham(og_OffscreenBuffer* videoBackbuffer,
                                  P2f centre_float,
                                  f32 radius_float,
                                  u32 colour) {
    P2i centre = P2f_to_P2i(centre_float);
    i32 radius = og_round_floatToI32(radius_float);
    
    i32 x = 0;
    i32 y = radius;
    i32 decider = 1 - radius;
    
    while (x <= y) {
        P2i drawPoint = {
            .x = centre.x + x,
            .y = centre.y + y
        };
        
        og_renderer_paintPixel(videoBackbuffer,
                               drawPoint,
                               colour);
        
        drawPoint.y = centre.y - y;
        og_renderer_paintPixel(videoBackbuffer,
                               drawPoint,
                               colour);
        
        drawPoint.x = centre.x - x;
        drawPoint.y = centre.y + y;
        og_renderer_paintPixel(videoBackbuffer,
                               drawPoint,
                               colour);
        
        drawPoint.y = centre.y - y;
        og_renderer_paintPixel(videoBackbuffer,
                               drawPoint,
                               colour);
        
        drawPoint.x = centre.x + y;
        drawPoint.y = centre.y + x;
        og_renderer_paintPixel(videoBackbuffer,
                               drawPoint,
                               colour);
        
        drawPoint.y = centre.y - x;
        og_renderer_paintPixel(videoBackbuffer,
                               drawPoint,
                               colour);
        
        drawPoint.x = centre.x - y;
        drawPoint.y = centre.y + x;
        og_renderer_paintPixel(videoBackbuffer,
                               drawPoint,
                               colour);
        
        drawPoint.y = centre.y - x;
        og_renderer_paintPixel(videoBackbuffer,
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

inline internal void
og_triangle_transform(Triangle3f* triangle,
                      readOnly Mat4D* transform) {
    og_v3f_transform(&triangle->points[0], transform);
    og_v3f_transform(&triangle->points[1], transform);
    og_v3f_transform(&triangle->points[2], transform);
}

#endif //OTTER_RENDERER
