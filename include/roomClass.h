#pragma once

#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>

#include "include/vec3.h"
#include "include/ray.h"


/*
* Classes for scene geometry, such as  triangles, objects and cube for the room itself
*/

/// Triangle class, the basis for all scene geoemtry 
class Triangle {
public:
	Vec3 v0, v1, v2;
	Vec3 color;
	Vec3 edge0, edge1;
	Vec3 normal;

	// Constructor 
	Triangle(const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& col) : v0(a), v1(b), v2(c), color(col) {
		edge0 = v1 - v0;
		edge1 = v2 - v0;
		normal = computeTriangeNormal();
	}

	/// Moller-Trumbore ray-triangle intersection algorithm --> object class uses this function, hence static
	static double RayTriangleIntersect(const Vec3& rayOrigin, const Vec3& rayDir, const Triangle& tri) {
		const double EPSILON = 1e-8;

		// Check if triangle normal and ray direction are anti-parallel
		double dotTest = tri.normal.dotProduct(rayDir);
		if (dotTest > -EPSILON) {
			return -1.0; // ray is parallel or points away
		}

		// Geometric Moller-Trumbore
		Vec3 R1 = rayDir.crossProduct(tri.edge1);
		double Cs = tri.edge0.dotProduct(R1);

		if (fabs(Cs) < EPSILON) return -1.0; // ray is parallel to triangle

		Vec3 C3 = rayOrigin - tri.v0;
		Vec3 R2 = C3.crossProduct(tri.edge0);

		double t = tri.edge1.dotProduct(R2) / Cs;
		double u = C3.dotProduct(R1) / Cs;
		double v = rayDir.dotProduct(R2) / Cs;

		if (t > EPSILON && u >= 0.0 && v >= 0.0 && (u + v) <= 1.0) {
			return t; // valid intersection
		}

		return -1.0;
	}

private:
	// Compute normalized triangle normal 
	Vec3 computeTriangeNormal() const {
		Vec3 n = edge0.crossProduct(edge1).normalize();

		// Ensure normals point inwards to the room 
		Vec3 roomCenter(2.0, 2.0, 2.0); 
		Vec3 triangleCentroid = (v0 + v1 + v2) / 3.0; 

		Vec3 dirToCenter = (roomCenter - triangleCentroid).normalize(); 

		// Flip normal if it points outwards 
		if (n.dotProduct(dirToCenter) < 0.0) {
			n = n * -1.0; 
		}

		return n;
	}

	// Print triangle vertices and normal --> debug 
	void printTriangleInfo() const {
		std::cout << "Triangle vertices: " << v0.x << "," << v0.y << "," << v0.z << " - "
			<< v1.x << "," << v1.y << "," << v1.z << " - "
			<< v2.x << "," << v2.y << "," << v2.z << "\n";
		std::cout << std::endl << "Triangle normal: " << normal.x << "," << normal.y << "," << normal.z << "\n";
	}
};



///  Class of triangle objects such as triangles or cubes 
class Object {
public:
	std::vector<Triangle> triangles;

	void addTriangle(const Triangle& tri) {
		triangles.push_back(tri);
	}

	// Function to check ray intersection with all triangles in the object
	bool intersect(const Ray& ray, double& tHit, Vec3& outNormal, Vec3& outColor) const {

		bool hit = false;
		double tClosest = std::numeric_limits<double>::infinity();

		// Loop all triangles and check for intersection
		for (const auto& tri : triangles) {
			double t = Triangle::RayTriangleIntersect(ray.origin, ray.direction, tri);

			// If there is an intersection and its the closest one, store it and take the color
			if (t > 0.0 && t < tClosest) {
				hit = true;
				tClosest = t;
				outColor = tri.color;
				outNormal = tri.normal;
				if (outNormal.dotProduct(ray.direction) > 0.0) {
					outNormal = outNormal * -1.0; // flip so it faces the incoming ray
				}
			}
		}

		if (hit) {
			tHit = tClosest;
		}

		return hit;
	}

};

class Sphere {
public: 
	Vec3 centerPoint; 
	double radius; 
	Vec3 color; 

	Sphere(const Vec3& c, double r, const Vec3& col) : centerPoint(c), radius(r), color(col) {}

	// Ray intersection test for spheres 
	bool RaySphereIntersection(const Ray& ray, double& tHit, Vec3& outNormal, Vec3& outColor) const {

		// Compute the vector distance from sphere center to ray
		Vec3 L = centerPoint - ray.origin; 

		double t_ca = L.dotProduct(ray.direction); 
		double d2 = L.dotProduct(L) - t_ca * t_ca; 

		if (d2 > std::pow(radius, 2)) {
			return false; 
		}

		double t_hc = std::sqrt(std::pow(radius, 2) - d2); 
		double t0 = t_ca - t_hc; 
		double t1 = t_ca + t_hc; 

		if (t1 < 0) {
			return false; 
		}

		tHit = (t0 > 0) ? t0 : t1; 

		Vec3 hitPoint = ray.origin + ray.direction * tHit; 
		outNormal = (hitPoint - centerPoint).normalize();
		outColor = color; 

		return true; 

	}

}; 


/// Class to make the scene room, that ie a cube 
class Scene {

public:
	std::vector<std::shared_ptr<Object>> objs;
	std::vector<std::shared_ptr<Sphere>> spheres;

	/*Vec3 lightPos = Vec3(4, 2, 10);*/
	Vec3 lightPos = Vec3(2, 2, 3);
	Vec3 lightColor = Vec3(1, 1, 1);
	double lightIntensity = 300.0;
	Vec3 backgroundColor = Vec3(0.05, 0.05, 0.05);

	// Constructor adds the room cube and a camera
	Scene() {

		Vec3 v0(0, 0, 0);
		Vec3 v1(4, 0, 0);
		Vec3 v2(4, 4, 0);
		Vec3 v3(0, 4, 0);
		Vec3 v4(0, 0, 4);
		Vec3 v5(4, 0, 4);
		Vec3 v6(4, 4, 4);
		Vec3 v7(0, 4, 4);
		
		// Floor - White
		auto floor = std::make_shared<Object>();
		floor->addTriangle(Triangle(v0, v1, v2, Vec3(0.8, 0.8, 0.8)));
		floor->addTriangle(Triangle(v0, v2, v3, Vec3(0.8, 0.8, 0.8)));
		addObject(floor);

		// The wall behind the camera
		/*auto leftWall = std::make_shared<Object>();
		leftWall->addTriangle(Triangle(v0, v3, v7, Vec3(0, 1, 0)));
		leftWall->addTriangle(Triangle(v0, v7, v4, Vec3(0, 1, 0)));
		addObject(leftWall);*/

		// Back wall - green
		auto rightWall = std::make_shared<Object>();
		rightWall->addTriangle(Triangle(v1, v5, v6, Vec3(0, 1, 0)));
		rightWall->addTriangle(Triangle(v1, v6, v2, Vec3(0, 1, 0)));
		addObject(rightWall);

		// Left wall - blue
		auto backWall = std::make_shared<Object>();
		backWall->addTriangle(Triangle(v0, v4, v5, Vec3(0, 0, 1)));
		backWall->addTriangle(Triangle(v0, v5, v1, Vec3(0, 0, 1)));
		addObject(backWall);

		// Right wall - yellow
		auto frontWall = std::make_shared<Object>();
		frontWall->addTriangle(Triangle(v3, v2, v6, Vec3(1, 1, 0)));
		frontWall->addTriangle(Triangle(v3, v6, v7, Vec3(1, 1, 0)));
		addObject(frontWall);

		// Ceiling - white
		auto ceiling = std::make_shared<Object>();
		ceiling->addTriangle(Triangle(v4, v7, v6, Vec3(0.8, 0.8, 0.8)));
		ceiling->addTriangle(Triangle(v4, v6, v5, Vec3(0.8, 0.8, 0.8)));
		addObject(ceiling);

		// Add a sphere to the scene
		Vec3 sphereCenterPoint(2.0, 2.0, 2.0);
		Vec3 sphereColor(0.9, 0.9, 0.9); 
		double sphereRadius = 0.4; 
		auto sphere = std::make_shared<Sphere>(sphereCenterPoint, sphereRadius, sphereColor);
		addSphere(sphere);
	}

	void addObject(const std::shared_ptr<Object>& obj) {
		objs.push_back(obj);
	}

	void addSphere(const std::shared_ptr<Sphere>& s) {
		spheres.push_back(s); 
	}

	bool trace(const Ray& ray, Vec3& hitColor) const {
		double tClosest = std::numeric_limits<double>::infinity();
		Vec3 normal, color, bestNormal, bestColor;

		bool hit = false;

		// Check intersection for all triangle-based objects
		for (const auto& obj : objs) {
			double t; Vec3 n, c;
			if (obj->intersect(ray, t, n, c) && t < tClosest) {
				tClosest = t;
				bestNormal = n;
				bestColor = c;
				hit = true;
			}
		}

		// Check intersection for all spheres 
		for (const auto& sphere : spheres) {
			double t; Vec3 n, c;

			if (sphere->RaySphereIntersection(ray, t, n, c) && t < tClosest) {
				tClosest = t; 
				bestNormal = n; 
				bestColor = c; 
				hit = true; 
			}
		}

		// Flat shading
		/*if (hit) {
			hitColor = bestColor;
			return true;
		}
		else {
			hitColor = backgroundColor;
			return false;
		}*/

		// Lambertian shading
		if (hit) {
			Vec3 hitPoint = ray.origin + ray.direction * tClosest;

			Vec3 lightVec = lightPos - hitPoint;
			Vec3 lightDir = lightVec.normalize();

			if (bestNormal.dotProduct(lightDir) < 0.0) {
				bestNormal = bestNormal * -1.0;
			}

			double diff = std::max(0.0, bestNormal.dotProduct(lightDir));
			double distance2 = lightVec.dotProduct(lightVec);
			double intensity = lightIntensity * diff / distance2;

			double ambient = 0.1; 
			hitColor = bestColor * (lightColor * intensity + Vec3(ambient, ambient, ambient));
		}
		else {
			hitColor = backgroundColor;
			return false;
		}
	}

};

