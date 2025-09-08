#include "include/glad/glad.h"
#include "include/GLFW/glfw3.h"
#include "glm.hpp"

#include <iostream>
#include<vector>


#include "include/roomClass.h"


/** Shader code for GL*/
const char* vertexShaderSrc = R"(
#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTex;
out vec2 TexCoord;
void main() {
    TexCoord = aTex;
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)";

const char* fragmentShaderSrc = R"(
#version 330 core
in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D screenTexture;
void main() {
    FragColor = texture(screenTexture, TexCoord);
}
)";

// ------------------------ Utility functions ------------------------
GLuint compileShader(GLenum type, const char* src) {
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &src, nullptr);
	glCompileShader(shader);

	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char info[512];
		glGetShaderInfoLog(shader, 512, nullptr, info);
		std::cerr << "Shader compile error: " << info << "\n";
	}
	return shader;
}

GLuint createShaderProgram() {
	GLuint vert = compileShader(GL_VERTEX_SHADER, vertexShaderSrc);
	GLuint frag = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSrc);
	GLuint program = glCreateProgram();
	glAttachShader(program, vert);
	glAttachShader(program, frag);
	glLinkProgram(program);

	int success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) std::cerr << "Shader linking failed\n";

	glDeleteShader(vert);
	glDeleteShader(frag);
	return program;
}



/** Ray-Triangle intersection computations using */
float rayTriangleIntersect(const Vec3& rayOrigin, const Vec3& theta, const Triangle& T) {

	const float EPSILON = 1e-8f;


	// Check if triangle normal and viewing direction theta are anti-parallel 
	float dotTest = dotProduct(T.normal, theta);

	if (dotTest > -EPSILON) {
		return -1.0f;
	}

	// C2 same as edge1 in Triangle class 
	// R1 = theta x C2
	Vec3 R1 = crossProduct(theta, T.edge1);

	// Copmute Cs = C1 * R1
	// C1 same as edge0 in Triangle class 
	float Cs = dotProduct(T.edge0, R1);

	// Ray is parallel to triangle
	if (abs(Cs) < EPSILON) {
		return -1.0f;
	}

	// Compmute R2 = C3 x C1, where C3 = e - v0 
	Vec3 C3 = rayOrigin - T.v0;
	Vec3 R2 = crossProduct(C3, T.edge0);

	// Compute parameter t = (C2 dot R2)/Cs 
	float t = (dotProduct(T.edge1, R2) / Cs);

	// Compute parameters u = (C3 dot R1)/Cs and v = (theta dot R2)/Cs 
	float u = (dotProduct(C3, R1) / Cs);
	float v = (dotProduct(theta, R2) / Cs);

	// Check that t>0, u,v>=0 and u+v<=1, else the ray misses 
	if (t > EPSILON && u >= 0.0f && v >= 0.0f && (u + v) <= 1.0f) {
		return t;
	}

	return -1.0f; 
}



// Camera setup
std::vector<unsigned char> rayTracer(int width, int height) {

	std::vector<unsigned char> image(width * height * 3, 0); // Image/window frame 

	// Specify eye location 
	Vec3 eye{ 0.0f, 0.0f, 2.0f };

	// Define field of view and the resolution 
	float viewportSize = 2.0f;
	float aspect = (float)width / (float)height;

	// Vertices for the triangle
	Vec3 a{ -0.5f, -0.5f, 0.0f };
	Vec3 b{ 0.5f, -0.5f, 0.0f };
	Vec3 c{ 0.0f,  0.5f, 0.0f };

	// Triangle color
	Vec3 color{ 0.0f, 0.0f, 255.0f }; 


	// Create a Triangle instance
	Triangle tri(a, b, c, color);

	// Loop all pixels 
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			float px = ((x + 0.5f) / width - 0.5f) * viewportSize * aspect;
			float py = (-(y + 0.5f) / height + 0.5f) * viewportSize;
			Vec3 pixelPos{ px, py, 0.0f };

			Vec3 viewDirection = normalize(pixelPos - eye);

			// Compute the ray tracing intersection
			float t = rayTriangleIntersect(eye, viewDirection, tri);
			unsigned char* pixel = &image[(y * width + x) * 3];

			// Color the pixel based on the intersection test
			if (t > 0.0f) {
				pixel[0] = tri.color.x;
				pixel[1] = tri.color.y;
				pixel[2] = tri.color.z;
			}
			else {
				pixel[0] = pixel[1] = pixel[2] = 0; // black
			}
		}
	}

	return image;
}

int main() {
	const int width = 400, height = 400;

	if (!glfwInit()) return -1;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(width, height, "Raytraced Triangle", nullptr, nullptr);
	if (!window) { glfwTerminate(); return -1; }
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD\n";
		return -1;
	}

	GLuint shaderProgram = createShaderProgram();
	glUseProgram(shaderProgram);

	// ------------------------ Quad setup ------------------------
	float quadVertices[] = {
		// positions   // texCoords
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f,
		-1.0f,  1.0f,  0.0f, 1.0f
	};
	unsigned int quadIndices[] = { 0,1,2, 2,3,0 };

	GLuint VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0); // pos
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float))); // tex
	glEnableVertexAttribArray(1);

	// ------------------------ Texture setup ------------------------
	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glUseProgram(shaderProgram);
	glUniform1i(glGetUniformLocation(shaderProgram, "screenTexture"), 0);

	// ------------------------ Main loop ------------------------
	while (!glfwWindowShouldClose(window)) {
		auto pixels = rayTracer(width, height);

		glBindTexture(GL_TEXTURE_2D, tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

		glClear(GL_COLOR_BUFFER_BIT);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}