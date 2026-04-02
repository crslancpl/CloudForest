cmake_minimum_required(VERSION 3.5)
project(LIBTOOLSET)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/Source/cmake")
find_package(PkgConfig REQUIRED)

pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)

if(UNIX)
    set(CFLIBPATH ${CMAKE_CURRENT_SOURCE_DIR}/libCFEmbed.so)
elseif(WIN32)
    set(CFLIBPATH ${CMAKE_CURRENT_SOURCE_DIR}/libCFEmbed.dll)
endif()

add_library( ${PROJECT_NAME}
    syntaxprovider/syntax_provider.cpp
    syntaxprovider/CFCore.cpp
    tools/text_tool.cpp
)

target_include_directories( ${PROJECT_NAME} PUBLIC ${CLOUDFOREST_ROOT_DIR})

target_link_libraries( ${PROJECT_NAME} ${CFLIBPATH} PkgConfig::GTK4)
