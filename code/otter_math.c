#include "otter.h"

//~ Vector stuff

inline internal Vec3f
otter_vec3fScale(Vec3f vector,
				 f32 scale) {
	Vec3f result = {0};
	
	result.x *= scale;
	result.y *= scale;
	result.z *= scale;
	
	return result;
}

inline internal Vec3f
otter_vec3fCross(Vec3f v1,
				 Vec3f v2) {
	
	Vec3f result = {
		.x = (v1.y * v2.z) - (v1.z * v2.y),
		.y = (v1.z * v2.x) - (v1.x * v2.z),
		.z = (v1.x * v2.y) - (v1.y * v2.x)
	};
	
	return result;
}

inline internal f32
otter_vec3fDot(Vec3f v1,
			   Vec3f v2) {
	
	f32 result = (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
	
	return result;
}

inline internal void
otter_vec3fNormalize(Vec3f* v) {
	
	f32 magnitude = sqrtf((v->x * v->x)
						  + (v->y * v->y)
						  + (v->z * v->z));
	
	v->x = v->x / magnitude;
	v->y = v->y / magnitude;
	v->z = v->z / magnitude;
}
