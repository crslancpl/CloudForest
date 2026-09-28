project(FILEMANAGLIB)

add_library( ${PROJECT_NAME}
    FileCallback.cpp
    FileManager.cpp
    FileReader.cpp
    FileTree.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
  ${CLOUDFOREST_ROOT_DIR}
  ${CLOUDFOREST_ROOT_DIR}/src/gui
)

target_link_libraries(${PROJECT_NAME}
    PkgConfig::GTK4 Python::Python LIBTOOLSET
)
