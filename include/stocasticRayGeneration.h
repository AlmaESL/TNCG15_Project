#pragma once

#include "vec3.h"
#include "ray.h"
#include<random>

#define _USE_MATH_DEFINES
#include <math.h>

class StocasticRayGeneration {
public:
	std::vector<Ray> rays;

	StocasticRayGeneration(const Vec3& o, const int& n) { // Create n random rays, uniform distribution
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis(-(M_PI/2.0), (M_PI / 2.0));

		for (int i = 0; i <= n; ++i) {
			double xTheta = abs(dis(gen));
			double yTheta = dis(gen);
			double zTheta = dis(gen);

			//std::cout << "added Ray: " << xTheta << ", " << yTheta << " , " << zTheta << "\n";

			auto ray = Ray(origin, Vec3{ xTheta, yTheta, zTheta });
			rays.push_back(ray);
		}
	}

private:
	Vec3 origin;
};