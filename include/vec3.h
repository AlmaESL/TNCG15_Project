#pragma once

/// Utility data structure to define 3D points and effectively 3D vectors
struct Vec3 {

	double x, y, z;

	// Constructor 
	Vec3(double xx = 0, double yy = 0, double zz = 0) : x(xx), y(yy), z(zz) {}

	// Computational operators
	Vec3 operator+ (const Vec3& v) const {
		return Vec3(x + v.x, y + v.y, z + v.z);
	}

	Vec3 operator- (const Vec3& v) const {
		return Vec3(x - v.x, y - v.y, z - v.z);
	}

	// Overloaded multiplication - vector and scalar 
	Vec3 operator* (double d) const {
		return Vec3(x * d, y * d, z * d);
	}

	// Overloaded multiplication - vector with vector
	Vec3 operator* (const Vec3& v)  const {
		return Vec3(x * v.x, y * v.y, z * v.z);
	}

	// Division operator (only for normalization)
	Vec3 operator/ (double d) const {
		return Vec3(x / d, y / d, z / d);
	}

	Vec3 operator/ (Vec3& v) const {
		return Vec3(x / v.x, y / v.y, z / v.z);
	}

	// Comparison operator
	bool operator== (const Vec3& v) const {
		return (x == v.x && y == v.y && z == v.z);
	}

	// Addition assignment operator
	Vec3& operator+= (const Vec3& v) {
		return *this = *this + v; // Implicitly uses operator +
	}

	// Division assignment operator
	Vec3& operator/= (double d) {
		return *this = *this / d; // Implicitly uses operator /
	}

	double dotProduct(const Vec3& v) const {
		return x * v.x + y * v.y + z * v.z;
	}

	Vec3 crossProduct(const Vec3& v) const {
		return Vec3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}

	double getLength() const {
		return std::sqrt(x * x + y * y + z * z);
	}

	double euclDist(const Vec3& v) const {
		return hypot(hypot(x - v.x, y - v.y), z - v.z);
	}

	Vec3 normalize() const {
		double len = getLength();
		return len > 0 ? (*this) / len : *this;
	}
};