cmake_minimum_required(VERSION 3.5)
project(CloudForest)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/Source/cmake")
find_package(PkgConfig REQUIRED)

pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)
find_package(Python REQUIRED Development)
#pkg_check_modules(WEBKIT REQUIRED IMPORTED_TARGET webkitgtk-6.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(UNIX)
    set(CFLIBPATH ${CMAKE_CURRENT_SOURCE_DIR}/libCFEmbed.so)
elseif(WIN32)
    set(CFLIBPATH ${CMAKE_CURRENT_SOURCE_DIR}/libCFEmbed.dll)
endif()



add_executable(${PROJECT_NAME}
    src/FilePanel.cpp
    src/Entry.cpp src/MainWindow.cpp src/CssLoader.cpp
    src/EditArea.cpp src/ToolFunctions.cpp
    src/FileManager.cpp src/HeaderBar.cpp
    src/TextTag.cpp src/Core.cpp src/SettingPanel.cpp
    src/PythonBackend/PythonMain.cpp
    )

target_link_libraries(${PROJECT_NAME} PRIVATE PkgConfig::GTK4 ${CFLIBPATH} Python::Python)
