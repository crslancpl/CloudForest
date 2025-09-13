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

add_library(PyBackend STATIC
    src/PythonBackend/PythonMain.cpp src/PythonBackend/cfModule.cpp
    )

target_link_libraries(PyBackend Python::Python PkgConfig::GTK4)

add_library(Gui STATIC
    src/gui/guiCore.cpp src/gui/MainWindow.cpp src/gui/HeaderBar.cpp
    src/gui/SettingPanel.cpp src/gui/FilePanel.cpp src/gui/EditArea.cpp
    src/gui/SearchReplaceDialog.cpp src/gui/Style.cpp
    )

target_link_libraries(Gui PkgConfig::GTK4)

add_library(CloudyForest STATIC
    src/cf/CFCore.cpp
    )

target_link_libraries(CloudyForest ${CFLIBPATH} PkgConfig::GTK4)

add_executable(${PROJECT_NAME}
    src/Core.cpp src/Global.cpp src/FileManager.cpp src/ToolFunctions.cpp
    )

target_link_libraries(${PROJECT_NAME} PRIVATE
    PkgConfig::GTK4 ${CFLIBPATH} PyBackend Gui CloudyForest)
