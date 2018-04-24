#include <iostream>
#include "ImageProcessor.hpp"

void ImageProcessor::get_header(std::fstream& image) {	
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

void ImageProcessor::populate_image_matrix(std::fstream& image) {
	int y = 0;
	int x = 0;
	std::vector<int> temp_matrix;

	image_matrix.resize(x_dim);
	for (int i = 0; i < x_dim; i++)
		image_matrix[i].resize(y_dim);

	while (!image.eof()) {
		std::string line;

		std::getline(image, line);
		while (line[0] == '#')
			std::getline(image, line);

		const char* chars = line.c_str();
		for (int i = 0; i < line.length(); i++) {
			std::string temp;

			while (*chars == ' ' || *chars == '\t')
				*chars++;

			while (*chars != ' ' && *chars != '\t') {
				temp += *chars++;
				i++;
			}
			temp += *chars++;

			temp_matrix.push_back(stoi(temp));
		}
	}

	for (int i = 0; i < temp_matrix.size(); i++)
		std::cout << temp_matrix[i] << std::endl;


}

void ImageProcessor::populate_energy_matrix() {

}

void ImageProcessor::remove_seams() {

}