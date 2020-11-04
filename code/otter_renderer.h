/* date = October 23rd 2020 8:50 pm */

#ifndef OTTER_RENDERER_H
#define OTTER_RENDERER_H

#include "utils.h"

//~ STRUCTURES

typedef union {
    struct {
		f32 x;
		f32 y;
		f32 z;
	};
	f32 coords[3];
} Point3f, Vecf;

typedef union {
	struct {
		i32 x;
		i32 y;
		i32 z;
	};
	i32 coords[3];
} Point3i, Vec3i;

typedef union {
    struct {
		f32 x;
		f32 y;
	};
	f32 coords[2];
} Point2f, Vec2f;

typedef union {
    struct {
		i32 x;
		i32 y;
	};
	i32 coords[2];
} Point2i, Vec2i;

typedef union {
	struct {
		Point2i a;
		Point2i b;
		Point2i c;
	}; 
	Point2i points[3];
} Triangle2i;

typedef union {
	struct {
		Point2f a;
		Point2f b;
		Point2f c;
	}; 
	Point2f points[3];
} Triangle2f;

typedef union {
	struct {
		Point3i a;
		Point3i b;
		Point3i c;
	}; 
	Point3i points[3];
} Triangle3i;

typedef union {
	struct {
		Point3f a;
		Point3f b;
		Point3f c;
	}; 
	Point3f points[3];
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
}

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
                if (drawPoint.y == b.y) { break; }
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
otter_drawTriangle(otter_OffscreenBuffer* videoBackbuffer,
                   Triangle2f triangle,
                   f32 red, f32 green, f32 blue) {
    
    Point2i point1 = Point2fToPoint2i(triangle.points[0]);
    Point2i point2 = Point2fToPoint2i(triangle.points[1]);
    Point2i point3 = Point2fToPoint2i(triangle.points[2]);
    
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
otter_fillTriangleBresenham(otter_OffscreenBuffer* videoBackbuffer,
                            Triangle2f triangle,
                            f32 red, f32 green, f32 blue) {
    
    Point2i point1 = Point2fToPoint2i(triangle.points[0]);
    Point2i point2 = Point2fToPoint2i(triangle.points[1]);
    Point2i point3 = Point2fToPoint2i(triangle.points[2]);
    
    u32 colour = (u32)round_floatToI32(red * 255.0f) << 16
        | (u32)round_floatToI32(green * 255.0f) << 8
        | (u32)round_floatToI32(blue * 255.0f)  << 0;
    
    if (point3.y < point2.y) {
        SWAP(point3, point2);
    }
    if (point3.y < point1.y) {
        SWAP(point3, point1);
    }
    if (point2.y < point1.y) {
        SWAP(point2, point1);
    }
    
    Point2i drawPoint_1, drawPoint_2;
    
	i32 dx_1 = 2 * (point3.x - point1.x);
    i32 dxSign_1 = (dx_1 < 0) ? -1 : 1;
    dx_1 *= dxSign_1;
    i32 dy_1 = 2 * (point3.y - point1.y);
    b32 swapped_1 = false;
    if (dx_1 < dy_1) { SWAP(dx_1, dy_1); swapped_1 = true; }
    i32 fault_1 = 2 * dy_1 - dx_1;
	
    i32 dx_2 = 2 * (point2.x - point1.x);
    i32 dxSign_2 = (dx_2 < 0) ? -1 : 1;
    dx_2 *= dxSign_2;
    i32 dy_2 = 2 * (point2.y - point1.y);
	
    if (dy_2 == 0) { drawPoint_2 = point2; }
    else { drawPoint_2 = point1; }
	drawPoint_1 = point1;
	
	b32 swapped_2 = false;
    if (dx_2 < dy_2) { SWAP(dx_2, dy_2); swapped_2 = true; }
    i32 fault_2 = 2 * dy_2 - dx_2;
    
	
	
	i32 largestDistance = point3.y - point1.y;
    
	// iterate from the topmost y to the lowermost y
	for (i32 i = 0; i <= largestDistance; ++i) {
        
		//draw a line between the two drawpoints
        otter_drawLineBresenham(videoBackbuffer,
                                colour,
                                drawPoint_1, drawPoint_2);
        
		// trace the first edge till we move once in y
		while (fault_1 >= 0) {
            
            if (swapped_1) { drawPoint_1.x += dxSign_1; }
            else { drawPoint_1.y += 1; }
            
            fault_1 -= 2 * dx_1;
        }
        
        if (swapped_1) { drawPoint_1.y += 1; }
        else { drawPoint_1.x += dxSign_1; }
        
        fault_1 += 2 * dy_1;
		
        // if the first two points are on the same horizontal line, skip the second edge and
		// start drawing the third edge
		if ((dy_2 == 0) 
			&& (!swapped_2)) {
			
			dx_2 = 2 * (point3.x - point2.x);
			dxSign_2 = (dx_2 < 0) ? -1 : 1;
			dx_2 *= dxSign_2;
			dy_2 = 2 * (point3.y - point2.y);
			swapped_2 = false;
			if (dx_2 < dy_2) { SWAP(dx_2, dy_2); swapped_2 = true; }
			fault_2 = 2 * dy_2 - dx_2;
		}
		
		// trace the second edge till the drawpoint is in the same y as the first point
		while (drawPoint_1.y != drawPoint_2.y) {
			
			while (fault_2 >= 0) {
				
				if (swapped_2) { drawPoint_2.x += dxSign_2;  }
				else { drawPoint_2.y += 1;  }
				
				fault_2 -= 2 * dx_2;
			}
			
			if (swapped_2) { drawPoint_2.y += 1; }
			else { drawPoint_2.x += dxSign_2;  }
			
			fault_2 += 2 * dy_2;
			
			if (drawPoint_2.y == point2.y) { break; }
		}
		
		// if we reach the end of the second edge, start tracing the third edge 
		if (drawPoint_2.y == point2.y) {
			
			dx_2 = 2 * (point3.x - point2.x);
			dxSign_2 = (dx_2 < 0) ? -1 : 1;
			dx_2 *= dxSign_2;
			dy_2 = 2 * (point3.y - point2.y);
			swapped_2 = false;
			if (dx_2 < dy_2) { SWAP(dx_2, dy_2); swapped_2 = true; }
			fault_2 = 2 * dy_2 - dx_2;
			
		}
	}
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

// TODO(Jai): Normalize the coordinates
inline internal Triangle2f
otter_project3DTriangleto2D(Triangle3f triangle3D,
							u32 screenWidth, u32 screenHeight) {
	
	Triangle2f result = {0};
	
	Point2f midPoint = {
		.x = (f32)screenWidth / 2,
		.y = (f32)screenHeight / 2
	};
	
	for (u32 i = 0; i < 3; ++i) {
		
		Point2f projectedMidPoint = {
			.x = midPoint.x * (triangle3D.points[i].z + 1),
			.y = midPoint.y * (triangle3D.points[i].z + 1)
		};
		
		result.points[i].x = ((triangle3D.points[i].x - projectedMidPoint.x)  / (triangle3D.points[i].z + 1)) 
			+ midPoint.x;
		result.points[i].y = ((triangle3D.points[i].y - projectedMidPoint.y) / (triangle3D.points[i].z + 1)) 
			+ midPoint.y;
	}
	
	return result;
}

#endif //OTTER_RENDERER_H
