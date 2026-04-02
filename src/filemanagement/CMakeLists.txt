project(FILEMANAGLIB)

set(FILEMANAGEMENT
    FileManagement_if.cpp
)

add_library( ${PROJECT_NAME}
    FileManagement_if.cpp
    FileOperation.cpp
    FileCallback.cpp
    FileReader.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CLOUDFOREST_ROOT_DIR})

target_link_libraries(${PROJECT_NAME}
    PkgConfig::GTK4
)
