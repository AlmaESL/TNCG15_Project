// Unifom on sqrt(n)
//#pragma once
//
//#include "include/roomClass.h"
//#include "include/ray.h"
//#include"stocasticRayGeneration.h"
//
//class Camera {
//public:
//	Vec3 eyePos, ll, ul, ur, lr;
//
//	Camera() {
//		eyePos = Vec3(0.5, 2.0, 2.0);
//		ll = Vec3(1.0, 1.5, 1.5);
//		ul = Vec3(1.0, 1.5, 2.5);
//		ur = Vec3(1.0, 2.5, 2.5);
//		lr = Vec3(1.0, 2.5, 1.5);
//	}
//
//	Ray generateViewRay(int x, int y, int width, int height) const {
//		double u = (x + 0.5) / width;
//		double v = 1.0 - (y + 0.5) / height;
//
//		Vec3 horizontal = lr - ll;
//		Vec3 vertical = ul - ll;
//
//		Vec3 pointOnImagePlane = ll + horizontal * u + vertical * v;
//
//		Vec3 ViewDir = (pointOnImagePlane - eyePos).normalize();
//
//		return Ray(eyePos, ViewDir);
//	}
//
//	// Generate a view ray from normalized pixel coordinates u in [0,1], v in [0,1]
//	Ray generateViewRayUV(double u, double v) const {
//		Vec3 horizontal = lr - ll;
//		Vec3 vertical = ul - ll;
//		Vec3 pointOnImagePlane = ll + horizontal * u + vertical * v;
//		Vec3 ViewDir = (pointOnImagePlane - eyePos).normalize();
//		return Ray(eyePos, ViewDir);
//	}
//
//	// Function generating n random rays through each pixel x,y in image plane of given height and width
//	// New version uses Gaussian distribution for non-uniform random importance sampling within each pixel
//	std::vector<Ray> generateRandomViewRays(int x, int y, int width, int height, int n) const {
//		std::vector<Ray> rays;
//		rays.reserve(n);
//
//		Vec3 horizontal = lr - ll;
//		Vec3 vertical = ul - ll;
//
//		// Local RNG
//		std::random_device rd;
//		std::mt19937 gen(rd());
//
//		// Gaussian distribution centered at 0.5, std dev controls spread
//		const double mean = 0.5; // Center of pixel
//		const double sigma = 0.15; // The spread of the distribution -increase/decrease to control focus
//		std::normal_distribution<> gaussian(mean, sigma);
//
//		int sqrtN = static_cast<int>(std::sqrt(n));
//		if (sqrtN * sqrtN < n) sqrtN++;
//
//		// Stratification with sqrt(n) strata --> can test other strata later
//		for (int i = 0; i < sqrtN; ++i) {
//			for (int j = 0; j < sqrtN; ++j) {
//				if ((int)rays.size() >= n) break;
//
//				// Normalized stratum width and height
//				double du = 1.0 / (width * sqrtN);
//				double dv = 1.0 / (height * sqrtN);
//
//				// Base pixel coordinate in [0,1]
//				double u1 = (x + i / (double)sqrtN) / width;
//				double u2 = 1.0 - (y + j / (double)sqrtN) / height;
//
//				// Offset within stratum using Gaussian sampling in [-0.5, 0.5] scaled by stratum size
//				double offSetu1 = gaussian(gen) - 0.5;
//				double offSetu2 = gaussian(gen) - 0.5;
//
//				// Scale offset by stratum size and add to base
//				double u = std::clamp(u1 + offSetu1 * du, 0.0, 1.0);
//				double v = std::clamp(u2 + offSetu2 * dv, 0.0, 1.0);
//
//				// Generate ray through the sampled pixel location
//				Vec3 pointOnImagePlane = ll + horizontal * u + vertical * v;
//				// Ray's direction from eye to point on image plane and beyond 
//				Vec3 dir = (pointOnImagePlane - eyePos).normalize();
//
//				rays.emplace_back(eyePos, dir);
//			}
//		}
//
//		return rays;
//	}
//
//
//	// Old random view ray version uses uniform random sampling within each sqrt(n) pixel stratum 
//	//std::vector<Ray> generateRandomViewRays(int x, int y, int width, int height, int n) const {
//
//	//	// Reserve space for rays
//	//	std::vector<Ray> rays;
//	//	rays.reserve(n);
//
//	//	// Image plane basis vectors
//	//	Vec3 horizontal = lr - ll;
//	//	Vec3 vertical = ul - ll;
//
//	//	// Local rand generator
//	//	std::random_device rd;
//	//	std::mt19937 gen(rd());
//	//	std::uniform_real_distribution<> dis(0.0, 1.0);
//
//	//	// Stratified pixel sampling
//	//	int sqrtN = static_cast<int>(std::sqrt(n));
//
//	//	// If n is not a perfect square, increase strata by 1
//	//	if (sqrtN * sqrtN < n) {
//	//		sqrtN++;
//	//	}
//
//
//	//	// Iterate strata and generate random samples
//	//	for (int i = 0; i < sqrtN; ++i) {
//	//		for (int j = 0; j < sqrtN; ++j) {
//
//	//			if ((int)rays.size() >= n) {
//	//				break;
//	//			}
//
//	//			// Get random sample directions within the pixel strata
//	//			double u = (x + (i + dis(gen)) / sqrtN) / width;
//	//			double v = 1.0 - (y + (j + dis(gen)) / sqrtN) / height;
//
//	//			// Generate ray through the random pixel location
//	//			Vec3 pointOnImagePlane = ll + horizontal * u + vertical * v;
//	//			Vec3 dir = (pointOnImagePlane - eyePos).normalize();
//
//	//			// Add shot ray to ray list
//	//			rays.emplace_back(eyePos, dir);
//	//		}
//	//	}
//	//	return rays;
//	//}
//};





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
	// New version uses Gaussian distribution for non-uniform random importance sampling within each pixel
	std::vector<Ray> generateRandomViewRays(int x, int y, int width, int height, int n) const {
		std::vector<Ray> rays;
		rays.reserve(n);

		Vec3 horizontal = lr - ll;
		Vec3 vertical = ul - ll;

		// Local RNG
		std::random_device rd;
		std::mt19937 gen(rd());

		// Gaussian distribution centered at 0.5, std dev controls spread
		const double mean = 0.5; // Center of pixel
		const double sigma = 0.15; // The spread of the distribution -increase/decrease to control focus
		std::normal_distribution<> gaussian(mean, sigma);

		int sqrtN = static_cast<int>(std::sqrt(n));
		if (sqrtN * sqrtN < n) sqrtN++;

		// Stratification with sqrt(n) strata --> can test other strata later
		for (int i = 0; i < n; ++i) {
			for (int j = 0; j < n; ++j) {
				if ((int)rays.size() >= n) break;

				// Normalized stratum width and height
				double du = 1.0 / (width * n);
				double dv = 1.0 / (height * n);

				// Base pixel coordinate in [0,1]
				double u1 = (x + i / (double)n) / width;
				double u2 = 1.0 - (y + j / (double)n) / height;

				// Offset within stratum using Gaussian sampling in [-0.5, 0.5] scaled by stratum size
				double offSetu1 = gaussian(gen) - 0.5;
				double offSetu2 = gaussian(gen) - 0.5;

				// Scale offset by stratum size and add to base
				double u = std::clamp(u1 + offSetu1 * du, 0.0, 1.0);
				double v = std::clamp(u2 + offSetu2 * dv, 0.0, 1.0);

				// Generate ray through the sampled pixel location
				Vec3 pointOnImagePlane = ll + horizontal * u + vertical * v;
				// Ray's direction from eye to point on image plane and beyond 
				Vec3 dir = (pointOnImagePlane - eyePos).normalize();

				rays.emplace_back(eyePos, dir);
			}
		}

		return rays;
	}


	// Old random view ray version uses uniform random sampling within each sqrt(n) pixel stratum 
	//std::vector<Ray> generateRandomViewRays(int x, int y, int width, int height, int n) const {

	//	// Reserve space for rays
	//	std::vector<Ray> rays;
	//	rays.reserve(n);

	//	// Image plane basis vectors
	//	Vec3 horizontal = lr - ll;
	//	Vec3 vertical = ul - ll;

	//	// Local rand generator
	//	std::random_device rd;
	//	std::mt19937 gen(rd());
	//	std::uniform_real_distribution<> dis(0.0, 1.0);

	//	// Stratified pixel sampling
	//	int sqrtN = static_cast<int>(std::sqrt(n));

	//	// If n is not a perfect square, increase strata by 1
	//	if (sqrtN * sqrtN < n) {
	//		sqrtN++;
	//	}


	//	// Iterate strata and generate random samples
	//	for (int i = 0; i < sqrtN; ++i) {
	//		for (int j = 0; j < sqrtN; ++j) {

	//			if ((int)rays.size() >= n) {
	//				break;
	//			}

	//			// Get random sample directions within the pixel strata
	//			double u = (x + (i + dis(gen)) / sqrtN) / width;
	//			double v = 1.0 - (y + (j + dis(gen)) / sqrtN) / height;

	//			// Generate ray through the random pixel location
	//			Vec3 pointOnImagePlane = ll + horizontal * u + vertical * v;
	//			Vec3 dir = (pointOnImagePlane - eyePos).normalize();

	//			// Add shot ray to ray list
	//			rays.emplace_back(eyePos, dir);
	//		}
	//	}
	//	return rays;
	//}
};






// Rectangular 
//#pragma once
//
//#include "include/roomClass.h"
//#include "include/ray.h"
//#include"stocasticRayGeneration.h"
//
//class Camera {
//public:
//	Vec3 eyePos, ll, ul, ur, lr;
//
//	Camera() {
//		eyePos = Vec3(0.5, 2.0, 2.0);
//		ll = Vec3(1.0, 1.5, 1.5);
//		ul = Vec3(1.0, 1.5, 2.5);
//		ur = Vec3(1.0, 2.5, 2.5);
//		lr = Vec3(1.0, 2.5, 1.5);
//	}
//
//	Ray generateViewRay(int x, int y, int width, int height) const {
//		double u = (x + 0.5) / width;
//		double v = 1.0 - (y + 0.5) / height;
//
//		Vec3 horizontal = lr - ll;
//		Vec3 vertical = ul - ll;
//
//		Vec3 pointOnImagePlane = ll + horizontal * u + vertical * v;
//
//		Vec3 ViewDir = (pointOnImagePlane - eyePos).normalize();
//
//		return Ray(eyePos, ViewDir);
//	}
//
//	// Generate a view ray from normalized pixel coordinates u in [0,1], v in [0,1]
//	Ray generateViewRayUV(double u, double v) const {
//		Vec3 horizontal = lr - ll;
//		Vec3 vertical = ul - ll;
//		Vec3 pointOnImagePlane = ll + horizontal * u + vertical * v;
//		Vec3 ViewDir = (pointOnImagePlane - eyePos).normalize();
//		return Ray(eyePos, ViewDir);
//	}
//
//	// Function generating n random rays through each pixel x,y in image plane of given height and width
//	// New version uses Gaussian distribution for non-uniform random importance sampling within each pixel
//	std::vector<Ray> generateRandomViewRays(int x, int y, int width, int height, int n) const {
//		std::vector<Ray> rays;
//		rays.reserve(n);
//
//		Vec3 horizontal = lr - ll;
//		Vec3 vertical = ul - ll;
//
//		int m = static_cast<int>(std::floor(std::sqrt(n)));
//		if (m < 1) m = 1;
//		int k = (n + m - 1) / m; 
//
//		// Local RNG
//		std::random_device rd;
//		std::mt19937 gen(rd());
//
//		// Gaussian distribution centered at 0.5, std dev controls spread
//		const double mean = 0.5; // Center of pixel
//		const double sigma = 0.15; // The spread of the distribution -increase/decrease to control focus
//		std::normal_distribution<> gaussian(mean, sigma);
//
//		for (int i = 0; i < m; ++i) {
//			for (int j = 0; j < k; ++j) {
//
//				if ((int)rays.size() >= n) break;
//
//				// Normalized stratum width and height
//				double du = 1.0 / (width * m);
//				double dv = 1.0 / (height * k);
//
//				// Base pixel coordinate in [0,1]
//				double u1 = (x + i / (double)m) / width;
//				double u2 = 1.0 - (y + j / (double)k) / height;
//
//				// Offset within stratum using Gaussian sampling in [-0.5, 0.5] scaled by stratum size
//				double offSetu1 = gaussian(gen) - 0.5;
//				double offSetu2 = gaussian(gen) - 0.5;
//
//				// Scale offset by stratum size and add to base
//				double u = std::clamp(u1 + offSetu1 * du, 0.0, 1.0);
//				double v = std::clamp(u2 + offSetu2 * dv, 0.0, 1.0);
//
//				// Generate ray through the sampled pixel location
//				Vec3 pointOnImagePlane = ll + horizontal * u + vertical * v;
//				// Ray's direction from eye to point on image plane and beyond 
//				Vec3 dir = (pointOnImagePlane - eyePos).normalize();
//
//				rays.emplace_back(eyePos, dir);
//			}
//			if ((int)rays.size() >= n) break;
//		}
//
//		return rays;
//	}
//};





// N-Rooks
//#pragma once
//
//#include "include/roomClass.h"
//#include "include/ray.h"
//#include"stocasticRayGeneration.h"
//
//#include <random>
//#include <algorithm> 
//#include <cmath>
//
//class Camera {
//public:
//	Vec3 eyePos, ll, ul, ur, lr;
//
//	Camera() {
//		eyePos = Vec3(0.5, 2.0, 2.0);
//		ll = Vec3(1.0, 1.5, 1.5);
//		ul = Vec3(1.0, 1.5, 2.5);
//		ur = Vec3(1.0, 2.5, 2.5);
//		lr = Vec3(1.0, 2.5, 1.5);
//	}
//
//	Ray generateViewRay(int x, int y, int width, int height) const {
//		double u = (x + 0.5) / width;
//		double v = 1.0 - (y + 0.5) / height;
//
//		Vec3 horizontal = lr - ll;
//		Vec3 vertical = ul - ll;
//
//		Vec3 pointOnImagePlane = ll + horizontal * u + vertical * v;
//
//		Vec3 ViewDir = (pointOnImagePlane - eyePos).normalize();
//
//		return Ray(eyePos, ViewDir);
//	}
//
//	// Generate a view ray from normalized pixel coordinates u in [0,1], v in [0,1]
//	Ray generateViewRayUV(double u, double v) const {
//		Vec3 horizontal = lr - ll;
//		Vec3 vertical = ul - ll;
//		Vec3 pointOnImagePlane = ll + horizontal * u + vertical * v;
//		Vec3 ViewDir = (pointOnImagePlane - eyePos).normalize();
//		return Ray(eyePos, ViewDir);
//	}
//
//	// Function generating n random rays through each pixel x,y in image plane of given height and width
//	// Uses N-Rooks sampling to ensure no two samples share the same row or column in the pixel subgrid.
//	std::vector<Ray> generateRandomViewRays(int x, int y, int width, int height, int n) const {
//		std::vector<Ray> rays;
//		rays.reserve(n);
//
//		Vec3 horizontal = lr - ll;
//		Vec3 vertical = ul - ll;
//
//		// RNG
//		std::random_device rd;
//		std::mt19937 gen(rd());
//		std::uniform_real_distribution<> uniform01(0.0, 1.0);
//
//		// Optional Gaussian importance sampling within each N-Rooks cell 
//		bool useGauss = true;
//		const double gaussMean = 0.5;
//		const double gaussSigma = 0.15;
//		std::normal_distribution<> gaussian(gaussMean, gaussSigma);
//
//		// Build a random permutation for columns (N-Rooks)
//		std::vector<int> perm(n);
//		for (int i = 0; i < n; ++i) perm[i] = i;
//		std::shuffle(perm.begin(), perm.end(), gen);
//
//		// Cell size in normalized pixel coordinates
//		const double invN = 1.0 / static_cast<double>(n);
//
//		double jitterU, double jitterV;
//
//		for (int i = 0; i < n; ++i) {
//
//			if (useGauss) {
//				// Gaussian-centered jitter
//				jitterU = std::clamp(gaussian(gen), 0.0, 1.0);
//				jitterV = std::clamp(gaussian(gen), 0.0, 1.0);
//			}
//			else {
//
//				// Uniform jitter inside the cell
//				jitterU = uniform01(gen);
//				jitterV = uniform01(gen);
//
//			}
//
//			// Normalized sample position inside the pixel using N-Rooks placement - one sample per
//			// row i and one sample per column perm[i]
//			double uSample = std::clamp((x + (i + jitterU) * invN) / static_cast<double>(width), 0.0, 1.0);
//			double vSample = std::clamp(1.0 - (y + (perm[i] + jitterV) * invN) / static_cast<double>(height), 0.0, 1.0);
//
//			Vec3 pointOnImagePlane = ll + horizontal * uSample + vertical * vSample;
//			Vec3 dir = (pointOnImagePlane - eyePos).normalize();
//
//			rays.emplace_back(eyePos, dir);
//		}
//
//		return rays;
//	}
//};

