cmake_minimum_required(VERSION 3.2)
set(CMAKE_CONFIGURATION_TYPES Debug Release CACHE TYPE INTERNAL FORCE)
project(iostreams.project)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/cmake-modules)

option(USE_ZLIB "enable compression streams" OFF)

set(BUILD_GTEST ON)
set(BUILD_GMOCK OFF)

add_subdirectory("${CMAKE_SOURCE_DIR}/iostreams")
add_subdirectory("${CMAKE_SOURCE_DIR}/3rdparty/babel/babel")
add_subdirectory("${CMAKE_SOURCE_DIR}/tests")
add_subdirectory("${CMAKE_SOURCE_DIR}/3rdparty/gtest")