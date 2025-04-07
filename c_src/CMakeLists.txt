cmake_minimum_required(VERSION 3.10)
project(NetFlowDensity)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)
include_directories(include)

# Cria o executável "read_csv" com os arquivos fonte necessários
add_executable(read_csv src/read_csv.cpp src/Data.cpp)
