cmake_minimum_required(VERSION 3.10)
project(TRACKER)

set(CMAKE_CXX_STANDARD 14)
find_package(OpenCV REQUIRED)

include_directories(include)

add_executable(TRACKER
        include/centroidtracker.h
        src/centroidtracker.cpp
        src/main.cpp)

target_link_libraries(TRACKER PUBLIC ${OpenCV_LIBS})


