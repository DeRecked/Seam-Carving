#pragma once

#include <fstream>
#include <iosfwd>
#include <map>
#include <string>
#include <vector>

class SeamCarving {
public:
	// Constructor
	SeamCarving() = default;

	// Public class members
	void print(std::vector<std::vector<int>> matrix);
	void get_header(std::ifstream& image);
	void populate_pixel_matrix(std::ifstream& image);
	void populate_energy_matrix();
	void populate_cumulative_matrix();
	void find_vertical_seams();
	auto find_min_column();
	void find_horizontal_seams();
	void remove_vertical_seams(int vertical_seams);
	void remove_horizontal_seams(int horizontal_seams);
	void write_file(std::ofstream& output);

private:

	// Private class members
	std::vector<std::vector<int>> pixel_matrix;
	std::vector<std::vector<int>> energy_matrix;
	std::vector<std::vector<int>> cumulative_matrix;
	std::map<int, int> position;
	std::string header, dimensions, max_gray;
	int	width, height;
};
