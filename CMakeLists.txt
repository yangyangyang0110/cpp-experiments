cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(experiments VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(logger)

#include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
#conan_basic_setup()

find_package(spdlog REQUIRED)







