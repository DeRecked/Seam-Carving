#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char** argv) {
	if (argc < 4)
		throw "Usage: PROGRAM FILENAME NUM_VERTICAL_SEAMS NUM_HORIZONTAL_SEAMS";
	else {
		std::string filename = argv[1];
		std::ifstream image(filename.c_str(), std::ios::binary);

		if (!image.is_open())
			throw "Could not open image file";
	}

	int vertical_seams = atoi(argv[2]);
	int horizontal_seams = atoi(argv[3]);

	return 0;
}