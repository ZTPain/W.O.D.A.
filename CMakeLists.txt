cmake_minimum_required(VERSION 3.20) # Kittens will die if you switch to an earlier version of CMake.
project(WODA)

enable_language(CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

set(CMAKE_BUILD_TYPE DEBUG)

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin")

add_compile_options(-MP -O0 -g)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "src/**.cpp")

add_executable(${CMAKE_PROJECT_NAME} 
  ${SOURCES}
)
