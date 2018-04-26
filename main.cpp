#include <iostream>
#include "SeamCarving.hpp"

int main(int argc, char** argv) {
	std::ifstream image;
	std::ofstream image_processed;

	try {
		if (argc != 4)
			throw "Usage: PROGRAM FILENAME NUM_VERTICAL_SEAMS NUM_HORIZONTAL_SEAMS";
		else {

			if (atoi(argv[2]) <= 0 || atoi(argv[3]) <= 0)
				throw "Number of vertical and horizontal seams must be greater than zero";

			std::string filename = argv[1];
			image.open(filename.c_str(), std::ios::binary);

			filename.erase(filename.length() - 4, 4);
			filename = filename + "_processed.pgm";
			image_processed.open(filename.c_str(), std::ios::binary);

			if (!image.is_open())
				throw "Could not open image file";

			if (!image_processed.is_open())
				throw "Could not create output file";
		}
	} catch (const char* err) {
		std::cout << "The application threw an exception: " << err << std::endl;
	}
	
	SeamCarving sc(atoi(argv[2]), atoi(argv[3]));
	sc.get_header(image);
	sc.populate_pixel_matrix(image);
	sc.remove_vertical_seams();
	sc.remove_horizontal_seams();

	return 0;
}
