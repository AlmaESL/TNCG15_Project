#pragma once

#include"roomClass.h"
#include "vec3.h"
#include "ray.h"

class Tracer {
public:
	Tracer() : tClosest(0.0), normal(Vec3(0.0,0.0,0.0)), bestNormal(Vec3(0.0, 0.0, 0.0)),
					bestColor(Vec3(0.0, 0.0, 0.0)), hitPoint(Vec3(0.0,0.0,0.0)){}

	bool trace(const Ray& ray, const Scene& scene, Vec3& hitColor,int depth = 0, int maxDepth = 0, std::string shadingMethod = "FLAT") {
		// Ray includes ray órigin and direction.
		// Scene includes all objects (speheres, planes, cubes, tetrahedrons),
		// light position, light color, light intensity, ambient color, and background color.
		// depth dictates how many bounces a ray has done, and maxDepth dictates how many bounces are allowed

		// Initilize values related to ray intersection
		bool hit = false;
		tClosest = std::numeric_limits<double>::infinity();

		// Check intersection for all triangle-based objects
		for (const auto& plane : scene.planes) {
			double t; Vec3 n, c;
			if (plane->intersect(ray, t, n, c) && t < tClosest) {
				tClosest = t;
				bestNormal = n;
				bestColor = c;
				hitMaterial = plane->material;
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
				hitMaterial = sphere->material;
				hit = true;
			}
		}

		// Check intersection for all cubes
		for (const auto& cube : scene.cubes) {
			double t; Vec3 n, c;

			if (cube->intersect(ray, t, n, c) && t < tClosest) {
				tClosest = t;
				bestNormal = n;
				bestColor = c;
				hitMaterial = cube->getMat();
				hit = true;
			}
		}

		// Check intersection for all tetras
		for (const auto& tetra : scene.tetrahedrons) {
			double t; Vec3 n, c;

			if (tetra->intersect(ray, t, n, c) && t < tClosest) {
				tClosest = t;
				bestNormal = n;
				bestColor = c;
				hitMaterial = tetra->getMat();
				hit = true;
			}
		}

		// If nothing in the scene was hit
		if (!hit) {
			hitColor = scene.backgroundColor;
			return false;
		}

		// Mirror reflection
		if (hitMaterial == "MIRROR" && (depth < maxDepth)) {
			Vec3 reflectDir = (ray.direction - (bestNormal * 2 * ray.direction.dotProduct(bestNormal))).normalize();
			Vec3 reflectOrigin = hitPoint + (reflectDir * 1e-4);
			Ray reflectRay = Ray(reflectOrigin, reflectDir);
			trace(reflectRay, scene, hitColor, (depth + 1), maxDepth, shadingMethod);
		}

		// Flat shading
		if (shadingMethod == "FLAT") {
			hitColor = bestColor;
			return true;
		}

		// Lambertian shading
		if (shadingMethod == "LAMBERTIAN") {
			if (shadowTest(ray, scene)) {
				hitColor = (bestColor * scene.ambient);
				return true;
			}else {
				// Reset ray intersections after shadow test
				intersections(ray, scene);
				// Ray's hit point with a scene surface
				hitPoint = ray.origin + ray.direction * tClosest;

				Vec3 lightVec = scene.lightPos - hitPoint;
				Vec3 lightDir = lightVec.normalize();

				// Lambertian relection factor
				double diff = std::max(0.0, bestNormal.dotProduct(lightDir));
				// Compute squared distance from light source to intersection surface point - squared distance since light
				// intesnity decreases by squared distance
				double distance2 = lightVec.dotProduct(lightVec);

				// Intensity of the reflection
				double intensity = scene.lightIntensity * diff / distance2;

				// Compute the color of the current pixel
				hitColor = (bestColor * (scene.lightColor * intensity));
				return true;
			}
		}
		
	}

	bool intersections(const Ray& ray, const Scene& scene){
		bool hit = false;
		tClosest = std::numeric_limits<double>::infinity();

		// Check intersection for all triangle-based objects
		for (const auto& plane : scene.planes) {
			double t; Vec3 n, c;
			if (plane->intersect(ray, t, n, c) && t < tClosest) {
				tClosest = t;
				bestNormal = n;
				bestColor = c;
				hitMaterial = plane->material;
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
				hitMaterial = sphere->material;
				hit = true;
			}
		}

		// Check intersection for all cubes
		for (const auto& cube : scene.cubes) {
			double t; Vec3 n, c;

			if (cube->intersect(ray, t, n, c) && t < tClosest) {
				tClosest = t;
				bestNormal = n;
				bestColor = c;
				hitMaterial = cube->getMat();
				hit = true;
			}
		}

		// Check intersection for all tetras
		for (const auto& tetra : scene.tetrahedrons) {
			double t; Vec3 n, c;

			if (tetra->intersect(ray, t, n, c) && t < tClosest) {
				tClosest = t;
				bestNormal = n;
				bestColor = c;
				hitMaterial = tetra->getMat();
				hit = true;
			}
		}

		return hit;
	}

	bool shadowTest(const Ray& ray, const Scene& scene) {
		hitPoint = ray.origin + ray.direction * tClosest;

		Ray sRay = Ray::shadowRay(hitPoint, scene.lightPos);

		double lightDist = sRay.origin.euclDist(scene.lightPos);

		bool hit = intersections(sRay, scene);

		if (hit && (tClosest < lightDist)) {
			return true;
		}else {
			return false;
		}
	}

private:
	double tClosest;
	std::string hitMaterial;
	Vec3 normal, color, bestNormal, bestColor, hitPoint;
};