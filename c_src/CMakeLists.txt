cmake_minimum_required(VERSION 3.10)
project(ProcessFiles)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)
include_directories(include)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_executable(process_file src/read_csv.cpp src/Data.cpp)

