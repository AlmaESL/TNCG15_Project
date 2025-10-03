#pragma once 

#include "include/roomClass.h"
#include "include/camera.h"
#include "include/ray.h"
#include "tracer.h"

/// Renderer class
class Renderer {
public:
	void render(const Scene& scene, const Camera& camera, int width, int height, const char* filename) {

		// Initialize frame buffer for image, *3 since rgb channels 
		std::vector<unsigned char> frameBuffer(width * height * 3);

		double maxVal = 0.0;

		// Max rays allowed per pixel
		// TODO: Use this variable for MC's adaptive sampling
		int maxRays = 10; 

		// Buffer for floating point color values before tone mapping
		std::vector<Vec3> floatBuffer(width * height);

		// Tracer object
		Tracer t;

		// Iterate all pixels 
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {

				// Supersampling: N samples per pixel 
				const int spp = 200; 
				// Accumulated color for the pixel from all samples
				Vec3 accum(0.0, 0.0, 0.0);

				// # of max bounces allowed for each ray
				int maxDepth = 8;

				// Shading method, flat or lambertian
				// TODO: Implement MC 
				std::string shadingMethod = "LAMBERTIAN";
				for (int s = 0; s < spp; ++s) {
					// Uniform subpixel jitter in [0,1)
					// TODO: Update to stratified sampling through each pixel
					double jx = ((double)std::rand() / RAND_MAX);
					double jy = ((double)std::rand() / RAND_MAX);
					double u = (x + jx) / width;
					double v = 1.0 - (y + jy) / height;
					Ray ray = camera.generateViewRayUV(u, v);
					Vec3 color;
					t.trace(ray, scene, color, 0, maxDepth, shadingMethod);
					accum = accum + color;
				}
				// Averaging color samples through each pixel
				Vec3 avg = accum / (double)spp;

				// Make buffer of average color values
				floatBuffer[y * width + x] = avg;

				// Get the max color value for the entire image plane
				maxVal = std::max({ maxVal, avg.x, avg.y, avg.z });
			}
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