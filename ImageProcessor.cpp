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

	x_dim = stoi(x);
	y_dim = stoi(y);
}

void ImageProcessor::populate_image_matrix(std::ifstream& image) {
	std::vector<int> temp_matrix;
	std::string line;
	std::string temp;

	image_matrix.resize(x_dim);
	for (int x = 0; x < x_dim; x++)
		image_matrix[x].resize(y_dim);

	while (!image.eof()) {

		std::getline(image, line);
		while (line[0] == '#')
			std::getline(image, line);

		const char* chars = line.c_str();
		for (int i = 0; i < line.length(); i++) {

			while (*chars != ' ' && *chars != '\t') {
				temp += *chars++;
				i++;
			}
			temp += *chars++;
			temp_matrix.push_back(stoi(temp));
			temp.clear();
		}
		line.clear();
	}

	int position = 0;
	for (int y = 0; y < y_dim; y++)
		for (int x = 0; x < x_dim; x++)
			image_matrix[x][y] = temp_matrix.at(position++);
}

void ImageProcessor::populate_energy_matrix() {

	energy_matrix.resize(x_dim);
	for (int x = 0; x < x_dim; x++)
		energy_matrix[x].resize(y_dim);

	for (int y = 0; y < y_dim; y++) {
		for (int x = 0; x < x_dim; x++) {
			
			// Top
			if (y == 0) {
				// Top left
				if (x == 0 && y == 0)
					energy_matrix[x][y] = abs(image_matrix[x][y] - image_matrix[x + 1][y]) + abs(image_matrix[x][y] - image_matrix[x][y + 1]);

				// Top right
				else if (x == x_dim - 1 && y == 0)
					energy_matrix[x][y] = abs(image_matrix[x][y] - image_matrix[x - 1][y]) + abs(image_matrix[x][y] - image_matrix[x][y + 1]);

				// Otherwise
				else
					energy_matrix[x][y] = abs(image_matrix[x][y] - image_matrix[x - 1][y]) + abs(image_matrix[x][y] - image_matrix[x + 1][y])
						+ abs(image_matrix[x][y] - image_matrix[x][y + 1]);
			}

			// Bottom
			else if (y == y_dim - 1) {
				// Bottom left
				if (x == 0 && y == y_dim - 1)
					energy_matrix[x][y] = abs(image_matrix[x][y] - image_matrix[x + 1][y]) + abs(image_matrix[x][y] - image_matrix[x][y - 1]);

				// Bottom right
				else if (x == x_dim - 1 && y == y_dim - 1)
					energy_matrix[x][y] = abs(image_matrix[x][y] - image_matrix[x - 1][y]) + abs(image_matrix[x][y] - image_matrix[x][y - 1]);

				// Otherwise
				else
					energy_matrix[x][y] = abs(image_matrix[x][y] - image_matrix[x - 1][y]) + abs(image_matrix[x][y] - image_matrix[x + 1][y])
						+ abs(image_matrix[x][y] - image_matrix[x][y - 1]);
			}

			// Sides
			else if (x == 0 || x == x_dim - 1) {

				// Left Side
				if (x == 0)
					energy_matrix[x][y] = abs(image_matrix[x][y] - image_matrix[x + 1][y]) + abs(image_matrix[x][y] - image_matrix[x][y - 1])
						+ abs(image_matrix[x][y] - image_matrix[x][y + 1]);

				// Right Side
				else if (x == x_dim - 1)
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

	cumulative_matrix.resize(x_dim);
	for (int x = 0; x < x_dim; x++)
		cumulative_matrix[x].resize(y_dim);

	// Copy top row of energy matrix to cumulative matrix
	for (int x = 0; x < x_dim; x++)
		cumulative_matrix[x][0] = energy_matrix[x][0];

	for (int y = 1; y < y_dim; y++) {
		for (int x = 0; x < x_dim; x++) {

			// Left side
			if (x == 0) {
				cumulative_matrix[x][y] = energy_matrix[x][y] + std::min(energy_matrix[x][y-1], energy_matrix[x+1][y-1]);
			}

			// Right side
			else if (x == x_dim - 1) {
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
	
	for (int y = 0; y < y_dim; y++) {
		std::cout << std::endl;
		for (int x = 0; x < x_dim; x++)
			std::cout << cumulative_matrix[x][y] << " ";
	}
	std::cout << std::endl;

	std::vector<std::vector<int>>::iterator it;
	int* vertical_path = new int[y_dim];
	int* horizontal_path = new int[x_dim];
	int min_value = 9999999;
	int x_val;

	for (int x = 0; x < x_dim; x++) {
		if (cumulative_matrix[x][0] < min_value) {
			min_value = cumulative_matrix[x][0];
			x_val = x;
		}
	}


	for (int y = 0; y < y_dim; y++) {
		std::cout << std::endl;
		for (int x = 0; x < x_dim; x++)
			std::cout << cumulative_matrix[x][y] << " ";
	}
	std::cout << std::endl;

}
