cmake_minimum_required(VERSION 3.15)
project(c__)

set(CMAKE_CXX_STANDARD 14)
find_package(OpenCV REQUIRED)

file(GLOB_RECURSE sources ./src/*.cu ./src/*.cpp)
file(GLOB_RECURSE includes ./include/*.h)

link_directories(${OpenCV_LIBRARY_DIRS})


add_executable(c__ src/main.cpp)