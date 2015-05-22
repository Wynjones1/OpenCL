cmake_minimum_required(VERSION 2.8.8)

add_definitions(-D_CRT_SECURE_NO_WARNINGS=1)
include_directories(deps/glm/)
add_subdirectory(src)
add_subdirectory(deps/glm/glm)
