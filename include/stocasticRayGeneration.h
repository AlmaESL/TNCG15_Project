// Uniform on sqrt(n)
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

        // Gaussian parameters for importance sampling
        double mean = 0.0;
        double stddev = 0.4;

        // Build orthonormal basis for random sampling on local hemisphere
        Vec3 w = forward.normalize();

        // Smaller than 0.999 to avoid numerical instability when forward is (0,1,0)
        Vec3 up = (std::abs(w.y) < 0.999) ? Vec3(0.0, 1.0, 0.0) : Vec3(1.0, 0.0, 0.0);
        Vec3 u = up.crossProduct(w).normalize(); // tangent
        Vec3 v = w.crossProduct(u).normalize();  // bitangent

        // Stratify with sqrt(n) --> TODO: test other strata
        int sqrtN = static_cast<int>(std::sqrt(n));
        if (sqrtN * sqrtN < n) sqrtN++;

        // Guassian distribution for importance sampling
        std::normal_distribution<double> gauss(mean, stddev);

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

                // Gaussian-weighted elevation angle theta
                double theta = std::abs(gauss(gen));
                theta = std::min(theta, M_PI / 2);

                // Cosine-weighted hemisphere sampling (CDF) - to produce non-uniformity
                /*double r = std::sqrt(u1);*/
                double phi = 2.0 * M_PI * u2;

                // Convert local cartesian to local spherical
                double lx = std::sin(theta) * std::cos(phi);
                double ly = std::sin(theta) * std::sin(phi);
                double lz = std::cos(theta); // sqrt(1-u1)

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



//
// // n
//#pragma once
//
//#include "vec3.h"
//#include "ray.h"
//
//#include <random>
//#include <cmath>
//
//#define _USE_MATH_DEFINES
//#include <math.h>
//
//class StocasticRayGeneration {
//public:
//    std::vector<Ray> rays;
//
//    // Generate n random rays distributed by cosine-weighted CDF around 'forward'
//    StocasticRayGeneration(const Vec3& o, int n, const Vec3& forward) {
//        origin = o;
//
//        // RNG: use a local generator
//        std::random_device rd;
//        std::mt19937 gen(rd());
//        std::uniform_real_distribution<> dis(0.0, 1.0);
//
//        // Gaussian parameters for importance sampling
//        double mean = 0.0;
//        double stddev = 0.4;
//
//        // Build orthonormal basis for random sampling on local hemisphere
//        Vec3 w = forward.normalize();
//
//        // Smaller than 0.999 to avoid numerical instability when forward is (0,1,0)
//        Vec3 up = (std::abs(w.y) < 0.999) ? Vec3(0.0, 1.0, 0.0) : Vec3(1.0, 0.0, 0.0);
//        Vec3 u = up.crossProduct(w).normalize(); // tangent
//        Vec3 v = w.crossProduct(u).normalize();  // bitangent
//
//        // Stratify with sqrt(n) --> TODO: test other strata
//        int sqrtN = static_cast<int>(std::sqrt(n));
//        if (sqrtN * sqrtN < n) sqrtN++;
//
//        // Guassian distribution for importance sampling
//        std::normal_distribution<double> gauss(mean, stddev);
//
//        // Loop strata and generate random samples
//        for (int i = 0; i < n; ++i) {
//            for (int j = 0; j < n; ++j) {
//
//                // Stop if we have enough samples
//                if ((int)rays.size() >= n) {
//                    break;
//                }
//
//                // Stratified samples in [0,1)
//                double u1 = (i + dis(gen)) / double(n);
//                double u2 = (j + dis(gen)) / double(n);
//
//                // Gaussian-weighted elevation angle theta
//                double theta = std::abs(gauss(gen));
//                theta = std::min(theta, M_PI / 2);
//
//                // Cosine-weighted hemisphere sampling (CDF) - to produce non-uniformity
//                /*double r = std::sqrt(u1);*/
//                double phi = 2.0 * M_PI * u2;
//
//                // Convert local cartesian to local spherical
//                double lx = std::sin(theta) * std::cos(phi);
//                double ly = std::sin(theta) * std::sin(phi);
//                double lz = std::cos(theta); // sqrt(1-u1)
//
//                // Convert to world space direction from local spherical
//                Vec3 worldDir = (u * lx + v * ly + w * lz).normalize();
//                rays.emplace_back(origin, worldDir);
//
//                // Break if n samples is reached
//                if ((int)rays.size() >= n) {
//                    break;
//                }
//            }
//            if ((int)rays.size() >= n) {
//                break;
//            }
//        }
//    }
//
//private:
//    Vec3 origin;
//};







//// Rectangular stratification
//#pragma once
//
//#include "vec3.h"
//#include "ray.h"
//
//#include <random>
//#include <cmath>
//#include <algorithm> // shuffle
//
//#define _USE_MATH_DEFINES
//#include <math.h>
//
//class StocasticRayGeneration {
//public:
//    std::vector<Ray> rays;
//
//    // Generate n random rays distributed by cosine-weighted CDF around 'forward'
//    StocasticRayGeneration(const Vec3& o, int n, const Vec3& forward) {
//        origin = o;
//
//        // RNG: use a local generator
//        std::random_device rd;
//        std::mt19937 gen(rd());
//        std::uniform_real_distribution<> dis(0.0, 1.0);
//
//        // Gaussian parameters for importance sampling
//        double mean = 0.0;
//        double stddev = 0.4;
//
//        // Build orthonormal basis for random sampling on local hemisphere
//        Vec3 w = forward.normalize();
//        Vec3 up = (std::abs(w.y) < 0.999) ? Vec3(0.0, 1.0, 0.0) : Vec3(1.0, 0.0, 0.0);
//        Vec3 u = up.crossProduct(w).normalize(); // tangent
//        Vec3 v = w.crossProduct(u).normalize();  // bitangent
//
//		// Rectangular stratification: generate n samples, one per row and column, with random column permutation
//        int m = static_cast<int>(std::floor(std::sqrt(n)));
//        if (m < 1) m = 1;
//        int k = (n + m - 1) / m;
//
//        for (int i = 0; i < m; ++i) {
//            for (int j = 0; j < k; ++j) {
//                if ((int)rays.size() >= n) break;
//
//                // N-Rooks: sample in [0,1) for each row and permuted column
//                double u1 = (i + dis(gen)) / double(m);
//                double u2 = (j + dis(gen)) / double(k);
//
//                // Gaussian-weighted elevation angle theta
//                double theta = std::abs(std::normal_distribution<double>(mean, stddev)(gen));
//                theta = std::min(theta, M_PI / 2);
//
//                double phi = 2.0 * M_PI * u2;
//
//                // Convert local cartesian to local spherical
//                double lx = std::sin(theta) * std::cos(phi);
//                double ly = std::sin(theta) * std::sin(phi);
//                double lz = std::cos(theta);
//
//                // Convert to world space direction from local spherical
//                Vec3 worldDir = (u * lx + v * ly + w * lz).normalize();
//                rays.emplace_back(origin, worldDir);
//            }
//            if ((int)rays.size() >= n) break;
//        }
//    }
//
//private:
//    Vec3 origin;
//};






//// N-rooks stratified sampling
//#pragma once
//
//#include "vec3.h"
//#include "ray.h"
//
//#include <random>
//#include <cmath>
//#include <algorithm> // shuffle
//
//#define _USE_MATH_DEFINES
//#include <math.h>
//
//class StocasticRayGeneration {
//public:
//    std::vector<Ray> rays;
//
//    // Generate n random rays distributed by cosine-weighted CDF around 'forward'
//    StocasticRayGeneration(const Vec3& o, int n, const Vec3& forward) {
//        origin = o;
//
//        // RNG: use a local generator
//        std::random_device rd;
//        std::mt19937 gen(rd());
//        std::uniform_real_distribution<> dis(0.0, 1.0);
//
//        // Gaussian parameters for importance sampling
//        double mean = 0.0;
//        double stddev = 0.4;
//
//        // Build orthonormal basis for random sampling on local hemisphere
//        Vec3 w = forward.normalize();
//        Vec3 up = (std::abs(w.y) < 0.999) ? Vec3(0.0, 1.0, 0.0) : Vec3(1.0, 0.0, 0.0);
//        Vec3 u = up.crossProduct(w).normalize(); // tangent
//        Vec3 v = w.crossProduct(u).normalize();  // bitangent
//
//        // N-Rooks sampling: generate n samples, one per row and column, with random column permutation
//        std::vector<int> perm(n);
//        for (int i = 0; i < n; ++i) perm[i] = i;
//        std::shuffle(perm.begin(), perm.end(), gen);
//
//        for (int i = 0; i < n; ++i) {
//            // N-Rooks: sample in [0,1) for each row and permuted column
//            double u1 = (i + dis(gen)) / n;
//            double u2 = (perm[i] + dis(gen)) / n;
//
//            // Gaussian-weighted elevation angle theta
//            double theta = std::abs(std::normal_distribution<double>(mean, stddev)(gen));
//            theta = std::min(theta, M_PI / 2);
//
//            double phi = 2.0 * M_PI * u2;
//
//            // Convert local cartesian to local spherical
//            double lx = std::sin(theta) * std::cos(phi);
//            double ly = std::sin(theta) * std::sin(phi);
//            double lz = std::cos(theta);
//
//            // Convert to world space direction from local spherical
//            Vec3 worldDir = (u * lx + v * ly + w * lz).normalize();
//            rays.emplace_back(origin, worldDir);
//        }
//    }
//
//private:
//    Vec3 origin;
//};