#pragma once

#include "vec3.h"
#include <iostream>


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
		/*
		if (n.dotProduct(dirToCenter) < 0.0) {
			n = n * -1.0;
		}
		*/
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