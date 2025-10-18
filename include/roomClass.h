#pragma once

#define _USE_MATH_DEFINES

#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <math.h>

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

	const double distToRoofOffset = 1e-4;

	/*Vec3 lightPos = Vec3(4, 2, 10);*/
	Vec3 lightPos = Vec3(2, 2, 4 - distToRoofOffset);
	Vec3 lightColor = Vec3(0.9, 0.9, 0.9);
	double lightIntensity = 5.0;
	/*double lightIntensity = 400.0;*/
	double ambient = 0.01;
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
		addLightSources(ceilingLight);

		// Floor - White
		auto floor = std::make_shared<TriObj>();
		floor->addTriangle(Triangle(v0, v1, v2, Vec3(0.8, 0.8, 0.8)));
		floor->addTriangle(Triangle(v0, v2, v3, Vec3(0.8, 0.8, 0.8)));
		floor->setMat("DIFFUSE");
		addTriObj(floor);

		// The wall behind the camera
		auto leftWall = std::make_shared<TriObj>();
		leftWall->addTriangle(Triangle(v0, v3, v7, Vec3(0.7, 0.2, 0.2)));
		leftWall->addTriangle(Triangle(v0, v7, v4, Vec3(0.7, 0.2, 0.2)));
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

		// Ceiling - white (red)
		auto ceiling = std::make_shared<TriObj>();
		/*	ceiling->addTriangle(Triangle(v4, v7, v6, Vec3(0.9, 0.3, 0.3)));
			ceiling->addTriangle(Triangle(v4, v6, v5, Vec3(0.9, 0.3, 0.3)));*/
		ceiling->addTriangle(Triangle(v4, v7, v6, Vec3(0.75, 0.75, 0.75)));
		ceiling->addTriangle(Triangle(v4, v6, v5, Vec3(0.75, 0.75, 0.75)));
		ceiling->setMat("DIFFUSE");
		addTriObj(ceiling);

		// Add a sphere to the scene
	/*	Vec3 sphereCenterPoint(2.0, 2.2, 1.0);
		Vec3 sphereColor(0.8, 0.8, 0.8);
		double sphereRadius = 0.45;
		std::string sphereMat = "GLASS";
		auto sphere = std::make_shared<Sphere>(sphereCenterPoint, sphereRadius, sphereColor, sphereMat);
		addSphere(sphere);*/

		// Add cube to the scene
		Vec3 cubeCenterPoint(2.5, 0, 2);
		double cubeSideLenghts = 1.0;
		Vec3 cubeColour(0.9, 0.9, 0.9);
		std::string cubeMat = "MIRROR";
		auto cube = std::make_shared<TriObj>();
		cube->createCube(cubeCenterPoint, cubeSideLenghts, cubeColour);
		cube->setMat(cubeMat);
		addTriObj(cube);

		//// Add tetraheder to the scene
		//auto tetra = std::make_shared<TriObj>();
		//Vec3 t0(3.2, 1.8, 0.0);
		//Vec3 t1(3.8, 1.8, 0.0);
		//Vec3 t2(3.5, 2.6, 0.0);

		//Vec3 t3(3.2, 2.07, 1.8);
		//Vec3 tetraColor(0.7, 0.7, 0.9);
		//tetra->createTetra(t0, t1, t2, t3, tetraColor);
		//tetra->setMat("DIFFUSE");
		//addTriObj(tetra);

		//// Add a sphere to the scene
		//Vec3 sphereOnTetraCenter = Vec3(t3.x, t3.y, t3.z + 0.5);
		//Vec3 sphereOnTetraColor(0.95, 0.95, 0.95);
		//double sphereOnTetraRadius = 0.6;
		//std::string sphereOnTetraMat = "MIRROR";
		//auto topSphere = std::make_shared<Sphere>(
		//	sphereOnTetraCenter,
		//	sphereOnTetraRadius,
		//	sphereOnTetraColor,
		//	sphereOnTetraMat
		//);
		//addSphere(topSphere);


		// Add a glass tetrahedron prism to the scene
		auto tetra = std::make_shared<TriObj>();

		// Rotation center for base (pivot point for Z-rotation)
		Vec3 center(3.5, 2.1, 0.0);  // near back wall, visually centered

		// Rotation angle in radians (e.g., 20 degrees)
		double angleDeg = 32.0;
		double angleRad = angleDeg * M_PI / 180.0;

		// Helper lambda to rotate a point around center in Z-plane
		auto rotateZ = [&](const Vec3& p) -> Vec3 {
			double x = std::cos(angleRad) * (p.x - center.x) - std::sin(angleRad) * (p.y - center.y) + center.x;
			double y = std::sin(angleRad) * (p.x - center.x) + std::cos(angleRad) * (p.y - center.y) + center.y;
			return Vec3(x, y, p.z);
			};

	

		Vec3 base0 = rotateZ(Vec3(3.1, 1.7, 0.05));
		Vec3 base1 = rotateZ(Vec3(3.9, 1.7, 0.05));
		Vec3 base2 = rotateZ(Vec3(3.5, 2.9, 0.05));

		Vec3 t3 = Vec3(3.5, 2.1, 1.8); 

		Vec3 tetraColor(0.8, 0.8, 0.8);
		tetra->createTetra(base0, base1, base2, t3, tetraColor);
		tetra->setMat("GLASS");
		addTriObj(tetra);

		// Sphere balancing on top of big tetrahedron
		double sphereRadius = 0.75;
		Vec3 sphereCenter(t3.x, t3.y, t3.z + sphereRadius);
		Vec3 sphereColor(0.9, 0.9, 0.9);
		std::string sphereMat = "MIRROR";

		auto topSphere = std::make_shared<Sphere>(
			sphereCenter,
			sphereRadius,
			sphereColor,
			sphereMat
		);
		addSphere(topSphere);

		// Cluster of 3 spheres in left corner
		double leftSphereRadius = 0.2;
		// Spheres' placement above floor
		double zOffset = 0.3;  

		// Cluster center (center sphere's center)
		double baseX = 2.55;
		double baseY = 1.25;
		double spacing = 0.45;

		// Mirror sphere (left)
		Vec3 mirrorCenter(baseX, baseY - spacing, leftSphereRadius + zOffset);
		auto mirrorSphere = std::make_shared<Sphere>(
			mirrorCenter,
			leftSphereRadius,
			Vec3(0.9, 0.9, 0.9),
			"MIRROR"
		);
		addSphere(mirrorSphere);

		// Glass sphere (center)
		Vec3 glassCenter(baseX, baseY, leftSphereRadius + zOffset);
		auto glassSphere = std::make_shared<Sphere>(
			glassCenter,
			leftSphereRadius,
			Vec3(0.9, 0.9, 0.9),
			"GLASS"
		);
		addSphere(glassSphere);

		// Diffuse sphere (right)
		Vec3 diffuseCenter(baseX, baseY + spacing, leftSphereRadius + zOffset);
		auto diffuseSphere = std::make_shared<Sphere>(
			diffuseCenter,
			leftSphereRadius,
			Vec3(1.0, 0.6, 0.8), 
			"DIFFUSE"
		);
		addSphere(diffuseSphere);

		// Mirror cube in right corner
		Vec3 mirrorCubeCenter(3.0, 3.35, 0.3); 
		double cubeSideLength = 0.65;
		Vec3 cubeColor(0.9, 0.9, 0.9);
		std::string cubeMat2 = "MIRROR";

		auto mirrorCube = std::make_shared<TriObj>();
		mirrorCube->createCube(mirrorCubeCenter, cubeSideLength, cubeColor);
		mirrorCube->setMat(cubeMat2);
		addTriObj(mirrorCube);

		// Glass sphere on mirror cube
		double glassSphereRadius = 0.25;
		double cubeTopZ = mirrorCubeCenter.z + cubeSideLength / 2.0;
		Vec3 glassSphereCenterOnCube(
			mirrorCubeCenter.x,
			mirrorCubeCenter.y,
			cubeTopZ + glassSphereRadius
		);

		Vec3 glassSphereColor(0.9, 0.9, 0.9); 

		auto glassSphereOnCube = std::make_shared<Sphere>(
			glassSphereCenterOnCube,
			glassSphereRadius,
			glassSphereColor,
			"GLASS"
		);

		addSphere(glassSphereOnCube);

		// Tetrahedron in front of mirror cube
		auto smallTetra = std::make_shared<TriObj>();
		Vec3 t00(2.6-0.6, 3.9-0.5, 0.2);
		Vec3 t11(2.9-0.6, 3.9-0.5, 0.2);
		Vec3 t22(2.75-0.6, 3.7-0.5, 0.2);
		Vec3 t33(2.75-0.6, 3.83-0.5, 0.85);;

		Vec3 tetraColor2(0.3, 0.9, 0.8); // Turquoise 
		smallTetra->createTetra(t00, t11, t22, t33, tetraColor2);
		smallTetra->setMat("DIFFUSE");
		addTriObj(smallTetra);
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