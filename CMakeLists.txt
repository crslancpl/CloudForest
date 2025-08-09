cmake_minimum_required(VERSION 3.5)
project(CloudForest)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/Source/cmake")
find_package(PkgConfig REQUIRED)

pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)
#pkg_check_modules(WEBKIT REQUIRED IMPORTED_TARGET webkitgtk-6.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PROJECT_NAME}
    src/FilePanel.cpp
    src/Entry.cpp src/MainWindow.cpp src/CssLoader.cpp
    src/EditArea.cpp src/ToolFunctions.cpp src/AssetManager.cpp
    src/FileManager.cpp src/HeaderBar.cpp src/Classes.cpp
    src/TextTag.cpp src/Core.cpp
    )

target_link_libraries(${PROJECT_NAME} PRIVATE PkgConfig::GTK4)
