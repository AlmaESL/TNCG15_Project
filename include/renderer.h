#pragma once 

#include "include/roomClass.h"
#include "include/camera.h"
#include "include/ray.h"

/// Renderer class
class Renderer {
public:
	void render(const Scene& scene, const Camera& camera, int width, int height, const char* filename) {

		// Initialize frame buffer for image, *3 since rgb channels 
		std::vector<unsigned char> frameBuffer(width * height * 3);

		double maxVal = 0.0;
		int maxRays = 500;
		std::vector<Vec3> floatBuffer(width * height);

		// Iterate all pixels 
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				// Single ray
				//Ray ray = camera.generateViewRay(x, y, width, height);

				// Uniform ray distribution
				StocasticRayGeneration randomRays = camera.generateViewRays(maxRays);
				Vec3 color;

				for (const auto& ray : randomRays.rays) {
					scene.trace(ray, color);
				}

				//scene.trace(ray, color);

				// Assign color to frame buffer on corresponding pixel
				floatBuffer[y * width + x] = color;

				maxVal = std::max({ maxVal, color.x, color.y, color.z });
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