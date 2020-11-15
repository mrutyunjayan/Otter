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
