#pragma once

#include"roomClass.h"
#include "vec3.h"
#include "ray.h"

class Tracer {
public:
	Tracer() : tClosest(0.0), normal(Vec3(0.0, 0.0, 0.0)), bestNormal(Vec3(0.0, 0.0, 0.0)),
		bestColor(Vec3(0.0, 0.0, 0.0)), hitPoint(Vec3(0.0, 0.0, 0.0)), hitType(""), hitMaterial("") {
	}

	bool trace(const Ray& ray, const Scene& scene, Vec3& hitColor, int depth, const int& maxDepth, const std::string& shadingMethod) {
		// Ray includes ray origin and direction.
		// Scene includes all objects (speheres, planes, cubes, tetrahedrons),
		// light position, light color, light intensity, ambient color, and background color.
		// depth dictates how many bounces a ray has done, and maxDepth dictates how many bounces are allowed

		// Test needed for Whitted ray termination
		if (depth >= maxDepth && shadingMethod != "MC") {
			hitColor = bestColor;
			return true;
		}

		// Initilize values related to ray intersection
		bool hit = false;
		tClosest = std::numeric_limits<double>::infinity();

		// Check intersection for all triangle-based objects
		for (const auto& obj : scene.objs) {
			double t; Vec3 n, c;
			if (obj->intersect(ray, t, n, c) && t < tClosest) {
				tClosest = t;
				bestNormal = n;
				bestColor = c;
				hitPoint = ray.origin + ray.direction * tClosest;
				hitType = "TRIANGLE";
				hitMaterial = obj->getMat();
				hit = true;
			}
		}

		// Check intersection for all spheres 
		for (const auto& sphere : scene.spheres) {
			double tsphere = sphere->RaySphereIntersection(ray);

			if (tsphere > 0.0 && tsphere < tClosest) {
				tClosest = tsphere;
				hitPoint = ray.origin + ray.direction * tsphere;
				bestNormal = (hitPoint - sphere->centerPoint).normalize();
				bestColor = sphere->color;
				hitType = "SPHERE";
				hitMaterial = sphere->material;
				hit = true;
			}
		}

		// If nothing in the scene was hit
		if (!hit) {
			hitColor = scene.backgroundColor;
			return false;
		}

		// Sphere perfect mirror material
		if (hitType == "SPHERE" && hitMaterial == "MIRROR" && (depth < maxDepth)) {
			Vec3 reflectDir = (ray.direction - (bestNormal * 2 * ray.direction.dotProduct(bestNormal))).normalize();
			Vec3 reflectOrigin = hitPoint + (reflectDir * 1e-4);
			Ray reflectRay = Ray(reflectOrigin, reflectDir);
			return trace(reflectRay, scene, hitColor, (++depth), maxDepth, shadingMethod);
		}

		// Sphere Fresnel reflection + refraction (only for transparent materials)
		if (hitType == "SPHERE" && hitMaterial == "GLASS" && (depth < maxDepth)) {

			// Refraction index for glass is [1.5,1.9]
			double refrIdx = 1.5;

			// Determine if the ray is inside or outside the surface
			bool frontFace = ray.direction.dotProduct(bestNormal) < 0.0;

			// Intitalize normal n and eta ratio (refrIdx1 / refrIdx2 for snell's law)
			double etaRatio;
			Vec3 n;

			// If the ray is outside the surface, eta is air / material refrIdx
			if (frontFace) {
				etaRatio = 1.0 / refrIdx;
				n = bestNormal;
			}
			// If the ray is inside the surface, eta is material refrIdx / air
			else {
				etaRatio = refrIdx / 1.0;
				n = bestNormal * (-1.0);
			}

			// Use the chosen normal and clamp cosTheta to [0,1]
			double cosTheta = -1.0 * ray.direction.dotProduct(n);
			cosTheta = std::max(0.0, std::min(1.0, cosTheta));

			// Schlick refelectance to get the reflection coefficent 
			double R = schlickReflectance(cosTheta, refrIdx);

			// Randomly choose reflection or refraction using Fresnel R
			double rnd = (double)std::rand() / RAND_MAX;

			// Choose reflection if random num smaller than R
			if (rnd < R) {

				// Reflect ray direction around normal
				Vec3 reflectDir = (ray.direction - (bestNormal * 2 * ray.direction.dotProduct(bestNormal))).normalize();
				Vec3 reflectOrigin = hitPoint + (reflectDir * 1e-4);
				Ray reflectRay = Ray(reflectOrigin, reflectDir);
				return trace(reflectRay, scene, hitColor, depth + 1, maxDepth, shadingMethod);
			}
			// Choose refraction if random num larger than R
			else {
				Vec3 refractDir = refractRay(ray.direction, n, etaRatio);
				// If total internal reflection, fall back to reflection
				if (refractDir.getLength() <= 0.0) {
					Vec3 reflectDir = (ray.direction - (bestNormal * 2 * ray.direction.dotProduct(bestNormal))).normalize();
					Vec3 reflectOrigin = hitPoint + (reflectDir * 1e-4);
					Ray reflectRay = Ray(reflectOrigin, reflectDir);
					return trace(reflectRay, scene, hitColor, depth + 1, maxDepth, shadingMethod);
				}
				Vec3 refractOrigin = hitPoint + (refractDir * 1e-4);
				Ray refrRay = Ray(refractOrigin, refractDir.normalize());
				return trace(refrRay, scene, hitColor, depth + 1, maxDepth, shadingMethod);
			}
		}

		// Mirror reflection with triangle object
		if (hitType == "TRIANGLE" && hitMaterial == "MIRROR" && (depth < maxDepth)) {
			Vec3 reflectDir = (ray.direction - (bestNormal * 2 * ray.direction.dotProduct(bestNormal))).normalize();
			Vec3 reflectOrigin = hitPoint + (reflectDir * 1e-4);
			Ray reflectRay = Ray(reflectOrigin, reflectDir);
			return trace(reflectRay, scene, hitColor, (++depth), maxDepth, shadingMethod);
		}

		/// Flat shading
		if (shadingMethod == "FLAT") {
			std::cout << "hi" << std::endl;
			hitColor = bestColor;
			return true;
		}

		/// Lambertian shading
		if (shadingMethod == "LAMBERTIAN") {
			if (shadowTest(scene)) {
				hitColor = (bestColor * scene.ambient);
				return true;
			}
			else {
				Vec3 lightVec = scene.lightPos - hitPoint;
				Vec3 lightDir = lightVec.normalize();

				// Lambertian relection factor
				double diff = std::max(0.0, bestNormal.dotProduct(lightDir));
				// Compute squared distance from light source to intersection surface point - squared distance since light
				// intesnity decreases by squared distance
				double distance2 = lightVec.dotProduct(lightVec);

				// Intensity of the reflection
				double intensity = scene.lightIntensity * diff / distance2;

				// Compute the color of the current pixel (ambient + direct)
				hitColor = bestColor * ((scene.lightColor * intensity) + scene.ambient);
				return true;
			}
		}


		/// MC Tracing 
		if (shadingMethod == "MC") {

			// Color of the surface, used when multiplying incoming light
			Vec3 albedo = bestColor;

			// For importance sampling of direct light, we do one direct shadow ray test
			Vec3 directLighting(0.0);

			// Indirect lighting uses hemisphere cosine-weighted sample --> this has to be same as maxDepth in
			// Renderer.h --> TODO: Fix this so we only have to change in one place, should only be to set this to max depth
			const int rrDepth = maxDepth;

			// Sample new ray direction using CDF hemisphere sampling
			StocasticRayGeneration sampler(hitPoint + bestNormal * 1e-4, 1, bestNormal);

			// Take the first ray only - single sample per bounce (can have deeper ray trees later)
			//Ray newRay = sampler.rays[0];

			// Check all the rays from sampler
			for (size_t i = 0; i < sampler.rays.size(); ++i) {
				// Check if new ray, intersects the area light source
				bool directLightHit = false;
				for (const auto& obj : scene.lightSources) {
					double t; Vec3 n, c;
					if (obj->getMat() == "EMISSIVE" && obj->intersect(sampler.rays[i], t, n, c)) {
						directLightHit = true; 
						break;
					}
				}

				if (directLightHit) {
					// Sample the light directly, treating light as a point
					Vec3 toLight = sampler.rays[i].direction;
					double dist2 = toLight.dotProduct(toLight);
					Vec3 lightDir = toLight.normalize();

					// Shadow ray to check visibility, small offset to avoid self-intersection
					Ray shadowRay(hitPoint + bestNormal * 1e-4, lightDir);
					// Occulusion bool
					bool occluded = false;

					// Iterate all objects in the scene and test for occlusion
					for (const auto& obj : scene.objs) {
						double t; Vec3 n, c;
						if (obj->intersect(shadowRay, t, n, c) && t * t < dist2) {
							occluded = true;
							break;
						}
					}
					for (const auto& s : scene.spheres) {
						double ts = s->RaySphereIntersection(shadowRay);
						if (ts > 0.0 && ts * ts < dist2) {
							occluded = true;
							break;
						}
					}

					if (!occluded) {
						// Lambertian term
						double NdotL = std::max(0.0, bestNormal.dotProduct(lightDir));

						// Incident irradiance from point light - intensity decreases with squared distance
						Vec3 irradiance = scene.lightColor * (scene.lightIntensity / dist2);

						// Get the direct light contribution
						directLighting = albedo * irradiance * NdotL;
					}
				}

				// Intitialize incoming color
				Vec3 incoming;

				// Recursively trace for all rays
				bool hitSomething = trace(sampler.rays[i], scene, incoming, depth + 1, maxDepth, shadingMethod);

				// If no hit, incoming color is background color
				if (!hitSomething) {
					incoming = scene.backgroundColor;
				}

				// Initialize survival probability for Russian roulette
				double survivalProb = 1.0;

				// Start Russian roulette after some depth to terminate low-contribution paths
				if (depth >= rrDepth) {

					// Pick survival based on the best hit color --> TODO: Test other russian roulette methods?
					double maxAlbedo = std::max({ albedo.x, albedo.y, albedo.z });

					// Clamp survival probability to avoid too many terminated paths
					survivalProb = std::min(0.95, maxAlbedo);

					// Generate random number and terminate if above survival probability
					static thread_local std::mt19937 rrGen(std::random_device{}());
					std::uniform_real_distribution<double> urnif(0.0, 1.0);

					// Ray termination = setting hit color to direct light only
					if (urnif(rrGen) >= survivalProb) {
						hitColor = directLighting;
						return true;
					}
				}

				// For cosine-weighted sampling, cos/pdf cancels -> contribution = albedo * incoming
				Vec3 indirectLighting = albedo * incoming;
				if (depth >= rrDepth && survivalProb > 0.0)
					indirectLighting = indirectLighting / survivalProb;

				// Combine direcr and indirect light contributions
				hitColor = directLighting + indirectLighting;
				return true;
			}
		}
		hitColor = scene.backgroundColor;
		return false;
	}

	// Test for shadow rays via occlusion
	bool shadowTest(const Scene& scene) const{

		// Create shadow ray from the surface hit point and the light source 
		Ray sRay = Ray::shadowRay(hitPoint, scene.lightPos);

		// Get the euclidean distance from the light source and the hit point 
		double lightDist = sRay.origin.euclDist(scene.lightPos);

		for (const auto& sphere : scene.spheres) {
			// Transparent materials don't occlude
			if (sphere->isTransparent()) continue;

			// Check shadow ray intersection with spheres
			double tsphere = sphere->RaySphereIntersection(sRay);
			if (tsphere > 0.0 && tsphere < lightDist) {
				return true;
			}
		}

		for (const auto& obj : scene.objs) {
			// Transparent materials don't occlude
			if (obj->isTransparent()) continue;
			double t; Vec3 n, c;
			// Check shadow ray intersection with triangles
			if (obj->intersect(sRay, t, n, c) && t < lightDist) {
				return true;
			}
		}

		// No occlusion found
		return false;
	}

	// Schlick's approximation for Fresnel reflectance
	double schlickReflectance(double cosTheta, double refrIdx) {

		// Compute the reflectance at normal incidence
		double r0 = pow((1 - refrIdx) / (1 + refrIdx), 2);

		// Approximate reflectance for all angles using Schlick's approximation
		return r0 + (1 - r0) * pow((1 - cosTheta), 5);
	}

	// Ray refraction with Snell's law
	Vec3 refractRay(Vec3 dir, Vec3 n, double eta) {
		double cosTheta = -1.0 * dir.dotProduct(n);
		double sin2Theta = 1.0 - (cosTheta * cosTheta);
		double k = 1.0 - (eta * eta * sin2Theta);

		if (k < 0) {
			return Vec3(0.0, 0.0, 0.0);
		}
		else {
			return (dir * eta) + n * (eta * cosTheta - sqrt(k));
		}
	}

private:
	double tClosest;
	std::string hitMaterial;
	std::string hitType;
	Vec3 normal, color, bestNormal, bestColor, hitPoint;
};