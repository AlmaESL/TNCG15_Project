#pragma once

#include "vec3.h"
#include "triangle.h"

class Sphere {
public:
	Sphere(const Vec3& c, double r, const Vec3& col) : centerPoint(c), radius(r), color(col) {}

	// Ray intersection test for spheres 
	double RaySphereIntersection(const Ray& ray) const {

		// Distance vector between sphere center and the start point of the ray (camera)
		Vec3 L = centerPoint - ray.origin;

		// If the distance vector L and the ray direction are parallel, then their dot product will be negative and 
		// intersection not occur
		double t_ca = L.dotProduct(ray.direction);
		if (t_ca < 0) {
			return -1.0;
		}

		// d2 is the vector length between vector B and the ray in Figure in lec 3
		double d2 = L.dotProduct(L) - t_ca * t_ca;

		// Sphere radius squared
		double r2 = radius * radius;

		if (d2 > r2) {
			return -1.0;  // ray misses sphere
		}

		// Compute hte offset 
		double t_hc = sqrt(r2 - d2);

		// Compute the intersection points t0 and t1, two since ray must enter and leave the sphere
		double t0 = t_ca - t_hc;
		double t1 = t_ca + t_hc;

		if (t0 > 0) {
			return t0;
		}
		if (t1 > 0) {
			return t1;
		}

		return -1.0;
	}

	Vec3 centerPoint;
	double radius;
	Vec3 color;
};

class Plane {
public:
	std::vector<Triangle> triangles;

	void addTriangle(const Triangle& tri) {
		triangles.push_back(tri);
	}

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
private:

};

class Cube {
public:
	Cube(const Vec3& c, double l, const Vec3& col) : centerPoint(c), sideLength(l), color(col) {
		double centerDist = sideLength / 2;

		Vec3 v0((centerPoint.x - centerDist), (centerPoint.y + centerDist), (centerPoint.z - centerDist));
		Vec3 v1((centerPoint.x - centerDist), (centerPoint.y - centerDist), (centerPoint.z - centerDist));
		Vec3 v2((centerPoint.x - centerDist), (centerPoint.y + centerDist), (centerPoint.z + centerDist));
		Vec3 v3((centerPoint.x - centerDist), (centerPoint.y - centerDist), (centerPoint.z + centerDist));
		Vec3 v4((centerPoint.x + centerDist), (centerPoint.y + centerDist), (centerPoint.z - centerDist));
		Vec3 v5((centerPoint.x + centerDist), (centerPoint.y - centerDist), (centerPoint.z - centerDist));
		Vec3 v6((centerPoint.x + centerDist), (centerPoint.y + centerDist), (centerPoint.z + centerDist));
		Vec3 v7((centerPoint.x + centerDist), (centerPoint.y - centerDist), (centerPoint.z + centerDist));
		
		//Front
		triangles.push_back(Triangle(v0, v1, v3, color));
		triangles.push_back(Triangle(v0, v3, v2, color));

		//Left
		triangles.push_back(Triangle(v0, v2, v6, color));
		triangles.push_back(Triangle(v0, v6, v4, color));

		// Bottom
		triangles.push_back(Triangle(v0, v5, v1, color));
		triangles.push_back(Triangle(v0, v4, v5, color));

		// Right
		triangles.push_back(Triangle(v1, v7, v3, color));
		triangles.push_back(Triangle(v1, v5, v7, color));

		// Back
		triangles.push_back(Triangle(v4, v6, v7, color));
		triangles.push_back(Triangle(v4, v7, v5, color));

		// Top
		triangles.push_back(Triangle(v2, v3, v7, color));
		triangles.push_back(Triangle(v6, v2, v7, color));
	};

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
	};

private:
	Vec3 centerPoint;
	double sideLength;
	Vec3 color;
	std::vector<Triangle> triangles;
};

class Tetrahedron {
public:
	Tetrahedron(const Vec3& vert0, const Vec3& vert1, const Vec3& vert2, const Vec3& vert3, Vec3& col)
		: v0(vert0), v1(vert1), v2(vert2), v3(vert3), color(col) {
		triangles.push_back(Triangle(v0, v1, v2, color));
		triangles.push_back(Triangle(v0, v2, v3, color));
		triangles.push_back(Triangle(v0, v3, v1, color));
		triangles.push_back(Triangle(v1, v3, v2, color));
	};

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
	};

private:
	Vec3 v0;
	Vec3 v1;
	Vec3 v2;
	Vec3 v3;
	Vec3 color;
	std::vector<Triangle> triangles;
};