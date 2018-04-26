#include <algorithm>
#include <cstdlib>
#include <iostream>
#include "SeamCarving.hpp"

void SeamCarving::print(std::vector<std::vector<int>> matrix) {
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++)
			std::cout << matrix[y][x] << " ";
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

void SeamCarving::get_header(std::ifstream& image) {	
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

	for (auto i = dimensions.size() - x.size()-1; i < dimensions.size(); i++)
		y += *chars++;

	width = stoi(x);
	height = stoi(y);
}

void SeamCarving::populate_pixel_matrix(std::ifstream& image) {

	// Resize vector
	pixel_matrix.resize(height);
	for (int i = 0; i < height; i++)
		pixel_matrix[i].resize(width);

	// Fill vector with pixel values
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++)
			image >> pixel_matrix[y][x];

	print(pixel_matrix);
}

void SeamCarving::populate_energy_matrix() {

	energy_matrix.resize(height);
	for (int i = 0; i < height; i++)
		energy_matrix[i].resize(width);

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			
			// Top
			if (y == 0) {
				// Top left
				if (x == 0 && y == 0)
					energy_matrix[y][x] = abs(pixel_matrix[y][x] - pixel_matrix[y][x + 1]) + abs(pixel_matrix[y][x] - pixel_matrix[y + 1][x]);

				// Top right
				else if (x == width - 1 && y == 0)
					energy_matrix[y][x] = abs(pixel_matrix[y][x] - pixel_matrix[y][x - 1]) + abs(pixel_matrix[y][x] - pixel_matrix[y + 1][x]);

				// Otherwise
				else
					energy_matrix[y][x] = abs(pixel_matrix[y][x] - pixel_matrix[y][x - 1]) + abs(pixel_matrix[y][x] - pixel_matrix[y][x + 1])
						+ abs(pixel_matrix[y][x] - pixel_matrix[y + 1][x]);
			}

			// Bottom
			else if (y == height - 1) {
				// Bottom left
				if (x == 0 && y == height - 1)
					energy_matrix[y][x] = abs(pixel_matrix[y][x] - pixel_matrix[y][x + 1]) + abs(pixel_matrix[y][x] - pixel_matrix[y - 1][x]);

				// Bottom right
				else if (x == width - 1 && y == height - 1)
					energy_matrix[y][x] = abs(pixel_matrix[y][x] - pixel_matrix[y][x - 1]) + abs(pixel_matrix[y][x] - pixel_matrix[y - 1][x]);

				// Otherwise
				else
					energy_matrix[y][x] = abs(pixel_matrix[y][x] - pixel_matrix[y][x - 1]) + abs(pixel_matrix[y][x] - pixel_matrix[y][x + 1])
						+ abs(pixel_matrix[y][x] - pixel_matrix[y - 1][x]);
			}

			// Sides
			else if (x == 0 || x == width - 1) {

				// Left Side
				if (x == 0)
					energy_matrix[y][x] = abs(pixel_matrix[y][x] - pixel_matrix[y][x + 1]) + abs(pixel_matrix[y][x] - pixel_matrix[y - 1][x])
						+ abs(pixel_matrix[y][x] - pixel_matrix[y + 1][x]);

				// Right Side
				else if (x == width - 1)
					energy_matrix[y][x] = abs(pixel_matrix[y][x] - pixel_matrix[y][x - 1]) + abs(pixel_matrix[y][x] - pixel_matrix[y - 1][x])
						+ abs(pixel_matrix[y][x] - pixel_matrix[y + 1][x]);
			}

			// Everything else
			else
				energy_matrix[y][x] = abs(pixel_matrix[y][x] - pixel_matrix[y][x - 1]) + abs(pixel_matrix[y][x] - pixel_matrix[y][x + 1])
					+ abs(pixel_matrix[y][x] - pixel_matrix[y - 1][x]) + abs(pixel_matrix[y][x] - pixel_matrix[y + 1][x]);
		}
	}

	print(energy_matrix);
}

void SeamCarving::populate_cumulative_matrix() {

	cumulative_matrix.resize(height);
	for (int i = 0; i < height; i++)
		cumulative_matrix[i].resize(width);

	// Copy top row of energy matrix to cumulative matrix
	for (int x = 0; x < width; x++)
		cumulative_matrix[0][x] = energy_matrix[0][x];

	for (int y = 1; y < height; y++) {
		for (int x = 0; x < width; x++) {

			// Left side
			if (x == 0) {
				cumulative_matrix[y][x] = energy_matrix[y][x] + std::min(energy_matrix[y-1][x], energy_matrix[y-1][x+1]);
			}

			// Right side
			else if (x == width - 1) {
				cumulative_matrix[y][x] = energy_matrix[y][x] + std::min(energy_matrix[y-1][x], energy_matrix[y-1][x-1]);
			}

			// Everything else
			else {
				cumulative_matrix[y][x] = energy_matrix[y][x] + std::min(std::min(energy_matrix[y-1][x-1], energy_matrix[y-1][x]), energy_matrix[y-1][x+1]);
			}
		}
	}

	print(cumulative_matrix);
}

void SeamCarving::remove_seams() { 
	int h = height - 1;

}

void SeamCarving::resize() {

}

std::vector<std::vector<int>> SeamCarving::get_output() {
	return pixel_matrix;
}