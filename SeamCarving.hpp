#pragma once

#include <fstream>
#include <iosfwd>
#include <string>
#include <vector>

class SeamCarving {
public:
	SeamCarving(int vs, int hs)
		: vertical_seams(vs), horizontal_seams(hs)
	{ }

	void print(std::vector<std::vector<int>> matrix);
	void get_header(std::ifstream& image);
	void populate_pixel_matrix(std::ifstream& image);
	void populate_energy_matrix();
	void populate_cumulative_matrix();
	void remove_seams();
	void resize();
	std::vector<std::vector<int>> get_output();

private:
	std::vector<std::vector<int>> pixel_matrix;
	std::vector<std::vector<int>> energy_matrix;
	std::vector<std::vector<int>> cumulative_matrix;
	std::string header, dimensions, max_gray;
	int	vertical_seams, horizontal_seams, width, height;
};
