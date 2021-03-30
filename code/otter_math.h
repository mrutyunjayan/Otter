#ifndef OTTER_MATH_H
#define OTTER_MATH_H

#include "otter.h"

//~ Structures
typedef union P3f {
    struct {
		f32 x;
		f32 y;
		f32 z;
	};
	f32 coords[3];
} P3f;
typedef P3f V3f;

typedef union P3i {
	struct {
		i32 x;
		i32 y;
		i32 z;
	};
	i32 coords[3];
} P3i;
typedef P3i V3i;

typedef union P2f{
    struct {
		f32 x;
		f32 y;
	};
	f32 coords[2];
} P2f;
typedef P2f V2f;

typedef union P2i {
    struct {
		i32 x;
		i32 y;
	};
	i32 coords[2];
} P2i;
typedef P2i V2i;

typedef union Triangle2i {
	struct {
		P2i a;
		P2i b;
		P2i c;
	};
	P2i points[3];
} Triangle2i;

typedef union Triangle2f {
	struct {
		P2f a;
        P2f b;
		P2f c;
	};
	P2f points[3];
} Triangle2f;

typedef union Triangle3i {
	struct {
		P3i a;
		P3i b;
		P3i c;
	};
	P3i points[3];
} Triangle3i;

typedef union Triangle3f {
	struct {
		P3f a;
		P3f b;
		P3f c;
	};
	P3f points[3];
} Triangle3f;

typedef struct Mesh {
    Triangle3f* triangles;
    memoryIndex triangleCount;
    P3f* depthBuffer;
    memoryIndex depthBufferSize;
} Mesh;

//- Matrices

typedef union Mat4D {
	f32 array[16]; // 4 rows of 4 elements
	f32 matrix[4][4];
} Mat4D;

//~ Functions

//- Vector stuff
inline internal void
og_v3f_translate(V3f* vector,
                 f32 translationX, f32 translationY, f32 translationZ) {
    vector->x += translationX;
    vector->y += translationY;
    vector->z += translationZ;
}

inline internal void
og_v3f_scale(V3f* vector,
             f32 scaleX, f32 scaleY, f32 scaleZ) {
	vector->x *= scaleX;
	vector->y *= scaleY;
	vector->z *= scaleZ;
}

inline internal V3f
og_v3f_cross(V3f v1,
             V3f v2) {
	V3f result = {
		.x = (v1.y * v2.z) - (v1.z * v2.y),
		.y = (v1.z * v2.x) - (v1.x * v2.z),
		.z = (v1.x * v2.y) - (v1.y * v2.x)
	};
    
	return result;
}

inline internal f32
og_v3f_dot(V3f v1,
           V3f v2) {
	f32 result = (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
    
	return result;
}

inline internal void
og_v3f_normalize(V3f* v) {
	f32 magnitude = sqrtf((v->x * v->x)
						  + (v->y * v->y)
						  + (v->z * v->z));
    
	v->x = v->x / magnitude;
	v->y = v->y / magnitude;
	v->z = v->z / magnitude;
}

internal void
og_v3f_transform(V3f* vector,
                 readOnly Mat4D* transform) {
	f32 x = vector->x * transform->matrix[0][0]
		+ vector->y * transform->matrix[1][0]
		+ vector->z * transform->matrix[2][0]
		+ transform->matrix[3][0];
	f32 y = vector->x * transform->matrix[0][1]
		+ vector->y * transform->matrix[1][1]
		+ vector->z * transform->matrix[2][1]
		+ transform->matrix[3][1];
	f32 z = vector->x * transform->matrix[0][2]
		+ vector->y * transform->matrix[1][2]
		+ vector->z * transform->matrix[2][2]
		+ transform->matrix[3][2];
    
	f32 w = vector->x * transform->matrix[0][3]
		+ vector->y * transform->matrix[1][3]
		+ vector->z * transform->matrix[2][3]
		+ transform->matrix[3][3];
    
	// Project from 4D space to 3D Cartesian space
	if (w != 0.0f) {
		vector->x = x / w;
		vector->y = y / w;
		vector->z = z / w;
	}
}

#endif //OTTER_MATH
