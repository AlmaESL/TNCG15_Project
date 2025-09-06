#include "include/glad/glad.h"
#include "include/GLFW/glfw3.h"
#include "include/computeNormals.h"
#include "glm.hpp"
#include <iostream>
#include<vector>

#include "include/roomClass.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}


// utility normalize function 
static glm::vec3 normalize(glm::vec3 v) {
	return v / (glm::normalize(v));
}


int main() {

	if (!glfwInit()) return -1;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "Global Illumination Renderer", nullptr, nullptr);
	if (!window) { glfwTerminate(); return -1; }
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to init GLAD\n";
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// TODO: global illumination rendering
		//glEnable(GL_DEPTH_TEST);
	  /*  std::vector<glm::vec3> triangle = { glm::vec3{1.0f, 0.0f, 0.0f}, glm::vec3{1.0f, 1.0f, 0.0f}, glm::vec3{0.0f, 0.0f, 1.0f} };
		glm::vec3 normal = glm::vec3{ 0.0f, 0.0f, 0.0f };

		normal = compNorm::computeFaceNormal(triangle[0], triangle[1], triangle[2]);

		std::cout << "Normal: x = "<< normal.x << " y = " << normal.y << " z = " << normal.z << "\n";*/

		Triangle tri = { glm::vec3{1.0f, 0.0f, 0.0f}, glm::vec3{1.0f, 1.0f, 0.0f}, glm::vec3{0.0f, 0.0f, 1.0f} };

		tri.printNormal();



		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
