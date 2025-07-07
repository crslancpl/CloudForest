cmake_minimum_required(VERSION 3.0)
project(CloudForest LANGUAGES C)

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
    src/Entry.c src/MainWindow.c src/CssLoader.c src/EditArea.c src/ToolFunctions.c src/AssetManager.c
    )

target_link_libraries(${PROJECT_NAME} PRIVATE PkgConfig::GTK4 PkgConfig::WEBKIT)
