cmake_minimum_required(VERSION 3.2)
set(CMAKE_CONFIGURATION_TYPES Debug Release CACHE TYPE INTERNAL FORCE)
project(iostreams)

include(GetProjectFiles)
include(SetCompilerOptions)

set(IOSTREAMS_FILES)
get_project_files(IOSTREAMS_FILES)

add_library(${PROJECT_NAME} STATIC ${IOSTREAMS_FILES})
set_compiler_options()

list(APPEND IOSTREAMS_INCLUDE_DIRS
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/src"
  "${CMAKE_SOURCE_DIR}/3rdparty/babel/babel/include"
  "${CMAKE_SOURCE_DIR}/3rdparty/liberror/include"
)

if(USE_ZLIB)
  include(${CMAKE_CURRENT_SOURCE_DIR}/zlib.cmake)
  
  list(APPEND IOSTREAMS_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/3rdparty/zlib")
  target_compile_definitions(${PROJECT_NAME} PRIVATE USE_ZLIB)
  target_link_libraries(${PROJECT_NAME} zlib)
endif()

set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY INCLUDE_DIRECTORIES ${IOSTREAMS_INCLUDE_DIRS})
