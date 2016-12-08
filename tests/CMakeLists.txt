cmake_minimum_required(VERSION 3.2)
set(CMAKE_CONFIGURATION_TYPES Debug Release CACHE TYPE INTERNAL FORCE)
project(iostreams.tests)

include(GetProjectFiles)
include(SetCompilerOptions)

set(IOSTREAMS_TESTS_FILES)
get_project_files(IOSTREAMS_TESTS_FILES)

add_executable(${PROJECT_NAME} ${IOSTREAMS_TESTS_FILES})
set_compiler_options()

list(APPEND TESTS_INCLUDE_DIRS
  "${CMAKE_SOURCE_DIR}/iostreams/include"
  "${CMAKE_SOURCE_DIR}/3rdparty/babel/babel/include"
  "${CMAKE_SOURCE_DIR}/3rdparty/liberror/include"
  "${CMAKE_SOURCE_DIR}/3rdparty/gtest/googletest/include"
)

if(USE_ZLIB)
  list(APPEND TESTS_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/3rdparty/zlib")
  target_compile_definitions(${PROJECT_NAME} PRIVATE USE_ZLIB)
endif()

set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY INCLUDE_DIRECTORIES ${TESTS_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PRIVATE gtest)
target_link_libraries(${PROJECT_NAME} PRIVATE iostreams)
target_link_libraries(${PROJECT_NAME} PRIVATE babel)

