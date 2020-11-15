/* date = October 23rd 2020 8:50 pm */
#include "otter.h"

#if defined(OTTER_DEBUG)
#include "stdio.h"
#endif

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
#if defined(OTTER_DEBUG)
	printf("    Point 1: %d, %d\n"
		   "    Point 2: %d, %d\n"
		   "    Point 3: %d, %d\n\n",
		   point1.x , point1.y, point2.x, point2.y, point3.x, point3.y);
	
	for (i32 i = (i32)videoBackbuffer->width; i > 0; --i) {
		Point2i drawpoint = { .y = point1.y, .x = i };
		
		otter_paintPixel(videoBackbuffer, 
						 drawpoint,
						 0xFF77FFFF);
	}
	for (i32 i = (i32)videoBackbuffer->height; i > 0; --i) {
		Point2i drawpoint = { .y = i, .x = point1.x };
		
		otter_paintPixel(videoBackbuffer, 
						 drawpoint,
						 0xFF77FFFF);
	}
	
	for (i32 i = (i32)videoBackbuffer->width; i > 0; --i) {
		Point2i drawpoint = { .y = point2.y, .x = i };
		
		otter_paintPixel(videoBackbuffer, 
						 drawpoint,
						 0xFFFF77FF);
	}
	for (i32 i = (i32)videoBackbuffer->height; i > 0; --i) {
		Point2i drawpoint = { .y = i, .x = point2.x };
		
		otter_paintPixel(videoBackbuffer, 
						 drawpoint,
						 0xFFFF77FF);
	}
	
	for (i32 i = (i32)videoBackbuffer->width; i > 0; --i) {
		Point2i drawpoint = { .y = point3.y, .x = i };
		
		otter_paintPixel(videoBackbuffer, 
						 drawpoint,
						 0xFFFFFF77);
	}
	for (i32 i = (i32)videoBackbuffer->height; i > 0; --i) {
		Point2i drawpoint = { .y = i, .x = point3.x };
		
		otter_paintPixel(videoBackbuffer, 
						 drawpoint,
						 0xFFFFFF77);
	}
#endif
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

internal Vec3f
transformVector3D(Vec3f vector,
				  const Mat4x4* transform) {
	
	Vec3f result = {0};
	
	result.x = vector.x * transform->matrix[0][0]
		+ vector.y * transform->matrix[1][0]
		+ vector.z * transform->matrix[2][0]
		+ transform->matrix[3][0];
	result.y = vector.x * transform->matrix[0][1]
		+ vector.y * transform->matrix[1][1]
		+ vector.z * transform->matrix[2][1]
		+ transform->matrix[3][1];
	result.z = vector.x * transform->matrix[0][2]
		+ vector.y * transform->matrix[1][2]
		+ vector.z * transform->matrix[2][2]
		+ transform->matrix[3][2];
	
	f32 w = vector.x * transform->matrix[0][3]
		+ vector.y * transform->matrix[1][3]
		+ vector.z * transform->matrix[2][3]
		+ transform->matrix[3][3];
	
	// Project from 4D space to 3D Cartesian space
	if (w != 0.0f) {
		result.x /= w;
		result.y /= w;
		result.z /= w;
	}
	
	return result;
}
