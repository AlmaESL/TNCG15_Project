#include <iostream>

#include "include/roomClass.h"
#include "include/camera.h"
#include "include/renderer.h"

int main() {
	int WIDTH = 1024;
	int HEIGHT = 1024; 


	Camera cam; 
	Scene scene;
	
	Renderer renderer;
	renderer.render(scene, cam, WIDTH, HEIGHT, "test.ppm");
	std::cout << "Rendered to test.ppm\n";
	return 0;

}