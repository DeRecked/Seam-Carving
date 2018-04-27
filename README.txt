Seam Carving

Description

This program takes a .pgm (image) file as an input and removes the lowest energy pixels in the image. The number of pixels removed is based on user input. Zero can be given for any direction.

To compile:
g++ -std=c++11 -o seam-carving *.cpp

To run:
./seam-carving <path to image> <number of vertical seams> <number of horizontal seams>

example: 
"./seam-carving test.pgm 1 1"