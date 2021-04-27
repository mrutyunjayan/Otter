#ifndef OTTER_MATH_H
#define OTTER_MATH_H

#include "otter.h"

//~ Structures
typedef union P3f {
    struct {
		f32 x;
		f32 y;
		f32 z;
        f32 w;
	};
	f32 coords[4];
} P3f;
typedef P3f V3f;

typedef union P3i {
	struct {
		i32 x;
		i32 y;
		i32 z;
        i32 w;
	};
	i32 coords[4];
} P3i;
typedef P3i V3i;
typedef P3i Colour3i;

typedef union P2f{
    struct {
		f32 x;
		f32 y;
        f32 a;
	};
    struct {
		f32 u;
		f32 v;
        f32 w;
	};
	f32 p[3];
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
	P2i p[3];
} Triangle2i;

typedef union Triangle2f {
	struct {
		P2f a;
        P2f b;
		P2f c;
	};
	P2f p[3];
} Triangle2f;

typedef union Triangle3i {
	struct {
		P3i a;
		P3i b;
		P3i c;
	};
	P3i p[3];
} Triangle3i;

typedef union Triangle3f {
	struct {
		P3f a;
		P3f b;
		P3f c;
	};
	P3f p[3];
} Triangle3f;

//- Matrices

typedef union Mat4D {
	f32 array[16]; // 4 rows of 4 elements
	f32 matrix[4][4];
    struct {
        V3f r1;
        V3f r2;
        V3f r3;
        V3f r4;
    };
} Mat4D;

//~ Functions

//- Vector stuff

internal inline V3f
ogMath_v3f_add(V3f a, V3f b) {
    V3f result = {
        a.x + b.x,
        a.y + b.y,
        a.z + b.z,
        1.0f
    };
    return result;
}

internal inline V3f
ogMath_v3f_subtract(V3f a, V3f b) {
    V3f result = {
        a.x - b.x,
        a.y - b.y,
        a.z - b.z,
        1.0f
    };
    return result;
}

internal inline V3f
ogMath_v3f_translate(V3f v,
                     f32 translationX, f32 translationY, f32 translationZ) {
    V3f result = {
        v.x + translationX,
        v.y + translationY,
        v.z + translationZ,
        1.0f
    };
    
    return result;
}

internal inline P3f
ogMath_p3f_translate(P3f v,
                     f32 translationX, f32 translationY, f32 translationZ) {
    P3f result = {
        v.x + translationX,
        v.y + translationY,
        v.z + translationZ,
        0.0f
    };
    
    return result;
}

internal inline V3f
ogMath_v3f_scale(V3f v,
                 f32 scaleX, f32 scaleY, f32 scaleZ) {
    V3f result = {
        v.x * scaleX,
        v.y * scaleY,
        v.z * scaleZ,
        1.0f
    };
    
    return result;
}

internal inline V3f
ogMath_v3f_scaleUniform(V3f v, f32 scale) {
    V3f result = {
        v.x * scale,
        v.y * scale,
        v.z * scale,
        1.0f
    };
    
    return result;
}

internal inline V3f
ogMath_v3f_cross(V3f v1,
                 V3f v2) {
	V3f result = {
		.x = (v1.y * v2.z) - (v1.z * v2.y),
		.y = (v1.z * v2.x) - (v1.x * v2.z),
		.z = (v1.x * v2.y) - (v1.y * v2.x),
        .w = 1.0f
	};
    
	return result;
}

internal inline f32
ogMath_v3f_dot(V3f v1,
               V3f v2) {
	f32 result = (v1.x * v2.x)
        + (v1.y * v2.y)
        + (v1.z * v2.z);
    
	return result;
}

// TODO(Jai): Vectorize this/use rsqrt SSE instruction instead
internal inline V3f
ogMath_v3f_normalize(V3f v) {
	f32 magnitude = sqrtf((v.x * v.x)
						  + (v.y * v.y)
						  + (v.z * v.z));
    
    V3f result = {0};
    if (magnitude > 0.0f) {
        result.x = v.x / magnitude;
        result.y = v.y / magnitude;
        result.z = v.z / magnitude;
        result.w = v.w / magnitude;
    }
    return result;
}

internal inline V3f
ogMath_v3f_transform(V3f v,
                     readOnly Mat4D* transform) {
    V3f result = {
        .x = v.x * transform->matrix[0][0]
            + v.y * transform->matrix[0][1]
            + v.z * transform->matrix[0][2]
            + v.w * transform->matrix[0][3],
        .y = v.x * transform->matrix[1][0]
            + v.y * transform->matrix[1][1]
            + v.z * transform->matrix[1][2]
            + v.w * transform->matrix[1][3],
        .z = v.x * transform->matrix[2][0]
            + v.y * transform->matrix[2][1]
            + v.z * transform->matrix[2][2]
            + v.w * transform->matrix[2][3],
        .w = v.x * transform->matrix[3][0]
            + v.y * transform->matrix[3][1]
            + v.z * transform->matrix[3][2]
            + v.w * transform->matrix[3][3]
    };
    
    return result;
}

internal inline V3f
ogMath_v3f_transform_homogeneous(V3f v,
                                 readOnly Mat4D* transform) {
    V3f result = ogMath_v3f_transform(v, transform);
    if (v.w >= 1.0f) {
        f32 oneOverW = 1 / result.w;
        result.x *= oneOverW;
        result.y *= oneOverW;
        result.z *= oneOverW;
        result.w *= oneOverW;
    }
    
    return result;
}
//- Matrix Stuff

internal inline Mat4D
ogMath_mat4d_make_projection(f32 aspectRatio,
                             f32 fovDegrees,
                             f32 near, f32 far) {
	f32 fovRadians = 1.0f / tanf(0.5f * (fovDegrees * PI / 180.0f));
    f32 q = far / (far - near);
    Mat4D result = {
        aspectRatio * fovRadians, 0.0f,       0.0f, 0.0f,
        0.0f,                     fovRadians, 0.0f, 0.0f,
        0.0f,                     0.0f,       q,    near - q,
        0.0f,                     0.0f,       1.0f, 0.0f
    };
    
    return result;
}

internal inline Mat4D
ogMath_mat4d_make_identity(void) {
    Mat4D result = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    
    return result;
}

internal inline Mat4D
ogMath_mat4d_make_rotationX(f32 theta) {
    Mat4D result = {
        1.0f, 0.0f,               0.0f,                0.0f,
        0.0f, cosf(theta), sinf(theta), 0.0f,
        0.0f, -sinf(theta), cosf(theta),  0.0f,
        0.0f, 0.0f,               0.0f,                1.0f
    };
    
    return result;
}

internal inline Mat4D
ogMath_mat4d_make_rotationY(f32 theta) {
    Mat4D result = {
        cosf(theta), 0.0f, -sinf(theta), 0.0f,
        0.0f,        1.0f,  0.0f, 0.0f,
        sinf(theta), 0.0f,  cosf(theta), 0.0f,
        0.0f,        0.0f,  0.0f, 1.0f
    };
    
    return result;
}

internal inline Mat4D
ogMath_mat4d_make_rotationZ(f32 theta) {
    Mat4D result = {
        cosf(theta), -sinf(theta), 0.0f, 0.0f,
        sinf(theta), cosf(theta),  0.0f, 0.0f,
        0.0f,        0.0f,         1.0f, 0.0f,
        0.0f,        0.0f,         0.0f, 1.0f
    };
    
    return result;
}

internal inline Mat4D
ogMath_mat4d_make_translation(f32 x, f32 y, f32 z) {
    Mat4D result = {
        1.0f, 0.0f, 0.0f, x,
        0.0f, 1.0f, 0.0f, y,
        0.0f, 0.0f, 1.0f, z,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    
    return result;
}

internal inline Mat4D
ogMath_mat4d_make_scaling(f32 x, f32 y, f32 z) {
    Mat4D result = {
        x,    0.0f, 0.0f, 0.0f,
        0.0f, y,    0.0f, 0.0f,
        0.0f, 0.0f, z,    0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    
    return result;
}

internal inline Mat4D
ogMath_mat4d_scale(Mat4D m, f32 scale) {
    Mat4D result = {
        m.matrix[0][0] * scale, m.matrix[0][1] * scale, m.matrix[0][2] * scale, m.matrix[0][3] * scale,
        m.matrix[1][0] * scale, m.matrix[1][1] * scale, m.matrix[1][2] * scale, m.matrix[1][3] * scale,
        m.matrix[2][0] * scale, m.matrix[2][1] * scale, m.matrix[2][2] * scale, m.matrix[2][3] * scale,
        m.matrix[3][0] * scale, m.matrix[3][1] * scale, m.matrix[3][2] * scale, m.matrix[3][3] * scale,
    };
    
    return result;
}

internal inline Mat4D
ogMath_mat4d_add(Mat4D m1, Mat4D m2) {
    Mat4D result = {
        (m1.matrix[0][0] + m2.matrix[0][0]), (m1.matrix[0][1] + m2.matrix[0][1]), (m1.matrix[0][2] + m2.matrix[0][2]), (m1.matrix[0][3] + m2.matrix[0][3]),
        (m1.matrix[1][0] + m2.matrix[1][0]), (m1.matrix[1][1] + m2.matrix[1][1]), (m1.matrix[1][2] + m2.matrix[1][2]), (m1.matrix[1][3] + m2.matrix[1][3]),
        (m1.matrix[2][0] + m2.matrix[2][0]), (m1.matrix[2][1] + m2.matrix[2][1]), (m1.matrix[2][2] + m2.matrix[2][2]), (m1.matrix[2][3] + m2.matrix[2][3]),
        (m1.matrix[3][0] + m2.matrix[3][0]), (m1.matrix[3][1] + m2.matrix[3][1]), (m1.matrix[3][2] + m2.matrix[3][2]), (m1.matrix[3][3] + m2.matrix[3][3]),
    };
    
    return result;
}

internal inline Mat4D
ogMath_mat4d_subtract(Mat4D m1, Mat4D m2) {
    Mat4D result = {
        (m1.matrix[0][0] - m2.matrix[0][0]), (m1.matrix[0][1] - m2.matrix[0][1]), (m1.matrix[0][2] - m2.matrix[0][2]), (m1.matrix[0][3] - m2.matrix[0][3]),
        (m1.matrix[1][0] - m2.matrix[1][0]), (m1.matrix[1][1] - m2.matrix[1][1]), (m1.matrix[1][2] - m2.matrix[1][2]), (m1.matrix[1][3] - m2.matrix[1][3]),
        (m1.matrix[2][0] - m2.matrix[2][0]), (m1.matrix[2][1] - m2.matrix[2][1]), (m1.matrix[2][2] - m2.matrix[2][2]), (m1.matrix[2][3] - m2.matrix[2][3]),
        (m1.matrix[3][0] - m2.matrix[3][0]), (m1.matrix[3][1] - m2.matrix[3][1]), (m1.matrix[3][2] - m2.matrix[3][2]), (m1.matrix[3][3] - m2.matrix[3][3]),
    };
    
    return result;
}

internal inline Mat4D
ogMath_mat4d_multiply(Mat4D m1, Mat4D m2) {
    Mat4D result = {0};
#pragma clang loop unroll(full)
    for (memoryIndex row = 0; row < 4; row++) {
#pragma clang loop unroll(full)
        for (memoryIndex col = 0; col < 4; col++) {
            result.matrix[row][col] =
                m1.matrix[row][0] * m2.matrix[0][col]
                + m1.matrix[row][1] * m2.matrix[1][col]
                + m1.matrix[row][2] * m2.matrix[2][col]
                + m1.matrix[row][3] * m2.matrix[3][col];
        }
    }
    
    return result;
}

//-

internal void
ogMath_lerp(i32 independentStart, f32 dependentStart,
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
ogMath_interpolate_integers(i32 independentStart, i32 dependentStart,
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

#endif //OTTER_MATH
