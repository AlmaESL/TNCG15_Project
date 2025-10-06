#pragma once

#include "vec3.h"
#include "ray.h"

#include <random>
#include <cmath>

#define _USE_MATH_DEFINES
#include <math.h>

class StocasticRayGeneration {
public:
	std::vector<Ray> rays;

	// Generate n random rays distributed by cosine-weighted CDF
	StocasticRayGeneration(const Vec3& o, int n, const Vec3& forward) {
		origin = o;

		// RNG
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis(0.0, 1.0);

		// Build orthonormal basis
		Vec3 F = forward.normalize();
		Vec3 up = (std::abs(F.y) < 0.999) ? Vec3(0.0, 1.0, 0.0) : Vec3(1.0, 0.0, 0.0);
		Vec3 T = up.crossProduct(F).normalize();
		Vec3 B = F.crossProduct(T).normalize();

		// Stratify
		int sqrtN = static_cast<int>(std::sqrt(n));
		if (sqrtN * sqrtN < n) sqrtN++;

		for (int i = 0; i < sqrtN; ++i) {
			for (int j = 0; j < sqrtN; ++j) {
				double u = (i + dis(gen)) / sqrtN;
				double v = (j + dis(gen)) / sqrtN;

				// Cosine-weighted hemisphere sampling (CDF-based)
				double phi = 2.0 * M_PI * u;
				double theta = std::acos(std::sqrt(1.0 - v));  // CDF-based theta

				double lx = std::sin(theta) * std::cos(phi);
				double ly = std::sin(theta) * std::sin(phi);
				double lz = std::cos(theta);

				Vec3 worldDir = (T * lx + B * ly + F * lz).normalize();
				rays.emplace_back(origin, worldDir);

				if ((int)rays.size() >= n) break;
			}
			if ((int)rays.size() >= n) break;
		}
	}

private:
	Vec3 origin;
};