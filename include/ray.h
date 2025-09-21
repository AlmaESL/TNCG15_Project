#pragma once

#include "include/vec3.h"


/// Utility structure for rays 
class Ray {
public:
	Vec3 origin, direction;
	Ray(const Vec3& o, const Vec3 dir) : origin(o), direction(dir.normalize()) {}


	// Make shadow rays from points light rays reach back to the eye 
	static Ray shadowRay(const Vec3& point, const Vec3& lightPos) {
		Vec3 lightDir = (lightPos - point).normalize(); 

		// Add offset so that rays don't intersect themselves
		const double offSetFactor = 1e-4; 
		Vec3 offSetDir = lightDir * offSetFactor;

		// Return shadow ray
		return Ray(point + offSetDir, lightDir);
	}
};

