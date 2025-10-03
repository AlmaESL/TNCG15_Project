#pragma once

#include "include/roomClass.h"
#include "include/ray.h"
#include"stocasticRayGeneration.h"

class Camera {
public:
	Vec3 eyePos, ll, ul, ur, lr;

	Camera() {
		eyePos = Vec3(0.5, 2.0, 2.0);
		ll = Vec3(1.0, 1.5, 1.5);
		ul = Vec3(1.0, 1.5, 2.5);
		ur = Vec3(1.0, 2.5, 2.5);
		lr = Vec3(1.0, 2.5, 1.5);
	}

	Ray generateViewRay(int x, int y, int width, int height) const {
		double u = (x + 0.5) / width;
		double v = 1.0 - (y + 0.5) / height;

		Vec3 horizontal = lr - ll;
		Vec3 vertical = ul - ll;

		Vec3 pointOnImagePlane = ll + horizontal * u + vertical * v;

		Vec3 ViewDir = (pointOnImagePlane - eyePos).normalize();

		return Ray(eyePos, ViewDir);
	}

	// Generate a view ray from normalized pixel coordinates u in [0,1], v in [0,1]
	Ray generateViewRayUV(double u, double v) const {
		Vec3 horizontal = lr - ll;
		Vec3 vertical = ul - ll;
		Vec3 pointOnImagePlane = ll + horizontal * u + vertical * v;
		Vec3 ViewDir = (pointOnImagePlane - eyePos).normalize();
		return Ray(eyePos, ViewDir);
	}

	StocasticRayGeneration generateViewRays(int n) const {
		return StocasticRayGeneration(eyePos, n);
	}
};