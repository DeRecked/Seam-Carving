#include <algorithm>
#include <cstdlib>
#include <iostream>
#include "ImageProcessor.hpp"

void ImageProcessor::get_header(std::ifstream& image) {	
	std::string x, y;
	
	std::getline(image, header);
	while (header[0] == '#')
		std::getline(image, header);

	std::getline(image, dimensions);
	while (dimensions[0] == '#')
		std::getline(image, dimensions);

	std::getline(image, max_gray);
	while (max_gray[0] == '#')
		std::getline(image, max_gray);

	const char* chars = dimensions.c_str();

	while (*chars != ' ')
		x += *chars++;

	for (int i = dimensions.size() - x.size()-1; i < dimensions.size(); i++)
		y += *chars++;

	columns = stoi(x);
	rows = stoi(y);
}

void ImageProcessor::populate_image_matrix(std::ifstream& image) {
	std::vector<int> temp_vector;

	image_matrix.resize(columns);
	for (int i = 0; i < columns; i++)
		image_matrix[i].resize(rows);

	while (!image.eof()) {
		std::string line;

		std::getline(image, line);
		while (line[0] == '#')
			std::getline(image, line);
		
		const char* ptr = line.c_str();		
		for (int i = 0; i < line.length(); i++) {
			std::string value;

			while (*ptr != ' ' && *ptr != '\t') {
				value += *ptr++;
				i++;
			}
			value += *ptr++;
			temp_vector.push_back(stoi(value));
		}
	}

	std::vector<int>::iterator it = temp_vector.begin();
	for (int y = 0; y < rows; y++) {
		for (int x = 0; x < columns; x++) {
			image_matrix[x][y] = *it++;
			std::cout << image_matrix[x][y] << " ";
		}
		std::cout << std::endl;
	}
}

void ImageProcessor::populate_energy_matrix() {

	energy_matrix.resize(columns);
	for (int x = 0; x < columns; x++)
		energy_matrix[x].resize(rows);

	for (int y = 0; y < rows; y++) {
		for (int x = 0; x < columns; x++) {
			
			// Top
			if (y == 0) {
				// Top left
				if (x == 0 && y == 0)
					energy_matrix[x][y] = abs(image_matrix[x][y] - image_matrix[x + 1][y]) + abs(image_matrix[x][y] - image_matrix[x][y + 1]);

				// Top right
				else if (x == columns - 1 && y == 0)
					energy_matrix[x][y] = abs(image_matrix[x][y] - image_matrix[x - 1][y]) + abs(image_matrix[x][y] - image_matrix[x][y + 1]);

				// Otherwise
				else
					energy_matrix[x][y] = abs(image_matrix[x][y] - image_matrix[x - 1][y]) + abs(image_matrix[x][y] - image_matrix[x + 1][y])
						+ abs(image_matrix[x][y] - image_matrix[x][y + 1]);
			}

			// Bottom
			else if (y == rows - 1) {
				// Bottom left
				if (x == 0 && y == rows - 1)
					energy_matrix[x][y] = abs(image_matrix[x][y] - image_matrix[x + 1][y]) + abs(image_matrix[x][y] - image_matrix[x][y - 1]);

				// Bottom right
				else if (x == columns - 1 && y == rows - 1)
					energy_matrix[x][y] = abs(image_matrix[x][y] - image_matrix[x - 1][y]) + abs(image_matrix[x][y] - image_matrix[x][y - 1]);

				// Otherwise
				else
					energy_matrix[x][y] = abs(image_matrix[x][y] - image_matrix[x - 1][y]) + abs(image_matrix[x][y] - image_matrix[x + 1][y])
						+ abs(image_matrix[x][y] - image_matrix[x][y - 1]);
			}

			// Sides
			else if (x == 0 || x == columns - 1) {

				// Left Side
				if (x == 0)
					energy_matrix[x][y] = abs(image_matrix[x][y] - image_matrix[x + 1][y]) + abs(image_matrix[x][y] - image_matrix[x][y - 1])
						+ abs(image_matrix[x][y] - image_matrix[x][y + 1]);

				// Right Side
				else if (x == columns - 1)
					energy_matrix[x][y] = abs(image_matrix[x][y] - image_matrix[x - 1][y]) + abs(image_matrix[x][y] - image_matrix[x][y - 1])
						+ abs(image_matrix[x][y] - image_matrix[x][y + 1]);
			}

			// Everything else
			else
				energy_matrix[x][y] = abs(image_matrix[x][y] - image_matrix[x - 1][y]) + abs(image_matrix[x][y] - image_matrix[x + 1][y])
					+ abs(image_matrix[x][y] - image_matrix[x][y - 1]) + abs(image_matrix[x][y] - image_matrix[x][y + 1]);
		}
	}
}

void ImageProcessor::populate_cumulative_matrix() {

	cumulative_matrix.resize(columns);
	for (int x = 0; x < columns; x++)
		cumulative_matrix[x].resize(rows);

	// Copy top row of energy matrix to cumulative matrix
	for (int x = 0; x < columns; x++)
		cumulative_matrix[x][0] = energy_matrix[x][0];

	for (int y = 1; y < rows; y++) {
		for (int x = 0; x < columns; x++) {

			// Left side
			if (x == 0) {
				cumulative_matrix[x][y] = energy_matrix[x][y] + std::min(energy_matrix[x][y-1], energy_matrix[x+1][y-1]);
			}

			// Right side
			else if (x == columns - 1) {
				cumulative_matrix[x][y] = energy_matrix[x][y] + std::min(energy_matrix[x][y-1], energy_matrix[x-1][y-1]);
			}

			// Everything else
			else {
				cumulative_matrix[x][y] = energy_matrix[x][y] + std::min(std::min(energy_matrix[x-1][y-1], energy_matrix[x][y-1]), energy_matrix[x+1][y-1]);
			}
		}
	}
}

void ImageProcessor::remove_seams() {
	
	for (int y = 0; y < rows; y++) {
		std::cout << std::endl;
		for (int x = 0; x < columns; x++)
			std::cout << cumulative_matrix[x][y] << " ";
	}
	std::cout << std::endl;

	std::vector<std::vector<int>>::iterator it;
	int* vertical_path = new int[rows];
	int* horizontal_path = new int[columns];
	int min_value = 9999999;
	int x_val;

	for (int x = 0; x < columns; x++) {
		if (cumulative_matrix[x][0] < min_value) {
			min_value = cumulative_matrix[x][0];
			x_val = x;
		}
	}


	for (int y = 0; y < rows; y++) {
		std::cout << std::endl;
		for (int x = 0; x < columns; x++)
			std::cout << cumulative_matrix[x][y] << " ";
	}
	std::cout << std::endl;

}
