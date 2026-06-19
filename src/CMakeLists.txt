add_subdirectory(filemanagement)
add_subdirectory(languages)
add_subdirectory(gui)
add_subdirectory(session)
add_executable(${CMAKE_PROJECT_NAME} Entry.cpp Setting.cpp)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${CLOUDFOREST_ROOT_DIR})

target_link_libraries(${CMAKE_PROJECT_NAME}
    PYTHONBACKENDLIB
    GUILIB
    FILEMANAGLIB
    LANGMANAGLIB
    LIBTOOLSET
    SESSIONLIB
    PkgConfig::GTK4
    Python::Python
)
