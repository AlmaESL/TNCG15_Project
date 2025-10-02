#pragma once

#include"roomClass.h"
#include "vec3.h"
#include "ray.h"

class Tracer {
public:
	Tracer() : tClosest(0.0), normal(Vec3(0.0,0.0,0.0)), bestNormal(Vec3(0.0, 0.0, 0.0)),
		bestColor(Vec3(0.0, 0.0, 0.0)), hitPoint(Vec3(0.0, 0.0, 0.0)), hitType(""), hitMaterial("") {}

	bool trace(const Ray& ray, const Scene& scene, Vec3& hitColor,int depth,const int& maxDepth, const std::string& shadingMethod) {
		// Ray includes ray órigin and direction.
		// Scene includes all objects (speheres, planes, cubes, tetrahedrons),
		// light position, light color, light intensity, ambient color, and background color.
		// depth dictates how many bounces a ray has done, and maxDepth dictates how many bounces are allowed

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

		// Sphere Fresnel Reflection (no transmission yet)
		if (hitType == "SPHERE") {
			double refrIdx = 1.5;
			double cosTheta = -1.0 * ray.direction.dotProduct(bestNormal);
			double reflectance = schlickReflectance(cosTheta, refrIdx);

			if (depth < maxDepth) {
				Vec3 reflectDir = (ray.direction - (bestNormal * 2 * ray.direction.dotProduct(bestNormal))).normalize();
				Vec3 reflectOrigin = hitPoint + (reflectDir * 1e-4);
				Ray reflectRay = Ray(reflectOrigin, reflectDir);

				Vec3 reflectColour;
				trace(reflectRay, scene, reflectColour, (++depth), maxDepth, shadingMethod);

				hitColor = reflectColour * reflectance;
				return true;
			}else {
				hitColor = bestColor * scene.ambient;
				return true;
			}
		}

		// Mirror reflection with triangle object
		if (hitType=="TRIANGLE" && hitMaterial == "MIRROR" && (depth < maxDepth)) {
			Vec3 reflectDir = (ray.direction - (bestNormal * 2 * ray.direction.dotProduct(bestNormal))).normalize();
			Vec3 reflectOrigin = hitPoint + (reflectDir * 1e-4);
			Ray reflectRay = Ray(reflectOrigin, reflectDir);
			return trace(reflectRay, scene, hitColor, (++depth), maxDepth, shadingMethod);
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
				hitColor = (bestColor * ((scene.lightColor /* + scene.ambient*/) * intensity));
				return true;
			}
		}
	}

	bool shadowTest(const Ray& ray, const Scene& scene) {
		Ray sRay = Ray::shadowRay(hitPoint, scene.lightPos);

		double lightDist = sRay.origin.euclDist(scene.lightPos);

		for (const auto& sphere : scene.spheres) {
			double tsphere = sphere->RaySphereIntersection(sRay);
			if (tsphere > 0.0 && tsphere < lightDist) {
				return true;
			}
		}

		for (const auto& obj : scene.objs) {
			double t; Vec3 n, c;
			if (obj->intersect(sRay, t, n, c) && t < lightDist) {
				return true;
			}
		}

		return false;
	}

	double schlickReflectance(double cosTheta, double refrIdx) {
		double r0 = pow(((1 - refrIdx) / (1 + refrIdx)), 2);
		return pow((r0 + (1 - r0) * (1 - cosTheta)), 5);
	}

private:
	double tClosest;
	std::string hitMaterial;
	std::string hitType;
	Vec3 normal, color, bestNormal, bestColor, hitPoint;
};