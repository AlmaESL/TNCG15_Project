#pragma once

#include"roomClass.h"
#include "vec3.h"
#include "ray.h"

class Tracer {
public:
	Tracer() : tClosest(0.0), normal(Vec3(0.0,0.0,0.0)), bestNormal(Vec3(0.0, 0.0, 0.0)),
					bestColor(Vec3(0.0, 0.0, 0.0)), hitPoint(Vec3(0.0,0.0,0.0)){}

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
				hit = true;
			}
		}

		return hit;
	}

	bool shadowTest(const Ray& ray, const Scene& scene , Vec3& hitColor) {
		intersections(ray, scene);

		// Ray's hit point with a scene surface
		hitPoint = ray.origin + ray.direction * tClosest;

		Ray sRay = Ray::shadowRay(hitPoint, scene.lightPos);

		double lightDist = sRay.origin.euclDist(scene.lightPos);

		bool hit = intersections(sRay, scene);

		if (hit && (tClosest < lightDist)) {
			hitColor = (bestColor * (scene.lightColor + scene.ambient));
			return true;
		}else {
			return false;
		}
	}

	bool flatTrace(const Ray& ray, const Scene& scene, Vec3& hitColor) {
		bool hit = intersections(ray, scene);

		// Flat shading
		if (hit) {
			hitColor = bestColor;
			return true;
		}
		else {
			hitColor = scene.backgroundColor;
			return false;
		}
	}

	bool lambTrace(const Ray& ray, const Scene& scene, Vec3& hitColor){

		bool hit = intersections(ray, scene);

		// Lambertian shading
		if (hit) {

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
			hitColor =  (bestColor * (scene.lightColor* intensity));

		}
		else {
			hitColor = scene.backgroundColor;
			return false;
		}
	}
private:
	double tClosest;
	Vec3 normal, color, bestNormal, bestColor, hitPoint;
};