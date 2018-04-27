#include <algorithm>
#include <cstdlib>
#include <iostream>
#include "SeamCarving.hpp"

// Read in header information
void SeamCarving::get_header(std::ifstream& image) {	
	std::string x, y;
	
	// Read in header
	std::getline(image, header);
	while (header[0] == '#')			// While line is a comment, keep reading
		std::getline(image, header);

	// Read in pixel dimensions
	std::getline(image, dimensions);
	while (dimensions[0] == '#')
		std::getline(image, dimensions);

	// Read in max gray value
	std::getline(image, max_gray);
	while (max_gray[0] == '#')
		std::getline(image, max_gray);

	// char pointer to iterate through dimensions
	const char* chars = dimensions.c_str();
	
	// Get x dimension
	while (*chars != ' ')
		x += *chars++;

	// Get y dimension
	for (int i = dimensions.size() - x.size()-1; i < dimensions.size(); i++)
		y += *chars++;

	// Store
	width = stoi(x);
	height = stoi(y);
}

// Read pixels into matrix (vector of vectors)
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

// Calculate and fill energy matrix
void SeamCarving::populate_energy_matrix() {
	
	// Resize matrix
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

// Calculate shortest paths
void SeamCarving::populate_cumulative_vertical() {
	cumulative_matrix.clear();
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
				cumulative_matrix[y][x] = energy_matrix[y][x] + std::min(cumulative_matrix[y-1][x], cumulative_matrix[y-1][x+1]);
			}

			// Right side
			else if (x == width - 1) {
				cumulative_matrix[y][x] = energy_matrix[y][x] + std::min(cumulative_matrix[y-1][x], cumulative_matrix[y-1][x-1]);
			}

			// Everything else
			else {
				cumulative_matrix[y][x] = energy_matrix[y][x] + std::min(std::min(cumulative_matrix[y-1][x-1], cumulative_matrix[y-1][x]), cumulative_matrix[y-1][x+1]);
			}
		}
	}
}

// Find shortest path thorough cumulative matrix
void SeamCarving::find_vertical_seams() { 
	std::vector<int>::iterator it;
	
	// Clear map used to hold path
	position.clear();

	// Find location of smallest value in last row
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
			it = std::min_element(cumulative_matrix[row - 1].end() - 2, cumulative_matrix[row - 1].end());
			position[row - 1] = std::distance(std::begin(cumulative_matrix[row - 1]), it);
		}

		// Special case
		else if (width <= 3) {
			it = std::min_element(cumulative_matrix[row - 1].begin(), cumulative_matrix[row - 1].end());
			position[row - 1] = std::distance(std::begin(cumulative_matrix[row - 1]), it);
		}

		// Otherwise
		else {
			it = std::min_element(cumulative_matrix[row - 1].begin() + position[row] - 1, cumulative_matrix[row - 1].begin() + position[row] + 2);
			position[row - 1] = std::distance(std::begin(cumulative_matrix[row - 1]), it);
		}
	}
}

// Remove vertical seams
void SeamCarving::remove_vertical_seams(int vertical_seams) {
	
	// Loop for each seam
	while (vertical_seams > 0) {
		populate_energy_matrix();		// Calculate energy matrix
		populate_cumulative_vertical(); // Calculate cumulative matrix
		find_vertical_seams();			// Find shortest path
		
		// Remove path from pixel matrix
		for (auto i : position)
			pixel_matrix[i.first].erase(pixel_matrix[i.first].begin() + i.second);

		width--;
		vertical_seams--;
	}
}

// Populate cumulative matrix for horizontal seams
void SeamCarving::populate_cumulative_horizontal() {
	
	// Reisze matrix
	cumulative_matrix.clear();
	cumulative_matrix.resize(height);
	for (int i = 0; i < height; i++)
		cumulative_matrix[i].resize(width);

	// Copy top row of energy matrix to cumulative matrix
	for (int y = 0; y < height; y++)
		cumulative_matrix[y][0] = energy_matrix[y][0];

	for (int x = 1; x < width; x++) {
		for (int y = 0; y < height; y++) {

			// Top
			if (y == 0) {
				cumulative_matrix[y][x] = energy_matrix[y][x] + std::min(cumulative_matrix[y][x - 1], cumulative_matrix[y + 1][x - 1]);
			}
			
			// Bottom
			else if (y == height - 1) {
				cumulative_matrix[y][x] = energy_matrix[y][x] + std::min(cumulative_matrix[y][x - 1], cumulative_matrix[y - 1][x - 1]);
			}

			// Everything else
			else {
				cumulative_matrix[y][x] = energy_matrix[y][x] + std::min(std::min(cumulative_matrix[y + 1][x - 1], cumulative_matrix[y][x - 1]), cumulative_matrix[y - 1][x - 1]);
			}
		}
	}
}

// Rotate matrix in order to calcualte horizontal shortest paths
void SeamCarving::rotate(int a) {	

	// Rotate 90 degrees clockwise
	if (a == 0) {

		// Resize
		cumulative_rotated.resize(width);
		pixel_rotated.resize(width);
		for (int i = 0; i < width; i++) {
			cumulative_rotated[i].resize(height);
			pixel_rotated[i].resize(height);
		}

		// Rotate
		for (int i = 0; i < width; i++)
			for (int j = 0; j < height; j++) {
				cumulative_rotated[i][j] = cumulative_matrix[height - 1 - j][i];
				pixel_rotated[i][j] = pixel_matrix[height - 1 - j][i];
			}
	}

	// Rotate 90 degress counter-clockwise
	else if (a == 1) {
		
		// Resize
		pixel_matrix.clear();
		pixel_matrix.resize(height);

		for (int i = 0; i < height; i++)
			pixel_matrix[i].resize(width);

		// Rotate
		for (int i = 0; i < height; i++)
			for (int j = 0; j < width; j++)
				pixel_matrix[i][j] = pixel_rotated[j][height - 1 - i];
	}
	// This should never happen
	else std::cout << "This should not happen" << std::endl;
}

// Locate shortest path
void SeamCarving::find_horizontal_seams() {
	std::vector<int>::iterator it;

	// Clear map
	position.clear();

	// Find bottom row min
	it = std::min_element(cumulative_rotated[width - 1].begin(), cumulative_rotated[width - 1].end());
	position[width - 1] = std::distance(std::begin(cumulative_rotated[width - 1]), it);

	for (int row = width - 1; row > 0; row--) {
		// Left side
		if (position[row] == 0) {
			it = std::min_element(cumulative_rotated[row - 1].begin(), cumulative_rotated[row - 1].begin() + 2);
			position[row - 1] = std::distance(std::begin(cumulative_rotated[row - 1]), it);
		}

		// Right side
		else if (position[row] == height - 1) {
			it = std::min_element(cumulative_rotated[row - 1].end() - 2, cumulative_rotated[row - 1].end());
			position[row - 1] = std::distance(std::begin(cumulative_rotated[row - 1]), it);
		}

		// Special case
		else if (height <= 3) {
			it = std::min_element(cumulative_rotated[row - 1].begin(), cumulative_rotated[row - 1].end());
			position[row - 1] = std::distance(std::begin(cumulative_rotated[row - 1]), it);
		}

		// Otherwise
		else {
			it = std::min_element(cumulative_rotated[row - 1].begin() + position[row] - 1, cumulative_rotated[row - 1].begin() + position[row] + 2);
			position[row - 1] = std::distance(std::begin(cumulative_rotated[row - 1]), it);
		}
	}
}

// Remove horzontal seams
void SeamCarving::remove_horizontal_seams(int horizontal_seams) {

	while (horizontal_seams > 0) {

		populate_energy_matrix();
		populate_cumulative_horizontal();
		rotate(0);
		find_horizontal_seams();

		for (auto i : position)
			pixel_rotated[i.first].erase(pixel_rotated[i.first].begin() + i.second);
		
		horizontal_seams--;
		height--;
		rotate(1);
	}
}

// Write back to file
void SeamCarving::write_file(std::ofstream& output) {
	std::vector<std::vector<int>>::iterator row;
	std::vector<int>::iterator column;

	output << "P2\n";
	output << "#\n";
	output << width << " " << height << "\n";
	output << max_gray << "\n";
	
	for (row = pixel_matrix.begin(); row != pixel_matrix.end(); row++) {
		for (column = row->begin(); column != row->end(); column++) {
			output << *column << " ";
		}
		output << "\n";
	}
	output << "\n";
}