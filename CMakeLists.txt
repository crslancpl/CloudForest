cmake_minimum_required(VERSION 3.5)
project(CloudForest)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/Source/cmake")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CLOUDFOREST_ROOT_DIR ${CMAKE_SOURCE_DIR})
set(CMAKE_BINARY_DIR ${CLOUDFOREST_ROOT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CLOUDFOREST_ROOT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CLOUDFOREST_ROOT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CLOUDFOREST_ROOT_DIR})


find_package(Python REQUIRED Development)
find_package(PkgConfig REQUIRED)

pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)

add_subdirectory(toolset)
add_subdirectory(pythonbackend)
add_subdirectory(src)
