#include <iostream>
#include "SeamCarving.hpp"

int main(int argc, char** argv) {
	std::ifstream image;
	std::ofstream image_processed;
	int v_seams, h_seams;

	try {

		// Check that correct number of arguments have been given
		if (argc != 4)
			throw "Usage: PROGRAM FILENAME NUM_VERTICAL_SEAMS NUM_HORIZONTAL_SEAMS";
		else {

			// Check that the correct number of seams to remove have been given
			if (atoi(argv[2]) < 0 || atoi(argv[3]) < 0)
				throw "Number of vertical and horizontal seams cannot be less than zero";
			else {
				v_seams = atoi(argv[2]);
				h_seams = atoi(argv[3]);
			}

			// Get file names
			std::string filename = argv[1];
			image.open(filename.c_str(), std::ios::binary);

			// Open file streams
			filename.erase(filename.length() - 4, 4);
			filename = filename + "_processed.pgm";
			image_processed.open(filename.c_str(), std::ios::binary);

			// Check that file streams have opened
			if (!image.is_open())
				throw "Could not open image file";

			if (!image_processed.is_open())
				throw "Could not create output file";
		}
	} catch (const char* err) {
		std::cout << "The application threw an exception: " << err << std::endl;
	}

	std::cout << "Removing " << v_seams << " vertical and " << h_seams << " horizontal seam(s)" << std::endl;
	
	SeamCarving sc;								// Create seam carving object
	sc.get_header(image);						// Read in header info
	sc.populate_pixel_matrix(image);			// Fill vector of vectors with pixels
	sc.remove_vertical_seams(v_seams);			// Remove vertical seams
	sc.remove_horizontal_seams(h_seams);		// Remove horizontal seams
	sc.write_file(image_processed);				// Write pixels to file

	// Close file streams
	image.close();
	image_processed.close();

	return 0;
}
