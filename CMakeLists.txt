project(seam-carving CXX)

set(CMAKE_CXX_FLAGS "-std=c++11")

add_executable(seam-carving
  SeamCarving.cpp
  ImageProcessor.cpp)
