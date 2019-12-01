cmake_minimum_required(VERSION 3.10)
project(TRACKER)

set(CMAKE_CXX_STANDARD 14)

#ASAN flags
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fsanitize=leak -fno-omit-frame-pointer -g")
#set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=address -fsanitize=leak -fno-omit-frame-pointer -g")
#set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fsanitize=leak -fno-omit-frame-pointer")
#set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fsanitize=leak -fno-omit-frame-pointer")

find_package(OpenCV REQUIRED)

include_directories(include)

add_executable(TRACKER
        include/centroidtracker.h
        src/centroidtracker.cpp
        src/main.cpp)

target_link_libraries(TRACKER PUBLIC ${OpenCV_LIBS})


