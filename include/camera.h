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

	// Function generating n random rays through each pixel x,y in image plane of given height and width
	std::vector<Ray> generateRandomViewRays(int x, int y, int width, int height, int n) const {

		std::vector<Ray> rays;
		rays.reserve(n);

		Vec3 horizontal = lr - ll;
		Vec3 vertical = ul - ll;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis(0.0, 1.0);

		// Stratified pixel sampling
		int sqrtN = static_cast<int>(std::sqrt(n));
		if (sqrtN * sqrtN < n) sqrtN++;

		for (int i = 0; i < sqrtN; ++i) {
			for (int j = 0; j < sqrtN; ++j) {
				if ((int)rays.size() >= n) break;

				double u = (x + (i + dis(gen)) / sqrtN) / width;
				double v = 1.0 - (y + (j + dis(gen)) / sqrtN) / height;

				Vec3 pointOnImagePlane = ll + horizontal * u + vertical * v;
				Vec3 dir = (pointOnImagePlane - eyePos).normalize();

				rays.emplace_back(eyePos, dir);
			}
		}
		return rays;
	}

	/*StocasticRayGeneration generateViewRays(int n) const {
		return StocasticRayGeneration(eyePos, n);
	}*/
};