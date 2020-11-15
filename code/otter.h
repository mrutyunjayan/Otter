/* date = October 12th 2020 2:39 pm */

#ifndef OTTER_H
#define OTTER_H

#include "utils.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#include <stretchy_buffer.h>
#pragma clang diagnostic pop

typedef struct {
    void* pixels;
    u32 width;
    u32 height;
	u32 aspectRatio;
    u32 pitch;
    u32 pixelStride;
} otter_OffscreenBuffer;

typedef struct {
    void* transientStorage;
    void* persistentStorage;
    
    u64 transientStorageSize;
    u64 persistentStorageSize;
    
    b32 isInitialized;
} otter_Memory;

typedef struct {
    void* assetData;
} otter_GameState;

//~ Rendering

typedef union {
    struct {
		f32 x;
		f32 y;
		f32 z;
	};
	f32 coords[3];
} Point3f, Vec3f;

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

//~ Matrices

typedef union {
	f32 array[16]; // 4 rows of 4 elements
	f32 matrix[4][4];
} Mat4x4;

#endif //OTTER_H
