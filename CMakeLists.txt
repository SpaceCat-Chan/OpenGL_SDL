cmake_minimum_required(VERSION 3.13)
if(POLICY CMP0076)
	cmake_policy(SET CMP0076 NEW)
endif()

project(OpenGL_SDL VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(OpenGL_SDL)