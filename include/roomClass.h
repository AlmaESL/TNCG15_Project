#pragma once

#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>

#include "include/vec3.h"
#include "include/ray.h"
#include "objectDrawer.h"


/*
* Classes for scene geometry, such as  triangles, objects and cube for the room itself
*/

/// Class to make the scene room, that ie a cube 
class Scene {
public:
	std::vector<std::shared_ptr<Plane>> planes;
	std::vector<std::shared_ptr<Sphere>> spheres;
	std::vector < std::shared_ptr<Cube>> cubes;
	std::vector < std::shared_ptr<Tetrahedron>> tetrahedrons;

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
		auto floor = std::make_shared<Plane>();
		floor->addTriangle(Triangle(v0, v1, v2, Vec3(0.8, 0.8, 0.8)));
		floor->addTriangle(Triangle(v0, v2, v3, Vec3(0.8, 0.8, 0.8)));
		addPlanes(floor);

		// The wall behind the camera
		/*auto leftWall = std::make_shared<Object>();
		leftWall->addTriangle(Triangle(v0, v3, v7, Vec3(0, 1, 0)));
		leftWall->addTriangle(Triangle(v0, v7, v4, Vec3(0, 1, 0)));
		addObject(leftWall);*/

		// Back wall - green
		auto rightWall = std::make_shared<Plane>();
		rightWall->addTriangle(Triangle(v1, v5, v6, Vec3(0, 1, 0)));
		rightWall->addTriangle(Triangle(v1, v6, v2, Vec3(0, 1, 0)));
		addPlanes(rightWall);

		// Left wall - blue
		auto backWall = std::make_shared<Plane>();
		backWall->addTriangle(Triangle(v0, v4, v5, Vec3(0, 0, 1)));
		backWall->addTriangle(Triangle(v0, v5, v1, Vec3(0, 0, 1)));
		addPlanes(backWall);

		// Right wall - yellow
		auto frontWall = std::make_shared<Plane>();
		frontWall->addTriangle(Triangle(v3, v2, v6, Vec3(1, 1, 0)));
		frontWall->addTriangle(Triangle(v3, v6, v7, Vec3(1, 1, 0)));
		addPlanes(frontWall);

		// Ceiling - white
		auto ceiling = std::make_shared<Plane>();
		ceiling->addTriangle(Triangle(v4, v7, v6, Vec3(0.8, 0.8, 0.8)));
		ceiling->addTriangle(Triangle(v4, v6, v5, Vec3(0.8, 0.8, 0.8)));
		addPlanes(ceiling);

		// Add a sphere to the scene
		Vec3 sphereCenterPoint(2.0, 2.0, 2.0);
		Vec3 sphereColor(0.9, 0.9, 0.9); 
		double sphereRadius = 0.2; 
		auto sphere = std::make_shared<Sphere>(sphereCenterPoint, sphereRadius, sphereColor);
		addSphere(sphere);

		// Add cube to the scene
		Vec3 cubeCenterPoint(3.5, 3.5, 0.5);
		double cubeSideLenghts = 1.0;
		Vec3 cubeColour(0.9, 0.3, 0.1);

		auto cube = std::make_shared<Cube>(cubeCenterPoint, cubeSideLenghts, cubeColour);
		addCubes(cube);

		Vec3 cubeCenterPoint2(3.5, 2.5, 1.5);
		double cubeSideLenghts2 = 1.0;
		Vec3 cubeColour2(0.7, 0.1, 0.5);

		auto cube2 = std::make_shared<Cube>(cubeCenterPoint2, cubeSideLenghts2, cubeColour2);
		addCubes(cube2);

		Vec3 cubeCenterPoint3(3.5, 1.5, 2.5);
		double cubeSideLenghts3 = 1.0;
		Vec3 cubeColour3(0.8, 0.1, 0.6);

		auto cube3 = std::make_shared<Cube>(cubeCenterPoint3, cubeSideLenghts3, cubeColour3);
		addCubes(cube3);

		Vec3 cubeCenterPoint4(3.5, 0.5, 3.5);
		double cubeSideLenghts4 = 1.0;
		Vec3 cubeColour4(0.9, 0.1, 0.7);

		auto cube4 = std::make_shared<Cube>(cubeCenterPoint4, cubeSideLenghts4, cubeColour4);
		addCubes(cube4);

		Vec3 tetra0(2.0, 2.0, 0.0);
		Vec3 tetra1(2.8, 1.4, 0.0);
		Vec3 tetra2(2.6, 1.8, 1.5);
		Vec3 tetra3(3.0, 2.5, 0.5);

		Vec3 tetraColour(1.0, 0.5, 0.8);
		auto tetra = std::make_shared<Tetrahedron>(tetra0, tetra1, tetra2, tetra3, tetraColour);
		addTetrahedron(tetra);

	}

	void addPlanes(const std::shared_ptr<Plane>& plane) {
		planes.push_back(plane);
	}

	void addCubes(const std::shared_ptr<Cube>& cube) {
		cubes.push_back(cube);
	}

	void addTetrahedron(const std::shared_ptr<Tetrahedron>& tetra) {
		tetrahedrons.push_back(tetra);
	}

	void addSphere(const std::shared_ptr<Sphere>& s) {
		spheres.push_back(s); 
	}

	bool trace(const Ray& ray, Vec3& hitColor) const {
		double tClosest = std::numeric_limits<double>::infinity();
		Vec3 normal, color, bestNormal, bestColor;

		bool hit = false;

		// Check intersection for all triangle-based objects
		for (const auto& plane : planes) {
			double t; Vec3 n, c;
			if (plane->intersect(ray, t, n, c) && t < tClosest) {
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

		// Check intersection for all cubes
		for (const auto& cube : cubes) {
			double t; Vec3 n, c;

			if (cube->intersect(ray, t, n, c) && t < tClosest) {
				tClosest = t;
				bestNormal = n;
				bestColor = c;
				hit = true;
			}
		}

		// Check intersection for all tetras
		for (const auto& tetra : tetrahedrons) {
			double t; Vec3 n, c;

			if (tetra->intersect(ray, t, n, c) && t < tClosest) {
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

