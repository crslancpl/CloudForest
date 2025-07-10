cmake_minimum_required(VERSION 3.0)
project(CloudForest)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/Source/cmake")
find_package(PkgConfig REQUIRED)

find_library(GTK gtk4)
find_library(WEBKIT webkitgtk-6.0)

pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)
pkg_check_modules(WEBKIT REQUIRED IMPORTED_TARGET webkitgtk-6.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${PROJECT_SOURCE_DIR} ${GTK_INCLUDE_DIRS} ${WEBKIT2_INCLUDE_DIRS})
link_directories(${GTK_LIBRARY_DIRS} ${WEBKIT2_LIBRARIES})

add_executable(${PROJECT_NAME}
    src/Entry.cpp src/MainWindow.cpp src/CssLoader.cpp
    src/EditArea.cpp src/ToolFunctions.cpp src/AssetManager.cpp
    src/FileManager.cpp src/HeaderBar.cpp src/SectionData.cpp
    )

target_link_libraries(${PROJECT_NAME} PRIVATE PkgConfig::GTK4 PkgConfig::WEBKIT)
