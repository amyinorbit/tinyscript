cmake_minimum_required(VERSION 3.2)
project(tinyscript VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include ${GLFW_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fno-exceptions -fno-rtti")
add_subdirectory(lib)
add_subdirectory(bin)
