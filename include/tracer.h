#pragma once

#include"roomClass.h"
#include "vec3.h"
#include "ray.h"
#include <random>

class Tracer {
public:
	Tracer() : tClosest(0.0), normal(Vec3(0.0, 0.0, 0.0)), bestNormal(Vec3(0.0, 0.0, 0.0)),
		bestColor(Vec3(0.0, 0.0, 0.0)), hitPoint(Vec3(0.0, 0.0, 0.0)), hitType(""), hitMaterial("") {
	}

	bool trace(const Ray& ray, const Scene& scene, Vec3& hitColor, int depth, const int& maxDepth, const std::string& shadingMethod, int& raySegments) {
		// Ray includes ray origin and direction.
		// Scene includes all objects (speheres, planes, cubes, tetrahedrons),
		// light position, light color, light intensity, ambient color, and background color.
		// depth dictates how many bounces a ray has done, and maxDepth dictates how many bounces are allowed

		// For statistics
		raySegments += 1;

		// Test needed for Whitted ray termination
		if (depth >= maxDepth) {
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
		if (hitType == "SPHERE" && hitMaterial == "MIRROR") {
			Vec3 reflectDir = (ray.direction - (bestNormal * 2 * ray.direction.dotProduct(bestNormal))).normalize();
			Vec3 reflectOrigin = hitPoint + (reflectDir * 1e-4);
			Ray reflectRay = Ray(reflectOrigin, reflectDir);
			return trace(reflectRay, scene, hitColor, (++depth), maxDepth, shadingMethod, raySegments);
		}

		// Sphere Fresnel reflection + refraction (only for transparent materials)
		if (hitMaterial == "GLASS") {

			// Refraction index for glass is [1.5,1.9]
			double refrIdx = 1.5;

			// Intitalize normal n and eta ratio (refrIdx1 / refrIdx2 for snell's law)
			bool frontFace = ray.direction.dotProduct(bestNormal) < 0.0;
			Vec3 n = frontFace ? bestNormal : bestNormal * -1.0;
			double etaRatio = frontFace ? (1.0 / refrIdx) : (refrIdx / 1.0);
			double cosTheta = std::max(0.0, std::min(1.0, -ray.direction.dotProduct(n)));

			// Schlick refelectance to get the reflection coefficent 
			double R = schlickReflectance(cosTheta, refrIdx);

			// Randomly choose reflection or refraction using Fresnel R
			static thread_local std::mt19937 gen(std::random_device{}());
			std::uniform_real_distribution<double> dis(0.0, 1.0);
			double rnd = dis(gen);

			Vec3 nextColor(0.0);

			// Choose reflection if random num smaller than R
			if (rnd < R) {
				// Reflect
				Vec3 reflectDir = (ray.direction - n * 2.0 * ray.direction.dotProduct(n)).normalize();
				Ray reflectRay(hitPoint + reflectDir * 1e-4, reflectDir);
				trace(reflectRay, scene, nextColor, depth + 1, maxDepth, shadingMethod, raySegments);
				hitColor = nextColor;  // no weighting needed, reflection already sampled with prob R
			}
			// Choose refraction if random num larger than R
			else {

				Vec3 refractDir = refractRay(ray.direction, n, etaRatio);

				if (refractDir.getLength() == 0.0) {
					// Total internal reflection fallback
					Vec3 reflectDir = (ray.direction - n * 2.0 * ray.direction.dotProduct(n)).normalize();
					Ray reflectRay(hitPoint + reflectDir * 1e-4, reflectDir);
					trace(reflectRay, scene, nextColor, depth + 1, maxDepth, shadingMethod, raySegments);
					hitColor = nextColor;
				}
				// If there's not total internal reflection we do refraction too
				else {
					Ray refrRay(hitPoint + refractDir * 1e-4, refractDir.normalize());
					trace(refrRay, scene, nextColor, depth + 1, maxDepth, shadingMethod, raySegments);

					// Apply tinting
					hitColor = nextColor * bestColor;
				}
			}
			return true;
		}


		// Mirror reflection with triangle object
		if (hitType == "TRIANGLE" && hitMaterial == "MIRROR") {
			Vec3 reflectDir = (ray.direction - (bestNormal * 2 * ray.direction.dotProduct(bestNormal))).normalize();
			Vec3 reflectOrigin = hitPoint + (reflectDir * 1e-4);
			Ray reflectRay = Ray(reflectOrigin, reflectDir);
			return trace(reflectRay, scene, hitColor, (++depth), maxDepth, shadingMethod, raySegments);
		}

		/// Flat shading
		if (shadingMethod == "FLAT") {
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

			// Sample new ray direction using CDF hemisphere sampling, only 1 child ray per surface interaction
			StocasticRayGeneration sampler(hitPoint + bestNormal * 1e-4, 1, bestNormal);

			Vec3 totalColor(0.0);

			// Check all the rays from sampler
			for (size_t i = 0; i < sampler.rays.size(); ++i) {

				// Check if new ray, intersects the area light source
				bool directLightHit = false;
				for (const auto& obj : scene.objs) {
					double t; Vec3 n, c;
					if (obj->getMat() == "EMISSIVE" && obj->intersect(sampler.rays[i], t, n, c)) {
						directLightHit = true;
						break;
					}
				}

				if (directLightHit) {

					double tLight = std::numeric_limits<double>::infinity();

					for (const auto& obj : scene.objs) {
						double t2; Vec3 n2, c2;
						if (obj->getMat() == "EMISSIVE" && obj->intersect(sampler.rays[i], t2, n2, c2)) {
							// take the nearest emissive intersection if multiple (defensive)
							if (t2 > 0.0 && t2 < tLight) tLight = t2;
						}
					}

					// Sample the light directly, treating light as a point
					if (tLight == std::numeric_limits<double>::infinity()) {
						directLighting = bestColor * scene.ambient;

					}
					else {
						// Direction toward the light is the sampled ray's direction (already)
						Vec3 toLightDir = sampler.rays[i].direction.normalize();

						// Squared distance to emitter
						double dist2 = tLight * tLight;

						// Shadow ray to check visibility, small offset to avoid self-intersection
						Ray shadowRay(hitPoint + bestNormal * 1e-4, toLightDir);

						// Iterate all objects in the scene and test for occlusion
						bool occluded = false;

						for (const auto& obj : scene.objs) {
							// Transparent materials don't occlude
							if (obj->isTransparent()) continue;
							double t3; Vec3 n3, c3;
							if (obj->intersect(shadowRay, t3, n3, c3) && t3 > 1e-6 && t3 < tLight) {
								occluded = true;
								break;
							}
						}

						if (!occluded) {
							for (const auto& s : scene.spheres) {
								if (s->isTransparent()) continue;
								double ts = s->RaySphereIntersection(shadowRay);
								if (ts > 1e-6 && ts < tLight) {
									occluded = true;
									break;
								}
							}
						}

						if (occluded) {
							directLighting = bestColor * scene.ambient;

						}
						else {
							// Lambertian term using NdotL and light intensity / squared distance
							double NdotL = std::max(0.0, bestNormal.dotProduct(toLightDir));

							// Incident irradiance from point light - intensity decreases with squared distance
							Vec3 irradiance = scene.lightColor * (scene.lightIntensity / dist2);

							// Get the direct light contribution
							directLighting = albedo * irradiance * NdotL;
						}
					}
				}

				// Intitialize incoming color
				Vec3 incoming(0.0);

				// Recursively trace for all rays
				bool hitSomething = trace(sampler.rays[i], scene, incoming, depth + 1, maxDepth, shadingMethod, raySegments);

				// If no hit, incoming color is background color
				if (!hitSomething) {
					incoming = scene.backgroundColor;
				}

				// Initialize survival probability for Russian roulette
				double survivalProb = 1.0;

				// Start Russian roulette after some depth to terminate low-contribution paths
				if (depth >= (rrDepth - 1)) {

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
						totalColor += directLighting;
						continue;
					}
				}

				// For cosine-weighted sampling, cos/pdf cancels -> contribution = albedo * incoming
				Vec3 indirectLighting = albedo * incoming;
				if (depth >= (rrDepth - 1) && survivalProb > 0.0)
					indirectLighting = indirectLighting / survivalProb;


				// Combine direcr and indirect light contributions
				totalColor += directLighting + indirectLighting;
			}
			hitColor = totalColor / double(sampler.rays.size());

			hitColor.x = std::min(1.0, std::max(0.0, hitColor.x));
			hitColor.y = std::min(1.0, std::max(0.0, hitColor.y));
			hitColor.z = std::min(1.0, std::max(0.0, hitColor.z));

			return true;
		}
		hitColor = scene.backgroundColor;

		hitColor.x = std::min(1.0, std::max(0.0, hitColor.x));
		hitColor.y = std::min(1.0, std::max(0.0, hitColor.y));
		hitColor.z = std::min(1.0, std::max(0.0, hitColor.z));

		return false;
	}

	// Test for shadow rays via occlusion
	bool shadowTest(const Scene& scene) const {

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
		double r0 = pow((refrIdx - 1) / (refrIdx + 1), 2);

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