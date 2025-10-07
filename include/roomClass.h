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
	std::vector<std::shared_ptr<TriObj>> objs;
	std::vector<std::shared_ptr<Sphere>> spheres;
	std::vector<std::shared_ptr<TriObj>> lightSources;

	/*Vec3 lightPos = Vec3(4, 2, 10);*/
	Vec3 lightPos = Vec3(2, 2, 3.8);
	Vec3 lightColor = Vec3(1, 1, 1);
	double lightIntensity = 400.0;
	double ambient = 0.5;
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

		// NOTE: make sure pure colors are NEVER used as they cause issues in color calculations

		// Area light
		auto ceilingLight = std::make_shared<TriObj>();

		// Set up vertices based on the specified light position, area light size specifes size of the area light
		double areaLightSize = 0.5;
		Vec3 lightV0(lightPos.x - areaLightSize, lightPos.y - areaLightSize, lightPos.z);
		Vec3 lightV1(lightPos.x - areaLightSize, lightPos.y + areaLightSize, lightPos.z);
		Vec3 lightV2(lightPos.x + areaLightSize, lightPos.y - areaLightSize, lightPos.z);
		Vec3 lightV3(lightPos.x + areaLightSize, lightPos.y + areaLightSize, lightPos.z);

		// Triangles lie flat on the z-plane
		ceilingLight->addTriangle(Triangle(lightV0, lightV1, lightV3, Vec3(1.0, 1.0, 1.0)));
		ceilingLight->addTriangle(Triangle(lightV0, lightV3, lightV2, Vec3(1.0, 0.95, 1.0)));
		ceilingLight->setMat("EMISSIVE");
		addTriObj(ceilingLight);

		// Floor - White
		auto floor = std::make_shared<TriObj>();
		floor->addTriangle(Triangle(v0, v1, v2, Vec3(0.8, 0.8, 0.8)));
		floor->addTriangle(Triangle(v0, v2, v3, Vec3(0.8, 0.8, 0.8)));
		floor->setMat("DIFFUSE");
		addTriObj(floor);

		// The wall behind the camera
		auto leftWall = std::make_shared<TriObj>();
		leftWall->addTriangle(Triangle(v0, v3, v7, Vec3(0.8, 0.2, 0.2)));
		leftWall->addTriangle(Triangle(v0, v7, v4, Vec3(0.8, 0.2, 0.2)));
		leftWall->setMat("DIFFUSE");
		addTriObj(leftWall);

		// Back wall - green
		auto rightWall = std::make_shared<TriObj>();
		rightWall->addTriangle(Triangle(v1, v5, v6, Vec3(0.2, 0.8, 0.2)));
		rightWall->addTriangle(Triangle(v1, v6, v2, Vec3(0.2, 0.8, 0.2)));
		rightWall->setMat("DIFFUSE");
		addTriObj(rightWall);

		// Left wall - blue
		auto backWall = std::make_shared<TriObj>();
		backWall->addTriangle(Triangle(v0, v4, v5, Vec3(0.2, 0.2, 0.8)));
		backWall->addTriangle(Triangle(v0, v5, v1, Vec3(0.2, 0.2, 0.8)));
		backWall->setMat("DIFFUSE");
		addTriObj(backWall);

		// Right wall - yellow
		auto frontWall = std::make_shared<TriObj>();
		frontWall->addTriangle(Triangle(v3, v2, v6, Vec3(0.8, 0.8, 0.2)));
		frontWall->addTriangle(Triangle(v3, v6, v7, Vec3(0.8, 0.8, 0.2)));
		frontWall->setMat("DIFFUSE");
		addTriObj(frontWall);

		// Ceiling - white
		auto ceiling = std::make_shared<TriObj>();
		ceiling->addTriangle(Triangle(v4, v7, v6, Vec3(0.9, 0.3, 0.3)));
		ceiling->addTriangle(Triangle(v4, v6, v5, Vec3(0.9, 0.3, 0.3)));
		ceiling->setMat("DIFFUSE");
		addTriObj(ceiling);

		// Add a sphere to the scene
		Vec3 sphereCenterPoint(3.0, 2.0, 1.7);
		Vec3 sphereColor(0.6, 0.6, 1.0);
		double sphereRadius = 0.9;
		std::string sphereMat = "GLASS";
		auto sphere = std::make_shared<Sphere>(sphereCenterPoint, sphereRadius, sphereColor, sphereMat);
		addSphere(sphere);

		// Add cube to the scene
		Vec3 cubeCenterPoint(2.5, 0, 2);
		double cubeSideLenghts = 1.0;
		Vec3 cubeColour(0.9, 0.9, 0.9);
		std::string cubeMat = "MIRROR";
		auto cube = std::make_shared<TriObj>();
		cube->createCube(cubeCenterPoint, cubeSideLenghts, cubeColour);
		cube->setMat(cubeMat);
		addTriObj(cube);
	}


	// Add objects to scene
	void addTriObj(const std::shared_ptr<TriObj>& obj) {
		objs.push_back(obj);
	}

	void addSphere(const std::shared_ptr<Sphere>& s) {
		spheres.push_back(s);
	}

	void addLightSources(const std::shared_ptr<TriObj>& obj) {
		lightSources.push_back(obj);
	}

};