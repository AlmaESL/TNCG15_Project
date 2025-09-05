#pragma once

#include "GLFW/glfw3.h"  // To use OpenGL datatypes
#include "glm.hpp"
#include <string>
#include <vector>

class compNorm {
public:
	static glm::vec3 computeFaceNormal(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2) {
		// Create edges from face verticies
		glm::vec3 edge0 = v1 - v0;
		glm::vec3 edge1 = v2 - v0;

		// Calculate normal from edges
		glm::vec3 n = glm::cross(glm::normalize(edge0), glm::normalize(edge1));

		// Return normal
		return(n);
	}

	/* Won't be used initally
	glm::vec3 computeVertexNormal(glm::vec3 vertex) {


	}
	*/
private:

};