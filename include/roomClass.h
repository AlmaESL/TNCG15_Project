#pragma once

#include <iostream>
#include <cmath>

/** 3D vectors and points */
class Vec3 {

	float x, y, z;

	// Constructor
	Vec3(float xx, float yy, float zz) : x(xx), y(yy), z(zz) {}

	
	Vec3 operator+ (const Vec3& b) const {
		return { x + b.x, y + b.y, z + b.z };
	}

	Vec3 operator- (const Vec3& b) const { 
		return { x - b.x, y - b.y, z - b.z };
	}

	Vec3 operator* (float s) const {
		return { x*s, y*s, z*s};
	}
};

inline float dotProduct(const Vec3& a, const Vec3& b) { 
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline Vec3 crossProduct(const Vec3& a, const Vec3& b) {
	return { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
}

inline float getLength(const Vec3& v) {
	return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

inline Vec3 normalize(const Vec3& v) {
	float vLength = getLength(v);
	return { v.x / vLength, v.y / vLength, v.z / vLength };
}


/** Triamngle class. The foundation for all meshes */
class Triangle {
public: 
	// Triangle's vertices and its normal  
	Vec3 v0, v1, v2; 
	Vec3 normal; 
	Vec3 edge0, edge1; 

	Triangle(Vec3 vert0, Vec3 vert1, Vec3 vert2) : v0(vert0), v1(vert1), v2(vert2) {
		edge0 = v1 - v0; 
		edge1 = v2 - v0; 
		normal = computeNormal(v0, v1, v2); 
	}

	void printNormal() const {
		std::cout << "Triangle normal: " << normal.x << ", " << normal.y << ", " << normal.z << std::endl; 
	}


private: 
	Vec3 computeNormal(const Vec3& v0, const Vec3& v1, const Vec3& v2) {

		Vec3 edge0 = v1 - v0; 
		Vec3 edge1 = v2 - v0; 

		Vec3 n = crossProduct(normalize(edge0), normalize(edge1)); 

		return n; 
	}

}; 