#pragma once

#include "GLFW/glfw3.h"  // To use OpenGL datatypes
#include "glm.hpp"
#include <string>
#include <vector>

class Room {

};

// Triangle healper class
class Triangle {
public:

	Triangle(glm::vec3 vert1, glm::vec3 vert2, glm::vec3 vert3) {
		v1 = vert1;
		v2 = vert2;
		v3 = vert3;

		normal = computeNormal(v1, v2, v3);
	}

	void printNormal() const {
		std::cout << "normal: " << normal.x << ", " << normal.y << ", " << normal.z << std::endl;
	}

	glm::vec3 v1;
	glm::vec3 v2;
	glm::vec3 v3;

	glm::vec3 normal;
	//glm::vec3 color;

private:

	glm::vec3 computeNormal(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2) {
		glm::vec3 edge0 = v1 - v0;
		glm::vec3 edge1 = v2 - v0;

		glm::vec3 n = glm::cross(glm::normalize(edge0), glm::normalize(edge1));

		// Return normal
		return(n);
	}


	/*glm::vec3 makeTriangleVertex(coordinatePoint v) {
		glm::vec3 vertexPoint = glm::vec3(v.x, v.y, v.z);
		return vertexPoint;
	}*/
};



//struct coordinatePoint {
//public:
//	coordinatePoint(float coordX, float coordY, float coordZ) {
//		x = coordX;
//		y = coordY;
//		z = coordZ;
//	}
//
//	float x;
//	float y;
//	float z;
//};