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

    // Generate n random rays distributed by cosine-weighted CDF around 'forward'
    StocasticRayGeneration(const Vec3& o, int n, const Vec3& forward) {
        origin = o;

        // RNG: use a local generator
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, 1.0);

		// Build orthonormal basis for random sampling on local hemisphere
        Vec3 w = forward.normalize();

		// Smaller than 0.999 to avoid numerical instability when forward is (0,1,0)
        Vec3 up = (std::abs(w.y) < 0.999) ? Vec3(0.0, 1.0, 0.0) : Vec3(1.0, 0.0, 0.0);
        Vec3 u = up.crossProduct(w).normalize(); // tangent
        Vec3 v = w.crossProduct(u).normalize();  // bitangent

        // Stratify with sqrt(n) --> TODO: test other strata
        int sqrtN = static_cast<int>(std::sqrt(n));
        if (sqrtN * sqrtN < n) sqrtN++;

		// Loop strata and generate random samples
        for (int i = 0; i < sqrtN; ++i) {
            for (int j = 0; j < sqrtN; ++j) {

				// Stop if we have enough samples
                if ((int)rays.size() >= n) {
                    break;
                } 

                // Stratified samples in [0,1)
                double u1 = (i + dis(gen)) / double(sqrtN);
                double u2 = (j + dis(gen)) / double(sqrtN);

                // Cosine-weighted hemisphere sampling (CDF)
                double r = std::sqrt(u1);
                double phi = 2.0 * M_PI * u2;

                // Convert local cartesian to local spherical
                double lx = r * std::cos(phi);
                double ly = r * std::sin(phi);
                double lz = std::sqrt(std::max(0.0, 1.0 - u1)); // sqrt(1-u1)

				// Convert to world space direction from local spherical
                Vec3 worldDir = (u * lx + v * ly + w * lz).normalize();
                rays.emplace_back(origin, worldDir);

                // Break if n samples is reached
                if ((int)rays.size() >= n) {
                    break;
                } 
            }
            if ((int)rays.size() >= n) {
                break;
            }
        }
    }

private:
    Vec3 origin;
};
