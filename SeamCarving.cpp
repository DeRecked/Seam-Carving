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
}

void SeamCarving::find_vertical_seams() { 
	std::vector<int>::iterator it;
	
	it = std::min_element(cumulative_matrix[height - 1].begin(), cumulative_matrix[height - 1].end());
	position[height-1] = std::distance(std::begin(cumulative_matrix[height - 1]), it);

	for (int row = height - 1; row > 0; row--) {
		// Left side
		if (position[row] == 0) {
			it = std::min_element(cumulative_matrix[row - 1].begin(), cumulative_matrix[row - 1].begin() + 2);
			position[row - 1] = std::distance(std::begin(cumulative_matrix[row - 1]), it);
		}

		// Right side
		else if (position[row] == width - 1) {
			it = std::min_element(cumulative_matrix[row - 1].begin() + position[row] - 1, cumulative_matrix[row - 1].end());
			position[row - 1] = std::distance(std::begin(cumulative_matrix[row - 1]), it);
		}

		// Special case
		else if (width <= 3) {
			it = std::min_element(cumulative_matrix[row - 1].begin(), cumulative_matrix[row - 1].end());
			position[row - 1] = std::distance(std::begin(cumulative_matrix[row - 1]), it);
		}

		// Otherwise
		else {
			it = std::min_element(cumulative_matrix[row - 1].begin() + position[row] - 1, cumulative_matrix[row - 1].begin() + position[row] + 1);
			position[row - 1] = std::distance(std::begin(cumulative_matrix[row - 1]), it);
		}
	}
}

void SeamCarving::remove_vertical_seams() {
	while (vertical_seams > 0) {
		populate_energy_matrix();
		populate_cumulative_matrix();
		find_vertical_seams();
		
		for (auto i : position) {
			pixel_matrix[i.first].erase(pixel_matrix[i.first].begin() + i.second);
			cumulative_matrix[i.first].erase(cumulative_matrix[i.first].begin() + i.second);
		}

		width--;
		vertical_seams--;
	}

	print(pixel_matrix);
}

void SeamCarving::find_horizontal_seams() {
	std::vector<int>::iterator it;

	it = std::min_element(cumulative_matrix[height - 1].begin(), cumulative_matrix[height - 1].end());
	position[height - 1] = std::distance(std::begin(cumulative_matrix[height - 1]), it);

	for (int row = height - 1; row > 0; row--) {
		// Left side
		if (position[row] == 0) {
			it = std::min_element(cumulative_matrix[row - 1].begin(), cumulative_matrix[row - 1].begin() + 2);
			position[row - 1] = std::distance(std::begin(cumulative_matrix[row - 1]), it);
		}

		// Right side
		else if (position[row] == width - 1) {
			it = std::min_element(cumulative_matrix[row - 1].begin() + position[row] - 1, cumulative_matrix[row - 1].end());
			position[row - 1] = std::distance(std::begin(cumulative_matrix[row - 1]), it);
		}

		// Special case
		else if (width <= 3) {
			it = std::min_element(cumulative_matrix[row - 1].begin(), cumulative_matrix[row - 1].end());
			position[row - 1] = std::distance(std::begin(cumulative_matrix[row - 1]), it);
		}

		// Otherwise
		else {
			it = std::min_element(cumulative_matrix[row - 1].begin() + position[row] - 1, cumulative_matrix[row - 1].begin() + position[row] + 1);
			position[row - 1] = std::distance(std::begin(cumulative_matrix[row - 1]), it);
		}
	}
}

void SeamCarving::remove_horizontal_seams() {
	while (horizontal_seams > 0) {
		populate_energy_matrix();
		populate_cumulative_matrix();
		find_horizontal_seams();

		for (auto i : position) {
			pixel_matrix[i.first].erase(pixel_matrix[i.first].begin() + i.second);
			cumulative_matrix[i.first].erase(cumulative_matrix[i.first].begin() + i.second);
		}

		height--;
		horizontal_seams--;
	}

	print(pixel_matrix);
}