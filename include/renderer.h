#pragma once 

#include "include/roomClass.h"
#include "include/camera.h"
#include "include/ray.h"
#include "tracer.h"

// Threading
#include <thread>
#include <random>

/// Renderer class
class Renderer {
public:
	void render(const Scene& scene, const Camera& camera, int width, int height, const char* filename) {

		// Initialize frame buffer for image, *3 since rgb channels 
		std::vector<unsigned char> frameBuffer(width * height * 3);

		// Max color value for entire image, used for tone mapping
		double maxVal = 0.0;

		// Max rays allowed per pixel
		// TODO: Use this variable for MC's adaptive sampling
		int maxRays = 10; 

		// Buffer for floating point color values before tone mapping
		std::vector<Vec3> floatBuffer(width * height);

		// Parallel row-based rendering --> one thread per row block
		unsigned int requestedThreads = std::thread::hardware_concurrency();

		// Request at least 4 threads
		unsigned int numThreads = requestedThreads > 0 ? requestedThreads : 4u;

		// Divide the image into blocks of rows for each thread
		int rowsPerThread = (height + (int)numThreads - 1) / (int)numThreads;

		// Workers are threads rendering the image
		std::vector<std::thread> workers;
		workers.reserve(numThreads);

		// Each thread stores its own local max color value
		std::vector<double> perThreadMax(numThreads, 0.0);

		// Rendering parameters
		const int spp = 200;
		const int maxDepth = 8; // Max number of bounced allowed for each ray

		const std::string shadingMethod = "LAMBERTIAN";

		// Iterate all threads
		for (unsigned int threadIndex = 0; threadIndex < numThreads; ++threadIndex) {
			int startY = (int)threadIndex * rowsPerThread;
			int endY = std::min(height, startY + rowsPerThread);
			if (startY >= endY) {
				continue;
			}

			// A lambda function that each thread executes, which renders a block of rows
			// Emplace adds a new thread to the workers vector
			workers.emplace_back([&, startY, endY, threadIndex]() {

				// Thread-local RNG to avoid data races from std::rand()
				std::random_device rd;
				std::mt19937 rng(rd() ^ ((unsigned int)startY + (unsigned int)width * 73856093u) ^ (unsigned int)threadIndex);
				std::uniform_real_distribution<double> jitter01(0.0, 1.0);
				Tracer tracer;
				double localMax = 0.0;

				// Iterate all pixels in each thread's row block  
				for (int y = startY; y < endY; ++y) {
					for (int x = 0; x < width; ++x) {
						Vec3 accumulatedColor(0.0, 0.0, 0.0);
						for (int sample = 0; sample < spp; ++sample) {

							// Jittered sampling means that we add a small random offset to each pixel coordinate 
							// to get a sub-pixel location
							double jx = jitter01(rng);
							double jy = jitter01(rng);

							// u and v are the normalized pixel coordinates in [0,1], ie local pixel coordinates
							double u = (x + jx) / width;
							double v = 1.0 - (y + jy) / height;

							// Generate the view ray for the pixel with local coordinates
							Ray ray = camera.generateViewRayUV(u, v);
							Vec3 sampleColor;
							tracer.trace(ray, scene, sampleColor, 0, maxDepth, shadingMethod);

							// Accumulate the color from each sample through each pixel
							accumulatedColor = accumulatedColor + sampleColor;
						}

						// Average all color contributions from the samples within each pixel
						Vec3 averageColor = accumulatedColor / (double)spp;
						floatBuffer[y * width + x] = averageColor;
						localMax = std::max({ localMax, averageColor.x, averageColor.y, averageColor.z });
					}
				}
				// Store the local max color value for current thread
				perThreadMax[threadIndex] = localMax;
			});
		}

		// Wait for all threads to finish
		for (std::thread &worker : workers) {
			if (worker.joinable()) {
				worker.join();
			}
		}

		// Find the global max color value from all threads --> this is the max value of the entire image used for
		// tone mapping
		for (double m : perThreadMax) {
			maxVal = std::max(maxVal, m);
		}

		// Tone mapping for better color range representation 
		for (int i = 0; i < width * height; i++) {
			Vec3 c = floatBuffer[i];

			// Normalize with max value for better color gamut
			if (maxVal > 0) {
				c = c / maxVal;
			}

			// Gamma correction with sqrt for gamma 2.0
			c = Vec3(std::sqrt(c.x), std::sqrt(c.y), std::sqrt(c.z));

			// Clamp and convert to unsigned char since stb_image_write needs that format
			frameBuffer[3 * i + 0] = (unsigned char)(std::min(255.0, c.x * 255));
			frameBuffer[3 * i + 1] = (unsigned char)(std::min(255.0, c.y * 255));
			frameBuffer[3 * i + 2] = (unsigned char)(std::min(255.0, c.z * 255));
		}

		// Write the image to file
		std::ofstream ofs(filename, std::ios::binary);
		ofs << "P6\n" << width << " " << height << "\n255\n";
		ofs.write(reinterpret_cast<char*>(frameBuffer.data()), frameBuffer.size());
		ofs.close();
	}
};