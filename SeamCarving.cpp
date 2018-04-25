#include <iostream>
#include "ImageProcessor.hpp"

int main(int argc, char** argv) {
	std::ifstream image;
	std::ofstream image_processed;
	std::string header, xy, max_gray;

	try {
		if (argc < 4)
			throw "Usage: PROGRAM FILENAME NUM_VERTICAL_SEAMS NUM_HORIZONTAL_SEAMS";
		else {

			std::string filename = argv[1];
			image.open(filename.c_str(), std::ios::in, std::ios::binary);

			filename.erase(filename.length() - 4, 4);
			filename = filename + "_processed.pgm";
			image_processed.open(filename.c_str(), std::ios::out, std::ios::binary);

			if (!image.is_open())
				throw "Could not open image file";

			if (!image_processed.is_open())
				throw "Could not open processed image output stream";
		}
	} catch (const char* err) {
		std::cout << "The application threw an exception:" << err << std::endl;
	}
	
	ImageProcessor ip(atoi(argv[2]), atoi(argv[3]));
	
	ip.get_header(image);
	ip.populate_image_matrix(image);
	ip.populate_energy_matrix();
	ip.populate_cumulative_matrix();
	ip.remove_seams();

	return 0;
}