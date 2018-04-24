#include <fstream>
#include <iostream>
#include <string>

#include "ImageProcessor.hpp"

int main(int argc, char** argv) {
	std::fstream image, image_processed;
	std::string header, xy, max_gray;

	try {
		if (argc < 4)
			throw "Usage: PROGRAM FILENAME NUM_VERTICAL_SEAMS NUM_HORIZONTAL_SEAMS";
		else {

			std::string filename = argv[1];
			image.open(filename.c_str(), std::ios::in, std::ios::binary);

			filename.erase(filename.length() - 4, 4);
			std::string filename_processed = filename + "_processed.pgm";
			image_processed.open(filename_processed.c_str(), std::ios::out, std::ios::binary);

			if (!image.is_open())
				throw "Could not open image file";

			if (!image_processed.is_open())
				throw "Could not open processed image output stream";
		}
	} catch (const char* err) {
		std::cout << "The application threw an exception:" << err << std::endl;
	}

	int vertical_seams = atoi(argv[2]);
	int horizontal_seams = atoi(argv[3]);

	std::getline(image, header);
	std::getline(image, xy);
	
	if (xy[0] == '#')
		std::getline(image, xy);

	std::getline(image, max_gray);

	std::cout << header << "\n" << xy << "\n" << max_gray << std::endl;

	return 0;
}