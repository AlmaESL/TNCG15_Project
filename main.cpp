#include <iostream>
#include <chrono>

#include "include/roomClass.h"
#include "include/camera.h"
#include "include/renderer.h"

int main() {
	int WIDTH = 512;
	int HEIGHT = 512;

	using std::chrono::high_resolution_clock;
	using std::chrono::duration_cast;
	using std::chrono::duration;
	using std::chrono::milliseconds;

	Camera cam;
	Scene scene;

	Renderer renderer;
	auto t1 = high_resolution_clock::now();
	renderer.render(scene, cam, WIDTH, HEIGHT, "test.ppm");
	auto t2 = high_resolution_clock::now();

	/* Getting number of milliseconds as an integer. */
	auto ms_int = duration_cast<milliseconds>(t2 - t1);

	/* Getting number of milliseconds as a double. */
	duration<double, std::milli> ms_double = t2 - t1;

	std::cout << "Rendering time: " << ms_int.count() << "ms\n";

	std::cout << "Rendered to test.ppm\n";
	return 0;

}