cmake_minimum_required(VERSION 3.0)
project(out)

find_package(OpenCL REQUIRED)

set(INCLUDE_DIR ${CMAKE_BINARY_DIR}/include)
make_directory(${INCLUDE_DIR})
configure_file(config.i ${INCLUDE_DIR}/config.h)


file(GLOB KERNELS ${CMAKE_CURRENT_SOURCE_DIR}/kernels/*.cl)

include_directories(${OpenCL_INCLUDE_DIR} ${INCLUDE_DIR})
add_executable(out main.cpp ${KERNELS})

set_source_files_properties(${KERNELS} PROPERTIES HEADER_FILES_ONLY TRUE)

source_group("kernels" FILES ${KERNELS})

target_link_libraries(out ${OpenCL_LIBRARIES})
