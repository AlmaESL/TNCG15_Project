#include "include/glad/glad.h"
#include "include/GLFW/glfw3.h"
#include "include/computeNormals.h"
#include "glm.hpp"
#include <iostream>
#include<vector>

#include "include/roomClass.h"

//void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
//	glViewport(0, 0, width, height);
//}
//
//
//// utility normalize function 
//static glm::vec3 normalize(glm::vec3 v) {
//	return v / (glm::normalize(v));
//}
//
//
//int main() {
//
//	if (!glfwInit()) return -1;
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//
//	GLFWwindow* window = glfwCreateWindow(800, 600, "Global Illumination Renderer", nullptr, nullptr);
//	if (!window) { glfwTerminate(); return -1; }
//	glfwMakeContextCurrent(window);
//
//	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
//		std::cerr << "Failed to init GLAD\n";
//		return -1;
//	}
//
//	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
//
//	while (!glfwWindowShouldClose(window)) {
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//		// TODO: global illumination rendering
//		//glEnable(GL_DEPTH_TEST);
//	  /*  std::vector<glm::vec3> triangle = { glm::vec3{1.0f, 0.0f, 0.0f}, glm::vec3{1.0f, 1.0f, 0.0f}, glm::vec3{0.0f, 0.0f, 1.0f} };
//		glm::vec3 normal = glm::vec3{ 0.0f, 0.0f, 0.0f };
//
//		normal = compNorm::computeFaceNormal(triangle[0], triangle[1], triangle[2]);
//
//		std::cout << "Normal: x = "<< normal.x << " y = " << normal.y << " z = " << normal.z << "\n";*/
//
//		Triangle tri = { glm::vec3{1.0f, 0.0f, 0.0f}, glm::vec3{1.0f, 1.0f, 0.0f}, glm::vec3{0.0f, 0.0f, 1.0f} };
//
//		tri.printNormal();
//
//
//
//		glfwSwapBuffers(window);
//		glfwPollEvents();
//	}
//
//	glfwTerminate();
//	return 0;
//}


#include <thread>


/** Ray-Triangle intersection computations using */
float rayTriangleIntersect(const Vec3& rayOrigin, const Vec3& theta, const Triangle& T) {

	const float EPSILON = 1e-8f; 


	// Check if triangle normal and viewing direction theta are anti-parallel 
	float dotTest = dotProduct(T.normal, theta); 

	if (dotTest < EPSILON) {
		return -1.0f; 
	}

	// C2 same as edge1 in Triangle class 
	// R1 = theta x C2
	Vec3 R1 = crossProduct(theta, T.edge1); 

	// Copmute Cs = C1 * R1
	// C1 same as edge0 in Triangle class 
	float Cs = dotProduct(T.edge0, R1); 

	// Compmute R2 = C3 x C1, where C3 = e - v0 
	Vec3 C3 = rayOrigin - theta; 
	Vec3 R2 = crossProduct(C3, T.edge0); 

	// Compute parameter t = (C2 dot R2)/Cs 
	float t = (dotProduct(T.edge1, R2) / Cs); 

	// Compute parameters u = (C3 dot R1)/Cs and v = (theta dot R2)/Cs 
	float u = (dotProduct(C3, R1) / Cs); 
	float v = (dotProduct(theta, R2) / Cs); 

	// Check that u,v<0, u+v>1 and t<0 are false (if true there is no intersection)
	if (!(u < EPSILON && v < EPSILON && (u + v > EPSILON) && t < EPSILON)) {
		return t;
	}
	else {
		return -1.0f;
	}
}


// Camera setup
std::vector<unsigned char> rayTracer(int width, int height) {

	std::vector<unsigned char> image(width * height * 3, 0); // Image/window frame 

	// Specify eye location 
	Vec3 eye{ 0.0f, 0.0f, 2.0f };

	// Define field of view and the resolution 
	float viewportSize = 2.0f;
	float aspect = (float)width / (float)height;

	Vec3 a(0.0f, 0.0f, 0.0f);
	Vec3 b(1.0f, 0.0f, 0.0f);
	Vec3 c(0.0f, 1.0f, 0.0f);

	// Create a Triangle instance
	Triangle tri(a, b, c);


}